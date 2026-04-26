#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks/freertos_mocks.hpp"
#include "freertos_semaphore.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

using freertos::error;
using freertos::expected;
using freertos::unexpected;

class CoverageSemaphore4Test : public ::testing::Test {
protected:
  void SetUp() override {
    mock = FreeRTOSMockInstance::getInstance().get();
    mock_sem = reinterpret_cast<SemaphoreHandle_t>(0x1000);
    mock_sem2 = reinterpret_cast<SemaphoreHandle_t>(0x1100);
    mock_counting = reinterpret_cast<SemaphoreHandle_t>(0x2000);
    mock_counting2 = reinterpret_cast<SemaphoreHandle_t>(0x2100);
    mock_mutex = reinterpret_cast<SemaphoreHandle_t>(0x3000);
    mock_mutex2 = reinterpret_cast<SemaphoreHandle_t>(0x3100);
    mock_recursive = reinterpret_cast<SemaphoreHandle_t>(0x4000);
    mock_recursive2 = reinterpret_cast<SemaphoreHandle_t>(0x4100);
  }

  void TearDown() override { FreeRTOSMockInstance::resetInstance(); }

  FreeRTOSMock *mock;
  SemaphoreHandle_t mock_sem;
  SemaphoreHandle_t mock_sem2;
  SemaphoreHandle_t mock_counting;
  SemaphoreHandle_t mock_counting2;
  SemaphoreHandle_t mock_mutex;
  SemaphoreHandle_t mock_mutex2;
  SemaphoreHandle_t mock_recursive;
  SemaphoreHandle_t mock_recursive2;
};

using BinarySemSA = freertos::sa::binary_semaphore;
using CountingSemSA = freertos::sa::counting_semaphore;
using MutexSA = freertos::sa::mutex;
using RecursiveMutexSA = freertos::sa::recursive_mutex;

// ============================================================
// binary_semaphore (static allocator) — Default constructor
// xSemaphoreCreateBinaryStatic
// ============================================================

TEST_F(CoverageSemaphore4Test, SaBinaryDefaultConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
}

// ============================================================
// binary_semaphore (static allocator) — Move constructor
// moved-from destructor with null handle
// ============================================================

TEST_F(CoverageSemaphore4Test, SaBinaryMoveConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem1;
  BinarySemSA sem2(std::move(sem1));
}

// ============================================================
// binary_semaphore (static allocator) — Move assignment + self-assignment
// ============================================================

TEST_F(CoverageSemaphore4Test, SaBinaryMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem))
      .WillOnce(Return(mock_sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  BinarySemSA sem1;
  BinarySemSA sem2;
  sem1 = std::move(sem2);
}

TEST_F(CoverageSemaphore4Test, SaBinaryMoveAssignmentSelf) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem1;
  sem1 = std::move(sem1);
}

// ============================================================
// binary_semaphore (static allocator) — Member swap + friend swap
// ============================================================

