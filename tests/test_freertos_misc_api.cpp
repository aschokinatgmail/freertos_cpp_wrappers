#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../include/freertos_task.hpp"
#include "FreeRTOS.h"

using namespace testing;
using namespace freertos;

namespace {
void empty_task_routine() {}
} // namespace

class FreeRTOSMiscApiTest : public Test {
protected:
  void SetUp() override {
    mock = std::make_unique<StrictMock<FreeRTOSMock>>();
    g_freertos_mock = mock.get();
    mock_task_handle = reinterpret_cast<TaskHandle_t>(0x12345678);
  }

  void TearDown() override {
    g_freertos_mock = nullptr;
    mock.reset();
  }

  std::unique_ptr<StrictMock<FreeRTOSMock>> mock;
  TaskHandle_t mock_task_handle;
};

TEST_F(FreeRTOSMiscApiTest, TaskBasePriority) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("BasePriorityTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, uxTaskBasePriorityGet(mock_task_handle))
      .WillOnce(Return(3));
  UBaseType_t bp = test_task.base_priority();
  EXPECT_EQ(bp, static_cast<UBaseType_t>(3));
}

TEST_F(FreeRTOSMiscApiTest, PeriodicTaskBasePriority) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task(
      "PeriodicBasePriorityTask", 2, empty_task_routine, empty_task_routine,
      empty_task_routine, std::chrono::milliseconds(100));

  EXPECT_CALL(*mock, uxTaskBasePriorityGet(mock_task_handle))
      .WillOnce(Return(5));
  UBaseType_t bp = periodic_task.base_priority();
  EXPECT_EQ(bp, static_cast<UBaseType_t>(5));
}

TEST_F(FreeRTOSMiscApiTest, IsIsrReturnsTrue) {
  EXPECT_CALL(*mock, xPortIsInsideInterrupt()).WillOnce(Return(pdTRUE));
  EXPECT_TRUE(is_isr());
}

TEST_F(FreeRTOSMiscApiTest, IsIsrReturnsFalse) {
  EXPECT_CALL(*mock, xPortIsInsideInterrupt()).WillOnce(Return(pdFALSE));
  EXPECT_FALSE(is_isr());
}

TEST_F(FreeRTOSMiscApiTest, CatchUpTicks) {
  EXPECT_CALL(*mock, xTaskCatchUpTicks(100));
  catch_up_ticks(100);
}

TEST_F(FreeRTOSMiscApiTest, CatchUpTicksZero) {
  EXPECT_CALL(*mock, xTaskCatchUpTicks(0));
  catch_up_ticks(0);
}

TEST_F(FreeRTOSMiscApiTest, TimerDaemonTaskHandle) {
  TaskHandle_t daemon_handle = reinterpret_cast<TaskHandle_t>(0xDEADBEEF);
  EXPECT_CALL(*mock, xTimerGetTimerDaemonTaskHandle())
      .WillOnce(Return(daemon_handle));
  TaskHandle_t result = timer_daemon_task_handle();
  EXPECT_EQ(result, daemon_handle);
}

TEST_F(FreeRTOSMiscApiTest, TimerDaemonTaskHandleNull) {
  EXPECT_CALL(*mock, xTimerGetTimerDaemonTaskHandle())
      .WillOnce(Return(nullptr));
  TaskHandle_t result = timer_daemon_task_handle();
  EXPECT_EQ(result, nullptr);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}