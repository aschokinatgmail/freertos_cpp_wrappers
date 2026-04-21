#include <chrono>
#include <cstdlib>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks/freertos_mocks.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_external_allocator.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

using freertos::error;
using freertos::expected;
using freertos::unexpected;

class CoverageSemaphore5Test : public ::testing::Test {
protected:
  void SetUp() override {
    mock = FreeRTOSMockInstance::getInstance().get();
    mock_sem = reinterpret_cast<SemaphoreHandle_t>(0x1000);
    mock_mutex = reinterpret_cast<SemaphoreHandle_t>(0x3000);
    mock_recursive = reinterpret_cast<SemaphoreHandle_t>(0x4000);
  }

  void TearDown() override { FreeRTOSMockInstance::resetInstance(); }

  FreeRTOSMock *mock;
  SemaphoreHandle_t mock_sem;
  SemaphoreHandle_t mock_mutex;
  SemaphoreHandle_t mock_recursive;
};

using MutexSA = freertos::sa::mutex;
using RecursiveMutexSA = freertos::sa::recursive_mutex;

using TestMemoryRegion = freertos::external_memory_region;

static void *test_allocate(size_t size) { return malloc(size); }
static void test_deallocate(void *ptr) { free(ptr); }

// ============================================================
// freertos::sa::mutex — lock with std::chrono::milliseconds
// ============================================================

TEST_F(CoverageSemaphore5Test, SaMutexLockChronoSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, pdMS_TO_TICKS(500)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto rc = mtx.lock(std::chrono::milliseconds(500));
  EXPECT_EQ(rc, pdTRUE);
  EXPECT_TRUE(mtx.locked());
  mtx.unlock();
}

TEST_F(CoverageSemaphore5Test, SaMutexLockChronoFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, pdMS_TO_TICKS(500)))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto rc = mtx.lock(std::chrono::milliseconds(500));
  EXPECT_EQ(rc, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}

// ============================================================
// freertos::sa::mutex — try_lock_until with system_clock
// ============================================================

TEST_F(CoverageSemaphore5Test, SaMutexTryLockUntilSystemClockPast) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto past = std::chrono::system_clock::now() - std::chrono::hours(1);
  bool result = mtx.try_lock_until(past);
  EXPECT_TRUE(result);
  mtx.unlock();
}

TEST_F(CoverageSemaphore5Test, SaMutexTryLockUntilSystemClockFuture) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto future = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
  bool result = mtx.try_lock_until(future);
  EXPECT_TRUE(result);
  mtx.unlock();
}

TEST_F(CoverageSemaphore5Test, SaMutexTryLockUntilSystemClockFutureFail) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto future = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
  bool result = mtx.try_lock_until(future);
  EXPECT_FALSE(result);
}

// ============================================================
// freertos::sa::mutex — try_lock_until with steady_clock
// ============================================================

TEST_F(CoverageSemaphore5Test, SaMutexTryLockUntilSteadyClockPast) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto past = std::chrono::steady_clock::now() - std::chrono::hours(1);
  bool result = mtx.try_lock_until(past);
  EXPECT_FALSE(result);
}

TEST_F(CoverageSemaphore5Test, SaMutexTryLockUntilSteadyClockFuture) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_))
      .WillOnce(Return(mock_mutex));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex));
  MutexSA mtx;
  auto future = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
  bool result = mtx.try_lock_until(future);
  EXPECT_TRUE(result);
  mtx.unlock();
}

// ============================================================
// freertos::sa::recursive_mutex — lock with
// std::chrono::milliseconds
// ============================================================

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexLockChronoSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, pdMS_TO_TICKS(500)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto rc = rm.lock(std::chrono::milliseconds(500));
  EXPECT_EQ(rc, pdTRUE);
  EXPECT_TRUE(rm.locked());
  rm.unlock();
}

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexLockChronoFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, pdMS_TO_TICKS(500)))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto rc = rm.lock(std::chrono::milliseconds(500));
  EXPECT_EQ(rc, pdFALSE);
  EXPECT_FALSE(rm.locked());
}

// ============================================================
// freertos::sa::recursive_mutex — try_lock_until system_clock
// ============================================================

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTryLockUntilSystemClockPast) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto past = std::chrono::system_clock::now() - std::chrono::hours(1);
  bool result = rm.try_lock_until(past);
  EXPECT_TRUE(result);
  rm.unlock();
}

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTryLockUntilSystemClockFuture) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto future = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
  bool result = rm.try_lock_until(future);
  EXPECT_TRUE(result);
  rm.unlock();
}

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTryLockUntilSystemClockFutureFail) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto future = std::chrono::system_clock::now() + std::chrono::milliseconds(100);
  bool result = rm.try_lock_until(future);
  EXPECT_FALSE(result);
}

