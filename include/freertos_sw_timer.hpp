/**
 * @file freertos_sw_timer.hpp
 * @author Andrey V. Shchekin <aschokin@gmail.com>
 * @brief FreeRTOS software timer wrapper
 * @version 0.1
 * @date 2024-04-07
 *
 * @copyright Copyright (c) 2024 Andrey V. Shchekin
 *
 */

#pragma once

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#include <FreeRTOS.h>
#include <chrono>
#include <cstdbool>
#include <functional>
#include <task.h>
#include <time.h>
#include <timers.h>

namespace freertos {

#if configUSE_TIMERS

using std::function;

#if configSUPPORT_STATIC_ALLOCATION
class static_sw_timer_allocator {
  StaticTimer_t m_timer_placeholder;

public:
  static_sw_timer_allocator() = default;
  static_sw_timer_allocator(const static_sw_timer_allocator &) = delete;
  static_sw_timer_allocator(static_sw_timer_allocator &&) = delete;

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
class dynamic_sw_timer_allocator {
public:
  TimerHandle_t create(const char *name, const TickType_t period_ticks,
                       UBaseType_t auto_reload, void *const timer_id,
                       TimerCallbackFunction_t callback) {
    return xTimerCreate(name, period_ticks, auto_reload, timer_id, callback);
  }
};
#endif

using timer_callback_t = function<void()>;

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
  explicit timer(const char *name, const TickType_t period_ticks,
                 UBaseType_t auto_reload, timer_callback_t &&callback)
      : m_timer{nullptr}, m_callback{callback}, m_started{false} {
    m_timer = m_allocator.create(name, period_ticks, auto_reload, this,
                                 callback_wrapper);
    configASSERT(m_timer);
  }
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
  timer(timer &&src) = delete;
  ~timer(void) {
    if (m_timer) {
      xTimerDelete(m_timer, portMAX_DELAY);
    }
  }

  timer &operator=(const timer &) = delete;
  timer &operator=(timer &&src) = delete;

  BaseType_t start(const TickType_t ticks_to_wait = portMAX_DELAY) {
    auto rc = xTimerStart(m_timer, ticks_to_wait);
    if (rc) {
      m_started = true;
    }
    return rc;
  }
  template <typename Rep, typename Period>
  BaseType_t start(const std::chrono::duration<Rep, Period> &timeout) {
    return start(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  BaseType_t start_isr(BaseType_t &high_priority_task_woken) {
    auto rc = xTimerStartFromISR(m_timer, &high_priority_task_woken);
    if (rc) {
      m_started = true;
    }
    return rc;
  }
  BaseType_t stop(const TickType_t ticks_to_wait = portMAX_DELAY) {
    auto rc = xTimerStop(m_timer, ticks_to_wait);
    if (rc) {
      m_started = false;
    }
    return rc;
  }
  template <typename Rep, typename Period>
  BaseType_t stop(const std::chrono::duration<Rep, Period> &timeout) {
    return stop(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  BaseType_t stop_isr(BaseType_t &high_priority_task_woken) {
    auto rc = xTimerStopFromISR(m_timer, &high_priority_task_woken);
    if (rc) {
      m_started = false;
    }
    return rc;
  }
  BaseType_t reset(const TickType_t ticks_to_wait = portMAX_DELAY) {
    return xTimerReset(m_timer, ticks_to_wait);
  }
  template <typename Rep, typename Period>
  BaseType_t reset(const std::chrono::duration<Rep, Period> &timeout) {
    return reset(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  BaseType_t reset_isr(BaseType_t &high_priority_task_woken) {
    return xTimerResetFromISR(m_timer, &high_priority_task_woken);
  }
  BaseType_t period(const TickType_t new_period_ticks,
                    const TickType_t ticks_to_wait = portMAX_DELAY) {
    return xTimerChangePeriod(m_timer, new_period_ticks, ticks_to_wait);
  }
  template <typename Rep, typename Period>
  BaseType_t period(const std::chrono::duration<Rep, Period> &new_period,
                    const std::chrono::duration<Rep, Period> &timeout) {
    return period(
        std::chrono::duration_cast<std::chrono::milliseconds>(new_period)
            .count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  BaseType_t period_isr(const TickType_t new_period_ticks,
                        BaseType_t &high_priority_task_woken) {
    return xTimerChangePeriodFromISR(m_timer, new_period_ticks,
                                     &high_priority_task_woken);
  }
  BaseType_t period_ticks(void) const { return xTimerGetPeriod(m_timer); }
  std::chrono::milliseconds period(void) const {
    return std::chrono::milliseconds{period_ticks()};
  }
  timer &reload_mode(UBaseType_t auto_reload) {
    vTimerSetReloadMode(m_timer, auto_reload);
    return *this;
  }
  UBaseType_t reload_mode(void) const { return uxTimerGetReloadMode(m_timer); }
  BaseType_t remaining_ticks(void) const {
    return xTimerGetExpiryTime(m_timer) - xTaskGetTickCount();
  }
  std::chrono::milliseconds remaining_time(void) const {
    return std::chrono::milliseconds{remaining_ticks()};
  }
  BaseType_t running(void) const { return xTimerIsTimerActive(m_timer); }
  const char *name(void) const { return pcTimerGetName(m_timer); }
};

#endif

} // namespace freertos
