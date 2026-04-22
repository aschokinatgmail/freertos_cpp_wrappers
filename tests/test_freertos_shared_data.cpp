#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_expected.hpp"
#include "freertos_shared_data.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

class SharedDataTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t mock_handle =
      reinterpret_cast<SemaphoreHandle_t>(0x12345678);
};

TEST_F(SharedDataTest, DefaultConstruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int> data;
  EXPECT_EQ(data.get(), 0);
}

TEST_F(SharedDataTest, ValueConstruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int> data(42);
  EXPECT_EQ(data.get(), 42);
}

TEST_F(SharedDataTest, MoveValueConstruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int> data(100);
  EXPECT_EQ(data.get(), 100);
}

TEST_F(SharedDataTest, GetAndSet) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int> data(10);

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(data.get(), 10);

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  data.set(20);

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(data.get(), 20);
}

TEST_F(SharedDataTest, SetMove) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<std::string> data("hello");

  std::string new_value = "world";
  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  data.set(std::move(new_value));

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(data.get(), "world");
}

TEST_F(SharedDataTest, UseLambda) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int> data(5);

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  int result = data.use([](int &v) {
    v *= 2;
    return v;
  });
  EXPECT_EQ(result, 10);

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(data.get(), 10);
}

TEST_F(SharedDataTest, GetExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int> data(42);

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  auto result = data.get_ex();
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(SharedDataTest, GetExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int> data(42);

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdFALSE));
  auto result = data.get_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(SharedDataTest, SetExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int> data(0);

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  auto result = data.set_ex(99);
  EXPECT_TRUE(result.has_value());

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(mock_handle)).WillOnce(Return(pdTRUE));
  EXPECT_EQ(data.get(), 99);
}

TEST_F(SharedDataTest, SetExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int> data(0);

  EXPECT_CALL(*mock, xSemaphoreTake(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdFALSE));
  auto result = data.set_ex(99);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(SharedDataTest, SharedDataWithRecursiveMutex) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(mock_handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(mock_handle));

  freertos::shared_data<int, freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>>
      data(7);

  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(mock_handle, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(mock_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_EQ(data.get(), 7);
}