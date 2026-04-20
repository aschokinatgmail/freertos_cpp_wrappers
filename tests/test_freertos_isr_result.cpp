/**
 * @file test_freertos_isr_result.cpp
 * @brief Unit tests for freertos::isr_result<T> struct
 *
 * Naming convention (going forward): TestSuiteName_TestCaseName
 * or Component_Function_Scenario
 *
 * Tests cover:
 * - Default construction
 * - Construction with values
 * - operator== and operator!= comparison
 * - Template instantiation with different types
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_isr_result.hpp"

using ::testing::Eq;

TEST(IsrResult, DefaultConstruction) {
    freertos::isr_result<int> r{};
    EXPECT_EQ(r.result, 0);
    EXPECT_EQ(r.higher_priority_task_woken, 0);
}

TEST(IsrResult, ConstructionWithValues) {
    freertos::isr_result<int> r{42, pdTRUE};
    EXPECT_EQ(r.result, 42);
    EXPECT_EQ(r.higher_priority_task_woken, pdTRUE);
}

TEST(IsrResult, OperatorEqualTrue) {
    freertos::isr_result<int> a{10, pdTRUE};
    freertos::isr_result<int> b{10, pdTRUE};
    EXPECT_TRUE(a == b);
}

TEST(IsrResult, OperatorEqualFalseDifferentResult) {
    freertos::isr_result<int> a{10, pdTRUE};
    freertos::isr_result<int> b{20, pdTRUE};
    EXPECT_FALSE(a == b);
}

TEST(IsrResult, OperatorEqualFalseDifferentWoken) {
    freertos::isr_result<int> a{10, pdTRUE};
    freertos::isr_result<int> b{10, pdFALSE};
    EXPECT_FALSE(a == b);
}

TEST(IsrResult, OperatorNotEqualTrueDifferentResult) {
    freertos::isr_result<int> a{1, pdFALSE};
    freertos::isr_result<int> b{2, pdFALSE};
    EXPECT_TRUE(a != b);
}

TEST(IsrResult, OperatorNotEqualTrueDifferentWoken) {
    freertos::isr_result<int> a{5, pdFALSE};
    freertos::isr_result<int> b{5, pdTRUE};
    EXPECT_TRUE(a != b);
}

TEST(IsrResult, OperatorNotEqualFalseSameValues) {
    freertos::isr_result<int> a{7, pdTRUE};
    freertos::isr_result<int> b{7, pdTRUE};
    EXPECT_FALSE(a != b);
}

TEST(IsrResult, OperatorEqualWithBaseTypeT) {
    freertos::isr_result<BaseType_t> a{pdTRUE, pdTRUE};
    freertos::isr_result<BaseType_t> b{pdTRUE, pdTRUE};
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(IsrResult, OperatorEqualWithBool) {
    freertos::isr_result<bool> a{true, pdTRUE};
    freertos::isr_result<bool> b{true, pdTRUE};
    EXPECT_TRUE(a == b);

    freertos::isr_result<bool> c{false, pdTRUE};
    EXPECT_TRUE(a != c);
}
