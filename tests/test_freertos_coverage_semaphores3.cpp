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

class CoverageSemaphore3Test : public ::testing::Test {
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
using BinarySemStatic = freertos::binary_semaphore<freertos::static_semaphore_allocator>;
using CountingSem = freertos::counting_semaphore<freertos::dynamic_semaphore_allocator>;
using CountingSemStatic = freertos::counting_semaphore<freertos::static_semaphore_allocator>;
using Mutex = freertos::mutex<freertos::dynamic_semaphore_allocator>;
using MutexStatic = freertos::mutex<freertos::static_semaphore_allocator>;
using RecursiveMutex = freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>;
using RecursiveMutexStatic = freertos::recursive_mutex<freertos::static_semaphore_allocator>;

// ============================================================
// binary_semaphore — SFINAE constructor with AllocatorArgs
// Lines 192-194, 196
// ============================================================

TEST_F(CoverageSemaphore3Test, BinarySfinaeConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  freertos::static_semaphore_allocator alloc;
  BinarySemStatic sem(std::move(alloc));
}

// ============================================================
// binary_semaphore — Move constructor
// Lines 198-201
// ============================================================

TEST_F(CoverageSemaphore3Test, BinaryMoveConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem1;
  BinarySem sem2(std::move(sem1));
}

// ============================================================
// binary_semaphore — Move assignment
// Lines 214-218
// ============================================================

TEST_F(CoverageSemaphore3Test, BinaryMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_sem))
      .WillOnce(Return(mock_sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  BinarySem sem1;
  BinarySem sem2;
  sem1 = std::move(sem2);
}

TEST_F(CoverageSemaphore3Test, BinaryMoveAssignmentSelf) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem1;
  sem1 = std::move(sem1);
}

// ============================================================
// binary_semaphore — Member + friend swap
// Lines 221, 223-225, 227-229
// ============================================================

TEST_F(CoverageSemaphore3Test, BinaryFriendSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary())
      .WillOnce(Return(mock_sem))
      .WillOnce(Return(mock_sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  BinarySem sem1;
  BinarySem sem2;
  swap(sem1, sem2);
}

// ============================================================
// binary_semaphore — give_isr and take_isr
// Lines 247-251, 273-277
// ============================================================

TEST_F(CoverageSemaphore3Test, BinaryGiveIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.give_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore3Test, BinaryGiveIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.give_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphore3Test, BinaryTakeIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.take_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore3Test, BinaryTakeIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.take_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

// ============================================================
// binary_semaphore — give_ex_isr, take_ex with would_block, take_ex_isr
// Lines 299, 301-310, 317, 320-328
// ============================================================

TEST_F(CoverageSemaphore3Test, BinaryGiveExSuccessUnexpected) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.give_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(CoverageSemaphore3Test, BinaryTakeExWouldBlockPath) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.take_ex(static_cast<TickType_t>(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore3Test, BinaryTakeExIsrFull) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySem sem;
  auto result = sem.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::would_block);
}

// ============================================================
// binary_semaphore — destructor for moved-from object (null semaphore)
// Lines 207-211
// ============================================================

TEST_F(CoverageSemaphore3Test, BinaryDestructorMovedFrom) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  {
    BinarySem sem1;
    BinarySem sem2(std::move(sem1));
  }
}

// ============================================================
// counting_semaphore — SFINAE constructor with AllocatorArgs
// Lines 377-379, 381
// ============================================================

TEST_F(CoverageSemaphore3Test, CountingSfinaeConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  freertos::static_semaphore_allocator alloc;
  CountingSemStatic sem(5, std::move(alloc));
}

// ============================================================
// counting_semaphore — Move constructor
// Lines 383-387
// ============================================================

TEST_F(CoverageSemaphore3Test, CountingMoveConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem1(5);
  CountingSem sem2(std::move(sem1));
}

// ============================================================
// counting_semaphore — Move assignment
// Lines 400-404
// ============================================================

