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

struct QueueData {
  freertos::da::queue<4, int> q;
  volatile bool producer_ok;
  volatile bool consumer_ok;
  volatile int received_sum;
};

struct QueueResults {
  volatile bool send_receive;
  volatile bool fifo_order;
  volatile bool producer_consumer;
  volatile bool blocking_on_full;
  volatile bool blocking_on_empty;
  volatile bool peek_no_remove;
  volatile bool all_done;
};

static QueueData s_queue_data;
static QueueResults s_queue{};

void queue_producer(void *param) {
  auto *d = static_cast<QueueData *>(param);
  for (int i = 1; i <= 10; ++i) {
    if (d->q.send(i, pdMS_TO_TICKS(100)) != pdTRUE) {
      d->producer_ok = false;
      vTaskDelete(nullptr);
      return;
    }
  }
  d->producer_ok = true;
  vTaskDelete(nullptr);
}

void queue_consumer(void *param) {
  auto *d = static_cast<QueueData *>(param);
  int sum = 0;
  int count = 0;
  while (count < 10) {
    auto val = d->q.receive(pdMS_TO_TICKS(500));
    if (val.has_value()) {
      sum += val.value();
      count++;
    } else {
      d->received_sum = sum;
      d->consumer_ok = false;
      vTaskDelete(nullptr);
      return;
    }
  }
  d->received_sum = sum;
  d->consumer_ok = true;
  vTaskDelete(nullptr);
}

void queue_orchestrator(void *param) {
  auto *r = static_cast<QueueResults *>(param);

  r->send_receive = false;
  r->fifo_order = false;
  r->producer_consumer = false;
  r->blocking_on_full = false;
  r->blocking_on_empty = false;
  r->peek_no_remove = false;

  {
    freertos::da::queue<4, int> q;
    q.send(42, pdMS_TO_TICKS(10));
    auto val = q.receive(pdMS_TO_TICKS(10));
    r->send_receive = val.has_value() && (val.value() == 42);
  }

  {
    freertos::da::queue<4, int> q;
    q.send(1, pdMS_TO_TICKS(10));
    q.send(2, pdMS_TO_TICKS(10));
    q.send(3, pdMS_TO_TICKS(10));
    auto v1 = q.receive(pdMS_TO_TICKS(10));
    auto v2 = q.receive(pdMS_TO_TICKS(10));
    auto v3 = q.receive(pdMS_TO_TICKS(10));
    r->fifo_order = v1.has_value() && v2.has_value() && v3.has_value() &&
                    (v1.value() == 1) && (v2.value() == 2) && (v3.value() == 3);
  }

  vTaskDelay(pdMS_TO_TICKS(500));
  r->producer_consumer = s_queue_data.producer_ok &&
                         s_queue_data.consumer_ok &&
                         (s_queue_data.received_sum == 55);

  {
    freertos::da::queue<2, int> q;
    q.send(1, pdMS_TO_TICKS(10));
    q.send(2, pdMS_TO_TICKS(10));
    volatile bool blocked = false;
    struct P { freertos::da::queue<2, int> *q; volatile bool *blocked; };
    P p{&q, &blocked};
    auto producer = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      pp->q->send(3, pdMS_TO_TICKS(500));
      *pp->blocked = true;
      vTaskDelete(nullptr);
    };
    xTaskCreate(producer, "qfull", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(100));
    r->blocking_on_full = !blocked;
    q.receive(pdMS_TO_TICKS(10));
    vTaskDelay(pdMS_TO_TICKS(200));
  }

  {
    freertos::da::queue<2, int> q;
    volatile bool was_empty = false;
    struct P { freertos::da::queue<2, int> *q; volatile bool *was_empty; };
    P p{&q, &was_empty};
    auto consumer = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      auto val = pp->q->receive(pdMS_TO_TICKS(500));
      *pp->was_empty = val.has_value() && (val.value() == 99);
      vTaskDelete(nullptr);
    };
    vTaskDelay(pdMS_TO_TICKS(50));
    xTaskCreate(consumer, "qempty", 512, &p, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(50));
    q.send(99, pdMS_TO_TICKS(10));
    vTaskDelay(pdMS_TO_TICKS(200));
    r->blocking_on_empty = was_empty;
  }

  {
    freertos::da::queue<4, int> q;
    q.send(77, pdMS_TO_TICKS(10));
    auto peek_val = q.peek(pdMS_TO_TICKS(10));
    bool peek_ok = peek_val.has_value() && (peek_val.value() == 77);
    auto recv_val = q.receive(pdMS_TO_TICKS(10));
    r->peek_no_remove = peek_ok && recv_val.has_value() && (recv_val.value() == 77);
  }

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimQueue, AllQueueTests) {
  memset(&s_queue, 0, sizeof(s_queue));

  xTaskCreate(queue_producer, "qprod", 512, &s_queue_data, 2, nullptr);
  xTaskCreate(queue_consumer, "qcons", 512, &s_queue_data, 3, nullptr);
  xTaskCreate(queue_orchestrator, "qorch", 2048, &s_queue, 1, nullptr);

  vTaskStartScheduler();

  EXPECT_TRUE(s_queue.send_receive) << "Queue send/receive";
  EXPECT_TRUE(s_queue.fifo_order) << "Queue FIFO ordering";
  EXPECT_TRUE(s_queue.producer_consumer) << "Queue producer/consumer";
  EXPECT_TRUE(s_queue.blocking_on_full) << "Queue blocking on full";
  EXPECT_TRUE(s_queue.blocking_on_empty) << "Queue blocking on empty";
  EXPECT_TRUE(s_queue.peek_no_remove) << "Queue peek no remove";
  EXPECT_TRUE(s_queue.all_done) << "All tests completed";
}