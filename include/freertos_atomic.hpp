/**********************************************************************************
@file freertos_atomic.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS atomic_flag and atomic<T> polyfill with wait/notify support
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
#include "freertos_expected.hpp"
#include "freertos_isr_result.hpp"
#include <FreeRTOS.h>
#include <atomic>
#include <cstddef>
#include <semphr.h>
#include <task.h>
#include <type_traits>

#if defined(CONFIG_FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY)
#include "freertos_atomic_wait.hpp"
#endif

namespace freertos {

class atomic_flag {
public:
    atomic_flag() noexcept = default;
    atomic_flag(const atomic_flag &) = delete;
    atomic_flag &operator=(const atomic_flag &) = delete;

    bool test_and_set(std::memory_order order = std::memory_order_seq_cst) noexcept {
        return m_flag.exchange(true, order);
    }

    void clear(std::memory_order order = std::memory_order_seq_cst) noexcept {
        m_flag.store(false, order);
    }

    bool test(std::memory_order order = std::memory_order_seq_cst) const noexcept {
        return m_flag.load(order);
    }

    void wait(bool old, std::memory_order order = std::memory_order_seq_cst) const {
        if (m_flag.load(order) != old) {
            return;
        }
        ensure_semaphore();
        while (m_flag.load(order) == old) {
            xSemaphoreTake(m_semaphore, portMAX_DELAY);
        }
    }

    void notify_one() noexcept {
        ensure_semaphore();
        xSemaphoreGive(m_semaphore);
    }

    void notify_all() noexcept {
        ensure_semaphore();
        xSemaphoreGive(m_semaphore);
        xSemaphoreGive(m_semaphore);
    }

    isr_result<void> notify_one_isr() noexcept {
        ensure_semaphore();
        isr_result<void> result{pdFALSE};
        xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
        return result;
    }

    isr_result<void> notify_all_isr() noexcept {
        ensure_semaphore();
        isr_result<void> result{pdFALSE};
        xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
        xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
        return result;
    }

    [[nodiscard]] expected<void, error> notify_one_ex() noexcept {
        ensure_semaphore();
        auto rc = xSemaphoreGive(m_semaphore);
        if (rc == pdTRUE) {
            return {};
        }
        return unexpected<error>(error::semaphore_not_owned);
    }

    [[nodiscard]] expected<void, error> notify_all_ex() noexcept {
        ensure_semaphore();
        auto rc = xSemaphoreGive(m_semaphore);
        if (rc != pdTRUE) {
            return unexpected<error>(error::semaphore_not_owned);
        }
        rc = xSemaphoreGive(m_semaphore);
        if (rc != pdTRUE) {
            return unexpected<error>(error::semaphore_not_owned);
        }
        return {};
    }

private:
    mutable std::atomic<bool> m_flag{false};
    mutable SemaphoreHandle_t m_semaphore{nullptr};
    mutable uint8_t m_semaphore_created{0};

#if configSUPPORT_STATIC_ALLOCATION
    mutable StaticSemaphore_t m_semaphore_storage{};
#endif

    void ensure_semaphore() const {
        if (m_semaphore_created == 0) {
#if configSUPPORT_STATIC_ALLOCATION
            m_semaphore = xSemaphoreCreateBinaryStatic(&m_semaphore_storage);
#else
            m_semaphore = xSemaphoreCreateBinary();
#endif
            m_semaphore_created = 1;
        }
    }
};

#if configSUPPORT_STATIC_ALLOCATION
namespace sa {

class atomic_flag_static {
public:
    atomic_flag_static() noexcept = default;
    atomic_flag_static(const atomic_flag_static &) = delete;
    atomic_flag_static &operator=(const atomic_flag_static &) = delete;

    bool test_and_set(std::memory_order order = std::memory_order_seq_cst) noexcept {
        return m_flag.exchange(true, order);
    }

    void clear(std::memory_order order = std::memory_order_seq_cst) noexcept {
        m_flag.store(false, order);
    }

    bool test(std::memory_order order = std::memory_order_seq_cst) const noexcept {
        return m_flag.load(order);
    }

    void wait(bool old, std::memory_order order = std::memory_order_seq_cst) const {
        if (m_flag.load(order) != old) {
            return;
        }
        if (m_semaphore_created == 0) {
            m_semaphore = xSemaphoreCreateBinaryStatic(&m_semaphore_storage);
            m_semaphore_created = 1;
        }
        while (m_flag.load(order) == old) {
            xSemaphoreTake(m_semaphore, portMAX_DELAY);
        }
    }

    void notify_one() noexcept {
        if (m_semaphore_created == 0) {
            m_semaphore = xSemaphoreCreateBinaryStatic(&m_semaphore_storage);
            m_semaphore_created = 1;
        }
        xSemaphoreGive(m_semaphore);
    }

    void notify_all() noexcept {
        if (m_semaphore_created == 0) {
            m_semaphore = xSemaphoreCreateBinaryStatic(&m_semaphore_storage);
            m_semaphore_created = 1;
        }
        xSemaphoreGive(m_semaphore);
        xSemaphoreGive(m_semaphore);
    }

    isr_result<void> notify_one_isr() noexcept {
        if (m_semaphore_created == 0) {
            m_semaphore = xSemaphoreCreateBinaryStatic(&m_semaphore_storage);
            m_semaphore_created = 1;
        }
        isr_result<void> result{pdFALSE};
        xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
        return result;
    }

    isr_result<void> notify_all_isr() noexcept {
        if (m_semaphore_created == 0) {
            m_semaphore = xSemaphoreCreateBinaryStatic(&m_semaphore_storage);
            m_semaphore_created = 1;
        }
        isr_result<void> result{pdFALSE};
        xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
        xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
        return result;
    }

private:
    mutable std::atomic<bool> m_flag{false};
    mutable SemaphoreHandle_t m_semaphore{nullptr};
    mutable uint8_t m_semaphore_created{0};
    mutable StaticSemaphore_t m_semaphore_storage{};
};

using atomic_flag = atomic_flag_static;

} // namespace sa
#endif

#if configSUPPORT_DYNAMIC_ALLOCATION
namespace da {

using atomic_flag = freertos::atomic_flag;

} // namespace da
#endif

template <typename T> class atomic : public std::atomic<T> {
    static_assert(std::is_trivially_copyable<T>::value,
                  "atomic<T> requires T to be trivially copyable");

public:
    using std::atomic<T>::atomic;
    using std::atomic<T>::operator=;

    atomic() noexcept : std::atomic<T>{} {}
    constexpr atomic(T desired) : std::atomic<T>(desired) {}

    atomic(const atomic &) = delete;
    atomic &operator=(const atomic &) = delete;

    void wait(T old, std::memory_order order = std::memory_order_seq_cst) const
#if defined(CONFIG_FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY)
        noexcept
#endif
    {
        if (this->load(order) != old) {
            return;
        }
#if defined(CONFIG_FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY)
        __cxx_atomic_contention_t expected = 0;
        while (this->load(order) == old) {
            __platform_wait_on_address(static_cast<void const *>(this),
                                       &expected, sizeof(T));
        }
#else
        while (this->load(order) == old) {
            taskYIELD();
        }
#endif
    }

    void notify_one() noexcept {
#if defined(CONFIG_FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY)
        __platform_wake_by_address(static_cast<void const *>(this), 1);
#endif
    }

    void notify_all() noexcept {
#if defined(CONFIG_FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY)
        __platform_wake_by_address(static_cast<void const *>(this),
                                   INT_MAX);
#endif
    }

    isr_result<void> notify_one_isr() noexcept {
#if defined(CONFIG_FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY) &&           \
    (CONFIG_FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 1 ||                        \
     CONFIG_FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 2)
        freertos::atomic_notify_one_isr(static_cast<void const *>(this));
#endif
        return {pdFALSE};
    }

    isr_result<void> notify_all_isr() noexcept {
#if defined(CONFIG_FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY) &&           \
    (CONFIG_FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 1 ||                        \
     CONFIG_FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 2)
        freertos::atomic_notify_all_isr(static_cast<void const *>(this));
#endif
        return {pdFALSE};
    }

    [[nodiscard]] expected<void, error> notify_one_ex() noexcept {
#if defined(CONFIG_FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY)
        __platform_wake_by_address(static_cast<void const *>(this), 1);
#endif
        return {};
    }

    [[nodiscard]] expected<void, error> notify_all_ex() noexcept {
#if defined(CONFIG_FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY)
        __platform_wake_by_address(static_cast<void const *>(this),
                                   INT_MAX);
#endif
        return {};
    }
};

using atomic_bool = atomic<bool>;
using atomic_char = atomic<char>;
using atomic_schar = atomic<signed char>;
using atomic_uchar = atomic<unsigned char>;
using atomic_short = atomic<short>;
using atomic_ushort = atomic<unsigned short>;
using atomic_int = atomic<int>;
using atomic_uint = atomic<unsigned int>;
using atomic_long = atomic<long>;
using atomic_ulong = atomic<unsigned long>;
using atomic_llong = atomic<long long>;
using atomic_ullong = atomic<unsigned long long>;
using atomic_ptrdiff_t = atomic<std::ptrdiff_t>;
using atomic_size_t = atomic<std::size_t>;

} // namespace freertos