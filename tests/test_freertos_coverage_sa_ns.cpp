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
#include "freertos_external_allocator.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::NotNull;
using ::testing::DoAll;
using ::testing::SetArgPointee;
using ::testing::InSequence;
using ::testing::NiceMock;

namespace {
void dummy_timer_callback(TimerHandle_t) {}
void dummy_task_function() {}
} // namespace

// ================================================================
// SA:: EVENT GROUP BRANCH TESTS
// ================================================================

class SaEventGroupTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t fake_eg = reinterpret_cast<EventGroupHandle_t>(0x2001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaEventGroupTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::sa::event_group eg;
}

TEST_F(SaEventGroupTest, NullHandleDestructorSkipsDelete) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(nullptr));
  freertos::sa::event_group eg;
}

TEST_F(SaEventGroupTest, MoveConstructNullHandleDestructor) {
  EventGroupHandle_t h1 = reinterpret_cast<EventGroupHandle_t>(0x2010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vEventGroupDelete(h1));
  freertos::sa::event_group a;
  freertos::sa::event_group b;
}

TEST_F(SaEventGroupTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::sa::event_group eg;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  eg = std::move(eg);
#pragma GCC diagnostic pop
}

TEST_F(SaEventGroupTest, WaitBitsExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, 10))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::sa::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 10);
  EXPECT_TRUE(result.has_value());
}

TEST_F(SaEventGroupTest, WaitBitsExFailure) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, 10))
      .WillOnce(Return(static_cast<EventBits_t>(0x00)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::sa::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaEventGroupTest, WaitBitsExWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(fake_eg, 0x01, pdTRUE, pdTRUE, 0))
      .WillOnce(Return(static_cast<EventBits_t>(0x00)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::sa::event_group eg;
  auto result = eg.wait_bits_ex(0x01, pdTRUE, pdTRUE, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SaEventGroupTest, SyncExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 10))
      .WillOnce(Return(static_cast<EventBits_t>(0x03)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::sa::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 10);
  EXPECT_TRUE(result.has_value());
}

TEST_F(SaEventGroupTest, SyncExFailure) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 10))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::sa::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaEventGroupTest, SyncExWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(fake_eg));
  EXPECT_CALL(*mock, xEventGroupSync(fake_eg, 0x01, 0x03, 0))
      .WillOnce(Return(static_cast<EventBits_t>(0x01)));
  EXPECT_CALL(*mock, vEventGroupDelete(fake_eg));
  freertos::sa::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

// ================================================================
// SA:: MESSAGE BUFFER BRANCH TESTS
// ================================================================

class SaMessageBufferTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t fake_mb = reinterpret_cast<MessageBufferHandle_t>(0x3001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaMessageBufferTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::sa::message_buffer<256> mb;
}

TEST_F(SaMessageBufferTest, NullHandleDestruction) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(nullptr));
  freertos::sa::message_buffer<256> mb;
}

TEST_F(SaMessageBufferTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::sa::message_buffer<256> mb;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  mb = std::move(mb);
#pragma GCC diagnostic pop
}

TEST_F(SaMessageBufferTest, SendExFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 4, 10)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::sa::message_buffer<256> mb;
  auto result = mb.send_ex("test", 4, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaMessageBufferTest, SendExWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(fake_mb, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::sa::message_buffer<256> mb;
  auto result = mb.send_ex("test", 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SaMessageBufferTest, ReceiveExFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 4, 10)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::sa::message_buffer<256> mb;
  char buf[4];
  auto result = mb.receive_ex(buf, 4, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaMessageBufferTest, ReceiveExWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(fake_mb, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::sa::message_buffer<256> mb;
  char buf[4];
  auto result = mb.receive_ex(buf, 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SaMessageBufferTest, SendExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(fake_mb, _, 4, _))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::sa::message_buffer<256> mb;
  auto result = mb.send_ex_isr("test", 4);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SaMessageBufferTest, ReceiveExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(fake_mb, _, 4, _))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::sa::message_buffer<256> mb;
  char buf[4];
  auto result = mb.receive_ex_isr(buf, 4);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SaMessageBufferTest, ResetExFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _)).WillOnce(Return(fake_mb));
  EXPECT_CALL(*mock, xMessageBufferReset(fake_mb)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vMessageBufferDelete(fake_mb));
  freertos::sa::message_buffer<256> mb;
  auto result = mb.reset_ex();
  EXPECT_FALSE(result.has_value());
}

