#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_task.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_expected.hpp"
#include "freertos_isr_result.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

// =============================================================================
// Task notification ISR and expected API tests
// =============================================================================

class TaskNotifyIsrTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TaskHandle_t fake_task = reinterpret_cast<TaskHandle_t>(0x3000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(TaskNotifyIsrTest, NotifyIsrResult) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyFromISR(fake_task, 0x05, eSetValueWithoutOverwrite, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  auto result = t.notify_isr(0x05, eSetValueWithoutOverwrite);
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(TaskNotifyIsrTest, NotifyIsrFailure) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyFromISR(fake_task, 0x05, eNoAction, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  auto result = t.notify_isr(0x05, eNoAction);
  EXPECT_EQ(result.result, pdFAIL);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(TaskNotifyIsrTest, NotifyAndQueryIsrResult) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  uint32_t prev_value = 0;
  EXPECT_CALL(*mock, xTaskNotifyAndQueryFromISR(fake_task, 0x10, eSetBits, _, _))
      .WillOnce(DoAll(SetArgPointee<3>(0xAA), SetArgPointee<4>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  auto result = t.notify_and_query_isr(0x10, eSetBits, prev_value);
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
  EXPECT_EQ(prev_value, 0xAAu);
}

// Indexed notification ISR tests (configTASK_NOTIFICATION_ARRAY_ENTRIES > 1)

TEST_F(TaskNotifyIsrTest, IndexedNotifyIsr) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyIndexedFromISR(fake_task, 1, 0x01, eSetBits, _))
      .WillOnce(DoAll(SetArgPointee<4>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  auto result = t.notify_isr(1, 0x01, eSetBits);
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyAndQueryIsr) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  uint32_t prev_value = 0;
  EXPECT_CALL(*mock, xTaskNotifyAndQueryIndexedFromISR(fake_task, 1, 0x20, eSetValueWithOverwrite, _, _))
      .WillOnce(DoAll(SetArgPointee<4>(0xFF), SetArgPointee<5>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  auto result = t.notify_and_query_isr(1, 0x20, eSetValueWithOverwrite, prev_value);
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_EQ(prev_value, 0xFFu);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyWaitSuccess) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0, 0, _, 100))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto rc = t.notify_wait(1, 0, 0, val, TickType_t(100));
  EXPECT_EQ(rc, pdTRUE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyWaitWouldBlock) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0, 0, _, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto rc = t.notify_wait(1, 0, 0, val, TickType_t(0));
  EXPECT_EQ(rc, pdFALSE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyWaitTimeout) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0, 0, _, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto rc = t.notify_wait(1, 0, 0, val, TickType_t(100));
  EXPECT_EQ(rc, pdFALSE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyWaitChronoSuccess) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0, 0, _, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto rc = t.notify_wait(1, 0, 0, val, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdTRUE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyWaitChronoTimeout) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0, 0, _, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto rc = t.notify_wait(1, 0, 0, val, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdFALSE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyGive) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyGiveIndexed(fake_task, 1)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_EQ(t.notify_give(1), pdTRUE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyTake) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY)).WillOnce(Return(5));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_EQ(t.notify_take(1, pdTRUE, portMAX_DELAY), 5u);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyTakeChrono) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, ulTaskNotifyTakeIndexed(1, pdFALSE, _)).WillOnce(Return(3));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_EQ(t.notify_take(1, pdFALSE, std::chrono::milliseconds(100)), 3u);
}

TEST_F(TaskNotifyIsrTest, IndexedNotify) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyIndexed(fake_task, 1, 0x42, eSetValueWithoutOverwrite))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_EQ(t.notify(1, 0x42, eSetValueWithoutOverwrite), pdTRUE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyAndQuery) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyAndQueryIndexed(fake_task, 1, 0x55, eSetBits, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t pv = 0;
  EXPECT_EQ(t.notify_and_query(1, 0x55, eSetBits, pv), pdTRUE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyWait) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0, 0, _, portMAX_DELAY))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  EXPECT_EQ(t.notify_wait(1, 0, 0, val, portMAX_DELAY), pdTRUE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyWaitChrono) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWaitIndexed(1, 0, 0, _, _))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  EXPECT_EQ(t.notify_wait(1, 0, 0, val, std::chrono::milliseconds(100)), pdTRUE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyStateClear) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyStateClearIndexed(fake_task, 1)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_EQ(t.notify_state_clear(1), pdTRUE);
}

