/**
 * @file test_freertos_mutex_swap_safety.cpp
 * @brief Tests for issue #338 — mutex / recursive_mutex swap safety policy.
 *
 * mutex::swap (and recursive_mutex::swap) are unsafe when a lock_guard,
 * try_lock_guard, or timeout_lock_guard holds a live reference to the
 * mutex being swapped: the guard's destructor would unlock the swapped-in
 * (foreign) semaphore, releasing a lock the guard never acquired.
 *
 * The fix maintains an atomic m_guard_refcount on the mutex; RAII guards
 * increment on successful acquire and decrement on dtor. swap / move-assign /
 * move-ctor configASSERT that the count is zero.
 *
 * configASSERT is a no-op in the host mock environment, so the negative
 * (death) cases described in the PR-H plan cannot trigger here. These tests
 * verify the positive contract: the refcount is correctly maintained, swap
 * works on quiescent mutexes, and lock guards leave the count clean on dtor.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_semaphore.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;

using Mutex = freertos::da::mutex;
using RecursiveMutex = freertos::da::recursive_mutex;

class MutexSwapSafetyTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new NiceMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    h1 = reinterpret_cast<SemaphoreHandle_t>(0xA001);
    h2 = reinterpret_cast<SemaphoreHandle_t>(0xA002);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  NiceMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t h1;
  SemaphoreHandle_t h2;
};

// ============================================================
// Default-constructed mutex has zero live guards.
// ============================================================
TEST_F(MutexSwapSafetyTest, MutexInitialRefcountZero) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h1));
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  Mutex m;
  EXPECT_EQ(m.guard_refcount(), 0u);
}

// ============================================================
// lock_guard increments on construction, decrements on destruction.
// ============================================================
TEST_F(MutexSwapSafetyTest, LockGuardIncrementsAndDecrementsRefcount) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xSemaphoreTake(h1, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(h1)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  Mutex m;
  EXPECT_EQ(m.guard_refcount(), 0u);
  {
    freertos::lock_guard<Mutex> guard(m);
    EXPECT_EQ(m.guard_refcount(), 1u);
  }
  EXPECT_EQ(m.guard_refcount(), 0u);
}

// ============================================================
// Nested lock_guard scopes accumulate and unwind correctly.
// (Not legal on a non-recursive mutex in production; we verify
// the refcount mechanics, not lock semantics.)
// ============================================================
TEST_F(MutexSwapSafetyTest, MultipleLockGuardsNestRefcount) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  EXPECT_CALL(*mock, xSemaphoreTake(_, _)).WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(_)).WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  Mutex m1;
  Mutex m2;
  {
    freertos::lock_guard<Mutex> g1(m1);
    {
      freertos::lock_guard<Mutex> g2(m2);
      EXPECT_EQ(m1.guard_refcount(), 1u);
      EXPECT_EQ(m2.guard_refcount(), 1u);
    }
    EXPECT_EQ(m1.guard_refcount(), 1u);
    EXPECT_EQ(m2.guard_refcount(), 0u);
  }
  EXPECT_EQ(m1.guard_refcount(), 0u);
  EXPECT_EQ(m2.guard_refcount(), 0u);
}

// ============================================================
// try_lock_guard: success path increments, failure path does not.
// ============================================================
TEST_F(MutexSwapSafetyTest, TryLockGuardSuccessIncrementsRefcount) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xSemaphoreTake(h1, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(h1)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  Mutex m;
  {
    freertos::try_lock_guard<Mutex> g(m);
    ASSERT_TRUE(g.locked());
    EXPECT_EQ(m.guard_refcount(), 1u);
  }
  EXPECT_EQ(m.guard_refcount(), 0u);
}

TEST_F(MutexSwapSafetyTest, TryLockGuardFailureLeavesRefcountZero) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xSemaphoreTake(h1, 0)).WillOnce(Return(pdFALSE));
  // No xSemaphoreGive expected — failed try_lock means dtor does not unlock.
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  Mutex m;
  {
    freertos::try_lock_guard<Mutex> g(m);
    ASSERT_FALSE(g.locked());
    EXPECT_EQ(m.guard_refcount(), 0u);
  }
  EXPECT_EQ(m.guard_refcount(), 0u);
}

// ============================================================
// timeout_lock_guard: success increments, failure does not.
// ============================================================
TEST_F(MutexSwapSafetyTest, TimeoutLockGuardSuccessIncrementsRefcount) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xSemaphoreTake(h1, TickType_t(50))).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(h1)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  Mutex m;
  {
    freertos::timeout_lock_guard<Mutex> g(m, TickType_t(50));
    ASSERT_TRUE(g.locked());
    EXPECT_EQ(m.guard_refcount(), 1u);
  }
  EXPECT_EQ(m.guard_refcount(), 0u);
}

TEST_F(MutexSwapSafetyTest, TimeoutLockGuardFailureLeavesRefcountZero) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xSemaphoreTake(h1, TickType_t(10))).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  Mutex m;
  {
    freertos::timeout_lock_guard<Mutex> g(m, TickType_t(10));
    ASSERT_FALSE(g.locked());
    EXPECT_EQ(m.guard_refcount(), 0u);
  }
  EXPECT_EQ(m.guard_refcount(), 0u);
}

// ============================================================
// Swap is allowed on quiescent (no-live-guard) mutexes — verify
// post-swap operability. (#338 "AllowedWhenQuiescent")
// ============================================================
TEST_F(MutexSwapSafetyTest, MutexSwapAllowedWhenQuiescent) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  // After swap, m1's underlying semaphore is h2; lock then unlock m1.
  EXPECT_CALL(*mock, xSemaphoreTake(h2, portMAX_DELAY)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(h2)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  Mutex m1;
  Mutex m2;
  m1.swap(m2);
  EXPECT_EQ(m1.guard_refcount(), 0u);
  EXPECT_EQ(m2.guard_refcount(), 0u);
  // m1 is operable post-swap.
  EXPECT_EQ(m1.lock(), pdTRUE);
  EXPECT_EQ(m1.unlock(), pdTRUE);
}

// ============================================================
// Move-construction is allowed when src has no live guards.
// ============================================================
TEST_F(MutexSwapSafetyTest, MutexMoveConstructorAllowedWhenQuiescent) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(h1));
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  Mutex m1;
  Mutex m2(std::move(m1));
  EXPECT_EQ(m2.guard_refcount(), 0u);
}

// ============================================================
// Move-assignment is allowed when both sides have no live guards.
// ============================================================
TEST_F(MutexSwapSafetyTest, MutexMoveAssignmentAllowedWhenQuiescent) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  Mutex m1;
  Mutex m2;
  m1 = std::move(m2);
  EXPECT_EQ(m1.guard_refcount(), 0u);
}

// ============================================================
// recursive_mutex: same contract.
// ============================================================
TEST_F(MutexSwapSafetyTest, RecursiveMutexInitialRefcountZero) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(h1));
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  RecursiveMutex rm;
  EXPECT_EQ(rm.guard_refcount(), 0u);
}

TEST_F(MutexSwapSafetyTest, RecursiveMutexLockGuardManagesRefcount) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(h1, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(h1)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  RecursiveMutex rm;
  {
    freertos::lock_guard<RecursiveMutex> g(rm);
    EXPECT_EQ(rm.guard_refcount(), 1u);
  }
  EXPECT_EQ(rm.guard_refcount(), 0u);
}

TEST_F(MutexSwapSafetyTest, RecursiveMutexSwapAllowedWhenQuiescent) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  RecursiveMutex rm1;
  RecursiveMutex rm2;
  rm1.swap(rm2);
  EXPECT_EQ(rm1.guard_refcount(), 0u);
  EXPECT_EQ(rm2.guard_refcount(), 0u);
}

TEST_F(MutexSwapSafetyTest, RecursiveMutexMoveConstructorAllowedWhenQuiescent) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(h1));
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  RecursiveMutex rm1;
  RecursiveMutex rm2(std::move(rm1));
  EXPECT_EQ(rm2.guard_refcount(), 0u);
}

TEST_F(MutexSwapSafetyTest, RecursiveMutexMoveAssignmentAllowedWhenQuiescent) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  RecursiveMutex rm1;
  RecursiveMutex rm2;
  rm1 = std::move(rm2);
  EXPECT_EQ(rm1.guard_refcount(), 0u);
}

// ============================================================
// Refcount stays at zero across swap on already-quiescent mutexes,
// and refcount is *not* swapped (it tracks live refs to *this object*,
// not the underlying handle).
// ============================================================
TEST_F(MutexSwapSafetyTest, MutexSwapDoesNotSwapRefcountField) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(h1))
      .WillOnce(Return(h2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  Mutex m1;
  Mutex m2;
  // Both quiescent.
  m1.swap(m2);
  // Both still quiescent — refcount doesn't migrate, it stays per-object.
  EXPECT_EQ(m1.guard_refcount(), 0u);
  EXPECT_EQ(m2.guard_refcount(), 0u);
}
