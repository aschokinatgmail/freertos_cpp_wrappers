#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "FreeRTOS.h"
#include "freertos_isr_result.hpp"
#include "freertos_task.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

using namespace freertos;
using namespace std::chrono_literals;

namespace {
void empty_task_routine() {}
} // namespace

class IndexedNotificationTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_task_handle = reinterpret_cast<TaskHandle_t>(0x12345678);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  TaskHandle_t mock_task_handle;
};

TEST_F(IndexedNotificationTest, TaskNotifyGiveIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, xTaskNotifyGiveIndexed(mock_task_handle, 0))
      .WillOnce(Return(pdTRUE));
  BaseType_t result = test_task.notify_give(0);
  EXPECT_EQ(result, pdTRUE);

  EXPECT_CALL(*mock, xTaskNotifyGiveIndexed(mock_task_handle, 1))
      .WillOnce(Return(pdTRUE));
  result = test_task.notify_give(1);
  EXPECT_EQ(result, pdTRUE);
}

TEST_F(IndexedNotificationTest, TaskNotifyGiveIndexedFailure) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, xTaskNotifyGiveIndexed(mock_task_handle, 0))
      .WillOnce(Return(pdFALSE));
  BaseType_t result = test_task.notify_give(0);
  EXPECT_EQ(result, pdFALSE);
}

TEST_F(IndexedNotificationTest, TaskNotifyTakeIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(0, pdTRUE, portMAX_DELAY))
      .WillOnce(Return(42));
  uint32_t value =
      test_task.notify_take(static_cast<UBaseType_t>(0), pdTRUE, portMAX_DELAY);
  EXPECT_EQ(value, 42);

  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(1, pdFALSE, 100))
      .WillOnce(Return(7));
  value = test_task.notify_take(static_cast<UBaseType_t>(1), pdFALSE, 100);
  EXPECT_EQ(value, 7);
}

TEST_F(IndexedNotificationTest, TaskNotifyTakeIndexedChrono) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(0, pdTRUE, 500))
      .WillOnce(Return(3));
  uint32_t value =
      test_task.notify_take(0, pdTRUE, std::chrono::milliseconds(500));
  EXPECT_EQ(value, 3);

  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(1, pdFALSE, 2000))
      .WillOnce(Return(5));
  value = test_task.notify_take(1, pdFALSE, std::chrono::seconds(2));
  EXPECT_EQ(value, 5);
}

TEST_F(IndexedNotificationTest, TaskNotifyIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, xTaskNotifyIndexed(mock_task_handle, 0, 0x12345678,
                                        eSetValueWithOverwrite))
      .WillOnce(Return(pdTRUE));
  BaseType_t result = test_task.notify(0, 0x12345678, eSetValueWithOverwrite);
  EXPECT_EQ(result, pdTRUE);

  EXPECT_CALL(*mock, xTaskNotifyIndexed(mock_task_handle, 1, 0xFF, eSetBits))
      .WillOnce(Return(pdFALSE));
  result = test_task.notify(1, 0xFF, eSetBits);
  EXPECT_EQ(result, pdFALSE);
}

TEST_F(IndexedNotificationTest, TaskNotifyAndQueryIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  uint32_t prev_value;
  EXPECT_CALL(*mock, xTaskNotifyAndQueryIndexed(mock_task_handle, 0, 0x87654321,
                                                eIncrement, _))
      .WillOnce(DoAll(SetArgPointee<4>(0x11111111), Return(pdTRUE)));
  BaseType_t result =
      test_task.notify_and_query(0, 0x87654321, eIncrement, prev_value);
  EXPECT_EQ(result, pdTRUE);
  EXPECT_EQ(prev_value, 0x11111111);
}

TEST_F(IndexedNotificationTest, TaskNotifyISR) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, xTaskNotifyIndexedFromISR(mock_task_handle, 0, 0xDEADBEEF,
                                               eSetBits, _))
      .WillOnce(DoAll(SetArgPointee<4>(pdFALSE), Return(pdTRUE)));
  auto isr_res = test_task.notify_isr(0, 0xDEADBEEF, eSetBits);
  EXPECT_EQ(isr_res.result, pdTRUE);
  EXPECT_EQ(isr_res.higher_priority_task_woken, pdFALSE);

  EXPECT_CALL(*mock, xTaskNotifyIndexedFromISR(mock_task_handle, 1, 0xCAFEBABE,
                                               eNoAction, _))
      .WillOnce(DoAll(SetArgPointee<4>(pdTRUE), Return(pdTRUE)));
  isr_res = test_task.notify_isr(1, 0xCAFEBABE, eNoAction);
  EXPECT_EQ(isr_res.result, pdTRUE);
  EXPECT_EQ(isr_res.higher_priority_task_woken, pdTRUE);
}

