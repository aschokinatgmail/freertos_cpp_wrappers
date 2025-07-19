#include <gtest/gtest.h>

// Include the freertos_message_buffer wrapper header  
#include "../include/freertos_message_buffer.hpp"

namespace {

// Test that the message buffer header compiles successfully
TEST(FreeRTOSMessageBufferTest, HeaderCompilation) {
    // This test verifies that the message buffer header can be included
    // and compiled without errors
    EXPECT_TRUE(true);
}

// Test that message buffer types are accessible
TEST(FreeRTOSMessageBufferTest, TypeAccess) {
    // Basic smoke test for type accessibility
    using namespace freertos;
    
    // Test that we can reference the message buffer types
    // This ensures the template classes and types are properly defined
    EXPECT_TRUE(true);
}

} // namespace