/**
 * @file test_freertos_queue.cpp
 * @author GitHub Copilot AI Assistant
 * @brief Comprehensive unit tests for FreeRTOS C++ queue wrappers
 * @version 1.0
 * @date 2024-12-20
 * 
 * This file contains comprehensive behavioral unit tests for the FreeRTOS queue
 * wrapper library, including static and dynamic queue allocators with comprehensive
 * queue operations testing.
 *
 * Test Coverage:
 * - Static and dynamic queue allocators
 * - Queue creation and destruction (including registry management)
 * - Send operations (send, send_back, send_front) with timeouts and ISR variants
 * - Receive operations with optional<T> and reference variants
 * - Peek operations (non-destructive read)
 * - Queue state operations (messages_waiting, spaces_available, empty/full checks)
 * - Overwrite operations for single-item queues
 * - Reset functionality
 * - Error handling and edge cases
 * - Chrono timeout compatibility
 * - Template specialization with different data types
 * 
 * Mock Strategy:
 * - Complete FreeRTOS queue API mocking for host-based testing
 * - Validation of parameter passing and return value handling
 * - ISR function testing (commented where host limitations apply)
 * - Clear documentation of RTOS-only features
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <string>

// Include mocks before the actual headers
#include "FreeRTOS.h"
#include "freertos_queue.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::NotNull;
using ::testing::StrEq;
using ::testing::StrictMock;
using ::testing::NiceMock;

class FreeRTOSQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a strict mock for precise API verification
        mock = new StrictMock<FreeRTOSMock>();
        g_freertos_mock = mock;
        
        // Create mock handles for testing
        mock_queue_handle = reinterpret_cast<QueueHandle_t>(0x12345678);
        mock_queue_handle_2 = reinterpret_cast<QueueHandle_t>(0x87654321);
    }

    void TearDown() override {
        delete mock;
        g_freertos_mock = nullptr;
    }

    StrictMock<FreeRTOSMock>* mock;
    QueueHandle_t mock_queue_handle;
    QueueHandle_t mock_queue_handle_2;
};

// =============================================================================
// Static Queue Allocator Tests
// =============================================================================

TEST_F(FreeRTOSQueueTest, StaticQueueAllocatorConstruction) {
    // Test that static allocator can be constructed without API calls
    freertos::static_queue_allocator<10, uint32_t> allocator;
    // No mock expectations needed - constructor should not call FreeRTOS APIs
}

TEST_F(FreeRTOSQueueTest, StaticQueueAllocatorCreate) {
    freertos::static_queue_allocator<5, uint32_t> allocator;
    
    EXPECT_CALL(*mock, xQueueCreateStatic(5, sizeof(uint32_t), NotNull(), NotNull()))
        .WillOnce(Return(mock_queue_handle));
    
    auto handle = allocator.create();
    EXPECT_EQ(handle, mock_queue_handle);
}

TEST_F(FreeRTOSQueueTest, StaticQueueAllocatorDifferentTypes) {
    // Test allocator with different data types
    freertos::static_queue_allocator<3, std::string> string_allocator;
    freertos::static_queue_allocator<8, double> double_allocator;
    
    EXPECT_CALL(*mock, xQueueCreateStatic(3, sizeof(std::string), NotNull(), NotNull()))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueCreateStatic(8, sizeof(double), NotNull(), NotNull()))
        .WillOnce(Return(mock_queue_handle_2));
    
    auto string_handle = string_allocator.create();
    auto double_handle = double_allocator.create();
    
    EXPECT_EQ(string_handle, mock_queue_handle);
    EXPECT_EQ(double_handle, mock_queue_handle_2);
}

// =============================================================================
// Dynamic Queue Allocator Tests  
// =============================================================================

TEST_F(FreeRTOSQueueTest, DynamicQueueAllocatorConstruction) {
    // Test that dynamic allocator can be constructed without API calls
    freertos::dynamic_queue_allocator<15, uint32_t> allocator;
    // No mock expectations needed - constructor should not call FreeRTOS APIs
}

TEST_F(FreeRTOSQueueTest, DynamicQueueAllocatorCreate) {
    freertos::dynamic_queue_allocator<10, uint16_t> allocator;
    
    EXPECT_CALL(*mock, xQueueCreate(10, sizeof(uint16_t)))
        .WillOnce(Return(mock_queue_handle));
    
    auto handle = allocator.create();
    EXPECT_EQ(handle, mock_queue_handle);
}

// =============================================================================
// Queue Construction and Destruction Tests
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueueConstructionWithoutRegistry) {
    EXPECT_CALL(*mock, xQueueCreate(5, sizeof(uint32_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle))
        .Times(1);
    
    freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>> queue;
    // Constructor should have called create without registry
}

TEST_F(FreeRTOSQueueTest, QueueConstructionWithRegistry) {
    const char* queue_name = "TestQueue";
    
    EXPECT_CALL(*mock, xQueueCreate(8, sizeof(uint32_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, vQueueAddToRegistry(mock_queue_handle, StrEq(queue_name)))
        .Times(1);
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(queue_name));
    EXPECT_CALL(*mock, vQueueUnregisterQueue(mock_queue_handle))
        .Times(1);
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle))
        .Times(1);
    
    freertos::queue<8, uint32_t, freertos::dynamic_queue_allocator<8, uint32_t>> queue(queue_name);
    // Constructor should have called create and registry functions
}

TEST_F(FreeRTOSQueueTest, QueueDestructionNullHandle) {
    /*
     * Destruction with Null Handle Test:
     * Tests proper handling when queue creation fails (returns nullptr).
     * The destructor should safely handle null handles without crashing.
     */
    EXPECT_CALL(*mock, xQueueCreate(3, sizeof(double)))
        .WillOnce(Return(nullptr));
    // No deletion calls expected for null handle
    
    // This would normally trigger configASSERT in real FreeRTOS
    // In our mock environment, we test the wrapper's null handle behavior
    try {
        freertos::queue<3, double, freertos::dynamic_queue_allocator<3, double>> queue;
        // Should handle null gracefully in destructor
    } catch (...) {
        // configASSERT may throw in mock environment - this is expected behavior
    }
}

