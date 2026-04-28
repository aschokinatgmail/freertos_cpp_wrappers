#include <array>
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "../include/freertos_isr_result.hpp"
#include "FreeRTOS.h"
#include "freertos_stream_batching_buffer.hpp"

using ::testing::_;
using ::testing::InSequence;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrictMock;

class FreeRTOSStreamBatchingBufferTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = new StrictMock<FreeRTOSMock>();
    g_freertos_mock = mock;

    mock_stream_buffer_handle =
        reinterpret_cast<StreamBufferHandle_t>(0x12345678);
    mock_stream_buffer_handle_2 =
        reinterpret_cast<StreamBufferHandle_t>(0x87654321);
  }

  void TearDown() override {
    delete mock;
    g_freertos_mock = nullptr;
  }

  StrictMock<FreeRTOSMock> *mock;
  StreamBufferHandle_t mock_stream_buffer_handle;
  StreamBufferHandle_t mock_stream_buffer_handle_2;
};

TEST_F(FreeRTOSStreamBatchingBufferTest, StaticAllocatorConstruction) {
  freertos::static_stream_batching_buffer_allocator<100> allocator;
}

TEST_F(FreeRTOSStreamBatchingBufferTest, StaticAllocatorCreate) {
  freertos::static_stream_batching_buffer_allocator<64> allocator;

  EXPECT_CALL(*mock, xStreamBufferGenericCreateStatic(64, 1, sbTYPE_STREAM_BATCHING_BUFFER, NotNull(), NotNull()))
      .WillOnce(Return(mock_stream_buffer_handle));

  auto handle = allocator.create();
  EXPECT_EQ(handle, mock_stream_buffer_handle);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, StaticAllocatorCreateWithTriggerLevel) {
  freertos::static_stream_batching_buffer_allocator<128> allocator;

  EXPECT_CALL(*mock, xStreamBufferGenericCreateStatic(128, 8, sbTYPE_STREAM_BATCHING_BUFFER, NotNull(), NotNull()))
      .WillOnce(Return(mock_stream_buffer_handle));

  auto handle = allocator.create(8);
  EXPECT_EQ(handle, mock_stream_buffer_handle);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, StaticAllocatorCreateFailure) {
  freertos::static_stream_batching_buffer_allocator<256> allocator;

  EXPECT_CALL(*mock, xStreamBufferGenericCreateStatic(256, 1, sbTYPE_STREAM_BATCHING_BUFFER, NotNull(), NotNull()))
      .WillOnce(Return(nullptr));

  auto handle = allocator.create();
  EXPECT_EQ(handle, nullptr);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, DynamicAllocatorConstruction) {
  freertos::dynamic_stream_batching_buffer_allocator<200> allocator;
}

TEST_F(FreeRTOSStreamBatchingBufferTest, DynamicAllocatorCreate) {
  freertos::dynamic_stream_batching_buffer_allocator<512> allocator;

  EXPECT_CALL(*mock, xStreamBufferGenericCreate(512, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));

  auto handle = allocator.create();
  EXPECT_EQ(handle, mock_stream_buffer_handle);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, DynamicAllocatorCreateWithTriggerLevel) {
  freertos::dynamic_stream_batching_buffer_allocator<1024> allocator;

  EXPECT_CALL(*mock, xStreamBufferGenericCreate(1024, 16, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));

  auto handle = allocator.create(16);
  EXPECT_EQ(handle, mock_stream_buffer_handle);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, DynamicAllocatorCreateFailure) {
  freertos::dynamic_stream_batching_buffer_allocator<2048> allocator;

  EXPECT_CALL(*mock, xStreamBufferGenericCreate(2048, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(nullptr));

  auto handle = allocator.create();
  EXPECT_EQ(handle, nullptr);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, StaticStreamBatchingBufferConstruction) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreateStatic(100, 1, sbTYPE_STREAM_BATCHING_BUFFER, NotNull(), NotNull()))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::sa::stream_batching_buffer<100> buffer;
}

