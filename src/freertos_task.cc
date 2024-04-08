#include "../include/freertos_task.hpp"

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