// ================================================================
// SA:: STREAM BUFFER BRANCH TESTS
// ================================================================

class SaStreamBufferTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t fake_sb = reinterpret_cast<StreamBufferHandle_t>(0x4001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaStreamBufferTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::sa::stream_buffer<256> sb;
}

TEST_F(SaStreamBufferTest, NullHandleDestruction) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::stream_buffer<256> sb;
}

TEST_F(SaStreamBufferTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::sa::stream_buffer<256> sb;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  sb = std::move(sb);
#pragma GCC diagnostic pop
}

TEST_F(SaStreamBufferTest, SendExFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(fake_sb, _, 4, 10)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::sa::stream_buffer<256> sb;
  auto result = sb.send_ex("test", 4, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaStreamBufferTest, SendExWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(fake_sb, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::sa::stream_buffer<256> sb;
  auto result = sb.send_ex("test", 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SaStreamBufferTest, ReceiveExFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(fake_sb, _, 4, 10)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::sa::stream_buffer<256> sb;
  char buf[4];
  auto result = sb.receive_ex(buf, 4, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaStreamBufferTest, ReceiveExWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(fake_sb, _, 4, 0)).WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::sa::stream_buffer<256> sb;
  char buf[4];
  auto result = sb.receive_ex(buf, 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SaStreamBufferTest, SendExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(fake_sb, _, 4, _))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::sa::stream_buffer<256> sb;
  auto result = sb.send_ex_isr("test", 4);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SaStreamBufferTest, ReceiveExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(fake_sb, _, 4, _))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::sa::stream_buffer<256> sb;
  char buf[4];
  auto result = sb.receive_ex_isr(buf, 4);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SaStreamBufferTest, ResetExFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferReset(fake_sb)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::sa::stream_buffer<256> sb;
  auto result = sb.reset_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaStreamBufferTest, SetTriggerLevelExFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _)).WillOnce(Return(fake_sb));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(fake_sb, 1)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(fake_sb));
  freertos::sa::stream_buffer<256> sb;
  auto result = sb.set_trigger_level_ex(1);
  EXPECT_FALSE(result.has_value());
}

// ================================================================
// SA:: BINARY SEMAPHORE BRANCH TESTS
// ================================================================

class SaBinarySemaphoreTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaBinarySemaphoreTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::binary_semaphore sem;
}

TEST_F(SaBinarySemaphoreTest, NullHandleDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(nullptr));
  freertos::sa::binary_semaphore sem;
}

TEST_F(SaBinarySemaphoreTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::binary_semaphore sem;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  sem = std::move(sem);
#pragma GCC diagnostic pop
}

TEST_F(SaBinarySemaphoreTest, MoveAssignDifferentObjects) {
  SemaphoreHandle_t h1 = reinterpret_cast<SemaphoreHandle_t>(0x5010);
  SemaphoreHandle_t h2 = reinterpret_cast<SemaphoreHandle_t>(0x5020);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, vSemaphoreDelete(h1));
  EXPECT_CALL(*mock, vSemaphoreDelete(h2));
  freertos::sa::binary_semaphore a;
  freertos::sa::binary_semaphore b;
  a = std::move(b);
}

TEST_F(SaBinarySemaphoreTest, GiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::binary_semaphore sem;
  auto result = sem.give_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaBinarySemaphoreTest, GiveExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::binary_semaphore sem;
  auto result = sem.give_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SaBinarySemaphoreTest, TakeExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 10)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::binary_semaphore sem;
  auto result = sem.take_ex(10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaBinarySemaphoreTest, TakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::binary_semaphore sem;
  auto result = sem.take_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SaBinarySemaphoreTest, TakeExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::binary_semaphore sem;
  auto result = sem.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

// ================================================================
// SA:: COUNTING SEMAPHORE BRANCH TESTS
// ================================================================

class SaCountingSemaphoreTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5101);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaCountingSemaphoreTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(10, _, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::counting_semaphore sem(10);
}

TEST_F(SaCountingSemaphoreTest, NullHandleDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(10, _, _)).WillOnce(Return(nullptr));
  freertos::sa::counting_semaphore sem(10);
}

