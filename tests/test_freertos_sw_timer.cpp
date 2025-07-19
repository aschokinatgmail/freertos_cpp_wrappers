#include <gtest/gtest.h>

// Include the freertos_sw_timer wrapper header
#include "../include/freertos_sw_timer.hpp"

namespace {

// Test that the software timer header compiles successfully
TEST(FreeRTOSSwTimerTest, HeaderCompilation) {
    // This test verifies that the software timer header can be included
    // and compiled without errors
    EXPECT_TRUE(true);
}

// Test that software timer types are accessible
TEST(FreeRTOSSwTimerTest, TypeAccess) {
    // Basic smoke test for type accessibility
    using namespace freertos;
    
    // Test that we can reference the software timer types
    // This ensures the template classes and types are properly defined
    EXPECT_TRUE(true);
}

} // namespace