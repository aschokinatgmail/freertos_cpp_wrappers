/**********************************************************************************
@file freertos_shared_mutex.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS shared_mutex wrapper providing C++20 std::shared_mutex semantics
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

#if FREERTOS_CPP_WRAPPERS_HAS_MUTEX && FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE

#include "freertos_expected.hpp"
#include "freertos_thread_safety.hpp"
#include <FreeRTOS.h>
#include <chrono>
#include <ctime>
#include <semphr.h>
#include <task.h>
#include <type_traits>
#include <utility>

#ifndef FREERTOS_CPP_WRAPPERS_SHARED_MUTEX_MAX_READERS
#define FREERTOS_CPP_WRAPPERS_SHARED_MUTEX_MAX_READERS 8
#endif

namespace freertos {

#if configSUPPORT_STATIC_ALLOCATION
class static_shared_mutex_allocator {
  StaticSemaphore_t m_mutex_placeholder{};
  StaticSemaphore_t m_reader_slots_placeholder{};

public:
  static constexpr bool is_static = true;

  static_shared_mutex_allocator() = default;
  ~static_shared_mutex_allocator() = default;
  static_shared_mutex_allocator(const static_shared_mutex_allocator &) = delete;
  static_shared_mutex_allocator(static_shared_mutex_allocator &&) = default;

  static_shared_mutex_allocator &
  operator=(const static_shared_mutex_allocator &) = delete;
  static_shared_mutex_allocator &
  operator=(static_shared_mutex_allocator &&) = delete;

  void swap(static_shared_mutex_allocator &other) noexcept {
    using std::swap;
    swap(m_mutex_placeholder, other.m_mutex_placeholder);
    swap(m_reader_slots_placeholder, other.m_reader_slots_placeholder);
  }

  SemaphoreHandle_t create_mutex() {
    return xSemaphoreCreateMutexStatic(&m_mutex_placeholder);
  }
  SemaphoreHandle_t create_counting(UBaseType_t max_count) {
    return xSemaphoreCreateCountingStatic(max_count, max_count,
                                          &m_reader_slots_placeholder);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
class dynamic_shared_mutex_allocator {
public:
  static constexpr bool is_static = false;

  void swap(dynamic_shared_mutex_allocator &other) noexcept { (void)other; }

  SemaphoreHandle_t create_mutex() { return xSemaphoreCreateMutex(); }
  SemaphoreHandle_t create_counting(UBaseType_t max_count) {
    return xSemaphoreCreateCounting(max_count, max_count);
  }
};
#endif

template <typename SharedMutexAllocator>
class FREERTOS_CAPABILITY("shared_mutex") shared_mutex {
  SharedMutexAllocator m_allocator{};
  SemaphoreHandle_t m_mutex_handle{nullptr};
  SemaphoreHandle_t m_reader_slots_handle{nullptr};
  UBaseType_t m_max_readers{FREERTOS_CPP_WRAPPERS_SHARED_MUTEX_MAX_READERS};

public:
  shared_mutex()
      : m_mutex_handle{m_allocator.create_mutex()},
        m_reader_slots_handle{m_allocator.create_counting(
            FREERTOS_CPP_WRAPPERS_SHARED_MUTEX_MAX_READERS)},
        m_max_readers{FREERTOS_CPP_WRAPPERS_SHARED_MUTEX_MAX_READERS} {
    configASSERT(m_mutex_handle);
    configASSERT(m_reader_slots_handle);
  }

  template <typename... AllocatorArgs,
            typename std::enable_if_t<(sizeof...(AllocatorArgs) > 0), int> = 0>
  explicit shared_mutex(AllocatorArgs &&...args)
      : m_allocator{std::forward<AllocatorArgs>(args)...},
        m_mutex_handle{m_allocator.create_mutex()},
        m_reader_slots_handle{m_allocator.create_counting(
            FREERTOS_CPP_WRAPPERS_SHARED_MUTEX_MAX_READERS)},
        m_max_readers{FREERTOS_CPP_WRAPPERS_SHARED_MUTEX_MAX_READERS} {
    configASSERT(m_mutex_handle);
    configASSERT(m_reader_slots_handle);
  }

  shared_mutex(const shared_mutex &) = delete;
  shared_mutex(shared_mutex &&) = delete;
  shared_mutex &operator=(const shared_mutex &) = delete;
  shared_mutex &operator=(shared_mutex &&) = delete;

  ~shared_mutex(void) noexcept {
    if (m_reader_slots_handle) {
      vSemaphoreDelete(m_reader_slots_handle);
    }
    if (m_mutex_handle) {
      vSemaphoreDelete(m_mutex_handle);
    }
  }

  void lock() FREERTOS_ACQUIRE() {
    xSemaphoreTake(m_mutex_handle, portMAX_DELAY);
    for (UBaseType_t i = 0; i < m_max_readers; i++) {
      xSemaphoreTake(m_reader_slots_handle, portMAX_DELAY);
    }
  }

  bool try_lock() FREERTOS_TRY_ACQUIRE(true) {
    if (xSemaphoreTake(m_mutex_handle, 0) != pdTRUE) {
      return false;
    }
    UBaseType_t taken = 0;
    for (UBaseType_t i = 0; i < m_max_readers; i++) {
      if (xSemaphoreTake(m_reader_slots_handle, 0) == pdTRUE) {
        taken++;
      } else {
        break;
      }
    }
    if (taken < m_max_readers) {
      for (UBaseType_t i = 0; i < taken; i++) {
        xSemaphoreGive(m_reader_slots_handle);
      }
      xSemaphoreGive(m_mutex_handle);
      return false;
    }
    return true;
  }

  void unlock() FREERTOS_RELEASE() {
    for (UBaseType_t i = 0; i < m_max_readers; i++) {
      xSemaphoreGive(m_reader_slots_handle);
    }
    xSemaphoreGive(m_mutex_handle);
  }

  template <typename Rep, typename Period>
  bool try_lock_for(const std::chrono::duration<Rep, Period> &timeout) {
    auto ms = static_cast<TickType_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
    TickType_t total_ticks = pdMS_TO_TICKS(ms);
    if (!xSemaphoreTake(m_mutex_handle, total_ticks)) {
      return false;
    }
    TickType_t start = xTaskGetTickCount();
    UBaseType_t taken = 0;
    for (UBaseType_t i = 0; i < m_max_readers; i++) {
      TickType_t elapsed = xTaskGetTickCount() - start;
      TickType_t remaining =
          (total_ticks > elapsed) ? (total_ticks - elapsed) : 0;
      if (!xSemaphoreTake(m_reader_slots_handle, remaining)) {
        for (UBaseType_t j = 0; j < taken; j++) {
          xSemaphoreGive(m_reader_slots_handle);
        }
        xSemaphoreGive(m_mutex_handle);
        return false;
      }
      taken++;
    }
    return true;
  }

  template <typename Clock, typename Duration>
  bool try_lock_until(const std::chrono::time_point<Clock, Duration> &tp) {
    auto now = Clock::now();
    if (tp <= now) {
      return try_lock();
    }
    return try_lock_for(
        std::chrono::duration_cast<std::chrono::milliseconds>(tp - now));
  }

  [[nodiscard]] expected<void, error>
  lock_ex(const TickType_t ticks_to_wait = portMAX_DELAY) {
    if (!xSemaphoreTake(m_mutex_handle, ticks_to_wait)) {
      return unexpected<error>(ticks_to_wait == 0 ? error::would_block
                                                   : error::timeout);
    }
    TickType_t start = xTaskGetTickCount();
    UBaseType_t taken = 0;
    for (UBaseType_t i = 0; i < m_max_readers; i++) {
      TickType_t elapsed = xTaskGetTickCount() - start;
      TickType_t remaining =
          (ticks_to_wait > elapsed) ? (ticks_to_wait - elapsed) : 0;
      if (!xSemaphoreTake(m_reader_slots_handle, remaining)) {
        for (UBaseType_t j = 0; j < taken; j++) {
          xSemaphoreGive(m_reader_slots_handle);
        }
        xSemaphoreGive(m_mutex_handle);
        return unexpected<error>(error::timeout);
      }
      taken++;
    }
    return {};
  }

  [[nodiscard]] expected<void, error> try_lock_ex() {
    if (!try_lock()) {
      return unexpected<error>(error::would_block);
    }
    return {};
  }

  template <typename Rep, typename Period>
  [[nodiscard]] expected<void, error>
  lock_ex(const std::chrono::duration<Rep, Period> &timeout) {
    auto ms = static_cast<TickType_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
    return lock_ex(pdMS_TO_TICKS(ms));
  }

  [[nodiscard]] expected<void, error> unlock_ex() {
    unlock();
    return {};
  }

  void lock_shared() FREERTOS_ACQUIRE_SHARED() {
    xSemaphoreTake(m_reader_slots_handle, portMAX_DELAY);
  }

  bool try_lock_shared() FREERTOS_TRY_ACQUIRE_SHARED(true) {
    return xSemaphoreTake(m_reader_slots_handle, 0) == pdTRUE;
  }

  void unlock_shared() FREERTOS_RELEASE_SHARED() {
    xSemaphoreGive(m_reader_slots_handle);
  }

  template <typename Rep, typename Period>
  bool try_lock_shared_for(const std::chrono::duration<Rep, Period> &timeout) {
    auto ms = static_cast<TickType_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
    return xSemaphoreTake(m_reader_slots_handle,
                          pdMS_TO_TICKS(ms)) == pdTRUE;
  }

  template <typename Clock, typename Duration>
  bool
  try_lock_shared_until(const std::chrono::time_point<Clock, Duration> &tp) {
    auto now = Clock::now();
    if (tp <= now) {
      return try_lock_shared();
    }
    return try_lock_shared_for(
        std::chrono::duration_cast<std::chrono::milliseconds>(tp - now));
  }

  [[nodiscard]] expected<void, error>
  lock_shared_ex(const TickType_t ticks_to_wait = portMAX_DELAY) {
    if (!xSemaphoreTake(m_reader_slots_handle, ticks_to_wait)) {
      return unexpected<error>(ticks_to_wait == 0 ? error::would_block
                                                   : error::timeout);
    }
    return {};
  }

  [[nodiscard]] expected<void, error> try_lock_shared_ex() {
    if (!try_lock_shared()) {
      return unexpected<error>(error::would_block);
    }
    return {};
  }

  template <typename Rep, typename Period>
  [[nodiscard]] expected<void, error>
  lock_shared_ex(const std::chrono::duration<Rep, Period> &timeout) {
    auto ms = static_cast<TickType_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
    return lock_shared_ex(pdMS_TO_TICKS(ms));
  }

  [[nodiscard]] expected<void, error> unlock_shared_ex() {
    if (xSemaphoreGive(m_reader_slots_handle) == pdTRUE) {
      return {};
    }
    return unexpected<error>(error::semaphore_not_owned);
  }

  SemaphoreHandle_t native_handle() const noexcept { return m_mutex_handle; }
};

#if configSUPPORT_STATIC_ALLOCATION
namespace sa {
using shared_mutex =
    freertos::shared_mutex<freertos::static_shared_mutex_allocator>;
} // namespace sa
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
namespace da {
using shared_mutex =
    freertos::shared_mutex<freertos::dynamic_shared_mutex_allocator>;
} // namespace da
#endif

} // namespace freertos

#endif // FREERTOS_CPP_WRAPPERS_HAS_MUTEX && FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE