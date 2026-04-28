#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_event_group.hpp"
#include "freertos_expected.hpp"
#include "freertos_external_allocator.hpp"
#include "freertos_isr_result.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_queue.hpp"
#include "freertos_queue_set.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_task.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

class ExpectedCoverageTest : public ::testing::Test {
protected:
};

TEST_F(ExpectedCoverageTest, VoidExpectedMoveConstruction) {
  freertos::expected<void, freertos::error> e1;
  freertos::expected<void, freertos::error> e2(std::move(e1));
  EXPECT_TRUE(e2.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedMoveConstructionFromError) {
  auto ue = freertos::unexpected<freertos::error>(freertos::error::timeout);
  freertos::expected<void, freertos::error> e2{std::move(ue)};
  EXPECT_FALSE(e2.has_value());
  EXPECT_EQ(e2.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedCopyAssignment) {
  freertos::expected<void, freertos::error> e1;
  freertos::expected<void, freertos::error> e2{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  e2 = e1;
  EXPECT_TRUE(e2.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedMoveAssignment) {
  freertos::expected<void, freertos::error> e1;
  freertos::expected<void, freertos::error> e2{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  e2 = std::move(e1);
  EXPECT_TRUE(e2.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedOperatorBool) {
  freertos::expected<void, freertos::error> e1;
  EXPECT_TRUE(static_cast<bool>(e1));
  freertos::expected<void, freertos::error> e2{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  EXPECT_FALSE(static_cast<bool>(e2));
}

TEST_F(ExpectedCoverageTest, VoidExpectedAndThenValue) {
  freertos::expected<void, freertos::error> e1;
  auto result = e1.and_then([]() { return freertos::expected<int, freertos::error>(42); });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedCoverageTest, VoidExpectedAndThenError) {
  freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.and_then([]() { return freertos::expected<int, freertos::error>(42); });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedOrElseValue) {
  freertos::expected<void, freertos::error> e1;
  auto result = e1.or_else([](freertos::error &) {
    return freertos::expected<void, freertos::error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedOrElseError) {
  freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.or_else([](freertos::error &err) {
    return freertos::expected<void, freertos::error>{freertos::unexpected<freertos::error>(err)};
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformValue) {
  freertos::expected<void, freertos::error> e1;
  auto result = e1.transform([]() { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformError) {
  freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.transform([]() { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, ExpectedRvalueAndThen) {
  freertos::expected<int, freertos::error> e1(42);
  auto result = std::move(e1).and_then([](int v) {
    return freertos::expected<int, freertos::error>(v * 2);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 84);
}

TEST_F(ExpectedCoverageTest, ExpectedRvalueOrElse) {
  freertos::expected<int, freertos::error> e1(42);
  auto result = e1.or_else([](freertos::error &) {
    return freertos::expected<int, freertos::error>(0);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedCoverageTest, ExpectedSelfAssignment) {
  freertos::expected<int, freertos::error> e1(42);
  e1 = e1;
  EXPECT_TRUE(e1.has_value());
  EXPECT_EQ(e1.value(), 42);
}

class StreamBufferCoverageTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_sb = reinterpret_cast<StreamBufferHandle_t>(0x66666666);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t mock_sb;
};

TEST_F(StreamBufferCoverageTest, StreamBufferSendExTimeout) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_sb, _, _, 100))
      .WillOnce(Return(size_t{0}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = sb.send_ex(data, 4, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(StreamBufferCoverageTest, StreamBufferSendExChrono) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_sb, _, _, _))
      .WillOnce(Return(size_t{4}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = sb.send_ex(data, 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), size_t{4});
}

TEST_F(StreamBufferCoverageTest, StreamBufferSendExIterator) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_sb, _, 4, _))
      .WillOnce(Return(size_t{4}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = sb.send_ex(data, data + 4, TickType_t(0));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), size_t{4});
}

TEST_F(StreamBufferCoverageTest, StreamBufferSendExIsrSuccess) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_sb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(size_t{4})));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = sb.send_ex_isr(data, 4);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), size_t{4});
}

TEST_F(StreamBufferCoverageTest, StreamBufferSendExIsrFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_sb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(size_t{0})));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = sb.send_ex_isr(data, 4);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

TEST_F(StreamBufferCoverageTest, StreamBufferReceiveExTimeout) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(mock_sb, _, 4, 100))
      .WillOnce(Return(size_t{0}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.receive_ex(buf, 4, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(StreamBufferCoverageTest, StreamBufferReceiveExChrono) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(mock_sb, _, 4, _))
      .WillOnce(Return(size_t{4}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.receive_ex(buf, 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), size_t{4});
}

TEST_F(StreamBufferCoverageTest, StreamBufferReceiveExIsrSuccess) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(mock_sb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(size_t{4})));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.receive_ex_isr(buf, 4);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), size_t{4});
}

TEST_F(StreamBufferCoverageTest, StreamBufferReceiveExIsrFailure) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(mock_sb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(size_t{0})));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.receive_ex_isr(buf, 4);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

TEST_F(StreamBufferCoverageTest, StreamBufferResetExFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferReset(mock_sb)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.reset_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(StreamBufferCoverageTest, StreamBufferSetTriggerLevelExFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(mock_sb, 10))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.set_trigger_level_ex(10);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_parameter);
}

class MessageBufferCoverageTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_mb = reinterpret_cast<MessageBufferHandle_t>(0x77777777);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  MessageBufferHandle_t mock_mb;
};

TEST_F(MessageBufferCoverageTest, MessageBufferSendExTimeout) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(mock_mb, _, _, 100))
      .WillOnce(Return(size_t{0}));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = mb.send_ex(data, 4, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(MessageBufferCoverageTest, MessageBufferSendExChrono) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferSend(mock_mb, _, _, _))
      .WillOnce(Return(size_t{4}));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = mb.send_ex(data, 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), size_t{4});
}

