#include <array>
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "mocks/freertos_mocks.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_message_buffer.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;
using ::testing::WithArg;

class SAStreamBufferTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = FreeRTOSMockInstance::getInstance();
    sb_handle = reinterpret_cast<StreamBufferHandle_t>(0x1000);
    sb_handle2 = reinterpret_cast<StreamBufferHandle_t>(0x2000);
  }

  void TearDown() override {
    FreeRTOSMockInstance::resetInstance();
    mock.reset();
  }

  std::shared_ptr<StrictMock<FreeRTOSMock>> mock;
  StreamBufferHandle_t sb_handle;
  StreamBufferHandle_t sb_handle2;
};

class SAMessageBufferTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = FreeRTOSMockInstance::getInstance();
    mb_handle = reinterpret_cast<MessageBufferHandle_t>(0x3000);
    mb_handle2 = reinterpret_cast<MessageBufferHandle_t>(0x4000);
  }

  void TearDown() override {
    FreeRTOSMockInstance::resetInstance();
    mock.reset();
  }

  std::shared_ptr<StrictMock<FreeRTOSMock>> mock;
  MessageBufferHandle_t mb_handle;
  MessageBufferHandle_t mb_handle2;
};

template <size_t N> class tag_sa_stream_allocator {
  int m_tag = 0;
  StaticStreamBuffer_t m_placeholder{};
  std::array<uint8_t, N> m_storage{};

public:
  tag_sa_stream_allocator() = default;
  explicit tag_sa_stream_allocator(int tag) : m_tag(tag) {}
  void swap(tag_sa_stream_allocator &other) noexcept {
    using std::swap;
    swap(m_tag, other.m_tag);
    swap(m_placeholder, other.m_placeholder);
    swap(m_storage, other.m_storage);
  }
  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    (void)m_tag;
    return xStreamBufferCreateStatic(N, trigger_level_bytes, m_storage.data(),
                                     &m_placeholder);
  }
};

template <size_t N> class tag_sa_message_allocator {
  int m_tag = 0;
  StaticMessageBuffer_t m_placeholder{};
  std::array<uint8_t, N> m_storage{};

public:
  tag_sa_message_allocator() = default;
  explicit tag_sa_message_allocator(int tag) : m_tag(tag) {}
  void swap(tag_sa_message_allocator &other) noexcept {
    using std::swap;
    swap(m_tag, other.m_tag);
    swap(m_placeholder, other.m_placeholder);
    swap(m_storage, other.m_storage);
  }
  MessageBufferHandle_t create() {
    (void)m_tag;
    return xMessageBufferCreateStatic(N, m_storage.data(), &m_placeholder);
  }
};

// =============================================================================
// freertos::sa::stream_buffer — SFINAE constructor with allocator args
// =============================================================================

TEST_F(SAStreamBufferTest, SFINAECtorWithAllocatorArg) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 4, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::stream_buffer<64, tag_sa_stream_allocator<64>> sb(4, 42);
}

// =============================================================================
// freertos::sa::stream_buffer — Move constructor
// =============================================================================

TEST_F(SAStreamBufferTest, MoveConstruction) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> sb1;
  auto saved = sb1.handle();
  freertos::sa::stream_buffer<64> sb2(std::move(sb1));
  EXPECT_EQ(sb2.handle(), saved);
}

// =============================================================================
// freertos::sa::stream_buffer — Move assignment
// =============================================================================

TEST_F(SAStreamBufferTest, MoveAssignmentDifferent) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle))
      .WillOnce(Return(sb_handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle2));

  freertos::sa::stream_buffer<64> sb1;
  freertos::sa::stream_buffer<64> sb2;
  sb1 = std::move(sb2);
  EXPECT_EQ(sb1.handle(), sb_handle2);
}

TEST_F(SAStreamBufferTest, MoveAssignmentSelf) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> sb1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  sb1 = std::move(sb1);
#pragma GCC diagnostic pop
  EXPECT_EQ(sb1.handle(), sb_handle);
}

// =============================================================================
// freertos::sa::stream_buffer — swap
// =============================================================================

TEST_F(SAStreamBufferTest, SwapMethod) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle))
      .WillOnce(Return(sb_handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle2));

  freertos::sa::stream_buffer<64> sb1;
  freertos::sa::stream_buffer<64> sb2;
  sb1.swap(sb2);
  EXPECT_EQ(sb1.handle(), sb_handle2);
  EXPECT_EQ(sb2.handle(), sb_handle);
}

TEST_F(SAStreamBufferTest, FriendSwap) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle))
      .WillOnce(Return(sb_handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle2));

  freertos::sa::stream_buffer<64> sb1;
  freertos::sa::stream_buffer<64> sb2;
  swap(sb1, sb2);
  EXPECT_EQ(sb1.handle(), sb_handle2);
  EXPECT_EQ(sb2.handle(), sb_handle);
}