TEST_F(TaskNotifyIsrTest, IndexedNotifyValueClear) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, ulTaskNotifyValueClearIndexed(fake_task, 1, 0xFF)).WillOnce(Return(0x42u));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_EQ(t.notify_value_clear(1, 0xFF), 0x42u);
}

// notify_wait_ex failure paths

class TaskNotifyWaitExTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TaskHandle_t fake_task = reinterpret_cast<TaskHandle_t>(0x3000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(TaskNotifyWaitExTest, NotifyWaitExWouldBlock) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto exp = t.notify_wait_ex(0, 0, val, 0);
  EXPECT_FALSE(exp.has_value());
  EXPECT_EQ(exp.error(), freertos::error::would_block);
}

TEST_F(TaskNotifyWaitExTest, NotifyWaitExTimeout) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto exp = t.notify_wait_ex(0, 0, val, 100);
  EXPECT_FALSE(exp.has_value());
  EXPECT_EQ(exp.error(), freertos::error::timeout);
}

TEST_F(TaskNotifyWaitExTest, NotifyWaitExSuccess) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, portMAX_DELAY))
      .WillOnce(DoAll(SetArgPointee<2>(0xAB), Return(pdTRUE)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto exp = t.notify_wait_ex(0, 0, val, portMAX_DELAY);
  EXPECT_TRUE(exp.has_value());
  EXPECT_EQ(exp.value(), 0xABu);
}

TEST_F(TaskNotifyWaitExTest, NotifyWaitExChronoWouldBlock) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto exp = t.notify_wait_ex(0, 0, val, std::chrono::milliseconds(0));
  EXPECT_FALSE(exp.has_value());
  EXPECT_EQ(exp.error(), freertos::error::would_block);
}

TEST_F(TaskNotifyWaitExTest, NotifyWaitExChronoTimeout) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto exp = t.notify_wait_ex(0, 0, val, std::chrono::milliseconds(200));
  EXPECT_FALSE(exp.has_value());
  EXPECT_EQ(exp.error(), freertos::error::timeout);
}

TEST_F(TaskNotifyWaitExTest, NotifyWaitExChronoSuccess) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(0xCD), Return(pdTRUE)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  uint32_t val = 0;
  auto exp = t.notify_wait_ex(0, 0, val, std::chrono::milliseconds(200));
  EXPECT_TRUE(exp.has_value());
  EXPECT_EQ(exp.value(), 0xCDu);
}

// Task utility accessors and constructors

class TaskAccessorTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TaskHandle_t fake_task = reinterpret_cast<TaskHandle_t>(0x3000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(TaskAccessorTest, HandleAccessor) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_EQ(t.handle(), fake_task);
}

TEST_F(TaskAccessorTest, CurrentTaskHandle) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x5001);
  EXPECT_CALL(*mock, xTaskGetCurrentTaskHandle()).WillOnce(Return(h));

  EXPECT_EQ(freertos::current_task_handle(), h);
}

TEST_F(TaskAccessorTest, IdleTaskHandle) {
  TaskHandle_t h = reinterpret_cast<TaskHandle_t>(0x5002);
  EXPECT_CALL(*mock, xTaskGetIdleTaskHandle()).WillOnce(Return(h));

  EXPECT_EQ(freertos::idle_task_handle(), h);
}

TEST_F(TaskAccessorTest, TickCount) {
  EXPECT_CALL(*mock, xTaskGetTickCount()).WillOnce(Return(1234));

  EXPECT_EQ(freertos::tick_count(), 1234u);
}

TEST_F(TaskAccessorTest, TickCountIsr) {
  EXPECT_CALL(*mock, xTaskGetTickCountFromISR()).WillOnce(Return(5678));

  EXPECT_EQ(freertos::tick_count_isr(), 5678u);
}

// Task static allocator constructor (SFINAE constructor path)

