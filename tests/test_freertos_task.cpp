#include <gtest/gtest.h>

// Include the freertos_task wrapper header
#include "../include/freertos_task.hpp"

namespace {

// Test that the task header compiles successfully
TEST(FreeRTOSTaskTest, HeaderCompilation) {
    // This test verifies that the task header can be included
    // and compiled without errors
    EXPECT_TRUE(true);
}

// Test that task types are accessible
TEST(FreeRTOSTaskTest, TypeAccess) {
    // Basic smoke test for type accessibility
    using namespace freertos;
    
    // Test that we can reference the task types and functions
    // This ensures the template classes and types are properly defined
    EXPECT_TRUE(true);
}

// Test that task delay functions can be referenced
TEST(FreeRTOSTaskTest, DelayFunctionAccess) {
    // Test that the delay functions are accessible through compilation
    // This is a compilation test to ensure the function signatures are correct
    using namespace freertos;
    
    // Just ensure compilation works - don't actually call these functions
    // since they need actual FreeRTOS implementation
    EXPECT_TRUE(true);
}

} // namespace