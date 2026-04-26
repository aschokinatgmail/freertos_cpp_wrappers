#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_semaphore.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

using freertos::error;
using freertos::expected;
using freertos::unexpected;

class CoverageSemaphore2Test : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_sem = reinterpret_cast<SemaphoreHandle_t>(0x1000);
    mock_sem2 = reinterpret_cast<SemaphoreHandle_t>(0x1100);
    mock_counting = reinterpret_cast<SemaphoreHandle_t>(0x2000);
    mock_counting2 = reinterpret_cast<SemaphoreHandle_t>(0x2100);
    mock_mutex = reinterpret_cast<SemaphoreHandle_t>(0x3000);
    mock_mutex2 = reinterpret_cast<SemaphoreHandle_t>(0x3100);
    mock_recursive = reinterpret_cast<SemaphoreHandle_t>(0x4000);
    mock_recursive2 = reinterpret_cast<SemaphoreHandle_t>(0x4100);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_sem;
  SemaphoreHandle_t mock_sem2;
  SemaphoreHandle_t mock_counting;
  SemaphoreHandle_t mock_counting2;
  SemaphoreHandle_t mock_mutex;
  SemaphoreHandle_t mock_mutex2;
  SemaphoreHandle_t mock_recursive;
  SemaphoreHandle_t mock_recursive2;
};

using BinarySem = freertos::binary_semaphore<freertos::dynamic_semaphore_allocator>;
using CountingSem = freertos::counting_semaphore<freertos::dynamic_semaphore_allocator>;
using Mutex = freertos::mutex<freertos::dynamic_semaphore_allocator>;
using RecursiveMutex = freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>;

// ============================================================
// binary_semaphore
// ============================================================

TEST_F(CoverageSemaphore2Test, BinaryGiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.give_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

TEST_F(CoverageSemaphore2Test, BinaryTakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.take_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore2Test, BinaryTakeExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.take_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageSemaphore2Test, BinaryTakeExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.take_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore2Test, BinaryTakeExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::would_block);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphore2Test, BinaryGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.give_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore2Test, BinaryGiveExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.give_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::semaphore_not_owned);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphore2Test, BinarySwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_sem))
      .WillOnce(Return(mock_sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  BinarySem sem1;
  BinarySem sem2;
  sem1.swap(sem2);
}

// ============================================================
// counting_semaphore
// ============================================================

TEST_F(CoverageSemaphore2Test, CountingGiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.give_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

TEST_F(CoverageSemaphore2Test, CountingTakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.take_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore2Test, CountingTakeExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.take_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageSemaphore2Test, CountingTakeExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.take_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore2Test, CountingTakeExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::would_block);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphore2Test, CountingGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.give_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore2Test, CountingGiveExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.give_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::semaphore_not_owned);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphore2Test, CountingSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting))
      .WillOnce(Return(mock_counting2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  CountingSem sem1(5);
  CountingSem sem2(5);
  sem1.swap(sem2);
}

// ============================================================
// mutex
// ============================================================

TEST_F(CoverageSemaphore2Test, MutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.lock_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphore2Test, MutexLockExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_mutex, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.lock_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_TRUE(mtx.locked());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore2Test, MutexLockExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_mutex, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.lock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::would_block);
  EXPECT_FALSE(mtx.locked());
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphore2Test, MutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.unlock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

TEST_F(CoverageSemaphore2Test, MutexUnlockExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_mutex, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  mtx.lock();
  EXPECT_TRUE(mtx.locked());
  auto result = mtx.unlock_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphore2Test, MutexUnlockExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_mutex, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  mtx.lock();
  EXPECT_TRUE(mtx.locked());
  auto result = mtx.unlock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::semaphore_not_owned);
  EXPECT_TRUE(mtx.locked());
}

TEST_F(CoverageSemaphore2Test, MutexTryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.try_lock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore2Test, MutexLockIsrFailureNoMutedState) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_mutex, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.lock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphore2Test, MutexUnlockIsrFailureNoStateChange) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_mutex, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  mtx.lock();
  EXPECT_TRUE(mtx.locked());
  auto result = mtx.unlock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_TRUE(mtx.locked());
}

TEST_F(CoverageSemaphore2Test, MutexSwapWithLockedState) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex))
      .WillOnce(Return(mock_mutex2));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  Mutex mtx1;
  Mutex mtx2;
  mtx1.lock();
  EXPECT_TRUE(mtx1.locked());
  EXPECT_FALSE(mtx2.locked());
  mtx1.swap(mtx2);
  EXPECT_FALSE(mtx1.locked());
  EXPECT_TRUE(mtx2.locked());
}

// ============================================================
// recursive_mutex
// ============================================================

TEST_F(CoverageSemaphore2Test, RecursiveMutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  auto result = rm.lock_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore2Test, RecursiveMutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  auto result = rm.unlock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

TEST_F(CoverageSemaphore2Test, RecursiveMutexTryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  auto result = rm.try_lock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore2Test, RecursiveMutexSwapWithRecursionCount) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive))
      .WillOnce(Return(mock_recursive2));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, portMAX_DELAY))
      .WillOnce(Return(pdTRUE))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  RecursiveMutex rm1;
  RecursiveMutex rm2;
  rm1.lock();
  rm1.lock();
  EXPECT_EQ(rm1.recursions_count(), 2);
  EXPECT_EQ(rm2.recursions_count(), 0);
  rm1.swap(rm2);
  EXPECT_EQ(rm1.recursions_count(), 0);
  EXPECT_EQ(rm2.recursions_count(), 2);
}

// ============================================================
// Guard classes
// ============================================================

TEST_F(CoverageSemaphore2Test, LockGuardIsrConstructorDestructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_mutex, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_mutex, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::lock_guard_isr<Mutex> guard(mtx);
    EXPECT_TRUE(guard.locked());
    EXPECT_EQ(guard.high_priority_task_woken(), pdTRUE);
  }
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphore2Test, LockGuardIsrLockFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_mutex, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::lock_guard_isr<Mutex> guard(mtx);
    EXPECT_FALSE(guard.locked());
    EXPECT_EQ(guard.high_priority_task_woken(), pdFALSE);
  }
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphore2Test, TimeoutLockGuardLockedSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::timeout_lock_guard<Mutex> guard(mtx, portMAX_DELAY);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore2Test, TimeoutLockGuardLockedFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, pdMS_TO_TICKS(100)))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::timeout_lock_guard<Mutex> guard(
        mtx, std::chrono::milliseconds(100));
    EXPECT_FALSE(guard.locked());
  }
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphore2Test, TryLockGuardLockedSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::try_lock_guard<Mutex> guard(mtx);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore2Test, TryLockGuardLockedFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::try_lock_guard<Mutex> guard(mtx);
    EXPECT_FALSE(guard.locked());
  }
}