TEST_F(CoverageSemaphore4Test, SaBinaryMemberSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem))
      .WillOnce(Return(mock_sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  BinarySemSA sem1;
  BinarySemSA sem2;
  sem1.swap(sem2);
}

TEST_F(CoverageSemaphore4Test, SaBinaryFriendSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem))
      .WillOnce(Return(mock_sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  BinarySemSA sem1;
  BinarySemSA sem2;
  swap(sem1, sem2);
}

// ============================================================
// binary_semaphore (static allocator) — give_isr / take_isr
// ============================================================

TEST_F(CoverageSemaphore4Test, SaBinaryGiveIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.give_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore4Test, SaBinaryGiveIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.give_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(CoverageSemaphore4Test, SaBinaryTakeIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.take_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore4Test, SaBinaryTakeIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.take_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

// ============================================================
// binary_semaphore (static allocator) — give_ex failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaBinaryGiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.give_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

// ============================================================
// binary_semaphore (static allocator) — give_ex_isr success and failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaBinaryGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.give_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore4Test, SaBinaryGiveExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.give_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::semaphore_not_owned);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

// ============================================================
// binary_semaphore (static allocator) — take_ex would_block and timeout
// ============================================================

TEST_F(CoverageSemaphore4Test, SaBinaryTakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.take_ex(static_cast<TickType_t>(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore4Test, SaBinaryTakeExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_sem, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.take_ex(static_cast<TickType_t>(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ============================================================
// binary_semaphore (static allocator) — take_ex_isr success and failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaBinaryTakeExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.take_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(CoverageSemaphore4Test, SaBinaryTakeExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
      .WillOnce(Return(mock_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_sem));
  BinarySemSA sem;
  auto result = sem.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::would_block);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

// ============================================================
// counting_semaphore (static allocator) — Default constructor
// xSemaphoreCreateCountingStatic
// ============================================================

TEST_F(CoverageSemaphore4Test, SaCountingDefaultConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
}

// ============================================================
// counting_semaphore (static allocator) — Move constructor
// ============================================================

TEST_F(CoverageSemaphore4Test, SaCountingMoveConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem1(5);
  CountingSemSA sem2(std::move(sem1));
}

// ============================================================
// counting_semaphore (static allocator) — Move assignment + self-assignment
// ============================================================

TEST_F(CoverageSemaphore4Test, SaCountingMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting))
      .WillOnce(Return(mock_counting2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  CountingSemSA sem1(5);
  CountingSemSA sem2(5);
  sem1 = std::move(sem2);
}

TEST_F(CoverageSemaphore4Test, SaCountingMoveAssignmentSelf) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem1(5);
  sem1 = std::move(sem1);
}

// ============================================================
// counting_semaphore (static allocator) — Member swap + friend swap
// ============================================================

TEST_F(CoverageSemaphore4Test, SaCountingMemberSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting))
      .WillOnce(Return(mock_counting2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  CountingSemSA sem1(5);
  CountingSemSA sem2(5);
  sem1.swap(sem2);
}

TEST_F(CoverageSemaphore4Test, SaCountingFriendSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting))
      .WillOnce(Return(mock_counting2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  CountingSemSA sem1(5);
  CountingSemSA sem2(5);
  swap(sem1, sem2);
}

// ============================================================
// counting_semaphore (static allocator) — give_isr / take_isr
// ============================================================

TEST_F(CoverageSemaphore4Test, SaCountingGiveIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.give_isr();
  EXPECT_EQ(result.result, pdTRUE);
}

TEST_F(CoverageSemaphore4Test, SaCountingGiveIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.give_isr();
  EXPECT_EQ(result.result, pdFALSE);
}

TEST_F(CoverageSemaphore4Test, SaCountingTakeIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.take_isr();
  EXPECT_EQ(result.result, pdTRUE);
}

TEST_F(CoverageSemaphore4Test, SaCountingTakeIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.take_isr();
  EXPECT_EQ(result.result, pdFALSE);
}

// ============================================================
// counting_semaphore (static allocator) — give_ex failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaCountingGiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_counting))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.give_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

// ============================================================
// counting_semaphore (static allocator) — give_ex_isr success and failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaCountingGiveExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.give_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(CoverageSemaphore4Test, SaCountingGiveExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.give_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::semaphore_not_owned);
}

// ============================================================
// counting_semaphore (static allocator) — take_ex would_block and timeout
// ============================================================

TEST_F(CoverageSemaphore4Test, SaCountingTakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.take_ex(static_cast<TickType_t>(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore4Test, SaCountingTakeExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_counting, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.take_ex(static_cast<TickType_t>(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ============================================================
// counting_semaphore (static allocator) — take_ex_isr success and failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaCountingTakeExIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.take_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(CoverageSemaphore4Test, SaCountingTakeExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(mock_counting));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_counting, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_counting));
  CountingSemSA sem(5);
  auto result = sem.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::would_block);
}

// ============================================================
// mutex (static allocator) — Default constructor
// xSemaphoreCreateMutexStatic
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexDefaultConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
}

// ============================================================
// mutex (static allocator) — Move constructor
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexMoveConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx1;
  MutexSA mtx2(std::move(mtx1));
}

// ============================================================
// mutex (static allocator) — Move assignment + self-assignment
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex))
      .WillOnce(Return(mock_mutex2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  MutexSA mtx1;
  MutexSA mtx2;
  mtx1 = std::move(mtx2);
}

TEST_F(CoverageSemaphore4Test, SaMutexMoveAssignmentSelf) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx1;
  mtx1 = std::move(mtx1);
}

// ============================================================
// mutex (static allocator) — Member swap (with locked state) + friend swap
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexMemberSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex))
      .WillOnce(Return(mock_mutex2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  MutexSA mtx1;
  MutexSA mtx2;
  mtx1.swap(mtx2);
}

TEST_F(CoverageSemaphore4Test, SaMutexFriendSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex))
      .WillOnce(Return(mock_mutex2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  MutexSA mtx1;
  MutexSA mtx2;
  swap(mtx1, mtx2);
}

TEST_F(CoverageSemaphore4Test, SaMutexSwapWithLockedState) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex))
      .WillOnce(Return(mock_mutex2));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  MutexSA mtx1;
  MutexSA mtx2;
  mtx1.lock();
  EXPECT_TRUE(mtx1.locked());
  EXPECT_FALSE(mtx2.locked());
  mtx1.swap(mtx2);
  EXPECT_FALSE(mtx1.locked());
  EXPECT_TRUE(mtx2.locked());
}

