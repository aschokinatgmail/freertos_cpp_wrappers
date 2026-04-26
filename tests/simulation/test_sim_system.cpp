#include <freertos.hpp>

#include <atomic>
#include <chrono>
#include <gtest/gtest.h>

// --- Test result flags ---
struct AllResults {
  std::atomic<bool> binary_sem_ok;
  std::atomic<bool> counting_sem_ok;
  std::atomic<bool> mutex_contention_ok;
  std::atomic<bool> recursive_mutex_ok;
  std::atomic<bool> queue_prod_cons_ok;
  std::atomic<bool> event_group_sync_ok;
  std::atomic<bool> timer_callback_ok;
  std::atomic<bool> task_notification_ok;
  std::atomic<bool> message_buffer_ok;
  std::atomic<bool> stream_buffer_ok;
  std::atomic<bool> lock_guard_ok;
  std::atomic<bool> move_semantics_ok;
  std::atomic<bool> all_done;

  void reset() {
    binary_sem_ok.store(false);
    counting_sem_ok.store(false);
    mutex_contention_ok.store(false);
    recursive_mutex_ok.store(false);
    queue_prod_cons_ok.store(false);
    event_group_sync_ok.store(false);
    timer_callback_ok.store(false);
    task_notification_ok.store(false);
    message_buffer_ok.store(false);
    stream_buffer_ok.store(false);
    lock_guard_ok.store(false);
    move_semantics_ok.store(false);
    all_done.store(false);
  }
};

// --- Shared data for multi-task tests ---
struct MutexData {
  freertos::da::mutex mtx;
  std::atomic<int> counter;
  std::atomic<bool> t1_ok;
  std::atomic<bool> t2_ok;

  void reset() {
    counter.store(0);
    t1_ok.store(false);
    t2_ok.store(false);
  }
};

struct QueueData {
  freertos::da::queue<4, int> q;
  std::atomic<bool> producer_ok;
  std::atomic<bool> consumer_ok;
  std::atomic<int> received_sum;

  void reset() {
    producer_ok.store(false);
    consumer_ok.store(false);
    received_sum.store(0);
  }
};

struct EventGroupData {
  freertos::da::event_group eg;
  std::atomic<bool> t1_ok;
  std::atomic<bool> t2_ok;

  void reset() {
    t1_ok.store(false);
    t2_ok.store(false);
  }
};

struct MessageBufferData {
  freertos::da::message_buffer<128> mb;
  std::atomic<bool> send_ok;
  std::atomic<bool> recv_ok;
  std::atomic<size_t> recv_len;
  char recv_buf[64];

  void reset() {
    send_ok.store(false);
    recv_ok.store(false);
    recv_len.store(0);
  }
};

struct StreamBufferData {
  freertos::da::stream_buffer<128> sb;
  std::atomic<bool> send_ok;
  std::atomic<bool> recv_ok;
  std::atomic<size_t> recv_len;
  uint8_t recv_buf[64];

  void reset() {
    send_ok.store(false);
    recv_ok.store(false);
    recv_len.store(0);
  }
};

// Globals accessible from both TEST body and task functions
static MutexData s_mutex_data;
static QueueData s_queue_data;
static EventGroupData s_eg_data;
static MessageBufferData s_mb_data;
static StreamBufferData s_sb_data;
static TaskHandle_t s_orch_handle = nullptr;

// --- Sub-task functions (all created BEFORE vTaskStartScheduler) ---

void mutex_task1(void *param) {
  auto *d = static_cast<MutexData *>(param);
  for (int i = 0; i < 50; ++i) {
    if (d->mtx.lock(pdMS_TO_TICKS(100)) == pdTRUE) {
      d->counter++;
      d->mtx.unlock();
    } else {
      d->t1_ok.store(false);
      vTaskDelete(nullptr);
      return;
    }
  }
  d->t1_ok.store(true);
  vTaskDelete(nullptr);
}

void mutex_task2(void *param) {
  auto *d = static_cast<MutexData *>(param);
  for (int i = 0; i < 50; ++i) {
    if (d->mtx.lock(pdMS_TO_TICKS(100)) == pdTRUE) {
      d->counter++;
      d->mtx.unlock();
    } else {
      d->t2_ok.store(false);
      vTaskDelete(nullptr);
      return;
    }
  }
  d->t2_ok.store(true);
  vTaskDelete(nullptr);
}