TEST_F(MessageBufferCoverageTest, MessageBufferSendExIsrSuccess) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(mock_mb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(size_t{4})));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = mb.send_ex_isr(data, 4);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), size_t{4});
}

TEST_F(MessageBufferCoverageTest, MessageBufferSendExIsrFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferSendFromISR(mock_mb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(size_t{0})));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = mb.send_ex_isr(data, 4);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

TEST_F(MessageBufferCoverageTest, MessageBufferReceiveExWouldBlock) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(mock_mb, _, 4, 0))
      .WillOnce(Return(size_t{0}));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  auto result = mb.receive_ex(buf, 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(MessageBufferCoverageTest, MessageBufferReceiveExTimeout) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(mock_mb, _, 4, 100))
      .WillOnce(Return(size_t{0}));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  auto result = mb.receive_ex(buf, 4, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(MessageBufferCoverageTest, MessageBufferReceiveExChrono) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferReceive(mock_mb, _, 4, _))
      .WillOnce(Return(size_t{4}));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  auto result = mb.receive_ex(buf, 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), size_t{4});
}

TEST_F(MessageBufferCoverageTest, MessageBufferReceiveExIsrSuccess) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(mock_mb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(size_t{4})));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  auto result = mb.receive_ex_isr(buf, 4);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), size_t{4});
}

TEST_F(MessageBufferCoverageTest, MessageBufferReceiveExIsrFailure) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferReceiveFromISR(mock_mb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(size_t{0})));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  auto result = mb.receive_ex_isr(buf, 4);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

TEST_F(MessageBufferCoverageTest, MessageBufferResetExFailure) {
  EXPECT_CALL(*mock, xMessageBufferCreate(128)).WillOnce(Return(mock_mb));
  EXPECT_CALL(*mock, xMessageBufferReset(mock_mb)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vMessageBufferDelete(mock_mb));

  freertos::da::message_buffer<128> mb;
  auto result = mb.reset_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

class EventGroupCoverageTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_eg = reinterpret_cast<EventGroupHandle_t>(0x55555555);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  EventGroupHandle_t mock_eg;
};

TEST_F(EventGroupCoverageTest, EventGroupSetBitsExIsr) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(mock_eg, 0x01, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(EventBits_t{0x01})));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.set_bits_ex_isr(0x01);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), EventBits_t{0x01});
}

TEST_F(EventGroupCoverageTest, EventGroupWaitBitsExWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(mock_eg, 0x03, _, _, 0))
      .WillOnce(Return(EventBits_t{0x00}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x03, pdTRUE, pdTRUE, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(EventGroupCoverageTest, EventGroupWaitBitsExChrono) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(mock_eg, 0x03, _, _, _))
      .WillOnce(Return(EventBits_t{0x03}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result =
      eg.wait_bits_ex(0x03, pdTRUE, pdTRUE, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), EventBits_t{0x03});
}

TEST_F(EventGroupCoverageTest, EventGroupSyncExWouldBlock) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupSync(mock_eg, 0x01, 0x03, 0))
      .WillOnce(Return(EventBits_t{0x01}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(EventGroupCoverageTest, EventGroupSyncExChrono) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupSync(mock_eg, 0x01, 0x03, _))
      .WillOnce(Return(EventBits_t{0x03}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), EventBits_t{0x03});
}

class QueueCoverageTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_queue = reinterpret_cast<QueueHandle_t>(0x33333333);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  QueueHandle_t mock_queue;
};