// ============================================================
// mutex (static allocator) — lock_isr / unlock_isr success and failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexLockIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto result = mtx.lock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphore4Test, SaMutexUnlockIsrReturnsFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto result = mtx.unlock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

// ============================================================
// mutex (static allocator) — lock_ex would_block and timeout
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto result = mtx.lock_ex(static_cast<TickType_t>(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore4Test, SaMutexLockExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 200))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto result = mtx.lock_ex(static_cast<TickType_t>(200));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ============================================================
// mutex (static allocator) — lock_ex_isr success and failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexLockExIsrReturnsWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto result = mtx.lock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::would_block);
  EXPECT_FALSE(mtx.locked());
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

// ============================================================
// mutex (static allocator) — unlock_ex failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto result = mtx.unlock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

// ============================================================
// mutex (static allocator) — unlock_ex_isr success and failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexUnlockExIsrReturnsNotOwned) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto result = mtx.unlock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), error::semaphore_not_owned);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

// ============================================================
// mutex (static allocator) — try_lock_ex failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexTryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto result = mtx.try_lock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ============================================================
// recursive_mutex (static allocator) — Default constructor
// xSemaphoreCreateRecursiveMutexStatic
// ============================================================

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexDefaultConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
}

// ============================================================
// recursive_mutex (static allocator) — Move constructor
// ============================================================

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexMoveConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm1;
  RecursiveMutexSA rm2(std::move(rm1));
}

// ============================================================
// recursive_mutex (static allocator) — Move assignment + self-assignment
// ============================================================

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive))
      .WillOnce(Return(mock_recursive2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  RecursiveMutexSA rm1;
  RecursiveMutexSA rm2;
  rm1 = std::move(rm2);
}

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexMoveAssignmentSelf) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm1;
  rm1 = std::move(rm1);
}

