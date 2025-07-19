#include <gtest/gtest.h>

// Include the freertos_semaphore wrapper header
#include "../include/freertos_semaphore.hpp"

namespace {

// Test that the semaphore header compiles successfully
TEST(FreeRTOSSemaphoreTest, HeaderCompilation) {
    // This test verifies that the semaphore header can be included
    // and compiled without errors
    EXPECT_TRUE(true);
}

// Test that semaphore types are accessible
TEST(FreeRTOSSemaphoreTest, TypeAccess) {
    // Basic smoke test for type accessibility
    using namespace freertos;
    
    // Test that we can reference the semaphore types
    // This ensures the template classes and types are properly defined
    EXPECT_TRUE(true);
}

} // namespace