TEST_F(QueueCoverageTest, QueueSendBackExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBack(mock_queue, _, 100))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex(42, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueCoverageTest, QueueSendBackExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBack(mock_queue, _, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex(42, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendBackExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendBackExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::queue_full);
}

TEST_F(QueueCoverageTest, QueueSendFrontExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFront(mock_queue, _, 0))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(QueueCoverageTest, QueueSendFrontExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFront(mock_queue, _, 100))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex(42, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueCoverageTest, QueueSendFrontExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFront(mock_queue, _, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex(42, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendFrontExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendFrontExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::queue_full);
}

TEST_F(QueueCoverageTest, QueueReceiveExWithExpectedType) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 0)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(0);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueReceiveExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto result = q.receive_ex(val, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueResetExFailure) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReset(mock_queue)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.reset_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(QueueCoverageTest, QueueOverwriteExFailure) {
  EXPECT_CALL(*mock, xQueueCreate(1, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueOverwrite(mock_queue, _)).WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<1, int> q;
  auto result = q.overwrite_ex(42);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(QueueCoverageTest, QueuePeekExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueuePeek(mock_queue, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto result = q.peek_ex(val, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(QueueCoverageTest, QueuePeekExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueuePeek(mock_queue, _, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto result = q.peek_ex(val, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueCoverageTest, QueuePeekExChrono) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueuePeek(mock_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto result = q.peek_ex(val, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

class TimerCoverageTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_timer = reinterpret_cast<TimerHandle_t>(0x44444444);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  TimerHandle_t mock_timer;
};

TEST_F(TimerCoverageTest, TimerStartExChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.start_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerCoverageTest, TimerStopExChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  t.start();
  auto result = t.stop_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerCoverageTest, TimerResetExChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerReset(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.reset_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerCoverageTest, TimerPeriodExChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(mock_timer, _, _))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.period_ex(std::chrono::milliseconds(200),
                             std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(TimerCoverageTest, TimerStartExIsr) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(mock_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.start_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerCoverageTest, TimerStopExIsr) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStopFromISR(mock_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  t.start();
  auto result = t.stop_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerCoverageTest, TimerResetExIsr) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerResetFromISR(mock_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.reset_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerCoverageTest, TimerPeriodExIsr) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(mock_timer, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.period_ex_isr(200);
  EXPECT_TRUE(result.result.has_value());
}

class QueueSetCoverageTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_queue_set_handle = reinterpret_cast<QueueSetHandle_t>(0x12345678);
    mock_member_handle =
        reinterpret_cast<QueueSetMemberHandle_t>(0xAAAABBBB);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  QueueSetHandle_t mock_queue_set_handle;
  QueueSetMemberHandle_t mock_member_handle;
};

TEST_F(QueueSetCoverageTest, QueueSetAddEx) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock,
              xQueueAddToSet(mock_member_handle, mock_queue_set_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(3);
  auto result = qs.add_ex(mock_member_handle);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueSetCoverageTest, QueueSetRemoveEx) {
  EXPECT_CALL(*mock, xQueueCreateSet(3))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(
      *mock, xQueueRemoveFromSet(mock_member_handle, mock_queue_set_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(3);
  auto result = qs.remove_ex(mock_member_handle);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueSetCoverageTest, QueueSetSelectExChrono) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSet(mock_queue_set_handle, _))
      .WillOnce(Return(mock_member_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), mock_member_handle);
}

TEST_F(QueueSetCoverageTest, QueueSetSelectExIsr) {
  EXPECT_CALL(*mock, xQueueCreateSet(5))
      .WillOnce(Return(mock_queue_set_handle));
  EXPECT_CALL(*mock, xQueueSelectFromSetFromISR(mock_queue_set_handle, _))
      .WillOnce(Return(mock_member_handle));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue_set_handle));

  freertos::da::queue_set qs(5);
  auto result = qs.select_ex_isr();
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), mock_member_handle);
}

class ExternalAllocatorCoverageTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    region.allocate = test_allocate;
    region.deallocate = test_deallocate;
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  freertos::external_memory_region region;

  static void *test_allocate(size_t size) { return malloc(size); }
  static void test_deallocate(void *ptr) { free(ptr); }
};

TEST_F(ExternalAllocatorCoverageTest, ExternalBinarySemaphoreCreateDestroy) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xDEADBEEF);
  EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(_)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle));

  freertos::binary_semaphore<freertos::external_semaphore_allocator<
      freertos::external_memory_region>>
      sem(region);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalMutexCreateDestroy) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xBEEFCAFE);
  EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(_)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle));

  freertos::mutex<freertos::external_semaphore_allocator<
      freertos::external_memory_region>>
      mtx(region);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalQueueCreateDestroy) {
  QueueHandle_t handle = reinterpret_cast<QueueHandle_t>(0x11111111);
  EXPECT_CALL(*mock, xQueueCreateStatic(5, sizeof(int), _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, pcQueueGetName(handle)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(handle));

  freertos::queue<5, int,
                  freertos::external_queue_allocator<
                      freertos::external_memory_region, 5, int>>
      q{nullptr, region};
}

TEST_F(ExternalAllocatorCoverageTest, ExternalCountingSemaphoreCreateDestroy) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xCAFE0001);
  EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle));

  freertos::counting_semaphore<freertos::external_semaphore_allocator<
      freertos::external_memory_region>>
      sem(1, region);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalRecursiveMutexCreateDestroy) {
  SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(0xCAFE0002);
  EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(_))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vSemaphoreDelete(handle));

  freertos::recursive_mutex<freertos::external_semaphore_allocator<
      freertos::external_memory_region>>
      mtx(region);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalEventGroupCreateDestroy) {
  EventGroupHandle_t handle = reinterpret_cast<EventGroupHandle_t>(0xCAFE0003);
  EXPECT_CALL(*mock, xEventGroupCreateStatic(_)).WillOnce(Return(handle));
  EXPECT_CALL(*mock, vEventGroupDelete(handle));

  freertos::event_group<freertos::external_event_group_allocator<
      freertos::external_memory_region>>
      eg(region);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalStreamBufferCreateDestroy) {
  StreamBufferHandle_t handle =
      reinterpret_cast<StreamBufferHandle_t>(0xCAFE0004);
  EXPECT_CALL(*mock, xStreamBufferCreateStatic(_, _, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle));

  freertos::stream_buffer<64, freertos::external_stream_buffer_allocator<
                                  freertos::external_memory_region, 64>>
      sb(1, region);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalMessageBufferCreateDestroy) {
  MessageBufferHandle_t handle =
      reinterpret_cast<MessageBufferHandle_t>(0xCAFE0005);
  EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vMessageBufferDelete(handle));

  freertos::message_buffer<128, freertos::external_message_buffer_allocator<
                                      freertos::external_memory_region, 128>>
      mb(region);
}