// =============================================================================
// freertos::sa::stream_buffer — send_isr / receive_isr
// =============================================================================

TEST_F(SAStreamBufferTest, SendIsr_WokenTrue) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                       Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_isr(data, 4);
  EXPECT_EQ(r.result, 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAStreamBufferTest, SendIsr_WokenFalse) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_isr(data, 4);
  EXPECT_EQ(r.result, 0u);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

TEST_F(SAStreamBufferTest, ReceiveIsr_WokenTrue) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                       Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_isr(data, 4);
  EXPECT_EQ(r.result, 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAStreamBufferTest, ReceiveIsr_WokenFalse) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_isr(data, 4);
  EXPECT_EQ(r.result, 0u);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

// =============================================================================
// freertos::sa::stream_buffer — send_ex failure paths
// =============================================================================

TEST_F(SAStreamBufferTest, SendEx_WouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(sb_handle, NotNull(), 4, 0))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, TickType_t(0));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(SAStreamBufferTest, SendEx_Timeout) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(sb_handle, NotNull(), 4, 100))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, TickType_t(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// =============================================================================
// freertos::sa::stream_buffer — receive_ex failure paths
// =============================================================================

TEST_F(SAStreamBufferTest, ReceiveEx_WouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReceive(sb_handle, NotNull(), 4, 0))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, TickType_t(0));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(SAStreamBufferTest, ReceiveEx_Timeout) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReceive(sb_handle, NotNull(), 4, 100))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, TickType_t(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// =============================================================================
// freertos::sa::stream_buffer — send_ex_isr success and failure
// =============================================================================

TEST_F(SAStreamBufferTest, SendExIsr_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                       Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAStreamBufferTest, SendExIsr_FailureWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

// =============================================================================
// freertos::sa::stream_buffer — receive_ex_isr success and failure
// =============================================================================

TEST_F(SAStreamBufferTest, ReceiveExIsr_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                       Return(4)));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAStreamBufferTest, ReceiveExIsr_FailureWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceiveFromISR(sb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

// =============================================================================
// freertos::sa::stream_buffer — reset_ex success and failure
// =============================================================================

TEST_F(SAStreamBufferTest, ResetEx_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(sb_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  auto r = buf.reset_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SAStreamBufferTest, ResetEx_Failure) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(sb_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  auto r = buf.reset_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}

// =============================================================================
// freertos::sa::stream_buffer — set_trigger_level_ex success and failure
// =============================================================================

TEST_F(SAStreamBufferTest, SetTriggerLevelEx_Success) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(sb_handle, 4))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  auto r = buf.set_trigger_level_ex(4);
  EXPECT_TRUE(r.has_value());
}

TEST_F(SAStreamBufferTest, SetTriggerLevelEx_Failure) {
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
      .WillOnce(Return(sb_handle));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(sb_handle, 100))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(sb_handle));

  freertos::sa::stream_buffer<64> buf;
  auto r = buf.set_trigger_level_ex(100);
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_parameter);
}

// =============================================================================
// freertos::sa::message_buffer — SFINAE constructor with allocator args
// =============================================================================

TEST_F(SAMessageBufferTest, SFINAECtorWithAllocatorArg) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::message_buffer<256, tag_sa_message_allocator<256>> mb(42);
}

// =============================================================================
// freertos::sa::message_buffer — Move constructor
// =============================================================================

TEST_F(SAMessageBufferTest, MoveConstruction) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle, NotNull(), 4, _))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> mb1;
  freertos::sa::message_buffer<256> mb2(std::move(mb1));
  uint8_t data[4] = {1, 2, 3, 4};
  EXPECT_EQ(mb2.send(data, 4, 0), 4u);
}

// =============================================================================
// freertos::sa::message_buffer — Move assignment
// =============================================================================

TEST_F(SAMessageBufferTest, MoveAssignmentDifferent) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle))
      .WillOnce(Return(mb_handle2));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle2));

  freertos::sa::message_buffer<256> mb1;
  freertos::sa::message_buffer<256> mb2;
  mb1 = std::move(mb2);
  uint8_t data[4] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle2, NotNull(), 4, _))
      .WillOnce(Return(4));
  EXPECT_EQ(mb1.send(data, 4, 0), 4u);
}

TEST_F(SAMessageBufferTest, MoveAssignmentSelf) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> mb1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
  mb1 = std::move(mb1);
#pragma GCC diagnostic pop
  uint8_t data[4] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle, NotNull(), 4, _))
      .WillOnce(Return(4));
  EXPECT_EQ(mb1.send(data, 4, 0), 4u);
}

