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

#include "freertos_sw_timer.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_queue.hpp"
#include "freertos_event_group.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::InSequence;

namespace {
void dummy_timer_callback(TimerHandle_t) {}
} // namespace

// ================================================================
// QUEUE BRANCH TESTS — sa:: and da:: with named/unnamed + moved-from
// ================================================================

class QueueBranch3Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_q = reinterpret_cast<QueueHandle_t>(0x5001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueBranch3Test, DaQueueNamedDestructor) {
  const char *name = "named_q";
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<10, int> q(name);
}

TEST_F(QueueBranch3Test, DaQueueUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<10, int> q(nullptr);
}

TEST_F(QueueBranch3Test, DaQueueMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0x5002);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::da::queue<10, int> a(nullptr);
  auto b = std::move(a);
}

TEST_F(QueueBranch3Test, DaQueueNamedMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0x5003);
  const char *name = "mq";
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, vQueueAddToRegistry(q1, name));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(q1));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::da::queue<10, int> a(name);
  auto b = std::move(a);
}

TEST_F(QueueBranch3Test, SaQueueNamedDestructor) {
  const char *name = "sa_named";
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<10, int> q(name);
}

TEST_F(QueueBranch3Test, SaQueueUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<10, int> q(nullptr);
}

TEST_F(QueueBranch3Test, SaQueueMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0x5010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::sa::queue<10, int> a(nullptr);
  auto b = std::move(a);
}

TEST_F(QueueBranch3Test, SaQueueNamedMovedFromDestructor) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0x5011);
  const char *name = "samq";
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, vQueueAddToRegistry(q1, name));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(q1));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::sa::queue<10, int> a(name);
  auto b = std::move(a);
}

TEST_F(QueueBranch3Test, DaQueueDifferentTypeNamedDestructor) {
  const char *name = "charq";
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<10, char> q(name);
}

TEST_F(QueueBranch3Test, DaQueueDifferentTypeUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::da::queue<10, char> q(nullptr);
}

TEST_F(QueueBranch3Test, DaQueueDifferentTypeMovedFrom) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0x5020);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::da::queue<10, char> a(nullptr);
  auto b = std::move(a);
}

TEST_F(QueueBranch3Test, SaQueueDifferentTypeNamedDestructor) {
  const char *name = "sacharq";
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, name));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(name));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<10, char> q(name);
}

TEST_F(QueueBranch3Test, SaQueueDifferentTypeUnnamedDestructor) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));

  freertos::sa::queue<10, char> q(nullptr);
}

TEST_F(QueueBranch3Test, SaQueueDifferentTypeMovedFrom) {
  QueueHandle_t q1 = reinterpret_cast<QueueHandle_t>(0x5021);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(q1));
  EXPECT_CALL(*mock, pcQueueGetName(q1)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(q1));

  freertos::sa::queue<10, char> a(nullptr);
  auto b = std::move(a);
}

// ================================================================
// SEMAPHORE BRANCH TESTS — sa:: and da:: with destructor + moved-from
// ================================================================

class SemaphoreBranch3Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x4001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SemaphoreBranch3Test, DaBinarySemaphoreNormalDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::binary_semaphore bs;
}

TEST_F(SemaphoreBranch3Test, DaBinarySemaphoreMovedFromDestruction) {
  SemaphoreHandle_t sem1 = reinterpret_cast<SemaphoreHandle_t>(0x4010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateBinary()).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));

  freertos::da::binary_semaphore a;
  auto b = std::move(a);
}

TEST_F(SemaphoreBranch3Test, SaBinarySemaphoreNormalDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::sa::binary_semaphore bs;
}

TEST_F(SemaphoreBranch3Test, SaBinarySemaphoreMovedFromDestruction) {
  SemaphoreHandle_t sem1 = reinterpret_cast<SemaphoreHandle_t>(0x4011);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));

  freertos::sa::binary_semaphore a;
  auto b = std::move(a);
}

TEST_F(SemaphoreBranch3Test, DaCountingSemaphoreNormalDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::counting_semaphore cs(5);
}

TEST_F(SemaphoreBranch3Test, DaCountingSemaphoreMovedFromDestruction) {
  SemaphoreHandle_t sem1 = reinterpret_cast<SemaphoreHandle_t>(0x4020);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateCounting(_, _)).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));

  freertos::da::counting_semaphore a(5);
  auto b = std::move(a);
}

TEST_F(SemaphoreBranch3Test, SaCountingSemaphoreNormalDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::sa::counting_semaphore cs(5);
}

TEST_F(SemaphoreBranch3Test, SaCountingSemaphoreMovedFromDestruction) {
  SemaphoreHandle_t sem1 = reinterpret_cast<SemaphoreHandle_t>(0x4021);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _)).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));

  freertos::sa::counting_semaphore a(5);
  auto b = std::move(a);
}

TEST_F(SemaphoreBranch3Test, DaMutexNormalDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::mutex mtx;
}

