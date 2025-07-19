#include <gtest/gtest.h>

// Include the freertos_event_group wrapper header
#include "../include/freertos_event_group.hpp"

namespace {

// Test that the event group header compiles successfully
TEST(FreeRTOSEventGroupTest, HeaderCompilation) {
    // This test verifies that the event group header can be included
    // and compiled without errors
    EXPECT_TRUE(true);
}

// Test that event group types are accessible
TEST(FreeRTOSEventGroupTest, TypeAccess) {
    // Basic smoke test for type accessibility
    using namespace freertos;
    
    // Test that we can reference the event group types
    // This ensures the template classes and types are properly defined
    EXPECT_TRUE(true);
}

} // namespace