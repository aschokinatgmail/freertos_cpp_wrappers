#include <array>
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/freertos_isr_result.hpp"
#include "FreeRTOS.h"
#include "freertos_event_group.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_queue.hpp"
#include "freertos_stream_buffer.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

static void test_send_callback(StreamBufferHandle_t, BaseType_t *, void *) {}
static void test_receive_callback(StreamBufferHandle_t, BaseType_t *, void *) {}

class StreamBufferCallbackTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_stream_buffer_handle =
        reinterpret_cast<StreamBufferHandle_t>(0x12345678);
    mock_message_buffer_handle =
        reinterpret_cast<MessageBufferHandle_t>(0x87654321);
    mock_queue_handle = reinterpret_cast<QueueHandle_t>(0xAAAA0000);
    mock_event_group_handle =
        reinterpret_cast<EventGroupHandle_t>(0xBBBB0000);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t mock_stream_buffer_handle;
  MessageBufferHandle_t mock_message_buffer_handle;
  QueueHandle_t mock_queue_handle;
  EventGroupHandle_t mock_event_group_handle;
};

TEST_F(StreamBufferCallbackTest, StreamBufferResetIsr) {
  EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferResetFromISR(mock_stream_buffer_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::da::stream_buffer<128> buffer;
  auto result = buffer.reset_isr();
  EXPECT_TRUE(result.result);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(StreamBufferCallbackTest, StreamBufferResetIsrFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferResetFromISR(mock_stream_buffer_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::da::stream_buffer<128> buffer;
  auto result = buffer.reset_isr();
  EXPECT_FALSE(result.result);
}

// Note: xStreamBufferResetFromISR in production FreeRTOS does NOT have a
// higher_priority_task_woken out-parameter — the woken flag is always pdFALSE
// in the returned isr_result. Test renamed and updated accordingly.
TEST_F(StreamBufferCallbackTest, StreamBufferResetIsrSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferResetFromISR(mock_stream_buffer_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::da::stream_buffer<128> buffer;
  auto result = buffer.reset_isr();
  EXPECT_TRUE(result.result);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(StreamBufferCallbackTest, StreamBufferResetExIsr) {
  EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferResetFromISR(mock_stream_buffer_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::da::stream_buffer<128> buffer;
  auto result = buffer.reset_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(StreamBufferCallbackTest, StreamBufferResetExIsrFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferResetFromISR(mock_stream_buffer_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::da::stream_buffer<128> buffer;
  auto result = buffer.reset_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(StreamBufferCallbackTest, StreamBufferSetNotificationIndex) {
  EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferSetStreamBufferNotificationIndex(
                          mock_stream_buffer_handle, 2));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::da::stream_buffer<128> buffer;
  buffer.set_notification_index(2);
}

TEST_F(StreamBufferCallbackTest, StreamBufferGetStaticBuffers) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(128, 1, NotNull(), NotNull()))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferGetStaticBuffers(mock_stream_buffer_handle,
                                                    NotNull(), NotNull()))
      .WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<uint8_t *>(0x1000)),
                       SetArgPointee<2>(
                           reinterpret_cast<StaticStreamBuffer_t *>(0x2000)),
                       Return(pdPASS)));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::sa::stream_buffer<128> buffer;
  uint8_t *storage = nullptr;
  StaticStreamBuffer_t *static_buf = nullptr;
  bool result = buffer.get_static_buffers(&storage, &static_buf);
  EXPECT_TRUE(result);
}

TEST_F(StreamBufferCallbackTest, StreamBufferGetStaticBuffersFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(128, 1, NotNull(), NotNull()))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferGetStaticBuffers(mock_stream_buffer_handle,
                                                    NotNull(), NotNull()))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::sa::stream_buffer<128> buffer;
  uint8_t *storage = nullptr;
  StaticStreamBuffer_t *static_buf = nullptr;
  bool result = buffer.get_static_buffers(&storage, &static_buf);
  EXPECT_FALSE(result);
}

TEST_F(StreamBufferCallbackTest, StreamBufferDynamicCallbackCreation) {
  EXPECT_CALL(*mock,
              xStreamBufferCreateWithCallback(128, 1, NotNull(), nullptr,
                                              NotNull(), nullptr))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::stream_buffer<128,
                         freertos::dynamic_stream_buffer_allocator_with_callback<
                             128>>
      buffer(1, test_send_callback, nullptr, test_receive_callback, nullptr);
}

TEST_F(StreamBufferCallbackTest, StreamBufferStaticCallbackCreation) {
  EXPECT_CALL(*mock,
              xStreamBufferCreateStaticWithCallback(
                  128, 1, NotNull(), NotNull(), NotNull(), nullptr, NotNull(),
                  nullptr))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::stream_buffer<128,
                         freertos::static_stream_buffer_allocator_with_callback<
                             128>>
      buffer(1, test_send_callback, nullptr, test_receive_callback, nullptr);
}

TEST_F(StreamBufferCallbackTest, MessageBufferResetIsr) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128))
      .WillOnce(Return(mock_message_buffer_handle));
  EXPECT_CALL(*mock,
              xMessageBufferResetFromISR(mock_message_buffer_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));

  freertos::da::message_buffer<128> buffer;
  auto result = buffer.reset_isr();
  EXPECT_TRUE(result.result);
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(StreamBufferCallbackTest, MessageBufferResetIsrFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128))
      .WillOnce(Return(mock_message_buffer_handle));
  EXPECT_CALL(*mock,
              xMessageBufferResetFromISR(mock_message_buffer_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));

  freertos::da::message_buffer<128> buffer;
  auto result = buffer.reset_isr();
  EXPECT_FALSE(result.result);
}