TEST_F(ExternalAllocatorCoverageTest, ExternalTimerCreateDestroy) {
  TimerHandle_t handle = reinterpret_cast<TimerHandle_t>(0xCAFE0006);
  EXPECT_CALL(*mock, xTimerCreateStatic(_, _, _, _, _, _))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, xTimerDelete(handle, portMAX_DELAY));

  freertos::timer<freertos::external_sw_timer_allocator<
      freertos::external_memory_region>>
      t("ExtTimer", 100, pdTRUE, []() {}, region);
}

TEST_F(ExpectedCoverageTest, ExpectedCopyAssignmentValueToError) {
  freertos::expected<int, freertos::error> e1(42);
  freertos::expected<int, freertos::error> e2{
      freertos::unexpected<freertos::error>(freertos::error::timeout)};
  e2 = e1;
  EXPECT_TRUE(e2.has_value());
  EXPECT_EQ(e2.value(), 42);
}

TEST_F(ExpectedCoverageTest, ExpectedCopyAssignmentErrorToValue) {
  freertos::expected<int, freertos::error> e1{
      freertos::unexpected<freertos::error>(freertos::error::timeout)};
  freertos::expected<int, freertos::error> e2(42);
  e2 = e1;
  EXPECT_FALSE(e2.has_value());
  EXPECT_EQ(e2.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedCopyAssignmentErrorToValue) {
  freertos::expected<void, freertos::error> e1{
      freertos::unexpected<freertos::error>(freertos::error::timeout)};
  freertos::expected<void, freertos::error> e2;
  e2 = e1;
  EXPECT_FALSE(e2.has_value());
  EXPECT_EQ(e2.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedCopyAssignmentValueToError) {
  freertos::expected<void, freertos::error> e1;
  freertos::expected<void, freertos::error> e2{
      freertos::unexpected<freertos::error>(freertos::error::timeout)};
  e2 = e1;
  EXPECT_TRUE(e2.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedAndThenConstRefValue) {
  const freertos::expected<void, freertos::error> e1;
  auto result = e1.and_then([]() {
    return freertos::expected<int, freertos::error>(42);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedCoverageTest, VoidExpectedAndThenConstRefError) {
  const freertos::expected<void, freertos::error> e1{
      freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.and_then([]() {
    return freertos::expected<int, freertos::error>(42);
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedOrElseConstRefValue) {
  const freertos::expected<void, freertos::error> e1;
  auto result = e1.or_else([](const freertos::error &) {
    return freertos::expected<void, freertos::error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedOrElseConstRefError) {
  const freertos::expected<void, freertos::error> e1{
      freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.or_else([](const freertos::error &err) {
    return freertos::expected<void, freertos::error>(
        freertos::unexpected<freertos::error>(err));
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformConstRefValue) {
  const freertos::expected<void, freertos::error> e1;
  auto result = e1.transform([]() { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformConstRefError) {
  const freertos::expected<void, freertos::error> e1{
      freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.transform([]() { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformErrorConstRefError) {
  const freertos::expected<void, freertos::error> e1{
      freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.transform_error([](const freertos::error &) {
    return freertos::error::would_block;
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformErrorConstRefValue) {
  const freertos::expected<void, freertos::error> e1;
  auto result = e1.transform_error([](const freertos::error &) {
    return freertos::error::would_block;
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueReceiveExWithExpectedTypeWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(QueueCoverageTest, QueueReceiveExWithExpectedTypeTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 100))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueCoverageTest, QueueReceiveExWithExpectedTypeChrono) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_ex(std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueReceiveExRefWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 0)).WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto result = q.receive_ex(val, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_empty);
}

TEST_F(QueueCoverageTest, QueueSendBackExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBack(mock_queue, _, 0))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(QueueCoverageTest, QueueSendExWouldBlock) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSend(mock_queue, _, 0))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::queue_full);
}

TEST_F(QueueCoverageTest, QueueSendExTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSend(mock_queue, _, 100))
      .WillOnce(Return(errQUEUE_FULL));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueCoverageTest, QueueReceiveIsrWithValue) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto result = q.receive_isr(val);
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(QueueCoverageTest, QueuePeekExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueuePeekFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  int val;
  auto result = q.peek_isr(val);
  EXPECT_EQ(result.result, pdPASS);
}

TEST_F(QueueCoverageTest, QueuePeekExIsrFailureVoidReturn) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueuePeekFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.peek_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueuePeekExIsrSuccessVoidReturn) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueuePeekFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.peek_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendExIsrSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendExIsrFailure) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::queue_full);
}

TEST_F(QueueCoverageTest, QueueSendBackExIsrExpectedSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendBackExIsrExpectedFailure) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::queue_full);
}

TEST_F(QueueCoverageTest, QueueReceiveExIsrVoidSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueueReceiveExIsrVoidFailure) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceiveFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdFALSE)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.receive_isr();
  EXPECT_FALSE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueueNullHandleDestructorSkip) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(nullptr));

  freertos::da::queue<10, int> q;
}

TEST_F(QueueCoverageTest, QueueRegisteredName) {
  EXPECT_CALL(*mock, xQueueCreate(10, sizeof(int)))
      .WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, vQueueAddToRegistry(mock_queue, "MyQ"));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return("MyQ"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(mock_queue));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q("MyQ");
}

TEST_F(TimerCoverageTest, TimerStartExFailureNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.start_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerStopExFailureNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.stop_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerResetExNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.reset_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerPeriodExNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.period_ex(200, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerStartExIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.start_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerStopExIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.stop_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerResetExIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.reset_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerPeriodExIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.period_ex_isr(200);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerStartExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStartFromISR(mock_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.start_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerStopExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStopFromISR(mock_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  t.start();
  auto result = t.stop_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerResetExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerResetFromISR(mock_timer, _))
      .WillOnce(DoAll(SetArgPointee<1>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.reset_ex_isr();
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerPeriodExIsrFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(mock_timer, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdFAIL)));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.period_ex_isr(200);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::invalid_handle);
}

TEST_F(TimerCoverageTest, TimerRunningNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  EXPECT_EQ(t.running(), pdFALSE);
}

TEST_F(TimerCoverageTest, TimerPeriodTicksNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period_ticks(), TickType_t(0));
}

TEST_F(TimerCoverageTest, TimerRemainingTicksNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  EXPECT_EQ(t.remaining_ticks(), TickType_t(0));
}

TEST_F(TimerCoverageTest, TimerNameNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  EXPECT_EQ(t.name(), nullptr);
}

TEST_F(TimerCoverageTest, TimerStartExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.start_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timer_queue_full);
}

TEST_F(TimerCoverageTest, TimerStopExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerStart(mock_timer, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, xTimerStop(mock_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  t.start();
  auto result = t.stop_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timer_queue_full);
}

TEST_F(TimerCoverageTest, TimerResetExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerReset(mock_timer, _)).WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.reset_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timer_queue_full);
}

TEST_F(TimerCoverageTest, TimerPeriodExFailure) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerChangePeriod(mock_timer, _, _))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.period_ex(200, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timer_queue_full);
}

TEST_F(TimerCoverageTest, TimerDestructorNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
}

TEST_F(TimerCoverageTest, TimerPeriodExIsrChrono) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _)).WillOnce(Return(mock_timer));
  EXPECT_CALL(*mock, xTimerChangePeriodFromISR(mock_timer, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, xTimerDelete(mock_timer, portMAX_DELAY));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.period_ex_isr(std::chrono::milliseconds(200));
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(TimerCoverageTest, TimerStartNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  EXPECT_EQ(t.start(), pdFAIL);
}

TEST_F(TimerCoverageTest, TimerStopNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  EXPECT_EQ(t.stop(), pdFAIL);
}

TEST_F(TimerCoverageTest, TimerResetNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  EXPECT_EQ(t.reset(), pdFAIL);
}

TEST_F(TimerCoverageTest, TimerPeriodNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  EXPECT_EQ(t.period(200), pdFAIL);
}

TEST_F(TimerCoverageTest, TimerStartIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.start_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(TimerCoverageTest, TimerStopIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.stop_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(TimerCoverageTest, TimerResetIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.reset_isr();
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(TimerCoverageTest, TimerPeriodIsrNullHandle) {
  EXPECT_CALL(*mock, xTimerCreate(_, _, _, _, _))
      .WillOnce(Return(nullptr));

  freertos::da::timer t("Test", 100, pdTRUE, []() {});
  auto result = t.period_isr(200);
  EXPECT_EQ(result.result, pdFAIL);
}

TEST_F(StreamBufferCoverageTest, StreamBufferSendExWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_sb, _, 4, 0))
      .WillOnce(Return(size_t{0}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result = sb.send_ex(data, 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(StreamBufferCoverageTest, StreamBufferReceiveExWouldBlock) {
  uint8_t buf[4];
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferReceive(mock_sb, _, 4, 0))
      .WillOnce(Return(size_t{0}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.receive_ex(buf, 4, 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(StreamBufferCoverageTest, StreamBufferSendExIteratorChrono) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_sb, _, 4, _))
      .WillOnce(Return(size_t{4}));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  uint8_t data[] = {1, 2, 3, 4};
  auto result =
      sb.send_ex(data, data + 4, std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), size_t{4});
}

TEST_F(EventGroupCoverageTest, EventGroupWaitBitsExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(mock_eg, 0x03, _, _, 100))
      .WillOnce(Return(EventBits_t{0x00}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x03, pdTRUE, pdTRUE, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(EventGroupCoverageTest, EventGroupSyncExTimeout) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupSync(mock_eg, 0x01, 0x03, 100))
      .WillOnce(Return(EventBits_t{0x01}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.sync_ex(0x01, 0x03, 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(EventGroupCoverageTest, EventGroupSetBitsEx) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupSetBits(mock_eg, 0x01))
      .WillOnce(Return(EventBits_t{0x01}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.set_bits_ex(0x01);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), EventBits_t{0x01});
}

TEST_F(EventGroupCoverageTest, EventGroupClearBitsEx) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupClearBits(mock_eg, 0x01))
      .WillOnce(Return(EventBits_t{0x00}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.clear_bits_ex(0x01);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), EventBits_t{0x00});
}

TEST_F(EventGroupCoverageTest, EventGroupWaitBitsExSuccess) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(mock_eg));
  EXPECT_CALL(*mock, xEventGroupWaitBits(mock_eg, 0x03, _, _, 0))
      .WillOnce(Return(EventBits_t{0x03}));
  EXPECT_CALL(*mock, vEventGroupDelete(mock_eg));

  freertos::da::event_group eg;
  auto result = eg.wait_bits_ex(0x03, pdTRUE, pdTRUE, 0);
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), EventBits_t{0x03});
}

TEST_F(EventGroupCoverageTest, EventGroupNullHandleDestructorSkip) {
  EXPECT_CALL(*mock, xEventGroupCreate()).WillOnce(Return(nullptr));

  freertos::da::event_group eg;
}

class TaskCoverageTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;
    mock_task = reinterpret_cast<TaskHandle_t>(0x22222222);
  }
  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }
  StrictMock<FreeRTOSMock> *mock;
  TaskHandle_t mock_task;
};

