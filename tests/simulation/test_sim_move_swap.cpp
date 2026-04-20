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

struct MoveResults {
  volatile bool binary_semaphore_move;
  volatile bool counting_semaphore_move;
  volatile bool queue_move;
  volatile bool mutex_move;
  volatile bool event_group_move;
  volatile bool binary_semaphore_swap;
  volatile bool queue_swap;
  volatile bool all_done;
};

static MoveResults s_move{};

void move_swap_orchestrator(void *param) {
  auto *r = static_cast<MoveResults *>(param);

  r->binary_semaphore_move = false;
  r->counting_semaphore_move = false;
  r->queue_move = false;
  r->mutex_move = false;
  r->event_group_move = false;
  r->binary_semaphore_swap = false;
  r->queue_swap = false;

  {
    freertos::da::binary_semaphore bs1;
    bs1.give();
    freertos::da::binary_semaphore bs2 = std::move(bs1);
    r->binary_semaphore_move = bs2.take(pdMS_TO_TICKS(10));
  }

  {
    freertos::da::counting_semaphore cs1(5);
    cs1.give();
    freertos::da::counting_semaphore cs2 = std::move(cs1);
    r->counting_semaphore_move = cs2.take(pdMS_TO_TICKS(10));
  }

  {
    freertos::da::queue<2, int> q1;
    q1.send(42, pdMS_TO_TICKS(10));
    freertos::da::queue<2, int> q2 = std::move(q1);
    auto val = q2.receive(pdMS_TO_TICKS(10));
    r->queue_move = val.has_value() && (val.value() == 42);
  }

  {
    freertos::da::mutex m1;
    m1.lock();
    m1.unlock();
    freertos::da::mutex m2 = std::move(m1);
    r->mutex_move = (m2.lock(pdMS_TO_TICKS(10)) == pdTRUE);
    m2.unlock();
  }

  {
    freertos::da::event_group eg1;
    eg1.set_bits(0x01);
    freertos::da::event_group eg2 = std::move(eg1);
    EventBits_t bits = eg2.wait_bits(0x01, pdFALSE, pdFALSE, pdMS_TO_TICKS(10));
    r->event_group_move = (bits & 0x01) != 0;
  }

  {
    freertos::da::binary_semaphore bs3;
    freertos::da::binary_semaphore bs4;
    bs3.give();
    bs3.swap(bs4);
    r->binary_semaphore_swap = bs4.take(pdMS_TO_TICKS(10)) &&
                               !bs3.take(pdMS_TO_TICKS(10));
  }

  {
    freertos::da::queue<2, int> q3;
    freertos::da::queue<2, int> q4;
    q3.send(99, pdMS_TO_TICKS(10));
    q3.swap(q4);
    auto val = q4.receive(pdMS_TO_TICKS(10));
    r->queue_swap = val.has_value() && (val.value() == 99);
  }

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimMoveSwap, AllMoveSwapTests) {
  s_move = {};

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