TEST_F(FreeRTOSStreamBatchingBufferTest, DynamicStreamBatchingBufferConstruction) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(200, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<200> buffer;
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ConstructionWithTriggerLevel) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(300, 10, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<300> buffer(10);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, DestructionNullHandle) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(150, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(nullptr));

  freertos::da::stream_batching_buffer<150> buffer;
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendBasic) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10,
                                        portMAX_DELAY))
      .WillOnce(Return(10));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = buffer.send(data, sizeof(data));
  EXPECT_EQ(result, 10);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendWithTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(256, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 5, 100))
      .WillOnce(Return(5));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<256> buffer;
  std::array<uint8_t, 5> data = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};

  auto result = buffer.send(data.data(), data.size(), 100);
  EXPECT_EQ(result, 5);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendChronoTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(512, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 8, 50))
      .WillOnce(Return(8));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<512> buffer;
  std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

  auto result =
      buffer.send(data.data(), data.size(), std::chrono::milliseconds(50));
  EXPECT_EQ(result, 8);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendIterators) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(1024, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 6,
                                        portMAX_DELAY))
      .WillOnce(Return(6));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<1024> buffer;
  std::vector<uint8_t> data = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60};

  auto result = buffer.send(data.begin(), data.end());
  EXPECT_EQ(result, 6);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendIteratorsWithTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(2048, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 4, 200))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<2048> buffer;
  std::string data = "test";

  auto result =
      buffer.send(data.begin(), data.end(), std::chrono::milliseconds(200));
  EXPECT_EQ(result, 4);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendPartial) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 20, 10))
      .WillOnce(Return(15));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;
  uint8_t data[20];
  std::fill(std::begin(data), std::end(data), 0xFF);

  auto result = buffer.send(data, sizeof(data), 10);
  EXPECT_EQ(result, 15);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendISR) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_stream_buffer_handle,
                                               NotNull(), 8, NotNull()))
      .WillOnce(Return(8));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[8] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0x07, 0x18};

  auto result = buffer.send_isr(data, sizeof(data));
  EXPECT_EQ(result.result, 8);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendISRIterators) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(512, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_stream_buffer_handle,
                                               NotNull(), 12, NotNull()))
      .WillOnce(Return(12));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<512> buffer;
  std::string data = "Hello World!";

  auto result = buffer.send_isr(data.begin(), data.end());
  EXPECT_EQ(result.result, 12);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveBasic) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(),
                                           10, portMAX_DELAY))
      .WillOnce(Return(10));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[10];

  auto result = buffer.receive(data, sizeof(data));
  EXPECT_EQ(result, 10);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveWithTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(256, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(
      *mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 5, 150))
      .WillOnce(Return(5));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<256> buffer;
  std::array<uint8_t, 5> data;

  auto result = buffer.receive(data.data(), data.size(), 150);
  EXPECT_EQ(result, 5);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveChronoTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(512, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(
      *mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 20, 75))
      .WillOnce(Return(15));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<512> buffer;
  std::vector<uint8_t> data(20);

  auto result =
      buffer.receive(data.data(), data.size(), std::chrono::milliseconds(75));
  EXPECT_EQ(result, 15);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(
      *mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 10, 50))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;
  uint8_t data[10];

  auto result = buffer.receive(data, sizeof(data), 50);
  EXPECT_EQ(result, 0);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveISR) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(mock_stream_buffer_handle,
                                                  NotNull(), 8, NotNull()))
      .WillOnce(Return(8));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[8];

  auto result = buffer.receive_isr(data, sizeof(data));
  EXPECT_EQ(result.result, 8);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, BytesAvailable) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferBytesAvailable(mock_stream_buffer_handle))
      .WillOnce(Return(42));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;

  auto result = buffer.available();
  EXPECT_EQ(result, 42);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SpacesAvailable) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(256, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSpacesAvailable(mock_stream_buffer_handle))
      .WillOnce(Return(86));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<256> buffer;

  auto result = buffer.free();
  EXPECT_EQ(result, 86);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, IsEmpty) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(mock_stream_buffer_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;

  auto result = buffer.empty();
  EXPECT_EQ(result, pdTRUE);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, IsNotEmpty) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(mock_stream_buffer_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;

  auto result = buffer.empty();
  EXPECT_EQ(result, pdFALSE);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, IsFull) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(256, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferIsFull(mock_stream_buffer_handle))
      .WillOnce(Return(pdTRUE));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<256> buffer;

  auto result = buffer.full();
  EXPECT_EQ(result, pdTRUE);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, IsNotFull) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(512, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferIsFull(mock_stream_buffer_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<512> buffer;

  auto result = buffer.full();
  EXPECT_EQ(result, pdFALSE);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, Reset) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(mock_stream_buffer_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;

  auto result = buffer.reset();
  EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ResetFailure) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(256, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(mock_stream_buffer_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<256> buffer;

  auto result = buffer.reset();
  EXPECT_EQ(result, pdFAIL);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SetTriggerLevel) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(512, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSetTriggerLevel(mock_stream_buffer_handle, 32))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<512> buffer;

  auto result = buffer.set_trigger_level(32);
  EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SetTriggerLevelFailure) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSetTriggerLevel(mock_stream_buffer_handle, 100))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;

  auto result = buffer.set_trigger_level(100);
  EXPECT_EQ(result, pdFAIL);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, GetHandle) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;

  auto handle = buffer.handle();
  EXPECT_EQ(handle, mock_stream_buffer_handle);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendZeroBytes) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 0,
                                         portMAX_DELAY))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[1] = {0x00};

  auto result = buffer.send(data, 0);
  EXPECT_EQ(result, 0);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveZeroBytes) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(),
                                            0, portMAX_DELAY))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[1];

  auto result = buffer.receive(data, 0);
  EXPECT_EQ(result, 0);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ComplexScenario) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(256, 4, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));

  InSequence seq;

  EXPECT_CALL(*mock,
              xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10, 100))
      .WillOnce(Return(10));

  EXPECT_CALL(*mock, xStreamBufferBytesAvailable(mock_stream_buffer_handle))
      .WillOnce(Return(10));

  EXPECT_CALL(*mock,
              xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 5, 50))
      .WillOnce(Return(5));

  EXPECT_CALL(*mock, xStreamBufferBytesAvailable(mock_stream_buffer_handle))
      .WillOnce(Return(5));

  EXPECT_CALL(*mock, xStreamBufferReset(mock_stream_buffer_handle))
      .WillOnce(Return(pdPASS));

  EXPECT_CALL(*mock, xStreamBufferIsEmpty(mock_stream_buffer_handle))
      .WillOnce(Return(pdTRUE));

  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<256> buffer(4);

  uint8_t send_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  uint8_t receive_data[5];

  EXPECT_EQ(buffer.send(send_data, sizeof(send_data), 100), 10);
  EXPECT_EQ(buffer.available(), 10);
  EXPECT_EQ(buffer.receive(receive_data, sizeof(receive_data), 50), 5);
  EXPECT_EQ(buffer.available(), 5);
  EXPECT_EQ(buffer.reset(), pdPASS);
  EXPECT_EQ(buffer.empty(), pdTRUE);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, StaticVsDynamicBehavior) {
  constexpr size_t BUFFER_SIZE = 128;
  constexpr size_t TRIGGER_LEVEL = 8;

  EXPECT_CALL(*mock, xStreamBufferGenericCreateStatic(BUFFER_SIZE, TRIGGER_LEVEL,
                                                       sbTYPE_STREAM_BATCHING_BUFFER,
                                                       NotNull(), NotNull()))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10,
                                        portMAX_DELAY))
      .WillOnce(Return(10));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  {
    freertos::sa::stream_batching_buffer<BUFFER_SIZE> static_buffer(TRIGGER_LEVEL);
    uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    EXPECT_EQ(static_buffer.send(data, sizeof(data)), 10);
  }

  EXPECT_CALL(*mock, xStreamBufferGenericCreate(BUFFER_SIZE, TRIGGER_LEVEL,
                                                 sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle_2));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle_2, NotNull(),
                                        10, portMAX_DELAY))
      .WillOnce(Return(10));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle_2)).Times(1);

  {
    freertos::da::stream_batching_buffer<BUFFER_SIZE> dynamic_buffer(TRIGGER_LEVEL);
    uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    EXPECT_EQ(dynamic_buffer.send(data, sizeof(data)), 10);
  }
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ChronoMicrosecondsTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 4, 2))
      .WillOnce(Return(4));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint32_t data = 0x12345678;

  auto result =
      buffer.send(&data, sizeof(data), std::chrono::microseconds(2000));
  EXPECT_EQ(result, 4);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ChronoNanosecondsTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(256, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 8, 3))
      .WillOnce(Return(8));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<256> buffer;
  uint64_t data;

  auto result =
      buffer.receive(&data, sizeof(data), std::chrono::nanoseconds(3000000));
  EXPECT_EQ(result, 8);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ChronoSecondsTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(512, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 16, 5000))
      .WillOnce(Return(16));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<512> buffer;
  std::array<uint8_t, 16> data;
  data.fill(0xAB);

  auto result = buffer.send(data.data(), data.size(), std::chrono::seconds(5));
  EXPECT_EQ(result, 16);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, APICompleteness) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(1024, 8, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10,
                                         portMAX_DELAY))
      .WillOnce(Return(10));
  EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(),
                                            10, portMAX_DELAY))
      .WillOnce(Return(10));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_stream_buffer_handle,
                                                NotNull(), 5, NotNull()))
      .WillOnce(Return(5));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(mock_stream_buffer_handle,
                                                   NotNull(), 5, NotNull()))
      .WillOnce(Return(5));
  EXPECT_CALL(*mock, xStreamBufferBytesAvailable(mock_stream_buffer_handle))
      .WillOnce(Return(100));
  EXPECT_CALL(*mock, xStreamBufferSpacesAvailable(mock_stream_buffer_handle))
      .WillOnce(Return(924));
  EXPECT_CALL(*mock, xStreamBufferIsEmpty(mock_stream_buffer_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, xStreamBufferIsFull(mock_stream_buffer_handle))
      .WillOnce(Return(pdFALSE));
  EXPECT_CALL(*mock, xStreamBufferReset(mock_stream_buffer_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock,
              xStreamBufferSetTriggerLevel(mock_stream_buffer_handle, 16))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<1024> buffer(8);

  uint8_t send_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  uint8_t receive_data[10];
  uint8_t isr_data[5] = {0xA, 0xB, 0xC, 0xD, 0xE};
  uint8_t isr_receive[5];

  EXPECT_EQ(buffer.send(send_data, sizeof(send_data)), 10);
  EXPECT_EQ(buffer.receive(receive_data, sizeof(receive_data)), 10);
  auto send_isr_result = buffer.send_isr(isr_data, sizeof(isr_data));
  EXPECT_EQ(send_isr_result.result, 5);
  auto receive_isr_result =
      buffer.receive_isr(isr_receive, sizeof(isr_receive));
  EXPECT_EQ(receive_isr_result.result, 5);
  EXPECT_EQ(buffer.available(), 100);
  EXPECT_EQ(buffer.free(), 924);
  EXPECT_EQ(buffer.empty(), pdFALSE);
  EXPECT_EQ(buffer.full(), pdFALSE);
  EXPECT_EQ(buffer.reset(), pdPASS);
  EXPECT_EQ(buffer.set_trigger_level(16), pdPASS);
  EXPECT_EQ(buffer.handle(), mock_stream_buffer_handle);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendExSuccess) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10,
                                         portMAX_DELAY))
      .WillOnce(Return(10));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = buffer.send_ex(data, sizeof(data));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 10);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendExTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10,
                                         100))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;
  uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = buffer.send_ex(data, sizeof(data), 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendExWouldBlock) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10,
                                         0))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;
  uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = buffer.send_ex(data, sizeof(data), 0);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::would_block);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveExSuccess) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(),
                                            10, portMAX_DELAY))
      .WillOnce(Return(10));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[10];

  auto result = buffer.receive_ex(data, sizeof(data));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 10);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveExTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(),
                                            10, 100))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;
  uint8_t data[10];

  auto result = buffer.receive_ex(data, sizeof(data), 100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::timeout);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ResetExSuccess) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(mock_stream_buffer_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;

  auto result = buffer.reset_ex();
  EXPECT_TRUE(result.has_value());
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ResetExFailure) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReset(mock_stream_buffer_handle))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;

  auto result = buffer.reset_ex();
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ResetISR) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferResetFromISR(mock_stream_buffer_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;

  auto result = buffer.reset_isr();
  EXPECT_TRUE(result.result);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ResetExISR) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferResetFromISR(mock_stream_buffer_handle))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;

  auto result = buffer.reset_ex_isr();
  EXPECT_TRUE(result.result.has_value());
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SetTriggerLevelExSuccess) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(512, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSetTriggerLevel(mock_stream_buffer_handle, 32))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<512> buffer;

  auto result = buffer.set_trigger_level_ex(32);
  EXPECT_TRUE(result.has_value());
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SetTriggerLevelExFailure) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock,
              xStreamBufferSetTriggerLevel(mock_stream_buffer_handle, 100))
      .WillOnce(Return(pdFAIL));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;

  auto result = buffer.set_trigger_level_ex(100);
  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), freertos::error::invalid_parameter);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendExISR) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_stream_buffer_handle,
                                                NotNull(), 8, NotNull()))
      .WillOnce(Return(8));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};

  auto result = buffer.send_ex_isr(data, sizeof(data));
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), 8);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendExISRFailure) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_stream_buffer_handle,
                                                NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;
  uint8_t data[4] = {1, 2, 3, 4};

  auto result = buffer.send_ex_isr(data, sizeof(data));
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveExISR) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(mock_stream_buffer_handle,
                                                   NotNull(), 8, NotNull()))
      .WillOnce(Return(8));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[8];

  auto result = buffer.receive_ex_isr(data, sizeof(data));
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), 8);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveExISRFailure) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(mock_stream_buffer_handle,
                                                   NotNull(), 4, NotNull()))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;
  uint8_t data[4];

  auto result = buffer.receive_ex_isr(data, sizeof(data));
  EXPECT_FALSE(result.result.has_value());
  EXPECT_EQ(result.result.error(), freertos::error::would_block);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, MoveConstruction) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle));

  freertos::stream_batching_buffer<64, freertos::dynamic_stream_batching_buffer_allocator<64>>
      sbb1;
  freertos::stream_batching_buffer<64, freertos::dynamic_stream_batching_buffer_allocator<64>>
      sbb2(std::move(sbb1));
}