TEST_F(TaskCoverageTest, TaskStdStringConstructor) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(mock_task), Return(pdPASS)));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  freertos::da::task<512> t(std::string("TestTask"), 1, []() {});
}

TEST_F(TaskCoverageTest, PeriodicTaskStdStringConstructor) {
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(mock_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));

  freertos::da::periodic_task<512> pt(
      std::string("PeriodicTask"), 1, []() {}, []() {}, []() {},
      std::chrono::milliseconds(100));
}

TEST_F(TaskCoverageTest, PeriodicTaskMoveAssignment) {
  TaskHandle_t mock_task2 = reinterpret_cast<TaskHandle_t>(0x22222223);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(mock_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(mock_task2), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task2));
  EXPECT_CALL(*mock, vTaskDelete(mock_task2));

  freertos::da::periodic_task<512> pt1(
      "PT1", 1, []() {}, []() {}, []() {}, std::chrono::milliseconds(100));
  freertos::da::periodic_task<512> pt2(
      "PT2", 2, []() {}, []() {}, []() {}, std::chrono::milliseconds(200));
  pt1 = std::move(pt2);
}

TEST_F(TaskCoverageTest, PeriodicTaskSwap) {
  TaskHandle_t mock_task2 = reinterpret_cast<TaskHandle_t>(0x22222223);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(mock_task), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
      .WillOnce(DoAll(SetArgPointee<5>(mock_task2), Return(pdPASS)));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task));
  EXPECT_CALL(*mock, vTaskDelete(mock_task));
  EXPECT_CALL(*mock, xTaskAbortDelay(mock_task2));
  EXPECT_CALL(*mock, vTaskDelete(mock_task2));

  freertos::da::periodic_task<512> pt1(
      "PT1", 1, []() {}, []() {}, []() {}, std::chrono::milliseconds(100));
  freertos::da::periodic_task<512> pt2(
      "PT2", 2, []() {}, []() {}, []() {}, std::chrono::milliseconds(200));
  swap(pt1, pt2);
}

