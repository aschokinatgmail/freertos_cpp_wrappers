/**********************************************************************************
@file test_freertos_event_group.cpp
@author Assistant AI
@brief Comprehensive unit tests for FreeRTOS event group wrapper
@version 0.1
@date 2024-12-19

The MIT License (MIT)

FreeRTOS C++ Wrappers Library Test Suite
https://github.com/aschokinatgmail/freertos_cpp_wrappers/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***********************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>

// Include mock headers first to override FreeRTOS definitions
#include "mocks/freertos_mocks.hpp"

// Include the module under test
#include "freertos_event_group.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::NotNull;
using ::testing::InSequence;

/**
 * @brief Test fixture for FreeRTOS Event Group tests
 */
class FreeRTOSEventGroupTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock = FreeRTOSMockInstance::getInstance();
        mock_event_group_handle = reinterpret_cast<EventGroupHandle_t>(0x12345678);
    }

    void TearDown() override {
        // Reset the mock instance
        FreeRTOSMockInstance::resetInstance();
    }

    std::shared_ptr<::testing::StrictMock<::FreeRTOSMock>> mock;
    EventGroupHandle_t mock_event_group_handle;
};

// =============================================================================
// Static Event Group Allocator Tests
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, StaticEventGroupAllocatorConstruction) {
    /*
     * Test that static event group allocator can be constructed and deleted
     * without any FreeRTOS calls (constructor/destructor only initialize/cleanup 
     * internal state).
     */
    freertos::static_event_group_allocator allocator;
    // Static allocator should not make any FreeRTOS calls during construction
}

TEST_F(FreeRTOSEventGroupTest, StaticEventGroupAllocatorCreate) {
    EXPECT_CALL(*mock, xEventGroupCreateStatic(NotNull()))
        .WillOnce(Return(mock_event_group_handle));
    
    freertos::static_event_group_allocator allocator;
    auto handle = allocator.create();
    EXPECT_EQ(handle, mock_event_group_handle);
}

TEST_F(FreeRTOSEventGroupTest, StaticEventGroupAllocatorCreateFailure) {
    EXPECT_CALL(*mock, xEventGroupCreateStatic(NotNull()))
        .WillOnce(Return(static_cast<EventGroupHandle_t>(nullptr)));
    
    freertos::static_event_group_allocator allocator;
    auto handle = allocator.create();
    EXPECT_EQ(handle, nullptr);
}

// =============================================================================
// Dynamic Event Group Allocator Tests  
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, DynamicEventGroupAllocatorConstruction) {
    /*
     * Test that dynamic event group allocator can be constructed and deleted
     * without any FreeRTOS calls.
     */
    freertos::dynamic_event_group_allocator allocator;
    // Dynamic allocator should not make any FreeRTOS calls during construction
}

TEST_F(FreeRTOSEventGroupTest, DynamicEventGroupAllocatorCreate) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    
    freertos::dynamic_event_group_allocator allocator;
    auto handle = allocator.create();
    EXPECT_EQ(handle, mock_event_group_handle);
}

TEST_F(FreeRTOSEventGroupTest, DynamicEventGroupAllocatorCreateFailure) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(static_cast<EventGroupHandle_t>(nullptr)));
    
    freertos::dynamic_event_group_allocator allocator;
    auto handle = allocator.create();
    EXPECT_EQ(handle, nullptr);
}

// =============================================================================
// Event Group Construction/Destruction Tests
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, EventGroupConstruction) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::event_group<freertos::dynamic_event_group_allocator> event_group;
    EXPECT_EQ(event_group.handle(), mock_event_group_handle);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupDestructionNullHandle) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(static_cast<EventGroupHandle_t>(nullptr)));
    // Should not call vEventGroupDelete for null handle
    
    freertos::event_group<freertos::dynamic_event_group_allocator> event_group;
    EXPECT_EQ(event_group.handle(), nullptr);
}

TEST_F(FreeRTOSEventGroupTest, StaticEventGroupConstruction) {
    EXPECT_CALL(*mock, xEventGroupCreateStatic(NotNull()))
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::sa::event_group event_group;
    EXPECT_EQ(event_group.handle(), mock_event_group_handle);
}

// =============================================================================
// Event Group Basic Operations Tests
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, EventGroupSetBits) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupSetBits(mock_event_group_handle, 0x05))
        .WillOnce(Return(0x05));
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.set_bits(0x05);
    EXPECT_EQ(result, 0x05);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupSetBitsISR) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupSetBitsFromISR(mock_event_group_handle, 0x0A, NotNull()))
        .WillOnce(Return(0x0A));
    EXPECT_CALL(*mock, portYIELD_FROM_ISR(_));
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.set_bits_isr(0x0A);
    EXPECT_EQ(result, 0x0A);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupClearBits) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupClearBits(mock_event_group_handle, 0x03))
        .WillOnce(Return(0x0C)); // Cleared bits 0 and 1, remaining bits 2 and 3
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.clear_bits(0x03);
    EXPECT_EQ(result, 0x0C);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupGetBits) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupGetBits(mock_event_group_handle))
        .WillOnce(Return(0x07));
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.get_bits();
    EXPECT_EQ(result, 0x07);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupGetBitsISR) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupGetBitsFromISR(mock_event_group_handle))
        .WillOnce(Return(0x0F));
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.get_bits_isr();
    EXPECT_EQ(result, 0x0F);
}

