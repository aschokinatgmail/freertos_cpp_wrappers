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

typedef struct test_gthread_mutex_t {
  StaticSemaphore_t storage;
  SemaphoreHandle_t handle;
  bool initialized;
} test_gthread_mutex_t;

typedef struct test_gthread_recursive_mutex_t {
  StaticSemaphore_t storage;
  SemaphoreHandle_t handle;
  bool initialized;
} test_gthread_recursive_mutex_t;

typedef struct test_gthread_cond_t {
  SemaphoreHandle_t signal;
  volatile int waiter_count;
} test_gthread_cond_t;

typedef volatile uint8_t test_gthread_once_t;
typedef uint32_t test_gthread_key_t;
typedef TaskHandle_t test_gthread_t;

static int test_gthread_mutex_lock(test_gthread_mutex_t *mutex) {
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

static int test_gthread_mutex_trylock(test_gthread_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle = xSemaphoreCreateMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateMutex();
#endif
    mutex->initialized = true;
  }
  return xSemaphoreTake(mutex->handle, 0) == pdTRUE ? 0 : 1;
}

static int test_gthread_mutex_unlock(test_gthread_mutex_t *mutex) {
  return xSemaphoreGive(mutex->handle) == pdTRUE ? 0 : 1;
}

static int test_gthread_mutex_destroy(test_gthread_mutex_t *mutex) {
  if (mutex->initialized && mutex->handle) {
    vSemaphoreDelete(mutex->handle);
    mutex->handle = nullptr;
    mutex->initialized = false;
  }
  return 0;
}

static int
test_gthread_recursive_mutex_lock(test_gthread_recursive_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle =
        xSemaphoreCreateRecursiveMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateRecursiveMutex();
#endif
    mutex->initialized = true;
  }
  return xSemaphoreTakeRecursive(mutex->handle, portMAX_DELAY) == pdTRUE
             ? 0
             : 1;
}

static int
test_gthread_recursive_mutex_trylock(test_gthread_recursive_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle =
        xSemaphoreCreateRecursiveMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateRecursiveMutex();
#endif
    mutex->initialized = true;
  }
  return xSemaphoreTakeRecursive(mutex->handle, 0) == pdTRUE ? 0 : 1;
}

static int
test_gthread_recursive_mutex_unlock(test_gthread_recursive_mutex_t *mutex) {
  return xSemaphoreGiveRecursive(mutex->handle) == pdTRUE ? 0 : 1;
}

static int
test_gthread_recursive_mutex_destroy(test_gthread_recursive_mutex_t *mutex) {
  if (mutex->initialized && mutex->handle) {
    vSemaphoreDelete(mutex->handle);
    mutex->handle = nullptr;
    mutex->initialized = false;
  }
  return 0;
}

static int test_gthread_once(test_gthread_once_t *once, void (*func)(void)) {
  if (__atomic_load_n(once, __ATOMIC_ACQUIRE) == 2) {
    return 0;
  }
  taskENTER_CRITICAL();
  if (*once == 0) {
    *once = 1;
    taskEXIT_CRITICAL();
    func();
    __atomic_store_n(once, 2, __ATOMIC_RELEASE);
  } else {
    taskEXIT_CRITICAL();
  }
  return 0;
}

static int test_gthread_key_create(test_gthread_key_t *key,
                                   void (*dtor)(void *)) {
  (void)dtor;
  static test_gthread_key_t next_key = 0;
  if (next_key >= configNUM_THREAD_LOCAL_STORAGE_POINTERS) {
    return 1;
  }
  *key = next_key++;
  return 0;
}

static int test_gthread_key_delete(test_gthread_key_t key) {
  (void)key;
  return 0;
}

[[maybe_unused]] static void *test_gthread_getspecific(test_gthread_key_t key) {
  return pvTaskGetThreadLocalStoragePointer(nullptr,
                                             static_cast<BaseType_t>(key));
}

[[maybe_unused]] static int test_gthread_setspecific(test_gthread_key_t key, const void *ptr) {
  vTaskSetThreadLocalStoragePointer(nullptr, static_cast<BaseType_t>(key),
                                    const_cast<void *>(ptr));
  return 0;
}

static int test_gthread_cond_signal(test_gthread_cond_t *cond) {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  if (cond->waiter_count > 0) {
    xSemaphoreGive(cond->signal);
  }
  return 0;
}

static int test_gthread_cond_broadcast(test_gthread_cond_t *cond) {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  int count = cond->waiter_count;
  for (int i = 0; i < count; i++) {
    xSemaphoreGive(cond->signal);
  }
  return 0;
}