TEST_F(IndexedNotificationTest, TaskNotifyAndQueryISR) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  uint32_t prev_val_isr;
  EXPECT_CALL(
      *mock, xTaskNotifyAndQueryIndexedFromISR(mock_task_handle, 0, 0x12AB34CD,
                                               eSetValueWithoutOverwrite, _, _))
      .WillOnce(DoAll(SetArgPointee<4>(0x56EF78AB), SetArgPointee<5>(pdTRUE),
                      Return(pdTRUE)));
  auto isr_q_res = test_task.notify_and_query_isr(
      0, 0x12AB34CD, eSetValueWithoutOverwrite, prev_val_isr);
  EXPECT_EQ(isr_q_res.result, pdTRUE);
  EXPECT_EQ(prev_val_isr, 0x56EF78AB);
  EXPECT_EQ(isr_q_res.higher_priority_task_woken, pdTRUE);
}

TEST_F(IndexedNotificationTest, TaskNotifyWaitIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  uint32_t notification_value;
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(0, 0, 0xFFFFFFFF, _, 1000))
      .WillOnce(DoAll(SetArgPointee<3>(0xABCDEF00), Return(pdTRUE)));
  BaseType_t result =
      test_task.notify_wait(0, 0, 0xFFFFFFFF, notification_value, 1000);
  EXPECT_EQ(result, pdTRUE);
  EXPECT_EQ(notification_value, 0xABCDEF00);
}

TEST_F(IndexedNotificationTest, TaskNotifyWaitIndexedChrono) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  uint32_t notification_value;
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0xFF, 0xFF00, _, 500))
      .WillOnce(DoAll(SetArgPointee<3>(0xDEAD), Return(pdTRUE)));
  BaseType_t result = test_task.notify_wait(1, 0xFF, 0xFF00, notification_value,
                                            std::chrono::milliseconds(500));
  EXPECT_EQ(result, pdTRUE);
  EXPECT_EQ(notification_value, 0xDEAD);
}

TEST_F(IndexedNotificationTest, TaskNotifyStateClearIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  EXPECT_CALL(*mock, xTaskNotifyStateClearIndexed(mock_task_handle, 0))
      .WillOnce(Return(pdTRUE));
  BaseType_t result = test_task.notify_state_clear(0);
  EXPECT_EQ(result, pdTRUE);

  EXPECT_CALL(*mock, xTaskNotifyStateClearIndexed(mock_task_handle, 1))
      .WillOnce(Return(pdFALSE));
  result = test_task.notify_state_clear(1);
  EXPECT_EQ(result, pdFALSE);
}