// =============================================================================
// freertos::sa::message_buffer — swap
// =============================================================================

TEST_F(SAMessageBufferTest, SwapMethod) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle))
      .WillOnce(Return(mb_handle2));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle2));

  freertos::sa::message_buffer<256> mb1;
  freertos::sa::message_buffer<256> mb2;
  mb1.swap(mb2);
  uint8_t data[4] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle2, NotNull(), 4, _))
      .WillOnce(Return(4));
  EXPECT_EQ(mb1.send(data, 4, 0), 4u);
}

TEST_F(SAMessageBufferTest, FriendSwap) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle))
      .WillOnce(Return(mb_handle2));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle2));

  freertos::sa::message_buffer<256> mb1;
  freertos::sa::message_buffer<256> mb2;
  swap(mb1, mb2);
  uint8_t data[4] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle2, NotNull(), 4, _))
      .WillOnce(Return(4));
  EXPECT_EQ(mb1.send(data, 4, 0), 4u);
}

// =============================================================================
// freertos::sa::message_buffer — send_isr / receive_isr
// =============================================================================

TEST_F(SAMessageBufferTest, SendIsr_WokenTrue) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSendFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                       Return(4)));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_isr(data, 4);
  EXPECT_EQ(r.result, 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAMessageBufferTest, SendIsr_WokenFalse) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSendFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_isr(data, 4);
  EXPECT_EQ(r.result, 0u);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

TEST_F(SAMessageBufferTest, ReceiveIsr_WokenTrue) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceiveFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                       Return(4)));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_isr(data, 4);
  EXPECT_EQ(r.result, 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAMessageBufferTest, ReceiveIsr_WokenFalse) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceiveFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_isr(data, 4);
  EXPECT_EQ(r.result, 0u);
  EXPECT_EQ(r.higher_priority_task_woken, pdFALSE);
}

// =============================================================================
// freertos::sa::message_buffer — send_ex failure paths
// =============================================================================

TEST_F(SAMessageBufferTest, SendEx_WouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle, NotNull(), 4, 0))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, TickType_t(0));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(SAMessageBufferTest, SendEx_Timeout) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferSend(mb_handle, NotNull(), 4, 100))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex(data, 4, TickType_t(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// =============================================================================
// freertos::sa::message_buffer — receive_ex failure paths
// =============================================================================

TEST_F(SAMessageBufferTest, ReceiveEx_WouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReceive(mb_handle, NotNull(), 4, 0))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, TickType_t(0));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::would_block);
}

TEST_F(SAMessageBufferTest, ReceiveEx_Timeout) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReceive(mb_handle, NotNull(), 4, 100))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex(data, 4, TickType_t(100));
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::timeout);
}

// =============================================================================
// freertos::sa::message_buffer — send_ex_isr success and failure
// =============================================================================

TEST_F(SAMessageBufferTest, SendExIsr_Success) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSendFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                       Return(4)));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAMessageBufferTest, SendExIsr_FailureWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferSendFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4] = {1, 2, 3, 4};
  auto r = buf.send_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

// =============================================================================
// freertos::sa::message_buffer — receive_ex_isr success and failure
// =============================================================================

TEST_F(SAMessageBufferTest, ReceiveExIsr_Success) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceiveFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(DoAll(WithArg<3>([](BaseType_t *w) { *w = pdTRUE; }),
                       Return(4)));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_TRUE(r.result.has_value());
  EXPECT_EQ(r.result.value(), 4u);
  EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST_F(SAMessageBufferTest, ReceiveExIsr_FailureWouldBlock) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock,
              xMessageBufferReceiveFromISR(mb_handle, NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  uint8_t data[4];
  auto r = buf.receive_ex_isr(data, 4);
  EXPECT_FALSE(r.result.has_value());
  EXPECT_EQ(r.result.error(), freertos::error::would_block);
}

// =============================================================================
// freertos::sa::message_buffer — reset_ex success and failure
// =============================================================================

TEST_F(SAMessageBufferTest, ResetEx_Success) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReset(mb_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  auto r = buf.reset_ex();
  EXPECT_TRUE(r.has_value());
}

TEST_F(SAMessageBufferTest, ResetEx_Failure) {
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, NotNull(), NotNull()))
      .WillOnce(Return(mb_handle));
  EXPECT_CALL(*mock, xMessageBufferReset(mb_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vMessageBufferDelete(mb_handle));

  freertos::sa::message_buffer<256> buf;
  auto r = buf.reset_ex();
  EXPECT_FALSE(r.has_value());
  EXPECT_EQ(r.error(), freertos::error::invalid_handle);
}