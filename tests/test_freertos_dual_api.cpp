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

class DualApiBinarySemaphoreTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_handle = reinterpret_cast<SemaphoreHandle_t>(0x1000);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_handle;
};

TEST_F(DualApiBinarySemaphoreTest, AcquireDefault) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  sem.acquire();
}

TEST_F(DualApiBinarySemaphoreTest, AcquireWithTicks) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, 100)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  sem.acquire(100);
}

TEST_F(DualApiBinarySemaphoreTest, AcquireWithChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  sem.acquire(std::chrono::milliseconds(50));
}

TEST_F(DualApiBinarySemaphoreTest, TryAcquireSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  EXPECT_TRUE(sem.try_acquire());
}

TEST_F(DualApiBinarySemaphoreTest, TryAcquireFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  EXPECT_FALSE(sem.try_acquire());
}

TEST_F(DualApiBinarySemaphoreTest, Release) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  sem.release();
}

TEST_F(DualApiBinarySemaphoreTest, ReleaseIsr) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_handle, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> sem;
  auto result = sem.release_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

class DualApiCountingSemaphoreTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_handle = reinterpret_cast<SemaphoreHandle_t>(0x2000);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_handle;
};

TEST_F(DualApiCountingSemaphoreTest, AcquireDefault) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  sem.acquire();
}

TEST_F(DualApiCountingSemaphoreTest, TryAcquireSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  EXPECT_TRUE(sem.try_acquire());
}

TEST_F(DualApiCountingSemaphoreTest, TryAcquireFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  EXPECT_FALSE(sem.try_acquire());
}

TEST_F(DualApiCountingSemaphoreTest, Release) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> sem(5);
  sem.release();
}

class DualApiMutexTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_handle = reinterpret_cast<SemaphoreHandle_t>(0x3000);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_handle;
};

TEST_F(DualApiMutexTest, TryLockForSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, 100)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  EXPECT_TRUE(m.try_lock_for(std::chrono::milliseconds(100)));
}

TEST_F(DualApiMutexTest, TryLockForFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  EXPECT_FALSE(m.try_lock_for(std::chrono::milliseconds(100)));
}

TEST_F(DualApiMutexTest, TryLockUntilSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto deadline =
      std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
  EXPECT_TRUE(m.try_lock_until(deadline));
}

TEST_F(DualApiMutexTest, TryLockUntilPastDeadline) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::mutex<freertos::dynamic_semaphore_allocator> m;
  auto past = std::chrono::steady_clock::now() - std::chrono::milliseconds(100);
  EXPECT_FALSE(m.try_lock_until(past));
}

class DualApiRecursiveMutexTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_handle = reinterpret_cast<SemaphoreHandle_t>(0x4000);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_handle;
};

TEST_F(DualApiRecursiveMutexTest, TryLockForSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_handle, 100))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> m;
  EXPECT_TRUE(m.try_lock_for(std::chrono::milliseconds(100)));
}

TEST_F(DualApiRecursiveMutexTest, TryLockForFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_handle, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> m;
  EXPECT_FALSE(m.try_lock_for(std::chrono::milliseconds(100)));
}

TEST_F(DualApiRecursiveMutexTest, TryLockUntilSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_handle, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> m;
  auto deadline =
      std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
  EXPECT_TRUE(m.try_lock_until(deadline));
}

TEST_F(DualApiRecursiveMutexTest, TryLockUntilPastDeadline) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_handle, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));
  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> m;
  auto past = std::chrono::steady_clock::now() - std::chrono::milliseconds(100);
  EXPECT_FALSE(m.try_lock_until(past));
}