void queue_producer(void *param) {
  auto *d = static_cast<QueueData *>(param);
  for (int i = 1; i <= 10; ++i) {
    if (d->q.send(i, pdMS_TO_TICKS(100)) != pdTRUE) {
      d->producer_ok.store(false);
      vTaskDelete(nullptr);
      return;
    }
  }
  d->producer_ok.store(true);
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
      d->received_sum.store(sum);
      d->consumer_ok.store(false);
      vTaskDelete(nullptr);
      return;
    }
  }
  d->received_sum.store(sum);
  d->consumer_ok.store(true);
  vTaskDelete(nullptr);
}

void eg_task1(void *param) {
  auto *d = static_cast<EventGroupData *>(param);
  d->eg.set_bits(0x01);
  EventBits_t result =
      d->eg.wait_bits(0x02, pdTRUE, pdTRUE, pdMS_TO_TICKS(2000));
  d->t1_ok.store((result & 0x02) != 0);
  vTaskDelete(nullptr);
}

void eg_task2(void *param) {
  auto *d = static_cast<EventGroupData *>(param);
  d->eg.set_bits(0x02);
  EventBits_t result =
      d->eg.wait_bits(0x01, pdTRUE, pdTRUE, pdMS_TO_TICKS(2000));
  d->t2_ok.store((result & 0x01) != 0);
  vTaskDelete(nullptr);
}

void mb_sender(void *param) {
  auto *d = static_cast<MessageBufferData *>(param);
  const char *msg = "hello freertos";
  size_t sent = d->mb.send(msg, 15, pdMS_TO_TICKS(100));
  d->send_ok.store(sent == 15);
  vTaskDelete(nullptr);
}

void mb_receiver(void *param) {
  auto *d = static_cast<MessageBufferData *>(param);
  d->recv_len.store(d->mb.receive(d->recv_buf, 64, pdMS_TO_TICKS(500)));
  d->recv_ok.store(d->recv_len > 0);
  vTaskDelete(nullptr);
}

void sb_sender(void *param) {
  auto *d = static_cast<StreamBufferData *>(param);
  const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
  d->send_ok.store(d->sb.send(data, 4, pdMS_TO_TICKS(100)) > 0);
  vTaskDelete(nullptr);
}

void sb_receiver(void *param) {
  auto *d = static_cast<StreamBufferData *>(param);
  d->recv_len.store(d->sb.receive(d->recv_buf, 64, pdMS_TO_TICKS(500)));
  d->recv_ok.store(d->recv_len == 4);
  vTaskDelete(nullptr);
}

void notifier_task(void *param) {
  TaskHandle_t target = static_cast<TaskHandle_t>(param);
  vTaskDelay(pdMS_TO_TICKS(50));
  xTaskNotifyGive(target);
  vTaskDelete(nullptr);
}

// --- Orchestrator: runs single-task tests and waits for sub-tasks ---

