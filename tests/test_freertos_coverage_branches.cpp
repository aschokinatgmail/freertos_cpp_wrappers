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
#include "freertos_event_group.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_external_allocator.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::NotNull;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::InSequence;

namespace {
void dummy_timer_callback(TimerHandle_t) {}
} // namespace

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

TEST_F(TimerBranchTest, StartFailureStartedFlagStaysFalse) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.start(100);
  EXPECT_EQ(rc, pdFAIL);
}

TEST_F(TimerBranchTest, StopFailureStartedFlagStaysTrue) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start(0);
  auto rc = t.stop(0);
  EXPECT_EQ(rc, pdFAIL);
}

TEST_F(TimerBranchTest, StartIsrSuccessStartedFlag) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_isr();
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(TimerBranchTest, StopIsrSuccessStartedFlag) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStopFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start(0);
  auto result = t.stop_isr();
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(TimerBranchTest, PeriodIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_isr(200);
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(TimerBranchTest, ReloadModeSetterNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto &ref = t.reload_mode(pdFALSE);
  EXPECT_EQ(&ref, &t);
}

TEST_F(TimerBranchTest, PeriodIsrChronoOverload) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(fake_timer, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_isr(std::chrono::milliseconds(200));
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(TimerBranchTest, StartChronoOverload) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.start(std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(TimerBranchTest, StopChronoOverload) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.stop(std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(TimerBranchTest, ResetChronoOverload) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.reset(std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(TimerBranchTest, PeriodChronoOverload) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, _, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.period(std::chrono::milliseconds(200),
                     std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

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

TEST_F(TaskBranchTest, TaskIsRunningDefaultBranch) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_FALSE(t.is_running());
}

TEST_F(TaskBranchTest, TaskIsAliveDeletedState) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_FALSE(t.is_alive());
}

TEST_F(TaskBranchTest, TaskIsAliveInvalidState) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eInvalid));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_FALSE(t.is_alive());
}

TEST_F(TaskBranchTest, TaskTerminate) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  t.terminate();
}

TEST_F(TaskBranchTest, TaskAbortDelayNullHandle) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  t.terminate();
  auto rc = t.abort_delay();
  EXPECT_EQ(rc, pdFALSE);
}

TEST_F(TaskBranchTest, TaskIsSuspended) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eSuspended));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::task<512> t("t", 1, []() {});
  EXPECT_TRUE(t.is_suspended());
}

TEST_F(TaskBranchTest, PeriodicTaskIsRunningDefaultBranch) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::periodic_task<512> pt(
      "t", 1, [] {}, [] {}, [] {}, std::chrono::milliseconds(100));
  EXPECT_FALSE(pt.is_running());
}

TEST_F(TaskBranchTest, PeriodicTaskIsAliveDeletedState) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eDeleted));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::periodic_task<512> pt(
      "t", 1, [] {}, [] {}, [] {}, std::chrono::milliseconds(100));
  EXPECT_FALSE(pt.is_alive());
}

TEST_F(TaskBranchTest, PeriodicTaskIsSuspended) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eSuspended));
  EXPECT_CALL(*mock, xTaskAbortDelay(fake_task)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::periodic_task<512> pt(
      "t", 1, [] {}, [] {}, [] {}, std::chrono::milliseconds(100));
  EXPECT_TRUE(pt.is_suspended());
}

TEST_F(TaskBranchTest, PeriodicTaskTerminate) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(fake_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));

  freertos::da::periodic_task<512> pt(
      "t", 1, [] {}, [] {}, [] {}, std::chrono::milliseconds(100));
  pt.terminate();
}

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

TEST_F(SemaphoreBranchTest, MutexTryLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  auto result = m.try_lock_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SemaphoreBranchTest, MutexTryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  auto result = m.try_lock_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SemaphoreBranchTest, MutexLockExIsrReturnsWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  auto result = m.lock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(SemaphoreBranchTest, MutexUnlockExIsrReturnsNotOwned) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  auto result = m.unlock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::semaphore_not_owned);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(SemaphoreBranchTest, RecursiveMutexUnlockCountZero) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem))
      .Times(2)
      .WillRepeatedly(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  rm.lock(0);
  rm.unlock();
  auto rc = rm.unlock();
  EXPECT_EQ(rc, pdTRUE);
}

