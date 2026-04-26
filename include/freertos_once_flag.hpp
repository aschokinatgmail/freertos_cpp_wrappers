/**********************************************************************************
@file freertos_once_flag.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS call_once / once_flag wrapper
@version 3.1.0
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

namespace freertos {

class once_flag {
public:
    constexpr once_flag() noexcept = default;
    once_flag(const once_flag &) = delete;
    once_flag &operator=(const once_flag &) = delete;
    ~once_flag() noexcept { if (m_semaphore) vSemaphoreDelete(m_semaphore); }

    [[nodiscard]] bool is_initialized() const noexcept {
        return m_state.load(std::memory_order_acquire) == 2;
    }

private:
    mutable SemaphoreHandle_t m_semaphore{nullptr};
    mutable std::atomic<uint8_t> m_state{0};

#if configSUPPORT_STATIC_ALLOCATION
    mutable StaticSemaphore_t m_semaphore_storage{};
    mutable uint8_t m_semaphore_created{0};
#endif

    template <typename Callable, typename... Args>
    friend void call_once(once_flag &, Callable &&, Args &&...);

    void ensure_semaphore() const {
        if (m_semaphore == nullptr) {
            taskENTER_CRITICAL();
            if (m_semaphore == nullptr) {
#if configSUPPORT_STATIC_ALLOCATION
                if (m_semaphore_created == 0) {
                    m_semaphore = xSemaphoreCreateBinaryStatic(&m_semaphore_storage);
                    m_semaphore_created = 1;
                }
#else
                m_semaphore = xSemaphoreCreateBinary();
#endif
            }
            taskEXIT_CRITICAL();
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
            try {
                func(arg);
            } catch (...) {
                m_state.store(0, std::memory_order_release);
                xSemaphoreGive(m_semaphore);
                throw;
            }
            m_state.store(2, std::memory_order_release);
            xSemaphoreGive(m_semaphore);
        } else {
            ensure_semaphore();
            xSemaphoreTake(m_semaphore, portMAX_DELAY);
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
    ~once_flag_static() noexcept = default;

    [[nodiscard]] bool is_initialized() const noexcept {
        return m_state.load(std::memory_order_acquire) == 2;
    }

private:
    mutable SemaphoreHandle_t m_semaphore{nullptr};
    mutable std::atomic<uint8_t> m_state{0};
    StaticSemaphore_t m_semaphore_storage{};
    mutable uint8_t m_semaphore_created{0};

    template <typename Callable, typename... Args>
    friend void call_once(once_flag_static &, Callable &&, Args &&...);

    void do_call(void (*func)(void *), void *arg) {
        uint8_t expected = 0;
        if (m_state.load(std::memory_order_acquire) == 2) {
            return;
        }
        if (m_state.compare_exchange_strong(expected, 1,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire)) {
            taskENTER_CRITICAL();
            if (m_semaphore_created == 0) {
                m_semaphore =
                    xSemaphoreCreateBinaryStatic(&m_semaphore_storage);
                m_semaphore_created = 1;
            }
            taskEXIT_CRITICAL();
            try {
                func(arg);
            } catch (...) {
                m_state.store(0, std::memory_order_release);
                xSemaphoreGive(m_semaphore);
                throw;
            }
            m_state.store(2, std::memory_order_release);
            xSemaphoreGive(m_semaphore);
        } else {
            taskENTER_CRITICAL();
            if (m_semaphore_created == 0) {
                m_semaphore =
                    xSemaphoreCreateBinaryStatic(&m_semaphore_storage);
                m_semaphore_created = 1;
            }
            taskEXIT_CRITICAL();
            xSemaphoreTake(m_semaphore, portMAX_DELAY);
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