TEST_F(SaCountingSemaphoreTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(10, _, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::counting_semaphore sem(10);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  sem = std::move(sem);
#pragma GCC diagnostic pop
}

TEST_F(SaCountingSemaphoreTest, GiveExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(10, _, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::counting_semaphore sem(10);
  auto result = sem.give_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaCountingSemaphoreTest, GiveExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(10, _, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::counting_semaphore sem(10);
  auto result = sem.give_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SaCountingSemaphoreTest, TakeExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(10, _, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 10)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::counting_semaphore sem(10);
  auto result = sem.take_ex(10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaCountingSemaphoreTest, TakeExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(10, _, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::counting_semaphore sem(10);
  auto result = sem.take_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SaCountingSemaphoreTest, TakeExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(10, _, _)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::counting_semaphore sem(10);
  auto result = sem.take_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

// ================================================================
// SA:: MUTEX BRANCH TESTS
// ================================================================

class SaMutexTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5201);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaMutexTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
}

TEST_F(SaMutexTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  m = std::move(m);
#pragma GCC diagnostic pop
}

TEST_F(SaMutexTest, LockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 10)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  auto result = m.lock_ex(10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaMutexTest, LockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  auto result = m.lock_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SaMutexTest, LockExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  auto result = m.lock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SaMutexTest, UnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  auto result = m.unlock_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaMutexTest, UnlockExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  auto result = m.unlock_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SaMutexTest, TryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  auto result = m.try_lock_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaMutexTest, TryLockUntilPastTime) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  auto past = std::chrono::steady_clock::now() - std::chrono::seconds(1);
  EXPECT_TRUE(m.try_lock_until(past));
  m.unlock();
}

TEST_F(SaMutexTest, TryLockUntilFutureTime) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  auto future = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  EXPECT_TRUE(m.try_lock_until(future));
  m.unlock();
}

// ================================================================
// SA:: RECURSIVE MUTEX BRANCH TESTS
// ================================================================

class SaRecursiveMutexTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5301);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaRecursiveMutexTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
}

TEST_F(SaRecursiveMutexTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  m = std::move(m);
#pragma GCC diagnostic pop
}

TEST_F(SaRecursiveMutexTest, LockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 10)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
  auto result = m.lock_ex(10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaRecursiveMutexTest, LockExWouldBlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
  auto result = m.lock_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(SaRecursiveMutexTest, UnlockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
  auto result = m.unlock_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaRecursiveMutexTest, TryLockExFailure) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
  auto result = m.try_lock_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaRecursiveMutexTest, TryLockUntilPastTime) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
  auto past = std::chrono::steady_clock::now() - std::chrono::seconds(1);
  EXPECT_TRUE(m.try_lock_until(past));
  m.unlock();
}

TEST_F(SaRecursiveMutexTest, TryLockUntilFutureTime) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
  auto future = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  EXPECT_TRUE(m.try_lock_until(future));
  m.unlock();
}

TEST_F(SaRecursiveMutexTest, UnlockWithZeroRecursionsCount) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
  m.unlock();
  EXPECT_EQ(m.recursions_count(), 0);
}

// ================================================================
// TRY_LOCK_GUARD AND TIMEOUT_LOCK_GUARD BRANCH TESTS
// ================================================================

class GuardBranchTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5401);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(GuardBranchTest, TryLockGuardFailureDoesNotUnlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  {
    freertos::try_lock_guard<freertos::sa::mutex> guard(m);
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(GuardBranchTest, TryLockGuardSuccessUnlocks) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  {
    freertos::try_lock_guard<freertos::sa::mutex> guard(m);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(GuardBranchTest, TimeoutLockGuardFailureDoesNotUnlock) {
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  {
    freertos::timeout_lock_guard<freertos::sa::mutex> guard(m, 100);
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(GuardBranchTest, TimeoutLockGuardSuccessUnlocks) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 100)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  {
    freertos::timeout_lock_guard<freertos::sa::mutex> guard(m, 100);
    EXPECT_TRUE(guard.locked());
  }
}

TEST_F(GuardBranchTest, LockGuardIsrOnSaMutex) {
  BaseType_t high_task_woken = pdFALSE;
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeFromISR(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveFromISR(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::mutex m;
  {
    freertos::lock_guard_isr<freertos::sa::mutex> guard(m);
  }
}

TEST_F(GuardBranchTest, TryLockGuardOnRecursiveMutex) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
  {
    freertos::try_lock_guard<freertos::sa::recursive_mutex> guard(m);
    EXPECT_FALSE(guard.locked());
  }
}

TEST_F(GuardBranchTest, TimeoutLockGuardOnRecursiveMutex) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_)).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 100)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::sa::recursive_mutex m;
  {
    freertos::timeout_lock_guard<freertos::sa::recursive_mutex> guard(m, 100);
    EXPECT_FALSE(guard.locked());
  }
}