TEST_F(CoverageSemaphore3Test, CountingMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting))
      .WillOnce(Return(mock_counting2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  CountingSem sem1(5);
  CountingSem sem2(5);
  sem1 = std::move(sem2);
}

TEST_F(CoverageSemaphore3Test, CountingMoveAssignmentSelf) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem1(5);
  sem1 = std::move(sem1);
}

// ============================================================
// counting_semaphore — swap member + friend
// Lines 407, 409-411, 413-415
// ============================================================

TEST_F(CoverageSemaphore3Test, CountingFriendSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting))
      .WillOnce(Return(mock_counting2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  CountingSem sem1(5);
  CountingSem sem2(5);
  swap(sem1, sem2);
}

// ============================================================
// counting_semaphore — give_isr and take_isr
// Lines 433-437, 459-463
// ============================================================

TEST_F(CoverageSemaphore3Test, CountingGiveIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.give_isr();
  EXPECT_EQ(result.result, pdTRUE);
}

TEST_F(CoverageSemaphore3Test, CountingGiveIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.give_isr();
  EXPECT_EQ(result.result, pdFALSE);
}

TEST_F(CoverageSemaphore3Test, CountingTakeIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.take_isr();
  EXPECT_EQ(result.result, pdTRUE);
}

TEST_F(CoverageSemaphore3Test, CountingTakeIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.take_isr();
  EXPECT_EQ(result.result, pdFALSE);
}

// ============================================================
// counting_semaphore — give_ex_isr, take_ex_isr, take_ex would_block
// Lines 485, 487-496, 503, 506-514
// ============================================================

TEST_F(CoverageSemaphore3Test, CountingGiveExFailureUnexpected) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.give_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

TEST_F(CoverageSemaphore3Test, CountingTakeExWouldBlockTicks) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.take_ex(static_cast<TickType_t>(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore3Test, CountingTakeExIsrFull) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSem sem(5);
  auto result = sem.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::would_block);
}

// ============================================================
// counting_semaphore — destructor for moved-from object
// Lines 393-397 (null semaphore path)
// ============================================================

TEST_F(CoverageSemaphore3Test, CountingDestructorMovedFrom) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  {
    CountingSem sem1(5);
    CountingSem sem2(std::move(sem1));
  }
}

// ============================================================
// mutex — SFINAE constructor with AllocatorArgs
// Lines 622-624, 626
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexSfinaeConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  freertos::static_semaphore_allocator alloc;
  MutexStatic mtx(std::move(alloc));
}

// ============================================================
// mutex — Move constructor
// Lines 628-634
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexMoveConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx1;
  Mutex mtx2(std::move(mtx1));
}

// ============================================================
// mutex — Move assignment
// Lines 647, 650-653
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex))
      .WillOnce(Return(mock_mutex2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  Mutex mtx1;
  Mutex mtx2;
  mtx1 = std::move(mtx2);
}

TEST_F(CoverageSemaphore3Test, MutexMoveAssignmentSelf) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx1;
  mtx1 = std::move(mtx1);
}

// ============================================================
// mutex — swap member + friend
// Lines 656, 658-663, 665
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexFriendSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex))
      .WillOnce(Return(mock_mutex2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  Mutex mtx1;
  Mutex mtx2;
  swap(mtx1, mtx2);
}

TEST_F(CoverageSemaphore3Test, MutexSwapBothLocked) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex))
      .WillOnce(Return(mock_mutex2));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex2, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  Mutex mtx1;
  Mutex mtx2;
  mtx1.lock();
  mtx2.lock();
  mtx1.swap(mtx2);
  EXPECT_TRUE(mtx1.locked());
  EXPECT_TRUE(mtx2.locked());
}

// ============================================================
// mutex — unlock_isr
// Lines 687-694
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexUnlockIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.unlock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}

