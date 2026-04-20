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

struct PCData {
  freertos::da::queue<8, int> q;
  volatile int sum;
  volatile int prod_count;
  volatile int cons_count;
  volatile bool p1_ok;
  volatile bool p2_ok;
  volatile bool c_ok;
};

struct ConcurrencyResults {
  volatile bool producer_consumer;
  volatile bool timer_task_sync;
  volatile bool deadlock_timeout;
  volatile bool all_done;
};

static PCData s_pc;
static ConcurrencyResults s_conc;

void producer1(void *param) {
  auto *d = static_cast<PCData *>(param);
  for (int i = 1; i <= 5; ++i) {
    if (d->q.send(i, pdMS_TO_TICKS(100)) != pdTRUE) {
      d->p1_ok = false;
      vTaskDelete(nullptr);
      return;
    }
    d->prod_count++;
  }
  d->p1_ok = true;
  vTaskDelete(nullptr);
}

void producer2(void *param) {
  auto *d = static_cast<PCData *>(param);
  for (int i = 6; i <= 10; ++i) {
    if (d->q.send(i, pdMS_TO_TICKS(100)) != pdTRUE) {
      d->p2_ok = false;
      vTaskDelete(nullptr);
      return;
    }
    d->prod_count++;
  }
  d->p2_ok = true;
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
  d->sum = total;
  d->cons_count = count;
  d->c_ok = (count == 10);
  vTaskDelete(nullptr);
}

void conc_orchestrator(void *param) {
  auto *r = static_cast<ConcurrencyResults *>(param);

  r->producer_consumer = false;
  r->timer_task_sync = false;
  r->deadlock_timeout = false;

  vTaskDelay(pdMS_TO_TICKS(1000));
  r->producer_consumer = s_pc.p1_ok && s_pc.p2_ok && s_pc.c_ok &&
                         (s_pc.sum == 55);

  {
    volatile bool timer_fired = false;
    volatile bool task_signaled = false;
    freertos::da::binary_semaphore sync;
    freertos::da::timer t("sync_timer", pdMS_TO_TICKS(100), pdFALSE,
                           [&timer_fired, &sync]() {
                             timer_fired = true;
                             sync.give();
                           });
    volatile bool got_signal = false;
    struct P { freertos::da::binary_semaphore *sync; volatile bool *got_signal; };
    P p{&sync, &got_signal};
    auto waiter = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      if (pp->sync->take(pdMS_TO_TICKS(2000))) {
        *pp->got_signal = true;
      }
      vTaskDelete(nullptr);
    };
    xTaskCreate(waiter, "syncw", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(10));
    t.start();
    vTaskDelay(pdMS_TO_TICKS(300));
    r->timer_task_sync = timer_fired && got_signal;
    t.stop(portMAX_DELAY);
  }

  {
    freertos::da::mutex m;
    volatile bool deadlock_detected = false;
    m.lock();
    auto result = m.try_lock();
    deadlock_detected = (result != pdTRUE);
    m.unlock();
    r->deadlock_timeout = deadlock_detected;
  }

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimConcurrency, AllConcurrencyTests) {
  s_pc.sum = 0;
  s_pc.prod_count = 0;
  s_pc.cons_count = 0;
  s_pc.p1_ok = false;
  s_pc.p2_ok = false;
  s_pc.c_ok = false;
  memset(&s_conc, 0, sizeof(s_conc));

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