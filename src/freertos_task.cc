#include "../include/freertos_task.hpp"

/**********************************************************************************
@file freertos_task.cc
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

namespace freertos {

void delay(TickType_t ticks) { vTaskDelay(ticks); }
void delay_until(TickType_t &previousWakeTime, TickType_t period) {
  vTaskDelayUntil(&previousWakeTime, period);
}
void delay_until(const std::chrono::system_clock::time_point &wakeTime) {
  auto now = std::chrono::system_clock::now();
  if (wakeTime > now) {
    delay(wakeTime - now);
  }
}
void delay_until(const std::chrono::steady_clock::time_point &wakeTime) {
  auto now = std::chrono::steady_clock::now();
  if (wakeTime > now) {
    delay(wakeTime - now);
  }
}
#if INCLUDE_xTaskGetCurrentTaskHandle
TaskHandle_t current_task_handle(void) { return xTaskGetCurrentTaskHandle(); }
#endif
#if INCLUDE_xTaskGetIdleTaskHandle
TaskHandle_t idle_task_handle(void) { return xTaskGetIdleTaskHandle(); }
#endif
TickType_t tick_count(void) { return xTaskGetTickCount(); }
TickType_t tick_count_isr(void) { return xTaskGetTickCountFromISR(); }
std::chrono::milliseconds time_since_scheduler_started(void) {
  return std::chrono::milliseconds{tick_count() * portTICK_PERIOD_MS};
}
std::chrono::milliseconds time_since_scheduler_started_isr(void) {
  return std::chrono::milliseconds{tick_count_isr() * portTICK_PERIOD_MS};
}
#if INCLUDE_xTaskGetSchedulerState || configUSE_TIMERS
BaseType_t get_scheduler_state(void) { return xTaskGetSchedulerState(); }
#endif
UBaseType_t task_count(void) { return uxTaskGetNumberOfTasks(); }
void yield(void) { taskYIELD(); }

} // namespace freertos