void orchestrator_task(void *param) {
  auto *r = static_cast<AllResults *>(param);

  // Binary semaphore
  {
    freertos::da::binary_semaphore sem;
    r->binary_sem_ok.store(sem.give() == pdTRUE);
    r->binary_sem_ok.store(r->binary_sem_ok && sem.take(pdMS_TO_TICKS(100)));
    r->binary_sem_ok.store(r->binary_sem_ok && !sem.take(pdMS_TO_TICKS(10)));
  }

  // Counting semaphore
  {
    freertos::da::counting_semaphore cs(3);
    r->counting_sem_ok.store(true);
    cs.give();
    cs.give();
    cs.give();
    r->counting_sem_ok.store(r->counting_sem_ok && (cs.count() == 3));
    r->counting_sem_ok.store(r->counting_sem_ok && cs.take(pdMS_TO_TICKS(10)));
    r->counting_sem_ok.store(r->counting_sem_ok && (cs.count() == 2));
    r->counting_sem_ok.store(r->counting_sem_ok && cs.take(pdMS_TO_TICKS(10)));
    r->counting_sem_ok.store(r->counting_sem_ok && cs.take(pdMS_TO_TICKS(10)));
    r->counting_sem_ok.store(r->counting_sem_ok && !cs.take(pdMS_TO_TICKS(10)));
  }

  // Recursive mutex
  {
    freertos::da::recursive_mutex rm;
    r->recursive_mutex_ok.store(true);
    r->recursive_mutex_ok.store(r->recursive_mutex_ok && (rm.lock() == pdTRUE));
    r->recursive_mutex_ok.store(r->recursive_mutex_ok && rm.locked());
    r->recursive_mutex_ok.store(
        r->recursive_mutex_ok && (rm.recursions_count() == 1));
    r->recursive_mutex_ok.store(r->recursive_mutex_ok && (rm.lock() == pdTRUE));
    r->recursive_mutex_ok.store(
        r->recursive_mutex_ok && (rm.recursions_count() == 2));
    r->recursive_mutex_ok.store(r->recursive_mutex_ok && (rm.unlock() == pdTRUE));
    r->recursive_mutex_ok.store(
        r->recursive_mutex_ok && (rm.recursions_count() == 1));
    r->recursive_mutex_ok.store(r->recursive_mutex_ok && (rm.unlock() == pdTRUE));
    r->recursive_mutex_ok.store(r->recursive_mutex_ok && !rm.locked());
    r->recursive_mutex_ok.store(
        r->recursive_mutex_ok && (rm.recursions_count() == 0));
  }

  // Lock guard RAII
  {
    freertos::da::mutex guard_mtx;
    r->lock_guard_ok.store(true);
    {
      freertos::lock_guard<freertos::da::mutex> lg(guard_mtx);
      r->lock_guard_ok.store(r->lock_guard_ok && guard_mtx.locked());
    }
    r->lock_guard_ok.store(r->lock_guard_ok && !guard_mtx.locked());

    freertos::da::mutex tlg_mtx;
    {
      freertos::try_lock_guard<freertos::da::mutex> tlg(tlg_mtx);
      r->lock_guard_ok.store(r->lock_guard_ok && tlg.locked() && tlg_mtx.locked());
    }
    r->lock_guard_ok.store(r->lock_guard_ok && !tlg_mtx.locked());

    freertos::da::mutex tolg_mtx;
    {
      freertos::timeout_lock_guard<freertos::da::mutex> tolg(
          tolg_mtx, std::chrono::milliseconds(50));
      r->lock_guard_ok.store(r->lock_guard_ok && tolg.locked() && tolg_mtx.locked());
    }
    r->lock_guard_ok.store(r->lock_guard_ok && !tolg_mtx.locked());
  }

  // Move/swap semantics
  {
    r->move_semantics_ok.store(true);
    freertos::da::binary_semaphore bs1;
    bs1.give();
    freertos::da::binary_semaphore bs2 = std::move(bs1);
    r->move_semantics_ok.store(r->move_semantics_ok && bs2.take(pdMS_TO_TICKS(10)));

    freertos::da::queue<2, int> q1;
    q1.send(42, pdMS_TO_TICKS(10));
    freertos::da::queue<2, int> q2 = std::move(q1);
    auto val = q2.receive(pdMS_TO_TICKS(10));
    r->move_semantics_ok.store(
        r->move_semantics_ok && val.has_value() && (val.value() == 42));

    freertos::da::counting_semaphore cs1(5);
    cs1.give();
    freertos::da::counting_semaphore cs2 = std::move(cs1);
    r->move_semantics_ok.store(r->move_semantics_ok && cs2.take(pdMS_TO_TICKS(10)));

    // Swap test with binary semaphore
    freertos::da::binary_semaphore bs3;
    freertos::da::binary_semaphore bs4;
    bs3.give();
    bs3.swap(bs4);
    r->move_semantics_ok.store(
        r->move_semantics_ok && bs4.take(pdMS_TO_TICKS(10)) &&
        !bs3.take(pdMS_TO_TICKS(10)));
  }

  // Software timer (auto-reload)
  {
    std::atomic<int> timer_count{0};
    freertos::da::timer t("test_timer", pdMS_TO_TICKS(50), pdTRUE,
                           [&timer_count]() { timer_count++; });
    t.start();
    vTaskDelay(pdMS_TO_TICKS(350));
    t.stop(portMAX_DELAY);
    r->timer_callback_ok.store(timer_count >= 3);
  }

  // Task notification
  r->task_notification_ok.store(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)) > 0);

  // Wait for mutex contention sub-tasks
  vTaskDelay(pdMS_TO_TICKS(500));
  r->mutex_contention_ok.store(
      s_mutex_data.t1_ok && s_mutex_data.t2_ok && (s_mutex_data.counter == 100));

  // Wait for queue sub-tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
  r->queue_prod_cons_ok.store(s_queue_data.producer_ok &&
                              s_queue_data.consumer_ok &&
                              (s_queue_data.received_sum == 55));

  // Wait for event group sub-tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
  r->event_group_sync_ok.store(s_eg_data.t1_ok && s_eg_data.t2_ok);

  // Wait for message buffer sub-tasks
  vTaskDelay(pdMS_TO_TICKS(500));
  r->message_buffer_ok.store(s_mb_data.send_ok && s_mb_data.recv_ok &&
                             (s_mb_data.recv_len == 15) &&
                             (s_mb_data.recv_buf[0] == 'h'));

  // Wait for stream buffer sub-tasks
  vTaskDelay(pdMS_TO_TICKS(500));
  r->stream_buffer_ok.store(
      s_sb_data.send_ok && s_sb_data.recv_ok && (s_sb_data.recv_len == 4) &&
      (s_sb_data.recv_buf[0] == 0xDE) && (s_sb_data.recv_buf[3] == 0xEF));

  r->all_done.store(true);
  vTaskEndScheduler();
}