TEST_F(FreeRTOSStreamBatchingBufferTest, MoveAssignment) {
  StreamBufferHandle_t handle1 = reinterpret_cast<StreamBufferHandle_t>(0x1111);
  StreamBufferHandle_t handle2 = reinterpret_cast<StreamBufferHandle_t>(0x2222);

  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle1));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle2));

  freertos::stream_batching_buffer<64, freertos::dynamic_stream_batching_buffer_allocator<64>>
      sbb1;
  freertos::stream_batching_buffer<64, freertos::dynamic_stream_batching_buffer_allocator<64>>
      sbb2;
  sbb1 = std::move(sbb2);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, Swap) {
  StreamBufferHandle_t handle1 = reinterpret_cast<StreamBufferHandle_t>(0x1111);
  StreamBufferHandle_t handle2 = reinterpret_cast<StreamBufferHandle_t>(0x2222);

  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle1));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle2));

  freertos::stream_batching_buffer<64, freertos::dynamic_stream_batching_buffer_allocator<64>>
      sbb1;
  freertos::stream_batching_buffer<64, freertos::dynamic_stream_batching_buffer_allocator<64>>
      sbb2;
  sbb1.swap(sbb2);
}

TEST_F(FreeRTOSStreamBatchingBufferTest,
       SwapExchangesAllocator) {
  StreamBufferHandle_t handle1 = reinterpret_cast<StreamBufferHandle_t>(0x1111);
  StreamBufferHandle_t handle2 = reinterpret_cast<StreamBufferHandle_t>(0x2222);

  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle1));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle2));

  freertos::stream_batching_buffer<64, freertos::dynamic_stream_batching_buffer_allocator<64>>
      sbb1;
  freertos::stream_batching_buffer<64, freertos::dynamic_stream_batching_buffer_allocator<64>>
      sbb2;
  sbb1.swap(sbb2);

  uint8_t data1[] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xStreamBufferSend(handle2, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_EQ(sbb1.send(data1, 4), 4);

  uint8_t data2[] = {5, 6, 7, 8};
  EXPECT_CALL(*mock, xStreamBufferSend(handle1, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_EQ(sbb2.send(data2, 4), 4);
}

TEST_F(FreeRTOSStreamBatchingBufferTest,
       MoveConstructionTransfersAllocator) {
  StreamBufferHandle_t handle1 = reinterpret_cast<StreamBufferHandle_t>(0x1111);

  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(handle1));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle1));

  freertos::stream_batching_buffer<64, freertos::dynamic_stream_batching_buffer_allocator<64>>
      sbb1;
  freertos::stream_batching_buffer<64, freertos::dynamic_stream_batching_buffer_allocator<64>>
      sbb2(std::move(sbb1));

  uint8_t data[] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xStreamBufferSend(handle1, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_EQ(sbb2.send(data, 4), 4);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendExChronoTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10,
                                         100))
      .WillOnce(Return(10));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  auto result = buffer.send_ex(data, sizeof(data), std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 10);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, ReceiveExChronoTimeout) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(),
                                            10, 100))
      .WillOnce(Return(10));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  uint8_t data[10];

  auto result = buffer.receive_ex(data, sizeof(data), std::chrono::milliseconds(100));
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 10);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendExIterators) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 5,
                                         portMAX_DELAY))
      .WillOnce(Return(5));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  std::vector<uint8_t> data = {1, 2, 3, 4, 5};

  auto result = buffer.send_ex(data.begin(), data.end());
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 5);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SendExISRIterators) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(64, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_stream_buffer_handle,
                                                NotNull(), 3, NotNull()))
      .WillOnce(Return(3));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<64> buffer;
  std::vector<uint8_t> data = {1, 2, 3};

  auto result = buffer.send_ex_isr(data.begin(), data.end());
  EXPECT_TRUE(result.result.has_value());
  EXPECT_EQ(result.result.value(), 3);
}

