#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#ifndef FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
#define FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION 1
#endif

#ifndef configNUM_THREAD_LOCAL_STORAGE_POINTERS
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 4
#endif

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

namespace test_ext {

typedef struct test_libcpp_mutex_t {
  StaticSemaphore_t storage;
  SemaphoreHandle_t handle;
  bool initialized;
} test_libcpp_mutex_t;

typedef struct test_libcpp_recursive_mutex_t {
  StaticSemaphore_t storage;
  SemaphoreHandle_t handle;
  bool initialized;
} test_libcpp_recursive_mutex_t;

typedef struct test_libcpp_condvar_t {
  SemaphoreHandle_t signal;
  volatile int waiter_count;
} test_libcpp_condvar_t;

typedef volatile uint8_t test_libcpp_exec_once_flag;
typedef TaskHandle_t test_libcpp_thread_id;
typedef TaskHandle_t test_libcpp_thread_t;
typedef uint32_t test_libcpp_tls_key;

static int test_libcpp_mutex_lock(test_libcpp_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle = xSemaphoreCreateMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateMutex();
#endif
    mutex->initialized = true;
  }
  return xSemaphoreTake(mutex->handle, portMAX_DELAY) == pdTRUE ? 0 : 1;
}

static bool test_libcpp_mutex_trylock(test_libcpp_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle = xSemaphoreCreateMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateMutex();
#endif
    mutex->initialized = true;
  }
  return xSemaphoreTake(mutex->handle, 0) == pdTRUE;
}

static int test_libcpp_mutex_unlock(test_libcpp_mutex_t *mutex) {
  return xSemaphoreGive(mutex->handle) == pdTRUE ? 0 : 1;
}

static int test_libcpp_mutex_destroy(test_libcpp_mutex_t *mutex) {
  if (mutex->initialized && mutex->handle) {
    vSemaphoreDelete(mutex->handle);
    mutex->handle = nullptr;
    mutex->initialized = false;
  }
  return 0;
}

static int
test_libcpp_recursive_mutex_init(test_libcpp_recursive_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle =
        xSemaphoreCreateRecursiveMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateRecursiveMutex();
#endif
    mutex->initialized = true;
  }
  return 0;
}

static int
test_libcpp_recursive_mutex_lock(test_libcpp_recursive_mutex_t *mutex) {
  if (!mutex->initialized) {
    test_libcpp_recursive_mutex_init(mutex);
  }
  return xSemaphoreTakeRecursive(mutex->handle, portMAX_DELAY) == pdTRUE
             ? 0
             : 1;
}

static bool
test_libcpp_recursive_mutex_trylock(test_libcpp_recursive_mutex_t *mutex) {
  if (!mutex->initialized) {
    test_libcpp_recursive_mutex_init(mutex);
  }
  return xSemaphoreTakeRecursive(mutex->handle, 0) == pdTRUE;
}

static int
test_libcpp_recursive_mutex_unlock(test_libcpp_recursive_mutex_t *mutex) {
  return xSemaphoreGiveRecursive(mutex->handle) == pdTRUE ? 0 : 1;
}

static int
test_libcpp_recursive_mutex_destroy(test_libcpp_recursive_mutex_t *mutex) {
  if (mutex->initialized && mutex->handle) {
    vSemaphoreDelete(mutex->handle);
    mutex->handle = nullptr;
    mutex->initialized = false;
  }
  return 0;
}

static int test_libcpp_condvar_signal(test_libcpp_condvar_t *cv) {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  if (cv->waiter_count > 0) {
    xSemaphoreGive(cv->signal);
  }
  return 0;
}

static int test_libcpp_condvar_broadcast(test_libcpp_condvar_t *cv) {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  int count = cv->waiter_count;
  for (int i = 0; i < count; i++) {
    xSemaphoreGive(cv->signal);
  }
  return 0;
}

static int test_libcpp_condvar_destroy(test_libcpp_condvar_t *cv) {
  if (cv->signal) {
    vSemaphoreDelete(cv->signal);
    cv->signal = nullptr;
  }
  return 0;
}

static void
test_libcpp_execute_once(test_libcpp_exec_once_flag *flag,
                         void (*func)(void *)) {
  if (__atomic_load_n(flag, __ATOMIC_ACQUIRE) == 2) {
    return;
  }
  taskENTER_CRITICAL();
  if (*flag == 0) {
    *flag = 1;
    taskEXIT_CRITICAL();
    func(nullptr);
    __atomic_store_n(flag, 2, __ATOMIC_RELEASE);
  } else {
    taskEXIT_CRITICAL();
  }
}

