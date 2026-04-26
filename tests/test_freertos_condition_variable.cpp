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
#include <gmock/gmock.h>
#include <mutex>
#include <chrono>

#include "mocks/freertos_mocks.hpp"

using dynamic_mutex = freertos::mutex<freertos::dynamic_semaphore_allocator>;

using ::testing::_;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::StrictMock;

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

TEST(ConditionVariableAnyTest, WaitWithLockBlocksAndReturns) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    m.lock();
    cv.wait(m);
}

TEST(ConditionVariableAnyTest, WaitWithUniqueLockBlocksAndReturns) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    freertos::unique_lock<dynamic_mutex> lock(m);
    cv.wait(lock);
    EXPECT_TRUE(lock.owns_lock());
}

TEST(ConditionVariableAnyTest, WaitPredicateImmediatelyTrue) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    m.lock();
    bool result = cv.wait(m, [] { return true; });
    EXPECT_TRUE(result);
}

TEST(ConditionVariableAnyTest, WaitPredicateBecomesTrue) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    m.lock();
    int count = 0;
    bool result = cv.wait(m, [&count] { return ++count > 1; });
    EXPECT_TRUE(result);
    EXPECT_GT(count, 1);
}

TEST(ConditionVariableAnyTest, WaitPredicateWithUniqueLockImmediatelyTrue) {
    freertos::condition_variable_any cv;
    dynamic_mutex m;
    freertos::unique_lock<dynamic_mutex> lock(m);
    bool result = cv.wait(lock, [] { return true; });
    EXPECT_TRUE(result);
    EXPECT_TRUE(lock.owns_lock());
}

#if configSUPPORT_DYNAMIC_ALLOCATION
TEST(ConditionVariableAnyTest, DaNamespaceAlias) {
    freertos::da::condition_variable_any cv;
    cv.notify_one();
}
#endif

// =============================================================================
// Mock-based branch coverage tests
// =============================================================================

class ConditionVariableMockTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock = FreeRTOSMockInstance::getInstance();
  }
  void TearDown() override {
    FreeRTOSMockInstance::resetInstance();
  }
  std::shared_ptr<::testing::StrictMock<::FreeRTOSMock>> mock;
};

// --- Null-semaphore guard branch tests ---

TEST_F(ConditionVariableMockTest, NotifyOne_NullSemaphore_ReturnsEarly) {
    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(static_cast<SemaphoreHandle_t>(nullptr)));

    freertos::condition_variable_any cv;
    // notify_one() on null semaphore should return early without calling xSemaphoreGive
    cv.notify_one();
}

TEST_F(ConditionVariableMockTest, NotifyAll_NullSemaphore_ReturnsEarly) {
    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(static_cast<SemaphoreHandle_t>(nullptr)));

    freertos::condition_variable_any cv;
    // notify_all() on null semaphore should return early without calling xSemaphoreGive
    cv.notify_all();
}

TEST_F(ConditionVariableMockTest, NotifyOneEx_NullSemaphore_ReturnsInvalidHandle) {
    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(static_cast<SemaphoreHandle_t>(nullptr)));

    freertos::condition_variable_any cv;
    auto result = cv.notify_one_ex();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(ConditionVariableMockTest, NotifyAllEx_NullSemaphore_ReturnsInvalidHandle) {
    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(static_cast<SemaphoreHandle_t>(nullptr)));

    freertos::condition_variable_any cv;
    auto result = cv.notify_all_ex();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), freertos::error::invalid_handle);
}

TEST_F(ConditionVariableMockTest, NotifyOneIsr_NullSemaphore_ReturnsFalse) {
    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(static_cast<SemaphoreHandle_t>(nullptr)));

    freertos::condition_variable_any cv;
    auto result = cv.notify_one_isr();
    EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(ConditionVariableMockTest, NotifyAllIsr_NullSemaphore_ReturnsFalse) {
    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(static_cast<SemaphoreHandle_t>(nullptr)));

    freertos::condition_variable_any cv;
    auto result = cv.notify_all_isr();
    EXPECT_EQ(result.higher_priority_task_woken, pdFALSE);
}

TEST_F(ConditionVariableMockTest, Wait_NullSemaphore_UnlocksAndRelocks) {
    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(static_cast<SemaphoreHandle_t>(nullptr)));

    freertos::condition_variable_any cv;
    std::mutex m;
    m.lock();
    // wait() on null semaphore should unlock and relock without calling xSemaphoreTake
    cv.wait(m);
    // After wait, the mutex should be re-locked. Verify by trying to unlock.
    m.unlock();
    m.lock();
}

TEST_F(ConditionVariableMockTest, WaitFor_NullSemaphore_ReturnsTimeout) {
    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(static_cast<SemaphoreHandle_t>(nullptr)));

    freertos::condition_variable_any cv;
    std::mutex m;
    m.lock();
    auto status = cv.wait_for(m, std::chrono::milliseconds(100));
    EXPECT_EQ(status, std::cv_status::timeout);
}