#if configSUPPORT_STATIC_ALLOCATION
TEST_F(FreeRTOSStreamBatchingBufferTest, StaticMoveConstruction) {
  StreamBufferHandle_t handle = reinterpret_cast<StreamBufferHandle_t>(0xAAAA);

  EXPECT_CALL(*mock, xStreamBufferGenericCreateStatic(64, 1, sbTYPE_STREAM_BATCHING_BUFFER,
                                                       NotNull(), NotNull()))
      .WillOnce(Return(handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle));

  freertos::sa::stream_batching_buffer<64> sbb1;
  freertos::sa::stream_batching_buffer<64> sbb2(std::move(sbb1));

  uint8_t data[] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xStreamBufferSend(handle, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_EQ(sbb2.send(data, 4), 4);
}

TEST_F(FreeRTOSStreamBatchingBufferTest, StaticSwap) {
  StreamBufferHandle_t handle1 = reinterpret_cast<StreamBufferHandle_t>(0xAAAA);
  StreamBufferHandle_t handle2 = reinterpret_cast<StreamBufferHandle_t>(0xBBBB);

  EXPECT_CALL(*mock, xStreamBufferGenericCreateStatic(64, 1, sbTYPE_STREAM_BATCHING_BUFFER,
                                                       NotNull(), NotNull()))
      .WillOnce(Return(handle1))
      .WillOnce(Return(handle2));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle1));
  EXPECT_CALL(*mock, vStreamBufferDelete(handle2));

  freertos::sa::stream_batching_buffer<64> sbb1;
  freertos::sa::stream_batching_buffer<64> sbb2;
  sbb1.swap(sbb2);

  uint8_t data1[] = {1, 2, 3, 4};
  EXPECT_CALL(*mock, xStreamBufferSend(handle2, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_EQ(sbb1.send(data1, 4), 4);

  uint8_t data2[] = {5, 6, 7, 8};
  EXPECT_CALL(*mock, xStreamBufferSend(handle1, NotNull(), 4, portMAX_DELAY))
      .WillOnce(Return(4));
  EXPECT_EQ(sbb2.send(data2, 4), 4);
}
#endif

TEST_F(FreeRTOSStreamBatchingBufferTest, BatchingBufferUsesBatchingType) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(256, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<256> buffer;
}

TEST_F(FreeRTOSStreamBatchingBufferTest, StaticBatchingBufferUsesBatchingType) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreateStatic(256, 1, sbTYPE_STREAM_BATCHING_BUFFER,
                                                        NotNull(), NotNull()))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::sa::stream_batching_buffer<256> buffer;
}