TEST_F(TaskAccessorTest, StaticAllocatorConstructor) {
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::sa::task<512> t("t", 1, []() {}, true,
                             freertos::static_task_allocator<512>{});
  EXPECT_EQ(t.handle(), fake_task);
}

// Move constructor and swap for task

TEST_F(TaskAccessorTest, MoveConstructor) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t1("t", 1, []() {});
  freertos::da::task<512> t2(std::move(t1));
  EXPECT_EQ(t2.handle(), fake_task);
}

// delay_until functions (free functions in freertos namespace)

class TaskDelayTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(TaskDelayTest, DelayUntilTicks) {
  TickType_t prev = 100;
  EXPECT_CALL(*mock, vTaskDelayUntil(&prev, 50));

  freertos::delay_until(prev, 50);
}

TEST_F(TaskDelayTest, DelayUntilChrono) {
  TickType_t prev = 100;
  EXPECT_CALL(*mock, vTaskDelayUntil(&prev, 50));

  freertos::delay_until(prev, std::chrono::milliseconds(50));
}

// =============================================================================
// Timer ISR, accessor, move, and _ex tests
// =============================================================================

class TimerIsrAccessorTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TimerHandle_t fake_timer = reinterpret_cast<TimerHandle_t>(0x1000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

// Timer ISR variants

TEST_F(TimerIsrAccessorTest, StartIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_isr();
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(TimerIsrAccessorTest, StartIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_isr();
  EXPECT_EQ(result.result, pdFAIL);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(TimerIsrAccessorTest, StopIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStopFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start();
  auto result = t.stop_isr();
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(TimerIsrAccessorTest, StopIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.stop_isr();
  EXPECT_EQ(result.result, pdFAIL);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(TimerIsrAccessorTest, ResetIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerResetFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_isr();
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(TimerIsrAccessorTest, ResetIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_isr();
  EXPECT_EQ(result.result, pdFAIL);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(TimerIsrAccessorTest, PeriodIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, 200, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_isr(200);
  EXPECT_EQ(result.result, pdPASS);
  EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(TimerIsrAccessorTest, PeriodIsrChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_isr(std::chrono::milliseconds(200));
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(TimerIsrAccessorTest, PeriodIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_isr(200);
  EXPECT_EQ(result.result, pdFAIL);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

// Timer accessor methods

TEST_F(TimerIsrAccessorTest, IsActive) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerIsTimerActive(fake_timer)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.running(), pdTRUE);
}

TEST_F(TimerIsrAccessorTest, IsInactive) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerIsTimerActive(fake_timer)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.running(), pdFALSE);
}

TEST_F(TimerIsrAccessorTest, IsActiveNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.running(), pdFALSE);
}

TEST_F(TimerIsrAccessorTest, GetPeriod) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerGetPeriod(fake_timer)).WillOnce(Return(100));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period_ticks(), 100u);
}

TEST_F(TimerIsrAccessorTest, GetPeriodChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerGetPeriod(fake_timer)).WillOnce(Return(100));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period(), std::chrono::milliseconds(100));
}

TEST_F(TimerIsrAccessorTest, GetPeriodNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period_ticks(), 0u);
}

TEST_F(TimerIsrAccessorTest, GetPeriodChronoNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period(), std::chrono::milliseconds(0));
}

TEST_F(TimerIsrAccessorTest, SetReloadMode) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, vTimerSetReloadMode(fake_timer, pdFALSE));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto &ref = t.reload_mode(pdFALSE);
  EXPECT_EQ(&ref, &t);
}

TEST_F(TimerIsrAccessorTest, SetReloadModeNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto &ref = t.reload_mode(pdFALSE);
  EXPECT_EQ(&ref, &t);
}

TEST_F(TimerIsrAccessorTest, GetReloadMode) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(fake_timer)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.reload_mode(), pdTRUE);
}