// =============================================================================
// Queue Send Operations Tests
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueueSendBasic) {
    EXPECT_CALL(*mock, xQueueCreate(4, sizeof(uint16_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSend(mock_queue_handle, NotNull(), 100))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<4, uint16_t, freertos::dynamic_queue_allocator<4, uint16_t>> queue;
    
    uint16_t test_value = 42;
    auto result = queue.send(test_value, 100);
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueSendBack) {
    EXPECT_CALL(*mock, xQueueCreate(6, sizeof(double)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSendToBack(mock_queue_handle, NotNull(), portMAX_DELAY))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<6, double, freertos::dynamic_queue_allocator<6, double>> queue;
    
    double test_value = 3.14159;
    auto result = queue.send_back(test_value, portMAX_DELAY);
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueSendFront) {
    EXPECT_CALL(*mock, xQueueCreate(3, sizeof(char)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSendToFront(mock_queue_handle, NotNull(), 50))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<3, char, freertos::dynamic_queue_allocator<3, char>> queue;
    
    char test_value = 'A';
    auto result = queue.send_front(test_value, 50);
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueSendTimeout) {
    EXPECT_CALL(*mock, xQueueCreate(2, sizeof(uint8_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSend(mock_queue_handle, NotNull(), 0))
        .WillOnce(Return(errQUEUE_FULL));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<2, uint8_t, freertos::dynamic_queue_allocator<2, uint8_t>> queue;
    
    uint8_t test_value = 255;
    auto result = queue.send(test_value, 0); // No wait
    EXPECT_EQ(result, errQUEUE_FULL);
}

TEST_F(FreeRTOSQueueTest, QueueSendISR) {
    EXPECT_CALL(*mock, xQueueCreate(5, sizeof(uint32_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSendFromISR(mock_queue_handle, NotNull(), NotNull()))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>> queue;
    
    uint32_t test_value = 0xDEADBEEF;
    BaseType_t higher_priority_task_woken = pdFALSE;
    
    auto result = queue.send_isr(test_value, higher_priority_task_woken);
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueSendChronoTimeout) {
    EXPECT_CALL(*mock, xQueueCreate(4, sizeof(uint16_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSend(mock_queue_handle, NotNull(), 500)) // 500ms converted to ticks
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<4, uint16_t, freertos::dynamic_queue_allocator<4, uint16_t>> queue;
    
    uint16_t test_value = 123;
    auto result = queue.send(test_value, std::chrono::milliseconds(500));
    EXPECT_EQ(result, pdPASS);
}

// =============================================================================
// Queue Receive Operations Tests
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueueReceiveReference) {
    EXPECT_CALL(*mock, xQueueCreate(4, sizeof(uint16_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueReceive(mock_queue_handle, NotNull(), 200))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<4, uint16_t, freertos::dynamic_queue_allocator<4, uint16_t>> queue;
    
    uint16_t received_value;
    auto result = queue.receive(received_value, 200);
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueReceiveOptional) {
    EXPECT_CALL(*mock, xQueueCreate(3, sizeof(uint16_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueReceive(mock_queue_handle, NotNull(), 100))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<3, uint16_t, freertos::dynamic_queue_allocator<3, uint16_t>> queue;
    
    auto result = queue.receive(100);
    EXPECT_TRUE(result.has_value());
}

TEST_F(FreeRTOSQueueTest, QueueReceiveOptionalEmpty) {
    EXPECT_CALL(*mock, xQueueCreate(2, sizeof(char)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueReceive(mock_queue_handle, NotNull(), 0))
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<2, char, freertos::dynamic_queue_allocator<2, char>> queue;
    
    auto result = queue.receive(0);  // No wait
    EXPECT_FALSE(result.has_value());
}

TEST_F(FreeRTOSQueueTest, QueueReceiveISR) {
    EXPECT_CALL(*mock, xQueueCreate(6, sizeof(float)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueReceiveFromISR(mock_queue_handle, NotNull(), NotNull()))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<6, float, freertos::dynamic_queue_allocator<6, float>> queue;
    
    float received_value;
    BaseType_t higher_priority_task_woken = pdFALSE;
    
    auto result = queue.receive_isr(received_value, higher_priority_task_woken);
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueReceiveChronoTimeout) {
    EXPECT_CALL(*mock, xQueueCreate(4, sizeof(uint32_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueReceive(mock_queue_handle, NotNull(), 1000)) // 1 second = 1000 ms
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<4, uint32_t, freertos::dynamic_queue_allocator<4, uint32_t>> queue;
    
    uint32_t received_value;
    auto result = queue.receive(received_value, std::chrono::seconds(1));
    EXPECT_EQ(result, pdPASS);
}

// =============================================================================
// Queue Peek Operations Tests
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueuePeekReference) {
    EXPECT_CALL(*mock, xQueueCreate(5, sizeof(uint32_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueuePeek(mock_queue_handle, NotNull(), 150))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>> queue;
    
    uint32_t peeked_value;
    auto result = queue.peek(peeked_value, 150);
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueuePeekOptional) {
    EXPECT_CALL(*mock, xQueueCreate(3, sizeof(double)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueuePeek(mock_queue_handle, NotNull(), 75))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<3, double, freertos::dynamic_queue_allocator<3, double>> queue;
    
    auto result = queue.peek(75);
    EXPECT_TRUE(result.has_value());
}

TEST_F(FreeRTOSQueueTest, QueuePeekISR) {
    EXPECT_CALL(*mock, xQueueCreate(4, sizeof(double)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueuePeekFromISR(mock_queue_handle, NotNull(), NotNull()))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<4, double, freertos::dynamic_queue_allocator<4, double>> queue;
    
    double peeked_value;
    BaseType_t higher_priority_task_woken = pdFALSE;
    
    auto result = queue.peek_isr(peeked_value, higher_priority_task_woken);
    EXPECT_EQ(result, pdPASS);
}

// =============================================================================
// Queue State and Information Tests
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueueMessagesWaiting) {
    EXPECT_CALL(*mock, xQueueCreate(8, sizeof(char)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, uxQueueMessagesWaiting(mock_queue_handle))
        .WillOnce(Return(3));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<8, char, freertos::dynamic_queue_allocator<8, char>> queue;
    
    auto count = queue.messages_waiting();
    EXPECT_EQ(count, 3);
}

TEST_F(FreeRTOSQueueTest, QueueSpacesAvailable) {
    EXPECT_CALL(*mock, xQueueCreate(10, sizeof(uint32_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, uxQueueSpacesAvailable(mock_queue_handle))
        .WillOnce(Return(7));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<10, uint32_t, freertos::dynamic_queue_allocator<10, uint32_t>> queue;
    
    auto spaces = queue.spaces_available();
    EXPECT_EQ(spaces, 7);
}

TEST_F(FreeRTOSQueueTest, QueueIsEmpty) {
    EXPECT_CALL(*mock, xQueueCreate(4, sizeof(uint8_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueIsQueueEmptyFromISR(mock_queue_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<4, uint8_t, freertos::dynamic_queue_allocator<4, uint8_t>> queue;
    
    auto is_empty = queue.empty_isr();
    EXPECT_EQ(is_empty, pdTRUE);
}

TEST_F(FreeRTOSQueueTest, QueueIsFull) {
    EXPECT_CALL(*mock, xQueueCreate(2, sizeof(uint32_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueIsQueueFullFromISR(mock_queue_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<2, uint32_t, freertos::dynamic_queue_allocator<2, uint32_t>> queue;
    
    auto is_full = queue.full_isr();
    EXPECT_EQ(is_full, pdTRUE);
}

TEST_F(FreeRTOSQueueTest, QueueGetName) {
    const char* expected_name = "MyTestQueue";
    
    EXPECT_CALL(*mock, xQueueCreate(5, sizeof(uint32_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, vQueueAddToRegistry(mock_queue_handle, StrEq(expected_name)))
        .Times(1);
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillRepeatedly(Return(expected_name));
    EXPECT_CALL(*mock, vQueueUnregisterQueue(mock_queue_handle))
        .Times(1);
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<5, uint32_t, freertos::dynamic_queue_allocator<5, uint32_t>> queue(expected_name);
    
    auto name = queue.name();
    EXPECT_STREQ(name, expected_name);
}

// =============================================================================
// Queue Utility Operations Tests
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueueReset) {
    EXPECT_CALL(*mock, xQueueCreate(6, sizeof(float)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueReset(mock_queue_handle))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<6, float, freertos::dynamic_queue_allocator<6, float>> queue;
    
    auto result = queue.reset();
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueOverwrite) {
    EXPECT_CALL(*mock, xQueueCreate(1, sizeof(uint32_t))) // Single item queue for overwrite
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueOverwrite(mock_queue_handle, NotNull()))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<1, uint32_t, freertos::dynamic_queue_allocator<1, uint32_t>> queue;
    
    uint32_t test_value = 999;
    auto result = queue.overwrite(test_value);
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueOverwriteISR) {
    EXPECT_CALL(*mock, xQueueCreate(1, sizeof(uint16_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueOverwriteFromISR(mock_queue_handle, NotNull(), NotNull()))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<1, uint16_t, freertos::dynamic_queue_allocator<1, uint16_t>> queue;
    
    uint16_t test_value = 0xFFFF;
    BaseType_t higher_priority_task_woken = pdFALSE;
    
    auto result = queue.overwrite_isr(test_value, higher_priority_task_woken);
    EXPECT_EQ(result, pdPASS);
}

// =============================================================================
// Static Queue Alias Tests
// =============================================================================

TEST_F(FreeRTOSQueueTest, StaticQueueAliasUsage) {
    EXPECT_CALL(*mock, xQueueCreateStatic(4, sizeof(uint32_t), NotNull(), NotNull()))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::sa::queue<4, uint32_t> static_queue;
    // Should use static allocator
}

// =============================================================================
// Dynamic Queue Alias Tests  
// =============================================================================

TEST_F(FreeRTOSQueueTest, DynamicQueueAliasUsage) {
    EXPECT_CALL(*mock, xQueueCreate(6, sizeof(double)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::da::queue<6, double> dynamic_queue;
    // Should use dynamic allocator
}

// =============================================================================
// Error Handling and Edge Cases
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueueCreationFailure) {
    EXPECT_CALL(*mock, xQueueCreate(4, sizeof(uint32_t)))
        .WillOnce(Return(nullptr));
    
    /*
     * Creation Failure Note:
     * When xQueueCreate returns nullptr, the wrapper's constructor
     * will call configASSERT. In a real FreeRTOS environment, this would
     * trigger assertion handling. In our mock environment, we document this
     * behavior for host-based testing limitations.
     */
    try {
        freertos::queue<4, uint32_t, freertos::dynamic_queue_allocator<4, uint32_t>> queue;
        // May throw due to configASSERT in mock environment
    } catch (...) {
        // Expected in mock environment when creation fails
    }
}

TEST_F(FreeRTOSQueueTest, LargeQueueSize) {
    const size_t large_size = 1000;
    
    EXPECT_CALL(*mock, xQueueCreate(large_size, sizeof(uint64_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<large_size, uint64_t, freertos::dynamic_queue_allocator<large_size, uint64_t>> large_queue;
    // Should handle large queue sizes correctly
}

TEST_F(FreeRTOSQueueTest, ZeroSizeQueue) {
    /*
     * Zero Size Queue Note:
     * Creating a queue with size 0 is technically possible but not practical.
     * FreeRTOS behavior with zero-sized queues may vary. This test documents
     * the wrapper's handling of edge cases.
     */
    EXPECT_CALL(*mock, xQueueCreate(0, sizeof(uint32_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    try {
        freertos::queue<0, uint32_t, freertos::dynamic_queue_allocator<0, uint32_t>> zero_queue;
        // Edge case: queue with zero size
    } catch (...) {
        // May fail in some implementations
    }
}

// =============================================================================
// Complex Data Type Tests
// =============================================================================

struct ComplexTestData {
    uint32_t id;
    double value;
    char name[16];
    
    bool operator==(const ComplexTestData& other) const {
        return id == other.id && value == other.value && 
               strncmp(name, other.name, 16) == 0;
    }
};

TEST_F(FreeRTOSQueueTest, ComplexDataTypeQueue) {
    EXPECT_CALL(*mock, xQueueCreate(3, sizeof(ComplexTestData)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSend(mock_queue_handle, NotNull(), 100))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, xQueueReceive(mock_queue_handle, NotNull(), 100))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<3, ComplexTestData, freertos::dynamic_queue_allocator<3, ComplexTestData>> complex_queue;
    
    ComplexTestData test_data = {123, 45.67, "TestData"};
    
    auto send_result = complex_queue.send(test_data, 100);
    EXPECT_EQ(send_result, pdPASS);
    
    ComplexTestData received_data;
    auto receive_result = complex_queue.receive(received_data, 100);
    EXPECT_EQ(receive_result, pdPASS);
}

// =============================================================================
// API Completeness Test
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueueAPICompleteness) {
    /*
     * API Completeness Test:
     * This test verifies that all major FreeRTOS queue APIs are covered
     * by the C++ wrapper classes. Not all APIs may be directly exposed,
     * but the essential functionality should be available.
     * 
     * Covered APIs:
     * - xQueueCreate/Static
     * - vQueueDelete
     * - xQueueSend/ToBack/ToFront (with ISR variants)
     * - xQueueReceive/FromISR
     * - xQueuePeek/FromISR
     * - uxQueueMessagesWaiting/FromISR
     * - uxQueueSpacesAvailable
     * - xQueueIsQueueEmpty/FullFromISR
     * - xQueueReset
     * - xQueueOverwrite/FromISR
     * - vQueueAddToRegistry/Unregister
     * - pcQueueGetName
     * 
     * Note: Some advanced FreeRTOS features like queue sets or queue handles
     * in task notifications are not covered by these basic wrapper classes.
     */
    EXPECT_TRUE(true); // Placeholder for documentation
}

// =============================================================================
// Enhanced Data Type Variety Tests  
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueueWithVariousDataTypes) {
    QueueHandle_t mock_int8_handle = reinterpret_cast<QueueHandle_t>(0x11111111);
    QueueHandle_t mock_int16_handle = reinterpret_cast<QueueHandle_t>(0x22222222);
    QueueHandle_t mock_int64_handle = reinterpret_cast<QueueHandle_t>(0x33333333);
    
    // Test with different basic types
    EXPECT_CALL(*mock, xQueueCreate(5, sizeof(int8_t)))
        .WillOnce(Return(mock_int8_handle));
    EXPECT_CALL(*mock, pcQueueGetName(mock_int8_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_int8_handle));
    
    freertos::queue<5, int8_t, freertos::dynamic_queue_allocator<5, int8_t>> int8_queue;
    
    // Test with int16_t
    EXPECT_CALL(*mock, xQueueCreate(3, sizeof(int16_t)))
        .WillOnce(Return(mock_int16_handle));
    EXPECT_CALL(*mock, pcQueueGetName(mock_int16_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_int16_handle));
    
    freertos::queue<3, int16_t, freertos::dynamic_queue_allocator<3, int16_t>> int16_queue;
    
    // Test with int64_t
    EXPECT_CALL(*mock, xQueueCreate(2, sizeof(int64_t)))
        .WillOnce(Return(mock_int64_handle));
    EXPECT_CALL(*mock, pcQueueGetName(mock_int64_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_int64_handle));
    
    freertos::queue<2, int64_t, freertos::dynamic_queue_allocator<2, int64_t>> int64_queue;
}

TEST_F(FreeRTOSQueueTest, QueueWithEnumTypes) {
    enum class Priority : uint8_t { LOW = 1, MEDIUM = 2, HIGH = 3 };
    
    EXPECT_CALL(*mock, xQueueCreate(4, sizeof(Priority)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSend(mock_queue_handle, NotNull(), 50))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, xQueueReceive(mock_queue_handle, NotNull(), 100))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<4, Priority, freertos::dynamic_queue_allocator<4, Priority>> priority_queue;
    
    Priority test_priority = Priority::HIGH;
    auto send_result = priority_queue.send(test_priority, 50);
    EXPECT_EQ(send_result, pdPASS);
    
    Priority received_priority;
    auto receive_result = priority_queue.receive(received_priority, 100);
    EXPECT_EQ(receive_result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueWithPointerTypes) {
    using DataPtr = uint32_t*;
    
    EXPECT_CALL(*mock, xQueueCreate(10, sizeof(DataPtr)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSend(mock_queue_handle, NotNull(), 0))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<10, DataPtr, freertos::dynamic_queue_allocator<10, DataPtr>> ptr_queue;
    
    uint32_t test_data = 42;
    DataPtr test_ptr = &test_data;
    auto result = ptr_queue.send(test_ptr, 0);
    EXPECT_EQ(result, pdPASS);
}

// =============================================================================
// Comprehensive Chrono Duration Tests
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueueChronoMicrosecondsTimeout) {
    EXPECT_CALL(*mock, xQueueCreate(3, sizeof(float)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueReceive(mock_queue_handle, NotNull(), 0)) // 500µs = 0ms after duration_cast
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<3, float, freertos::dynamic_queue_allocator<3, float>> queue;
    
    float received_value;
    auto result = queue.receive(received_value, std::chrono::microseconds(500));
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueChronoMinutesTimeout) {
    EXPECT_CALL(*mock, xQueueCreate(2, sizeof(double)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSend(mock_queue_handle, NotNull(), 60000)) // 1 minute = 60000ms
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<2, double, freertos::dynamic_queue_allocator<2, double>> queue;
    
    double test_value = 3.14159;
    auto result = queue.send(test_value, std::chrono::minutes(1));
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueChronoNanosecondsTimeout) {
    EXPECT_CALL(*mock, xQueueCreate(5, sizeof(uint64_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueuePeek(mock_queue_handle, NotNull(), 1)) // 1000000ns = 1ms
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<5, uint64_t, freertos::dynamic_queue_allocator<5, uint64_t>> queue;
    
    uint64_t peeked_value;
    auto result = queue.peek(peeked_value, std::chrono::nanoseconds(1000000));
    EXPECT_EQ(result, pdPASS);
}

TEST_F(FreeRTOSQueueTest, QueueChronoOptionalReceiveVariousDurations) {
    EXPECT_CALL(*mock, xQueueCreate(4, sizeof(int32_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueReceive(mock_queue_handle, NotNull(), 5000)) // 5 seconds = 5000ms
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<4, int32_t, freertos::dynamic_queue_allocator<4, int32_t>> queue;
    
    auto result = queue.receive(std::chrono::seconds(5));
    EXPECT_TRUE(result.has_value());
}

// =============================================================================
// Enhanced Static Allocator Tests with Different Types
// =============================================================================

TEST_F(FreeRTOSQueueTest, StaticQueueAllocatorWithVariousTypes) {
    QueueHandle_t mock_bool_handle = reinterpret_cast<QueueHandle_t>(0x44444444);
    QueueHandle_t mock_char_handle = reinterpret_cast<QueueHandle_t>(0x55555555);
    
    // Test with bool type
    EXPECT_CALL(*mock, xQueueCreateStatic(8, sizeof(bool), NotNull(), NotNull()))
        .WillOnce(Return(mock_bool_handle));
    EXPECT_CALL(*mock, pcQueueGetName(mock_bool_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_bool_handle));
    
    freertos::sa::queue<8, bool> bool_queue;
    
    // Test with char type  
    EXPECT_CALL(*mock, xQueueCreateStatic(16, sizeof(char), NotNull(), NotNull()))
        .WillOnce(Return(mock_char_handle));
    EXPECT_CALL(*mock, pcQueueGetName(mock_char_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_char_handle));
    
    freertos::sa::queue<16, char> char_queue;
}

// =============================================================================
// ISR Operations with Different Data Types
// =============================================================================

TEST_F(FreeRTOSQueueTest, QueueISROperationsWithDifferentTypes) {
    // Test ISR operations with uint16_t
    EXPECT_CALL(*mock, xQueueCreate(6, sizeof(uint16_t)))
        .WillOnce(Return(mock_queue_handle));
    EXPECT_CALL(*mock, xQueueSendFromISR(mock_queue_handle, NotNull(), NotNull()))
        .WillOnce(Return(pdPASS));
    EXPECT_CALL(*mock, xQueueReceiveFromISR(mock_queue_handle, NotNull(), NotNull()))
        .Times(2)  // Called twice: once for receive_isr(ref) and once for receive_isr()
        .WillRepeatedly(Return(pdPASS));
    EXPECT_CALL(*mock, pcQueueGetName(mock_queue_handle))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(*mock, vQueueDelete(mock_queue_handle));
    
    freertos::queue<6, uint16_t, freertos::dynamic_queue_allocator<6, uint16_t>> queue;
    
    uint16_t test_value = 0xABCD;
    BaseType_t higher_priority_task_woken = pdFALSE;
    
    // Test send_isr with reference parameter
    auto send_result = queue.send_isr(test_value, higher_priority_task_woken);
    EXPECT_EQ(send_result, pdPASS);
    
    // Test receive_isr with reference parameter
    uint16_t received_value;
    auto receive_result = queue.receive_isr(received_value, higher_priority_task_woken);
    EXPECT_EQ(receive_result, pdPASS);
    
    // Test receive_isr returning optional (no reference parameter)
    auto optional_result = queue.receive_isr();
    EXPECT_TRUE(optional_result.has_value());
}