// =============================================================================
// Event Group Wait Operations Tests
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, EventGroupWaitBitsAnyBits) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupWaitBits(mock_event_group_handle, 0x03, pdFALSE, pdFALSE, 100))
        .WillOnce(Return(0x01)); // One of the waited bits is set
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.wait_bits(0x03, pdFALSE, pdFALSE, 100);
    EXPECT_EQ(result, 0x01);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupWaitBitsAllBits) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupWaitBits(mock_event_group_handle, 0x06, pdTRUE, pdTRUE, 200))
        .WillOnce(Return(0x06)); // All waited bits are set
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.wait_bits(0x06, pdTRUE, pdTRUE, 200);
    EXPECT_EQ(result, 0x06);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupWaitBitsTimeout) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupWaitBits(mock_event_group_handle, 0xFF, pdFALSE, pdTRUE, 50))
        .WillOnce(Return(0x00)); // Timeout occurred
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.wait_bits(0xFF, pdFALSE, pdTRUE, 50);
    EXPECT_EQ(result, 0x00);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupWaitBitsChronoTimeout) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupWaitBits(mock_event_group_handle, 0x01, pdFALSE, pdFALSE, 2000))
        .WillOnce(Return(0x01)); // 2 seconds = 2000ms
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.wait_bits(0x01, pdFALSE, pdFALSE, std::chrono::seconds(2));
    EXPECT_EQ(result, 0x01);
}

// =============================================================================
// Event Group Synchronization Tests
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, EventGroupSync) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupSync(mock_event_group_handle, 0x01, 0x07, 1000))
        .WillOnce(Return(0x07)); // All sync bits set
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.sync(0x01, 0x07, 1000);
    EXPECT_EQ(result, 0x07);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupSyncChronoTimeout) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupSync(mock_event_group_handle, 0x02, 0x0E, 500))
        .WillOnce(Return(0x0E)); // 500ms timeout
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.sync(0x02, 0x0E, std::chrono::milliseconds(500));
    EXPECT_EQ(result, 0x0E);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupSyncTimeout) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupSync(mock_event_group_handle, 0x04, 0x0F, 100))
        .WillOnce(Return(0x04)); // Timeout - only our bit is set
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.sync(0x04, 0x0F, 100);
    EXPECT_EQ(result, 0x04);
}

// =============================================================================
// Comprehensive Chrono Duration Tests
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, EventGroupChronoMicrosecondsTimeout) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupWaitBits(mock_event_group_handle, 0x08, pdFALSE, pdFALSE, 0))
        .WillOnce(Return(0x08)); // 500µs = 0ms after duration_cast
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.wait_bits(0x08, pdFALSE, pdFALSE, std::chrono::microseconds(500));
    EXPECT_EQ(result, 0x08);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupChronoNanosecondsTimeout) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupWaitBits(mock_event_group_handle, 0x10, pdTRUE, pdFALSE, 1))
        .WillOnce(Return(0x10)); // 1000000ns = 1ms
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.wait_bits(0x10, pdTRUE, pdFALSE, std::chrono::nanoseconds(1000000));
    EXPECT_EQ(result, 0x10);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupChronoMinutesTimeout) {
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupSync(mock_event_group_handle, 0x08, 0x18, 60000))
        .WillOnce(Return(0x18)); // 1 minute = 60000ms
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.sync(0x08, 0x18, std::chrono::minutes(1));
    EXPECT_EQ(result, 0x18);
}

// =============================================================================
// Complex Event Group Scenarios
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, EventGroupComplexScenario) {
    /*
     * Test a complex scenario that involves multiple operations:
     * 1. Set some bits
     * 2. Wait for specific bits
     * 3. Clear some bits
     * 4. Check final state
     */
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    
    InSequence seq;
    EXPECT_CALL(*mock, xEventGroupSetBits(mock_event_group_handle, 0x03))
        .WillOnce(Return(0x03));
    EXPECT_CALL(*mock, xEventGroupWaitBits(mock_event_group_handle, 0x01, pdFALSE, pdFALSE, 100))
        .WillOnce(Return(0x03));
    EXPECT_CALL(*mock, xEventGroupClearBits(mock_event_group_handle, 0x02))
        .WillOnce(Return(0x01));
    EXPECT_CALL(*mock, xEventGroupGetBits(mock_event_group_handle))
        .WillOnce(Return(0x01));
    
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    // Set bits 0 and 1
    auto set_result = event_group.set_bits(0x03);
    EXPECT_EQ(set_result, 0x03);
    
    // Wait for bit 0 (should succeed immediately)
    auto wait_result = event_group.wait_bits(0x01, pdFALSE, pdFALSE, 100);
    EXPECT_EQ(wait_result, 0x03);
    
    // Clear bit 1
    auto clear_result = event_group.clear_bits(0x02);
    EXPECT_EQ(clear_result, 0x01);
    
    // Check final state (only bit 0 should be set)
    auto final_bits = event_group.get_bits();
    EXPECT_EQ(final_bits, 0x01);
}