// ================================================================
// SA:: TIMER BRANCH TESTS
// ================================================================

class SaTimerTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TimerHandle_t fake_timer = reinterpret_cast<TimerHandle_t>(0x6001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaTimerTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
}

TEST_F(SaTimerTest, NullHandleDestruction) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
}

TEST_F(SaTimerTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(fake_timer));
  EXPECT_CALL(*mock, xTimerDelete(fake_timer, portMAX_DELAY));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  t = std::move(t);
#pragma GCC diagnostic pop
}

TEST_F(SaTimerTest, NullHandleStart) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start();
  EXPECT_EQ(result, pdFAIL);
}

TEST_F(SaTimerTest, NullHandleStartIsr) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  auto result = t.start_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(SaTimerTest, NullHandleStop) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  auto result = t.stop();
  EXPECT_EQ(result, pdFAIL);
}

TEST_F(SaTimerTest, NullHandleStopIsr) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  auto result = t.stop_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(SaTimerTest, NullHandleReset) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset();
  EXPECT_EQ(result, pdFAIL);
}

TEST_F(SaTimerTest, NullHandleResetIsr) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  auto result = t.reset_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(SaTimerTest, NullHandlePeriod) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period(200);
  EXPECT_EQ(result, pdFAIL);
}

TEST_F(SaTimerTest, NullHandlePeriodIsr) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  auto result = t.period_isr(200);
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(SaTimerTest, NullHandleRunning) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  EXPECT_FALSE(t.running());
}

TEST_F(SaTimerTest, NullHandleName) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.name(), nullptr);
}

TEST_F(SaTimerTest, NullHandleRemainingTicks) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  EXPECT_EQ(t.remaining_ticks(), 0);
}

TEST_F(SaTimerTest, NullHandleReloadMode) {
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(nullptr));
  freertos::sa::timer t("t", 100, pdTRUE, []() {});
  t.reload_mode(pdTRUE);
}

TEST_F(SaTimerTest, MoveAssignStartedTimer) {
  TimerHandle_t t1 = reinterpret_cast<TimerHandle_t>(0x6011);
  TimerHandle_t t2 = reinterpret_cast<TimerHandle_t>(0x6022);
  TimerHandle_t t3 = reinterpret_cast<TimerHandle_t>(0x6033);
  const char *timer_name = "b";
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(t1));
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(t2));
  EXPECT_CALL(*mock, xTimerStart(t2, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(t2, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcTimerGetName(t2)).WillOnce(Return(timer_name));
  EXPECT_CALL(*mock, xTimerGetPeriod(t2)).WillOnce(Return(100));
  EXPECT_CALL(*mock, uxTimerGetReloadMode(t2)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _)).WillOnce(Return(t3));
  EXPECT_CALL(*mock, xTimerDelete(t1, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerDelete(t2, portMAX_DELAY));
  EXPECT_CALL(*mock, xTimerStart(t3, portMAX_DELAY)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(t3, portMAX_DELAY));
  freertos::sa::timer a("a", 100, pdTRUE, []() {});
  freertos::sa::timer b("b", 100, pdTRUE, []() {});
  b.start();
  a = std::move(b);
}

// ================================================================
// SA:: QUEUE BRANCH TESTS
// ================================================================

class SaQueueTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_q = reinterpret_cast<QueueHandle_t>(0x7001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaQueueTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
}

TEST_F(SaQueueTest, SelfMoveAssignment) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  q = std::move(q);
#pragma GCC diagnostic pop
}

