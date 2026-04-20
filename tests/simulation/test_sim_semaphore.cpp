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

struct SemResults {
  volatile bool binary_give_take;
  volatile bool binary_take_blocks;
  volatile bool binary_multiple_waiters;
  volatile bool counting_give_take;
  volatile bool counting_max_count;
  volatile bool counting_blocks_at_zero;
  volatile bool all_done;
};

static SemResults s_sem;

void sem_orchestrator(void *param) {
  auto *r = static_cast<SemResults *>(param);

  r->binary_give_take = false;
  r->binary_take_blocks = false;
  r->binary_multiple_waiters = false;
  r->counting_give_take = false;
  r->counting_max_count = false;
  r->counting_blocks_at_zero = false;

  {
    freertos::da::binary_semaphore bs;
    r->binary_give_take = (bs.give() == pdTRUE) && bs.take(pdMS_TO_TICKS(100));
  }

  {
    freertos::da::binary_semaphore sync;
    volatile bool waiter_ok = false;
    struct P { freertos::da::binary_semaphore *s; volatile bool *ok; };
    P p{&sync, &waiter_ok};
    auto waiter = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      if (pp->s->take(pdMS_TO_TICKS(1000))) *pp->ok = true;
      vTaskDelete(nullptr);
    };
    xTaskCreate(waiter, "w", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(50));
    sync.give();
    vTaskDelay(pdMS_TO_TICKS(100));
    r->binary_take_blocks = waiter_ok;
  }

  {
    freertos::da::binary_semaphore sync;
    volatile int w1 = 0, w2 = 0;
    struct P { freertos::da::binary_semaphore *s; volatile int *r1; volatile int *r2; };
    P p{&sync, &w1, &w2};
    auto w1f = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      if (pp->s->take(pdMS_TO_TICKS(500))) (*pp->r1) = 1;
      vTaskDelete(nullptr);
    };
    auto w2f = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      if (pp->s->take(pdMS_TO_TICKS(500))) (*pp->r2) = 1;
      vTaskDelete(nullptr);
    };
    xTaskCreate(w1f, "w1", 512, &p, 2, nullptr);
    xTaskCreate(w2f, "w2", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(50));
    sync.give();
    vTaskDelay(pdMS_TO_TICKS(100));
    r->binary_multiple_waiters = ((w1 + w2) == 1);
  }

  {
    freertos::da::counting_semaphore cs(5);
    r->counting_give_take = (cs.count() == 5);
    r->counting_give_take = r->counting_give_take && cs.take(pdMS_TO_TICKS(10));
    r->counting_give_take = r->counting_give_take && (cs.count() == 4);
    r->counting_give_take = r->counting_give_take && cs.take(pdMS_TO_TICKS(10));
    r->counting_give_take = r->counting_give_take && (cs.count() == 3);
  }

  {
    freertos::da::counting_semaphore cs(3);
    r->counting_max_count = (cs.count() == 3);
    r->counting_max_count = r->counting_max_count && (cs.give() != pdTRUE);
  }

  {
    freertos::da::counting_semaphore cs(3);
    cs.take(pdMS_TO_TICKS(10));
    cs.take(pdMS_TO_TICKS(10));
    cs.take(pdMS_TO_TICKS(10));
    volatile bool waiter_ok = false;
    struct P { freertos::da::counting_semaphore *s; volatile bool *ok; };
    P p{&cs, &waiter_ok};
    auto waiter = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      if (pp->s->take(pdMS_TO_TICKS(500))) *pp->ok = true;
      vTaskDelete(nullptr);
    };
    xTaskCreate(waiter, "cw", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(50));
    cs.give();
    vTaskDelay(pdMS_TO_TICKS(100));
    r->counting_blocks_at_zero = waiter_ok;
  }

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimSemaphore, AllSemaphoreTests) {
  s_sem = {};
  xTaskCreate(sem_orchestrator, "sem", 2048, &s_sem, 1, nullptr);
  vTaskStartScheduler();

  EXPECT_TRUE(s_sem.binary_give_take) << "Binary give/take";
  EXPECT_TRUE(s_sem.binary_take_blocks) << "Binary take blocks until give";
  EXPECT_TRUE(s_sem.binary_multiple_waiters) << "Binary multiple waiters";
  EXPECT_TRUE(s_sem.counting_give_take) << "Counting give/take";
  EXPECT_TRUE(s_sem.counting_max_count) << "Counting max count";
  EXPECT_TRUE(s_sem.counting_blocks_at_zero) << "Counting blocks at zero";
  EXPECT_TRUE(s_sem.all_done) << "All tests completed";
}