TEST_F(SemaphoreBranch3Test, DaMutexMovedFromDestruction) {
  SemaphoreHandle_t sem1 = reinterpret_cast<SemaphoreHandle_t>(0x4030);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));

  freertos::da::mutex a;
  auto b = std::move(a);
}

TEST_F(SemaphoreBranch3Test, SaMutexNormalDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::sa::mutex mtx;
}

TEST_F(SemaphoreBranch3Test, SaMutexMovedFromDestruction) {
  SemaphoreHandle_t sem1 = reinterpret_cast<SemaphoreHandle_t>(0x4031);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));

  freertos::sa::mutex a;
  auto b = std::move(a);
}

TEST_F(SemaphoreBranch3Test, DaRecursiveMutexNormalDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::da::recursive_mutex rm;
}

TEST_F(SemaphoreBranch3Test, DaRecursiveMutexMovedFromDestruction) {
  SemaphoreHandle_t sem1 = reinterpret_cast<SemaphoreHandle_t>(0x4040);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));

  freertos::da::recursive_mutex a;
  auto b = std::move(a);
}

TEST_F(SemaphoreBranch3Test, SaRecursiveMutexNormalDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));

  freertos::sa::recursive_mutex rm;
}

TEST_F(SemaphoreBranch3Test, SaRecursiveMutexMovedFromDestruction) {
  SemaphoreHandle_t sem1 = reinterpret_cast<SemaphoreHandle_t>(0x4041);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(sem1));
  EXPECT_CALL(*mock, vSemaphoreDelete(sem1));

  freertos::sa::recursive_mutex a;
  auto b = std::move(a);
}

// ================================================================
// STREAM BUFFER BRANCH TESTS — sa:: and da:: with destructor + moved-from
// ================================================================

class StreamBufferBranch3Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t fake_sb = reinterpret_cast<StreamBufferHandle_t>(0x7001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(StreamBufferBranch3Test, DaStreamBufferNormalDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::da::stream_buffer<64> sb(1);
}

TEST_F(StreamBufferBranch3Test, DaStreamBufferMovedFromDestruction) {
  StreamBufferHandle_t sb1 = reinterpret_cast<StreamBufferHandle_t>(0x7010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreate(_, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));

  freertos::da::stream_buffer<64> a(1);
  auto b = std::move(a);
}

TEST_F(StreamBufferBranch3Test, SaStreamBufferNormalDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));

  freertos::sa::stream_buffer<256> sb(1);
}

TEST_F(StreamBufferBranch3Test, SaStreamBufferMovedFromDestruction) {
  StreamBufferHandle_t sb1 = reinterpret_cast<StreamBufferHandle_t>(0x7011);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(sb1));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb1));

  freertos::sa::stream_buffer<256> a(1);
  auto b = std::move(a);
}

// ================================================================
// MESSAGE BUFFER BRANCH TESTS — sa:: and da:: with destructor + moved-from
// ================================================================

class MessageBufferBranch3Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t fake_mb = reinterpret_cast<MessageBufferHandle_t>(0x8001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(MessageBufferBranch3Test, DaMessageBufferNormalDestruction) {
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::da::message_buffer<64> mb;
}

TEST_F(MessageBufferBranch3Test, DaMessageBufferMovedFromDestruction) {
  MessageBufferHandle_t mb1 = reinterpret_cast<MessageBufferHandle_t>(0x8010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xMessageBufferCreate(_)).WillOnce(Return(mb1));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb1));

  freertos::da::message_buffer<64> a;
  auto b = std::move(a);
}

TEST_F(MessageBufferBranch3Test, SaMessageBufferNormalDestruction) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));

  freertos::sa::message_buffer<64> mb;
}

TEST_F(MessageBufferBranch3Test, SaMessageBufferMovedFromDestruction) {
  MessageBufferHandle_t mb1 = reinterpret_cast<MessageBufferHandle_t>(0x8011);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(mb1));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb1));

  freertos::sa::message_buffer<64> a;
  auto b = std::move(a);
}

// ================================================================
// SW TIMER BRANCH TESTS — additional branches
// ================================================================

class TimerBranch3Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TimerHandle_t fake_timer = reinterpret_cast<TimerHandle_t>(0x1001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(TimerBranch3Test, SaTimerNormalDestruction) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::sa::timer t("sa_t", 100, pdTRUE, []() {});
}

TEST_F(TimerBranch3Test, SaTimerMovedFromDestruction) {
  TimerHandle_t t1 = reinterpret_cast<TimerHandle_t>(0x1010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, vTimerSetTimerID(t1, _));
  EXPECT_CALL(*mock, xTimerDelete(t1, portMAX_DELAY));

  freertos::sa::timer a("sa_a", 100, pdTRUE, []() {});
  auto b = std::move(a);
}

TEST_F(TimerBranch3Test, DaTimerMoveConstructWithNullHandle) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer a("a", 100, pdTRUE, []() {});
  freertos::da::timer b("b", 100, pdTRUE, []() {});
}