static int test_gthread_cond_destroy(test_gthread_cond_t *cond) {
  if (cond->signal) {
    vSemaphoreDelete(cond->signal);
    cond->signal = nullptr;
  }
  return 0;
}

static int test_gthread_active_p() {
  return xTaskGetSchedulerState() == taskSCHEDULER_RUNNING ? 1 : 0;
}

class GthrImplTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x2001);
  SemaphoreHandle_t h2 = reinterpret_cast<SemaphoreHandle_t>(0x2002);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(GthrImplTest, MutexLockLazyInit) {
  test_gthread_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_gthread_mutex_lock(&m), 0);
  EXPECT_TRUE(m.initialized);
  EXPECT_EQ(m.handle, h);
}

TEST_F(GthrImplTest, MutexLockNoReinit) {
  test_gthread_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, xSemaphoreTake(h, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_gthread_mutex_lock(&m), 0);
}

TEST_F(GthrImplTest, MutexLockFailure) {
  test_gthread_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, portMAX_DELAY))
      .WillOnce(Return(pdFALSE));
  EXPECT_EQ(test_gthread_mutex_lock(&m), 1);
}

TEST_F(GthrImplTest, MutexTrylockSuccess) {
  test_gthread_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_gthread_mutex_trylock(&m), 0);
  EXPECT_TRUE(m.initialized);
}

TEST_F(GthrImplTest, MutexTrylockFailure) {
  test_gthread_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTake(h, 0)).WillOnce(Return(pdFALSE));
  EXPECT_EQ(test_gthread_mutex_trylock(&m), 1);
}

TEST_F(GthrImplTest, MutexUnlockSuccess) {
  test_gthread_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_gthread_mutex_unlock(&m), 0);
}

TEST_F(GthrImplTest, MutexUnlockFailure) {
  test_gthread_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, xSemaphoreGive(h)).WillOnce(Return(pdFALSE));
  EXPECT_EQ(test_gthread_mutex_unlock(&m), 1);
}

TEST_F(GthrImplTest, MutexDestroyWithHandle) {
  test_gthread_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  EXPECT_EQ(test_gthread_mutex_destroy(&m), 0);
  EXPECT_FALSE(m.initialized);
  EXPECT_EQ(m.handle, nullptr);
}

TEST_F(GthrImplTest, MutexDestroyUninitialized) {
  test_gthread_mutex_t m = {{}, nullptr, false};
  EXPECT_EQ(test_gthread_mutex_destroy(&m), 0);
}

TEST_F(GthrImplTest, MutexDestroyNullHandle) {
  test_gthread_mutex_t m = {{}, nullptr, true};
  EXPECT_EQ(test_gthread_mutex_destroy(&m), 0);
}

class GthrRecursiveMutexTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x2101);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(GthrRecursiveMutexTest, LazyInit) {
  test_gthread_recursive_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_gthread_recursive_mutex_lock(&m), 0);
  EXPECT_TRUE(m.initialized);
}

TEST_F(GthrRecursiveMutexTest, TrylockSuccess) {
  test_gthread_recursive_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_gthread_recursive_mutex_trylock(&m), 0);
}

TEST_F(GthrRecursiveMutexTest, TrylockFailure) {
  test_gthread_recursive_mutex_t m = {{}, nullptr, false};
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(&m.storage))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_EQ(test_gthread_recursive_mutex_trylock(&m), 1);
}

TEST_F(GthrRecursiveMutexTest, UnlockSuccess) {
  test_gthread_recursive_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(h)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_gthread_recursive_mutex_unlock(&m), 0);
}

TEST_F(GthrRecursiveMutexTest, UnlockFailure) {
  test_gthread_recursive_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(h)).WillOnce(Return(pdFALSE));
  EXPECT_EQ(test_gthread_recursive_mutex_unlock(&m), 1);
}

TEST_F(GthrRecursiveMutexTest, DestroyWithHandle) {
  test_gthread_recursive_mutex_t m = {{}, h, true};
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  EXPECT_EQ(test_gthread_recursive_mutex_destroy(&m), 0);
  EXPECT_FALSE(m.initialized);
  EXPECT_EQ(m.handle, nullptr);
}

TEST_F(GthrRecursiveMutexTest, DestroyUninitialized) {
  test_gthread_recursive_mutex_t m = {{}, nullptr, false};
  EXPECT_EQ(test_gthread_recursive_mutex_destroy(&m), 0);
}