TEST_F(TimerIsrAccessorTest, GetTimerId) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, pvTimerGetTimerID(fake_timer)).WillOnce(Return(reinterpret_cast<void *>(0x1234)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  void *id = pvTimerGetTimerID(fake_timer);
  EXPECT_EQ(id, reinterpret_cast<void *>(0x1234));
}

TEST_F(TimerIsrAccessorTest, RemainingTicks) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerGetExpiryTime(fake_timer)).WillOnce(Return(200));
  EXPECT_CALL(*mock, xTaskGetTickCount()).WillOnce(Return(100));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.remaining_ticks(), 100u);
}

TEST_F(TimerIsrAccessorTest, RemainingTicksNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.remaining_ticks(), 0u);
}

TEST_F(TimerIsrAccessorTest, RemainingTime) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerGetExpiryTime(fake_timer)).WillOnce(Return(200));
  EXPECT_CALL(*mock, xTaskGetTickCount()).WillOnce(Return(100));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.remaining_time(), std::chrono::milliseconds(100));
}

TEST_F(TimerIsrAccessorTest, TimerName) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, pcTimerGetName(fake_timer)).WillOnce(Return("test_timer"));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("test_timer", 100, pdTRUE, []() {});
  EXPECT_STREQ(t.name(), "test_timer");
}

TEST_F(TimerIsrAccessorTest, TimerNameNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.name(), nullptr);
}

// Timer _ex failure paths (would_block, timeout)

TEST_F(TimerIsrAccessorTest, StopExChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto r = t.stop_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(r.has_value());
}

TEST_F(TimerIsrAccessorTest, ResetExChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto r = t.reset_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(r.has_value());
}

// Timer move constructor with valid handle

TEST_F(TimerIsrAccessorTest, MoveConstructorValidHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, vTimerSetTimerID(fake_timer, _));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer src("t", 100, pdTRUE, []() {});
  freertos::da::timer dst(std::move(src));
}

// Timer swap

TEST_F(TimerIsrAccessorTest, SwapBothValid) {
  TimerHandle_t t1 = reinterpret_cast<TimerHandle_t>(0x1001);
  TimerHandle_t t2 = reinterpret_cast<TimerHandle_t>(0x1002);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t2));
  EXPECT_CALL(*mock, vTimerSetTimerID(t2, _));
  EXPECT_CALL(*mock, vTimerSetTimerID(t1, _));
  EXPECT_CALL(*mock, xTimerDelete(t1, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerDelete(t2, portMAX_DELAY));

  freertos::da::timer a("a", 100, pdTRUE, []() {});
  freertos::da::timer b("b", 100, pdFALSE, []() {});
  a.swap(b);
}

// Timer move assignment with valid source and started

TEST_F(TimerIsrAccessorTest, MoveAssignmentStartedTimer) {
  TimerHandle_t dt = reinterpret_cast<TimerHandle_t>(0x1001);
  TimerHandle_t st = reinterpret_cast<TimerHandle_t>(0x1002);
  TimerHandle_t nt = reinterpret_cast<TimerHandle_t>(0x1003);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(dt));
  EXPECT_CALL(*mock, xTimerStart(dt, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(st));
  EXPECT_CALL(*mock, xTimerStart(st, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(st, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(st)).WillOnce(Return("src"));
  EXPECT_CALL(*mock, xTimerGetPeriod(st)).WillOnce(Return(200));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(st)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nt));
  EXPECT_CALL(*mock, xTimerDelete(dt, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerDelete(st, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerStart(nt, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(nt, portMAX_DELAY));

  freertos::da::timer dst("dst", 100, pdTRUE, []() {});
  dst.start();
  freertos::da::timer src("src", 200, pdTRUE, []() {});
  src.start();
  dst = std::move(src);
}

// Timer move assignment with valid source, recreation fails

TEST_F(TimerIsrAccessorTest, MoveAssignmentRecreationFails) {
  TimerHandle_t dt = reinterpret_cast<TimerHandle_t>(0x1001);
  TimerHandle_t st = reinterpret_cast<TimerHandle_t>(0x1002);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(dt));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(st));
  EXPECT_CALL(*mock, xTimerStart(st, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(st, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(st)).WillOnce(Return("src"));
  EXPECT_CALL(*mock, xTimerGetPeriod(st)).WillOnce(Return(200));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(st)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, xTimerDelete(st, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerDelete(dt, portMAX_DELAY));

  freertos::da::timer dst("dst", 100, pdTRUE, []() {});
  freertos::da::timer src("src", 200, pdTRUE, []() {});
  src.start();
  dst = std::move(src);
}

// Timer constructor with chrono period

TEST_F(TimerIsrAccessorTest, ChronoConstructor) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", std::chrono::milliseconds(100), pdTRUE, []() {});
}

// Timer constructor with chrono period and allocator args (SFINAE path)

TEST_F(TimerIsrAccessorTest, ChronoConstructorWithAllocatorArg) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::static_sw_timer_allocator alloc;
  freertos::timer<freertos::static_sw_timer_allocator> t(
      "t", std::chrono::milliseconds(100), pdTRUE, []() {}, std::move(alloc));
}

// Timer constructor with allocator args (SFINAE path for static)

TEST_F(TimerIsrAccessorTest, ConstructorWithStaticAllocatorArg) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::static_sw_timer_allocator alloc;
  freertos::timer<freertos::static_sw_timer_allocator> t(
      "t", 100, pdTRUE, []() {}, std::move(alloc));
}

