/**
 * @file test_freertos_isr_context.cpp
 * @brief Unit tests for freertos::in_isr() and freertos::current_execution_context()
 *
 * Tests cover:
 * - in_isr() returns false in task context (default)
 * - in_isr() returns true when ISR flag is set
 * - current_execution_context() returns task in task context
 * - current_execution_context() returns isr in ISR context
 * - execution_context enum values
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_isr_context.hpp"
#include "freertos_mocks.hpp"

using ::testing::Eq;
using ::testing::Return;

class IsrContextTest : public ::testing::Test {
protected:
    std::shared_ptr<::testing::StrictMock<::FreeRTOSMock>> mock;

    void SetUp() override {
        mock = FreeRTOSMockInstance::getInstance();
    }

    void TearDown() override {
        FreeRTOSMockInstance::resetInstance();
    }
};

TEST_F(IsrContextTest, InIsrReturnsFalseInTaskContext) {
    EXPECT_CALL(*mock, xPortIsInsideInterrupt())
        .WillOnce(Return(pdFALSE));
    EXPECT_FALSE(freertos::in_isr());
}

TEST_F(IsrContextTest, InIsrReturnsTrueInIsrContext) {
    EXPECT_CALL(*mock, xPortIsInsideInterrupt())
        .WillOnce(Return(pdTRUE));
    EXPECT_TRUE(freertos::in_isr());
}

TEST_F(IsrContextTest, CurrentExecutionContextReturnsTaskInTaskContext) {
    EXPECT_CALL(*mock, xPortIsInsideInterrupt())
        .WillOnce(Return(pdFALSE));
    EXPECT_EQ(freertos::current_execution_context(),
              freertos::execution_context::task);
}

TEST_F(IsrContextTest, CurrentExecutionContextReturnsIsrInIsrContext) {
    EXPECT_CALL(*mock, xPortIsInsideInterrupt())
        .WillOnce(Return(pdTRUE));
    EXPECT_EQ(freertos::current_execution_context(),
              freertos::execution_context::isr);
}

TEST_F(IsrContextTest, ExecutionContextEnumValues) {
    EXPECT_EQ(static_cast<uint8_t>(freertos::execution_context::task), 0);
    EXPECT_EQ(static_cast<uint8_t>(freertos::execution_context::isr), 1);
}