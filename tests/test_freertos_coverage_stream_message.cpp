#include <array>
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "FreeRTOS.h"
#include "../include/freertos_isr_result.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;
using ::testing::WithArg;

class StreamMessageCoverageTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    sb_handle = reinterpret_cast<StreamBufferHandle_t>(0x1000);
    sb_handle2 = reinterpret_cast<StreamBufferHandle_t>(0x2000);
    mb_handle = reinterpret_cast<MessageBufferHandle_t>(0x3000);
    mb_handle2 = reinterpret_cast<MessageBufferHandle_t>(0x4000);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t sb_handle;
  StreamBufferHandle_t sb_handle2;
  MessageBufferHandle_t mb_handle;
  MessageBufferHandle_t mb_handle2;
};

// =============================================================================
// Custom allocators for SFINAE constructor coverage (lines 126-130, 121-125)
// =============================================================================

template <size_t N> class tag_stream_allocator {
  int m_tag = 0;

public:
  tag_stream_allocator() = default;
  explicit tag_stream_allocator(int tag) : m_tag(tag) {}
  void swap(tag_stream_allocator &other) noexcept {
    using std::swap;
    swap(m_tag, other.m_tag);
  }
  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    (void)m_tag;
    return xStreamBufferCreate(N, trigger_level_bytes);
  }
};

template <size_t N> class tag_message_allocator {
  int m_tag = 0;

public:
  tag_message_allocator() = default;
  explicit tag_message_allocator(int tag) : m_tag(tag) {}
  void swap(tag_message_allocator &other) noexcept {
    using std::swap;
    swap(m_tag, other.m_tag);
  }
  MessageBufferHandle_t create() { return xMessageBufferCreate(N); }
};

// =============================================================================
// STREAM BUFFER COVERAGE TESTS
// =============================================================================

// --- SFINAE constructor with allocator args (lines 124-130) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_CtorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 4))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::stream_buffer<64, tag_stream_allocator<64>> sb(4, 42);
}

// --- static_stream_buffer_allocator::swap (lines 73-77) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_StaticAllocatorSwap) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle))
      .WillOnce(Return(sb_handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> sb1;
  freertos::sa::stream_buffer<64> sb2;
  sb1.swap(sb2);

  uint8_t data1[] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xStreamBufferSend(sb_handle2, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_EQ(sb1.send(data1, 4), 4u);
}

// --- move assignment operator (lines 149-153) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_MoveAssignmentDifferentObjects) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle))
      .WillOnce(Return(sb_handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle2));

  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb1;
  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb2;
  sb1 = std::move(sb2);

  EXPECT_EQ(sb1.handle(), sb_handle2);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_MoveAssignmentSelf) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  sb1 = std::move(sb1);
#pragma GCC diagnostic pop

  uint8_t data[4] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xStreamBufferSend(sb_handle, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_EQ(sb1.send(data, 4), 4u);
}

// --- swap method (lines 156-160) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_SwapMethod) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle))
      .WillOnce(Return(sb_handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle2));

  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb1;
  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb2;
  sb1.swap(sb2);

  EXPECT_EQ(sb1.handle(), sb_handle2);
  EXPECT_EQ(sb2.handle(), sb_handle);
}

// --- friend swap (line 162) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_FriendSwap) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle))
      .WillOnce(Return(sb_handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle2));

  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb1;
  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb2;
  swap(sb1, sb2);

  EXPECT_EQ(sb1.handle(), sb_handle2);
  EXPECT_EQ(sb2.handle(), sb_handle);
}

// --- send_isr body with higher_priority_task_woken (lines 238-242) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendIsr_WokenTrue) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *woken) { *woken = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_isr(data, 4);
  EXPECT_EQ(r.result, 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendIsr_WokenFalse) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_isr(data, 4);
  EXPECT_EQ(r.result, 0u);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendIsrIterators_WokenSet) {
  EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 5, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *woken) { *woken = pdTRUE; }),
                      Return(5)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<128> buf;
  std::string data = "hello";
  auto r = buf.send_isr(data.begin(), data.end());
  EXPECT_EQ(r.result, 5u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

// --- receive_isr body (lines 302-306) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_ReceiveIsr_WokenTrue) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *woken) { *woken = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_isr(data, 4);
  EXPECT_EQ(r.result, 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_ReceiveIsr_WokenFalseZeroResult) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_isr(data, 4);
  EXPECT_EQ(r.result, 0u);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

// --- reset() (line 330) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_ResetPdPass) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(sb_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  EXPECT_EQ(buf.reset(), pdPASS);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_ResetPdFail) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(sb_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  EXPECT_EQ(buf.reset(), pdFAIL);
}

// --- set_trigger_level() (lines 339-340) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_SetTriggerLevel_Pass) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(sb_handle, 8))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  EXPECT_EQ(buf.set_trigger_level(8), pdPASS);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SetTriggerLevel_Fail) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(sb_handle, 200))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  EXPECT_EQ(buf.set_trigger_level(200), pdFAIL);
}

