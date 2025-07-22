/**********************************************************************************
@file freertos_task.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS task routines wrapper
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
#include <array>
#include <cassert>
#include <chrono>
#include <functional>
#include <semphr.h>
#include <string>
#include <task.h>

namespace freertos {

using namespace std::chrono_literals;

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief An allocator for the task that uses a static memory allocation.
 *
 */
template <size_t StackSize> class static_task_allocator {
  std::array<StackType_t, StackSize / sizeof(StackType_t)> m_stackBuffer;
  StaticTask_t m_taskBuffer{};

public:
  static_task_allocator() = default;
  ~static_task_allocator() = default;
  static_task_allocator(const static_task_allocator &) = delete;
  static_task_allocator(static_task_allocator &&) = default;

  static_task_allocator &operator=(const static_task_allocator &) = delete;
  static_task_allocator &operator=(static_task_allocator &&) = delete;

  TaskHandle_t create(TaskFunction_t taskFunction, const char *name,
                      UBaseType_t priority, void *context) {
    return xTaskCreateStatic(taskFunction, name,
                             StackSize / sizeof(StackType_t), context, priority,
                             m_stackBuffer.data(), &m_taskBuffer);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief An allocator for the task that uses a dynamic memory allocation.
 *
 */
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

/**
 * @brief Callback type for the task routine function
 *
 */
using task_routine_t = std::function<void(void)>;

/**
 * @brief A modern C++ wrapper for FreeRTOS tasks with RAII semantics.
 *
 * This class provides a type-safe, exception-safe wrapper around FreeRTOS
 * tasks. It automatically manages task lifecycle and supports both static and
 * dynamic memory allocation strategies.
 *
 * @tparam TaskAllocator Type of the task allocator (static or dynamic)
 *
 * ## Features:
 * - RAII automatic resource management
 * - Move semantics support
 * - Type-safe task function binding
 * - Configurable stack size at compile time
 * - Support for suspended task creation
 * - std::chrono timeout support
 *
 * ## Usage Examples:
 *
 * ### Basic Task Creation:
 * ```cpp
 * // Create a simple task with dynamic allocation
 * freertos::task<1024> worker_task("WorkerTask", 3, []() {
 *     while (true) {
 *         // Task work here
 *         printf("Task running\\n");
 *         vTaskDelay(pdMS_TO_TICKS(1000));
 *     }
 * });
 * ```
 *
 * ### Static Allocation:
 * ```cpp
 * // Create task with static memory allocation
 * freertos::task<1024, freertos::static_task_allocator<1024>> static_task(
 *     "StaticTask", 2, []() {
 *         // Task implementation
 *     }
 * );
 * ```
 *
 * ### Task with Capture:
 * ```cpp
 * int shared_data = 42;
 * freertos::task<512> capture_task("CaptureTask", 1, [&shared_data]() {
 *     printf("Shared data: %d\\n", shared_data);
 * });
 * ```
 *
 * ### Task Control:
 * ```cpp
 * freertos::task<1024> controlled_task("ControlTask", 3, task_function);
 *
 * // Suspend and resume
 * controlled_task.suspend();
 * vTaskDelay(pdMS_TO_TICKS(1000));
 * controlled_task.resume();
 *
 * // Check task state
 * if (controlled_task.is_suspended()) {
 *     printf("Task is suspended\\n");
 * }
 * ```
 */
template <typename TaskAllocator> class task {
  TaskAllocator m_allocator;
  task_routine_t m_taskRoutine;
#if INCLUDE_vTaskSuspend
  uint8_t m_start_suspended : 1;
#endif
  TaskHandle_t m_hTask;

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
#if INCLUDE_vTaskSuspend
  /**
   * @brief Construct a new task object
   *
   * @param name name of the task
   * @param priority priority of the task
   * @param task_routine task routine function
   * @param start_suspended start the task in the suspended state
   */
  task(const char *name, UBaseType_t priority, task_routine_t &&task_routine,
       bool start_suspended = true)
      : m_allocator{}, m_taskRoutine{std::move(task_routine)},
        m_start_suspended{start_suspended},
        m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
  /**
   * @brief Construct a new task object
   *
   * @param name name of the task
   * @param priority priority of the task
   * @param task_routine task routine function
   * @param start_suspended start the task in the suspended state
   */
  task(const std::string &name, UBaseType_t priority,
       task_routine_t &&task_routine, bool start_suspended = true)
      : task{name.c_str(), priority, std::move(task_routine), start_suspended} {
  }
#else
  /**
   * @brief Construct a new task object
   *
   * @param name  name of the task
   * @param priority  priority of the task
   * @param task_routine  task routine function
   */
  task(const char *name, UBaseType_t priority, task_routine_t &&task_routine)
      : m_allocator{}, m_taskRoutine{task_routine},
        m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
  /**
   * @brief Construct a new task object
   *
   * @param name  name of the task
   * @param priority  priority of the task
   * @param task_routine  task routine function
   */
  task(const std::string &name, UBaseType_t priority,
       task_routine_t &&task_routine)
      : task{name.c_str(), priority,
             std::forward<std::function<void()>>(task_routine)} {}
#endif
  task(const task &) = delete;
  task(task &&other) noexcept
      : m_allocator(std::move(other.m_allocator)), m_hTask(other.m_hTask),
        m_taskRoutine(std::move(other.m_taskRoutine))
#if INCLUDE_vTaskSuspend
        ,
        m_start_suspended(other.m_start_suspended)
#endif
  {
    other.m_hTask = nullptr; // Transfer ownership - moved-from object should
                             // not delete the task
  }
  /**
   * @brief Destruct the task object and delete the task instance if it was
   * created.
   *
   */
  ~task(void) {
#if INCLUDE_vTaskDelete
    if (m_hTask) {
      vTaskDelete(m_hTask);
    }
#endif
  }

  task &operator=(const task &) = delete;
  task &operator=(task &&other) = delete;

  /**
   * @brief Return the handle of the task.
   *
   * @return TaskHandle_t task handle
   */
  TaskHandle_t handle(void) const { return m_hTask; }
#if INCLUDE_vTaskSuspend
  /**
   * @brief Suspend the task.
   *
   */
  void suspend(void) { vTaskSuspend(m_hTask); }
  /**
   * @brief Resume the task.
   *
   */
  void resume(void) { vTaskResume(m_hTask); }
  /**
   * @brief Resume the task from an ISR.
   *
   * @return BaseType_t pdTRUE if the task was resumed, pdFALSE otherwise
   */
  BaseType_t resume_isr(void) { return xTaskResumeFromISR(m_hTask); }
#endif
  /**
   * @brief Check if the task is running.
   *
   * @return bool true if the task is running, false otherwise
   */
  bool is_running(void) const {
    switch (state()) {
    case eRunning:
    case eReady:
    case eBlocked:
    case eSuspended:
      return true;
    default:
      return false;
    }
  }
  /**
   * @brief Terminates the task.
   *
   */
  void terminate(void) { vTaskDelete(m_hTask); }
#if INCLUDE_xTaskAbortDelay
  /**
   * @brief Abort the delay of the task.
   *
   * @return BaseType_t pdTRUE if the delay was aborted, pdFALSE otherwise
   */
  BaseType_t abort_delay(void) {
    return m_hTask ? xTaskAbortDelay(m_hTask) : pdFALSE;
  }
#endif
#if INCLUDE_uxTaskPriorityGet && configUSE_MUTEXES
  /**
   * @brief Get the priority of the task.
   *
   * @return UBaseType_t task priority
   */
  UBaseType_t priority(void) const { return uxTaskPriorityGet(m_hTask); }
  /**
   * @brief Get the priority of the task from an ISR.
   *
   * @return UBaseType_t task priority
   */
  UBaseType_t priority_isr(void) const {
    return uxTaskPriorityGetFromISR(m_hTask);
  }
#endif
#if INCLUDE_vTaskPrioritySet
  /**
   * @brief Set the priority of the task.
   *
   * @param priority task priority
   * @return task& reference to the task object
   */
  task &priority(UBaseType_t priority) {
    vTaskPrioritySet(m_hTask, priority);
    return *this;
  }
#endif
#if configUSE_TRACE_FACILITY
  /**
   * @brief Get the status of the task.
   *
   * @param getFreeStackSpace get the free stack space
   * @param eState task state
   * @return TaskStatus_t task status
   */
  TaskStatus_t status(BaseType_t getFreeStackSpace = pdFALSE,
                      eTaskState eState = eInvalid) const {
    TaskStatus_t status;
    vTaskGetInfo(m_hTask, &status, getFreeStackSpace, eState);
    return status;
  }
#endif
#if configUSE_APPLICATION_TASK_TAG
  /**
   * @brief Set the tag of the task.
   *
   * @param tag task tag
   * @return task& reference to the task object
   */
  task &tag(TaskHookFunction_t tag) {
    vTaskSetApplicationTaskTag(m_hTask, tag);
    return *this;
  }
  /**
   * @brief Get the tag of the task.
   *
   * @return TaskHookFunction_t task tag
   */
  TaskHookFunction_t tag(void) const {
    return ulTaskGetApplicationTaskTag(m_hTask);
  }
  /**
   * @brief Get the tag of the task from an ISR.
   *
   * @return TaskHookFunction_t task tag
   */
  TaskHookFunction_t tag_isr(void) const {
    return ulTaskGetApplicationTaskTagFromISR(m_hTask);
  }
#endif
#if INCLUDE_uxTaskGetStackHighWaterMark
  /**
   * @brief Get the high water mark of the task stack.
   *
   * @return size_t high water mark
   */
  size_t stack_high_water_mark(void) const {
    return uxTaskGetStackHighWaterMark(m_hTask);
  }
#endif
#if INCLUDE_uxTaskGetStackHighWaterMark2
  /**
   * @brief Get the high water mark of the task stack.
   *
   * @return size_t high water mark
   */
  size_t stack_high_water_mark2(void) const {
    return uxTaskGetStackHighWaterMark2(m_hTask);
  }
#endif
#if INCLUDE_eTaskGetState
  /**
   * @brief Get the state of the task.
   *
   * @return eTaskState task state
   */
  eTaskState state(void) const { return eTaskGetState(m_hTask); }
#endif
  /**
   * @brief Get the name of the task.
   *
   * @return const char* task name
   */
  const char *name(void) const { return pcTaskGetName(m_hTask); }
// Task notification API
#if configUSE_TASK_NOTIFICATIONS
  /**
   * @brief Give a notification to the task.
   *
   * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_give(void) { return xTaskNotifyGive(m_hTask); }
  /**
   * @brief Take a notification from the task.
   *
   * @param clearCountOnExit clear the count on exit
   * @param ticksToWait ticks to wait
   * @return uint32_t notification value
   */
  uint32_t notify_take(BaseType_t clearCountOnExit = pdTRUE,
                       TickType_t ticksToWait = portMAX_DELAY) {
    return ulTaskNotifyTake(clearCountOnExit, ticksToWait);
  }
  /**
   * @brief Take a notification from the task.
   *
   * @param clearCountOnExit clear the count on exit
   * @param duration duration to wait
   * @return uint32_t notification value
   */
  template <typename Rep, typename Period>
  uint32_t notify_take(BaseType_t clearCountOnExit,
                       std::chrono::duration<Rep, Period> duration) {
    return notify_take(
        clearCountOnExit,
        std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            .count());
  }
  /**
   * @brief Notify the task.
   *
   * @param val notification value
   * @param action notification action
   * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify(const uint32_t val, eNotifyAction action) {
    return xTaskNotify(m_hTask, val, action);
  }
  /**
   * @brief Notify the task and query the previous value.
   *
   * @param val notification value
   * @param action notification action
   * @param prev_value previous value
   * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_and_query(const uint32_t val, eNotifyAction action,
                              uint32_t &prev_value) {
    return xTaskNotifyAndQuery(m_hTask, val, action, &prev_value);
  }
  /**
   * @brief Notify the task from an ISR.
   *
   * @param val notification value
   * @param action notification action
   * @param higherPriorityTaskWoken higher priority task woken
   * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_isr(const uint32_t val, eNotifyAction action,
                        BaseType_t *higherPriorityTaskWoken = nullptr) {
    return xTaskNotifyFromISR(m_hTask, val, action, higherPriorityTaskWoken);
  }
  /**
   * @brief Notify the task from an ISR and query the previous value.
   *
   * @param val notification value
   * @param action notification action
   * @param prev_value previous value
   * @param higherPriorityTaskWoken higher priority task woken
   * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t
  notify_and_query_isr(const uint32_t val, eNotifyAction action,
                       uint32_t &prev_value,
                       BaseType_t *higherPriorityTaskWoken = nullptr) {
    return xTaskNotifyAndQueryFromISR(m_hTask, val, action, &prev_value,
                                      higherPriorityTaskWoken);
  }
  /**
   * @brief Wait for the notification.
   *
   * @param ulBitsToClearOnEntry bits to clear on entry
   * @param ulBitsToClearOnExit bits to clear on exit
   * @param notification_value notification value
   * @param xTicksToWait ticks to wait
   * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
                         uint32_t ulBitsToClearOnExit,
                         uint32_t &notification_value,
                         TickType_t xTicksToWait) {
    return xTaskNotifyWait(ulBitsToClearOnEntry, ulBitsToClearOnExit,
                           &notification_value, xTicksToWait);
  }
  /**
   * @brief Wait for the notification.
   *
   * @param ulBitsToClearOnEntry bits to clear on entry
   * @param ulBitsToClearOnExit bits to clear on exit
   * @param notification_value notification value
   * @param duration duration to wait
   * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
   */
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
  /**
   * @brief Clear the notification state.
   *
   * @return BaseType_t pdTRUE if the notification state was cleared, pdFALSE
   * otherwise
   */
  BaseType_t notify_state_clear(void) { return xTaskNotifyStateClear(m_hTask); }
  /**
   * @brief Clear the notification value.
   *
   * @param ulBitsToClear bits to clear
   * @return uint32_t bits cleared
   */
  uint32_t notify_value_clear(uint32_t ulBitsToClear) {
    return ulTaskNotifyValueClear(m_hTask, ulBitsToClear);
  }
#endif
};

/**
 * @brief A wrapper for the FreeRTOS periodic task.
 *
 * @tparam TaskAllocator type of the task allocator
 */
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
        uint32_t notification_value = 0;
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
  /**
   * @brief Construct a new periodic task object
   *
   * @tparam Rep duration representation
   * @tparam Period duration period
   * @param name name of the task
   * @param priority priority of the task
   * @param on_start task routine function to be called on start
   * @param on_stop task routine function to be called on stop
   * @param periodic_routine periodic task routine function
   * @param period period of the periodic task
   * @param start_suspended start the task in the suspended state
   */
  template <typename Rep, typename Period>
  periodic_task(const char *name, UBaseType_t priority,
                task_routine_t &&on_start, task_routine_t &&on_stop,
                task_routine_t &&periodic_routine,
                const std::chrono::duration<Rep, Period> &period,
                bool start_suspended = true)
      : m_period{std::chrono::duration_cast<std::chrono::milliseconds>(period)},
        m_on_start{std::move(on_start)}, m_on_stop{std::move(on_stop)},
        m_periodic_routine{std::move(periodic_routine)},
        m_task{name, priority, [this]() { run(); }, start_suspended} {}
  /**
   * @brief Construct a new periodic task object
   *
   * @tparam Rep  duration representation
   * @tparam Period  duration period
   * @param name  name of the task
   * @param priority  priority of the task
   * @param on_start  task routine function to be called on start
   * @param on_stop   task routine function to be called on stop
   * @param periodic_routine  periodic task routine function
   * @param period  period of the periodic task
   * @param start_suspended  start the task in the suspended state
   */
  template <typename Rep, typename Period>
  periodic_task(const std::string &name, UBaseType_t priority,
                task_routine_t &&on_start, task_routine_t &&on_stop,
                task_routine_t &&periodic_routine,
                const std::chrono::duration<Rep, Period> &period,
                bool start_suspended = true)
      : periodic_task{name.c_str(),
                      priority,
                      std::move(on_start),
                      std::move(on_stop),
                      std::move(periodic_routine),
                      period,
                      start_suspended} {}
  /**
   * @brief Construct a new periodic task object
   *
   * @param name  name of the task
   * @param priority  priority of the task
   * @param on_start  task routine function to be called on start
   * @param on_stop  task routine function to be called on stop
   * @param periodic_routine  periodic task routine function
   * @param start_suspended  start the task in the suspended state
   */
  periodic_task(const char *name, UBaseType_t priority,
                task_routine_t &&on_start, task_routine_t &&on_stop,
                task_routine_t &&periodic_routine, bool start_suspended = true)
      : periodic_task{name,
                      priority,
                      std::move(on_start),
                      std::move(on_stop),
                      std::move(periodic_routine),
                      std::chrono::milliseconds{0},
                      start_suspended} {}
  /**
   * @brief Construct a new periodic task object
   *
   * @param name  name of the task
   * @param priority  priority of the task
   * @param on_start  task routine function to be called on start
   * @param on_stop  task routine function to be called on stop
   * @param periodic_routine  periodic task routine function
   * @param start_suspended  start the task in the suspended state
   */
  periodic_task(const std::string &name, UBaseType_t priority,
                task_routine_t &&on_start, task_routine_t &&on_stop,
                task_routine_t &&periodic_routine, bool start_suspended = true)
      : periodic_task{name.c_str(),
                      priority,
                      std::move(on_start),
                      std::move(on_stop),
                      std::move(periodic_routine),
                      start_suspended} {}
  periodic_task(const periodic_task &) = delete;
  periodic_task(periodic_task &&other) noexcept
      : m_period(other.m_period), m_on_start(std::move(other.m_on_start)),
        m_on_stop(std::move(other.m_on_stop)),
        m_periodic_routine(std::move(other.m_periodic_routine)),
        m_task(std::move(other.m_task)) {
    // m_task move constructor will handle ownership transfer
  }
  /**
   * @brief Destruct the periodic task object and delete the task instance if it
   * was created.
   *
   */
  ~periodic_task(void) {
#if INCLUDE_xTaskAbortDelay
    m_task.abort_delay();
#endif
  }

  periodic_task &operator=(const periodic_task &) = delete;
  periodic_task &operator=(periodic_task &&other) = delete;

  /**
   * @brief Return the handle of the task.
   *
   * @return TaskHandle_t task handle
   */
  TaskHandle_t handle(void) const { return m_task.handle(); }
#if INCLUDE_vTaskSuspend
  /**
   * @brief Suspend the task.
   *
   */
  void suspend(void) { m_task.suspend(); }
  /**
   * @brief Resume the task.
   *
   */
  void resume(void) { m_task.resume(); }
  /**
   * @brief Resume the task from an ISR.
   *
   * @return BaseType_t pdTRUE if the task was resumed, pdFALSE otherwise
   */
  BaseType_t resume_isr(void) { return m_task.resume_isr(); }
#endif
  /**
   * @brief Check if the task is running.
   *
   * @return bool true if the task is running, false otherwise
   */
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
  /**
   * @brief Terminate the task.
   *
   */
  void terminate(void) { m_task.terminate(); }
#if INCLUDE_xTaskAbortDelay
  /**
   * @brief Abort the delay of the task.
   *
   * @return BaseType_t pdTRUE if the delay was aborted, pdFALSE otherwise
   */
  BaseType_t abort_delay(void) { return m_task.abort_delay(); }
#endif
#if INCLUDE_uxTaskPriorityGet && configUSE_MUTEXES
  /**
   * @brief Return the priority of the task.
   *
   * @return UBaseType_t  task priority
   */
  UBaseType_t priority(void) const { return m_task.priority(); }
  /**
   * @brief Return the priority of the task from an ISR.
   *
   * @return UBaseType_t  task priority
   */
  UBaseType_t priority_isr(void) const { return m_task.priority_isr(); }
#endif
#if INCLUDE_vTaskPrioritySet
  /**
   * @brief Set the priority of the task.
   *
   * @param priority  task priority
   * @return periodic_task& reference to the periodic task object
   */
  periodic_task &priority(UBaseType_t priority) {
    m_task.priority(priority);
    return *this;
  }
#endif
#if configUSE_TRACE_FACILITY
  /**
   * @brief Get the status of the task.
   *
   * @param getFreeStackSpace  get the free stack space
   * @param eState  task state
   * @return TaskStatus_t  task status
   */
  TaskStatus_t status(BaseType_t getFreeStackSpace = pdFALSE,
                      eTaskState eState = eInvalid) const {
    return m_task.status(getFreeStackSpace, eState);
  }
#endif
#if configUSE_APPLICATION_TASK_TAG
  /**
   * @brief Set the tag of the task.
   *
   * @param tag  task tag
   * @return periodic_task& reference to the periodic task object
   */
  periodic_task &tag(TaskHookFunction_t tag) {
    m_task.tag(tag);
    return *this;
  }
  /**
   * @brief Get the tag of the task.
   *
   * @return TaskHookFunction_t  task tag
   */
  TaskHookFunction_t tag(void) const { return m_task.tag(); }
  /**
   * @brief Get the tag of the task from an ISR.
   *
   * @return TaskHookFunction_t  task tag
   */
  TaskHookFunction_t tag_isr(void) const { return m_task.tag_isr(); }
#endif
#if INCLUDE_uxTaskGetStackHighWaterMark
  /**
   * @brief Get the high water mark of the task stack.
   *
   * @return size_t  high water mark
   */
  size_t stack_high_water_mark(void) const {
    return m_task.stack_high_water_mark();
  }
#endif
#if INCLUDE_uxTaskGetStackHighWaterMark2
  /**
   * @brief Get the high water mark of the task stack.
   *
   * @return size_t  high water mark
   */
  size_t stack_high_water_mark2(void) const {
    return m_task.stack_high_water_mark2();
  }
#endif
#if INCLUDE_eTaskGetState
  /**
   * @brief Get the state of the task.
   *
   * @return eTaskState  task state
   */
  eTaskState state(void) const { return m_task.state(); }
#endif
  /**
   * @brief Get the name of the task.
   *
   * @return const char*  task name
   */
  const char *name(void) const { return m_task.name(); }
// Task notification API
#if configUSE_TASK_NOTIFICATIONS
  /**
   * @brief Give a notification to the task.
   *
   * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_give(void) { return m_task.notify_give(); }
  /**
   * @brief Take a notification from the task.
   *
   * @param clearCountOnExit  clear the count on exit
   * @param ticksToWait  ticks to wait
   * @return uint32_t  notification value
   */
  uint32_t notify_take(BaseType_t clearCountOnExit = pdTRUE,
                       TickType_t ticksToWait = portMAX_DELAY) {
    return m_task.notify_take(clearCountOnExit, ticksToWait);
  }
  /**
   * @brief Take a notification from the task.
   *
   * @param clearCountOnExit  clear the count on exit
   * @param duration  duration to wait
   * @return uint32_t  notification value
   */
  template <typename Rep, typename Period>
  uint32_t notify_take(BaseType_t clearCountOnExit,
                       std::chrono::duration<Rep, Period> duration) {
    return notify_take(
        clearCountOnExit,
        std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            .count());
  }
  /**
   * @brief Notify the task.
   *
   * @param val  notification value
   * @param action  notification action
   * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify(const uint32_t val, eNotifyAction action) {
    return m_task.notify(val, action);
  }
  /**
   * @brief Notify the task and query the previous value.
   *
   * @param val  notification value
   * @param action  notification action
   * @param prev_value  previous value
   * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_and_query(const uint32_t val, eNotifyAction action,
                              uint32_t &prev_value) {
    return m_task.notify_and_query(val, action, prev_value);
  }
  /**
   * @brief Notify the task from an ISR.
   *
   * @param val  notification value
   * @param action  notification action
   * @param higherPriorityTaskWoken  higher priority task woken
   * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_isr(const uint32_t val, eNotifyAction action,
                        BaseType_t &higherPriorityTaskWoken) {
    return m_task.notify_isr(val, action, higherPriorityTaskWoken);
  }
  /**
   * @brief Notify the task from an ISR and query the previous value.
   *
   * @param val  notification value
   * @param action  notification action
   * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_isr(const uint32_t val, eNotifyAction action) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    return m_task.notify_isr(val, action, higherPriorityTaskWoken);
  }
  /**
   * @brief Notify the task from an ISR and query the previous value.
   *
   * @param val  notification value
   * @param action  notification action
   * @param prev_value  previous value
   * @param higherPriorityTaskWoken  higher priority task woken
   * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_and_query_isr(const uint32_t val, eNotifyAction action,
                                  uint32_t &prev_value,
                                  BaseType_t &higherPriorityTaskWoken) {
    return m_task.notify_and_query_isr(val, action, prev_value,
                                       higherPriorityTaskWoken);
  }
  /**
   * @brief Notify the task from an ISR and query the previous value.
   *
   * @param val  notification value
   * @param action  notification action
   * @param prev_value  previous value
   * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_and_query_isr(const uint32_t val, eNotifyAction action,
                                  uint32_t &prev_value) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    return m_task.notify_and_query_isr(val, action, prev_value,
                                       higherPriorityTaskWoken);
  }
  /**
   * @brief Wait for the notification.
   *
   * @param ulBitsToClearOnEntry  bits to clear on entry
   * @param ulBitsToClearOnExit  bits to clear on exit
   * @param notification_value  notification value
   * @param xTicksToWait  ticks to wait
   * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
   */
  BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
                         uint32_t ulBitsToClearOnExit,
                         uint32_t &notification_value,
                         TickType_t xTicksToWait) {
    return m_task.notify_wait(ulBitsToClearOnEntry, ulBitsToClearOnExit,
                              notification_value, xTicksToWait);
  }
  /**
   * @brief Wait for the notification.
   *
   * @param ulBitsToClearOnEntry  bits to clear on entry
   * @param ulBitsToClearOnExit  bits to clear on exit
   * @param notification_value  notification value
   * @param duration  duration to wait
   * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
   */
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
  /**
   * @brief Clear the notification state.
   *
   * @return BaseType_t  pdTRUE if the notification state was cleared, pdFALSE
   * otherwise
   */
  BaseType_t notify_state_clear(void) { return m_task.notify_state_clear(); }
  /**
   * @brief Clear the notification value.
   *
   * @param ulBitsToClear  bits to clear
   * @return uint32_t  bits cleared
   */
  uint32_t notify_value_clear(uint32_t ulBitsToClear) {
    return m_task.notify_value_clear(ulBitsToClear);
  }
#endif
};