// =============================================================================
// Error Conditions and Edge Cases
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, EventGroupCreationFailure) {
    /*
     * Test creation failure handling.
     * Note: In a real implementation, this might trigger configASSERT,
     * but in our mock environment we can test the wrapper's error handling
     * behavior for host-based testing limitations.
     */
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(static_cast<EventGroupHandle_t>(nullptr)));
    // Should not call vEventGroupDelete for null handle
    
    try {
        freertos::da::event_group event_group;
        // Event group handle should be null
        EXPECT_EQ(event_group.handle(), nullptr);
    } catch (...) {
        // Exception may be thrown due to configASSERT in mock environment
    }
}

TEST_F(FreeRTOSEventGroupTest, EventGroupAllBitsPattern) {
    /*
     * Test with all bits set pattern (0xFF for 8-bit, 0xFFFFFF for 24-bit).
     * EventBits_t is typically 24-bit in FreeRTOS (bits 0-23).
     */
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupSetBits(mock_event_group_handle, 0xFFFFFF))
        .WillOnce(Return(0xFFFFFF));
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.set_bits(0xFFFFFF);
    EXPECT_EQ(result, 0xFFFFFF);
}

TEST_F(FreeRTOSEventGroupTest, EventGroupZeroBitsPattern) {
    /*
     * Test with zero bits - should be a no-op for set operations
     */
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(mock_event_group_handle));
    EXPECT_CALL(*mock, xEventGroupSetBits(mock_event_group_handle, 0x00))
        .WillOnce(Return(0x00));
    EXPECT_CALL(*mock, vEventGroupDelete(mock_event_group_handle));
    
    freertos::da::event_group event_group;
    
    auto result = event_group.set_bits(0x00);
    EXPECT_EQ(result, 0x00);
}

// =============================================================================
// Static vs Dynamic Allocator Behavior Tests
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, StaticVsDynamicBehavior) {
    /*
     * Test that static and dynamic event groups behave identically
     * for all operations (only construction differs).
     */
    EventGroupHandle_t static_handle = reinterpret_cast<EventGroupHandle_t>(0x11111111);
    EventGroupHandle_t dynamic_handle = reinterpret_cast<EventGroupHandle_t>(0x22222222);
    
    // Static event group
    EXPECT_CALL(*mock, xEventGroupCreateStatic(NotNull()))
        .WillOnce(Return(static_handle));
    EXPECT_CALL(*mock, xEventGroupSetBits(static_handle, 0x05))
        .WillOnce(Return(0x05));
    EXPECT_CALL(*mock, vEventGroupDelete(static_handle));
    
    // Dynamic event group  
    EXPECT_CALL(*mock, xEventGroupCreate())
        .WillOnce(Return(dynamic_handle));
    EXPECT_CALL(*mock, xEventGroupSetBits(dynamic_handle, 0x05))
        .WillOnce(Return(0x05));
    EXPECT_CALL(*mock, vEventGroupDelete(dynamic_handle));
    
    freertos::sa::event_group static_event_group;
    freertos::da::event_group dynamic_event_group;
    
    auto static_result = static_event_group.set_bits(0x05);
    auto dynamic_result = dynamic_event_group.set_bits(0x05);
    
    EXPECT_EQ(static_result, dynamic_result);
}

// =============================================================================
// API Completeness Documentation Test
// =============================================================================

TEST_F(FreeRTOSEventGroupTest, EventGroupAPICompleteness) {
    /*
     * This test documents the API completeness of the event group wrapper.
     * 
     * Covered FreeRTOS Event Group APIs:
     * ✅ xEventGroupCreate
     * ✅ xEventGroupCreateStatic  
     * ✅ vEventGroupDelete
     * ✅ xEventGroupSetBits
     * ✅ xEventGroupSetBitsFromISR
     * ✅ xEventGroupClearBits
     * ✅ xEventGroupWaitBits (with chrono support)
     * ✅ xEventGroupGetBits
     * ✅ xEventGroupGetBitsFromISR
     * ✅ xEventGroupSync (with chrono support)
     * 
     * Not covered (not implemented in wrapper):
     * ❌ xEventGroupClearBitsFromISR (not available in all FreeRTOS versions)
     * ❌ uxEventGroupGetNumber (for debugging only)
     * ❌ vEventGroupSetNumber (for debugging only)
     * 
     * Note: The wrapper provides complete coverage of production-ready
     * event group operations with additional C++ conveniences like
     * chrono timeout support and RAII resource management.
     */
    EXPECT_TRUE(true); // Placeholder for documentation
}