// --- send_ex failure paths (lines 367-371) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendEx_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(sb_handle, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendEx_WouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(sb_handle, NotNull(), 4, 0))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, TickType_t(0));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendEx_Timeout) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(sb_handle, NotNull(), 4, 100))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, TickType_t(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendEx_ChronoTimeut) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(sb_handle, NotNull(), 4, pdMS_TO_TICKS(50)))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendEx_ChronoSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(sb_handle, NotNull(), 4, pdMS_TO_TICKS(50)))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, std::chrono::milliseconds(50));
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendEx_IteratorsTimeut) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(sb_handle, NotNull(), 3, 0))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  std::vector<uint8_t> data = {1, 2, 3};
  auto r = buf.send_ex(data.begin(), data.end(), TickType_t(0));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendEx_IteratorsChronoTimeut) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(sb_handle, NotNull(), 3, pdMS_TO_TICKS(100)))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  std::vector<uint8_t> data = {1, 2, 3};
  auto r = buf.send_ex(data.begin(), data.end(),
                       std::chrono::milliseconds(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// --- send_ex_isr body (lines 395-404) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendExIsr_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *woken) { *woken = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendExIsr_FailureWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SendExIsr_IteratorsSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 3, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *woken) { *woken = pdFALSE; }),
                      Return(3)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  std::vector<uint8_t> data = {1, 2, 3};
  auto r = buf.send_ex_isr(data.begin(), data.end());
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 3u);
}

// --- receive_ex failure paths (lines 411-417) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_ReceiveEx_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceive(sb_handle, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4);
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_ReceiveEx_WouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReceive(sb_handle, NotNull(), 4, 0))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, TickType_t(0));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_ReceiveEx_Timeout) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReceive(sb_handle, NotNull(), 4, 100))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, TickType_t(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_ReceiveEx_ChronoSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceive(sb_handle, NotNull(), 4, pdMS_TO_TICKS(50)))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, std::chrono::milliseconds(50));
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_ReceiveEx_ChronoTimeut) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceive(sb_handle, NotNull(), 4, pdMS_TO_TICKS(50)))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// --- receive_ex_isr body (lines 430-438) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_ReceiveExIsr_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *woken) { *woken = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_ReceiveExIsr_FailureWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

// --- reset_ex (lines 440-446) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_ResetEx_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(sb_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  auto r = buf.reset_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_ResetEx_Failure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(sb_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  auto r = buf.reset_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

// --- set_trigger_level_ex (lines 447-454) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_SetTriggerLevelEx_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(sb_handle, 4))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  auto r = buf.set_trigger_level_ex(4);
  EXPECT_TRUE(r.has_value());
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_SetTriggerLevelEx_Failure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(sb_handle, 100))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  auto r = buf.set_trigger_level_ex(100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_parameter);
}

// --- empty()/full() accessors (lines 354, 361) ---

TEST_F(StreamMessageCoverageTest, StreamBuffer_EmptyTrue) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(sb_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  EXPECT_EQ(buf.empty(), pdTRUE);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_EmptyFalse) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(sb_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  EXPECT_EQ(buf.empty(), pdFALSE);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_FullTrue) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferIsFull(sb_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  EXPECT_EQ(buf.full(), pdTRUE);
}

TEST_F(StreamMessageCoverageTest, StreamBuffer_FullFalse) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferIsFull(sb_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::da::stream_buffer<64> buf;
  EXPECT_EQ(buf.full(), pdFALSE);
}

// =============================================================================
// MESSAGE BUFFER COVERAGE TESTS
// =============================================================================

// --- SFINAE constructor with allocator args (lines 121-125) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_CtorWithAllocatorArgs) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::message_buffer<256, tag_message_allocator<256>> mb(42);
}

// --- move assignment (lines 144-148) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_MoveAssignmentDifferent) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle))
      .WillOnce(Return(mb_handle2));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle2));

  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>>
      mb1;
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>>
      mb2;
  mb1 = std::move(mb2);
}

// --- swap method (lines 151-155) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_SwapMethod) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle))
      .WillOnce(Return(mb_handle2));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle2));

  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>>
      mb1;
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>>
      mb2;
  mb1.swap(mb2);
}

// --- friend swap (line 157) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_FriendSwap) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle))
      .WillOnce(Return(mb_handle2));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle2));

  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>>
      mb1;
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>>
      mb2;
  swap(mb1, mb2);
}

// --- send chrono template (lines 183-191) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSend(mb_handle, NotNull(), 4, pdMS_TO_TICKS(100)))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send(data, 4, std::chrono::milliseconds(100));
  EXPECT_EQ(r, 4u);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendChronoZeroResult) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSend(mb_handle, NotNull(), 4, pdMS_TO_TICKS(50)))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send(data, 4, std::chrono::milliseconds(50));
  EXPECT_EQ(r, 0u);
}

// --- receive chrono template (lines 217-225) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceive(mb_handle, NotNull(), 4, pdMS_TO_TICKS(75)))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive(data, 4, std::chrono::milliseconds(75));
  EXPECT_EQ(r, 4u);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveChronoZeroResult) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceive(mb_handle, NotNull(), 4, pdMS_TO_TICKS(25)))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive(data, 4, std::chrono::milliseconds(25));
  EXPECT_EQ(r, 0u);
}

