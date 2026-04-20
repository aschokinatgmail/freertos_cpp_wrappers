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

struct TimerResults {
  volatile bool one_shot_fires;
  volatile bool auto_reload_fires;
  volatile bool stop_prevents_callback;
  volatile bool reset_restarts;
  volatile bool all_done;
};

static TimerResults s_timer{};

void timer_orchestrator(void *param) {
  auto *r = static_cast<TimerResults *>(param);

  r->one_shot_fires = false;
  r->auto_reload_fires = false;
  r->stop_prevents_callback = false;
  r->reset_restarts = false;

  {
    volatile int count = 0;
    freertos::da::timer t("oneshot", pdMS_TO_TICKS(100), pdFALSE,
                           [&count]() { count++; });
    t.start();
    vTaskDelay(pdMS_TO_TICKS(300));
    r->one_shot_fires = (count == 1);
  }

  {
    volatile int count = 0;
    freertos::da::timer t("autoreload", pdMS_TO_TICKS(50), pdTRUE,
                           [&count]() { count++; });
    t.start();
    vTaskDelay(pdMS_TO_TICKS(350));
    t.stop(portMAX_DELAY);
    r->auto_reload_fires = (count >= 3);
  }

  {
    volatile int count = 0;
    freertos::da::timer t("stopped", pdMS_TO_TICKS(50), pdTRUE,
                           [&count]() { count++; });
    t.start();
    vTaskDelay(pdMS_TO_TICKS(150));
    t.stop(portMAX_DELAY);
    int count_at_stop = count;
    vTaskDelay(pdMS_TO_TICKS(200));
    r->stop_prevents_callback = (count == count_at_stop);
  }

  {
    volatile int count = 0;
    freertos::da::timer t("reset_test", pdMS_TO_TICKS(200), pdFALSE,
                           [&count]() { count++; });
    t.start();
    vTaskDelay(pdMS_TO_TICKS(100));
    t.reset(portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(100));
    int count_before_expiry = count;
    vTaskDelay(pdMS_TO_TICKS(200));
    r->reset_restarts = (count_before_expiry == 0) && (count >= 1);
  }

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimTimer, AllTimerTests) {
  s_timer = {};

  xTaskCreate(timer_orchestrator, "tmrch", 2048, &s_timer, 1, nullptr);
  vTaskStartScheduler();

  EXPECT_TRUE(s_timer.one_shot_fires) << "One-shot timer fires once";
  EXPECT_TRUE(s_timer.auto_reload_fires) << "Auto-reload timer fires repeatedly";
  EXPECT_TRUE(s_timer.stop_prevents_callback) << "Stop prevents callback";
  EXPECT_TRUE(s_timer.reset_restarts) << "Reset restarts period";
  EXPECT_TRUE(s_timer.all_done) << "All tests completed";
}