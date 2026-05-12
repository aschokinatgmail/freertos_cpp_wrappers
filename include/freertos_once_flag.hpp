/**********************************************************************************
@file freertos_once_flag.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS call_once / once_flag wrapper
@version 3.2.1
@date 2026-04-22

The MIT License (MIT)

FreeRTOS C++ Wrappers Library
https://github.com/aschokinatgmail/freertos_cpp_wrappers/

Copyright(c) 2024 Andrey V. Shchekin <aschokin@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***********************************************************************************/

#pragma once

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#include "freertos_config.hpp"
#include "freertos_thread_safety.hpp"
#include <FreeRTOS.h>
#include <atomic>
#include <semphr.h>
#include <utility>

class OnceFlagTest;

namespace freertos {

#ifndef FREERTOS_CPP_WRAPPERS_ONCE_FLAG_MAX_WAITERS
#define FREERTOS_CPP_WRAPPERS_ONCE_FLAG_MAX_WAITERS 8
#endif

class once_flag {
public:
    constexpr once_flag() noexcept = default;
    once_flag(const once_flag &) = delete;
    once_flag &operator=(const once_flag &) = delete;
    ~once_flag() noexcept { if (m_semaphore != nullptr) vSemaphoreDelete(m_semaphore); }

    [[nodiscard]] bool is_initialized() const noexcept {
        return m_state.load(std::memory_order_acquire) == 2;
    }

private:
    mutable SemaphoreHandle_t m_semaphore{nullptr};
    mutable std::atomic<uint8_t> m_state{0};
    // Lazy-init state for m_semaphore: 0 = not started, 1 = creation in
    // progress (some thread is constructing the semaphore), 2 = created and
    // m_semaphore is published. Always present so ensure_semaphore() never
    // needs a critical section regardless of static/dynamic allocation.
    mutable std::atomic<uint8_t> m_sem_init{0};

#if configSUPPORT_STATIC_ALLOCATION
    mutable StaticSemaphore_t m_semaphore_storage{};
#endif

    template <typename Callable, typename... Args>
    friend void call_once(once_flag &, Callable &&, Args &&...);

    friend class ::OnceFlagTest;

    // Lazy, race-safe semaphore creation that does NOT call any FreeRTOS API
    // from inside a critical section (Bug #262). FreeRTOS forbids invoking
    // potentially-blocking APIs (xSemaphoreCreate*) while the scheduler is
    // suspended via taskENTER_CRITICAL. Instead we elect a single creator via
    // an atomic CAS on m_sem_init and have the losers spin (taskYIELD) until
    // creation is published.
    void ensure_semaphore() const {
        if (m_sem_init.load(std::memory_order_acquire) == 2) {
            return;
        }
        uint8_t expected = 0;
        if (m_sem_init.compare_exchange_strong(
                expected, 1, std::memory_order_acq_rel,
                std::memory_order_acquire)) {
            // Winner: create the semaphore OUTSIDE any critical section.
#if configSUPPORT_STATIC_ALLOCATION
            m_semaphore = xSemaphoreCreateCountingStatic(
                FREERTOS_CPP_WRAPPERS_ONCE_FLAG_MAX_WAITERS, 0,
                &m_semaphore_storage);
#else
            m_semaphore = xSemaphoreCreateCounting(
                FREERTOS_CPP_WRAPPERS_ONCE_FLAG_MAX_WAITERS, 0);
#endif
            m_sem_init.store(2, std::memory_order_release);
        } else {
            // Loser: wait for the winner to publish the semaphore.
            while (m_sem_init.load(std::memory_order_acquire) != 2) {
                taskYIELD();
            }
        }
    }

