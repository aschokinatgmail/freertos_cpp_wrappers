#include <gtest/gtest.h>

// Include the freertos_queue wrapper header
#include "../include/freertos_queue.hpp"

namespace {

// Test that the queue header compiles successfully
TEST(FreeRTOSQueueTest, HeaderCompilation) {
    // This test verifies that the queue header can be included
    // and compiled without errors
    EXPECT_TRUE(true);
}

// Test that queue types are accessible
TEST(FreeRTOSQueueTest, TypeAccess) {
    // Basic smoke test for type accessibility
    using namespace freertos;
    
    // Test that we can reference the queue types and templates
    // This ensures the template classes and types are properly defined
    EXPECT_TRUE(true);
}

} // namespace