TEST_F(SemaphoreBranchTest, RecursiveMutexTryLockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  auto result = rm.try_lock_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SemaphoreBranchTest, RecursiveMutexTryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  auto result = rm.try_lock_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SemaphoreBranchTest, RecursiveMutexUnlockExSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  rm.lock(0);
  auto result = rm.unlock_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(SemaphoreBranchTest, RecursiveMutexUnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  rm.lock(0);
  auto result = rm.unlock_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SemaphoreBranchTest, MutexSwap) {
  SemaphoreHandle_t sem1 = reinterpret_cast<SemaphoreHandle_t>(0x4001);
  SemaphoreHandle_t sem2 = reinterpret_cast<SemaphoreHandle_t>(0x4002);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(sem2));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem2));

  freertos::da::mutex m1, m2;
  m1.swap(m2);
  EXPECT_FALSE(m1.locked());
  EXPECT_FALSE(m2.locked());
}

TEST_F(SemaphoreBranchTest, TryLockGuardSuccess) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  {
    freertos::try_lock_guard<freertos::da::mutex> guard(m);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(SemaphoreBranchTest, TryLockGuardFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  {
    freertos::try_lock_guard<freertos::da::mutex> guard(m);
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(SemaphoreBranchTest, TryLockGuardRecursiveMutex) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
      .WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
  {
    freertos::try_lock_guard<freertos::da::recursive_mutex> guard(rm);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(SemaphoreBranchTest, TimeoutLockGuardChronoConstructor) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex m;
  {
    freertos::timeout_lock_guard<freertos::da::mutex> guard(
        m, std::chrono::milliseconds(100));
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(SemaphoreBranchTest, BinarySemaphoreTakeChrono) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::binary_semaphore bs;
  auto rc = bs.take(std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdTRUE);
}

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

TEST_F(QueueBranchTest, QueueReceiveFailure) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReceive(fake_queue, _, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive(100);
  EXPECT_FALSE(result.has_value());
}

TEST_F(QueueBranchTest, QueuePeekFailure) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueuePeek(fake_queue, _, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.peek(100);
  EXPECT_FALSE(result.has_value());
}

TEST_F(QueueBranchTest, QueueReceiveIsrFailurePath) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(fake_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(QueueBranchTest, QueuePeekIsrFailurePath) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueuePeekFromISR(fake_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.peek_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(QueueBranchTest, QueueSendChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueSend(fake_queue, _, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto rc = q.send(42, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(QueueBranchTest, QueueReceiveChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueueReceive(fake_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto rc = q.receive(val, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(QueueBranchTest, QueuePeekChrono) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_queue));
  EXPECT_CALL(*mock, xQueuePeek(fake_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(fake_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_queue));

  freertos::da::queue<10, int> q;
  auto result = q.peek(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

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

TEST_F(EventGroupBranchTest, WaitBitsExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, 100))
      .WillOnce(Return(0x00));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(EventGroupBranchTest, SyncExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 100))
      .WillOnce(Return(0x00));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

class AllocatorBranchTest : public ::testing::Test {
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

static void *failing_allocate(size_t) { return nullptr; }
static void no_op_deallocate(void *) {}
static freertos::external_memory_region failing_region{failing_allocate,
                                                       no_op_deallocate};

TEST_F(AllocatorBranchTest, ExternalSemaphoreBinaryCreateFailure) {
  freertos::external_semaphore_allocator<freertos::external_memory_region> alloc(
      failing_region);
  auto handle = alloc.create_binary();
  EXPECT_EQ(handle, nullptr);
}

TEST_F(AllocatorBranchTest, ExternalSemaphoreCountingCreateFailure) {
  freertos::external_semaphore_allocator<freertos::external_memory_region> alloc(
      failing_region);
  auto handle = alloc.create_counting(5);
  EXPECT_EQ(handle, nullptr);
}

TEST_F(AllocatorBranchTest, ExternalSemaphoreMutexCreateFailure) {
  freertos::external_semaphore_allocator<freertos::external_memory_region> alloc(
      failing_region);
  auto handle = alloc.create_mutex();
  EXPECT_EQ(handle, nullptr);
}

TEST_F(AllocatorBranchTest, ExternalSemaphoreRecursiveMutexCreateFailure) {
  freertos::external_semaphore_allocator<freertos::external_memory_region> alloc(
      failing_region);
  auto handle = alloc.create_recursive_mutex();
  EXPECT_EQ(handle, nullptr);
}

TEST_F(AllocatorBranchTest, ExternalEventGroupCreateFailure) {
  freertos::external_event_group_allocator<freertos::external_memory_region>
      alloc(failing_region);
  auto handle = alloc.create();
  EXPECT_EQ(handle, nullptr);
}

TEST_F(AllocatorBranchTest, ExternalSwTimerCreateFailure) {
  freertos::external_sw_timer_allocator<freertos::external_memory_region> alloc(
      failing_region);
  auto handle = alloc.create("t", 100, pdTRUE, nullptr, dummy_timer_callback);
  EXPECT_EQ(handle, nullptr);
}

static int alloc_count = 0;
static void *partial_fail_allocate(size_t sz) {
  alloc_count++;
  if (alloc_count == 2) {
    return nullptr;
  }
  static char buf1[sizeof(StaticQueue_t)];
  static char buf2[256];
  if (alloc_count == 1)
    return buf1;
  return buf2;
}
static void partial_fail_deallocate(void *) {}
static freertos::external_memory_region partial_fail_region{
    partial_fail_allocate, partial_fail_deallocate};

TEST_F(AllocatorBranchTest, ExternalQueueCreateStructSuccessStorageFailure) {
  alloc_count = 0;
  freertos::external_queue_allocator<freertos::external_memory_region, 1, int>
      alloc(partial_fail_region);
  auto handle = alloc.create();
  EXPECT_EQ(handle, nullptr);
}

TEST_F(AllocatorBranchTest, ExternalStreamBufferCreateStructSuccessStorageFailure) {
  alloc_count = 0;
  freertos::external_stream_buffer_allocator<freertos::external_memory_region,
                                             64>
      alloc(partial_fail_region);
  auto handle = alloc.create(1);
  EXPECT_EQ(handle, nullptr);
}

TEST_F(AllocatorBranchTest, ExternalMessageBufferCreateStructSuccessStorageFailure) {
  alloc_count = 0;
  freertos::external_message_buffer_allocator<freertos::external_memory_region,
                                              64>
      alloc(partial_fail_region);
  auto handle = alloc.create();
  EXPECT_EQ(handle, nullptr);
}

static void *queue_first_fail_allocate(size_t) {
  alloc_count++;
  if (alloc_count == 1) {
    return nullptr;
  }
  static char buf[256];
  return buf;
}
static freertos::external_memory_region queue_first_fail_region{
    queue_first_fail_allocate, no_op_deallocate};

TEST_F(AllocatorBranchTest, ExternalQueueCreateStructFailure) {
  alloc_count = 0;
  freertos::external_queue_allocator<freertos::external_memory_region, 1, int>
      alloc(queue_first_fail_region);
  auto handle = alloc.create();
  EXPECT_EQ(handle, nullptr);
}

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

TEST_F(StreamBufferBranchTest, SendExIsrIteratorFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(fake_sb, _, _, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(0)));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
  uint8_t data[] = {1, 2, 3};
  auto result = sb.send_ex_isr(data, data + 3);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(StreamBufferBranchTest, SendChronoOverload) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(fake_sb, _, 3, _))
      .WillOnce(Return(3));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
  uint8_t data[] = {1, 2, 3};
  auto rc = sb.send(data, 3, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, 3);
}

TEST_F(StreamBufferBranchTest, ReceiveChronoOverload) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(fake_sb, _, 3, _))
      .WillOnce(Return(3));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
  uint8_t buf[3];
  auto rc = sb.receive(buf, 3, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, 3);
}

class MessageBufferBranchTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t fake_mb =
      reinterpret_cast<MessageBufferHandle_t>(0x8000);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(MessageBufferBranchTest, SendExIsrSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(fake_mb, _, 3, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(3)));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t data[] = {1, 2, 3};
  auto result = mb.send_ex_isr(data, 3);
  EXPECT_TRUE(result.result.has_value());
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

TEST_F(MessageBufferBranchTest, ReceiveExIsrSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(fake_mb, _, 64, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(3)));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t buf[64];
  auto result = mb.receive_ex_isr(buf, 64);
  EXPECT_TRUE(result.result.has_value());
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

TEST_F(MessageBufferBranchTest, SendChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 3, _)).WillOnce(Return(3));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t data[] = {1, 2, 3};
  auto rc = mb.send(data, 3, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, 3);
}

TEST_F(MessageBufferBranchTest, ReceiveChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 64, _))
      .WillOnce(Return(3));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
  uint8_t buf[64];
  auto rc = mb.receive(buf, 64, std::chrono::milliseconds(100));
  EXPECT_EQ(rc, 3);
}