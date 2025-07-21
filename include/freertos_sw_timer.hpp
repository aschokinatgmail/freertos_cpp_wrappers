/**********************************************************************************
@file freertos_sw_timer.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS software timer wrapper
@version 0.1
@date 2024-04-07

The MIT License (MIT)

FreeRTOS C++ Wrappers Library
https://github.com/aschokinatgmail/freertos_cpp_wrappers/

Copyright(c) 2024 Andrey V. Shchekin <aschokin@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
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

#include <FreeRTOS.h>
#include <chrono>
#include <cstdbool>
#include <functional>
#include <task.h>
#include <ctime>
#include <timers.h>

namespace freertos {

#if configUSE_TIMERS

using std::function;

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief An allocator for the software timer that uses a static memory
 * allocation.
 *
 */
class static_sw_timer_allocator {
  StaticTimer_t m_timer_placeholder{};

public:
  static_sw_timer_allocator() = default;
  ~static_sw_timer_allocator() = default;
  static_sw_timer_allocator(const static_sw_timer_allocator &) = delete;
  static_sw_timer_allocator(static_sw_timer_allocator &&) = default;

  static_sw_timer_allocator &
  operator=(const static_sw_timer_allocator &) = delete;
  static_sw_timer_allocator &operator=(static_sw_timer_allocator &&) = delete;

  TimerHandle_t create(const char *name, const TickType_t period_ticks,
                       UBaseType_t auto_reload, void *const timer_id,
                       TimerCallbackFunction_t callback) {
    return xTimerCreateStatic(name, period_ticks, auto_reload, timer_id,
                              callback, &m_timer_placeholder);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief An allocator for the software timer that uses a dynamic memory
 * allocation.
 *
 */
class dynamic_sw_timer_allocator {
public:
  TimerHandle_t create(const char *name, const TickType_t period_ticks,
                       UBaseType_t auto_reload, void *const timer_id,
                       TimerCallbackFunction_t callback) {
    return xTimerCreate(name, period_ticks, auto_reload, timer_id, callback);
  }
};
#endif

/**
 * @brief Timer callback routine type definition based on std::function.
 *
 */
using timer_callback_t = function<void()>;

/**
 * @brief A wrapper for the FreeRTOS software timer.
 *
 * @tparam SwTimerAllocator type of the software timer allocator
 */
template <typename SwTimerAllocator> class timer {
  SwTimerAllocator m_allocator;
  TimerHandle_t m_timer;
  timer_callback_t m_callback;
  uint8_t m_started : 1;

  static void callback_wrapper(TimerHandle_t t) {
    auto *const self = static_cast<timer *>(pvTimerGetTimerID(t));
    configASSERT(self);
    self->m_callback();
  }

public:
  /**
   * @brief Construct a new timer object
   *
   * @param name name of the timer
   * @param period_ticks period of the timer in ticks
   * @param auto_reload auto-reload flag
   * @param callback callback routine
   */
  explicit timer(const char *name, const TickType_t period_ticks,
                 UBaseType_t auto_reload, timer_callback_t &&callback)
      : m_timer{nullptr}, m_callback{std::move(callback)}, m_started{false} {
    m_timer = m_allocator.create(name, period_ticks, auto_reload, this,
                                 callback_wrapper);
    configASSERT(m_timer);
  }
  /**
   * @brief Construct a new timer object
   *
   * @tparam Rep duration representation type
   * @tparam Period duration period type
   * @param name name of the timer
   * @param period period of the timer
   * @param auto_reload auto-reload flag
   * @param callback callback routine
   */
  template <typename Rep, typename Period>
  explicit timer(const char *name,
                 const std::chrono::duration<Rep, Period> &period,
                 UBaseType_t auto_reload, timer_callback_t &&callback)
      : timer{name,
              static_cast<TickType_t>(
                  std::chrono::duration_cast<std::chrono::milliseconds>(period)
                      .count()),
              auto_reload, std::move(callback)} {}
  timer(const timer &) = delete;
  /**
   * @brief Move constructor that properly transfers timer ownership.
   * 
   * This constructor ensures that when a timer is moved, the source timer's
   * handle is set to nullptr to prevent double deletion. Previously, the
   * default move constructor would perform a shallow copy, causing both
   * source and destination timers to share the same handle, leading to
   * premature timer deletion when the source was destroyed.
   * 
   * @param src The source timer to move from (will be invalidated)
   */
  timer(timer &&src) noexcept 
      : m_allocator(std::move(src.m_allocator)), 
        m_timer(src.m_timer), 
        m_callback(std::move(src.m_callback)), 
        m_started(src.m_started) {
    // Transfer ownership: clear the source timer handle to prevent double deletion
    src.m_timer = nullptr;
    src.m_started = false;
  }
  /**
   * @brief Destruct the timer object and delete the software timer kernel
   * object instance if it was created.
   *
   */
  ~timer(void) {
    if (m_timer) {
      auto rc = xTimerDelete(m_timer, portMAX_DELAY);
      if (rc == pdPASS) {
        while (xTimerIsTimerActive(m_timer) != pdFALSE) {
          vTaskDelay(pdMS_TO_TICKS(1));
        }
      }
    }
  }

  timer &operator=(const timer &) = delete;
  timer &operator=(timer &&src) noexcept {
    if (this != &src) {
      if (m_timer) {
        xTimerDelete(m_timer, portMAX_DELAY);
      }
      auto rc = xTimerStop(src.m_timer, portMAX_DELAY);
      if (rc == pdPASS) {
        while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
          vTaskDelay(pdMS_TO_TICKS(1));
        }
        auto name = pcTimerGetName(src.m_timer);
        auto period = xTimerGetPeriod(src.m_timer);
        auto auto_reload = uxTimerGetReloadMode(src.m_timer);
        rc = xTimerDelete(src.m_timer, portMAX_DELAY);
        if (rc == pdPASS) {
          while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
            vTaskDelay(pdMS_TO_TICKS(1));
          }
          src.m_timer = nullptr;
          m_callback = std::move(src.m_callback);
          m_timer = m_allocator.create(name, period, auto_reload, this,
                                       callback_wrapper);
          if (m_timer) {
            if (src.m_started) {
              rc = xTimerStart(m_timer, portMAX_DELAY);
              if (rc == pdPASS) {
                m_started = true;
              }
            }
          }
        }
      }
    }
    return *this;
  }

