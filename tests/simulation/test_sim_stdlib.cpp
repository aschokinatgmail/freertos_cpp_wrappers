#include <FreeRTOS.h>
#include <gtest/gtest.h>
#include <semphr.h>
#include <task.h>

#include <atomic>

struct StdlibResults {
  std::atomic<bool> mutex_lazy_init;
  std::atomic<bool> mutex_lock_unlock;
  std::atomic<bool> mutex_trylock;
  std::atomic<bool> recursive_mutex;
  std::atomic<bool> condvar_counting_sem;
  std::atomic<bool> condvar_task_notify;
  std::atomic<bool> once_flag;
  std::atomic<bool> thread_create_join;
  std::atomic<bool> tls;
  std::atomic<bool> all_done;

  void reset() {
    mutex_lazy_init.store(false);
    mutex_lock_unlock.store(false);
    mutex_trylock.store(false);
    recursive_mutex.store(false);
    condvar_counting_sem.store(false);
    condvar_task_notify.store(false);
    once_flag.store(false);
    thread_create_join.store(false);
    tls.store(false);
    all_done.store(false);
  }
};

static StdlibResults s_stdlib;

struct freertos_mutex_t {
  StaticSemaphore_t storage;
  SemaphoreHandle_t handle;
  bool initialized;
};

struct freertos_recursive_mutex_t {
  StaticSemaphore_t storage;
  SemaphoreHandle_t handle;
  bool initialized;
};

struct freertos_cond_t_counting {
  SemaphoreHandle_t signal;
  volatile int waiter_count;
};

struct freertos_cond_t_notify {
  TaskHandle_t waiting_task;
  volatile uint8_t signaled;
};

static int freertos_mutex_lock(freertos_mutex_t *m) {
  if (!m->initialized) {
    m->handle = xSemaphoreCreateMutexStatic(&m->storage);
    m->initialized = true;
  }
  return xSemaphoreTake(m->handle, portMAX_DELAY) == pdTRUE ? 0 : 1;
}

static int freertos_mutex_trylock(freertos_mutex_t *m) {
  if (!m->initialized) {
    m->handle = xSemaphoreCreateMutexStatic(&m->storage);
    m->initialized = true;
  }
  return xSemaphoreTake(m->handle, 0) == pdTRUE ? 0 : 1;
}

static int freertos_mutex_unlock(freertos_mutex_t *m) {
  return xSemaphoreGive(m->handle) == pdTRUE ? 0 : 1;
}

static int freertos_recursive_mutex_lock(freertos_recursive_mutex_t *m) {
  if (!m->initialized) {
    m->handle = xSemaphoreCreateRecursiveMutexStatic(&m->storage);
    m->initialized = true;
  }
  return xSemaphoreTakeRecursive(m->handle, portMAX_DELAY) == pdTRUE ? 0 : 1;
}

static int freertos_recursive_mutex_unlock(freertos_recursive_mutex_t *m) {
  return xSemaphoreGiveRecursive(m->handle) == pdTRUE ? 0 : 1;
}

static volatile uint8_t test_once_flag = 0;
static std::atomic<int> once_call_count{0};

void test_once_func(void) { once_call_count++; }

void worker_mutex_contention(void *param) {
  auto *m = static_cast<freertos_mutex_t *>(param);
  for (int i = 0; i < 10; ++i) {
    if (freertos_mutex_lock(m) == 0) {
      vTaskDelay(1);
      freertos_mutex_unlock(m);
      vTaskDelay(1);
    }
  }
  vTaskDelete(nullptr);
}

void worker_condvar_waiter(void *param) {
  auto *cv = static_cast<freertos_cond_t_counting *>(param);
  cv->waiter_count++;
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  xSemaphoreTake(cv->signal, portMAX_DELAY);
  cv->waiter_count--;
  vTaskDelete(nullptr);
}

void worker_condvar_notify_waiter(void *param) {
  auto *cv = static_cast<freertos_cond_t_notify *>(param);
  cv->waiting_task = xTaskGetCurrentTaskHandle();
  cv->signaled = 0;
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  cv->signaled = 1;
  vTaskDelete(nullptr);
}

