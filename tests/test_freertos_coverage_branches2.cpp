#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <queue.h>
#include <event_groups.h>
#include <stream_buffer.h>
#include <message_buffer.h>

#include "freertos_task.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_queue.hpp"
#include "freertos_queue_set.hpp"
#include "freertos_event_group.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::NotNull;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::InSequence;

namespace {
void dummy_timer_callback(TimerHandle_t) {}
void dummy_task_function() {}
} // namespace

// ================================================================
// TASK BRANCH TESTS
// ================================================================

class TaskBranchTest : public ::testing::Test {
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

TEST_F(TaskBranchTest, IsRunningTrue) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eRunning));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_TRUE(t.is_running());
}

TEST_F(TaskBranchTest, IsRunningFalse) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eSuspended));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_FALSE(t.is_running());
}

TEST_F(TaskBranchTest, IsAliveTrue) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eReady));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_TRUE(t.is_alive());
}

TEST_F(TaskBranchTest, IsAliveFalseDeleted) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_FALSE(t.is_alive());
}

TEST_F(TaskBranchTest, IsSuspendedTrue) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eSuspended));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_TRUE(t.is_suspended());
}

TEST_F(TaskBranchTest, IsSuspendedFalse) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eRunning));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_FALSE(t.is_suspended());
}

TEST_F(TaskBranchTest, SwapTwoTasks) {
  TaskHandle_t h1 = reinterpret_cast<TaskHandle_t>(0x3001);
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0x3002);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(h1), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(h2), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(h1));
  EXPECT_CALL(*mock, vTaskDelete(h2));

  freertos::da::task<512> a("a", 1, []() {});
  freertos::da::task<512> b("b", 1, []() {});
  a.swap(b);
}

TEST_F(TaskBranchTest, MoveAssignIntoTaskWithExistingHandle) {
  TaskHandle_t h1 = reinterpret_cast<TaskHandle_t>(0x3001);
  TaskHandle_t h2 = reinterpret_cast<TaskHandle_t>(0x3002);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(h1), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(h2), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(h1));
  EXPECT_CALL(*mock, vTaskDelete(h2));

  freertos::da::task<512> a("a", 1, []() {});
  freertos::da::task<512> b("b", 1, []() {});
  a = std::move(b);
}

TEST_F(TaskBranchTest, PeriodicTaskIsRunningTrue) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eBlocked));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::periodic_task<512> pt(
      "t", 1, [] {}, [] {}, [] {}, std::chrono::milliseconds(100));
  EXPECT_TRUE(pt.is_running());
}

TEST_F(TaskBranchTest, PeriodicTaskIsAliveTrue) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eSuspended));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::periodic_task<512> pt(
      "t", 1, [] {}, [] {}, [] {}, std::chrono::milliseconds(100));
  EXPECT_TRUE(pt.is_alive());
}

// ================================================================
// TIMER BRANCH TESTS
// ================================================================

class TimerBranchTest : public ::testing::Test {
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

TEST_F(TimerBranchTest, MoveConstructNullHandleDestructorSkipsDelete) {
  TimerHandle_t t1 = reinterpret_cast<TimerHandle_t>(0x1001);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, xTimerDelete(t1, portMAX_DELAY));

  freertos::da::timer a("a", 100, pdTRUE, []() {});
  freertos::da::timer b("b", 100, pdTRUE, []() {});
}

TEST_F(TimerBranchTest, MoveAssignSelfMove) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  t = std::move(t);
#pragma GCC diagnostic pop
}

TEST_F(TimerBranchTest, MoveAssignBothNull) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(reinterpret_cast<TimerHandle_t>(0x1001)));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, xTimerDelete(reinterpret_cast<TimerHandle_t>(0x1001), portMAX_DELAY));

  freertos::da::timer a("a", 100, pdTRUE, []() {});
  freertos::da::timer b("b", 100, pdTRUE, []() {});
}

TEST_F(TimerBranchTest, MoveAssignStopFails) {
  TimerHandle_t t1 = reinterpret_cast<TimerHandle_t>(0x1001);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(t1, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, portMAX_DELAY)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer a("a", 100, pdTRUE, []() {});
  freertos::da::timer b("b", 100, pdTRUE, []() {});
  a = std::move(b);
}

