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

class CoverageSemaphoreTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_sem = reinterpret_cast<SemaphoreHandle_t>(0x1000);
    mock_mutex = reinterpret_cast<SemaphoreHandle_t>(0x2000);
    mock_recursive_mutex = reinterpret_cast<SemaphoreHandle_t>(0x3000);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_sem;
  SemaphoreHandle_t mock_mutex;
  SemaphoreHandle_t mock_recursive_mutex;
};

using BinarySem = freertos::binary_semaphore<freertos::dynamic_semaphore_allocator>;
using CountingSem = freertos::counting_semaphore<freertos::dynamic_semaphore_allocator>;
using Mutex = freertos::mutex<freertos::dynamic_semaphore_allocator>;
using RecursiveMutex = freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>;

TEST_F(CoverageSemaphoreTest, BinaryGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.give_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphoreTest, BinaryGiveExIsrFailure) {
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

TEST_F(CoverageSemaphoreTest, BinaryTakeExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.take_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphoreTest, BinaryTakeExIsrFailure) {
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

TEST_F(CoverageSemaphoreTest, BinaryTakeExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, pdMS_TO_TICKS(100)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.take_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(CoverageSemaphoreTest, BinaryAcquireChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, pdMS_TO_TICKS(100)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  sem.acquire(std::chrono::milliseconds(100));
}

TEST_F(CoverageSemaphoreTest, BinaryReleaseCall) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  sem.release();
}

TEST_F(CoverageSemaphoreTest, BinaryReleaseIsrCall) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.release_isr();
  EXPECT_EQ(result.result, pdTRUE);
}

TEST_F(CoverageSemaphoreTest, CountingGiveExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  auto result = sem.give_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(CoverageSemaphoreTest, CountingGiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  auto result = sem.give_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

TEST_F(CoverageSemaphoreTest, CountingGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  auto result = sem.give_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphoreTest, CountingGiveExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  auto result = sem.give_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::semaphore_not_owned);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphoreTest, CountingTakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  auto result = sem.take_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphoreTest, CountingTakeExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  auto result = sem.take_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageSemaphoreTest, CountingTakeExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, pdMS_TO_TICKS(100)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  auto result = sem.take_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(CoverageSemaphoreTest, CountingTakeExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  auto result = sem.take_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphoreTest, CountingTakeExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  auto result = sem.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::would_block);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphoreTest, CountingAcquireDefault) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  sem.acquire();
}

TEST_F(CoverageSemaphoreTest, CountingAcquireChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, pdMS_TO_TICKS(100)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  sem.acquire(std::chrono::milliseconds(100));
}

TEST_F(CoverageSemaphoreTest, CountingTryAcquireSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  EXPECT_TRUE(sem.try_acquire());
}

TEST_F(CoverageSemaphoreTest, CountingTryAcquireFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  EXPECT_FALSE(sem.try_acquire());
}

TEST_F(CoverageSemaphoreTest, CountingRelease) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  sem.release();
}

TEST_F(CoverageSemaphoreTest, CountingReleaseIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  auto result = sem.release_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphoreTest, CountingOperatorPlusPlus) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  ++sem;
}

TEST_F(CoverageSemaphoreTest, CountingOperatorPlusPlusInt) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  sem++;
}

TEST_F(CoverageSemaphoreTest, CountingOperatorMinusMinus) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  --sem;
}

TEST_F(CoverageSemaphoreTest, CountingOperatorMinusMinusInt) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  sem--;
}

TEST_F(CoverageSemaphoreTest, CountingOperatorPlusEqual) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).Times(3);
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  sem += 3;
}

TEST_F(CoverageSemaphoreTest, CountingCount) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5)).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_sem)).WillOnce(Return(3));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  CountingSem sem(5);
  EXPECT_EQ(sem.count(), 3);
}

TEST_F(CoverageSemaphoreTest, MutexLockExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, pdMS_TO_TICKS(100)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.lock_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_TRUE(mtx.locked());
}

TEST_F(CoverageSemaphoreTest, MutexLockIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.lock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphoreTest, MutexUnlockIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.unlock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphoreTest, MutexTryLockFor) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, pdMS_TO_TICKS(100)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  EXPECT_TRUE(mtx.try_lock_for(std::chrono::milliseconds(100)));
}

TEST_F(CoverageSemaphoreTest, MutexTryLockUntilPastTime) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto past = std::chrono::steady_clock::now() - std::chrono::milliseconds(100);
  EXPECT_FALSE(mtx.try_lock_until(past));
}

TEST_F(CoverageSemaphoreTest, MutexTryLockUntilFutureTime) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto future = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
  EXPECT_TRUE(mtx.try_lock_until(future));
}

TEST_F(CoverageSemaphoreTest, MutexLockFailureMutedState) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, portMAX_DELAY))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.lock();
  EXPECT_EQ(result, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphoreTest, MutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.unlock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

TEST_F(CoverageSemaphoreTest, RecursiveMutexLockExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive_mutex));
  EXPECT_CALL(*mock,
              xSemaphoreTakeRecursive(mock_recursive_mutex, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex));
  RecursiveMutex rm;
  auto result = rm.lock_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

TEST_F(CoverageSemaphoreTest, RecursiveMutexLockExChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive_mutex));
  EXPECT_CALL(*mock,
              xSemaphoreTakeRecursive(mock_recursive_mutex, pdMS_TO_TICKS(100)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex));
  RecursiveMutex rm;
  auto result = rm.lock_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(CoverageSemaphoreTest, RecursiveMutexTryLockFor) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive_mutex));
  EXPECT_CALL(*mock,
              xSemaphoreTakeRecursive(mock_recursive_mutex, pdMS_TO_TICKS(100)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex));
  RecursiveMutex rm;
  EXPECT_TRUE(rm.try_lock_for(std::chrono::milliseconds(100)));
}

TEST_F(CoverageSemaphoreTest, RecursiveMutexTryLockUntilPastTime) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive_mutex));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive_mutex, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex));
  RecursiveMutex rm;
  auto past = std::chrono::steady_clock::now() - std::chrono::milliseconds(100);
  EXPECT_FALSE(rm.try_lock_until(past));
}

TEST_F(CoverageSemaphoreTest, RecursiveMutexTryLockUntilFutureTime) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive_mutex));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive_mutex, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex));
  RecursiveMutex rm;
  auto future = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
  EXPECT_TRUE(rm.try_lock_until(future));
}

TEST_F(CoverageSemaphoreTest, LockGuardIsrWithMutexFails) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::lock_guard_isr<Mutex> guard(mtx);
    EXPECT_FALSE(guard.locked());
    EXPECT_EQ(guard.high_priority_task_woken(), pdFALSE);
  }
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphoreTest, TimeoutLockGuardMutexFail) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, pdMS_TO_TICKS(100)))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::timeout_lock_guard<Mutex> guard(
        mtx, std::chrono::milliseconds(100));
    EXPECT_FALSE(guard.locked());
    EXPECT_FALSE(mtx.locked());
  }
  EXPECT_FALSE(mtx.locked());
}