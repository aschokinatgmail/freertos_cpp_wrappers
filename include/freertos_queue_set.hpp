/**********************************************************************************
@file freertos_queue_set.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS queue set wrapper
@version 3.2.0
@date 2026-04-17

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

#include "freertos_expected.hpp"
#include "freertos_isr_result.hpp"
#include <FreeRTOS.h>
#include <chrono>
#include <queue.h>
#include <type_traits>
#include <utility>

namespace freertos {

#if configUSE_QUEUE_SETS

/** @brief Dynamic allocation policy for queue_set.
 *
 * Uses xQueueCreateSet() to allocate the queue set from the FreeRTOS heap.
 * Available only when configSUPPORT_DYNAMIC_ALLOCATION is enabled.
 */
#if configSUPPORT_DYNAMIC_ALLOCATION
class dynamic_queue_set_allocator {
public:
  QueueSetHandle_t create(UBaseType_t uxEventQueueLength) {
    return xQueueCreateSet(uxEventQueueLength);
  }
};
#endif

/** @brief RAII wrapper for FreeRTOS queue sets (configUSE_QUEUE_SETS).
 *
 * A queue set allows monitoring multiple queues and semaphores simultaneously.
 * Use add() to register members, select() to block until a member is ready,
 * or select_isr() for ISR context. The _ex variants return expected<T,error>.
 * Available only when configUSE_QUEUE_SETS is enabled.
 *
 * @tparam QueueSetAllocator Allocator policy (dynamic_queue_set_allocator or custom)
 */
template <typename QueueSetAllocator> class queue_set {
  QueueSetAllocator m_allocator{};
  QueueSetHandle_t m_queue_set{nullptr};

public:
  explicit queue_set(UBaseType_t event_queue_length)
      : m_queue_set{m_allocator.create(event_queue_length)} {
    configASSERT(m_queue_set);
  }

  template <typename... AllocatorArgs,
            typename std::enable_if_t<(sizeof...(AllocatorArgs) > 0), int> = 0>
  explicit queue_set(UBaseType_t event_queue_length, AllocatorArgs &&...args)
      : m_allocator{std::forward<AllocatorArgs>(args)...},
        m_queue_set{m_allocator.create(event_queue_length)} {
    configASSERT(m_queue_set);
  }

  queue_set(const queue_set &) = delete;
  queue_set(queue_set &&src) noexcept : m_queue_set(src.m_queue_set) {
    src.m_queue_set = nullptr;
  }

  ~queue_set(void) {
    if (m_queue_set) {
      vQueueDelete(m_queue_set);
    }
  }

  queue_set &operator=(const queue_set &) = delete;
  queue_set &operator=(queue_set &&src) noexcept {
    if (this != &src) {
      swap(src);
    }
    return *this;
  }

  void swap(queue_set &other) noexcept {
    using std::swap;
    swap(m_queue_set, other.m_queue_set);
  }

  friend void swap(queue_set &a, queue_set &b) noexcept { a.swap(b); }

  BaseType_t add(QueueSetMemberHandle_t xQueueOrSemaphore) {
    return xQueueAddToSet(xQueueOrSemaphore, m_queue_set);
  }

  BaseType_t remove(QueueSetMemberHandle_t xQueueOrSemaphore) {
    return xQueueRemoveFromSet(xQueueOrSemaphore, m_queue_set);
  }

  QueueSetMemberHandle_t select(TickType_t xTicksToWait) {
    return xQueueSelectFromSet(m_queue_set, xTicksToWait);
  }

  template <typename Rep, typename Period>
  QueueSetMemberHandle_t
  select(const std::chrono::duration<Rep, Period> &timeout) {
    return select(pdMS_TO_TICKS(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
            .count()));
  }

  isr_result<QueueSetMemberHandle_t> select_isr(void) {
    isr_result<QueueSetMemberHandle_t> result{nullptr, pdFALSE};
    result.result = xQueueSelectFromSetFromISR(
        m_queue_set, &result.higher_priority_task_woken);
    return result;
  }

  [[nodiscard]] expected<void, error>
  add_ex(QueueSetMemberHandle_t xQueueOrSemaphore) {
    auto rc = add(xQueueOrSemaphore);
    if (rc == pdPASS) {
      return {};
    }
    return unexpected<error>(error::invalid_handle);
  }

  [[nodiscard]] expected<void, error>
  remove_ex(QueueSetMemberHandle_t xQueueOrSemaphore) {
    auto rc = remove(xQueueOrSemaphore);
    if (rc == pdPASS) {
      return {};
    }
    return unexpected<error>(error::invalid_handle);
  }

  [[nodiscard]] expected<QueueSetMemberHandle_t, error>
  select_ex(TickType_t xTicksToWait) {
    auto rc = select(xTicksToWait);
    if (rc != nullptr) {
      return rc;
    }
    return unexpected<error>(xTicksToWait == 0 ? error::would_block
                                               : error::timeout);
  }

  template <typename Rep, typename Period>
  [[nodiscard]] expected<QueueSetMemberHandle_t, error>
  select_ex(const std::chrono::duration<Rep, Period> &timeout) {
    return select_ex(pdMS_TO_TICKS(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
            .count()));
  }

  [[nodiscard]] isr_result<expected<QueueSetMemberHandle_t, error>>
  select_ex_isr(void) {
    auto result = select_isr();
    isr_result<expected<QueueSetMemberHandle_t, error>> ret{
        unexpected<error>(error::would_block),
        result.higher_priority_task_woken};
    if (result.result != nullptr) {
      ret.result = result.result;
    }
    return ret;
  }
};

#if configSUPPORT_DYNAMIC_ALLOCATION
namespace da {
using queue_set = freertos::queue_set<freertos::dynamic_queue_set_allocator>;
} // namespace da
#endif

#endif // configUSE_QUEUE_SETS

} // namespace freertos