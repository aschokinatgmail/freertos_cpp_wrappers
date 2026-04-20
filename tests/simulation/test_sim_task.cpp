#include <freertos.hpp>
#include <gtest/gtest.h>

extern "C" {
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxIdleTaskStackBuffer,
                                     configSTACK_DEPTH_TYPE *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxIdleTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
}

struct TaskResults {
  volatile bool notify_give_take;
  volatile bool priority_get;
  volatile bool all_done;
};

static TaskResults s_task{};
static TaskHandle_t s_orch_handle = nullptr;

void notifier_task(void *param) {
  vTaskDelay(pdMS_TO_TICKS(50));
  xTaskNotifyGive(s_orch_handle);
  vTaskDelete(nullptr);
}

void task_orchestrator(void *param) {
  auto *r = static_cast<TaskResults *>(param);

  r->notify_give_take = false;
  r->priority_get = false;

  {
    uint32_t val = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500));
    r->notify_give_take = (val > 0);
  }

  {
    UBaseType_t prio = uxTaskPriorityGet(xTaskGetCurrentTaskHandle());
    r->priority_get = (prio >= 1);
  }

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimTask, AllTaskTests) {
  memset(&s_task, 0, sizeof(s_task));

  s_orch_handle = nullptr;
  xTaskCreate(task_orchestrator, "taskorch", 2048, &s_task, 1, &s_orch_handle);
  xTaskCreate(notifier_task, "notif", 512, nullptr, 2, nullptr);

  vTaskStartScheduler();

  EXPECT_TRUE(s_task.notify_give_take) << "Task notify give/take";
  EXPECT_TRUE(s_task.priority_get) << "Task priority get";
  EXPECT_TRUE(s_task.all_done) << "All tests completed";
}