// Period ISR chrono overload

TEST_F(TimerIsrAccessorTest, PeriodTicksNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period_ticks(), 0u);
}

// Start_ex / stop_ex / reset_ex / period_ex success paths with timer started flag

TEST_F(TimerIsrAccessorTest, StartExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto r = t.start_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(TimerIsrAccessorTest, StopExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start();
  auto r = t.stop_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(TimerIsrAccessorTest, ResetExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto r = t.reset_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(TimerIsrAccessorTest, PeriodExSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, 200, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto r = t.period_ex(200);
  EXPECT_TRUE(r.has_value());
}

TEST_F(TimerIsrAccessorTest, StopExFromStoppedTimer) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto r = t.stop_ex();
  EXPECT_TRUE(r.has_value());
}

// Timer _ex_isr success paths

TEST_F(TimerIsrAccessorTest, StartExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto r = t.start_ex_isr();
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(TimerIsrAccessorTest, StopExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStopFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start();
  auto r = t.stop_ex_isr();
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(TimerIsrAccessorTest, ResetExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerResetFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto r = t.reset_ex_isr();
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(TimerIsrAccessorTest, PeriodExIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, 200, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto r = t.period_ex_isr(200);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(TimerIsrAccessorTest, PeriodExIsrChronoSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto r = t.period_ex_isr(std::chrono::milliseconds(200));
  EXPECT_TRUE(r.result.has_value());
}

// Timer start/stop/reset/period null handle paths (for start/stop/reset/period)

TEST_F(TimerIsrAccessorTest, StartNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.start(), pdFAIL);
}

TEST_F(TimerIsrAccessorTest, StopNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.stop(), pdFAIL);
}

TEST_F(TimerIsrAccessorTest, ResetNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.reset(), pdFAIL);
}

TEST_F(TimerIsrAccessorTest, PeriodNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period(200), pdFAIL);
}

// Timer period_ticks/period null handle for remaining ticks/time null handle coverage

TEST_F(TimerIsrAccessorTest, RemainingTimeNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.remaining_time(), std::chrono::milliseconds(0));
}

// Timer start/stop/reset with chrono

TEST_F(TimerIsrAccessorTest, StartChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.start(std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(TimerIsrAccessorTest, StopChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.stop(std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(TimerIsrAccessorTest, ResetChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.reset(std::chrono::milliseconds(100)), pdPASS);
}

TEST_F(TimerIsrAccessorTest, PeriodChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period(std::chrono::milliseconds(200), std::chrono::milliseconds(100)), pdPASS);
}

// Timer destructor with null m_timer (when xTimerCreate returns null already handled
// since no xTimerDelete is called)

TEST_F(TimerIsrAccessorTest, DestructorNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
}

// Timer move constructor - source null m_started path

TEST_F(TimerIsrAccessorTest, MoveConstructorSrcNotStarted) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, vTimerSetTimerID(fake_timer, _));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer src("t", 100, pdTRUE, []() {});
  freertos::da::timer dst(std::move(src));
}