// ============================================================
// mutex — lock_isr
// Lines 718-725
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexLockIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.lock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}

// ============================================================
// mutex — lock_ex would_block, lock_ex_isr, unlock_ex, unlock_ex_isr, try_lock_ex
// Lines 759, 762-770, 776, 778-787, 793
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.lock_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore3Test, MutexLockExIsrReturnsWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.lock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::would_block);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphore3Test, MutexUnlockExFailure2) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.unlock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

TEST_F(CoverageSemaphore3Test, MutexUnlockExIsrReturnsNotOwned) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.unlock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::semaphore_not_owned);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphore3Test, MutexTryLockExFailure2) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.try_lock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore3Test, MutexTryLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto result = mtx.try_lock_ex();
  EXPECT_TRUE(result.has_value());
}

// ============================================================
// mutex — locked() accessor
// Line 822
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexLockedAccessor) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  EXPECT_FALSE(mtx.locked());
}

// ============================================================
// mutex — destructor for moved-from object (null semaphore)
// Lines 640-644
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexDestructorMovedFrom) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  {
    Mutex mtx1;
    Mutex mtx2(std::move(mtx1));
  }
}

// ============================================================
// recursive_mutex — SFINAE constructor with AllocatorArgs
// Lines 847-849, 851
// ============================================================

TEST_F(CoverageSemaphore3Test, RecursiveMutexSfinaeConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  freertos::static_semaphore_allocator alloc;
  RecursiveMutexStatic rm(std::move(alloc));
}

// ============================================================
// recursive_mutex — Move constructor
// Lines 853-860
// ============================================================

TEST_F(CoverageSemaphore3Test, RecursiveMutexMoveConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm1;
  RecursiveMutex rm2(std::move(rm1));
}

// ============================================================
// recursive_mutex — Move assignment
// Lines 873, 876-879
// ============================================================

TEST_F(CoverageSemaphore3Test, RecursiveMutexMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive))
      .WillOnce(Return(mock_recursive2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  RecursiveMutex rm1;
  RecursiveMutex rm2;
  rm1 = std::move(rm2);
}

TEST_F(CoverageSemaphore3Test, RecursiveMutexMoveAssignmentSelf) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm1;
  rm1 = std::move(rm1);
}

// ============================================================
// recursive_mutex — swap member + friend
// Lines 882, 884-887, 889-891
// ============================================================

TEST_F(CoverageSemaphore3Test, RecursiveMutexFriendSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive))
      .WillOnce(Return(mock_recursive2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  RecursiveMutex rm1;
  RecursiveMutex rm2;
  swap(rm1, rm2);
}

TEST_F(CoverageSemaphore3Test, RecursiveMutexSwapMember) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive))
      .WillOnce(Return(mock_recursive2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  RecursiveMutex rm1;
  RecursiveMutex rm2;
  rm1.swap(rm2);
}

// ============================================================
// recursive_mutex — lock_ex would_block, unlock_ex, try_lock_ex
// Lines 957, 965, 972
// ============================================================

TEST_F(CoverageSemaphore3Test, RecursiveMutexLockExWouldBlock) {
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

TEST_F(CoverageSemaphore3Test, RecursiveMutexUnlockExFailure2) {
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

TEST_F(CoverageSemaphore3Test, RecursiveMutexTryLockExFailure2) {
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

TEST_F(CoverageSemaphore3Test, RecursiveMutexTryLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  auto result = rm.try_lock_ex();
  EXPECT_TRUE(result.has_value());
}

// ============================================================
// recursive_mutex — locked(), recursions_count()
// Lines 1001, 1007
// ============================================================

TEST_F(CoverageSemaphore3Test, RecursiveMutexLockedAccessor) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  EXPECT_FALSE(rm.locked());
  EXPECT_EQ(rm.recursions_count(), 0);
}

TEST_F(CoverageSemaphore3Test, RecursiveMutexLockedAfterRecursion) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  rm.lock();
  EXPECT_TRUE(rm.locked());
  EXPECT_EQ(rm.recursions_count(), 1);
}

// ============================================================
// recursive_mutex — destructor for moved-from object (null semaphore)
// Lines 866-870
// ============================================================

TEST_F(CoverageSemaphore3Test, RecursiveMutexDestructorMovedFrom) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  {
    RecursiveMutex rm1;
    RecursiveMutex rm2(std::move(rm1));
  }
}

// ============================================================
// Guard classes — try_lock_guard
// Lines 1067-1069, 1074-1078, 1091
// ============================================================

TEST_F(CoverageSemaphore3Test, TryLockGuardSuccess) {
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

TEST_F(CoverageSemaphore3Test, TryLockGuardFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::try_lock_guard<Mutex> guard(mtx);
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(CoverageSemaphore3Test, TryLockGuardLockedMethod) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::try_lock_guard<Mutex> guard(mtx);
    EXPECT_TRUE(guard.locked());
    EXPECT_TRUE(mtx.locked());
  }
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphore3Test, TryLockGuardLockedMethodFail) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::try_lock_guard<Mutex> guard(mtx);
    EXPECT_FALSE(guard.locked());
    EXPECT_FALSE(mtx.locked());
  }
}