// ============================================================
// recursive_mutex (static allocator) — Member swap (with recursion count)
// + friend swap
// ============================================================

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexMemberSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive))
      .WillOnce(Return(mock_recursive2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  RecursiveMutexSA rm1;
  RecursiveMutexSA rm2;
  rm1.swap(rm2);
}

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexFriendSwap) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive))
      .WillOnce(Return(mock_recursive2));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  RecursiveMutexSA rm1;
  RecursiveMutexSA rm2;
  swap(rm1, rm2);
}

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexSwapWithRecursionCount) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive))
      .WillOnce(Return(mock_recursive2));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, portMAX_DELAY))
      .WillOnce(Return(pdTRUE))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(_)).Times(2);
  RecursiveMutexSA rm1;
  RecursiveMutexSA rm2;
  rm1.lock();
  rm1.lock();
  EXPECT_EQ(rm1.recursions_count(), 2);
  EXPECT_EQ(rm2.recursions_count(), 0);
  rm1.swap(rm2);
  EXPECT_EQ(rm1.recursions_count(), 0);
  EXPECT_EQ(rm2.recursions_count(), 2);
}

// ============================================================
// recursive_mutex (static allocator) — lock_ex would_block and timeout
// ============================================================

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto result = rm.lock_ex(static_cast<TickType_t>(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexLockExTimeout) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 200))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto result = rm.lock_ex(static_cast<TickType_t>(200));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::timeout);
}

// ============================================================
// recursive_mutex (static allocator) — unlock_ex failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto result = rm.unlock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::semaphore_not_owned);
}

// ============================================================
// recursive_mutex (static allocator) — try_lock_ex failure
// ============================================================

TEST_F(CoverageSemaphore4Test, SaRecursiveMutexTryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto result = rm.try_lock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), error::would_block);
}

// ============================================================
// Guard classes on static mutex — try_lock_guard
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexTryLockGuardSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  {
    freertos::try_lock_guard<MutexSA> guard(mtx);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore4Test, SaMutexTryLockGuardFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  {
    freertos::try_lock_guard<MutexSA> guard(mtx);
    EXPECT_FALSE(guard.locked());
  }
}

// ============================================================
// Guard classes on static mutex — lock_guard_isr
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexLockGuardIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  {
    freertos::lock_guard_isr<MutexSA> guard(mtx);
    EXPECT_FALSE(guard.locked());
    EXPECT_EQ(guard.high_priority_task_woken(), pdFALSE);
  }
}

// ============================================================
// Guard classes on static mutex — timeout_lock_guard
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexTimeoutLockGuardTicksSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 100)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  {
    freertos::timeout_lock_guard<MutexSA> guard(
        mtx, static_cast<TickType_t>(100));
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore4Test, SaMutexTimeoutLockGuardTicksFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  {
    freertos::timeout_lock_guard<MutexSA> guard(
        mtx, static_cast<TickType_t>(100));
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(CoverageSemaphore4Test, SaMutexTimeoutLockGuardChronoSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, pdMS_TO_TICKS(200)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  {
    freertos::timeout_lock_guard<MutexSA> guard(
        mtx, std::chrono::milliseconds(200));
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore4Test, SaMutexTimeoutLockGuardChronoFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, pdMS_TO_TICKS(200)))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  {
    freertos::timeout_lock_guard<MutexSA> guard(
        mtx, std::chrono::milliseconds(200));
    EXPECT_FALSE(guard.locked());
  }
}

// ============================================================
// Guard classes on static mutex — locked() accessor edge cases
// ============================================================

TEST_F(CoverageSemaphore4Test, SaMutexLockedAccessor) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphore4Test, SaMutexTryLockGuardLockedEdge) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  {
    freertos::try_lock_guard<MutexSA> guard(mtx);
    EXPECT_TRUE(guard.locked());
    EXPECT_TRUE(mtx.locked());
  }
  EXPECT_FALSE(mtx.locked());
}

TEST_F(CoverageSemaphore4Test, SaMutexTryLockGuardLockedEdgeFail) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  {
    freertos::try_lock_guard<MutexSA> guard(mtx);
    EXPECT_FALSE(guard.locked());
    EXPECT_FALSE(mtx.locked());
  }
}

TEST_F(CoverageSemaphore4Test, SaMutexTimeoutLockGuardLockedEdgeFail) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  {
    freertos::timeout_lock_guard<MutexSA> guard(
        mtx, static_cast<TickType_t>(100));
    EXPECT_FALSE(guard.locked());
  }
}