static bool test_libcpp_thread_create(test_libcpp_thread_t *thread,
                                      void *(*func)(void *), void *args) {
  (void)func;
  (void)args;
  *thread = nullptr;
  BaseType_t result = xTaskCreate(nullptr, "lcxx", configMINIMAL_STACK_SIZE,
                                  nullptr, 2, thread);
  return result == pdPASS;
}

static bool test_libcpp_thread_join(test_libcpp_thread_t thread,
                                    void **value_ptr) {
  (void)value_ptr;
  eTaskState state;
  do {
    state = eTaskGetState(thread);
    if (state == eDeleted) {
      break;
    }
    vTaskDelay(1);
  } while (1);
  vTaskDelete(thread);
  return true;
}

static bool test_libcpp_thread_detach(test_libcpp_thread_t thread) {
  (void)thread;
  return true;
}

[[maybe_unused]] static void test_libcpp_thread_yield() { taskYIELD(); }

static test_libcpp_thread_id
test_libcpp_thread_get_id(test_libcpp_thread_t thread) {
  return thread;
}

static test_libcpp_thread_id test_libcpp_thread_get_current_id() {
  return xTaskGetCurrentTaskHandle();
}

static bool test_libcpp_thread_id_equal(test_libcpp_thread_id t1,
                                        test_libcpp_thread_id t2) {
  return t1 == t2;
}

static bool test_libcpp_thread_id_less(test_libcpp_thread_id t1,
                                       test_libcpp_thread_id t2) {
  return t1 < t2;
}

static bool test_libcpp_thread_isnull(test_libcpp_thread_t thread) {
  return thread == nullptr;
}

static int test_libcpp_tls_create(test_libcpp_tls_key *key,
                                  void (*dtor)(void *)) {
  (void)dtor;
  static test_libcpp_tls_key next_key = 0;
  if (next_key >= configNUM_THREAD_LOCAL_STORAGE_POINTERS) {
    return 1;
  }
  *key = next_key++;
  return 0;
}

static void *test_libcpp_tls_get(test_libcpp_tls_key key) {
  return pvTaskGetThreadLocalStoragePointer(nullptr,
                                             static_cast<BaseType_t>(key));
}

static void test_libcpp_tls_set(test_libcpp_tls_key key, void *ptr) {
  vTaskSetThreadLocalStoragePointer(nullptr, static_cast<BaseType_t>(key),
                                    ptr);
}

} // namespace test_ext

class ExtThreadingMutexTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExtThreadingMutexTest, LazyInit) {
  test_ext::test_libcpp_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_ext::test_libcpp_mutex_lock(&m), 0);
  EXPECT_TRUE(m.initialized);
  EXPECT_EQ(m.handle, h);
}

TEST_F(ExtThreadingMutexTest, LockNoReinit) {
  test_ext::test_libcpp_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, xSemaphoreTake(h, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_ext::test_libcpp_mutex_lock(&m), 0);
  EXPECT_TRUE(m.initialized);
}

TEST_F(ExtThreadingMutexTest, TrylockSuccess) {
  test_ext::test_libcpp_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdTRUE));
  EXPECT_TRUE(test_ext::test_libcpp_mutex_trylock(&m));
}

TEST_F(ExtThreadingMutexTest, TrylockFailure) {
  test_ext::test_libcpp_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdFALSE));
  EXPECT_FALSE(test_ext::test_libcpp_mutex_trylock(&m));
}

TEST_F(ExtThreadingMutexTest, UnlockSuccess) {
  test_ext::test_libcpp_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_ext::test_libcpp_mutex_unlock(&m), 0);
}

TEST_F(ExtThreadingMutexTest, UnlockFailure) {
  test_ext::test_libcpp_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdFALSE));
  EXPECT_EQ(test_ext::test_libcpp_mutex_unlock(&m), 1);
}

TEST_F(ExtThreadingMutexTest, Destroy) {
  test_ext::test_libcpp_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  EXPECT_EQ(test_ext::test_libcpp_mutex_destroy(&m), 0);
  EXPECT_FALSE(m.initialized);
  EXPECT_EQ(m.handle, nullptr);
}