// ============================================================
// Guard classes — lock_guard_isr with recursive_mutex
// Lines 1103, 1111-1115, 1120-1123
// ============================================================

// lock_guard_isr uses lock_isr/unlock_isr which only exist on mutex, not recursive_mutex.
// So we test with Mutex only.

TEST_F(CoverageSemaphore3Test, LockGuardIsrFullCycle) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::lock_guard_isr<Mutex> guard(mtx);
    EXPECT_FALSE(guard.locked());
    EXPECT_EQ(guard.high_priority_task_woken(), pdFALSE);
  }
}

TEST_F(CoverageSemaphore3Test, LockGuardIsrFailCycle) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::lock_guard_isr<Mutex> guard(mtx);
    EXPECT_FALSE(guard.locked());
    EXPECT_EQ(guard.high_priority_task_woken(), pdFALSE);
  }
}

// ============================================================
// Guard classes — timeout_lock_guard with TickType_t and chrono
// Lines 1166-1169, 1188-1192, 1205
// ============================================================

TEST_F(CoverageSemaphore3Test, TimeoutLockGuardTicksSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 100)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::timeout_lock_guard<Mutex> guard(mtx, static_cast<TickType_t>(100));
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore3Test, TimeoutLockGuardTicksFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::timeout_lock_guard<Mutex> guard(mtx, static_cast<TickType_t>(100));
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(CoverageSemaphore3Test, TimeoutLockGuardChronoSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, pdMS_TO_TICKS(200)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::timeout_lock_guard<Mutex> guard(
        mtx, std::chrono::milliseconds(200));
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore3Test, TimeoutLockGuardChronoFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, pdMS_TO_TICKS(200)))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::timeout_lock_guard<Mutex> guard(
        mtx, std::chrono::milliseconds(200));
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(CoverageSemaphore3Test, TimeoutLockGuardLockedMethod) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 100)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::timeout_lock_guard<Mutex> guard(mtx, static_cast<TickType_t>(100));
    EXPECT_TRUE(guard.locked());
    EXPECT_TRUE(mtx.locked());
  }
}

TEST_F(CoverageSemaphore3Test, TimeoutLockGuardLockedMethodFail) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  {
    freertos::timeout_lock_guard<Mutex> guard(mtx, static_cast<TickType_t>(100));
    EXPECT_FALSE(guard.locked());
  }
}

// ============================================================
// timeout_lock_guard with recursive_mutex
// Lines 1166-1169, 1188-1192, 1205
// ============================================================