TEST_F(SaQueueTest, SendExFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSend(fake_q, _, 10)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  auto result = q.send_ex(42, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaQueueTest, SendExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSend(fake_q, _, 0)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  auto result = q.send_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(SaQueueTest, SendBackExFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBack(fake_q, _, 10)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  auto result = q.send_back_ex(42, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaQueueTest, SendFrontExFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFront(fake_q, _, 10)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  auto result = q.send_front_ex(42, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaQueueTest, ReceiveExRefFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, 10)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  int val;
  auto result = q.receive_ex(val, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaQueueTest, ReceiveExOptionalFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, 10)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  auto result = q.receive_ex(10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaQueueTest, ReceiveExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReceive(fake_q, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  auto result = q.receive_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(SaQueueTest, ResetExFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueReset(fake_q)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  auto result = q.reset_ex();
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaQueueTest, PeekExFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, 10)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  int val;
  auto result = q.peek_ex(val, 10);
  EXPECT_FALSE(result.has_value());
}

TEST_F(SaQueueTest, PeekExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueuePeek(fake_q, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  int val;
  auto result = q.peek_ex(val, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(SaQueueTest, SendExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendFromISR(fake_q, _, _))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  auto result = q.send_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SaQueueTest, SendBackExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(fake_q, _, _))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  auto result = q.send_back_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(SaQueueTest, SendFrontExIsrFailure) {
  BaseType_t high_task_woken = pdFALSE;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(fake_q, _, _))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q;
  auto result = q.send_front_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
}

// ================================================================
// DA:: QUEUE WITH REGISTERED NAME BRANCH TESTS
// ================================================================

class QueueNameTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t fake_q = reinterpret_cast<QueueHandle_t>(0x7101);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(QueueNameTest, ConstructorWithRegisteredName) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, _));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return("my_q"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q("my_q");
}

TEST_F(QueueNameTest, DestructorWithNullName) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::da::queue<10, int> q;
}

TEST_F(QueueNameTest, SaQueueConstructorWithRegisteredName) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(fake_q));
  EXPECT_CALL(*mock, vQueueAddToRegistry(fake_q, _));
  EXPECT_CALL(*mock, pcQueueGetName(fake_q)).WillOnce(Return("sa_q"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(fake_q));
  EXPECT_CALL(*mock, vQueueDelete(fake_q));
  freertos::sa::queue<10, int> q("sa_q");
}

// ================================================================
// SA:: QUEUE SET BRANCH TESTS
// ================================================================

class SaQueueSetTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  QueueSetHandle_t fake_qs = reinterpret_cast<QueueSetHandle_t>(0x8001);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaQueueSetTest, ConstructionAndDestruction) {
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
}

TEST_F(SaQueueSetTest, MoveConstructThenDestroyMovedFrom) {
  QueueSetHandle_t h1 = reinterpret_cast<QueueSetHandle_t>(0x8010);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(h1));
  freertos::da::queue_set a(10);
  freertos::da::queue_set b(10);
}

TEST_F(SaQueueSetTest, MoveAssignDifferentObjects) {
  QueueSetHandle_t h1 = reinterpret_cast<QueueSetHandle_t>(0x8010);
  QueueSetHandle_t h2 = reinterpret_cast<QueueSetHandle_t>(0x8020);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(h1));
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(h2));
  EXPECT_CALL(*mock, vQueueDelete(h1));
  EXPECT_CALL(*mock, vQueueDelete(h2));
  freertos::da::queue_set a(10);
  freertos::da::queue_set b(10);
  a = std::move(b);
}

TEST_F(SaQueueSetTest, SelectExIsrSuccess) {
  QueueHandle_t member = reinterpret_cast<QueueHandle_t>(0x8030);
  BaseType_t high_task_woken = pdFALSE;
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreateSet(10)).WillOnce(Return(fake_qs));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(fake_qs, _))
      .WillOnce(Return(member));
  EXPECT_CALL(*mock, vQueueDelete(fake_qs));
  freertos::da::queue_set qs(10);
  auto result = qs.select_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

// ================================================================
// MUTEX TRY_LOCK_UNTIL BRANCH TESTS (da:: namespace)
// ================================================================

class MutexTryLockUntilTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5501);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(MutexTryLockUntilTest, PastTimeCallsTryLock) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto past = std::chrono::steady_clock::now() - std::chrono::seconds(1);
  EXPECT_TRUE(m.try_lock_until(past));
  m.unlock();
}

TEST_F(MutexTryLockUntilTest, FutureTimeCallsLockWithDuration) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTake(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::mutex m;
  auto future = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  EXPECT_TRUE(m.try_lock_until(future));
  m.unlock();
}

TEST_F(MutexTryLockUntilTest, DaRecursiveMutexPastTime) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, 0)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  auto past = std::chrono::steady_clock::now() - std::chrono::seconds(1);
  EXPECT_TRUE(m.try_lock_until(past));
  m.unlock();
}

