#include <gtest/gtest.h>

#if FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_THREADING && !defined(UNIT_TESTING)

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "freertos_external_threading.hpp"

TEST(FreeRTOSExternalThreading, TypeSizes) {
    EXPECT_GT(sizeof(std::__libcpp_mutex_t), 0u);
    EXPECT_GT(sizeof(std::__libcpp_recursive_mutex_t), 0u);
    EXPECT_GT(sizeof(std::__libcpp_condvar_t), 0u);
    EXPECT_GT(sizeof(std::__libcpp_exec_once_flag), 0u);
    EXPECT_EQ(sizeof(std::__libcpp_thread_t), sizeof(TaskHandle_t));
}

TEST(FreeRTOSExternalThreading, MutexInitStruct) {
    std::__libcpp_mutex_t m = _LIBCPP_MUTEX_INITIALIZER;
    EXPECT_FALSE(m.initialized);
    EXPECT_EQ(m.handle, nullptr);
}

TEST(FreeRTOSExternalThreading, RecursiveMutexInitStruct) {
    std::__libcpp_recursive_mutex_t m = { {}, nullptr, false };
    EXPECT_FALSE(m.initialized);
    EXPECT_EQ(m.handle, nullptr);
}

TEST(FreeRTOSExternalThreading, CondInitStruct) {
    std::__libcpp_condvar_t c = _LIBCPP_CONDVAR_INITIALIZER;
    (void)c;
}

TEST(FreeRTOSExternalThreading, OnceInitValue) {
    std::__libcpp_exec_once_flag once = _LIBCPP_EXEC_ONCE_INITIALIZER;
    EXPECT_EQ(once, 0);
}

TEST(FreeRTOSExternalThreading, MacrosDefined) {
    EXPECT_EQ(_LIBCPP_HAS_THREADS, 1);
}

TEST(FreeRTOSExternalThreading, NullThread) {
    EXPECT_EQ(_LIBCPP_NULL_THREAD, nullptr);
}

#else

TEST(FreeRTOSExternalThreading, BackendDisabled) {
}

#endif