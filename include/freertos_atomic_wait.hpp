/**********************************************************************************
@file freertos_atomic_wait.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS platform hooks for C++20 std::atomic::wait/notify_one/notify_all
@version 3.2.0
@date 2026-04-21

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

#include <FreeRTOS.h>
#include <atomic>
#include <cstddef>
#include <semphr.h>
#include <task.h>

#if defined(FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY)

#ifndef FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL
#define FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL 1
#endif

#ifndef FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_TABLE_SIZE
#define FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_TABLE_SIZE 16
#endif

using __cxx_atomic_contention_t = unsigned int;

inline constexpr size_t atomic_wait_table_size =
    FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_TABLE_SIZE;

[[nodiscard]] inline size_t atomic_wait_hash(void const *addr) {
    auto const key = reinterpret_cast<uintptr_t>(addr);
    auto const hash = key ^ (key >> 16);
    return hash & (atomic_wait_table_size - 1);
}

struct freertos_wait_entry {
    void const *address;
    SemaphoreHandle_t semaphore;
    std::atomic<__cxx_atomic_contention_t> waiter_count;
};

struct freertos_waiter_node {
    TaskHandle_t task;
    void const *address;
    freertos_waiter_node *next;
};

struct freertos_wait_bucket {
    std::atomic<__cxx_atomic_contention_t> waiter_count;
    freertos_waiter_node *waiters;
    SemaphoreHandle_t mutex;
};

#if FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 1

extern freertos_wait_entry freertos_wait_table[atomic_wait_table_size];

#elif FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 2

extern freertos_wait_bucket freertos_wait_buckets[atomic_wait_table_size];

#endif

extern "C" bool __platform_wait_on_address(void const *addr,
                                           __cxx_atomic_contention_t const *expected,
                                           int size);

extern "C" void __platform_wake_by_address(void const *addr, int count);

namespace freertos {

#if FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 1

inline void atomic_notify_one_isr(void const *addr) {
    auto idx = atomic_wait_hash(addr);
    auto &entry = freertos_wait_table[idx];
    BaseType_t higher_priority_task_woken = pdFALSE;
    xSemaphoreGiveFromISR(entry.semaphore, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

inline void atomic_notify_all_isr(void const *addr) {
    auto idx = atomic_wait_hash(addr);
    auto &entry = freertos_wait_table[idx];
    auto count = entry.waiter_count.load(std::memory_order_seq_cst);
    BaseType_t higher_priority_task_woken = pdFALSE;
    for (__cxx_atomic_contention_t i = 0; i < count; ++i) {
        xSemaphoreGiveFromISR(entry.semaphore, &higher_priority_task_woken);
    }
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

#elif FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL == 2

inline void atomic_notify_one_isr(void const *addr) {
    auto idx = atomic_wait_hash(addr);
    auto &bucket = freertos_wait_buckets[idx];
    BaseType_t higher_priority_task_woken = pdFALSE;
    auto *node = bucket.waiters;
    while (node != nullptr) {
        if (node->address == addr) {
            vTaskNotifyGiveFromISR(node->task, &higher_priority_task_woken);
            break;
        }
        node = node->next;
    }
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

inline void atomic_notify_all_isr(void const *addr) {
    auto idx = atomic_wait_hash(addr);
    auto &bucket = freertos_wait_buckets[idx];
    BaseType_t higher_priority_task_woken = pdFALSE;
    auto *node = bucket.waiters;
    while (node != nullptr) {
        if (node->address == addr) {
            vTaskNotifyGiveFromISR(node->task, &higher_priority_task_woken);
        }
        node = node->next;
    }
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

#endif

} // namespace freertos

#endif