TEST_F(TimerBranch3Test, DaTimerStartNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.start(0);
  EXPECT_EQ(rc, pdFAIL);
}

TEST_F(TimerBranch3Test, DaTimerStopNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.stop(0);
  EXPECT_EQ(rc, pdFAIL);
}

TEST_F(TimerBranch3Test, DaTimerResetNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.reset(0);
  EXPECT_EQ(rc, pdFAIL);
}

TEST_F(TimerBranch3Test, DaTimerPeriodNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.period(200, 0);
  EXPECT_EQ(rc, pdFAIL);
}

TEST_F(TimerBranch3Test, DaTimerStartIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(TimerBranch3Test, DaTimerStopIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.stop_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(TimerBranch3Test, DaTimerResetIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(nullptr));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(TimerBranch3Test, DaTimerStartIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_isr();
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(TimerBranch3Test, DaTimerStopIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStopFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start();
  auto result = t.stop_isr();
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(TimerBranch3Test, DaTimerResetSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerReset(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start();
  auto rc = t.reset();
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(TimerBranch3Test, DaTimerResetIsrSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerResetFromISR(fake_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_isr();
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(TimerBranch3Test, DaTimerPeriodSuccess) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(fake_timer, 200, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.period(200, portMAX_DELAY);
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(TimerBranch3Test, DaTimerStartSuccessMStarted) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  auto rc = t.start();
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(TimerBranch3Test, DaTimerStopSuccessMStarted) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerStart(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(fake_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
  t.start();
  auto rc = t.stop();
  EXPECT_EQ(rc, pdPASS);
}

TEST_F(TimerBranch3Test, SaTimerSwapBothValid) {
  TimerHandle_t t1 = reinterpret_cast<TimerHandle_t>(0x1020);
  TimerHandle_t t2 = reinterpret_cast<TimerHandle_t>(0x1021);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(t2));
  EXPECT_CALL(*mock, vTimerSetTimerID(t2, _));
  EXPECT_CALL(*mock, vTimerSetTimerID(t1, _));
  EXPECT_CALL(*mock, xTimerDelete(t1, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerDelete(t2, portMAX_DELAY));

  freertos::sa::timer a("a", 100, pdTRUE, []() {});
  freertos::sa::timer b("b", 200, pdFALSE, []() {});
  a.swap(b);
}

TEST_F(TimerBranch3Test, DaTimerSelfMoveAssignment) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));

  freertos::da::timer t("t", 100, pdTRUE, []() {});
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  t = std::move(t);
#pragma GCC diagnostic pop
}

// ================================================================
// EVENT GROUP BRANCH TESTS — sa:: and da:: with destructor + moved-from
// ================================================================

class EventGroupBranch3Test : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t fake_eg = reinterpret_cast<EventGroupHandle_t>(0x6001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(EventGroupBranch3Test, DaEventGroupNormalDestruction) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
}

TEST_F(EventGroupBranch3Test, DaEventGroupMovedFromDestruction) {
  EventGroupHandle_t eg1 = reinterpret_cast<EventGroupHandle_t>(0x6010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(eg1));
  EXPECT_CALL(*mock, vEventGroupDelete(eg1));

  freertos::da::event_group a;
  auto b = std::move(a);
}

TEST_F(EventGroupBranch3Test, SaEventGroupNormalDestruction) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::sa::event_group eg;
}

TEST_F(EventGroupBranch3Test, SaEventGroupMovedFromDestruction) {
  EventGroupHandle_t eg1 = reinterpret_cast<EventGroupHandle_t>(0x6011);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(eg1));
  EXPECT_CALL(*mock, vEventGroupDelete(eg1));

  freertos::sa::event_group a;
  auto b = std::move(a);
}

TEST_F(EventGroupBranch3Test, DaSetBitsExIsrSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(fake_eg, 0x01, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.set_bits_ex_isr(0x01);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(EventGroupBranch3Test, DaSetBitsExIsrFailure) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(fake_eg, 0x01, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.set_bits_ex_isr(0x01);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(EventGroupBranch3Test, SaSetBitsExIsrSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(fake_eg, 0x02, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdTRUE), Return(pdPASS)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::sa::event_group eg;
  auto result = eg.set_bits_ex_isr(0x02);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(EventGroupBranch3Test, SaSetBitsExIsrFailure) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(fake_eg, 0x02, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::sa::event_group eg;
  auto result = eg.set_bits_ex_isr(0x02);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(EventGroupBranch3Test, DaSyncExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 100))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(EventGroupBranch3Test, SaSyncExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 100))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::sa::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(EventGroupBranch3Test, DaWaitBitsExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, 100))
      .WillOnce(Return(static_cast<EventBits_t>(0x00)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(EventGroupBranch3Test, SaWaitBitsExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, 100))
      .WillOnce(Return(static_cast<EventBits_t>(0x00)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));

  freertos::sa::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}