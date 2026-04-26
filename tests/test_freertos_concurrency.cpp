/**
 * @file test_freertos_concurrency.cpp
 * @brief Concurrency and ISR testing framework for FreeRTOS C++ Wrappers
 *
 * Tests verify lock guard RAII behavior, ISR context simulation,
 * and context_switch_needed propagation under simulated concurrent access.
 *
 * Mock ISR context is simulated via g_is_isr_context flag and
 * xPortIsInsideInterrupt() mock return values.
 *
 * Naming convention: TestSuiteName_TestCaseName
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_isr_result.hpp"
#include "freertos_semaphore.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

static bool g_is_isr_context = false;

class ConcurrencyTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    g_is_isr_context = false;

    mock_mutex_handle = reinterpret_cast<SemaphoreHandle_t>(0xDEADBEEF);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
    g_is_isr_context = false;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_mutex_handle;
};

using mutex_t = freertos::mutex<freertos::dynamic_semaphore_allocator>;

class MockMutex {
public:
  MOCK_METHOD(BaseType_t, lock, (TickType_t));
  MOCK_METHOD(void, unlock, ());
  MOCK_METHOD(BaseType_t, try_lock, ());
  MOCK_METHOD(freertos::isr_result<BaseType_t>, lock_isr, ());
  MOCK_METHOD(freertos::isr_result<BaseType_t>, unlock_isr, ());
  MOCK_METHOD(bool, locked, (), (const));

  BaseType_t lock() { return lock(portMAX_DELAY); }
};

// =============================================================================
// lock_guard tests
// =============================================================================

TEST(LockGuardTest, AcquiresOnConstruction) {
  StrictMock<MockMutex> m;
  EXPECT_CALL(m, lock(portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(m, unlock());
  freertos::lock_guard<MockMutex> guard(m);
}

TEST(LockGuardTest, ReleasesOnDestruction) {
  StrictMock<MockMutex> m;
  {
    EXPECT_CALL(m, lock(portMAX_DELAY)).WillOnce(Return(pdTRUE));
    freertos::lock_guard<MockMutex> guard(m);
    EXPECT_CALL(m, unlock());
  }
}

TEST(LockGuardTest, LockedReturnsTrueWhenMutexLocked) {
  NiceMock<MockMutex> m;
  EXPECT_CALL(m, locked()).WillOnce(Return(true));
  freertos::lock_guard<MockMutex> guard(m);
  EXPECT_TRUE(guard.locked());
}

TEST(LockGuardTest, LockedReturnsFalseWhenMutexNotLocked) {
  NiceMock<MockMutex> m;
  EXPECT_CALL(m, locked()).WillOnce(Return(false));
  freertos::lock_guard<MockMutex> guard(m);
  EXPECT_FALSE(guard.locked());
}

// =============================================================================
// try_lock_guard tests
// =============================================================================

TEST(TryLockGuardTest, AcquiresOnSuccess) {
  StrictMock<MockMutex> m;
  EXPECT_CALL(m, try_lock()).WillOnce(Return(pdTRUE));
  EXPECT_CALL(m, unlock());
  EXPECT_CALL(m, locked()).WillOnce(Return(true));
  freertos::try_lock_guard<MockMutex> guard(m);
  EXPECT_TRUE(guard.locked());
}

TEST(TryLockGuardTest, DoesNotAcquireOnFailure) {
  StrictMock<MockMutex> m;
  EXPECT_CALL(m, try_lock()).WillOnce(Return(false));
  freertos::try_lock_guard<MockMutex> guard(m);
  EXPECT_FALSE(guard.locked());
}

TEST(TryLockGuardTest, ReleasesOnDestructionIfAcquired) {
  StrictMock<MockMutex> m;
  EXPECT_CALL(m, try_lock()).WillOnce(Return(true));
  {
    freertos::try_lock_guard<MockMutex> guard(m);
    EXPECT_CALL(m, unlock());
  }
}

TEST(TryLockGuardTest, NoReleaseOnDestructionIfNotAcquired) {
  StrictMock<MockMutex> m;
  EXPECT_CALL(m, try_lock()).WillOnce(Return(false));
  {
    freertos::try_lock_guard<MockMutex> guard(m);
  }
}

// =============================================================================
// timeout_lock_guard tests
// =============================================================================

TEST(TimeoutLockGuardTest, AcquiresOnSuccess) {
  StrictMock<MockMutex> m;
  EXPECT_CALL(m, lock(100)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(m, unlock());
  EXPECT_CALL(m, locked()).WillOnce(Return(true));
  freertos::timeout_lock_guard<MockMutex> guard(m, TickType_t(100));
  EXPECT_TRUE(guard.locked());
}

TEST(TimeoutLockGuardTest, DoesNotAcquireOnTimeout) {
  StrictMock<MockMutex> m;
  EXPECT_CALL(m, lock(50)).WillOnce(Return(pdFALSE));
  freertos::timeout_lock_guard<MockMutex> guard(m, TickType_t(50));
  EXPECT_FALSE(guard.locked());
}

TEST(TimeoutLockGuardTest, ReleasesOnDestructionIfAcquired) {
  StrictMock<MockMutex> m;
  EXPECT_CALL(m, lock(200)).WillOnce(Return(pdTRUE));
  {
    freertos::timeout_lock_guard<MockMutex> guard(m, TickType_t(200));
    EXPECT_CALL(m, unlock());
  }
}

TEST(TimeoutLockGuardTest, NoReleaseOnDestructionIfNotAcquired) {
  StrictMock<MockMutex> m;
  EXPECT_CALL(m, lock(200)).WillOnce(Return(pdFALSE));
  {
    freertos::timeout_lock_guard<MockMutex> guard(m, TickType_t(200));
  }
}

TEST(TimeoutLockGuardTest, ChronoTimeoutAcquires) {
  StrictMock<MockMutex> m;
  EXPECT_CALL(m, lock(_)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(m, unlock());
  EXPECT_CALL(m, locked()).WillOnce(Return(true));
  freertos::timeout_lock_guard<MockMutex> guard(m, std::chrono::milliseconds(100));
  EXPECT_TRUE(guard.locked());
}

// =============================================================================
// lock_guard_isr tests
// =============================================================================

TEST(LockGuardIsrTest, CallsLockIsrOnConstruction) {
  StrictMock<MockMutex> m;
  freertos::isr_result<BaseType_t> lock_result{pdTRUE, pdFALSE};
  EXPECT_CALL(m, lock_isr()).WillOnce(Return(lock_result));
  freertos::isr_result<BaseType_t> unlock_result{pdTRUE, pdFALSE};
  EXPECT_CALL(m, unlock_isr()).WillOnce(Return(unlock_result));
  freertos::lock_guard_isr<MockMutex> guard(m);
}

TEST(LockGuardIsrTest, CallsUnlockIsrOnDestruction) {
  StrictMock<MockMutex> m;
  freertos::isr_result<BaseType_t> lock_result{pdTRUE, pdFALSE};
  freertos::isr_result<BaseType_t> unlock_result{pdTRUE, pdFALSE};
  EXPECT_CALL(m, lock_isr()).WillOnce(Return(lock_result));
  {
    freertos::lock_guard_isr<MockMutex> guard(m);
    EXPECT_CALL(m, unlock_isr()).WillOnce(Return(unlock_result));
  }
}

TEST(LockGuardIsrTest, PropagatesHigherPriorityTaskWokenFromLock) {
  StrictMock<MockMutex> m;
  freertos::isr_result<BaseType_t> lock_result{pdTRUE, pdTRUE};
  EXPECT_CALL(m, lock_isr()).WillOnce(Return(lock_result));
  freertos::isr_result<BaseType_t> unlock_result{pdTRUE, pdFALSE};
  EXPECT_CALL(m, unlock_isr()).WillOnce(Return(unlock_result));
  freertos::lock_guard_isr<MockMutex> guard(m);
  EXPECT_EQ(guard.high_priority_task_woken(), pdTRUE);
}

TEST(LockGuardIsrTest, PropagatesHigherPriorityTaskWokenFromUnlock) {
  StrictMock<MockMutex> m;
  freertos::isr_result<BaseType_t> lock_result{pdTRUE, pdFALSE};
  EXPECT_CALL(m, lock_isr()).WillOnce(Return(lock_result));
  freertos::isr_result<BaseType_t> unlock_result{pdTRUE, pdTRUE};
  {
    EXPECT_CALL(m, unlock_isr()).WillOnce(Return(unlock_result));
    freertos::lock_guard_isr<MockMutex> guard(m);
    EXPECT_EQ(guard.high_priority_task_woken(), pdFALSE);
  }
}

TEST(LockGuardIsrTest, ReturnsLockedState) {
  NiceMock<MockMutex> m;
  freertos::isr_result<BaseType_t> result{pdTRUE, pdFALSE};
  EXPECT_CALL(m, lock_isr()).WillOnce(Return(result));
  EXPECT_CALL(m, locked()).WillOnce(Return(true));
  freertos::lock_guard_isr<MockMutex> guard(m);
  EXPECT_TRUE(guard.locked());
}

// =============================================================================
// ISR context simulation with real freertos::mutex
// =============================================================================

TEST_F(ConcurrencyTest, IsrContextFlagSimulatesInterrupt) {
  g_is_isr_context = true;
  EXPECT_CALL(*mock, xPortIsInsideInterrupt()).WillOnce(Return(pdTRUE));
  EXPECT_EQ(xPortIsInsideInterrupt(), pdTRUE);

  g_is_isr_context = false;
  EXPECT_CALL(*mock, xPortIsInsideInterrupt()).WillOnce(Return(pdFALSE));
  EXPECT_EQ(xPortIsInsideInterrupt(), pdFALSE);
}

TEST_F(ConcurrencyTest, MutexLockIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  g_is_isr_context = true;
  auto lock_result = m.lock_isr();
  EXPECT_EQ(lock_result.result, pdFALSE);
  EXPECT_EQ(lock_result.higher_priority_task_woken, pdFALSE);
  auto unlock_result = m.unlock_isr();
  EXPECT_EQ(unlock_result.result, pdFALSE);
  EXPECT_EQ(unlock_result.higher_priority_task_woken, pdFALSE);
  g_is_isr_context = false;
}

TEST_F(ConcurrencyTest, MutexUnlockIsrReturnsFailureNoContextSwitch) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  auto lock_result = m.lock_isr();
  EXPECT_EQ(lock_result.result, pdFALSE);

  auto unlock_result = m.unlock_isr();
  EXPECT_EQ(unlock_result.result, pdFALSE);
  EXPECT_EQ(unlock_result.higher_priority_task_woken, pdFALSE);
}

TEST_F(ConcurrencyTest, BinarySemaphoreGiveIsrReturnsContextSwitchNeeded) {
  SemaphoreHandle_t sem_handle = reinterpret_cast<SemaphoreHandle_t>(0xAABBCCDD);
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(sem_handle));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(sem_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem_handle));

  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto result = sem.give_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(ConcurrencyTest, BinarySemaphoreTakeIsrReturnsContextSwitchNeeded) {
  SemaphoreHandle_t sem_handle = reinterpret_cast<SemaphoreHandle_t>(0xAABBCCDD);
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(sem_handle));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(sem_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem_handle));

  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto result = sem.take_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(ConcurrencyTest, CountingSemaphoreGiveIsrReturnsContextSwitchNeeded) {
  SemaphoreHandle_t sem_handle = reinterpret_cast<SemaphoreHandle_t>(0x11223344);
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(sem_handle));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(sem_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem_handle));

  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  auto result = sem.give_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

// =============================================================================
// lock_guard_isr with real freertos::mutex
// =============================================================================

TEST_F(ConcurrencyTest, LockGuardIsrWithRealMutexFailsToAcquire) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  {
    freertos::lock_guard_isr<mutex_t> guard(m);
    EXPECT_EQ(guard.high_priority_task_woken(), pdFALSE);
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(ConcurrencyTest, LockGuardIsrNoContextSwitch) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  BaseType_t woken = pdFALSE;
  {
    freertos::lock_guard_isr<mutex_t> guard(m);
    woken = guard.high_priority_task_woken();
  }
  EXPECT_EQ(woken, pdFALSE);
}

// =============================================================================
// No-deadlock verification: basic acquire/release patterns
// =============================================================================

TEST_F(ConcurrencyTest, LockGuardAcquireReleaseNoDeadlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  { freertos::lock_guard<mutex_t> guard(m); }
}

TEST_F(ConcurrencyTest, TryLockGuardAcquireReleaseNoDeadlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  { freertos::try_lock_guard<mutex_t> guard(m); }
}

TEST_F(ConcurrencyTest, TimeoutLockGuardAcquireReleaseNoDeadlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 100))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  { freertos::timeout_lock_guard<mutex_t> guard(m, TickType_t(100)); }
}

TEST_F(ConcurrencyTest, TryLockGuardFailsGracefullyNoDeadlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  {
    freertos::try_lock_guard<mutex_t> guard(m);
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(ConcurrencyTest, TimeoutLockGuardFailsGracefullyNoDeadlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 10))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  {
    freertos::timeout_lock_guard<mutex_t> guard(m, TickType_t(10));
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(ConcurrencyTest, NestedScopesNoDeadlock) {
  SemaphoreHandle_t handle1 = reinterpret_cast<SemaphoreHandle_t>(0x11111111);
  SemaphoreHandle_t handle2 = reinterpret_cast<SemaphoreHandle_t>(0x22222222);

  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, xSemaphoreTake(handle1, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreTake(handle2, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(handle1))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(handle2))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle1));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle2));

  mutex_t m1, m2;
  {
    freertos::lock_guard<mutex_t> g1(m1);
    { freertos::lock_guard<mutex_t> g2(m2); }
  }
}

// =============================================================================
// ISR methods distinguish task vs ISR context
// =============================================================================

TEST_F(ConcurrencyTest, TaskContextUsesNormalApi) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  EXPECT_EQ(m.lock(), pdTRUE);
  EXPECT_EQ(m.unlock(), pdTRUE);
}

TEST_F(ConcurrencyTest, IsrContextMutexIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  g_is_isr_context = true;

  auto lock_result = m.lock_isr();
  EXPECT_EQ(lock_result.result, pdFALSE);
  EXPECT_EQ(lock_result.higher_priority_task_woken, pdFALSE);

  auto unlock_result = m.unlock_isr();
  EXPECT_EQ(unlock_result.result, pdFALSE);
  EXPECT_EQ(unlock_result.higher_priority_task_woken, pdFALSE);

  g_is_isr_context = false;
}

TEST_F(ConcurrencyTest, PortYieldFromIsrNotCalledWhenIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  mutex_t m;
  g_is_isr_context = true;

  auto lock_result = m.lock_isr();
  EXPECT_EQ(lock_result.higher_priority_task_woken, pdFALSE);

  auto unlock_result = m.unlock_isr();
  EXPECT_EQ(unlock_result.higher_priority_task_woken, pdFALSE);

  g_is_isr_context = false;
}

// =============================================================================
// isr_result struct verification with lock guards
// =============================================================================

TEST(IsrResultIntegration, ContextSwitchNeededPreservedAcrossOperations) {
  freertos::isr_result<BaseType_t> r1{pdTRUE, pdTRUE};
  freertos::isr_result<BaseType_t> r2{pdFALSE, pdFALSE};
  freertos::isr_result<BaseType_t> r3{pdTRUE, pdFALSE};

  EXPECT_EQ(r1.higher_priority_task_woken, pdTRUE);
  EXPECT_EQ(r2.higher_priority_task_woken, pdFALSE);
  EXPECT_EQ(r3.higher_priority_task_woken, pdFALSE);

  BaseType_t accumulated = pdFALSE;
  if (r1.higher_priority_task_woken)
    accumulated = pdTRUE;
  if (r3.higher_priority_task_woken)
    accumulated = pdTRUE;

  EXPECT_EQ(accumulated, pdTRUE);
}

TEST(IsrResultIntegration, EqualityComparisonForContextSwitch) {
  freertos::isr_result<int> a{1, pdTRUE};
  freertos::isr_result<int> b{1, pdTRUE};
  freertos::isr_result<int> c{1, pdFALSE};

  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}
