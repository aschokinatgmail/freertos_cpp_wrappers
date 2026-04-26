#include <freertos.hpp>
#include <gtest/gtest.h>

#include <atomic>

struct SemResults {
  std::atomic<bool> binary_give_take;
  std::atomic<bool> binary_take_blocks;
  std::atomic<bool> binary_multiple_waiters;
  std::atomic<bool> counting_give_take;
  std::atomic<bool> counting_max_count;
  std::atomic<bool> counting_blocks_at_zero;
  std::atomic<bool> all_done;

  void reset() {
    binary_give_take.store(false);
    binary_take_blocks.store(false);
    binary_multiple_waiters.store(false);
    counting_give_take.store(false);
    counting_max_count.store(false);
    counting_blocks_at_zero.store(false);
    all_done.store(false);
  }
};

static SemResults s_sem;

void sem_orchestrator(void *param) {
  auto *r = static_cast<SemResults *>(param);

  r->binary_give_take.store(false);
  r->binary_take_blocks.store(false);
  r->binary_multiple_waiters.store(false);
  r->counting_give_take.store(false);
  r->counting_max_count.store(false);
  r->counting_blocks_at_zero.store(false);

  {
    freertos::da::binary_semaphore bs;
    r->binary_give_take.store((bs.give() == pdTRUE) && bs.take(pdMS_TO_TICKS(100)));
  }

  {
    freertos::da::binary_semaphore sync;
    std::atomic<bool> waiter_ok{false};
    struct P { freertos::da::binary_semaphore *s; std::atomic<bool> *ok; };
    P p{&sync, &waiter_ok};
    auto waiter = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      if (pp->s->take(pdMS_TO_TICKS(1000))) pp->ok->store(true);
      vTaskDelete(nullptr);
    };
    xTaskCreate(waiter, "w", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(50));
    sync.give();
    vTaskDelay(pdMS_TO_TICKS(100));
    r->binary_take_blocks.store(waiter_ok);
  }

  {
    freertos::da::binary_semaphore sync;
    std::atomic<int> w1{0}, w2{0};
    struct P { freertos::da::binary_semaphore *s; std::atomic<int> *r1; std::atomic<int> *r2; };
    P p{&sync, &w1, &w2};
    auto w1f = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      if (pp->s->take(pdMS_TO_TICKS(500))) pp->r1->store(1);
      vTaskDelete(nullptr);
    };
    auto w2f = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      if (pp->s->take(pdMS_TO_TICKS(500))) pp->r2->store(1);
      vTaskDelete(nullptr);
    };
    xTaskCreate(w1f, "w1", 512, &p, 2, nullptr);
    xTaskCreate(w2f, "w2", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(50));
    sync.give();
    vTaskDelay(pdMS_TO_TICKS(100));
    r->binary_multiple_waiters.store((w1 + w2) == 1);
  }

  {
    freertos::da::counting_semaphore cs(5);
    r->counting_give_take.store(cs.count() == 5);
    r->counting_give_take.store(r->counting_give_take && cs.take(pdMS_TO_TICKS(10)));
    r->counting_give_take.store(r->counting_give_take && cs.count() == 4);
    r->counting_give_take.store(r->counting_give_take && cs.take(pdMS_TO_TICKS(10)));
    r->counting_give_take.store(r->counting_give_take && cs.count() == 3);
  }

  {
    freertos::da::counting_semaphore cs(3);
    r->counting_max_count.store(cs.count() == 3);
    r->counting_max_count.store(r->counting_max_count && cs.give() != pdTRUE);
  }

  {
    freertos::da::counting_semaphore cs(3);
    cs.take(pdMS_TO_TICKS(10));
    cs.take(pdMS_TO_TICKS(10));
    cs.take(pdMS_TO_TICKS(10));
    std::atomic<bool> waiter_ok{false};
    struct P { freertos::da::counting_semaphore *s; std::atomic<bool> *ok; };
    P p{&cs, &waiter_ok};
    auto waiter = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      if (pp->s->take(pdMS_TO_TICKS(500))) pp->ok->store(true);
      vTaskDelete(nullptr);
    };
    xTaskCreate(waiter, "cw", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(50));
    cs.give();
    vTaskDelay(pdMS_TO_TICKS(100));
    r->counting_blocks_at_zero.store(waiter_ok);
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimSemaphore, AllSemaphoreTests) {
  s_sem.reset();
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