/**********************************************************************************
@file freertos_condition_variable.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS condition_variable_any wrapper
@version 3.2.0
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
#include "freertos_thread_safety.hpp"
#include <FreeRTOS.h>
#include <chrono>
#include <ctime>
#include <semphr.h>
#include <task.h>
#include <type_traits>
#include <utility>

namespace freertos {

#ifndef FREERTOS_CPP_WRAPPERS_CONDITION_VARIABLE_MAX_WAITERS
#define FREERTOS_CPP_WRAPPERS_CONDITION_VARIABLE_MAX_WAITERS 8
#endif

class condition_variable_any {
  SemaphoreHandle_t m_semaphore{nullptr};

#if configSUPPORT_STATIC_ALLOCATION
  StaticSemaphore_t m_semaphore_storage{};
  uint8_t m_semaphore_created{0};
#endif

public:
  condition_variable_any() {
#if configSUPPORT_STATIC_ALLOCATION
    m_semaphore =
        xSemaphoreCreateCountingStatic(FREERTOS_CPP_WRAPPERS_CONDITION_VARIABLE_MAX_WAITERS, 0,
                                       &m_semaphore_storage);
    configASSERT(m_semaphore != nullptr);
    m_semaphore_created = 1;
#else
    m_semaphore = xSemaphoreCreateCounting(FREERTOS_CPP_WRAPPERS_CONDITION_VARIABLE_MAX_WAITERS, 0);
    configASSERT(m_semaphore != nullptr);
#endif
  }

  condition_variable_any(const condition_variable_any &) = delete;
  condition_variable_any &operator=(const condition_variable_any &) = delete;

  ~condition_variable_any() noexcept {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  void notify_one() noexcept {
    configASSERT(m_semaphore != nullptr);
    if (!m_semaphore) {
      return;
    }
    (void)xSemaphoreGive(m_semaphore);
  }

  void notify_all() noexcept {
    configASSERT(m_semaphore != nullptr);
    if (!m_semaphore) {
      return;
    }
    for (UBaseType_t i = 0; i < FREERTOS_CPP_WRAPPERS_CONDITION_VARIABLE_MAX_WAITERS; i++) {
      xSemaphoreGive(m_semaphore);
    }
  }

  isr_result<void> notify_one_isr() noexcept {
    if (!m_semaphore) {
      return {pdFALSE};
    }
    isr_result<void> result{pdFALSE};
    xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
    return result;
  }

  isr_result<void> notify_all_isr() noexcept {
    if (!m_semaphore) {
      return {pdFALSE};
    }
    isr_result<void> result{pdFALSE};
    for (UBaseType_t i = 0; i < FREERTOS_CPP_WRAPPERS_CONDITION_VARIABLE_MAX_WAITERS; i++) {
      BaseType_t woken = pdFALSE;
      xSemaphoreGiveFromISR(m_semaphore, &woken);
      if (woken) {
        result.higher_priority_task_woken = pdTRUE;
      }
    }
    return result;
  }

  template <typename Lock> void wait(Lock &lock) {
    if (!m_semaphore) {
      lock.unlock();
      lock.lock();
      return;
    }
    lock.unlock();
    xSemaphoreTake(m_semaphore, portMAX_DELAY);
    lock.lock();
  }

  template <typename Lock, typename Predicate>
  bool wait(Lock &lock, Predicate pred) {
    while (!pred()) {
      wait(lock);
    }
    return true;
  }

  template <typename Lock, typename Rep, typename Period>
  std::cv_status wait_for(Lock &lock,
                           const std::chrono::duration<Rep, Period> &rel_time) {
    if (!m_semaphore) {
      lock.unlock();
      lock.lock();
      return std::cv_status::timeout;
    }
    lock.unlock();
    auto ms = static_cast<TickType_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(rel_time).count());
    TickType_t ticks = pdMS_TO_TICKS(ms);
    BaseType_t result = xSemaphoreTake(m_semaphore, ticks);
    lock.lock();
    return (result == pdTRUE) ? std::cv_status::no_timeout
                              : std::cv_status::timeout;
  }

  template <typename Lock, typename Rep, typename Period, typename Predicate>
  bool wait_for(Lock &lock,
                const std::chrono::duration<Rep, Period> &rel_time,
                Predicate pred) {
    auto deadline = std::chrono::steady_clock::now() + rel_time;
    while (!pred()) {
      if (std::chrono::steady_clock::now() >= deadline) {
        return pred();
      }
      auto remaining = deadline - std::chrono::steady_clock::now();
      if (remaining.count() <= 0) {
        return pred();
      }
      auto status = wait_for(lock, remaining);
      if (status == std::cv_status::timeout) {
        return pred();
      }
    }
    return true;
  }

  template <typename Lock, typename Clock, typename Duration>
  std::cv_status
  wait_until(Lock &lock,
             const std::chrono::time_point<Clock, Duration> &abs_time) {
    auto now = Clock::now();
    if (abs_time <= now) {
      return std::cv_status::timeout;
    }
    return wait_for(lock, abs_time - now);
  }

  template <typename Lock, typename Clock, typename Duration, typename Predicate>
  bool
  wait_until(Lock &lock,
             const std::chrono::time_point<Clock, Duration> &abs_time,
             Predicate pred) {
    auto now = Clock::now();
    if (abs_time <= now) {
      return pred();
    }
    return wait_for(lock, abs_time - now, pred);
  }

  [[nodiscard]] expected<void, error> notify_one_ex() noexcept {
    if (!m_semaphore) {
      return unexpected<error>(error::invalid_handle);
    }
    auto rc = xSemaphoreGive(m_semaphore);
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(error::would_block);
  }

  [[nodiscard]] expected<void, error> notify_all_ex() noexcept {
    if (!m_semaphore) {
      return unexpected<error>(error::invalid_handle);
    }
    notify_all();
    return {};
  }

  SemaphoreHandle_t native_handle() const noexcept { return m_semaphore; }
};

#if configSUPPORT_STATIC_ALLOCATION
namespace sa {
using condition_variable_any = freertos::condition_variable_any;
} // namespace sa
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
namespace da {
using condition_variable_any = freertos::condition_variable_any;
} // namespace da
#endif

} // namespace freertos