  /**
   * @brief Method to start the timer.
   * @ref https://www.freertos.org/xTimerStart.html
   *
   * @param ticks_to_wait timeout in ticks to wait for the timer to start
   * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
   */
  BaseType_t start(const TickType_t ticks_to_wait = portMAX_DELAY) {
    if (!m_timer) {
      return pdFAIL;
    }
    auto rc = xTimerStart(m_timer, ticks_to_wait);
    if (rc) {
      m_started = true;
    }
    return rc;
  }
  /**
   * @brief Method to start the timer.
   * @ref https://www.freertos.org/xTimerStart.html
   *
   * @tparam Rep duration representation type
   * @tparam Period duration period type
   * @param timeout timeout to wait for the timer to start
   * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
   */
  template <typename Rep, typename Period>
  BaseType_t start(const std::chrono::duration<Rep, Period> &timeout) {
    return start(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  /**
   * @brief Method to start the timer from an ISR.
   * @ref https://www.freertos.org/xTimerStartFromISR.html
   *
   * @param high_priority_task_woken flag to indicate if a high priority task
   * was woken
   * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
   */
  BaseType_t start_isr(BaseType_t &high_priority_task_woken) {
    if (!m_timer) {
      return pdFAIL;
    }
    auto rc = xTimerStartFromISR(m_timer, &high_priority_task_woken);
    if (rc) {
      m_started = true;
    }
    return rc;
  }
  /**
   * @brief Method to start the timer from an ISR.
   * @ref https://www.freertos.org/xTimerStartFromISR.html
   *
   * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
   */
  BaseType_t start_isr(void) {
    BaseType_t high_priority_task_woken = pdFALSE;
    return start_isr(high_priority_task_woken);
  }
  /**
   * @brief Method to stop the timer.
   * @ref https://www.freertos.org/xTimerStop.html
   *
   * @param ticks_to_wait timeout in ticks to wait for the timer to stop
   * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
   */
  BaseType_t stop(const TickType_t ticks_to_wait = portMAX_DELAY) {
    if (!m_timer) {
      return pdFAIL;
    }
    auto rc = xTimerStop(m_timer, ticks_to_wait);
    if (rc) {
      m_started = false;
    }
    return rc;
  }
  /**
   * @brief Method to stop the timer.
   * @ref https://www.freertos.org/xTimerStop.html
   *
   * @tparam Rep duration representation type
   * @tparam Period duration period type
   * @param timeout timeout to wait for the timer to stop
   * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
   */
  template <typename Rep, typename Period>
  BaseType_t stop(const std::chrono::duration<Rep, Period> &timeout) {
    return stop(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  /**
   * @brief Method to stop the timer from an ISR.
   * @ref https://www.freertos.org/xTimerStopFromISR.html
   *
   * @param high_priority_task_woken flag to indicate if a high priority task
   * was woken
   * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
   */
  BaseType_t stop_isr(BaseType_t &high_priority_task_woken) {
    if (!m_timer) {
      return pdFAIL;
    }
    auto rc = xTimerStopFromISR(m_timer, &high_priority_task_woken);
    if (rc) {
      m_started = false;
    }
    return rc;
  }
  /**
   * @brief Method to stop the timer from an ISR.
   * @ref https://www.freertos.org/xTimerStopFromISR.html
   *
   * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
   */
  BaseType_t stop_isr(void) {
    BaseType_t high_priority_task_woken = pdFALSE;
    return stop_isr(high_priority_task_woken);
  }
  /**
   * @brief Method to reset the timer.
   * @ref https://www.freertos.org/xTimerReset.html
   *
   * @param ticks_to_wait timeout in ticks to wait for the timer to reset
   * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
   */
  BaseType_t reset(const TickType_t ticks_to_wait = portMAX_DELAY) {
    if (!m_timer) {
      return pdFAIL;
    }
    return xTimerReset(m_timer, ticks_to_wait);
  }
  /**
   * @brief Method to reset the timer.
   * @ref https://www.freertos.org/xTimerReset.html
   *
   * @tparam Rep duration representation type
   * @tparam Period duration period type
   * @param timeout timeout to wait for the timer to reset
   * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
   */
  template <typename Rep, typename Period>
  BaseType_t reset(const std::chrono::duration<Rep, Period> &timeout) {
    return reset(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  /**
   * @brief Method to reset the timer from an ISR.
   * @ref https://www.freertos.org/xTimerResetFromISR.html
   *
   * @param high_priority_task_woken flag to indicate if a high priority task
   * was woken
   * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
   */
  BaseType_t reset_isr(BaseType_t &high_priority_task_woken) {
    if (!m_timer) {
      return pdFAIL;
    }
    return xTimerResetFromISR(m_timer, &high_priority_task_woken);
  }
  /**
   * @brief Method to reset the timer from an ISR.
   * @ref https://www.freertos.org/xTimerResetFromISR.html
   *
   * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
   */
  BaseType_t reset_isr(void) {
    BaseType_t high_priority_task_woken = pdFALSE;
    return reset_isr(high_priority_task_woken);
  }
  /**
   * @brief Method to change the period of the timer.
   * @ref https://www.freertos.org/xTimerChangePeriod.html
   *
   * @param new_period_ticks new period of the timer in ticks
   * @param ticks_to_wait timeout in ticks to wait for the timer to change the
   * period
   * @return BaseType_t pdPASS if the timer period was changed successfully else
   * pdFAIL
   */
  BaseType_t period(const TickType_t new_period_ticks,
                    const TickType_t ticks_to_wait = portMAX_DELAY) {
    if (!m_timer) {
      return pdFAIL;
    }
    return xTimerChangePeriod(m_timer, new_period_ticks, ticks_to_wait);
  }
  /**
   * @brief Method to change the period of the timer.
   * @ref https://www.freertos.org/xTimerChangePeriod.html
   *
   * @tparam RepPeriod duration representation type for new period
   * @tparam PeriodPeriod duration period type for new period
   * @tparam RepTimeout duration representation type for timeout
   * @tparam PeriodTimeout duration period type for timeout
   * @param new_period new period of the timer
   * @param timeout timeout to wait for the timer to change the period
   * @return BaseType_t pdPASS if the timer period was changed successfully else
   * pdFAIL
   */
  template <typename RepPeriod, typename PeriodPeriod, typename RepTimeout, typename PeriodTimeout>
  BaseType_t period(const std::chrono::duration<RepPeriod, PeriodPeriod> &new_period,
                    const std::chrono::duration<RepTimeout, PeriodTimeout> &timeout) {
    return period(
        std::chrono::duration_cast<std::chrono::milliseconds>(new_period)
            .count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  /**
   * @brief Method to change the period of the timer from an ISR.
   * @ref https://www.freertos.org/xTimerChangePeriodFromISR.html
   *
   * @param new_period_ticks new period of the timer in ticks
   * @param high_priority_task_woken flag to indicate if a high priority task
   * was woken
   * @return BaseType_t pdPASS if the timer period was changed successfully else
   * pdFAIL
   */
  BaseType_t period_isr(const TickType_t new_period_ticks,
                        BaseType_t &high_priority_task_woken) {
    if (!m_timer) {
      return pdFAIL;
    }
    return xTimerChangePeriodFromISR(m_timer, new_period_ticks,
                                     &high_priority_task_woken);
  }
  /**
   * @brief Method to change the period of the timer from an ISR.
   * @ref https://www.freertos.org/xTimerChangePeriodFromISR.html
   *
   * @tparam Rep duration representation type
   * @tparam Period duration period type
   * @param new_period new period of the timer
   * @param high_priority_task_woken flag to indicate if a high priority task
   * was woken
   * @return BaseType_t pdPASS if the timer period was changed successfully else
   * pdFAIL
   */
  template <typename Rep, typename Period>
  BaseType_t period_isr(const std::chrono::duration<Rep, Period> &new_period,
                        BaseType_t &high_priority_task_woken) {
    return period_isr(
        std::chrono::duration_cast<std::chrono::milliseconds>(new_period)
            .count(),
        high_priority_task_woken);
  }
  /**
   * @brief Method to change the period of the timer from an ISR.
   * @ref https://www.freertos.org/xTimerChangePeriodFromISR.html
   *
   * @param new_period_ticks new period of the timer in ticks
   * @return BaseType_t pdPASS if the timer period was changed successfully else
   * pdFAIL
   */
  BaseType_t period_isr(const TickType_t new_period_ticks) {
    BaseType_t high_priority_task_woken = pdFALSE;
    return period_isr(new_period_ticks, high_priority_task_woken);
  }
  /**
   * @brief Method to change the period of the timer from an ISR.
   * @ref https://www.freertos.org/xTimerChangePeriodFromISR.html
   *
   * @tparam Rep duration representation type
   * @tparam Period duration period type
   * @param new_period new period of the timer
   * @return BaseType_t pdPASS if the timer period was changed successfully else
   * pdFAIL
   */
  template <typename Rep, typename Period>
  BaseType_t period_isr(const std::chrono::duration<Rep, Period> &new_period) {
    return period_isr(
        std::chrono::duration_cast<std::chrono::milliseconds>(new_period)
            .count());
  }
  /**
   * @brief Method to change the period of the timer from an ISR.
   * @ref https://www.freertos.org/xTimerChangePeriodFromISR.html
   *
   * @tparam Rep duration representation type
   * @tparam Period duration period type
   * @param new_period new period of the timer
   * @param high_priority_task_woken flag to indicate if a high priority task
   * was woken
   * @return TickType_t timer period in ticks
   */
  TickType_t period_ticks(void) const {
    if (!m_timer) {
      return 0;
    }
    return xTimerGetPeriod(m_timer);
  }
  /**
   * @brief Method to get the period of the timer.
   *
   * @return std::chrono::milliseconds period of the timer in milliseconds
   */
  std::chrono::milliseconds period(void) const {
    return std::chrono::milliseconds{period_ticks()};
  }
  /**
   * @brief Method to change the timer reload mode flag.
   * @ref https://www.freertos.org/FreeeRTOS-timers-vTimerSetReloadMode.html
   *
   * @param auto_reload pdTRUE to enable auto-reload mode, pdFALSE to disable
   * @return timer& reference to the timer object
   */
  timer &reload_mode(UBaseType_t auto_reload) {
    if (m_timer) {
      vTimerSetReloadMode(m_timer, auto_reload);
    }
    return *this;
  }
  /**
   * @brief Method to get the timer reload mode flag.
   * @ref https://www.freertos.org/xTimerGetReloadMode.html
   *
   * @return UBaseType_t pdTRUE if auto-reload mode is enabled, pdFALSE
   * otherwise
   */
  UBaseType_t reload_mode(void) const { return uxTimerGetReloadMode(m_timer); }
  /**
   * @brief Method to get number of remaining ticks before the timer expires.
   *
   * @return TickType_t number of remaining ticks before the timer expires.
   */
  TickType_t remaining_ticks(void) const {
    if (m_timer) {
      return xTimerGetExpiryTime(m_timer) - xTaskGetTickCount();
    } else {
      return 0;
    }
  }
  /**
   * @brief Method to get the remaining time before the timer expires.
   *
   * @return std::chrono::milliseconds remaining time before the timer expires
   * in milliseconds.
   */
  std::chrono::milliseconds remaining_time(void) const {
    return std::chrono::milliseconds{remaining_ticks()};
  }
  /**
   * @brief Method to check if the timer is running.
   * @ref https://www.freertos.org/xTimerIsTimerActive.html
   *
   * @return BaseType_t pdTRUE if the timer is running, pdFALSE otherwise
   */
  BaseType_t running(void) const {
    if (!m_timer) {
      return pdFALSE;
    }
    return xTimerIsTimerActive(m_timer);
  }
  /**
   * @brief Method to get the name of the timer.
   *
   * @return const char* name of the timer
   */
  const char *name(void) const {
    if (!m_timer) {
      return nullptr;
    }
    return pcTimerGetName(m_timer);
  }
};

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief Namespace for the kernel objects static memory allocation.
 *
 */
namespace sa {
/**
 * @brief Alias for the software timer that uses a static memory allocation.
 *
 */
using timer = freertos::timer<freertos::static_sw_timer_allocator>;
} // namespace sa
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief Namespace for the kernel objects dynamic memory allocation.
 *
 */
namespace da {
/**
 * @brief Alias for the software timer that uses a dynamic memory allocation.
 *
 */
using timer = freertos::timer<freertos::dynamic_sw_timer_allocator>;
} // namespace da
#endif

#endif

} // namespace freertos