// --- Non-null semaphore branch coverage tests ---

TEST_F(ConditionVariableMockTest, NotifyOneEx_Success) {
    SemaphoreHandle_t sem_handle = reinterpret_cast<SemaphoreHandle_t>(0x1234);

    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(sem_handle));
    EXPECT_CALL(*mock, xSemaphoreGive(sem_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(sem_handle));

    freertos::condition_variable_any cv;
    auto result = cv.notify_one_ex();
    EXPECT_TRUE(result.has_value());
}

TEST_F(ConditionVariableMockTest, NotifyOneEx_SemaphoreNotOwned) {
    SemaphoreHandle_t sem_handle = reinterpret_cast<SemaphoreHandle_t>(0x1234);

    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(sem_handle));
    EXPECT_CALL(*mock, xSemaphoreGive(sem_handle))
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, vSemaphoreDelete(sem_handle));

    freertos::condition_variable_any cv;
    auto result = cv.notify_one_ex();
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), freertos::error::semaphore_not_owned);
}

TEST_F(ConditionVariableMockTest, NotifyAllEx_Success) {
    SemaphoreHandle_t sem_handle = reinterpret_cast<SemaphoreHandle_t>(0x1234);

    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(sem_handle));
    // notify_all_ex calls notify_all() which gives FREERTOS_CPP_WRAPPERS_CONDITION_VARIABLE_MAX_WAITERS times
    EXPECT_CALL(*mock, xSemaphoreGive(sem_handle))
        .Times(FREERTOS_CPP_WRAPPERS_CONDITION_VARIABLE_MAX_WAITERS)
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(sem_handle));

    freertos::condition_variable_any cv;
    auto result = cv.notify_all_ex();
    EXPECT_TRUE(result.has_value());
}

TEST_F(ConditionVariableMockTest, WaitFor_Success_ReturnsNoTimeout) {
    SemaphoreHandle_t sem_handle = reinterpret_cast<SemaphoreHandle_t>(0x1234);

    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(sem_handle));
    // wait_for: unlock mutex (no mock needed for std::mutex), take semaphore, then lock mutex
    EXPECT_CALL(*mock, xSemaphoreTake(sem_handle, _))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(sem_handle));

    freertos::condition_variable_any cv;
    std::mutex m;
    m.lock();
    auto status = cv.wait_for(m, std::chrono::milliseconds(100));
    EXPECT_EQ(status, std::cv_status::no_timeout);
}

TEST_F(ConditionVariableMockTest, NotifyOneIsr_WithWoken) {
    SemaphoreHandle_t sem_handle = reinterpret_cast<SemaphoreHandle_t>(0x1234);

    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(sem_handle));
    EXPECT_CALL(*mock, xSemaphoreGiveFromISR(sem_handle, _))
        .WillOnce(
            ::testing::Invoke([](SemaphoreHandle_t, BaseType_t *higherPriorityTaskWoken) {
                *higherPriorityTaskWoken = pdTRUE;
                return pdTRUE;
            }));
    EXPECT_CALL(*mock, vSemaphoreDelete(sem_handle));

    freertos::condition_variable_any cv;
    auto result = cv.notify_one_isr();
    EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

TEST_F(ConditionVariableMockTest, NotifyAllIsr_WithWoken) {
    SemaphoreHandle_t sem_handle = reinterpret_cast<SemaphoreHandle_t>(0x1234);

    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(sem_handle));
    // notify_all_isr gives multiple times, at least one should wake a higher priority task
    EXPECT_CALL(*mock, xSemaphoreGiveFromISR(sem_handle, _))
        .WillOnce(
            ::testing::Invoke([](SemaphoreHandle_t, BaseType_t *higherPriorityTaskWoken) {
                *higherPriorityTaskWoken = pdTRUE;
                return pdTRUE;
            }))
        .WillRepeatedly(Return(pdFALSE));
    EXPECT_CALL(*mock, vSemaphoreDelete(sem_handle));

    freertos::condition_variable_any cv;
    auto result = cv.notify_all_isr();
    EXPECT_EQ(result.higher_priority_task_woken, pdTRUE);
}

#if configSUPPORT_STATIC_ALLOCATION
TEST_F(ConditionVariableMockTest, SaConditionVariableUsesStaticAllocation) {
    SemaphoreHandle_t sem_handle = reinterpret_cast<SemaphoreHandle_t>(0x5678);

    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, NotNull()))
        .WillOnce(Return(sem_handle));
    EXPECT_CALL(*mock, xSemaphoreGive(sem_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(sem_handle));

    freertos::sa::condition_variable_any cv;
    cv.notify_one();
}
#endif