TEST_F(StreamBufferCallbackTest, MessageBufferResetExIsr) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128))
      .WillOnce(Return(mock_message_buffer_handle));
  EXPECT_CALL(*mock,
              xMessageBufferResetFromISR(mock_message_buffer_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));

  freertos::da::message_buffer<128> buffer;
  auto result = buffer.reset_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(StreamBufferCallbackTest, MessageBufferResetExIsrFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128))
      .WillOnce(Return(mock_message_buffer_handle));
  EXPECT_CALL(*mock,
              xMessageBufferResetFromISR(mock_message_buffer_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));

  freertos::da::message_buffer<128> buffer;
  auto result = buffer.reset_ex_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(StreamBufferCallbackTest, MessageBufferSetNotificationIndex) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128))
      .WillOnce(Return(mock_message_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferSetStreamBufferNotificationIndex(
                          mock_message_buffer_handle, 1));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));

  freertos::da::message_buffer<128> buffer;
  buffer.set_notification_index(1);
}

TEST_F(StreamBufferCallbackTest, MessageBufferGetStaticBuffers) {
  EXPECT_CALL(*mock,
              xMessageBufferCreateStatic(128, NotNull(), NotNull()))
      .WillOnce(Return(mock_message_buffer_handle));
  EXPECT_CALL(*mock, xMessageBufferGetStaticBuffers(mock_message_buffer_handle,
                                                     NotNull(), NotNull()))
      .WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<uint8_t *>(0x3000)),
                       SetArgPointee<2>(
                           reinterpret_cast<StaticMessageBuffer_t *>(0x4000)),
                       Return(pdPASS)));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));

  freertos::sa::message_buffer<128> buffer;
  uint8_t *storage = nullptr;
  StaticMessageBuffer_t *static_buf = nullptr;
  bool result = buffer.get_static_buffers(&storage, &static_buf);
  EXPECT_TRUE(result);
}

TEST_F(StreamBufferCallbackTest, MessageBufferDynamicCallbackCreation) {
  EXPECT_CALL(*mock,
              xMessageBufferCreateWithCallback(128, NotNull(), nullptr,
                                               NotNull(), nullptr))
      .WillOnce(Return(mock_message_buffer_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));

  freertos::message_buffer<
      128, freertos::dynamic_message_buffer_allocator_with_callback<128>>
      buffer(test_send_callback, nullptr, test_receive_callback, nullptr);
}

TEST_F(StreamBufferCallbackTest, MessageBufferStaticCallbackCreation) {
  EXPECT_CALL(*mock,
              xMessageBufferCreateStaticWithCallback(
                  128, NotNull(), NotNull(), NotNull(), nullptr, NotNull(),
                  nullptr))
      .WillOnce(Return(mock_message_buffer_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));

  freertos::message_buffer<
      128, freertos::static_message_buffer_allocator_with_callback<128>>
      buffer(test_send_callback, nullptr, test_receive_callback, nullptr);
}

TEST_F(StreamBufferCallbackTest, QueueGetStaticBuffers) {
  EXPECT_CALL(*mock, xQueueCreateStatic(10, 4, NotNull(), NotNull()))
      .WillOnce(Return(mock_queue_handle));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*mock,
              xQueueGetStaticBuffers(mock_queue_handle, NotNull(), NotNull()))
      .WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<uint8_t *>(0x5000)),
                       SetArgPointee<2>(
                           reinterpret_cast<StaticQueue_t *>(0x6000)),
                       Return(pdPASS)));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));

  freertos::sa::queue<10, int32_t> q;
  uint8_t *storage = nullptr;
  StaticQueue_t *static_q = nullptr;
  bool result = q.get_static_buffers(&storage, &static_q);
  EXPECT_TRUE(result);
}

TEST_F(StreamBufferCallbackTest, QueueGetStaticBuffersFailure) {
  EXPECT_CALL(*mock, xQueueCreateStatic(10, 4, NotNull(), NotNull()))
      .WillOnce(Return(mock_queue_handle));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*mock,
              xQueueGetStaticBuffers(mock_queue_handle, NotNull(), NotNull()))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));

  freertos::sa::queue<10, int32_t> q;
  uint8_t *storage = nullptr;
  StaticQueue_t *static_q = nullptr;
  bool result = q.get_static_buffers(&storage, &static_q);
  EXPECT_FALSE(result);
}

TEST_F(StreamBufferCallbackTest, EventGroupGetStaticBuffer) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(NotNull()))
      .WillOnce(Return(mock_event_group_handle));
  EXPECT_CALL(*mock, xEventGroupGetStaticBuffer(mock_event_group_handle,
                                                 NotNull()))
      .WillOnce(DoAll(SetArgPointee<1>(
                           reinterpret_cast<StaticEventGroup_t *>(0x7000)),
                       Return(pdPASS)));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));

  freertos::sa::event_group eg;
  StaticEventGroup_t *static_eg = nullptr;
  bool result = eg.get_static_buffer(&static_eg);
  EXPECT_TRUE(result);
}

TEST_F(StreamBufferCallbackTest, EventGroupGetStaticBufferFailure) {
  EXPECT_CALL(*mock, xEventGroupCreateStatic(NotNull()))
      .WillOnce(Return(mock_event_group_handle));
  EXPECT_CALL(*mock, xEventGroupGetStaticBuffer(mock_event_group_handle,
                                                 NotNull()))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));

  freertos::sa::event_group eg;
  StaticEventGroup_t *static_eg = nullptr;
  bool result = eg.get_static_buffer(&static_eg);
  EXPECT_FALSE(result);
}