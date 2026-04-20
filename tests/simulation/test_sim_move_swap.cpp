#include <freertos.hpp>
#include <gtest/gtest.h>

#include <atomic>

struct MoveResults {
  std::atomic<bool> binary_semaphore_move;
  std::atomic<bool> counting_semaphore_move;
  std::atomic<bool> queue_move;
  std::atomic<bool> mutex_move;
  std::atomic<bool> event_group_move;
  std::atomic<bool> binary_semaphore_swap;
  std::atomic<bool> queue_swap;
  std::atomic<bool> all_done;

  void reset() {
    binary_semaphore_move.store(false);
    counting_semaphore_move.store(false);
    queue_move.store(false);
    mutex_move.store(false);
    event_group_move.store(false);
    binary_semaphore_swap.store(false);
    queue_swap.store(false);
    all_done.store(false);
  }
};

static MoveResults s_move;

void move_swap_orchestrator(void *param) {
  auto *r = static_cast<MoveResults *>(param);

  r->binary_semaphore_move.store(false);
  r->counting_semaphore_move.store(false);
  r->queue_move.store(false);
  r->mutex_move.store(false);
  r->event_group_move.store(false);
  r->binary_semaphore_swap.store(false);
  r->queue_swap.store(false);

  {
    freertos::da::binary_semaphore bs1;
    bs1.give();
    freertos::da::binary_semaphore bs2 = std::move(bs1);
    r->binary_semaphore_move.store(bs2.take(pdMS_TO_TICKS(10)));
  }

  {
    freertos::da::counting_semaphore cs1(5);
    cs1.give();
    freertos::da::counting_semaphore cs2 = std::move(cs1);
    r->counting_semaphore_move.store(cs2.take(pdMS_TO_TICKS(10)));
  }

  {
    freertos::da::queue<2, int> q1;
    q1.send(42, pdMS_TO_TICKS(10));
    freertos::da::queue<2, int> q2 = std::move(q1);
    auto val = q2.receive(pdMS_TO_TICKS(10));
    r->queue_move.store(val.has_value() && (val.value() == 42));
  }

  {
    freertos::da::mutex m1;
    m1.lock();
    m1.unlock();
    freertos::da::mutex m2 = std::move(m1);
    r->mutex_move.store(m2.lock(pdMS_TO_TICKS(10)) == pdTRUE);
    m2.unlock();
  }

  {
    freertos::da::event_group eg1;
    eg1.set_bits(0x01);
    freertos::da::event_group eg2 = std::move(eg1);
    EventBits_t bits = eg2.wait_bits(0x01, pdFALSE, pdFALSE, pdMS_TO_TICKS(10));
    r->event_group_move.store((bits & 0x01) != 0);
  }

  {
    freertos::da::binary_semaphore bs3;
    freertos::da::binary_semaphore bs4;
    bs3.give();
    bs3.swap(bs4);
    r->binary_semaphore_swap.store(bs4.take(pdMS_TO_TICKS(10)) &&
                                   !bs3.take(pdMS_TO_TICKS(10)));
  }

  {
    freertos::da::queue<2, int> q3;
    freertos::da::queue<2, int> q4;
    q3.send(99, pdMS_TO_TICKS(10));
    q3.swap(q4);
    auto val = q4.receive(pdMS_TO_TICKS(10));
    r->queue_swap.store(val.has_value() && (val.value() == 99));
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimMoveSwap, AllMoveSwapTests) {
  s_move.reset();

  xTaskCreate(move_swap_orchestrator, "move", 2048, &s_move, 1, nullptr);
  vTaskStartScheduler();

  EXPECT_TRUE(s_move.binary_semaphore_move) << "Binary semaphore move";
  EXPECT_TRUE(s_move.counting_semaphore_move) << "Counting semaphore move";
  EXPECT_TRUE(s_move.queue_move) << "Queue move";
  EXPECT_TRUE(s_move.mutex_move) << "Mutex move";
  EXPECT_TRUE(s_move.event_group_move) << "Event group move";
  EXPECT_TRUE(s_move.binary_semaphore_swap) << "Binary semaphore swap";
  EXPECT_TRUE(s_move.queue_swap) << "Queue swap";
  EXPECT_TRUE(s_move.all_done) << "All tests completed";
}