TEST_F(IndexedNotificationTest, TaskNotifyValueClearIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  EXPECT_CALL(*mock,
              ulTaskNotifyValueClearIndexed(mock_task_handle, 0, 0x0000FFFF))
      .WillOnce(Return(0xFFFF0000));
  uint32_t cleared = test_task.notify_value_clear(0, 0x0000FFFF);
  EXPECT_EQ(cleared, 0xFFFF0000);

  EXPECT_CALL(*mock, ulTaskNotifyValueClearIndexed(mock_task_handle, 1, 0xFF00))
      .WillOnce(Return(0x00FF));
  cleared = test_task.notify_value_clear(1, 0xFF00);
  EXPECT_EQ(cleared, 0x00FF);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyGiveIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, xTaskNotifyGiveIndexed(mock_task_handle, 0))
      .WillOnce(Return(pdTRUE));
  BaseType_t result = periodic_task.notify_give(0);
  EXPECT_EQ(result, pdTRUE);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyTakeIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(1, pdTRUE, 1000))
      .WillOnce(Return(5));
  uint32_t take_result = periodic_task.notify_take(1, pdTRUE, 1000);
  EXPECT_EQ(take_result, 5);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyTakeIndexedChrono) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(0, pdTRUE, 500))
      .WillOnce(Return(2));
  uint32_t take_result =
      periodic_task.notify_take(0, pdTRUE, std::chrono::milliseconds(500));
  EXPECT_EQ(take_result, 2);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, xTaskNotifyIndexed(mock_task_handle, 0, 0x12345, eSetBits))
      .WillOnce(Return(pdTRUE));
  BaseType_t result = periodic_task.notify(0, 0x12345, eSetBits);
  EXPECT_EQ(result, pdTRUE);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyAndQueryIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  uint32_t prev_value;
  EXPECT_CALL(*mock, xTaskNotifyAndQueryIndexed(mock_task_handle, 0, 0x12345,
                                                eSetBits, _))
      .WillOnce(DoAll(SetArgPointee<4>(0xABCD), Return(pdTRUE)));
  BaseType_t result =
      periodic_task.notify_and_query(0, 0x12345, eSetBits, prev_value);
  EXPECT_EQ(result, pdTRUE);
  EXPECT_EQ(prev_value, 0xABCD);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyISR) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, xTaskNotifyIndexedFromISR(mock_task_handle, 0, 0xDEAD,
                                               eSetBits, _))
      .WillOnce(DoAll(SetArgPointee<4>(pdTRUE), Return(pdTRUE)));
  auto isr_res = periodic_task.notify_isr(0, 0xDEAD, eSetBits);
  EXPECT_EQ(isr_res.result, pdTRUE);
  EXPECT_EQ(isr_res.higher_priority_task_woken, pdTRUE);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyAndQueryISR) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  uint32_t prev_value;
  EXPECT_CALL(*mock,
              xTaskNotifyAndQueryIndexedFromISR(mock_task_handle, 1, 0xBEEF,
                                                eSetValueWithOverwrite, _, _))
      .WillOnce(DoAll(SetArgPointee<4>(0x1234), SetArgPointee<5>(pdFALSE),
                      Return(pdTRUE)));
  auto isr_res = periodic_task.notify_and_query_isr(
      1, 0xBEEF, eSetValueWithOverwrite, prev_value);
  EXPECT_EQ(isr_res.result, pdTRUE);
  EXPECT_EQ(prev_value, 0x1234);
  EXPECT_EQ(isr_res.higher_priority_task_woken, pdFALSE);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyWaitIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  uint32_t notification_value;
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(0, 0, 0xFFFFFFFF, _, 500))
      .WillOnce(DoAll(SetArgPointee<3>(42), Return(pdTRUE)));
  BaseType_t result =
      periodic_task.notify_wait(0, 0, 0xFFFFFFFF, notification_value, 500);
  EXPECT_EQ(result, pdTRUE);
  EXPECT_EQ(notification_value, 42);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyWaitIndexedChrono) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  uint32_t notification_value;
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0, 0xFF, _, 1000))
      .WillOnce(DoAll(SetArgPointee<3>(99), Return(pdTRUE)));
  BaseType_t result = periodic_task.notify_wait(1, 0, 0xFF, notification_value,
                                                std::chrono::seconds(1));
  EXPECT_EQ(result, pdTRUE);
  EXPECT_EQ(notification_value, 99);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyStateClearIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, xTaskNotifyStateClearIndexed(mock_task_handle, 0))
      .WillOnce(Return(pdTRUE));
  BaseType_t result = periodic_task.notify_state_clear(0);
  EXPECT_EQ(result, pdTRUE);
}

TEST_F(IndexedNotificationTest, PeriodicTaskNotifyValueClearIndexed) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::periodic_task<1024> periodic_task("PeriodicIndexedTask", 2,
                                        empty_task_routine, empty_task_routine,
                                        empty_task_routine, 100ms);

  EXPECT_CALL(*mock, ulTaskNotifyValueClearIndexed(mock_task_handle, 0, 0xFF00))
      .WillOnce(Return(0x00FF));
  uint32_t cleared = periodic_task.notify_value_clear(0, 0xFF00);
  EXPECT_EQ(cleared, 0x00FF);
}

TEST_F(IndexedNotificationTest, TaskNotifyISRFailure) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  EXPECT_CALL(
      *mock, xTaskNotifyIndexedFromISR(mock_task_handle, 0, 0x1, eIncrement, _))
      .WillOnce(DoAll(SetArgPointee<4>(pdFALSE), Return(pdFALSE)));
  auto isr_res = test_task.notify_isr(0, 0x1, eIncrement);
  EXPECT_EQ(isr_res.result, pdFALSE);
  EXPECT_EQ(isr_res.higher_priority_task_woken, pdFALSE);
}

TEST_F(IndexedNotificationTest, TaskNotifyWaitIndexedFailure) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(mock_task_handle));
  EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));

  sa::task<1024> test_task("IndexedTask", 2, empty_task_routine);

  uint32_t notification_value;
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(0, 0, 0, _, portMAX_DELAY))
      .WillOnce(Return(pdFALSE));
  BaseType_t result =
      test_task.notify_wait(0, 0, 0, notification_value, portMAX_DELAY);
  EXPECT_EQ(result, pdFALSE);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}