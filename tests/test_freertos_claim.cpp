#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_semaphore.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

class ClaimTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_mutex_handle = reinterpret_cast<SemaphoreHandle_t>(0x87654321);
    mock_recursive_mutex_handle =
        reinterpret_cast<SemaphoreHandle_t>(0x11223344);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_mutex_handle;
  SemaphoreHandle_t mock_recursive_mutex_handle;
};

TEST_F(ClaimTest, MutexClaimReturnsValue) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::mutex<freertos::dynamic_semaphore_allocator> m;

  int result = m.claim([]() { return 42; });
  EXPECT_EQ(result, 42);
}

TEST_F(ClaimTest, MutexClaimVoid) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::mutex<freertos::dynamic_semaphore_allocator> m;

  bool called = false;
  m.claim([&called]() { called = true; });
  EXPECT_TRUE(called);
}

TEST_F(ClaimTest, MutexWithLockAlias) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::mutex<freertos::dynamic_semaphore_allocator> m;

  auto result = m.with_lock([]() { return 100; });
  EXPECT_EQ(result, 100);
}

TEST_F(ClaimTest, MutexClaimReference) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::mutex<freertos::dynamic_semaphore_allocator> m;

  std::string value = m.claim([]() { return std::string("hello"); });
  EXPECT_EQ(value, "hello");
}

TEST_F(ClaimTest, RecursiveMutexClaim) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive_mutex_handle));
  EXPECT_CALL(*mock,
              xSemaphoreTakeRecursive(mock_recursive_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));

  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> rm;

  int result = rm.claim([]() { return 99; });
  EXPECT_EQ(result, 99);
}

TEST_F(ClaimTest, RecursiveMutexWithLock) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_recursive_mutex_handle));
  EXPECT_CALL(*mock,
              xSemaphoreTakeRecursive(mock_recursive_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_recursive_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));

  freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> rm;

  auto result = rm.with_lock([]() { return 77; });
  EXPECT_EQ(result, 77);
}

TEST_F(ClaimTest, MutexClaimModifiesVariable) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex())
      .WillOnce(Return(mock_mutex_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));

  freertos::mutex<freertos::dynamic_semaphore_allocator> m;

  int value = 0;
  m.claim([&value]() { value = 123; });
  EXPECT_EQ(value, 123);
}