#include <freertos.hpp>
#include <gtest/gtest.h>

#include <atomic>

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

struct MutexResults {
  std::atomic<bool> lock_unlock;
  std::atomic<bool> contention;
  std::atomic<bool> try_lock_when_held;
  std::atomic<bool> recursive_lock;
  std::atomic<bool> recursive_unlock;
  std::atomic<bool> lock_guard_raii;
  std::atomic<bool> all_done;

  void reset() {
    lock_unlock.store(false);
    contention.store(false);
    try_lock_when_held.store(false);
    recursive_lock.store(false);
    recursive_unlock.store(false);
    lock_guard_raii.store(false);
    all_done.store(false);
  }
};

static MutexData s_mutex_data;
static MutexResults s_mutex;

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

void mutex_orchestrator(void *param) {
  auto *r = static_cast<MutexResults *>(param);

  r->lock_unlock.store(false);
  r->contention.store(false);
  r->try_lock_when_held.store(false);
  r->recursive_lock.store(false);
  r->recursive_unlock.store(false);
  r->lock_guard_raii.store(false);

  {
    freertos::da::mutex m;
    r->lock_unlock.store((m.lock() == pdTRUE) && m.locked());
    m.unlock();
    r->lock_unlock.store(r->lock_unlock && !m.locked());
  }

  r->contention.store(s_mutex_data.t1_ok && s_mutex_data.t2_ok &&
                      (s_mutex_data.counter == 100));

  {
    freertos::da::mutex m;
    m.lock();
    r->try_lock_when_held.store(m.try_lock() != pdTRUE);
    m.unlock();
  }

  {
    freertos::da::recursive_mutex rm;
    r->recursive_lock.store((rm.lock() == pdTRUE) && rm.locked() &&
                            (rm.recursions_count() == 1));
    r->recursive_lock.store(r->recursive_lock && (rm.lock() == pdTRUE) &&
                            (rm.recursions_count() == 2));
    r->recursive_unlock.store((rm.unlock() == pdTRUE) &&
                              (rm.recursions_count() == 1));
    r->recursive_unlock.store(r->recursive_unlock && (rm.unlock() == pdTRUE) &&
                              !rm.locked() && (rm.recursions_count() == 0));
  }

  {
    freertos::da::mutex guard_mtx;
    r->lock_guard_raii.store(true);
    {
      freertos::lock_guard<freertos::da::mutex> lg(guard_mtx);
      r->lock_guard_raii.store(r->lock_guard_raii && guard_mtx.locked());
    }
    r->lock_guard_raii.store(r->lock_guard_raii && !guard_mtx.locked());
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimMutex, AllMutexTests) {
  s_mutex.reset();
  s_mutex_data.reset();

  xTaskCreate(mutex_task1, "mtx1", 256, &s_mutex_data, 3, nullptr);
  xTaskCreate(mutex_task2, "mtx2", 256, &s_mutex_data, 3, nullptr);
  xTaskCreate(mutex_orchestrator, "morch", 2048, &s_mutex, 1, nullptr);

  vTaskStartScheduler();

  EXPECT_TRUE(s_mutex.lock_unlock) << "Mutex lock/unlock";
  EXPECT_TRUE(s_mutex.contention) << "Mutex contention";
  EXPECT_TRUE(s_mutex.try_lock_when_held) << "Try_lock when held";
  EXPECT_TRUE(s_mutex.recursive_lock) << "Recursive mutex lock";
  EXPECT_TRUE(s_mutex.recursive_unlock) << "Recursive mutex unlock";
  EXPECT_TRUE(s_mutex.lock_guard_raii) << "Lock guard RAII";
  EXPECT_TRUE(s_mutex.all_done) << "All tests completed";
}