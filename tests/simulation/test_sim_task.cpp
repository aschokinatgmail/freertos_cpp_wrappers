#include <freertos.hpp>
#include <gtest/gtest.h>

#include <atomic>

struct TaskResults {
  std::atomic<bool> notify_give_take;
  std::atomic<bool> priority_get;
  std::atomic<bool> all_done;

  void reset() {
    notify_give_take.store(false);
    priority_get.store(false);
    all_done.store(false);
  }
};

static TaskResults s_task;
static TaskHandle_t s_orch_handle = nullptr;

void notifier_task(void *param) {
  (void)param;
  vTaskDelay(pdMS_TO_TICKS(50));
  xTaskNotifyGive(s_orch_handle);
  vTaskDelete(nullptr);
}

void task_orchestrator(void *param) {
  auto *r = static_cast<TaskResults *>(param);

  r->notify_give_take.store(false);
  r->priority_get.store(false);

  {
    uint32_t val = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500));
    r->notify_give_take.store(val > 0);
  }

  {
    UBaseType_t prio = uxTaskPriorityGet(xTaskGetCurrentTaskHandle());
    r->priority_get.store(prio >= 1);
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimTask, AllTaskTests) {
  s_task.reset();
  s_orch_handle = nullptr;

  xTaskCreate(task_orchestrator, "taskorch", 2048, &s_task, 1, &s_orch_handle);
  xTaskCreate(notifier_task, "notif", 512, nullptr, 2, nullptr);

  vTaskStartScheduler();

  EXPECT_TRUE(s_task.notify_give_take) << "Task notify give/take";
  EXPECT_TRUE(s_task.priority_get) << "Task priority get";
  EXPECT_TRUE(s_task.all_done) << "All tests completed";
}