TEST_F(TimerBranchTest, MoveAssignCreateFails) {
  TimerHandle_t t1 = reinterpret_cast<TimerHandle_t>(0x1001);
  const char *timer_name = "b";
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(t1, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(fake_timer)).WillOnce(Return(timer_name));
  EXPECT_CALL(*mock, xTimerGetPeriod(fake_timer)).WillOnce(Return(100));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(fake_timer)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  // a ends up with null handle, no delete on destructor

  freertos::da::timer a("a", 100, pdTRUE, []() {});
  freertos::da::timer b("b", 100, pdTRUE, []() {});
  a = std::move(b);
}

TEST_F(TimerBranchTest, MoveAssignNotStarted) {
  TimerHandle_t t1 = reinterpret_cast<TimerHandle_t>(0x1001);
  TimerHandle_t t2 = reinterpret_cast<TimerHandle_t>(0x1002);
  const char *timer_name = "b";
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t2));
  EXPECT_CALL(*mock, xTimerDelete(t1, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerStop(t2, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(t2)).WillOnce(Return(timer_name));
  EXPECT_CALL(*mock, xTimerGetPeriod(t2)).WillOnce(Return(100));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(t2)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerDelete(t2, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer a("a", 100, pdTRUE, []() {});
  freertos::da::timer b("b", 100, pdTRUE, []() {});
  a = std::move(b);
}

TEST_F(TimerBranchTest, MoveAssignStartFails) {
  TimerHandle_t t1 = reinterpret_cast<TimerHandle_t>(0x1001);
  TimerHandle_t t2 = reinterpret_cast<TimerHandle_t>(0x1002);
  TimerHandle_t t3 = reinterpret_cast<TimerHandle_t>(0x1003);
  const char *timer_name = "b";
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t2));
  EXPECT_CALL(*mock, xTimerStart(t2, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(t1, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerStop(t2, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(t2)).WillOnce(Return(timer_name));
  EXPECT_CALL(*mock, xTimerGetPeriod(t2)).WillOnce(Return(100));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(t2)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerDelete(t2, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(t3));
  EXPECT_CALL(*mock, xTimerStart(t3, portMAX_DELAY)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(t3, portMAX_DELAY));

  freertos::da::timer a("a", 100, pdTRUE, []() {});
  freertos::da::timer b("b", 100, pdTRUE, []() {});
  b.start();
  a = std::move(b);
}

TEST_F(TimerBranchTest, StartIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(TimerBranchTest, StopIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStopFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start();
  auto result = t.stop_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(TimerBranchTest, StartFailureMStartedStaysFalse) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.start();
  EXPECT_EQ(rc, pdFAIL);
}

TEST_F(TimerBranchTest, StopFailureMStartedStaysTrue) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start();
  auto rc = t.stop();
  EXPECT_EQ(rc, pdFAIL);
}

TEST_F(TimerBranchTest, RunningReturnsFalse) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerIsTimerActive(fake_timer)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.running(), pdFALSE);
}

TEST_F(TimerBranchTest, ReloadModeGetter) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(fake_timer)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.reload_mode(), pdTRUE);
}

TEST_F(TimerBranchTest, RemainingTicks) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerGetExpiryTime(fake_timer)).WillOnce(Return(500));
  EXPECT_CALL(*mock, xTaskGetTickCount()).WillOnce(Return(400));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.remaining_ticks(), 100);
}

TEST_F(TimerBranchTest, NameOnValidHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, pcTimerGetName(fake_timer)).WillOnce(Return("test"));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_STREQ(t.name(), "test");
}

TEST_F(TimerBranchTest, PeriodTicks) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerGetPeriod(fake_timer)).WillOnce(Return(200));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period_ticks(), 200);
}

