/**
 * @file test_freertos_message_buffer.cpp
 * @brief Comprehensive unit tests for FreeRTOS C++ message buffer wrappers
 * 
 * This file provides comprehensive behavioral unit tests for the message buffer modules,
 * including static and dynamic message buffer allocators.
 * 
 * Tests cover:
 * - Correct construction, allocation, and deallocation using static and dynamic allocators
 * - Typical usage patterns: creation, sending, receiving, and reset operations
 * - Correct operation of message buffer state checks (available, empty, full)
 * - Error and edge cases, including buffer full/empty conditions, timeouts, invalid params
 * - Chrono duration compatibility for timeout parameters
 * - Use stubs/mocks for FreeRTOS internals to enable host-based testing
 * 
 * RTOS-only features documented in comments:
 * - Actual timeout behavior depends on FreeRTOS scheduler (host testing uses mock return values)
 * - ISR variants of functions are not testable in host environment but are mocked for completeness
 * - Real message ordering and thread safety depend on FreeRTOS kernel (host testing simulates)
 * - Actual memory layout and buffer overflow protection are RTOS-specific features
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <memory>
#include <array>

// Include mocks first to set up FreeRTOS environment
#include "FreeRTOS.h"

// Include the actual FreeRTOS message buffer wrapper
#include "../include/freertos_message_buffer.hpp"

using namespace testing;
using namespace freertos;
using namespace std::chrono_literals;

class FreeRTOSMessageBufferTest : public Test {
protected:
    void SetUp() override {
        mock = std::make_unique<StrictMock<FreeRTOSMock>>();
        g_freertos_mock = mock.get();
        
        // Common mock message buffer handle for testing
        mock_message_buffer_handle = reinterpret_cast<MessageBufferHandle_t>(0x12345678);
    }
    
    void TearDown() override {
        g_freertos_mock = nullptr;
        mock.reset();
    }
    
    std::unique_ptr<StrictMock<FreeRTOSMock>> mock;
    MessageBufferHandle_t mock_message_buffer_handle;
};

// =============================================================================
// STATIC MESSAGE BUFFER ALLOCATOR TESTS
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, StaticMessageBufferAllocatorConstruction) {
    // Test that static message buffer allocator can be constructed
    static_message_buffer_allocator<512> allocator;
    
    // Test that copy constructor is deleted (should not compile if uncommented)
    // static_message_buffer_allocator<512> copy_allocator(allocator);  // Should not compile
    
    // Test that assignment is deleted (should not compile if uncommented)
    // static_message_buffer_allocator<512> other;
    // other = allocator;  // Should not compile
}

TEST_F(FreeRTOSMessageBufferTest, StaticMessageBufferAllocatorCreate) {
    static_message_buffer_allocator<512> allocator;
    
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(
        512,                    // buffer size
        NotNull(),              // storage area
        NotNull()               // static message buffer
    )).WillOnce(Return(mock_message_buffer_handle));
    
    MessageBufferHandle_t handle = allocator.create();
    
    EXPECT_EQ(handle, mock_message_buffer_handle);
}

TEST_F(FreeRTOSMessageBufferTest, StaticMessageBufferAllocatorCreateFailure) {
    static_message_buffer_allocator<512> allocator;
    
    // Test case where FreeRTOS returns NULL (allocation failure)
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(nullptr));
    
    MessageBufferHandle_t handle = allocator.create();
    
    EXPECT_EQ(handle, nullptr);
}

// =============================================================================
// DYNAMIC MESSAGE BUFFER ALLOCATOR TESTS
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, DynamicMessageBufferAllocatorConstruction) {
    // Test that dynamic message buffer allocator can be constructed
    dynamic_message_buffer_allocator<512> allocator;
    
    // Test that copy constructor is implicitly available for dynamic allocator
    dynamic_message_buffer_allocator<512> copy_allocator(allocator);
    
    // Test that assignment is implicitly available for dynamic allocator
    dynamic_message_buffer_allocator<512> other;
    other = allocator;
}

TEST_F(FreeRTOSMessageBufferTest, DynamicMessageBufferAllocatorCreateSuccess) {
    dynamic_message_buffer_allocator<512> allocator;
    
    EXPECT_CALL(*mock, xMessageBufferCreate(512))
        .WillOnce(Return(mock_message_buffer_handle));
    
    MessageBufferHandle_t handle = allocator.create();
    
    EXPECT_EQ(handle, mock_message_buffer_handle);
}

TEST_F(FreeRTOSMessageBufferTest, DynamicMessageBufferAllocatorCreateFailure) {
    dynamic_message_buffer_allocator<512> allocator;
    
    // Test case where FreeRTOS returns NULL (allocation failure)
    EXPECT_CALL(*mock, xMessageBufferCreate(_))
        .WillOnce(Return(nullptr));
    
    MessageBufferHandle_t handle = allocator.create();
    
    EXPECT_EQ(handle, nullptr);
}

// =============================================================================
// BASIC MESSAGE BUFFER TESTS (STATIC ALLOCATION)
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, StaticMessageBufferConstruction) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(256, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    // Note: Handle is private in the wrapper, testing behavior through operations
}

TEST_F(FreeRTOSMessageBufferTest, StaticMessageBufferDestruction) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
    
    {
        sa::message_buffer<256> test_buffer;
    } // Destructor should call vMessageBufferDelete
}

TEST_F(FreeRTOSMessageBufferTest, StaticMessageBufferDestructionNullHandle) {
    // Test destruction when message buffer creation failed
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(nullptr));
    
    // Should NOT call vMessageBufferDelete for null handle
    EXPECT_CALL(*mock, vMessageBufferDelete(_)).Times(0);
    
    {
        sa::message_buffer<256> test_buffer;
    }
}

// =============================================================================
// BASIC MESSAGE BUFFER TESTS (DYNAMIC ALLOCATION)
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, DynamicMessageBufferConstruction) {
    EXPECT_CALL(*mock, xMessageBufferCreate(512))
        .WillOnce(Return(mock_message_buffer_handle));
    
    da::message_buffer<512> test_buffer;
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, DynamicMessageBufferConstructionFailure) {
    EXPECT_CALL(*mock, xMessageBufferCreate(_))
        .WillOnce(Return(nullptr));
    
    da::message_buffer<512> test_buffer;
    
    // Should NOT call vMessageBufferDelete for null handle
    EXPECT_CALL(*mock, vMessageBufferDelete(_)).Times(0);
}

// =============================================================================
// MESSAGE BUFFER SEND OPERATION TESTS
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, MessageBufferSendSuccess) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    const char test_data[] = "Hello, World!";
    const size_t data_length = strlen(test_data);
    
    EXPECT_CALL(*mock, xMessageBufferSend(
        mock_message_buffer_handle,
        test_data,
        data_length,
        100  // timeout ticks
    )).WillOnce(Return(data_length));
    
    size_t bytes_sent = test_buffer.send(test_data, data_length, 100);
    EXPECT_EQ(bytes_sent, data_length);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferSendTimeout) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    const char test_data[] = "Test message";
    const size_t data_length = strlen(test_data);
    
    // Simulate timeout - buffer full, send returns 0
    EXPECT_CALL(*mock, xMessageBufferSend(
        mock_message_buffer_handle,
        test_data,
        data_length,
        50  // timeout ticks
    )).WillOnce(Return(0));
    
    size_t bytes_sent = test_buffer.send(test_data, data_length, 50);
    EXPECT_EQ(bytes_sent, 0);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferSendWithChrono) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    const uint32_t test_value = 0x12345678;
    
    // Test chrono duration conversion: 100ms should convert to 100 ticks (with portTICK_PERIOD_MS = 1)
    EXPECT_CALL(*mock, xMessageBufferSend(
        mock_message_buffer_handle,
        &test_value,
        sizeof(test_value),
        100  // 100ms converted to ticks
    )).WillOnce(Return(sizeof(test_value)));
    
    size_t bytes_sent = test_buffer.send(&test_value, sizeof(test_value), 100ms);
    EXPECT_EQ(bytes_sent, sizeof(test_value));
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferSendZeroLengthMessage) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    // Test sending zero-length message
    EXPECT_CALL(*mock, xMessageBufferSend(
        mock_message_buffer_handle,
        nullptr,
        0,
        10
    )).WillOnce(Return(0));
    
    size_t bytes_sent = test_buffer.send(nullptr, 0, 10);
    EXPECT_EQ(bytes_sent, 0);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferSendMaxSizeMessage) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<512> test_buffer;
    
    // Test sending maximum size message (buffer size - overhead)
    std::array<uint8_t, 500> large_data;
    large_data.fill(0xAA);
    
    EXPECT_CALL(*mock, xMessageBufferSend(
        mock_message_buffer_handle,
        large_data.data(),
        large_data.size(),
        portMAX_DELAY
    )).WillOnce(Return(large_data.size()));
    
    size_t bytes_sent = test_buffer.send(large_data.data(), large_data.size(), portMAX_DELAY);
    EXPECT_EQ(bytes_sent, large_data.size());
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

// =============================================================================
// MESSAGE BUFFER RECEIVE OPERATION TESTS
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, MessageBufferReceiveSuccess) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    char receive_buffer[64];
    memset(receive_buffer, 0, sizeof(receive_buffer));  // Clear buffer first
    const size_t expected_length = 13;
    
    EXPECT_CALL(*mock, xMessageBufferReceive(
        mock_message_buffer_handle,
        receive_buffer,
        sizeof(receive_buffer),
        100  // timeout ticks
    )).WillOnce(DoAll(
        // Simulate receiving "Hello, World!" message
        Invoke([expected_length](MessageBufferHandle_t, void* pvRxData, size_t, TickType_t) {
            memcpy(pvRxData, "Hello, World!", expected_length);
        }),
        Return(expected_length)
    ));
    
    size_t bytes_received = test_buffer.receive(receive_buffer, sizeof(receive_buffer), 100);
    EXPECT_EQ(bytes_received, expected_length);
    
    // For message buffers, we need to null-terminate manually if treating as string
    receive_buffer[bytes_received] = '\0';
    EXPECT_STREQ(receive_buffer, "Hello, World!");
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferReceiveTimeout) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    char receive_buffer[64];
    
    // Simulate timeout - no message available, receive returns 0
    EXPECT_CALL(*mock, xMessageBufferReceive(
        mock_message_buffer_handle,
        receive_buffer,
        sizeof(receive_buffer),
        50  // timeout ticks
    )).WillOnce(Return(0));
    
    size_t bytes_received = test_buffer.receive(receive_buffer, sizeof(receive_buffer), 50);
    EXPECT_EQ(bytes_received, 0);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferReceiveWithChrono) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    uint32_t receive_value;
    
    // Test chrono duration conversion: 250ms should convert to 250 ticks
    EXPECT_CALL(*mock, xMessageBufferReceive(
        mock_message_buffer_handle,
        &receive_value,
        sizeof(receive_value),
        250  // 250ms converted to ticks
    )).WillOnce(DoAll(
        Invoke([](MessageBufferHandle_t, void* pvRxData, size_t, TickType_t) {
            *static_cast<uint32_t*>(pvRxData) = 0x87654321;
        }),
        Return(sizeof(uint32_t))
    ));
    
    size_t bytes_received = test_buffer.receive(&receive_value, sizeof(receive_value), 250ms);
    EXPECT_EQ(bytes_received, sizeof(uint32_t));
    EXPECT_EQ(receive_value, 0x87654321);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferReceiveBufferTooSmall) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    char small_buffer[5];  // Too small for the incoming message
    
    // In real FreeRTOS, this might truncate or return an error
    // For testing, we simulate truncated receive
    EXPECT_CALL(*mock, xMessageBufferReceive(
        mock_message_buffer_handle,
        small_buffer,
        sizeof(small_buffer),
        100
    )).WillOnce(DoAll(
        Invoke([](MessageBufferHandle_t, void* pvRxData, size_t xBufferLengthBytes, TickType_t) {
            // Simulate truncation - only copy what fits
            memcpy(pvRxData, "Hell", std::min(size_t(4), xBufferLengthBytes));
        }),
        Return(4)  // Return actual bytes that could be received
    ));
    
    size_t bytes_received = test_buffer.receive(small_buffer, sizeof(small_buffer), 100);
    EXPECT_EQ(bytes_received, 4);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

// =============================================================================
// MESSAGE BUFFER STATE OPERATION TESTS
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, MessageBufferAvailableSpace) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    EXPECT_CALL(*mock, xMessageBufferSpaceAvailable(mock_message_buffer_handle))
        .WillOnce(Return(200));
    
    size_t available_space = test_buffer.available();
    EXPECT_EQ(available_space, 200);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferIsEmpty) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    // Test when buffer is empty
    EXPECT_CALL(*mock, xMessageBufferIsEmpty(mock_message_buffer_handle))
        .WillOnce(Return(pdTRUE));
    
    BaseType_t is_empty = test_buffer.empty();
    EXPECT_EQ(is_empty, pdTRUE);
    
    // Test when buffer is not empty
    EXPECT_CALL(*mock, xMessageBufferIsEmpty(mock_message_buffer_handle))
        .WillOnce(Return(pdFALSE));
    
    is_empty = test_buffer.empty();
    EXPECT_EQ(is_empty, pdFALSE);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferIsFull) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    // Test when buffer is full
    EXPECT_CALL(*mock, xMessageBufferIsFull(mock_message_buffer_handle))
        .WillOnce(Return(pdTRUE));
    
    BaseType_t is_full = test_buffer.full();
    EXPECT_EQ(is_full, pdTRUE);
    
    // Test when buffer is not full
    EXPECT_CALL(*mock, xMessageBufferIsFull(mock_message_buffer_handle))
        .WillOnce(Return(pdFALSE));
    
    is_full = test_buffer.full();
    EXPECT_EQ(is_full, pdFALSE);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferReset) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    EXPECT_CALL(*mock, xMessageBufferReset(mock_message_buffer_handle))
        .WillOnce(Return(pdPASS));
    
    BaseType_t reset_result = test_buffer.reset();
    EXPECT_EQ(reset_result, pdPASS);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

// =============================================================================
// ERROR AND EDGE CASE TESTS
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, MessageBufferCreationFailure) {
    // Test behavior when message buffer creation fails
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(nullptr));
    
    // Should NOT call vMessageBufferDelete for null handle
    EXPECT_CALL(*mock, vMessageBufferDelete(_)).Times(0);
    
    sa::message_buffer<256> test_buffer;
    
    // Operations on failed buffer should handle null handle gracefully
    // Note: The actual wrapper may assert or handle this differently in a real RTOS environment
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferZeroSizeBuffer) {
    // Test with very small buffer size
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(4, _, _))
        .WillOnce(Return(nullptr));  // FreeRTOS would likely fail this
    
    sa::message_buffer<4> tiny_buffer;
    
    EXPECT_CALL(*mock, vMessageBufferDelete(_)).Times(0);
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferVeryLargeBuffer) {
    // Test with large buffer size
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(65536, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<65536> large_buffer;
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

// =============================================================================
// CHRONO COMPATIBILITY TESTS
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, MessageBufferChronoMicrosecondsTimeout) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    const char test_data[] = "Microsecond test";
    
    // Test microseconds conversion: 5000μs = 5ms = 5 ticks
    EXPECT_CALL(*mock, xMessageBufferSend(
        mock_message_buffer_handle,
        test_data,
        strlen(test_data),
        5  // 5000μs converted to ticks
    )).WillOnce(Return(strlen(test_data)));
    
    size_t bytes_sent = test_buffer.send(test_data, strlen(test_data), 5000us);
    EXPECT_EQ(bytes_sent, strlen(test_data));
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

TEST_F(FreeRTOSMessageBufferTest, MessageBufferChronoSecondsTimeout) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<256> test_buffer;
    
    char receive_buffer[32];
    
    // Test seconds conversion: 2s = 2000ms = 2000 ticks
    EXPECT_CALL(*mock, xMessageBufferReceive(
        mock_message_buffer_handle,
        receive_buffer,
        sizeof(receive_buffer),
        2000  // 2s converted to ticks
    )).WillOnce(Return(0));  // Timeout
    
    size_t bytes_received = test_buffer.receive(receive_buffer, sizeof(receive_buffer), 2s);
    EXPECT_EQ(bytes_received, 0);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

// =============================================================================
// STATIC VS DYNAMIC BEHAVIOR TESTS
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, StaticVsDynamicBehaviorComparison) {
    // Test that static and dynamic message buffers have the same behavior
    // for operations, differing only in allocation method
    
    // Static buffer test
    {
        EXPECT_CALL(*mock, xMessageBufferCreateStatic(512, _, _))
            .WillOnce(Return(mock_message_buffer_handle));
        
        sa::message_buffer<512> static_buffer;
        
        const char test_message[] = "Static test";
        EXPECT_CALL(*mock, xMessageBufferSend(mock_message_buffer_handle, test_message, strlen(test_message), 100))
            .WillOnce(Return(strlen(test_message)));
        
        size_t static_bytes_sent = static_buffer.send(test_message, strlen(test_message), 100);
        EXPECT_EQ(static_bytes_sent, strlen(test_message));
        
        EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
    }
    
    // Dynamic buffer test with same behavior
    {
        MessageBufferHandle_t dynamic_handle = reinterpret_cast<MessageBufferHandle_t>(0x87654321);
        
        EXPECT_CALL(*mock, xMessageBufferCreate(512))
            .WillOnce(Return(dynamic_handle));
        
        da::message_buffer<512> dynamic_buffer;
        
        const char test_message[] = "Dynamic test";
        EXPECT_CALL(*mock, xMessageBufferSend(dynamic_handle, test_message, strlen(test_message), 100))
            .WillOnce(Return(strlen(test_message)));
        
        size_t dynamic_bytes_sent = dynamic_buffer.send(test_message, strlen(test_message), 100);
        EXPECT_EQ(dynamic_bytes_sent, strlen(test_message));
        
        EXPECT_CALL(*mock, vMessageBufferDelete(dynamic_handle));
    }
}

// =============================================================================
// COMPLEX SCENARIO TESTS
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, MessageBufferComplexSendReceiveScenario) {
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<1024> test_buffer;
    
    // Scenario: Send multiple messages of different sizes, then receive them
    
    // Send first message
    const char msg1[] = "First message";
    EXPECT_CALL(*mock, xMessageBufferSend(mock_message_buffer_handle, msg1, strlen(msg1), 50))
        .WillOnce(Return(strlen(msg1)));
    size_t sent1 = test_buffer.send(msg1, strlen(msg1), 50);
    EXPECT_EQ(sent1, strlen(msg1));
    
    // Check available space after first send
    EXPECT_CALL(*mock, xMessageBufferSpaceAvailable(mock_message_buffer_handle))
        .WillOnce(Return(900));  // Simulated remaining space
    size_t available = test_buffer.available();
    EXPECT_EQ(available, 900);
    
    // Send second message
    const uint32_t msg2_data[] = {0x12345678, 0x9ABCDEF0};
    EXPECT_CALL(*mock, xMessageBufferSend(mock_message_buffer_handle, msg2_data, sizeof(msg2_data), 50))
        .WillOnce(Return(sizeof(msg2_data)));
    size_t sent2 = test_buffer.send(msg2_data, sizeof(msg2_data), 50);
    EXPECT_EQ(sent2, sizeof(msg2_data));
    
    // Check that buffer is not empty
    EXPECT_CALL(*mock, xMessageBufferIsEmpty(mock_message_buffer_handle))
        .WillOnce(Return(pdFALSE));
    EXPECT_EQ(test_buffer.empty(), pdFALSE);
    
    // Receive first message
    char recv_buffer1[32];
    memset(recv_buffer1, 0, sizeof(recv_buffer1));  // Clear buffer first
    EXPECT_CALL(*mock, xMessageBufferReceive(mock_message_buffer_handle, recv_buffer1, sizeof(recv_buffer1), 50))
        .WillOnce(DoAll(
            Invoke([&msg1](MessageBufferHandle_t, void* pvRxData, size_t, TickType_t) {
                memcpy(pvRxData, msg1, strlen(msg1));
            }),
            Return(strlen(msg1))
        ));
    size_t received1 = test_buffer.receive(recv_buffer1, sizeof(recv_buffer1), 50);
    EXPECT_EQ(received1, strlen(msg1));
    
    // Null-terminate for string comparison
    recv_buffer1[received1] = '\0';
    EXPECT_STREQ(recv_buffer1, msg1);
    
    // Receive second message
    uint32_t recv_buffer2[2];
    EXPECT_CALL(*mock, xMessageBufferReceive(mock_message_buffer_handle, recv_buffer2, sizeof(recv_buffer2), 50))
        .WillOnce(DoAll(
            Invoke([&msg2_data](MessageBufferHandle_t, void* pvRxData, size_t, TickType_t) {
                memcpy(pvRxData, msg2_data, sizeof(msg2_data));
            }),
            Return(sizeof(msg2_data))
        ));
    size_t received2 = test_buffer.receive(recv_buffer2, sizeof(recv_buffer2), 50);
    EXPECT_EQ(received2, sizeof(msg2_data));
    EXPECT_EQ(recv_buffer2[0], 0x12345678);
    EXPECT_EQ(recv_buffer2[1], 0x9ABCDEF0);
    
    // Reset buffer
    EXPECT_CALL(*mock, xMessageBufferReset(mock_message_buffer_handle))
        .WillOnce(Return(pdPASS));
    BaseType_t reset_result = test_buffer.reset();
    EXPECT_EQ(reset_result, pdPASS);
    
    // Verify buffer is empty after reset
    EXPECT_CALL(*mock, xMessageBufferIsEmpty(mock_message_buffer_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_EQ(test_buffer.empty(), pdTRUE);
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
}

// =============================================================================
// MESSAGE BUFFER API COMPLETENESS TEST
// =============================================================================

TEST_F(FreeRTOSMessageBufferTest, MessageBufferAPICompleteness) {
    // This test verifies that all major message buffer operations are covered
    // and that the C++ wrapper provides complete functionality
    
    EXPECT_CALL(*mock, xMessageBufferCreateStatic(_, _, _))
        .WillOnce(Return(mock_message_buffer_handle));
    
    sa::message_buffer<512> test_buffer;
    
    // Test all send variants
    const char test_data[] = "API test data";
    
    // Send with TickType_t timeout
    EXPECT_CALL(*mock, xMessageBufferSend(mock_message_buffer_handle, test_data, strlen(test_data), 100))
        .WillOnce(Return(strlen(test_data)));
    test_buffer.send(test_data, strlen(test_data), TickType_t(100));
    
    // Send with chrono timeout
    EXPECT_CALL(*mock, xMessageBufferSend(mock_message_buffer_handle, test_data, strlen(test_data), 200))
        .WillOnce(Return(strlen(test_data)));
    test_buffer.send(test_data, strlen(test_data), 200ms);
    
    // Test all receive variants
    char receive_buffer[32];
    
    // Receive with TickType_t timeout
    EXPECT_CALL(*mock, xMessageBufferReceive(mock_message_buffer_handle, receive_buffer, sizeof(receive_buffer), 150))
        .WillOnce(Return(strlen(test_data)));
    test_buffer.receive(receive_buffer, sizeof(receive_buffer), TickType_t(150));
    
    // Receive with chrono timeout
    EXPECT_CALL(*mock, xMessageBufferReceive(mock_message_buffer_handle, receive_buffer, sizeof(receive_buffer), 300))
        .WillOnce(Return(strlen(test_data)));
    test_buffer.receive(receive_buffer, sizeof(receive_buffer), 300ms);
    
    // Test all state check operations
    EXPECT_CALL(*mock, xMessageBufferSpaceAvailable(mock_message_buffer_handle))
        .WillOnce(Return(256));
    test_buffer.available();
    
    EXPECT_CALL(*mock, xMessageBufferIsEmpty(mock_message_buffer_handle))
        .WillOnce(Return(pdFALSE));
    test_buffer.empty();
    
    EXPECT_CALL(*mock, xMessageBufferIsFull(mock_message_buffer_handle))
        .WillOnce(Return(pdFALSE));
    test_buffer.full();
    
    EXPECT_CALL(*mock, xMessageBufferReset(mock_message_buffer_handle))
        .WillOnce(Return(pdPASS));
    test_buffer.reset();
    
    EXPECT_CALL(*mock, vMessageBufferDelete(mock_message_buffer_handle));
    
    // Note: ISR variants of functions are available in FreeRTOS but not exposed
    // in the C++ wrapper interface as they require different calling contexts
    // (interrupt service routines) which are not applicable to typical C++ usage patterns.
    // Host-based testing cannot properly simulate ISR behavior anyway.
}

// Test entry point
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}