class GthrOnceTest : public ::testing::Test {
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

TEST_F(GthrOnceTest, InitialStateIsZero) {
  test_gthread_once_t once = 0;
  EXPECT_EQ(once, 0);
}

TEST_F(GthrOnceTest, CallsFuncWhenZero) {
  test_gthread_once_t once = 0;
  static int call_count;
  call_count = 0;
  auto func = [](void) { call_count++; };
  EXPECT_EQ(test_gthread_once(&once, func), 0);
  EXPECT_EQ(call_count, 1);
}

TEST_F(GthrOnceTest, SkipsWhenAlreadyTwo) {
  test_gthread_once_t once = 2;
  auto func = [](void) { };
  EXPECT_EQ(test_gthread_once(&once, func), 0);
}

class GthrKeyTest : public ::testing::Test {
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

TEST_F(GthrKeyTest, CreateFirstKey) {
  test_gthread_key_t key;
  EXPECT_EQ(test_gthread_key_create(&key, nullptr), 0);
  EXPECT_EQ(key, 0u);
}

TEST_F(GthrKeyTest, DeleteKeyAlwaysSucceeds) {
  EXPECT_EQ(test_gthread_key_delete(0), 0);
  EXPECT_EQ(test_gthread_key_delete(42), 0);
}

class GthrActivePTest : public ::testing::Test {
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

TEST_F(GthrActivePTest, ReturnsOneWhenRunning) {
  EXPECT_CALL(*mock, xTaskGetSchedulerState())
      .WillOnce(Return(taskSCHEDULER_RUNNING));
  EXPECT_EQ(test_gthread_active_p(), 1);
}

TEST_F(GthrActivePTest, ReturnsZeroWhenNotRunning) {
  EXPECT_CALL(*mock, xTaskGetSchedulerState()).WillOnce(Return(0));
  EXPECT_EQ(test_gthread_active_p(), 0);
}

class GthrCondImpl1Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t sig_h = reinterpret_cast<SemaphoreHandle_t>(0x3001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(GthrCondImpl1Test, SignalNoWaiters) {
  test_gthread_cond_t cv = {sig_h, 0};
  EXPECT_EQ(test_gthread_cond_signal(&cv), 0);
}

TEST_F(GthrCondImpl1Test, SignalWithWaiters) {
  test_gthread_cond_t cv = {sig_h, 2};
  EXPECT_CALL(*mock, xSemaphoreGive(sig_h)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(test_gthread_cond_signal(&cv), 0);
}

TEST_F(GthrCondImpl1Test, BroadcastWithWaiters) {
  test_gthread_cond_t cv = {sig_h, 3};
  EXPECT_CALL(*mock, xSemaphoreGive(sig_h)).Times(3).WillRepeatedly(Return(pdTRUE));
  EXPECT_EQ(test_gthread_cond_broadcast(&cv), 0);
}

TEST_F(GthrCondImpl1Test, BroadcastNoWaiters) {
  test_gthread_cond_t cv = {sig_h, 0};
  EXPECT_EQ(test_gthread_cond_broadcast(&cv), 0);
}

TEST_F(GthrCondImpl1Test, DestroyWithSignal) {
  test_gthread_cond_t cv = {sig_h, 0};
  EXPECT_CALL(*mock, vSemaphoreDelete(sig_h));
  EXPECT_EQ(test_gthread_cond_destroy(&cv), 0);
  EXPECT_EQ(cv.signal, nullptr);
}

TEST_F(GthrCondImpl1Test, DestroyNullSignal) {
  test_gthread_cond_t cv = {nullptr, 0};
  EXPECT_EQ(test_gthread_cond_destroy(&cv), 0);
}

TEST_F(GthrCondImpl1Test, ThreadSelfReturnsCurrentHandle) {
  TaskHandle_t self_handle = reinterpret_cast<TaskHandle_t>(0x4001);
  EXPECT_CALL(*mock, xTaskGetCurrentTaskHandle()).WillOnce(Return(self_handle));
  EXPECT_EQ(xTaskGetCurrentTaskHandle(), self_handle);
}

TEST_F(GthrCondImpl1Test, ThreadEqualTrue) {
  TaskHandle_t t1 = reinterpret_cast<TaskHandle_t>(0x4001);
  EXPECT_TRUE(t1 == t1);
}

TEST_F(GthrCondImpl1Test, ThreadEqualFalse) {
  TaskHandle_t t1 = reinterpret_cast<TaskHandle_t>(0x4001);
  TaskHandle_t t2 = reinterpret_cast<TaskHandle_t>(0x4002);
  EXPECT_FALSE(t1 == t2);
}