struct ThreadJoinData {
  TaskHandle_t handle;
  std::atomic<bool> started;
  std::atomic<bool> finished;

  void reset() {
    handle = nullptr;
    started.store(false);
    finished.store(false);
  }
};

static ThreadJoinData s_join_data;

void worker_thread_fn(void *param) {
  auto *d = static_cast<ThreadJoinData *>(param);
  d->started.store(true);
  vTaskDelay(pdMS_TO_TICKS(50));
  d->finished.store(true);
  vTaskDelete(nullptr);
}

void worker_tls(void *param) {
  auto *ok = static_cast<std::atomic<bool> *>(param);
  vTaskSetThreadLocalStoragePointer(nullptr, 0,
                                    reinterpret_cast<void *>(0xABCDEF01));
  void *ptr = pvTaskGetThreadLocalStoragePointer(nullptr, 0);
  ok->store(ptr == reinterpret_cast<void *>(0xABCDEF01));
  vTaskDelete(nullptr);
}

void stdlib_orchestrator(void *param) {
  auto *r = static_cast<StdlibResults *>(param);

  {
    freertos_mutex_t m = {{}, nullptr, false};
    r->mutex_lazy_init.store(
        freertos_mutex_lock(&m) == 0 && m.initialized && m.handle != nullptr);
    freertos_mutex_unlock(&m);
    vSemaphoreDelete(m.handle);
  }

  {
    freertos_mutex_t m = {{}, nullptr, false};
    int lock_ok = freertos_mutex_lock(&m);
    int unlock_ok = freertos_mutex_unlock(&m);
    r->mutex_lock_unlock.store(lock_ok == 0 && unlock_ok == 0);
    vSemaphoreDelete(m.handle);
  }

  {
    freertos_mutex_t m = {{}, nullptr, false};
    freertos_mutex_lock(&m);
    int trylock_held = freertos_mutex_trylock(&m);
    freertos_mutex_unlock(&m);
    int trylock_free = freertos_mutex_trylock(&m);
    freertos_mutex_unlock(&m);
    r->mutex_trylock.store(trylock_held != 0 && trylock_free == 0);
    vSemaphoreDelete(m.handle);
  }

  {
    std::atomic<bool> w1_ok{false};
    std::atomic<bool> w2_ok{false};
    freertos_mutex_t m = {{}, nullptr, false};
    struct P {
      freertos_mutex_t *m;
      std::atomic<bool> *ok;
    };
    P p1{&m, &w1_ok};
    P p2{&m, &w2_ok};
    auto worker1 = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      for (int i = 0; i < 10; ++i) {
        if (freertos_mutex_lock(pp->m) == 0) {
          vTaskDelay(1);
          freertos_mutex_unlock(pp->m);
        }
      }
      pp->ok->store(true);
      vTaskDelete(nullptr);
    };
    auto worker2 = [](void *pv) {
      auto *pp = static_cast<P *>(pv);
      for (int i = 0; i < 10; ++i) {
        if (freertos_mutex_lock(pp->m) == 0) {
          vTaskDelay(1);
          freertos_mutex_unlock(pp->m);
        }
      }
      pp->ok->store(true);
      vTaskDelete(nullptr);
    };
    xTaskCreate(worker1, "mw1", 512, &p1, 2, nullptr);
    xTaskCreate(worker2, "mw2", 512, &p2, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(500));
    vSemaphoreDelete(m.handle);
  }

  {
    freertos_recursive_mutex_t m = {{}, nullptr, false};
    int lock1 = freertos_recursive_mutex_lock(&m);
    int lock2 = freertos_recursive_mutex_lock(&m);
    int unlock1 = freertos_recursive_mutex_unlock(&m);
    int unlock2 = freertos_recursive_mutex_unlock(&m);
    r->recursive_mutex.store(lock1 == 0 && lock2 == 0 && unlock1 == 0 &&
                             unlock2 == 0);
    vSemaphoreDelete(m.handle);
  }

  {
    SemaphoreHandle_t sig = xSemaphoreCreateCounting(2, 0);
    freertos_cond_t_counting cv = {sig, 0};
    xTaskCreate(worker_condvar_waiter, "cw1", 512, &cv, 2, nullptr);
    xTaskCreate(worker_condvar_waiter, "cw2", 512, &cv, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(100));
    int wc = cv.waiter_count;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    if (wc > 0) {
      xSemaphoreGive(cv.signal);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    r->condvar_counting_sem.store(wc >= 1);
    vSemaphoreDelete(cv.signal);
  }

  {
    freertos_cond_t_notify cv = {nullptr, 0};
    xTaskCreate(worker_condvar_notify_waiter, "nw", 512, &cv, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(100));
    if (cv.waiting_task) {
      xTaskNotifyGive(cv.waiting_task);
    }
    vTaskDelay(pdMS_TO_TICKS(200));
    r->condvar_task_notify.store(cv.signaled == 1);
  }

  {
    test_once_flag = 0;
    once_call_count.store(0);
    if (__atomic_load_n(&test_once_flag, __ATOMIC_ACQUIRE) == 2) {
    } else {
      taskENTER_CRITICAL();
      if (test_once_flag == 0) {
        test_once_flag = 1;
        taskEXIT_CRITICAL();
        test_once_func();
        __atomic_store_n(&test_once_flag, 2, __ATOMIC_RELEASE);
      } else {
        taskEXIT_CRITICAL();
      }
    }
    bool first_call = (once_call_count.load() == 1) && (test_once_flag == 2);

    if (__atomic_load_n(&test_once_flag, __ATOMIC_ACQUIRE) == 2) {
    } else {
      taskENTER_CRITICAL();
      if (test_once_flag == 0) {
        test_once_flag = 1;
        taskEXIT_CRITICAL();
        test_once_func();
        __atomic_store_n(&test_once_flag, 2, __ATOMIC_RELEASE);
      } else {
        taskEXIT_CRITICAL();
      }
    }
    bool second_call_skipped = (once_call_count.load() == 1);
    r->once_flag.store(first_call && second_call_skipped);
  }

  {
    s_join_data.reset();
    xTaskCreate(worker_thread_fn, "tj", 512, &s_join_data, 2,
                &s_join_data.handle);
    vTaskDelay(pdMS_TO_TICKS(200));
    bool join_ok = s_join_data.started && s_join_data.finished;
    r->thread_create_join.store(join_ok);
  }

  {
    std::atomic<bool> tls_ok{false};
    xTaskCreate(worker_tls, "tls", 512, &tls_ok, 2, nullptr);
    vTaskDelay(pdMS_TO_TICKS(200));
    r->tls.store(tls_ok);
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimStdlib, AllStdlibTests) {
  s_stdlib.reset();

  xTaskCreate(stdlib_orchestrator, "stdorch", 2048, &s_stdlib, 1, nullptr);

  vTaskStartScheduler();

  EXPECT_TRUE(s_stdlib.mutex_lazy_init) << "Mutex lazy init";
  EXPECT_TRUE(s_stdlib.mutex_lock_unlock) << "Mutex lock/unlock";
  EXPECT_TRUE(s_stdlib.mutex_trylock) << "Mutex trylock";
  EXPECT_TRUE(s_stdlib.recursive_mutex) << "Recursive mutex";
  EXPECT_TRUE(s_stdlib.condvar_counting_sem) << "Condvar counting semaphore";
  EXPECT_TRUE(s_stdlib.condvar_task_notify) << "Condvar task notification";
  EXPECT_TRUE(s_stdlib.once_flag) << "Once flag state machine";
  EXPECT_TRUE(s_stdlib.thread_create_join) << "Thread create/join";
  EXPECT_TRUE(s_stdlib.tls) << "TLS get/set";
  EXPECT_TRUE(s_stdlib.all_done) << "All tests completed";
}