// ============================================================
// freertos::sa::recursive_mutex — try_lock_until steady_clock
// ============================================================

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTryLockUntilSteadyClockPast) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto past = std::chrono::steady_clock::now() - std::chrono::hours(1);
  bool result = rm.try_lock_until(past);
  EXPECT_FALSE(result);
}

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTryLockUntilSteadyClockFuture) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  auto future = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
  bool result = rm.try_lock_until(future);
  EXPECT_TRUE(result);
  rm.unlock();
}

// ============================================================
// freertos::sa::recursive_mutex — locked() method
// ============================================================

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexLockedWhenUnlocked) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  EXPECT_FALSE(rm.locked());
}

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexLockedWhenLocked) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  rm.lock();
  EXPECT_TRUE(rm.locked());
  rm.unlock();
  EXPECT_FALSE(rm.locked());
}

// ============================================================
// freertos::try_lock_guard on freertos::sa::recursive_mutex
// ============================================================

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTryLockGuardSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  {
    freertos::try_lock_guard<RecursiveMutexSA> guard(rm);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTryLockGuardFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  {
    freertos::try_lock_guard<RecursiveMutexSA> guard(rm);
    EXPECT_FALSE(guard.locked());
  }
}

// ============================================================
// freertos::timeout_lock_guard on freertos::sa::recursive_mutex
// ============================================================

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTimeoutLockGuardSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 100))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  {
    freertos::timeout_lock_guard<RecursiveMutexSA> guard(
        rm, static_cast<TickType_t>(100));
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTimeoutLockGuardFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  {
    freertos::timeout_lock_guard<RecursiveMutexSA> guard(
        rm, static_cast<TickType_t>(100));
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTimeoutLockGuardChronoSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, pdMS_TO_TICKS(200)))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  {
    freertos::timeout_lock_guard<RecursiveMutexSA> guard(
        rm, std::chrono::milliseconds(200));
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(CoverageSemaphore5Test, SaRecursiveMutexTimeoutLockGuardChronoFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(mock_recursive));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_recursive, pdMS_TO_TICKS(200)))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive));
  RecursiveMutexSA rm;
  {
    freertos::timeout_lock_guard<RecursiveMutexSA> guard(
        rm, std::chrono::milliseconds(200));
    EXPECT_FALSE(guard.locked());
  }
}

// ============================================================
// SFINAE constructors with external_semaphore_allocator
// (SemArgAllocator / SemAllocatorArg coverage)
// ============================================================

using ExtSemAlloc = freertos::external_semaphore_allocator<TestMemoryRegion>;

TEST_F(CoverageSemaphore5Test, ArgAllocatorBinarySemaphoreConstructor) {
  TestMemoryRegion region{test_allocate, test_deallocate};
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5000);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::binary_semaphore<ExtSemAlloc> sem(region);
}

TEST_F(CoverageSemaphore5Test, ArgAllocatorBinarySemaphoreDestructor) {
  TestMemoryRegion region{test_allocate, test_deallocate};
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5001);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  {
    freertos::binary_semaphore<ExtSemAlloc> sem(region);
  }
}

TEST_F(CoverageSemaphore5Test, ArgAllocatorCountingSemaphoreConstructor) {
  TestMemoryRegion region{test_allocate, test_deallocate};
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5002);
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, _))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::counting_semaphore<ExtSemAlloc> sem(5, region);
}

TEST_F(CoverageSemaphore5Test, ArgAllocatorCountingSemaphoreDestructor) {
  TestMemoryRegion region{test_allocate, test_deallocate};
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5003);
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(3, 3, _))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  {
    freertos::counting_semaphore<ExtSemAlloc> sem(3, region);
  }
}

TEST_F(CoverageSemaphore5Test, ArgAllocatorMutexConstructor) {
  TestMemoryRegion region{test_allocate, test_deallocate};
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5004);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::mutex<ExtSemAlloc> mtx(region);
}

TEST_F(CoverageSemaphore5Test, ArgAllocatorMutexDestructor) {
  TestMemoryRegion region{test_allocate, test_deallocate};
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5005);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  {
    freertos::mutex<ExtSemAlloc> mtx(region);
  }
}

TEST_F(CoverageSemaphore5Test, ArgAllocatorRecursiveMutexConstructor) {
  TestMemoryRegion region{test_allocate, test_deallocate};
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5006);
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  freertos::recursive_mutex<ExtSemAlloc> rm(region);
}

TEST_F(CoverageSemaphore5Test, ArgAllocatorRecursiveMutexDestructor) {
  TestMemoryRegion region{test_allocate, test_deallocate};
  SemaphoreHandle_t h = reinterpret_cast<SemaphoreHandle_t>(0x5007);
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(h));
  EXPECT_CALL(*mock, vSemaphoreDelete(h));
  {
    freertos::recursive_mutex<ExtSemAlloc> rm(region);
  }
}