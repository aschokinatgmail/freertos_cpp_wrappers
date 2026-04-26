#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "../include/freertos_isr_result.hpp"
#include "../include/freertos_task.hpp"

using namespace testing;
using namespace freertos;

namespace {
void empty_task_routine() {}
} // namespace

class FreeRTOSTaskJoinTest : public Test {
protected:
  void SetUp() override {
    mock = std::make_unique<StrictMock<FreeRTOSMock>>();
    g_freertos_mock = mock.get();
    mock_task_handle = reinterpret_cast<TaskHandle_t>(0x12345678);
    mock_current_handle = reinterpret_cast<TaskHandle_t>(0xDEADBEEF);
  }

  void TearDown() override {
    g_freertos_mock = nullptr;
    mock.reset();
  }

  std::unique_ptr<StrictMock<FreeRTOSMock>> mock;
  TaskHandle_t mock_task_handle;
  TaskHandle_t mock_current_handle;
};

TEST_F(FreeRTOSTaskJoinTest, JoinableReturnsTrueForValidHandle) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("JoinableTask", 2, empty_task_routine);
  EXPECT_TRUE(test_task.joinable());
}

TEST_F(FreeRTOSTaskJoinTest, JoinableReturnsFalseForNullHandle) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vTaskDelete(_)).Times(0);

  sa::task<1024> test_task("NullTask", 2, empty_task_routine);
  EXPECT_FALSE(test_task.joinable());
}

TEST_F(FreeRTOSTaskJoinTest, JoinableReturnsFalseAfterMove) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> original_task("MoveTask", 2, empty_task_routine);
  EXPECT_TRUE(original_task.joinable());

  sa::task<1024> moved_task = std::move(original_task);
  EXPECT_FALSE(original_task.joinable());
  EXPECT_TRUE(moved_task.joinable());
}

TEST_F(FreeRTOSTaskJoinTest, JoinCallsNotifyAndWait) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));

  EXPECT_CALL(*mock, xTaskGetCurrentTaskHandle())
      .WillOnce(Return(mock_current_handle));
  EXPECT_CALL(*mock, ulTaskNotifyTake(pdTRUE, portMAX_DELAY))
      .WillOnce(Return(1));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("JoinTask", 2, empty_task_routine);
  test_task.join();
  EXPECT_FALSE(test_task.joinable());
}

TEST_F(FreeRTOSTaskJoinTest, JoinableReturnsFalseAfterJoin) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));

  EXPECT_CALL(*mock, xTaskGetCurrentTaskHandle())
      .WillOnce(Return(mock_current_handle));
  EXPECT_CALL(*mock, ulTaskNotifyTake(pdTRUE, portMAX_DELAY))
      .WillOnce(Return(1));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("JoinAndCheck", 2, empty_task_routine);
  test_task.join();
  EXPECT_FALSE(test_task.joinable());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}