// === TEST ===

TEST(SimulationSystem, AllWrapperTests) {
  AllResults results;
  results.reset();
  s_mutex_data.reset();
  s_queue_data.reset();
  s_eg_data.reset();
  s_mb_data.reset();
  s_sb_data.reset();

  // All tasks must be created BEFORE vTaskStartScheduler
  // (FreeRTOS POSIX port limitation: xTaskCreate after vTaskStartScheduler
  //  triggers configASSERT in prvWaitForStart at port.c:568)
  xTaskCreate(orchestrator_task, "orch", 2048, &results, 1, &s_orch_handle);
  xTaskCreate(mutex_task1, "mtx1", 256, &s_mutex_data, 3, nullptr);
  xTaskCreate(mutex_task2, "mtx2", 256, &s_mutex_data, 3, nullptr);
  xTaskCreate(queue_producer, "qprod", 256, &s_queue_data, 2, nullptr);
  xTaskCreate(queue_consumer, "qcons", 256, &s_queue_data, 3, nullptr);
  xTaskCreate(eg_task1, "eg1", 256, &s_eg_data, 3, nullptr);
  xTaskCreate(eg_task2, "eg2", 256, &s_eg_data, 3, nullptr);
  xTaskCreate(mb_sender, "mbsnd", 256, &s_mb_data, 3, nullptr);
  xTaskCreate(mb_receiver, "mbrcv", 256, &s_mb_data, 2, nullptr);
  xTaskCreate(sb_sender, "sbsnd", 256, &s_sb_data, 3, nullptr);
  xTaskCreate(sb_receiver, "sbrcv", 256, &s_sb_data, 2, nullptr);
  xTaskCreate(notifier_task, "notif", 256, s_orch_handle, 2, nullptr);

  vTaskStartScheduler();

  EXPECT_TRUE(results.binary_sem_ok) << "Binary semaphore";
  EXPECT_TRUE(results.counting_sem_ok) << "Counting semaphore";
  EXPECT_TRUE(results.mutex_contention_ok) << "Mutex contention";
  EXPECT_TRUE(results.recursive_mutex_ok) << "Recursive mutex";
  EXPECT_TRUE(results.queue_prod_cons_ok) << "Queue producer/consumer";
  EXPECT_TRUE(results.event_group_sync_ok) << "Event group sync";
  EXPECT_TRUE(results.timer_callback_ok) << "Timer callback";
  EXPECT_TRUE(results.task_notification_ok) << "Task notification";
  EXPECT_TRUE(results.message_buffer_ok) << "Message buffer";
  EXPECT_TRUE(results.stream_buffer_ok) << "Stream buffer";
  EXPECT_TRUE(results.lock_guard_ok) << "Lock guard RAII";
  EXPECT_TRUE(results.move_semantics_ok) << "Move/swap semantics";
  EXPECT_TRUE(results.all_done) << "All tests completed";
}