TEST_F(ExpectedCoverageTest, VoidExpectedMoveAssignmentSelf) {
  freertos::expected<void, freertos::error> e1;
  e1 = std::move(e1);
  EXPECT_TRUE(e1.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedMoveAssignmentErrorToValue) {
  freertos::expected<void, freertos::error> e1;
  freertos::expected<void, freertos::error> e2{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  e2 = std::move(e1);
  EXPECT_TRUE(e2.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedMoveAssignmentValueToError) {
  freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  freertos::expected<void, freertos::error> e2;
  e2 = std::move(e1);
  EXPECT_FALSE(e2.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedMoveAssignmentErrorToError) {
  freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  freertos::expected<void, freertos::error> e2{freertos::unexpected<freertos::error>(freertos::error::invalid_handle)};
  e2 = std::move(e1);
  EXPECT_FALSE(e2.has_value());
  EXPECT_EQ(e2.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedMoveConstructionError) {
  freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  freertos::expected<void, freertos::error> e2(std::move(e1));
  EXPECT_FALSE(e2.has_value());
  EXPECT_EQ(e2.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedAndThenLValue) {
  freertos::expected<void, freertos::error> e1;
  auto result = e1.and_then([]() { return freertos::expected<int, freertos::error>(42); });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedAndThenConstLValue) {
  const freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.and_then([]() { return freertos::expected<int, freertos::error>(42); });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedOrElseLValue) {
  freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.or_else([](freertos::error &err) {
    return freertos::expected<void, freertos::error>{freertos::unexpected<freertos::error>(err)};
  });
  EXPECT_FALSE(result.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedOrElseConstLValueOnValue) {
  const freertos::expected<void, freertos::error> e1;
  auto result = e1.or_else([](const freertos::error &) {
    return freertos::expected<void, freertos::error>();
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedOrElseConstLValueOnError) {
  const freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.or_else([](const freertos::error &err) {
    return freertos::expected<void, freertos::error>{freertos::unexpected<freertos::error>(err)};
  });
  EXPECT_FALSE(result.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformLValue) {
  freertos::expected<void, freertos::error> e1;
  auto result = e1.transform([]() { return 42; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformConstLValueOnError) {
  const freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.transform([]() { return 42; });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformErrorLValue) {
  freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.transform_error([](freertos::error &err) {
    return freertos::error::invalid_handle;
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformErrorLValueOnValue) {
  freertos::expected<void, freertos::error> e1;
  auto result = e1.transform_error([](freertos::error &) {
    return freertos::error::invalid_handle;
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformErrorConstLValueOnError) {
  const freertos::expected<void, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.transform_error([](const freertos::error &) {
    return freertos::error::invalid_handle;
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(ExpectedCoverageTest, VoidExpectedTransformErrorConstLValueOnValue) {
  const freertos::expected<void, freertos::error> e1;
  auto result = e1.transform_error([](const freertos::error &) {
    return freertos::error::invalid_handle;
  });
  EXPECT_TRUE(result.has_value());
}

TEST_F(ExpectedCoverageTest, ExpectedMoveAssignment) {
  freertos::expected<int, freertos::error> e1(42);
  freertos::expected<int, freertos::error> e2(0);
  e2 = std::move(e1);
  EXPECT_TRUE(e2.has_value());
  EXPECT_EQ(e2.value(), 42);
}

TEST_F(ExpectedCoverageTest, ExpectedMoveAssignmentValueToError) {
  freertos::expected<int, freertos::error> e1(42);
  freertos::expected<int, freertos::error> e2{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  e2 = std::move(e1);
  EXPECT_TRUE(e2.has_value());
  EXPECT_EQ(e2.value(), 42);
}

TEST_F(ExpectedCoverageTest, ExpectedMoveAssignmentErrorToValue) {
  freertos::expected<int, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  freertos::expected<int, freertos::error> e2(42);
  e2 = std::move(e1);
  EXPECT_FALSE(e2.has_value());
  EXPECT_EQ(e2.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, ExpectedMoveAssignmentErrorToError) {
  freertos::expected<int, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  freertos::expected<int, freertos::error> e2{freertos::unexpected<freertos::error>(freertos::error::invalid_handle)};
  e2 = std::move(e1);
  EXPECT_FALSE(e2.has_value());
  EXPECT_EQ(e2.error(), freertos::error::timeout);
}

TEST_F(ExpectedCoverageTest, ExpectedAndThenConstLValue) {
  const freertos::expected<int, freertos::error> e1(42);
  auto result = e1.and_then([](const int &v) {
    return freertos::expected<int, freertos::error>(v * 2);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 84);
}

TEST_F(ExpectedCoverageTest, ExpectedAndThenConstLValueOnError) {
  const freertos::expected<int, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.and_then([](const int &) {
    return freertos::expected<int, freertos::error>(0);
  });
  EXPECT_FALSE(result.has_value());
}

TEST_F(ExpectedCoverageTest, ExpectedOrElseConstLValueOnError) {
  const freertos::expected<int, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.or_else([](const freertos::error &err) {
    return freertos::expected<int, freertos::error>{freertos::unexpected<freertos::error>(err)};
  });
  EXPECT_FALSE(result.has_value());
}

TEST_F(ExpectedCoverageTest, ExpectedAndThenLValueRef) {
  freertos::expected<int, freertos::error> e1(42);
  auto result = e1.and_then([](int &v) {
    return freertos::expected<int, freertos::error>(v * 2);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 84);
}

TEST_F(ExpectedCoverageTest, ExpectedAndThenLValueRefOnError) {
  freertos::expected<int, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.and_then([](int &) {
    return freertos::expected<int, freertos::error>(0);
  });
  EXPECT_FALSE(result.has_value());
}

TEST_F(ExpectedCoverageTest, ExpectedOrElseLValueRefOnError) {
  freertos::expected<int, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.or_else([](freertos::error &err) {
    return freertos::expected<int, freertos::error>{freertos::unexpected<freertos::error>(err)};
  });
  EXPECT_FALSE(result.has_value());
}

TEST_F(ExpectedCoverageTest, ExpectedOrElseLValueRefOnValue) {
  freertos::expected<int, freertos::error> e1(42);
  auto result = e1.or_else([](freertos::error &) {
    return freertos::expected<int, freertos::error>(0);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedCoverageTest, ExpectedOrElseConstLValueRefOnValue) {
  const freertos::expected<int, freertos::error> e1(42);
  auto result = e1.or_else([](const freertos::error &) {
    return freertos::expected<int, freertos::error>(0);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedCoverageTest, ExpectedTransformLValueRef) {
  freertos::expected<int, freertos::error> e1(42);
  auto result = e1.transform([](int &v) { return v * 2; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 84);
}

TEST_F(ExpectedCoverageTest, ExpectedTransformLValueRefOnError) {
  freertos::expected<int, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.transform([](int &) { return 0; });
  EXPECT_FALSE(result.has_value());
}

TEST_F(ExpectedCoverageTest, ExpectedTransformConstLValueRef) {
  const freertos::expected<int, freertos::error> e1(42);
  auto result = e1.transform([](const int &v) { return v * 2; });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 84);
}

TEST_F(ExpectedCoverageTest, ExpectedTransformConstLValueRefOnError) {
  const freertos::expected<int, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.transform([](const int &) { return 0; });
  EXPECT_FALSE(result.has_value());
}

TEST_F(ExpectedCoverageTest, ExpectedTransformErrorLValueRefOnError) {
  freertos::expected<int, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.transform_error([](freertos::error &err) {
    return freertos::error::invalid_handle;
  });
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(ExpectedCoverageTest, ExpectedTransformErrorLValueRefOnValue) {
  freertos::expected<int, freertos::error> e1(42);
  auto result = e1.transform_error([](freertos::error &) {
    return freertos::error::invalid_handle;
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedCoverageTest, ExpectedTransformErrorConstLValueRef) {
  const freertos::expected<int, freertos::error> e1{freertos::unexpected<freertos::error>(freertos::error::timeout)};
  auto result = e1.transform_error([](const freertos::error &) {
    return freertos::error::invalid_handle;
  });
  EXPECT_FALSE(result.has_value());
}

TEST_F(ExpectedCoverageTest, ExpectedTransformErrorConstLValueRefOnValue) {
  const freertos::expected<int, freertos::error> e1(42);
  auto result = e1.transform_error([](const freertos::error &) {
    return freertos::error::invalid_handle;
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST_F(ExpectedCoverageTest, ExpectedAndThenConstLValueRefOnValue) {
  const freertos::expected<int, freertos::error> e1(42);
  auto result = e1.and_then([](const int &v) {
    return freertos::expected<int, freertos::error>(v * 2);
  });
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 84);
}

TEST_F(QueueCoverageTest, QueueRegisteredNameConstructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, vQueueAddToRegistry(mock_queue, "MyQueue"));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return("MyQueue"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(mock_queue));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q("MyQueue");
}

TEST_F(QueueCoverageTest, QueueRegisteredNameNoNameConstructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q(nullptr);
}

TEST_F(QueueCoverageTest, QueueStaticAllocRegisteredName) {
  EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, vQueueAddToRegistry(mock_queue, "StaticQ"));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return("StaticQ"));
  EXPECT_CALL(*mock, vQueueUnregisterQueue(mock_queue));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::sa::queue<10, int> q("StaticQ");
}

TEST_F(QueueCoverageTest, QueueMoveConstructor) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q1;
  freertos::da::queue<10, int> q2(std::move(q1));
}

TEST_F(QueueCoverageTest, QueueMoveAssignment) {
  QueueHandle_t mock_queue2 = reinterpret_cast<QueueHandle_t>(0x44444444);
  testing::InSequence seq;
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue2));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue2)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue2));

  freertos::da::queue<10, int> q1;
  freertos::da::queue<10, int> q2;
  q1 = std::move(q2);
}

TEST_F(StreamBufferCoverageTest, StreamBufferSendExIsrIterator) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  uint8_t data[] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_sb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(size_t{4})));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.send_ex_isr(data, data + 4);
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), size_t{4});
}

TEST_F(StreamBufferCoverageTest, StreamBufferSendExIsrIteratorFailure) {
  EXPECT_CALL(*mock, xStreamBufferCreate(64, _)).WillOnce(Return(mock_sb));
  uint8_t data[] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_sb, _, 4, _))
      .WillOnce(DoAll(SetArgPointee<3>(pdFALSE), Return(size_t{0})));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_sb));

  freertos::da::stream_buffer<64> sb;
  auto result = sb.send_ex_isr(data, data + 4);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

TEST_F(QueueCoverageTest, QueueSendExSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSend(mock_queue, _, 100)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex(42, 100);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendBackExSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBack(mock_queue, _, 0)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex(42, TickType_t(0));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueReceiveExRefSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 0)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  int item = 0;
  auto result = q.receive_ex(item, TickType_t(0));
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueReceiveExRefTimeout) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReceive(mock_queue, _, 100)).WillOnce(Return(errQUEUE_EMPTY));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  int item = 0;
  auto result = q.receive_ex(item, TickType_t(100));
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(QueueCoverageTest, QueueResetExSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueReset(mock_queue)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.reset_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueOverwriteExSuccess) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueOverwrite(mock_queue, _)).WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.overwrite_ex(42);
  EXPECT_TRUE(result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendExIsrSuccessExpected) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendExIsrFailureExpected) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::queue_full);
}

TEST_F(QueueCoverageTest, QueueSendBackExIsrSuccessExpectedExtra) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendBackExIsrFailureExpectedExtra) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToBackFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_back_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::queue_full);
}

TEST_F(QueueCoverageTest, QueueSendFrontExIsrSuccessExpected) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(pdPASS)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex_isr(42);
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(QueueCoverageTest, QueueSendFrontExIsrFailureExpected) {
  EXPECT_CALL(*mock, xQueueCreate(_, _)).WillOnce(Return(mock_queue));
  EXPECT_CALL(*mock, xQueueSendToFrontFromISR(mock_queue, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(pdFALSE), Return(errQUEUE_FULL)));
  EXPECT_CALL(*mock, pcQueueGetName(mock_queue)).WillOnce(Return(nullptr));
  EXPECT_CALL(*mock, vQueueDelete(mock_queue));

  freertos::da::queue<10, int> q;
  auto result = q.send_front_ex_isr(42);
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::queue_full);
}