    void do_call(void (*func)(void *), void *arg) {
        uint8_t expected = 0;
        if (m_state.load(std::memory_order_acquire) == 2) {
            return;
        }
        if (m_state.compare_exchange_strong(expected, 1,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire)) {
            ensure_semaphore();
#if __cpp_exceptions
            // Bug #263: ensure waiters are always released and the state is
            // reset to 0 if func() throws, so subsequent call_once attempts
            // can retry instead of dead-locking on a never-given semaphore.
            // Order matters: reset m_state to 0 BEFORE giving so that woken
            // waiters observe state != 1 and exit their wait loop.
            bool succeeded = false;
            try {
                func(arg);
                succeeded = true;
            } catch (...) {
                m_state.store(0, std::memory_order_release);
                for (uint8_t i = 0;
                     i < FREERTOS_CPP_WRAPPERS_ONCE_FLAG_MAX_WAITERS; ++i)
                    xSemaphoreGive(m_semaphore);
                throw;
            }
            (void)succeeded;
#else
            // Exceptions disabled (e.g. -fno-exceptions or simulation build):
            // a throwing func() would terminate the program anyway, so we
            // simply call it without a try/catch wrapper. State recovery on
            // exception is not possible without exceptions.
            func(arg);
#endif
            m_state.store(2, std::memory_order_release);
            for (uint8_t i = 0; i < FREERTOS_CPP_WRAPPERS_ONCE_FLAG_MAX_WAITERS; ++i)
                xSemaphoreGive(m_semaphore);
        } else {
            ensure_semaphore();
            for (;;) {
                xSemaphoreTake(m_semaphore, portMAX_DELAY);
                if (m_state.load(std::memory_order_acquire) != 1)
                    return;
            }
        }
    }
};

namespace detail {

template <typename Callable> void call_once_wrapper(void *arg) {
    auto *f = static_cast<Callable *>(arg);
    (*f)();
}

} // namespace detail

template <typename Callable, typename... Args>
void call_once(once_flag &flag, Callable &&func, Args &&...args) {
    if (flag.m_state.load(std::memory_order_acquire) == 2) {
        return;
    }
    auto wrapper = [&]() mutable { std::forward<Callable>(func)(std::forward<Args>(args)...); };
    flag.do_call(detail::call_once_wrapper<decltype(wrapper)>,
                 static_cast<void *>(&wrapper));
}

#if configSUPPORT_STATIC_ALLOCATION
namespace sa {

class once_flag_static {
public:
    constexpr once_flag_static() noexcept = default;
    once_flag_static(const once_flag_static &) = delete;
    once_flag_static &operator=(const once_flag_static &) = delete;
    ~once_flag_static() noexcept { if (m_semaphore != nullptr) vSemaphoreDelete(m_semaphore); }

    [[nodiscard]] bool is_initialized() const noexcept {
        return m_state.load(std::memory_order_acquire) == 2;
    }

private:
    mutable SemaphoreHandle_t m_semaphore{nullptr};
    mutable std::atomic<uint8_t> m_state{0};
    mutable StaticSemaphore_t m_semaphore_storage{};
    // See once_flag::m_sem_init for state semantics (0/1/2).
    mutable std::atomic<uint8_t> m_sem_init{0};

    template <typename Callable, typename... Args>
    friend void call_once(once_flag_static &, Callable &&, Args &&...);

    friend class ::OnceFlagTest;

    // Bug #262: must not create the semaphore inside a critical section.
    // Use CAS-based election + spin-wait instead.
    void ensure_semaphore() const {
        if (m_sem_init.load(std::memory_order_acquire) == 2) {
            return;
        }
        uint8_t expected = 0;
        if (m_sem_init.compare_exchange_strong(
                expected, 1, std::memory_order_acq_rel,
                std::memory_order_acquire)) {
            m_semaphore = xSemaphoreCreateCountingStatic(
                FREERTOS_CPP_WRAPPERS_ONCE_FLAG_MAX_WAITERS, 0,
                &m_semaphore_storage);
            m_sem_init.store(2, std::memory_order_release);
        } else {
            while (m_sem_init.load(std::memory_order_acquire) != 2) {
                taskYIELD();
            }
        }
    }

    void do_call(void (*func)(void *), void *arg) {
        uint8_t expected = 0;
        if (m_state.load(std::memory_order_acquire) == 2) {
            return;
        }
        if (m_state.compare_exchange_strong(expected, 1,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire)) {
            ensure_semaphore();
#if __cpp_exceptions
            // Bug #263: on exception, reset state to 0 and release waiters so
            // they exit their wait loop and a subsequent call_once can retry.
            try {
                func(arg);
            } catch (...) {
                m_state.store(0, std::memory_order_release);
                for (uint8_t i = 0;
                     i < FREERTOS_CPP_WRAPPERS_ONCE_FLAG_MAX_WAITERS; ++i)
                    xSemaphoreGive(m_semaphore);
                throw;
            }
#else
            // Exceptions disabled: state recovery on exception is not possible.
            func(arg);
#endif
            m_state.store(2, std::memory_order_release);
            for (uint8_t i = 0; i < FREERTOS_CPP_WRAPPERS_ONCE_FLAG_MAX_WAITERS; ++i)
                xSemaphoreGive(m_semaphore);
        } else {
            ensure_semaphore();
            for (;;) {
                xSemaphoreTake(m_semaphore, portMAX_DELAY);
                if (m_state.load(std::memory_order_acquire) != 1)
                    return;
            }
        }
    }
};

template <typename Callable, typename... Args>
void call_once(once_flag_static &flag, Callable &&func, Args &&...args) {
    if (flag.m_state.load(std::memory_order_acquire) == 2) {
        return;
    }
    auto wrapper = [&]() mutable { std::forward<Callable>(func)(std::forward<Args>(args)...); };
    flag.do_call(freertos::detail::call_once_wrapper<decltype(wrapper)>,
                 static_cast<void *>(&wrapper));
}

using once_flag = once_flag_static;

} // namespace sa
#endif

#if configSUPPORT_DYNAMIC_ALLOCATION
namespace da {

using once_flag = freertos::once_flag;

using freertos::call_once;

} // namespace da
#endif

} // namespace freertos