// --- reset() (line 274) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_ResetPdPass) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReset(mb_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  EXPECT_EQ(buf.reset(), pdPASS);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_ResetPdFail) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReset(mb_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  EXPECT_EQ(buf.reset(), pdFAIL);
}

// --- send_ex failure paths (lines 290-298) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendEx_Success) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle, NotNull(), 4, 10))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, TickType_t(10));
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendEx_WouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle, NotNull(), 4, 0))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, TickType_t(0));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendEx_Timeout) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle, NotNull(), 4, 100))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, TickType_t(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// --- send_ex with chrono (lines 301-308) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendEx_ChronoSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSend(mb_handle, NotNull(), 4, pdMS_TO_TICKS(100)))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendEx_ChronoTimeut) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSend(mb_handle, NotNull(), 4, pdMS_TO_TICKS(50)))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// --- send_ex_isr body (lines 310-320) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendExIsr_Success) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSendFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *woken) { *woken = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendExIsr_FailureWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSendFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

// --- receive_ex failure paths (lines 321-329) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveEx_Success) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReceive(mb_handle, NotNull(), 4, 10))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, TickType_t(10));
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveEx_WouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReceive(mb_handle, NotNull(), 4, 0))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, TickType_t(0));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveEx_Timeout) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReceive(mb_handle, NotNull(), 4, 100))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, TickType_t(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// --- receive_ex with chrono (lines 332-340) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveEx_ChronoSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceive(mb_handle, NotNull(), 4, pdMS_TO_TICKS(100)))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(r.has_value());
  EXPECT_EQ(r.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveEx_ChronoTimeut) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceive(mb_handle, NotNull(), 4, pdMS_TO_TICKS(50)))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, std::chrono::milliseconds(50));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// --- receive_ex_isr body (lines 341-350) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveExIsr_Success) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceiveFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *woken) { *woken = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 4u);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveExIsr_FailureWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceiveFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

// --- reset_ex (lines 352-358) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_ResetEx_Success) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReset(mb_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  auto r = buf.reset_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_ResetEx_Failure) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReset(mb_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  auto r = buf.reset_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

// --- empty()/full() accessors (lines 281, 288) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_EmptyTrue) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferIsEmpty(mb_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  EXPECT_EQ(buf.empty(), pdTRUE);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_EmptyFalse) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferIsEmpty(mb_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  EXPECT_EQ(buf.empty(), pdFALSE);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_FullTrue) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferIsFull(mb_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  EXPECT_EQ(buf.full(), pdTRUE);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_FullFalse) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferIsFull(mb_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  EXPECT_EQ(buf.full(), pdFALSE);
}

// --- send_isr with woken flag (lines 236-241) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendIsr_WokenTrue) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSendFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *woken) { *woken = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_isr(data, 4);
  EXPECT_EQ(r.result, 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_SendIsr_WokenFalseZeroResult) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSendFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_isr(data, 4);
  EXPECT_EQ(r.result, 0u);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

// --- receive_isr with woken flag (lines 252-257) ---

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveIsr_WokenTrue) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceiveFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *woken) { *woken = pdTRUE; }),
                      Return(4)));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_isr(data, 4);
  EXPECT_EQ(r.result, 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(StreamMessageCoverageTest, MessageBuffer_ReceiveIsr_WokenFalseZeroResult) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceiveFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::da::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_isr(data, 4);
  EXPECT_EQ(r.result, 0u);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

// --- static allocator swap for message buffer (lines 72-76) ---

#if configSUPPORT_STATIC_ALLOCATION
TEST_F(StreamMessageCoverageTest, MessageBuffer_StaticAllocatorSwap) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle))
      .WillOnce(Return(mb_handle2));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle2));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> mb1;
  freertos::sa::message_buffer<256> mb2;
  mb1.swap(mb2);

  uint8_t data1[] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle2, NotNull(), 4, _))
      .WillOnce(Return(4));
  EXPECT_EQ(mb1.send(data1, 4, 0), 4u);
}
#endif

// --- move construction edge case for stream buffer ---
TEST_F(StreamMessageCoverageTest,
       StreamBuffer_MoveConstructionTransfersHandle) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb1;
  auto saved_handle = sb1.handle();
  freertos::stream_buffer<64, freertos::dynamic_stream_buffer_allocator<64>>
      sb2(std::move(sb1));

  EXPECT_EQ(sb2.handle(), saved_handle);
}

// --- move construction edge case for message buffer ---
TEST_F(StreamMessageCoverageTest,
       MessageBuffer_MoveConstructionTransfersHandle) {
  EXPECT_CALL(*mock, xMessageBufferCreate(256))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>>
      mb1;
  freertos::message_buffer<256, freertos::dynamic_message_buffer_allocator<256>>
      mb2(std::move(mb1));

  uint8_t data[4] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle, NotNull(), 4, _))
      .WillOnce(Return(4));
  EXPECT_EQ(mb2.send(data, 4, 0), 4u);
}