TEST_F(ExtThreadingMutexTest, DestroyUninitialized) {
  test_ext::test_libcpp_mutex_t m = {{}, nullptr, false};
  EXPECT_EQ(test_ext::test_libcpp_mutex_destroy(&m), 0);
}

class ExtThreadingRecMutexTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5101);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExtThreadingRecMutexTest, InitThenLock) {
  test_ext::test_libcpp_recursive_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_EQ(test_ext::test_libcpp_recursive_mutex_init(&m), 0);
  EXPECT_TRUE(m.initialized);

  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_ext::test_libcpp_recursive_mutex_lock(&m), 0);
}

TEST_F(ExtThreadingRecMutexTest, InitNoReinit) {
  test_ext::test_libcpp_recursive_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_ext::test_libcpp_recursive_mutex_lock(&m), 0);
}

TEST_F(ExtThreadingRecMutexTest, TrylockSuccess) {
  test_ext::test_libcpp_recursive_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_TRUE(test_ext::test_libcpp_recursive_mutex_trylock(&m));
}

TEST_F(ExtThreadingRecMutexTest, TrylockFailure) {
  test_ext::test_libcpp_recursive_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_FALSE(test_ext::test_libcpp_recursive_mutex_trylock(&m));
}

TEST_F(ExtThreadingRecMutexTest, UnlockSuccess) {
  test_ext::test_libcpp_recursive_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(h)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_ext::test_libcpp_recursive_mutex_unlock(&m), 0);
}

TEST_F(ExtThreadingRecMutexTest, DestroyWithHandle) {
  test_ext::test_libcpp_recursive_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  EXPECT_EQ(test_ext::test_libcpp_recursive_mutex_destroy(&m), 0);
  EXPECT_FALSE(m.initialized);
}

class ExtThreadingCondvarTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t sig_h = reinterpret_cast<SemaphoreHandle_t>(0x6001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExtThreadingCondvarTest, SignalNoWaiters) {
  test_ext::test_libcpp_condvar_t cv = {sig_h, 0};
  EXPECT_EQ(test_ext::test_libcpp_condvar_signal(&cv), 0);
}

TEST_F(ExtThreadingCondvarTest, SignalWithWaiters) {
  test_ext::test_libcpp_condvar_t cv = {sig_h, 1};
  EXPECT_CALL(*mock, xSemaphoreGive(sig_h)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_ext::test_libcpp_condvar_signal(&cv), 0);
}

TEST_F(ExtThreadingCondvarTest, BroadcastWithWaiters) {
  test_ext::test_libcpp_condvar_t cv = {sig_h, 2};
  EXPECT_CALL(*mock, xSemaphoreGive(sig_h)).Times(2).WillRepeatedly(Return(pdTRUE));
  EXPECT_EQ(test_ext::test_libcpp_condvar_broadcast(&cv), 0);
}

TEST_F(ExtThreadingCondvarTest, BroadcastNoWaiters) {
  test_ext::test_libcpp_condvar_t cv = {sig_h, 0};
  EXPECT_EQ(test_ext::test_libcpp_condvar_broadcast(&cv), 0);
}

TEST_F(ExtThreadingCondvarTest, DestroyWithSignal) {
  test_ext::test_libcpp_condvar_t cv = {sig_h, 0};
  EXPECT_CALL(*mock, vSemaphoreDelete(sig_h));
  EXPECT_EQ(test_ext::test_libcpp_condvar_destroy(&cv), 0);
  EXPECT_EQ(cv.signal, nullptr);
}

TEST_F(ExtThreadingCondvarTest, DestroyNullSignal) {
  test_ext::test_libcpp_condvar_t cv = {nullptr, 0};
  EXPECT_EQ(test_ext::test_libcpp_condvar_destroy(&cv), 0);
}

class ExtThreadingOnceTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExtThreadingOnceTest, CallsFuncWhenZero) {
  test_ext::test_libcpp_exec_once_flag flag = 0;
  auto func = [](void *) { };
  test_ext::test_libcpp_execute_once(&flag, func);
}

TEST_F(ExtThreadingOnceTest, SkipsWhenAlreadyTwo) {
  test_ext::test_libcpp_exec_once_flag flag = 2;
  test_ext::test_libcpp_execute_once(&flag, [](void *) {});
}

class ExtThreadingThreadTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExtThreadingThreadTest, CreateSuccess) {
  test_ext::test_libcpp_thread_t thread = nullptr;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(Return(pdPASS));
  EXPECT_TRUE(test_ext::test_libcpp_thread_create(
      &thread, reinterpret_cast<void *(*)(void *)>(1), nullptr));
}

TEST_F(ExtThreadingThreadTest, CreateFailure) {
  test_ext::test_libcpp_thread_t thread = nullptr;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_FALSE(test_ext::test_libcpp_thread_create(
      &thread, reinterpret_cast<void *(*)(void *)>(1), nullptr));
}

TEST_F(ExtThreadingThreadTest, JoinWaitsForDeleted) {
  test_ext::test_libcpp_thread_t thread = reinterpret_cast<TaskHandle_t>(0x7001);
  EXPECT_CALL(*mock, eTaskGetState(thread))
      .WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, vTaskDelete(thread));
  EXPECT_TRUE(test_ext::test_libcpp_thread_join(thread, nullptr));
}

TEST_F(ExtThreadingThreadTest, JoinLoopsUntilDeleted) {
  test_ext::test_libcpp_thread_t thread = reinterpret_cast<TaskHandle_t>(0x7002);
  EXPECT_CALL(*mock, eTaskGetState(thread))
      .WillOnce(Return(eRunning))
      .WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, vTaskDelay(1));
  EXPECT_CALL(*mock, vTaskDelete(thread));
  EXPECT_TRUE(test_ext::test_libcpp_thread_join(thread, nullptr));
}

TEST_F(ExtThreadingThreadTest, DetachAlwaysTrue) {
  EXPECT_TRUE(test_ext::test_libcpp_thread_detach(nullptr));
}

TEST_F(ExtThreadingThreadTest, ThreadIsNull) {
  EXPECT_TRUE(test_ext::test_libcpp_thread_isnull(nullptr));
  EXPECT_FALSE(test_ext::test_libcpp_thread_isnull(
      reinterpret_cast<TaskHandle_t>(1)));
}

TEST_F(ExtThreadingThreadTest, IdEqual) {
  TaskHandle_t t1 = reinterpret_cast<TaskHandle_t>(0x7101);
  EXPECT_TRUE(test_ext::test_libcpp_thread_id_equal(t1, t1));
  EXPECT_FALSE(test_ext::test_libcpp_thread_id_equal(
      t1, reinterpret_cast<TaskHandle_t>(0x7102)));
}

TEST_F(ExtThreadingThreadTest, IdLess) {
  TaskHandle_t t1 = reinterpret_cast<TaskHandle_t>(0x7101);
  TaskHandle_t t2 = reinterpret_cast<TaskHandle_t>(0x7102);
  EXPECT_TRUE(test_ext::test_libcpp_thread_id_less(t1, t2));
  EXPECT_FALSE(test_ext::test_libcpp_thread_id_less(t2, t1));
}

TEST_F(ExtThreadingThreadTest, GetCurrentId) {
  TaskHandle_t self = reinterpret_cast<TaskHandle_t>(0x7201);
  EXPECT_CALL(*mock, xTaskGetCurrentTaskHandle()).WillOnce(Return(self));
  EXPECT_EQ(test_ext::test_libcpp_thread_get_current_id(), self);
}

TEST_F(ExtThreadingThreadTest, GetId) {
  TaskHandle_t t = reinterpret_cast<TaskHandle_t>(0x7202);
  EXPECT_EQ(test_ext::test_libcpp_thread_get_id(t), t);
}

class ExtThreadingTlsTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(ExtThreadingTlsTest, CreateKey) {
  test_ext::test_libcpp_tls_key key;
  EXPECT_EQ(test_ext::test_libcpp_tls_create(&key, nullptr), 0);
}

TEST_F(ExtThreadingTlsTest, TlsSetAndGet) {
  test_ext::test_libcpp_tls_key key = 0;
  void *ptr = reinterpret_cast<void *>(0x8001);
  EXPECT_CALL(*mock,
              vTaskSetThreadLocalStoragePointer(nullptr, 0, ptr));
  test_ext::test_libcpp_tls_set(key, ptr);

  EXPECT_CALL(*mock,
              pvTaskGetThreadLocalStoragePointer(nullptr, 0))
      .WillOnce(Return(ptr));
  EXPECT_EQ(test_ext::test_libcpp_tls_get(key), ptr);
}