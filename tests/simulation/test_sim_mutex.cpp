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

struct MutexData {
  freertos::da::mutex mtx;
  volatile int counter;
  volatile bool t1_ok;
  volatile bool t2_ok;
};

struct MutexResults {
  volatile bool lock_unlock;
  volatile bool contention;
  volatile bool try_lock_when_held;
  volatile bool recursive_lock;
  volatile bool recursive_unlock;
  volatile bool lock_guard_raii;
  volatile bool all_done;
};

static MutexData s_mutex_data{};
static MutexResults s_mutex{};

void mutex_task1(void *param) {
  auto *d = static_cast<MutexData *>(param);
  for (int i = 0; i < 50; ++i) {
    if (d->mtx.lock(pdMS_TO_TICKS(100)) == pdTRUE) {
      d->counter++;
      d->mtx.unlock();
    } else {
      d->t1_ok = false;
      vTaskDelete(nullptr);
      return;
    }
  }
  d->t1_ok = true;
  vTaskDelete(nullptr);
}

void mutex_task2(void *param) {
  auto *d = static_cast<MutexData *>(param);
  for (int i = 0; i < 50; ++i) {
    if (d->mtx.lock(pdMS_TO_TICKS(100)) == pdTRUE) {
      d->counter++;
      d->mtx.unlock();
    } else {
      d->t2_ok = false;
      vTaskDelete(nullptr);
      return;
    }
  }
  d->t2_ok = true;
  vTaskDelete(nullptr);
}

void mutex_orchestrator(void *param) {
  auto *r = static_cast<MutexResults *>(param);

  r->lock_unlock = false;
  r->contention = false;
  r->try_lock_when_held = false;
  r->recursive_lock = false;
  r->recursive_unlock = false;
  r->lock_guard_raii = false;

  {
    freertos::da::mutex m;
    r->lock_unlock = (m.lock() == pdTRUE) && m.locked();
    m.unlock();
    r->lock_unlock = r->lock_unlock && !m.locked();
  }

  r->contention = s_mutex_data.t1_ok && s_mutex_data.t2_ok &&
                  (s_mutex_data.counter == 100);

  {
    freertos::da::mutex m;
    m.lock();
    r->try_lock_when_held = (m.try_lock() != pdTRUE);
    m.unlock();
  }

  {
    freertos::da::recursive_mutex rm;
    r->recursive_lock = (rm.lock() == pdTRUE) && rm.locked() &&
                        (rm.recursions_count() == 1);
    r->recursive_lock = r->recursive_lock && (rm.lock() == pdTRUE) &&
                        (rm.recursions_count() == 2);
    r->recursive_unlock = (rm.unlock() == pdTRUE) &&
                          (rm.recursions_count() == 1);
    r->recursive_unlock = r->recursive_unlock && (rm.unlock() == pdTRUE) &&
                          !rm.locked() && (rm.recursions_count() == 0);
  }

  {
    freertos::da::mutex guard_mtx;
    r->lock_guard_raii = true;
    {
      freertos::lock_guard<freertos::da::mutex> lg(guard_mtx);
      r->lock_guard_raii = r->lock_guard_raii && guard_mtx.locked();
    }
    r->lock_guard_raii = r->lock_guard_raii && !guard_mtx.locked();
  }

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimMutex, AllMutexTests) {
  memset(&s_mutex, 0, sizeof(s_mutex));
  s_mutex_data.counter = 0;
  s_mutex_data.t1_ok = false;
  s_mutex_data.t2_ok = false;

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