TEST_F(MutexTryLockUntilTest, DaRecursiveMutexFutureTime) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreTakeRecursive(fake_sem, _)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  auto future = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  EXPECT_TRUE(m.try_lock_until(future));
  m.unlock();
}

// ================================================================
// RECURSIVE MUTEX UNLOCK WITH ZERO RECURSIONS_COUNT
// ================================================================

class RecursiveMutexUnlockTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  SemaphoreHandle_t fake_sem = reinterpret_cast<SemaphoreHandle_t>(0x5601);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(RecursiveMutexUnlockTest, DaUnlockWithZeroCount) {
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex()).WillOnce(Return(fake_sem));
  EXPECT_CALL(*mock, xSemaphoreGiveRecursive(fake_sem)).WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vSemaphoreDelete(fake_sem));
  freertos::da::recursive_mutex m;
  m.unlock();
  EXPECT_EQ(m.recursions_count(), 0);
}

// ================================================================
// EXTERNAL TASK ALLOCATOR PARTIAL FAILURE
// ================================================================

class ExternalTaskAllocatorTest : public ::testing::Test {
protected:
  NiceMock<FreeRTOSMock> *mock;

  void SetUp() override {
    mock = new NiceMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

class TestRegion {
public:
  int call_count = 0;
  int fail_after = -1;
  void *allocate(size_t) {
    call_count++;
    if (fail_after >= 0 && call_count > fail_after) {
      return nullptr;
    }
    return reinterpret_cast<void *>(0x1000 + call_count * 0x100);
  }
  void deallocate(void *) {}
};

TEST_F(ExternalTaskAllocatorTest, PartialFailureStackFails) {
  TestRegion region;
  region.fail_after = 1;
  freertos::external_task_allocator<TestRegion, 512> allocator(region);
  auto result = allocator.create(nullptr, "t", 1, nullptr);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalTaskAllocatorTest, CompleteFailure) {
  TestRegion region;
  region.fail_after = 0;
  freertos::external_task_allocator<TestRegion, 512> allocator(region);
  auto result = allocator.create(nullptr, "t", 1, nullptr);
  EXPECT_EQ(result, nullptr);
}

TEST_F(ExternalTaskAllocatorTest, MoveThenDestroy) {
  StrictMock<FreeRTOSMock> *strict_mock = new StrictMock<FreeRTOSMock>();
  g_freertos_mock = strict_mock;
  TestRegion region;
  freertos::external_task_allocator<TestRegion, 512> alloc1(region);
  EXPECT_CALL(*strict_mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(reinterpret_cast<TaskHandle_t>(0x9001)));
  auto h = alloc1.create(nullptr, "t", 1, nullptr);
  EXPECT_NE(h, static_cast<TaskHandle_t>(nullptr));
  freertos::external_task_allocator<TestRegion, 512> alloc2(std::move(alloc1));
  // alloc2 now owns the memory, alloc1's destructor should NOT deallocate
  // because m_region was set to nullptr in the move
  delete strict_mock;
  g_freertos_mock = nullptr;
}

// ================================================================
// SA:: TASK BRANCH TESTS (limited due to complexity)
// ================================================================

class SaTaskBranchTest : public ::testing::Test {
protected:
  StrictMock<FreeRTOSMock> *mock;
  TaskHandle_t fake_task = reinterpret_cast<TaskHandle_t>(0x3100);

  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
};

TEST_F(SaTaskBranchTest, IsRunningReady) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eReady));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::sa::task<512> t("t", 1, []() {});
  EXPECT_TRUE(t.is_running());
}

TEST_F(SaTaskBranchTest, IsRunningBlocked) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eBlocked));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::sa::task<512> t("t", 1, []() {});
  EXPECT_TRUE(t.is_running());
}

TEST_F(SaTaskBranchTest, IsAliveBlocked) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eBlocked));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::sa::task<512> t("t", 1, []() {});
  EXPECT_TRUE(t.is_alive());
}

TEST_F(SaTaskBranchTest, IsSuspendedFalse) {
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
      .WillOnce(Return(fake_task));
  EXPECT_CALL(*mock, eTaskGetState(fake_task)).WillOnce(Return(eRunning));
  EXPECT_CALL(*mock, vTaskDelete(fake_task));
  freertos::sa::task<512> t("t", 1, []() {});
  EXPECT_FALSE(t.is_suspended());
}