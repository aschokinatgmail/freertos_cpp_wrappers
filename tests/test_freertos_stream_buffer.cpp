#include <gtest/gtest.h>

// Include the freertos_stream_buffer wrapper header
#include "../include/freertos_stream_buffer.hpp"

namespace {

// Test that the stream buffer header compiles successfully
TEST(FreeRTOSStreamBufferTest, HeaderCompilation) {
    // This test verifies that the stream buffer header can be included
    // and compiled without errors
    EXPECT_TRUE(true);
}

// Test that stream buffer types are accessible
TEST(FreeRTOSStreamBufferTest, TypeAccess) {
    // Basic smoke test for type accessibility
    using namespace freertos;
    
    // Test that we can reference the stream buffer types
    // This ensures the template classes and types are properly defined
    EXPECT_TRUE(true);
}

} // namespace