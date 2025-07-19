#include <gtest/gtest.h>

// Include the main freertos wrapper header
#include "../include/freertos.hpp"

namespace {

// Test that the main header compiles successfully
TEST(FreeRTOSTest, HeaderCompilation) {
    // This test simply verifies that the header can be included and compiled
    // without errors. The act of including the header exercises the compilation.
    EXPECT_TRUE(true);
}

// Test that the freertos namespace is accessible
TEST(FreeRTOSTest, NamespaceAccess) {
    // Test that we can access the freertos namespace
    // This is a basic smoke test to ensure the namespace structure is correct
    using namespace freertos;
    EXPECT_TRUE(true);
}

} // namespace