TEST_F(TimerBranchTest, StartExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(TimerBranchTest, StopExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start();
  auto result = t.stop_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(TimerBranchTest, ResetExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(TimerBranchTest, PeriodExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, 200, _))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_ex(200);
  EXPECT_FALSE(result.has_value());
}

TEST_F(TimerBranchTest, StartExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(TimerBranchTest, StopExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStopFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start();
  auto result = t.stop_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(TimerBranchTest, ResetExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerResetFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(TimerBranchTest, PeriodExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_ex_isr(200);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(TimerBranchTest, ReloadModeSetterValid) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, vTimerSetReloadMode(fake_timer, pdFALSE));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.reload_mode(pdFALSE);
}

// ================================================================
// SEMAPHORE BRANCH TESTS
// ================================================================

class SemaphoreBranchTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x4000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

// --- mutex ---

TEST_F(SemaphoreBranchTest, MutexUnlockFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  mtx.lock();
  auto rc = mtx.unlock();
  EXPECT_EQ(rc, pdFALSE);
  EXPECT_TRUE(mtx.locked());
}

TEST_F(SemaphoreBranchTest, MutexLockFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  auto rc = mtx.lock();
  EXPECT_EQ(rc, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}

TEST_F(SemaphoreBranchTest, MutexTryLockFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  auto rc = mtx.try_lock();
  EXPECT_EQ(rc, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}

TEST_F(SemaphoreBranchTest, MutexLockIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  auto result = mtx.lock_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_TRUE(mtx.locked());
}

TEST_F(SemaphoreBranchTest, MutexLockIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  auto result = mtx.lock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_FALSE(mtx.locked());
}

TEST_F(SemaphoreBranchTest, MutexUnlockIsrSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdTRUE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  mtx.lock();
  auto result = mtx.unlock_isr();
  EXPECT_EQ(result.result, pdTRUE);
  EXPECT_FALSE(mtx.locked());
}

TEST_F(SemaphoreBranchTest, MutexUnlockIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  mtx.lock();
  auto result = mtx.unlock_isr();
  EXPECT_EQ(result.result, pdFALSE);
  EXPECT_TRUE(mtx.locked());
}

TEST_F(SemaphoreBranchTest, MutexSelfMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  mtx = std::move(mtx);
#pragma GCC diagnostic pop
}

TEST_F(SemaphoreBranchTest, MutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  auto result = mtx.lock_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SemaphoreBranchTest, MutexTryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  auto result = mtx.try_lock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SemaphoreBranchTest, MutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  auto result = mtx.unlock_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(SemaphoreBranchTest, MutexLockExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  auto result = mtx.lock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SemaphoreBranchTest, MutexUnlockExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  mtx.lock();
  auto result = mtx.unlock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

// --- recursive_mutex ---

TEST_F(SemaphoreBranchTest, RecursiveMutexLockFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  auto rc = rm.lock();
  EXPECT_EQ(rc, pdFALSE);
}

TEST_F(SemaphoreBranchTest, RecursiveMutexTryLockFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  auto rc = rm.try_lock();
  EXPECT_EQ(rc, pdFALSE);
}

TEST_F(SemaphoreBranchTest, RecursiveMutexSelfMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  rm = std::move(rm);
#pragma GCC diagnostic pop
}

TEST_F(SemaphoreBranchTest, RecursiveMutexLockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  auto result = rm.lock_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SemaphoreBranchTest, RecursiveMutexTryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  auto result = rm.try_lock_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SemaphoreBranchTest, RecursiveMutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  rm.lock();
  auto result = rm.unlock_ex();
  EXPECT_FALSE(result.has_value());
}

// --- binary_semaphore ---

TEST_F(SemaphoreBranchTest, BinarySemaphoreSelfMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::binary_semaphore bs;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  bs = std::move(bs);
#pragma GCC diagnostic pop
}

TEST_F(SemaphoreBranchTest, CountingSemaphoreSelfMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  cs = std::move(cs);
#pragma GCC diagnostic pop
}

TEST_F(SemaphoreBranchTest, BinarySemaphoreGiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::binary_semaphore bs;
  auto result = bs.give_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(SemaphoreBranchTest, BinarySemaphoreTakeExWouldBlockDirect) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::binary_semaphore bs;
  auto result = bs.take_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SemaphoreBranchTest, BinarySemaphoreGiveExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::binary_semaphore bs;
  auto result = bs.give_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SemaphoreBranchTest, BinarySemaphoreTakeExIsrFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::binary_semaphore bs;
  auto result = bs.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SemaphoreBranchTest, CountingSemaphoreTakeExWouldBlockDirect) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
  auto result = cs.take_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

// --- try_lock_guard ---

TEST_F(SemaphoreBranchTest, TryLockGuardFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  { freertos::try_lock_guard<freertos::da::mutex> guard(mtx); }
}

// --- timeout_lock_guard failure ---

TEST_F(SemaphoreBranchTest, TimeoutLockGuardFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
  { freertos::timeout_lock_guard<freertos::da::mutex> guard(mtx, 0); }
}

// ================================================================
// EVENT GROUP BRANCH TESTS
// ================================================================

class EventGroupBranchTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t fake_eg = reinterpret_cast<EventGroupHandle_t>(0x6000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(EventGroupBranchTest, WaitBitsExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x03, pdTRUE, pdTRUE, 100))
      .WillOnce(Return(0x03));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x03, pdTRUE, pdTRUE, 100);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), EventBits_t{0x03});
}

TEST_F(EventGroupBranchTest, SyncExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 100))
      .WillOnce(Return(0x03));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), EventBits_t{0x03});
}

