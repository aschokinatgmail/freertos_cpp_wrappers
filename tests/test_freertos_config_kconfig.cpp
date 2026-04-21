/**********************************************************************************
@file test_freertos_config_kconfig.cpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief Tests for Kconfig-based configuration system
@version 3.1.0
@date 2026-04-21

Verifies that all configuration macros from the Kconfig system are
properly defined and have correct default values after including
freertos_config.hpp.
**********************************************************************************/

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FreeRTOS.h"
#include "freertos_config.hpp"

// ============================================================================
// Kconfig configuration macros must be defined
// ============================================================================

TEST(KconfigConfigTest, AllMacrosAreDefined) {
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_GTHR_BACKEND, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_THREADING, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_TABLE_SIZE, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_STD_THREAD, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_STD_MUTEX, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_STD_SHARED_MUTEX, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_STD_FUTURE, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_EXPECTED, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_SPAN, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_THREAD_SAFETY, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_CONCEPTS, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_ALLOCATOR, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_ISR_RESULT, -1);
    EXPECT_NE(FREERTOS_CPP_WRAPPERS_ENABLE_STRONG_TYPES, -1);
}

// ============================================================================
// Defaults match Kconfig defaults
// ============================================================================

TEST(KconfigConfigTest, DefaultGthrBackendIsOff) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_GTHR_BACKEND, 0);
}

TEST(KconfigConfigTest, DefaultExternalThreadingIsOff) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_THREADING, 0);
}

TEST(KconfigConfigTest, DefaultAtomicWaitNotifyIsOff) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY, 0);
}

TEST(KconfigConfigTest, DefaultAtomicWaitImplIsSemaphoreTable) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL, 1);
}

TEST(KconfigConfigTest, DefaultAtomicWaitTableSizeIs16) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_TABLE_SIZE, 16);
}

TEST(KconfigConfigTest, DefaultAllocStrategyIsAuto) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY, 3);
}

TEST(KconfigConfigTest, DefaultCondvarImplIsCountingSemaphore) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL, 1);
}

TEST(KconfigConfigTest, DefaultStdThreadIsOn) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_STD_THREAD, 1);
}

TEST(KconfigConfigTest, DefaultStdMutexIsOn) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_STD_MUTEX, 1);
}

TEST(KconfigConfigTest, DefaultStdSharedMutexIsOn) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_STD_SHARED_MUTEX, 1);
}

TEST(KconfigConfigTest, DefaultStdFutureIsOff) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_STD_FUTURE, 0);
}

TEST(KconfigConfigTest, DefaultExpectedIsOn) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_EXPECTED, 1);
}

TEST(KconfigConfigTest, DefaultSpanIsOn) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_SPAN, 1);
}

TEST(KconfigConfigTest, DefaultThreadSafetyIsOn) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_THREAD_SAFETY, 1);
}

TEST(KconfigConfigTest, DefaultConceptsIsOn) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_CONCEPTS, 1);
}

TEST(KconfigConfigTest, DefaultExternalAllocatorIsOn) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_ALLOCATOR, 1);
}

TEST(KconfigConfigTest, DefaultIsrResultIsOn) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_ISR_RESULT, 1);
}

TEST(KconfigConfigTest, DefaultStrongTypesIsOn) {
    EXPECT_EQ(FREERTOS_CPP_WRAPPERS_ENABLE_STRONG_TYPES, 1);
}

// ============================================================================
// Boolean config macros are 0 or 1
// ============================================================================

TEST(KconfigConfigTest, BooleanMacrosAreValid) {
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_GTHR_BACKEND == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_GTHR_BACKEND == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_THREADING == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_THREADING == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_STD_THREAD == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_STD_THREAD == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_STD_MUTEX == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_STD_MUTEX == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_STD_SHARED_MUTEX == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_STD_SHARED_MUTEX == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_STD_FUTURE == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_STD_FUTURE == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_EXPECTED == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_EXPECTED == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_SPAN == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_SPAN == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_THREAD_SAFETY == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_THREAD_SAFETY == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_CONCEPTS == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_CONCEPTS == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_ALLOCATOR == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_ALLOCATOR == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_ISR_RESULT == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_ISR_RESULT == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ENABLE_STRONG_TYPES == 0 ||
                FREERTOS_CPP_WRAPPERS_ENABLE_STRONG_TYPES == 1);
}

// ============================================================================
// Enum config macros have valid values
// ============================================================================

TEST(KconfigConfigTest, AtomicWaitImplIsValid) {
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL >= 0 &&
                FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL <= 3);
}

TEST(KconfigConfigTest, AllocStrategyIsValid) {
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY >= 1 &&
                FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY <= 3);
}

TEST(KconfigConfigTest, CondvarImplIsValid) {
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL >= 1 &&
                FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL <= 3);
}

TEST(KconfigConfigTest, AtomicWaitTableSizeIsPositive) {
    EXPECT_GT(FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_TABLE_SIZE, 0);
}

// ============================================================================
// Existing FreeRTOS feature detection macros still work
// ============================================================================

TEST(KconfigConfigTest, FreeRTOSFeatureMacrosStillWork) {
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_HAS_MUTEX == 0 ||
                FREERTOS_CPP_WRAPPERS_HAS_MUTEX == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE == 0 ||
                FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_HAS_RECURSIVE_MUTEX == 0 ||
                FREERTOS_CPP_WRAPPERS_HAS_RECURSIVE_MUTEX == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_HAS_SW_TIMER == 0 ||
                FREERTOS_CPP_WRAPPERS_HAS_SW_TIMER == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_HAS_TASK_NOTIFICATIONS == 0 ||
                FREERTOS_CPP_WRAPPERS_HAS_TASK_NOTIFICATIONS == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION == 0 ||
                FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_HAS_DYNAMIC_ALLOCATION == 0 ||
                FREERTOS_CPP_WRAPPERS_HAS_DYNAMIC_ALLOCATION == 1);
    EXPECT_TRUE(FREERTOS_CPP_WRAPPERS_CPP17 == 0 ||
                FREERTOS_CPP_WRAPPERS_CPP17 == 1);
}