TEST_F(CoverageSemaphore3Test, TimeoutLockGuardRecursiveMutexSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 100))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  {
    freertos::timeout_lock_guard<RecursiveMutex> guard(
        rm, static_cast<TickType_t>(100));
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore3Test, TimeoutLockGuardRecursiveMutexFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  {
    freertos::timeout_lock_guard<RecursiveMutex> guard(
        rm, static_cast<TickType_t>(100));
    EXPECT_FALSE(guard.locked());
  }
}

// ============================================================
// lock_guard with recursive_mutex
// Lines 1016-1048
// ============================================================

TEST_F(CoverageSemaphore3Test, LockGuardRecursiveMutex) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  {
    freertos::lock_guard<RecursiveMutex> guard(rm);
    EXPECT_TRUE(guard.locked());
  }
}

// ============================================================
// try_lock_guard with recursive_mutex
// Lines 1056-1092
// ============================================================

TEST_F(CoverageSemaphore3Test, TryLockGuardRecursiveMutexSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  {
    freertos::try_lock_guard<RecursiveMutex> guard(rm);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore3Test, TryLockGuardRecursiveMutexFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  {
    freertos::try_lock_guard<RecursiveMutex> guard(rm);
    EXPECT_FALSE(guard.locked());
  }
}

// ============================================================
// static_semaphore_allocator move constructor and swap
// Lines 64, 70-73
// ============================================================

TEST_F(CoverageSemaphore3Test, StaticAllocatorSwap) {
  freertos::static_semaphore_allocator alloc1;
  freertos::static_semaphore_allocator alloc2;
  alloc1.swap(alloc2);
}

TEST_F(CoverageSemaphore3Test, StaticAllocatorMoveConstruct) {
  freertos::static_semaphore_allocator alloc1;
  freertos::static_semaphore_allocator alloc2(std::move(alloc1));
  (void)alloc2;
}

// ============================================================
// dynamic_semaphore_allocator swap
// Line 97
// ============================================================

TEST_F(CoverageSemaphore3Test, DynamicAllocatorSwap) {
  freertos::dynamic_semaphore_allocator alloc1;
  freertos::dynamic_semaphore_allocator alloc2;
  alloc1.swap(alloc2);
}

// ============================================================
// mutex — unlock_ex success path
// Line 771-776 fully
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexUnlockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  mtx.lock();
  auto result = mtx.unlock_ex();
  EXPECT_TRUE(result.has_value());
}

// ============================================================
// recursive_mutex — unlock_ex success
// Lines 960-964 fully covered
// ============================================================

TEST_F(CoverageSemaphore3Test, RecursiveMutexUnlockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  rm.lock();
  auto result = rm.unlock_ex();
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(rm.recursions_count(), 0);
}

// ============================================================
// recursive_mutex — lock_ex timeout path (non-zero ticks)
// Line 957-958
// ============================================================

TEST_F(CoverageSemaphore3Test, RecursiveMutexLockExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 200))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  auto result = rm.lock_ex(200);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ============================================================
// recursive_mutex — try_lock success
// Lines 943-949 fully
// ============================================================

TEST_F(CoverageSemaphore3Test, RecursiveMutexTryLockSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  auto rc = rm.try_lock();
  EXPECT_EQ(rc, pdTRUE);
  EXPECT_EQ(rm.recursions_count(), 1);
}

TEST_F(CoverageSemaphore3Test, RecursiveMutexTryLockFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutex rm;
  auto rc = rm.try_lock();
  EXPECT_EQ(rc, pdFALSE);
  EXPECT_EQ(rm.recursions_count(), 0);
}

// ============================================================
// mutex — try_lock success/failure
// Lines 745-751
// ============================================================

TEST_F(CoverageSemaphore3Test, MutexTryLockSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto rc = mtx.try_lock();
  EXPECT_EQ(rc, pdTRUE);
  EXPECT_TRUE(mtx.locked());
}

TEST_F(CoverageSemaphore3Test, MutexTryLockFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  Mutex mtx;
  auto rc = mtx.try_lock();
  EXPECT_EQ(rc, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}