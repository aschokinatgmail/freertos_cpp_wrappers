#include <freertos.hpp>
#include <gtest/gtest.h>

#include <atomic>

struct PCData {
  freertos::da::queue<8, int> q;
  std::atomic<int> sum;
  std::atomic<int> prod_count;
  std::atomic<int> cons_count;
  std::atomic<bool> p1_ok;
  std::atomic<bool> p2_ok;
  std::atomic<bool> c_ok;

  void reset() {
    sum.store(0);
    prod_count.store(0);
    cons_count.store(0);
    p1_ok.store(false);
    p2_ok.store(false);
    c_ok.store(false);
  }
};

struct ConcurrencyResults {
  std::atomic<bool> producer_consumer;
  std::atomic<bool> timer_task_sync;
  std::atomic<bool> deadlock_timeout;
  std::atomic<bool> all_done;

  void reset() {
    producer_consumer.store(false);
    timer_task_sync.store(false);
    deadlock_timeout.store(false);
    all_done.store(false);
  }
};

static PCData s_pc;
static ConcurrencyResults s_conc;

void producer1(void *param) {
  auto *d = static_cast<PCData *>(param);
  for (int i = 1; i <= 5; ++i) {
    if (d->q.send(i, pdMS_TO_TICKS(100)) != pdTRUE) {
      d->p1_ok.store(false);
      vTaskDelete(nullptr);
      return;
    }
    d->prod_count++;
  }
  d->p1_ok.store(true);
  vTaskDelete(nullptr);
}

void producer2(void *param) {
  auto *d = static_cast<PCData *>(param);
  for (int i = 6; i <= 10; ++i) {
    if (d->q.send(i, pdMS_TO_TICKS(100)) != pdTRUE) {
      d->p2_ok.store(false);
      vTaskDelete(nullptr);
      return;
    }
    d->prod_count++;
  }
  d->p2_ok.store(true);
  vTaskDelete(nullptr);
}

void consumer(void *param) {
  auto *d = static_cast<PCData *>(param);
  int total = 0;
  int count = 0;
  while (count < 10) {
    auto val = d->q.receive(pdMS_TO_TICKS(500));
    if (val.has_value()) {
      total += val.value();
      count++;
    } else {
      break;
    }
  }
  d->sum.store(total);
  d->cons_count.store(count);
  d->c_ok.store(count == 10);
  vTaskDelete(nullptr);
}

void conc_orchestrator(void *param) {
  auto *r = static_cast<ConcurrencyResults *>(param);

  r->producer_consumer.store(false);
  r->timer_task_sync.store(false);
  r->deadlock_timeout.store(false);

  vTaskDelay(pdMS_TO_TICKS(1000));
  r->producer_consumer.store(s_pc.p1_ok && s_pc.p2_ok && s_pc.c_ok &&
                             (s_pc.sum == 55));

  {
    std::atomic<bool> timer_fired{false};
    freertos::da::binary_semaphore sync;
    freertos::da::timer t("sync_timer", pdMS_TO_TICKS(100), pdFALSE,
                           [&timer_fired, &sync]() {
                             timer_fired.store(true);
                             sync.give();
                           });
    std::atomic<bool> got_signal{false};
    struct P { freertos::da::binary_semaphore *sync; std::atomic<bool> *got_signal; };
    P p{&sync, &got_signal};
    auto waiter = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      if (pp->sync->take(pdMS_TO_TICKS(2000))) {
        pp->got_signal->store(true);
      }
      vTaskDelete(nullptr);
    };
    xTaskCreate(waiter, "syncw", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(10));
    t.start();
    vTaskDelay(pdMS_TO_TICKS(300));
    r->timer_task_sync.store(timer_fired && got_signal);
    t.stop(portMAX_DELAY);
  }

  {
    freertos::da::mutex m;
    std::atomic<bool> deadlock_detected{false};
    m.lock();
    auto result = m.try_lock();
    deadlock_detected.store(result != pdTRUE);
    m.unlock();
    r->deadlock_timeout.store(deadlock_detected);
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimConcurrency, AllConcurrencyTests) {
  s_pc.reset();
  s_conc.reset();

  xTaskCreate(producer1, "p1", 512, &s_pc, 2, nullptr);
  xTaskCreate(producer2, "p2", 512, &s_pc, 2, nullptr);
  xTaskCreate(consumer, "cons", 512, &s_pc, 3, nullptr);
  xTaskCreate(conc_orchestrator, "concorch", 2048, &s_conc, 1, nullptr);

  vTaskStartScheduler();

  EXPECT_TRUE(s_conc.producer_consumer) << "Producer-consumer";
  EXPECT_TRUE(s_conc.timer_task_sync) << "Timer-task sync";
  EXPECT_TRUE(s_conc.deadlock_timeout) << "Deadlock timeout";
  EXPECT_TRUE(s_conc.all_done) << "All tests completed";
}