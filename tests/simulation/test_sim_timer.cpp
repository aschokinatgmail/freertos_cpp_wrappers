#include <freertos.hpp>
#include <gtest/gtest.h>

#include <atomic>

struct TimerResults {
  std::atomic<bool> one_shot_fires;
  std::atomic<bool> auto_reload_fires;
  std::atomic<bool> stop_prevents_callback;
  std::atomic<bool> reset_restarts;
  std::atomic<bool> all_done;

  void reset() {
    one_shot_fires.store(false);
    auto_reload_fires.store(false);
    stop_prevents_callback.store(false);
    reset_restarts.store(false);
    all_done.store(false);
  }
};

static TimerResults s_timer;

void timer_orchestrator(void *param) {
  auto *r = static_cast<TimerResults *>(param);

  r->one_shot_fires.store(false);
  r->auto_reload_fires.store(false);
  r->stop_prevents_callback.store(false);
  r->reset_restarts.store(false);

  {
    std::atomic<int> count{0};
    freertos::da::timer t("oneshot", pdMS_TO_TICKS(100), pdFALSE,
                           [&count]() { count++; });
    t.start();
    vTaskDelay(pdMS_TO_TICKS(300));
    r->one_shot_fires.store(count == 1);
  }

  {
    std::atomic<int> count{0};
    freertos::da::timer t("autoreload", pdMS_TO_TICKS(50), pdTRUE,
                           [&count]() { count++; });
    t.start();
    vTaskDelay(pdMS_TO_TICKS(350));
    t.stop(portMAX_DELAY);
    r->auto_reload_fires.store(count >= 3);
  }

  {
    std::atomic<int> count{0};
    freertos::da::timer t("stopped", pdMS_TO_TICKS(50), pdTRUE,
                           [&count]() { count++; });
    t.start();
    vTaskDelay(pdMS_TO_TICKS(150));
    t.stop(portMAX_DELAY);
    int count_at_stop = count;
    vTaskDelay(pdMS_TO_TICKS(200));
    r->stop_prevents_callback.store(count == count_at_stop);
  }

  {
    std::atomic<int> count{0};
    freertos::da::timer t("reset_test", pdMS_TO_TICKS(200), pdFALSE,
                           [&count]() { count++; });
    t.start();
    vTaskDelay(pdMS_TO_TICKS(100));
    t.reset(portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(100));
    int count_before_expiry = count;
    vTaskDelay(pdMS_TO_TICKS(200));
    r->reset_restarts.store(count_before_expiry == 0 && count >= 1);
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimTimer, AllTimerTests) {
  s_timer.reset();

  xTaskCreate(timer_orchestrator, "tmrch", 2048, &s_timer, 1, nullptr);
  vTaskStartScheduler();

  EXPECT_TRUE(s_timer.one_shot_fires) << "One-shot timer fires once";
  EXPECT_TRUE(s_timer.auto_reload_fires) << "Auto-reload timer fires repeatedly";
  EXPECT_TRUE(s_timer.stop_prevents_callback) << "Stop prevents callback";
  EXPECT_TRUE(s_timer.reset_restarts) << "Reset restarts period";
  EXPECT_TRUE(s_timer.all_done) << "All tests completed";
}