TEST_F(FreeRTOSStreamBatchingBufferTest, SetNotificationIndex) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreate(128, 1, sbTYPE_STREAM_BATCHING_BUFFER))
      .WillOnce(Return(mock_stream_buffer_handle));
  EXPECT_CALL(*mock, vStreamBufferSetStreamBufferNotificationIndex(mock_stream_buffer_handle, 1))
      .Times(1);
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::da::stream_batching_buffer<128> buffer;
  buffer.set_notification_index(1);
}

#if configSUPPORT_STATIC_ALLOCATION
TEST_F(FreeRTOSStreamBatchingBufferTest, GetStaticBuffers) {
  EXPECT_CALL(*mock, xStreamBufferGenericCreateStatic(128, 1, sbTYPE_STREAM_BATCHING_BUFFER,
                                                        NotNull(), NotNull()))
      .WillOnce(Return(mock_stream_buffer_handle));
  uint8_t *storage_ptr = nullptr;
  StaticStreamBuffer_t *static_ptr = nullptr;
  EXPECT_CALL(*mock, xStreamBufferGetStaticBuffers(mock_stream_buffer_handle, NotNull(), NotNull()))
      .WillOnce(Return(pdPASS));
  EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle)).Times(1);

  freertos::sa::stream_batching_buffer<128> buffer;
  auto result = buffer.get_static_buffers(&storage_ptr, &static_ptr);
  EXPECT_TRUE(result);
}
#endif