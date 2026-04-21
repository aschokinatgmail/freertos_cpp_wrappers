#include <gtest/gtest.h>

#if FREERTOS_CPP_WRAPPERS_ENABLE_GTHR_BACKEND && !defined(UNIT_TESTING)

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "freertos_gthr.hpp"

TEST(FreeRTOSGthr, TypeSizes) {
    EXPECT_GT(sizeof(__gthread_mutex_t), 0u);
    EXPECT_GT(sizeof(__gthread_recursive_mutex_t), 0u);
    EXPECT_GT(sizeof(__gthread_cond_t), 0u);
    EXPECT_GT(sizeof(__gthread_once_t), 0u);
    EXPECT_EQ(sizeof(__gthread_t), sizeof(TaskHandle_t));
}

TEST(FreeRTOSGthr, MutexInitStruct) {
    __gthread_mutex_t m = { {}, nullptr, false };
    EXPECT_FALSE(m.initialized);
    EXPECT_EQ(m.handle, nullptr);
}

TEST(FreeRTOSGthr, RecursiveMutexInitStruct) {
    __gthread_recursive_mutex_t m = { {}, nullptr, false };
    EXPECT_FALSE(m.initialized);
    EXPECT_EQ(m.handle, nullptr);
}

TEST(FreeRTOSGthr, CondInitStruct) {
    __gthread_cond_t c = __GTHREAD_COND_INIT;
    (void)c;
}

TEST(FreeRTOSGthr, OnceInitValue) {
    __gthread_once_t once = __GTHREAD_ONCE_INIT;
    EXPECT_EQ(once, 0);
}

TEST(FreeRTOSGthr, MacrosDefined) {
    EXPECT_EQ(__GTHREADS, 1);
#if FREERTOS_CPP_WRAPPERS_ENABLE_STD_THREAD
    EXPECT_EQ(__GTHREADS_CXX0X, 1);
#endif
#if FREERTOS_CPP_WRAPPERS_ENABLE_STD_MUTEX
    EXPECT_EQ(__GTHREAD_HAS_COND, 1);
#endif
}

#else

TEST(FreeRTOSGthr, BackendDisabled) {
}

#endif