// TODO(maintainer): add less than ms delays

/**
 * @brief Delay the task for the specified number of ticks.
 *
 * @param ticks number of ticks to delay
 */
void delay(TickType_t ticks);

/**
 * @brief Delay the task for the specified duration.
 *
 * @tparam Rep duration representation
 * @tparam Period duration period
 * @param duration duration to delay
 */
template <typename Rep, typename Period>
void delay(std::chrono::duration<Rep, Period> duration) {
  delay(
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

/**
 * @brief Sleep the task for the specified duration.
 *
 * @tparam Rep duration representation
 * @tparam Period duration period
 * @param duration duration to sleep
 */
template <typename Rep, typename Period>
void sleep_for(std::chrono::duration<Rep, Period> duration) {
  delay(duration);
}

/**
 * @brief Sleep the task until the specified time point.
 *
 * @tparam Clock clock type
 * @tparam Duration duration type
 * @param timePoint time point to sleep until
 */
void delay_until(TickType_t &previousWakeTime, TickType_t period);

template <typename Rep, typename Period>
void delay_until(TickType_t &previousWakeTime,
                 std::chrono::duration<Rep, Period> period) {
  delay_until(
      previousWakeTime,
      std::chrono::duration_cast<std::chrono::milliseconds>(period).count());
}

/**
 * @brief Sleep the task until the specified time point.
 *
 * @tparam Clock clock type
 * @tparam Duration duration type
 * @param timePoint time point to sleep until
 */
void delay_until(const std::chrono::system_clock::time_point &wakeTime);
/**
 * @brief Sleep the task until the specified time point.
 *
 * @tparam Clock clock type
 * @tparam Duration duration type
 * @param timePoint time point to sleep until
 */
void delay_until(const std::chrono::steady_clock::time_point &wakeTime);

// task utilities:

#if configUSE_TRACE_FACILITY
/**
 * @brief Task system status.
 *
 * @tparam status_array_capacity
 */
template <size_t status_array_capacity> class task_system_status {
  std::array<TaskStatus_t, status_array_capacity> m_status_array{};
  UBaseType_t m_task_count{0};
  uint32_t m_total_run_time{0};

public:
  /**
   * @brief Construct a new task system status object
   *
   */
  task_system_status(void) {
    m_task_count = uxTaskGetSystemState(
        m_status_array.data(), status_array_capacity, &m_total_run_time);
  }

  /**
   * @brief Get the number of tasks.
   *
   * @return UBaseType_t number of tasks
   */
  UBaseType_t count(void) const { return m_task_count; }
  std::chrono::milliseconds total_run_time(void) const {
    return std::chrono::milliseconds{m_total_run_time};
  }
  /**
   * @brief Return the begin iterator of the task status array.
   *
   * @return const TaskStatus_t* begin iterator
   */
  const TaskStatus_t *begin(void) const { return m_status_array.data(); }
  /**
   * @brief Return the end iterator of the task status array.
   *
   * @return const TaskStatus_t* end iterator
   */
  const TaskStatus_t *end(void) const {
    return m_status_array.data() + m_task_count;
  }
};
#endif
#if INCLUDE_xTaskGetCurrentTaskHandle
/**
 * @brief Get the handle of the current task.
 *
 * @return TaskHandle_t task handle
 */
TaskHandle_t current_task_handle(void);
#endif
#if INCLUDE_xTaskGetIdleTaskHandle
/**
 * @brief Get the handle of the idle task.
 *
 * @return TaskHandle_t task handle
 */
TaskHandle_t idle_task_handle(void);
#endif

/**
 * @brief Get number of ticks since the scheduler started.
 *
 * @return TickType_t number of ticks
 */
TickType_t tick_count(void);
/**
 * @brief Get number of ticks since the scheduler started from an ISR.
 *
 * @return TickType_t number of ticks
 */
TickType_t tick_count_isr(void);

/**
 * @brief Get the time since the scheduler started.
 *
 * @return std::chrono::milliseconds time since the scheduler started
 */
std::chrono::milliseconds time_since_scheduler_started(void);
/**
 * @brief Get the time since the scheduler started from an ISR.
 *
 * @return std::chrono::milliseconds time since the scheduler started
 */
std::chrono::milliseconds time_since_scheduler_started_isr(void);

#if INCLUDE_xTaskGetSchedulerState || configUSE_TIMERS
/**
 * @brief Get the state of the scheduler.
 *
 * @return UBaseType_t scheduler state
 */
BaseType_t get_scheduler_state(void);
#endif
/**
 * @brief Get the number of tasks in the system.
 *
 * @return UBaseType_t number of tasks
 */
UBaseType_t task_count(void);

// RTOS Kernel Control:

/**
 * @brief Start the scheduler.
 *
 */
void yield(void);

/**
 * @brief Critical section guard for the scheduler (RAII). Enter a critical
 * section on construction and exits it on destruction.
 *
 */
class critical_section {
public:
  /**
   * @brief Construct a new critical section object
   *
   */
  critical_section(void) { taskENTER_CRITICAL(); }
  /**
   * @brief Destroy the critical section object
   *
   */
  ~critical_section(void) { taskEXIT_CRITICAL(); }

  // Delete copy and move operations for RAII safety
  critical_section(const critical_section &) = delete;
  critical_section(critical_section &&) = delete;
  critical_section &operator=(const critical_section &) = delete;
  critical_section &operator=(critical_section &&) = delete;
};

/**
 * @brief Critical section guard for the scheduler from an ISR (RAII). Enter a
 * critical section from an ISR on construction and exits it on destruction.
 *
 */
class critical_section_isr {
  UBaseType_t m_saved_interrupt_status{taskENTER_CRITICAL_FROM_ISR()};

public:
  /**
   * @brief Construct a new critical section isr object
   *
   */
  critical_section_isr(void) = default;
  /**
   * @brief Destroy the critical section isr object
   *
   */
  ~critical_section_isr(void) {
    taskEXIT_CRITICAL_FROM_ISR(m_saved_interrupt_status);
  }

  // Delete copy and move operations for RAII safety
  critical_section_isr(const critical_section_isr &) = delete;
  critical_section_isr(critical_section_isr &&) = delete;
  critical_section_isr &operator=(const critical_section_isr &) = delete;
  critical_section_isr &operator=(critical_section_isr &&) = delete;
};

/**
 * @brief Barrier for the interrupts (RAII). Disables interrupts on construction
 * and enables them on destruction.
 *
 */
class interrupt_barrier {
public:
  /**
   * @brief Construct a new interrupt barrier object
   *
   */
  interrupt_barrier(void) { taskDISABLE_INTERRUPTS(); }
  /**
   * @brief Destroy the interrupt barrier object
   *
   */
  ~interrupt_barrier(void) { taskENABLE_INTERRUPTS(); }

  // Delete copy and move operations for RAII safety
  interrupt_barrier(const interrupt_barrier &) = delete;
  interrupt_barrier(interrupt_barrier &&) = delete;
  interrupt_barrier &operator=(const interrupt_barrier &) = delete;
  interrupt_barrier &operator=(interrupt_barrier &&) = delete;
};

/**
 * @brief Barrier for the scheduler (RAII). Suspends the scheduler on
 * construction and resumes it on destruction.
 *
 */
class scheduler_barrier {
public:
  /**
   * @brief Construct a new scheduler barrier object
   *
   */
  scheduler_barrier(void) { vTaskSuspendAll(); }
  /**
   * @brief Destroy the scheduler barrier object
   *
   */
  ~scheduler_barrier(void) { xTaskResumeAll(); }

  // Delete copy and move operations for RAII safety
  scheduler_barrier(const scheduler_barrier &) = delete;
  scheduler_barrier(scheduler_barrier &&) = delete;
  scheduler_barrier &operator=(const scheduler_barrier &) = delete;
  scheduler_barrier &operator=(scheduler_barrier &&) = delete;
};

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief Namespace for static allocation of kernel objects.
 *
 */
namespace sa {
/**
 * @brief Task using static allocation.
 *
 * @tparam StackSize stack size
 */
template <size_t StackSize>
using task = freertos::task<freertos::static_task_allocator<StackSize>>;
/**
 * @brief Periodic task using static allocation.
 *
 * @tparam StackSize stack size
 */
template <size_t StackSize>
using periodic_task =
    freertos::periodic_task<freertos::static_task_allocator<StackSize>>;
} // namespace sa
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief Namespace for dynamic allocation of kernel objects.
 *
 */
namespace da {
/**
 * @brief Task using dynamic allocation.
 *
 * @tparam StackSize stack size
 */
template <size_t StackSize>
using task = freertos::task<freertos::dynamic_task_allocator<StackSize>>;
/**
 * @brief Periodic task using dynamic allocation.
 *
 * @tparam StackSize stack size
 */
template <size_t StackSize>
using periodic_task =
    freertos::periodic_task<freertos::dynamic_task_allocator<StackSize>>;
} // namespace da
#endif

} // namespace freertos
