/**
 *
 * @file freertos_task.hpp
 * @author Andrey V. Shchekin <aschokin@gmail.com>
 * @brief convenience wrappers for FreeRTOS tasks
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
#include <cassert>
#include <chrono>
#include <functional>
#include <semphr.h>
#include <string>
#include <task.h>

namespace freertos {

using namespace std::chrono_literals;

#if configSUPPORT_STATIC_ALLOCATION
template <size_t StackSize> class static_task_allocator {
  StackType_t m_stackBuffer[StackSize / sizeof(StackType_t)];
  StaticTask_t m_taskBuffer;

public:
  static_task_allocator() = default;
  static_task_allocator(const static_task_allocator &) = delete;
  static_task_allocator(static_task_allocator &&) = delete;

  static_task_allocator &operator=(const static_task_allocator &) = delete;
  static_task_allocator &operator=(static_task_allocator &&) = delete;

  TaskHandle_t create(TaskFunction_t taskFunction, const char *name,
                      UBaseType_t priority, void *context) {
    return xTaskCreateStatic(taskFunction, name,
                             StackSize / sizeof(StackType_t), context, priority,
                             m_stackBuffer, &m_taskBuffer);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
template <size_t StackSize> class dynamic_task_allocator {
public:
  TaskHandle_t create(TaskFunction_t taskFunction, const char *name,
                      UBaseType_t priority, void *context) {
    TaskHandle_t hTask = nullptr;
    auto rc = xTaskCreate(taskFunction, name, StackSize / sizeof(StackType_t),
                          context, priority, &hTask);
    return pdPASS == rc ? hTask : nullptr;
  }
};
#endif

using task_routine_t = std::function<void(void)>;

template <typename TaskAllocator> class task {
  TaskAllocator m_allocator;
  TaskHandle_t m_hTask;
  task_routine_t m_taskRoutine;
  uint8_t m_start_suspended : 1;

  static void task_exec(void *context) {
    auto pThis = static_cast<task *>(context);
    assert(nullptr != pThis);
#if INCLUDE_vTaskSuspend
    if (pThis->m_start_suspended) {
      pThis->suspend();
    }
#endif
    pThis->m_taskRoutine();
  }

public:
  task(const char *name, UBaseType_t priority, task_routine_t &&task_routine,
       bool start_suspended = true)
      : m_allocator{}, m_hTask{nullptr}, m_taskRoutine{task_routine},
        m_start_suspended{start_suspended} {
    m_hTask = m_allocator.create(task_exec, name, priority, this);
  }
  task(const std::string &name, UBaseType_t priority,
       task_routine_t &&task_routine, bool start_suspended = true)
      : task{name.c_str(), priority,
             std::forward<std::function<void()>>(task_routine),
             start_suspended} {}
  task(const task &) = delete;
  task(task &&other) noexcept
      : m_allocator{std::move(other.m_allocator)}, m_hTask{other.m_hTask},
        m_taskRoutine{std::move(other.m_taskRoutine)} {
    other.m_hTask = nullptr;
  }
  ~task(void) {
#if INCLUDE_vTaskDelete
    if (m_hTask) {
      vTaskDelete(m_hTask);
    }
#endif
  }

  task &operator=(const task &) = delete;
  task &operator=(task &&other) noexcept {
    if (this != &other) {
      if (m_hTask) {
        vTaskDelete(m_hTask);
      }
      m_allocator = std::move(other.m_allocator);
      m_hTask = other.m_hTask;
      m_taskRoutine = std::move(other.m_taskRoutine);
      other.m_hTask = nullptr;
    }
    return *this;
  }

  TaskHandle_t handle(void) const { return m_hTask; }
#if INCLUDE_vTaskSuspend
  void suspend(void) { vTaskSuspend(m_hTask); }
  void resume(void) { vTaskResume(m_hTask); }
  BaseType_t resume_isr(void) { return xTaskResumeFromISR(m_hTask); }
#endif
  void terminate(void) { vTaskDelete(m_hTask); }
#if INCLUDE_xTaskAbortDelay
  BaseType_t abort_delay(void) { return xTaskAbortDelay(m_hTask); }
#endif
#if INCLUDE_uxTaskPriorityGet && configUSE_MUTEXES
  UBaseType_t priority(void) const { return uxTaskPriorityGet(m_hTask); }
  UBaseType_t priority_isr(void) const {
    return uxTaskPriorityGetFromISR(m_hTask);
  }
#endif
#if INCLUDE_vTaskPrioritySet
  task &priority(UBaseType_t priority) {
    vTaskPrioritySet(m_hTask, priority);
    return *this;
  }
#endif
#if configUSE_TRACE_FACILITY
  TaskStatus_t status(BaseType_t getFreeStackSpace = pdFALSE,
                      eTaskState eState = eInvalid) const {
    TaskStatus_t status;
    vTaskGetInfo(m_hTask, &status, getFreeStackSpace, eState);
    return status;
  }
#endif
#if configUSE_APPLICATION_TASK_TAG
  task &tag(TaskHookFunction_t tag) {
    vTaskSetApplicationTaskTag(m_hTask, tag);
    return *this;
  }
  TaskHookFunction_t tag(void) const {
    return ulTaskGetApplicationTaskTag(m_hTask);
  }
  TaskHookFunction_t tag_isr(void) const {
    return ulTaskGetApplicationTaskTagFromISR(m_hTask);
  }
#endif
#if INCLUDE_uxTaskGetStackHighWaterMark
  size_t stack_high_water_mark(void) const {
    return uxTaskGetStackHighWaterMark(m_hTask);
  }
#endif
#if INCLUDE_uxTaskGetStackHighWaterMark2
  size_t stack_high_water_mark2(void) const {
    return uxTaskGetStackHighWaterMark2(m_hTask);
  }
#endif
#if INCLUDE_eTaskGetState
  eTaskState state(void) const { return eTaskGetState(m_hTask); }
#endif
  const char *name(void) const { return pcTaskGetName(m_hTask); }
// Task notification API
#if configUSE_TASK_NOTIFICATIONS
  BaseType_t notify_give(void) { return xTaskNotifyGive(m_hTask); }
  uint32_t notify_take(BaseType_t clearCountOnExit = pdTRUE,
                       TickType_t ticksToWait = portMAX_DELAY) {
    return ulTaskNotifyTake(clearCountOnExit, ticksToWait);
  }
  template <typename Rep, typename Period>
  uint32_t notify_take(BaseType_t clearCountOnExit,
                       std::chrono::duration<Rep, Period> duration) {
    return notify_take(
        clearCountOnExit,
        std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            .count());
  }
  BaseType_t notify(const uint32_t val, eNotifyAction action) {
    return xTaskNotify(m_hTask, val, action);
  }
  BaseType_t notfy_and_query(const uint32_t val, eNotifyAction action,
                             uint32_t &prev_value) {
    return xTaskNotifyAndQuery(m_hTask, val, action, &prev_value);
  }
  BaseType_t notify_isr(const uint32_t val, eNotifyAction action,
                        BaseType_t &higherPriorityTaskWoken = nullptr) {
    return xTaskNotifyFromISR(m_hTask, val, action, &higherPriorityTaskWoken);
  }
  BaseType_t
  notify_and_query_isr(const uint32_t val, eNotifyAction action,
                       uint32_t &prev_value,
                       BaseType_t &higherPriorityTaskWoken = nullptr) {
    return xTaskNotifyAndQueryFromISR(m_hTask, val, action, &prev_value,
                                      &higherPriorityTaskWoken);
  }
  BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
                         uint32_t ulBitsToClearOnExit,
                         uint32_t &notification_value,
                         TickType_t xTicksToWait) {
    return xTaskNotifyWait(ulBitsToClearOnEntry, ulBitsToClearOnExit,
                           &notification_value, xTicksToWait);
  }
  template <typename Rep, typename Period>
  BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
                         uint32_t ulBitsToClearOnExit,
                         uint32_t &notification_value,
                         std::chrono::duration<Rep, Period> duration) {
    return notify_wait(
        ulBitsToClearOnEntry, ulBitsToClearOnExit, notification_value,
        std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            .count());
  }
  BaseType_t notify_state_clear(void) { return xTaskNotifyStateClear(m_hTask); }
  uint32_t notify_value_clear(uint32_t ulBitsToClear) {
    return ulTaskNotifyValueClear(m_hTask, ulBitsToClear);
  }
#endif
};

template <typename TaskAllocator> class periodic_task {
  std::chrono::milliseconds m_period;
  task_routine_t m_on_start;
  task_routine_t m_on_stop;
  task_routine_t m_periodic_routine;
  task<TaskAllocator> m_task;

  void run() {
    m_on_start();
    while (is_running()) {
      if (0 != m_period.count()) {
#if configUSE_TASK_NOTIFICATIONS
        uint32_t notification_value;
        m_task.notify_wait(0, 0, notification_value, m_period);
#else
        delay(m_period);
#endif
      }
      m_periodic_routine();
    }
    m_on_stop();
  }

public:
  template <typename Rep, typename Period>
  periodic_task(const char *name, UBaseType_t priority,
                task_routine_t &&on_start, task_routine_t &&on_stop,
                task_routine_t &&periodic_routine,
                const std::chrono::duration<Rep, Period> &period,
                bool start_suspended = true)
      : m_period{std::chrono::duration_cast<std::chrono::milliseconds>(period)},
        m_on_start{on_start}, m_on_stop{on_stop},
        m_periodic_routine{periodic_routine},
        m_task{name, priority, [this]() { run(); }, start_suspended} {}
  template <typename Rep, typename Period>
  periodic_task(const std::string &name, UBaseType_t priority,
                task_routine_t &&on_start, task_routine_t &&on_stop,
                task_routine_t &&periodic_routine,
                const std::chrono::duration<Rep, Period> &period,
                bool start_suspended = true)
      : periodic_task{name.c_str(),
                      priority,
                      std::forward<std::function<void()>>(on_start),
                      std::forward<std::function<void()>>(on_stop),
                      std::forward<std::function<void()>>(periodic_routine),
                      period,
                      start_suspended} {}
  periodic_task(const char *name, UBaseType_t priority,
                task_routine_t &&on_start, task_routine_t &&on_stop,
                task_routine_t &&periodic_routine, bool start_suspended = true)
      : periodic_task{name,
                      priority,
                      std::forward<task_routine_t>(on_start),
                      std::forward<task_routine_t>(on_stop),
                      std::forward<task_routine_t>(periodic_routine),
                      std::chrono::milliseconds{0},
                      start_suspended} {}
  periodic_task(const std::string &name, UBaseType_t priority,
                task_routine_t &&on_start, task_routine_t &&on_stop,
                task_routine_t &&periodic_routine, bool start_suspended = true)
      : periodic_task{name.c_str(),
                      priority,
                      std::forward<std::function<void()>>(on_start),
                      std::forward<std::function<void()>>(on_stop),
                      std::forward<std::function<void()>>(periodic_routine),
                      start_suspended} {}
  periodic_task(const periodic_task &) = delete;
  periodic_task(periodic_task &&other) {
    m_period = other.m_period;
    m_on_start = std::move(other.m_on_start);
    m_on_stop = std::move(other.m_on_stop);
    m_periodic_routine = std::move(other.m_periodic_routine);
    m_task = std::move(other.m_task);
  }
  ~periodic_task(void) {
#if INCLUDE_xTaskAbortDelay
    m_task.abort_delay();
#endif
  }

  periodic_task &operator=(const periodic_task &) = delete;
  periodic_task &operator=(periodic_task &&other) {
    if (this != &other) {
      m_period = other.m_period;
      m_on_start = std::move(other.m_on_start);
      m_on_stop = std::move(other.m_on_stop);
      m_periodic_routine = std::move(other.m_periodic_routine);
      m_task = std::move(other.m_task);
    }
    return *this;
  }

  TaskHandle_t handle(void) const { return m_task.handle(); }
#if INCLUDE_vTaskSuspend
  void suspend(void) { m_task.suspend(); }
  void resume(void) { m_task.resume(); }
  BaseType_t resume_isr(void) { return m_task.resume_isr(); }
#endif
  bool is_running(void) const {
    switch (m_task.state()) {
    case eRunning:
    case eReady:
    case eBlocked:
    case eSuspended:
      return true;
    default:
      return false;
    }
  }
  void terminate(void) { m_task.terminate(); }
#if INCLUDE_xTaskAbortDelay
  BaseType_t abort_delay(void) { return m_task.abort_delay(); }
#endif
#if INCLUDE_uxTaskPriorityGet && configUSE_MUTEXES
  UBaseType_t priority(void) const { return m_task.priority(); }
  UBaseType_t priority_isr(void) const { return m_task.priority_isr(); }
#endif
#if INCLUDE_vTaskPrioritySet
  periodic_task &priority(UBaseType_t priority) {
    m_task.priority(priority);
    return *this;
  }
#endif
#if configUSE_TRACE_FACILITY
  TaskStatus_t status(BaseType_t getFreeStackSpace = pdFALSE,
                      eTaskState eState = eInvalid) const {
    return m_task.status(getFreeStackSpace, eState);
  }
#endif
#if configUSE_APPLICATION_TASK_TAG
  periodic_task &tag(TaskHookFunction_t tag) {
    m_task.tag(tag);
    return *this;
  }
  TaskHookFunction_t tag(void) const { return m_task.tag(); }
  TaskHookFunction_t tag_isr(void) const { return m_task.tag_isr(); }
#endif
#if INCLUDE_uxTaskGetStackHighWaterMark
  size_t stack_high_water_mark(void) const {
    return m_task.stack_high_water_mark();
  }
#endif
#if INCLUDE_uxTaskGetStackHighWaterMark2
  size_t stack_high_water_mark2(void) const {
    return m_task.stack_high_water_mark2();
  }
#endif
#if INCLUDE_eTaskGetState
  eTaskState state(void) const { return m_task.state(); }
#endif
  const char *name(void) const { return m_task.name(); }
// Task notification API
#if configUSE_TASK_NOTIFICATIONS
  BaseType_t notify_give(void) { return m_task.notify_give(); }
  uint32_t notify_take(BaseType_t clearCountOnExit = pdTRUE,
                       TickType_t ticksToWait = portMAX_DELAY) {
    return m_task.notify_take(clearCountOnExit, ticksToWait);
  }
  template <typename Rep, typename Period>
  uint32_t notify_take(BaseType_t clearCountOnExit,
                       std::chrono::duration<Rep, Period> duration) {
    return notify_take(
        clearCountOnExit,
        std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            .count());
  }
  BaseType_t notify(const uint32_t val, eNotifyAction action) {
    return m_task.notify(val, action);
  }
  BaseType_t notfy_and_query(const uint32_t val, eNotifyAction action,
                             uint32_t &prev_value) {
    return m_task.notify_and_query(val, action, prev_value);
  }
  BaseType_t notify_isr(const uint32_t val, eNotifyAction action,
                        BaseType_t &higherPriorityTaskWoken = nullptr) {
    return m_task.notify_isr(val, action, higherPriorityTaskWoken);
  }
  BaseType_t
  notify_and_query_isr(const uint32_t val, eNotifyAction action,
                       uint32_t &prev_value,
                       BaseType_t &higherPriorityTaskWoken = nullptr) {
    return m_task.notify_and_query_isr(val, action, prev_value,
                                       higherPriorityTaskWoken);
  }
  BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
                         uint32_t ulBitsToClearOnExit,
                         uint32_t &notification_value,
                         TickType_t xTicksToWait) {
    return m_task.notify_wait(ulBitsToClearOnEntry, ulBitsToClearOnExit,
                              notification_value, xTicksToWait);
  }
  template <typename Rep, typename Period>
  BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
                         uint32_t ulBitsToClearOnExit,
                         uint32_t &notification_value,
                         std::chrono::duration<Rep, Period> duration) {
    return notify_wait(
        ulBitsToClearOnEntry, ulBitsToClearOnExit, notification_value,
        std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            .count());
  }
  BaseType_t notify_state_clear(void) { return m_task.notify_state_clear(); }
  uint32_t notify_value_clear(uint32_t ulBitsToClear) {
    return m_task.notify_value_clear(ulBitsToClear);
  }
#endif
};

// TODO: add less than ms delays

void delay(TickType_t ticks);

template <typename Rep, typename Period>
void delay(std::chrono::duration<Rep, Period> duration) {
  delay(
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

template <typename Rep, typename Period>
void sleep_for(std::chrono::duration<Rep, Period> duration) {
  delay(duration);
}

void delay_until(TickType_t &previousWakeTime, TickType_t period);

template <typename Rep, typename Period>
void delay_until(TickType_t &previousWakeTime,
                 std::chrono::duration<Rep, Period> period) {
  delay_until(
      previousWakeTime,
      std::chrono::duration_cast<std::chrono::milliseconds>(period).count());
}

void delay_until(const std::chrono::system_clock::time_point &wakeTime);
void delay_until(const std::chrono::steady_clock::time_point &wakeTime);

// task utilities:

#if configUSE_TRACE_FACILITY
template <size_t status_array_capacity> class task_system_status {
  TaskStatus_t m_status_array[status_array_capacity];
  UBaseType_t m_task_count;
  long m_total_run_time;

public:
  task_system_status(void) : m_task_count{0}, m_total_run_time{0} {
    m_task_count = uxTaskGetSystemState(m_status_array, status_array_capacity,
                                        &m_total_run_time);
  }

  UBaseType_t count(void) const { return m_task_count; }
  std::chrono::milliseconds total_run_time(void) const {
    return std::chrono::milliseconds{m_total_run_time};
  }
  const TaskStatus_t *begin(void) const { return m_status_array; }
  const TaskStatus_t *end(void) const { return m_status_array + m_task_count; }
};
#endif
#if INCLUDE_xTaskGetCurrentTaskHandle
TaskHandle_t current_task_handle(void);
#endif
#if INCLUDE_xTaskGetIdleTaskHandle
TaskHandle_t idle_task_handle(void);
#endif

TickType_t tick_count(void);
TickType_t tick_count_isr(void);

std::chrono::milliseconds time_since_scheduler_started(void);
std::chrono::milliseconds time_since_scheduler_started_isr(void);

#if INCLUDE_xTaskGetSchedulerState || configUSE_TIMERS
BaseType_t get_scheduler_state(void);
#endif
UBaseType_t task_count(void);

// RTOS Kernel Control:

void yield(void);

class critical_section {
public:
  critical_section(void) { taskENTER_CRITICAL(); }
  ~critical_section(void) { taskEXIT_CRITICAL(); }
};

class critical_section_isr {
  UBaseType_t m_saved_interrupt_status;

public:
  critical_section_isr(void)
      : m_saved_interrupt_status{taskENTER_CRITICAL_FROM_ISR()} {}
  ~critical_section_isr(void) {
    taskEXIT_CRITICAL_FROM_ISR(m_saved_interrupt_status);
  }
};

class interrupt_barrier {
public:
  interrupt_barrier(void) { taskDISABLE_INTERRUPTS(); }
  ~interrupt_barrier(void) { taskENABLE_INTERRUPTS(); }
};

class scheduler_barrier {
public:
  scheduler_barrier(void) { vTaskSuspendAll(); }
  ~scheduler_barrier(void) { xTaskResumeAll(); }
};

} // namespace freertos