TEST_F(EventGroupBranchTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  eg = std::move(eg);
#pragma GCC diagnostic pop
}

TEST_F(EventGroupBranchTest, MoveConstructNullHandleDestructor) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(reinterpret_cast<EventGroupHandle_t>(0x6001)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  EXPECT_CALL(*mock, vEventGroupDelete(reinterpret_cast<EventGroupHandle_t>(0x6001)));

  freertos::da::event_group a, b;
  auto c = std::move(a);
}

TEST_F(EventGroupBranchTest, WaitBitsExWouldBlockDirect) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, 0))
      .WillOnce(Return(0x00));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(EventGroupBranchTest, SyncExWouldBlockDirect) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 0))
      .WillOnce(Return(0x00));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

// ================================================================
// QUEUE BRANCH TESTS
// ================================================================

class QueueBranchTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_queue = reinterpret_cast<QueueHandle_t>(0x5000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueBranchTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  q = std::move(q);
#pragma GCC diagnostic pop
}

TEST_F(QueueBranchTest, PeekSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueuePeek(fake_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.peek(100);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueBranchTest, ReceiveOptionalSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReceive(fake_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive(100);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueBranchTest, ReceiveOptionalFailure) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReceive(fake_queue, _, _)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive(100);
  EXPECT_FALSE(result.has_value());
}

TEST_F(QueueBranchTest, ReceiveIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(fake_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto result = q.receive_isr(val);
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(QueueBranchTest, SendExQueueFullDirect) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSend(fake_queue, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(QueueBranchTest, SendExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSendFromISR(fake_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::queue_full);
}

TEST_F(QueueBranchTest, ReceiveExQueueEmptyDirect) {
  int val;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReceive(fake_queue, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(val, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(QueueBranchTest, ReceiveExExpectedSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReceive(fake_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(100);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueBranchTest, ReceiveExExpectedQueueEmptyDirect) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReceive(fake_queue, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(QueueBranchTest, ResetExFailure) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReset(fake_queue)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.reset_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(QueueBranchTest, PeekExFailure) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueuePeek(fake_queue, _, 0)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto result = q.peek_ex(val, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(QueueBranchTest, SendBackExQueueFullDirect) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSendToBack(fake_queue, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(QueueBranchTest, SendFrontExQueueFullDirect) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSendToFront(fake_queue, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(QueueBranchTest, SendBackExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(fake_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(QueueBranchTest, SendFrontExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(fake_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
}

// ================================================================
// STREAM BUFFER BRANCH TESTS
// ================================================================

class StreamBufferBranchTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t fake_sb = reinterpret_cast<StreamBufferHandle_t>(0x7000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(StreamBufferBranchTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  sb = std::move(sb);
#pragma GCC diagnostic pop
}

TEST_F(StreamBufferBranchTest, SendExWouldBlockDirect) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(fake_sb, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
  uint8_t data[] = {1, 2, 3, 4};
  auto result = sb.send_ex(data, 4, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(StreamBufferBranchTest, ReceiveExWouldBlockDirect) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(fake_sb, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
  uint8_t buf[4];
  auto result = sb.receive_ex(buf, 4, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(StreamBufferBranchTest, ResetExFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReset(fake_sb)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
  auto result = sb.reset_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(StreamBufferBranchTest, SetTriggerLevelExFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(fake_sb, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
  auto result = sb.set_trigger_level_ex(4);
  EXPECT_FALSE(result.has_value());
}

TEST_F(StreamBufferBranchTest, SendExIsrFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(fake_sb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(0)));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
  uint8_t data[] = {1, 2, 3, 4};
  auto result = sb.send_ex_isr(data, 4);
  EXPECT_FALSE(result.result.has_value());
}

// ================================================================
// MESSAGE BUFFER BRANCH TESTS
// ================================================================

class MessageBufferBranchTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t fake_mb = reinterpret_cast<MessageBufferHandle_t>(0x8000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(MessageBufferBranchTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  mb = std::move(mb);
#pragma GCC diagnostic pop
}

TEST_F(MessageBufferBranchTest, SendExWouldBlockDirectTicks) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 3, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t data[] = {1, 2, 3};
  auto result = mb.send_ex(data, 3, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(MessageBufferBranchTest, ReceiveExWouldBlockDirectTicks) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 64, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t buf[64];
  auto result = mb.receive_ex(buf, 64, TickType_t(0));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(MessageBufferBranchTest, ResetExFailureDirect) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReset(fake_mb)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  auto result = mb.reset_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(MessageBufferBranchTest, SendExIsrFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(fake_mb, _, 3, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(0)));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t data[] = {1, 2, 3};
  auto result = mb.send_ex_isr(data, 3);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(MessageBufferBranchTest, ReceiveExIsrFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(fake_mb, _, 64, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(0)));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t buf[64];
  auto result = mb.receive_ex_isr(buf, 64);
  EXPECT_FALSE(result.result.has_value());
}

// ================================================================
// QUEUE SET BRANCH TESTS
// ================================================================

class QueueSetBranchTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueSetHandle_t fake_qs = reinterpret_cast<QueueSetHandle_t>(0x9000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueSetBranchTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xQueueCreateSet(_)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));

  freertos::da::queue_set qs(10);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  qs = std::move(qs);
#pragma GCC diagnostic pop
}

TEST_F(QueueSetBranchTest, AddExFailure) {
  EXPECT_CALL(*mock, xQueueCreateSet(_)).WillOnce(Return(fake_qs));
  QueueHandle_t q = reinterpret_cast<QueueHandle_t>(0x5001);
  EXPECT_CALL(*mock, xQueueAddToSet(q, fake_qs)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));

  freertos::da::queue_set qs(10);
  auto result = qs.add_ex(q);
  EXPECT_FALSE(result.has_value());
}

TEST_F(QueueSetBranchTest, RemoveExFailure) {
  EXPECT_CALL(*mock, xQueueCreateSet(_)).WillOnce(Return(fake_qs));
  QueueHandle_t q = reinterpret_cast<QueueHandle_t>(0x5001);
  EXPECT_CALL(*mock, xQueueRemoveFromSet(q, fake_qs)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));

  freertos::da::queue_set qs(10);
  auto result = qs.remove_ex(q);
  EXPECT_FALSE(result.has_value());
}

TEST_F(QueueSetBranchTest, SelectExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateSet(_)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSet(fake_qs, 0)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));

  freertos::da::queue_set qs(10);
  auto result = qs.select_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(QueueSetBranchTest, SelectExTimeout) {
  EXPECT_CALL(*mock, xQueueCreateSet(_)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSet(fake_qs, 100)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));

  freertos::da::queue_set qs(10);
  auto result = qs.select_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueSetBranchTest, SelectExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreateSet(_)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(fake_qs, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(nullptr)));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));

  freertos::da::queue_set qs(10);
  auto result = qs.select_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}