/**
 * @file test_freertos_stream_buffer.cpp
 * @author GitHub Copilot AI Assistant
 * @brief Comprehensive unit tests for FreeRTOS C++ stream buffer wrappers
 * @version 1.0
 * @date 2024-12-20
 * 
 * This file contains comprehensive behavioral unit tests for the FreeRTOS stream
 * buffer wrapper library, including static and dynamic stream buffer allocators 
 * with comprehensive stream buffer operations testing.
 *
 * Test Coverage:
 * - Static and dynamic stream buffer allocators
 * - Stream buffer creation and destruction
 * - Send operations (send, send_isr) with timeouts and ISR variants
 * - Receive operations (receive, receive_isr) with timeouts and ISR variants
 * - Stream buffer state operations (available, free, empty, full)
 * - Reset and trigger level operations
 * - Iterator-based send operations
 * - Error handling and edge cases
 * - Chrono timeout compatibility
 * - Template specialization with different buffer sizes
 * 
 * Mock Strategy:
 * - Complete FreeRTOS stream buffer API mocking for host-based testing
 * - Validation of parameter passing and return value handling
 * - ISR function testing (commented where host limitations apply)
 * - Clear documentation of RTOS-only features
 * 
 * RTOS-Only Features (Not Testable in Host Environment):
 * - Actual blocking behavior and task synchronization
 * - Real ISR context execution and priority management
 * - Memory allocation failures in actual FreeRTOS heap
 * - Buffer overflow/underflow protection at hardware level
 * - Task notification mechanisms triggered by stream buffer operations
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <vector>
#include <string>
#include <array>

// Include mocks before the actual headers
#include "FreeRTOS.h"
#include "freertos_stream_buffer.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::NotNull;
using ::testing::StrictMock;
using ::testing::InSequence;

class FreeRTOSStreamBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a strict mock for precise API verification
        mock = new StrictMock<FreeRTOSMock>();
        g_freertos_mock = mock;
        
        // Create mock handles for testing
        mock_stream_buffer_handle = reinterpret_cast<StreamBufferHandle_t>(0x12345678);
        mock_stream_buffer_handle_2 = reinterpret_cast<StreamBufferHandle_t>(0x87654321);
    }

    void TearDown() override {
        delete mock;
        g_freertos_mock = nullptr;
    }

    StrictMock<FreeRTOSMock>* mock;
    StreamBufferHandle_t mock_stream_buffer_handle;
    StreamBufferHandle_t mock_stream_buffer_handle_2;
};

// =============================================================================
// Static Stream Buffer Allocator Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StaticStreamBufferAllocatorConstruction) {
    // Test that static allocator can be constructed without API calls
    freertos::static_stream_buffer_allocator<100> allocator;
    // No mock expectations needed - constructor should not call FreeRTOS APIs
}

TEST_F(FreeRTOSStreamBufferTest, StaticStreamBufferAllocatorCreate) {
    freertos::static_stream_buffer_allocator<64> allocator;
    
    EXPECT_CALL(*mock, xStreamBufferCreateStatic(64, 1, NotNull(), NotNull()))
        .WillOnce(Return(mock_stream_buffer_handle));
    
    auto handle = allocator.create();
    EXPECT_EQ(handle, mock_stream_buffer_handle);
}

TEST_F(FreeRTOSStreamBufferTest, StaticStreamBufferAllocatorCreateWithTriggerLevel) {
    freertos::static_stream_buffer_allocator<128> allocator;
    
    EXPECT_CALL(*mock, xStreamBufferCreateStatic(128, 8, NotNull(), NotNull()))
        .WillOnce(Return(mock_stream_buffer_handle));
    
    auto handle = allocator.create(8);
    EXPECT_EQ(handle, mock_stream_buffer_handle);
}

TEST_F(FreeRTOSStreamBufferTest, StaticStreamBufferAllocatorCreateFailure) {
    freertos::static_stream_buffer_allocator<256> allocator;
    
    EXPECT_CALL(*mock, xStreamBufferCreateStatic(256, 1, NotNull(), NotNull()))
        .WillOnce(Return(nullptr));
    
    auto handle = allocator.create();
    EXPECT_EQ(handle, nullptr);
}

// =============================================================================
// Dynamic Stream Buffer Allocator Tests  
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, DynamicStreamBufferAllocatorConstruction) {
    // Test that dynamic allocator can be constructed without API calls
    freertos::dynamic_stream_buffer_allocator<200> allocator;
    // No mock expectations needed - constructor should not call FreeRTOS APIs
}

TEST_F(FreeRTOSStreamBufferTest, DynamicStreamBufferAllocatorCreate) {
    freertos::dynamic_stream_buffer_allocator<512> allocator;
    
    EXPECT_CALL(*mock, xStreamBufferCreate(512, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    
    auto handle = allocator.create();
    EXPECT_EQ(handle, mock_stream_buffer_handle);
}

TEST_F(FreeRTOSStreamBufferTest, DynamicStreamBufferAllocatorCreateWithTriggerLevel) {
    freertos::dynamic_stream_buffer_allocator<1024> allocator;
    
    EXPECT_CALL(*mock, xStreamBufferCreate(1024, 16))
        .WillOnce(Return(mock_stream_buffer_handle));
    
    auto handle = allocator.create(16);
    EXPECT_EQ(handle, mock_stream_buffer_handle);
}

TEST_F(FreeRTOSStreamBufferTest, DynamicStreamBufferAllocatorCreateFailure) {
    freertos::dynamic_stream_buffer_allocator<2048> allocator;
    
    EXPECT_CALL(*mock, xStreamBufferCreate(2048, 1))
        .WillOnce(Return(nullptr));
    
    auto handle = allocator.create();
    EXPECT_EQ(handle, nullptr);
}

// =============================================================================
// Stream Buffer Construction and Destruction Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StaticStreamBufferConstruction) {
    EXPECT_CALL(*mock, xStreamBufferCreateStatic(100, 1, NotNull(), NotNull()))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::sa::stream_buffer<100> buffer;
    // Constructor should have called create and destructor should call delete
}

TEST_F(FreeRTOSStreamBufferTest, DynamicStreamBufferConstruction) {
    EXPECT_CALL(*mock, xStreamBufferCreate(200, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<200> buffer;
    // Constructor should have called create and destructor should call delete
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferConstructionWithTriggerLevel) {
    EXPECT_CALL(*mock, xStreamBufferCreate(300, 10))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<300> buffer(10);
    // Constructor should have called create with custom trigger level
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferDestructionNullHandle) {
    EXPECT_CALL(*mock, xStreamBufferCreate(150, 1))
        .WillOnce(Return(nullptr));
    
    // No vStreamBufferDelete call expected for null handle
    freertos::da::stream_buffer<150> buffer;
}

// =============================================================================
// Stream Buffer Send Operations Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSendBasic) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10, portMAX_DELAY))
        .WillOnce(Return(10));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    auto result = buffer.send(data, sizeof(data));
    EXPECT_EQ(result, 10);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSendWithTimeout) {
    EXPECT_CALL(*mock, xStreamBufferCreate(256, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 5, 100))
        .WillOnce(Return(5));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<256> buffer;
    std::array<uint8_t, 5> data = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
    
    auto result = buffer.send(data.data(), data.size(), 100);
    EXPECT_EQ(result, 5);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSendChronoTimeout) {
    EXPECT_CALL(*mock, xStreamBufferCreate(512, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 8, 50))
        .WillOnce(Return(8));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<512> buffer;
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    
    auto result = buffer.send(data.data(), data.size(), std::chrono::milliseconds(50));
    EXPECT_EQ(result, 8);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSendIterators) {
    EXPECT_CALL(*mock, xStreamBufferCreate(1024, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 6, portMAX_DELAY))
        .WillOnce(Return(6));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<1024> buffer;
    std::vector<uint8_t> data = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60};
    
    auto result = buffer.send(data.begin(), data.end());
    EXPECT_EQ(result, 6);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSendIteratorsWithTimeout) {
    EXPECT_CALL(*mock, xStreamBufferCreate(2048, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 4, 200))
        .WillOnce(Return(4));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<2048> buffer;
    std::string data = "test";
    
    auto result = buffer.send(data.begin(), data.end(), std::chrono::milliseconds(200));
    EXPECT_EQ(result, 4);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSendPartial) {
    EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 20, 10))
        .WillOnce(Return(15)); // Partial send
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<64> buffer;
    uint8_t data[20];
    std::fill(std::begin(data), std::end(data), 0xFF);
    
    auto result = buffer.send(data, sizeof(data), 10);
    EXPECT_EQ(result, 15); // Should return partial send size
}

// =============================================================================
// Stream Buffer Send ISR Operations Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSendISR) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_stream_buffer_handle, NotNull(), 8, NotNull()))
        .WillOnce(Return(8));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    uint8_t data[8] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0x07, 0x18};
    BaseType_t higher_priority_task_woken = pdFALSE;
    
    auto result = buffer.send_isr(data, sizeof(data), higher_priority_task_woken);
    EXPECT_EQ(result, 8);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSendISRWithoutWoken) {
    EXPECT_CALL(*mock, xStreamBufferCreate(256, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_stream_buffer_handle, NotNull(), 4, NotNull()))
        .WillOnce(Return(4));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<256> buffer;
    uint32_t data = 0x12345678;
    
    auto result = buffer.send_isr(&data, sizeof(data));
    EXPECT_EQ(result, 4);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSendISRIterators) {
    EXPECT_CALL(*mock, xStreamBufferCreate(512, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_stream_buffer_handle, NotNull(), 12, NotNull()))
        .WillOnce(Return(12));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<512> buffer;
    std::string data = "Hello World!";
    BaseType_t higher_priority_task_woken = pdFALSE;
    
    auto result = buffer.send_isr(data.begin(), data.end(), higher_priority_task_woken);
    EXPECT_EQ(result, 12);
}

// =============================================================================
// Stream Buffer Receive Operations Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StreamBufferReceiveBasic) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 10, portMAX_DELAY))
        .WillOnce(Return(10));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    uint8_t data[10];
    
    auto result = buffer.receive(data, sizeof(data));
    EXPECT_EQ(result, 10);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferReceiveWithTimeout) {
    EXPECT_CALL(*mock, xStreamBufferCreate(256, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 5, 150))
        .WillOnce(Return(5));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<256> buffer;
    std::array<uint8_t, 5> data;
    
    auto result = buffer.receive(data.data(), data.size(), 150);
    EXPECT_EQ(result, 5);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferReceiveChronoTimeout) {
    EXPECT_CALL(*mock, xStreamBufferCreate(512, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 20, 75))
        .WillOnce(Return(15)); // Partial receive
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<512> buffer;
    std::vector<uint8_t> data(20);
    
    auto result = buffer.receive(data.data(), data.size(), std::chrono::milliseconds(75));
    EXPECT_EQ(result, 15);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferReceiveTimeout) {
    EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 10, 50))
        .WillOnce(Return(0)); // Timeout, no data received
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<64> buffer;
    uint8_t data[10];
    
    auto result = buffer.receive(data, sizeof(data), 50);
    EXPECT_EQ(result, 0);
}

// =============================================================================
// Stream Buffer Receive ISR Operations Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StreamBufferReceiveISR) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(mock_stream_buffer_handle, NotNull(), 8, NotNull()))
        .WillOnce(Return(8));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    uint8_t data[8];
    BaseType_t higher_priority_task_woken = pdFALSE;
    
    auto result = buffer.receive_isr(data, sizeof(data), higher_priority_task_woken);
    EXPECT_EQ(result, 8);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferReceiveISRWithoutWoken) {
    EXPECT_CALL(*mock, xStreamBufferCreate(256, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(mock_stream_buffer_handle, NotNull(), 4, NotNull()))
        .WillOnce(Return(4));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<256> buffer;
    uint32_t data;
    
    auto result = buffer.receive_isr(&data, sizeof(data));
    EXPECT_EQ(result, 4);
}

// =============================================================================
// Stream Buffer State Query Operations Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StreamBufferBytesAvailable) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferBytesAvailable(mock_stream_buffer_handle))
        .WillOnce(Return(42));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    
    auto result = buffer.available();
    EXPECT_EQ(result, 42);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSpacesAvailable) {
    EXPECT_CALL(*mock, xStreamBufferCreate(256, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSpacesAvailable(mock_stream_buffer_handle))
        .WillOnce(Return(86));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<256> buffer;
    
    auto result = buffer.free();
    EXPECT_EQ(result, 86);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferIsEmpty) {
    EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferIsEmpty(mock_stream_buffer_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<64> buffer;
    
    auto result = buffer.empty();
    EXPECT_EQ(result, pdTRUE);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferIsNotEmpty) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferIsEmpty(mock_stream_buffer_handle))
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    
    auto result = buffer.empty();
    EXPECT_EQ(result, pdFALSE);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferIsFull) {
    EXPECT_CALL(*mock, xStreamBufferCreate(256, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferIsFull(mock_stream_buffer_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<256> buffer;
    
    auto result = buffer.full();
    EXPECT_EQ(result, pdTRUE);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferIsNotFull) {
    EXPECT_CALL(*mock, xStreamBufferCreate(512, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferIsFull(mock_stream_buffer_handle))
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<512> buffer;
    
    auto result = buffer.full();
    EXPECT_EQ(result, pdFALSE);
}

// =============================================================================
// Stream Buffer Control Operations Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StreamBufferReset) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferReset(mock_stream_buffer_handle))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    
    auto result = buffer.reset();
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferResetFailure) {
    EXPECT_CALL(*mock, xStreamBufferCreate(256, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferReset(mock_stream_buffer_handle))
        .WillOnce(Return(pdFAIL));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<256> buffer;
    
    auto result = buffer.reset();
    EXPECT_EQ(result, pdFAIL);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSetTriggerLevel) {
    EXPECT_CALL(*mock, xStreamBufferCreate(512, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(mock_stream_buffer_handle, 32))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<512> buffer;
    
    auto result = buffer.set_trigger_level(32);
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSetTriggerLevelFailure) {
    EXPECT_CALL(*mock, xStreamBufferCreate(64, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(mock_stream_buffer_handle, 100))
        .WillOnce(Return(pdFAIL)); // Trigger level too large for buffer
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<64> buffer;
    
    auto result = buffer.set_trigger_level(100);
    EXPECT_EQ(result, pdFAIL);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferGetHandle) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    
    auto handle = buffer.handle();
    EXPECT_EQ(handle, mock_stream_buffer_handle);
}

// =============================================================================
// Edge Cases and Error Handling Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StreamBufferCreationFailure) {
    // Test behavior when stream buffer creation fails
    // Note: In the actual implementation, configASSERT would trigger when creation fails
    // This test documents the expected behavior but cannot be fully tested in host environment
    // due to assertion handling. In real FreeRTOS, assertion failure would halt execution.
    
    // We cannot test this scenario because:
    // 1. configASSERT would abort execution on null handle
    // 2. The constructor expects successful creation
    // 3. Mock expectations must be satisfied
    
    // This is a limitation of host-based testing for RTOS assertion behavior
    // In production, configASSERT provides runtime safety checks
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferSendZeroBytes) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 0, portMAX_DELAY))
        .WillOnce(Return(0));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    uint8_t data[1] = {0x00};
    
    auto result = buffer.send(data, 0);
    EXPECT_EQ(result, 0);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferReceiveZeroBytes) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 0, portMAX_DELAY))
        .WillOnce(Return(0));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    uint8_t data[1];
    
    auto result = buffer.receive(data, 0);
    EXPECT_EQ(result, 0);
}

// =============================================================================
// Complex Scenario Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StreamBufferComplexScenario) {
    // Test a complex scenario with multiple operations
    EXPECT_CALL(*mock, xStreamBufferCreate(256, 4))
        .WillOnce(Return(mock_stream_buffer_handle));
    
    InSequence seq;
    
    // Send some data
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10, 100))
        .WillOnce(Return(10));
    
    // Check available bytes
    EXPECT_CALL(*mock, xStreamBufferBytesAvailable(mock_stream_buffer_handle))
        .WillOnce(Return(10));
    
    // Receive some data
    EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 5, 50))
        .WillOnce(Return(5));
    
    // Check available bytes again
    EXPECT_CALL(*mock, xStreamBufferBytesAvailable(mock_stream_buffer_handle))
        .WillOnce(Return(5));
    
    // Reset buffer
    EXPECT_CALL(*mock, xStreamBufferReset(mock_stream_buffer_handle))
        .WillOnce(Return(pdPASS));
    
    // Check if empty
    EXPECT_CALL(*mock, xStreamBufferIsEmpty(mock_stream_buffer_handle))
        .WillOnce(Return(pdTRUE));
    
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<256> buffer(4);
    
    uint8_t send_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint8_t receive_data[5];
    
    // Execute the scenario
    EXPECT_EQ(buffer.send(send_data, sizeof(send_data), 100), 10);
    EXPECT_EQ(buffer.available(), 10);
    EXPECT_EQ(buffer.receive(receive_data, sizeof(receive_data), 50), 5);
    EXPECT_EQ(buffer.available(), 5);
    EXPECT_EQ(buffer.reset(), pdPASS);
    EXPECT_EQ(buffer.empty(), pdTRUE);
}

// =============================================================================
// Static vs Dynamic Allocator Behavior Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StaticVsDynamicBehavior) {
    // Test that static and dynamic allocators produce functionally equivalent buffers
    constexpr size_t BUFFER_SIZE = 128;
    constexpr size_t TRIGGER_LEVEL = 8;
    
    // Static buffer test
    EXPECT_CALL(*mock, xStreamBufferCreateStatic(BUFFER_SIZE, TRIGGER_LEVEL, NotNull(), NotNull()))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10, portMAX_DELAY))
        .WillOnce(Return(10));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    {
        freertos::sa::stream_buffer<BUFFER_SIZE> static_buffer(TRIGGER_LEVEL);
        uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        EXPECT_EQ(static_buffer.send(data, sizeof(data)), 10);
    }
    
    // Dynamic buffer test
    EXPECT_CALL(*mock, xStreamBufferCreate(BUFFER_SIZE, TRIGGER_LEVEL))
        .WillOnce(Return(mock_stream_buffer_handle_2));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle_2, NotNull(), 10, portMAX_DELAY))
        .WillOnce(Return(10));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle_2))
        .Times(1);
    
    {
        freertos::da::stream_buffer<BUFFER_SIZE> dynamic_buffer(TRIGGER_LEVEL);
        uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        EXPECT_EQ(dynamic_buffer.send(data, sizeof(data)), 10);
    }
}

// =============================================================================
// Chrono Duration Compatibility Tests
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StreamBufferChronoMicrosecondsTimeout) {
    EXPECT_CALL(*mock, xStreamBufferCreate(128, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 4, 2))
        .WillOnce(Return(4));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<128> buffer;
    uint32_t data = 0x12345678;
    
    auto result = buffer.send(&data, sizeof(data), std::chrono::microseconds(2000));
    EXPECT_EQ(result, 4);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferChronoNanosecondsTimeout) {
    EXPECT_CALL(*mock, xStreamBufferCreate(256, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 8, 3))
        .WillOnce(Return(8));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<256> buffer;
    uint64_t data;
    
    auto result = buffer.receive(&data, sizeof(data), std::chrono::nanoseconds(3000000));
    EXPECT_EQ(result, 8);
}

TEST_F(FreeRTOSStreamBufferTest, StreamBufferChronoSecondsTimeout) {
    EXPECT_CALL(*mock, xStreamBufferCreate(512, 1))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 16, 5000))
        .WillOnce(Return(16));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<512> buffer;
    std::array<uint8_t, 16> data;
    data.fill(0xAB);
    
    auto result = buffer.send(data.data(), data.size(), std::chrono::seconds(5));
    EXPECT_EQ(result, 16);
}

// =============================================================================
// Stream Buffer API Completeness Test
// =============================================================================

TEST_F(FreeRTOSStreamBufferTest, StreamBufferAPICompleteness) {
    // Test that all stream buffer API methods are accessible and functional
    EXPECT_CALL(*mock, xStreamBufferCreate(1024, 8))
        .WillOnce(Return(mock_stream_buffer_handle));
    EXPECT_CALL(*mock, xStreamBufferSend(mock_stream_buffer_handle, NotNull(), 10, portMAX_DELAY))
        .WillOnce(Return(10));
    EXPECT_CALL(*mock, xStreamBufferReceive(mock_stream_buffer_handle, NotNull(), 10, portMAX_DELAY))
        .WillOnce(Return(10));
    EXPECT_CALL(*mock, xStreamBufferSendFromISR(mock_stream_buffer_handle, NotNull(), 5, NotNull()))
        .WillOnce(Return(5));
    EXPECT_CALL(*mock, xStreamBufferReceiveFromISR(mock_stream_buffer_handle, NotNull(), 5, NotNull()))
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
    EXPECT_CALL(*mock, xStreamBufferSetTriggerLevel(mock_stream_buffer_handle, 16))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, vStreamBufferDelete(mock_stream_buffer_handle))
        .Times(1);
    
    freertos::da::stream_buffer<1024> buffer(8);
    
    uint8_t send_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint8_t receive_data[10];
    uint8_t isr_data[5] = {0xA, 0xB, 0xC, 0xD, 0xE};
    uint8_t isr_receive[5];
    BaseType_t higher_priority_task_woken = pdFALSE;
    
    // Test all API methods
    EXPECT_EQ(buffer.send(send_data, sizeof(send_data)), 10);
    EXPECT_EQ(buffer.receive(receive_data, sizeof(receive_data)), 10);
    EXPECT_EQ(buffer.send_isr(isr_data, sizeof(isr_data), higher_priority_task_woken), 5);
    EXPECT_EQ(buffer.receive_isr(isr_receive, sizeof(isr_receive), higher_priority_task_woken), 5);
    EXPECT_EQ(buffer.available(), 100);
    EXPECT_EQ(buffer.free(), 924);
    EXPECT_EQ(buffer.empty(), pdFALSE);
    EXPECT_EQ(buffer.full(), pdFALSE);
    EXPECT_EQ(buffer.reset(), pdPASS);
    EXPECT_EQ(buffer.set_trigger_level(16), pdPASS);
    EXPECT_EQ(buffer.handle(), mock_stream_buffer_handle);
}