/**********************************************************************************
@file test_freertos_condition_variable.cpp
@brief Tests for freertos::condition_variable_any
@version 3.1.0
@date 2026-04-22
***********************************************************************************/

#include <freertos_condition_variable.hpp>
#include <freertos_lock.hpp>
#include <freertos_semaphore.hpp>

#include <gtest/gtest.h>
#include <mutex>
#include <chrono>

using dynamic_mutex = freertos::mutex<freertos::dynamic_semaphore_allocator>;

TEST(ConditionVariableAnyTest, HasNativeHandle) {
    freertos::condition_variable_any cv;
    (void)cv.native_handle();
}

TEST(ConditionVariableAnyTest, NotifyOne) {
    freertos::condition_variable_any cv;
    cv.notify_one();
}

TEST(ConditionVariableAnyTest, NotifyAll) {
    freertos::condition_variable_any cv;
    cv.notify_all();
}

TEST(ConditionVariableAnyTest, WaitWithMutex) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    m.lock();
    std::cv_status status = cv.wait_for(m, std::chrono::milliseconds(1));
    EXPECT_EQ(status, std::cv_status::timeout);
}

TEST(ConditionVariableAnyTest, WaitUntilTimeout) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    m.lock();
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1);
    std::cv_status status = cv.wait_until(m, deadline);
    EXPECT_EQ(status, std::cv_status::timeout);
}

TEST(ConditionVariableAnyTest, WaitForPredicate) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    m.lock();
    bool flag = false;
    bool result = cv.wait_for(m, std::chrono::milliseconds(1), [&flag] { return flag; });
    EXPECT_FALSE(result);
}

TEST(ConditionVariableAnyTest, WaitUntilPredicate) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    m.lock();
    bool flag = false;
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1);
    bool result = cv.wait_until(m, deadline, [&flag] { return flag; });
    EXPECT_FALSE(result);
}

TEST(ConditionVariableAnyTest, WaitPredicateSatisfied) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    m.lock();
    bool flag = true;
    bool result = cv.wait_for(m, std::chrono::milliseconds(10), [&flag] { return flag; });
    EXPECT_TRUE(result);
}

TEST(ConditionVariableAnyTest, WithUniqueLock) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    freertos::unique_lock<dynamic_mutex> lock(m);
    std::cv_status status = cv.wait_for(lock, std::chrono::milliseconds(1));
    EXPECT_EQ(status, std::cv_status::timeout);
}

TEST(ConditionVariableAnyTest, NotifyOneIsr) {
    freertos::condition_variable_any cv;
    cv.notify_one_isr();
}

TEST(ConditionVariableAnyTest, NotifyAllIsr) {
    freertos::condition_variable_any cv;
    cv.notify_all_isr();
}

#if configSUPPORT_DYNAMIC_ALLOCATION
TEST(ConditionVariableAnyTest, DaNamespaceAlias) {
    freertos::da::condition_variable_any cv;
    cv.notify_one();
}
#endif