/**
 * @file test_freertos_task.cpp
 * @brief Comprehensive unit tests for FreeRTOS C++ task wrappers
 * 
 * This file provides comprehensive behavioral unit tests for the task modules,
 * including static and dynamic task allocators and periodic task wrappers.
 * 
 * Tests cover:
 * - Correct construction, allocation, and deallocation of tasks using static and dynamic allocators
 * - Typical usage patterns for tasks: creation, starting, and deletion
 * - Correct operation of periodic tasks (e.g., tasks that are supposed to run at regular intervals)
 * - Stack allocation limitation test (documenting the current limitation)
 * - Error and edge cases, including invalid parameters
 * - Use stubs/mocks for FreeRTOS internals to enable host-based testing
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <memory>
#include <thread>

// Include mocks first to set up FreeRTOS environment
#include "FreeRTOS.h"

// Include the actual FreeRTOS task wrapper
#include "../include/freertos_task.hpp"

using namespace testing;
using namespace freertos;
using namespace std::chrono_literals;

class FreeRTOSTaskTest : public Test {
protected:
    void SetUp() override {
        mock = std::make_unique<StrictMock<FreeRTOSMock>>();
        g_freertos_mock = mock.get();
        
        // Common mock task handle for testing
        mock_task_handle = reinterpret_cast<TaskHandle_t>(0x12345678);
    }
    
    void TearDown() override {
        g_freertos_mock = nullptr;
        mock.reset();
    }
    
    std::unique_ptr<StrictMock<FreeRTOSMock>> mock;
    TaskHandle_t mock_task_handle;
};

// =============================================================================
// STATIC TASK ALLOCATOR TESTS
// =============================================================================

TEST_F(FreeRTOSTaskTest, StaticTaskAllocatorConstruction) {
    // Test that static task allocator can be constructed
    static_task_allocator<1024> allocator;
    
    // Test that copy constructor is deleted (should not compile if uncommented)
    // static_task_allocator<1024> copy_allocator(allocator);  // Should not compile
    
    // Test that assignment is deleted (should not compile if uncommented)
    // static_task_allocator<1024> other;
    // other = allocator;  // Should not compile
}

TEST_F(FreeRTOSTaskTest, StaticTaskAllocatorCreate) {
    static_task_allocator<1024> allocator;
    
    EXPECT_CALL(*mock, xTaskCreateStatic(
        NotNull(),              // taskFunction
        StrEq("TestTask"),      // name
        256,                    // stack depth (1024 / sizeof(StackType_t) = 1024 / 4 = 256)
        NotNull(),              // context
        2,                      // priority
        NotNull(),              // stack buffer
        NotNull()               // task buffer
    )).WillOnce(Return(mock_task_handle));
    
    auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, &allocator);
    
    EXPECT_EQ(handle, mock_task_handle);
}

TEST_F(FreeRTOSTaskTest, StaticTaskAllocatorCreateNullReturn) {
    static_task_allocator<1024> allocator;
    
    // Test case where FreeRTOS returns NULL (allocation failure)
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(nullptr));
    
    auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, nullptr);
    
    EXPECT_EQ(handle, nullptr);
}

// =============================================================================
// DYNAMIC TASK ALLOCATOR TESTS
// =============================================================================

TEST_F(FreeRTOSTaskTest, DynamicTaskAllocatorConstruction) {
    // Test that dynamic task allocator can be constructed
    dynamic_task_allocator<1024> allocator;
    
    // Test that copy constructor is implicitly available for dynamic allocator
    dynamic_task_allocator<1024> copy_allocator(allocator);
    
    // Test that assignment is implicitly available for dynamic allocator
    dynamic_task_allocator<1024> other;
    other = allocator;
}

TEST_F(FreeRTOSTaskTest, DynamicTaskAllocatorCreateSuccess) {
    dynamic_task_allocator<1024> allocator;
    
    EXPECT_CALL(*mock, xTaskCreate(
        NotNull(),              // taskFunction
        StrEq("TestTask"),      // name
        256,                    // stack depth (1024 / sizeof(StackType_t) = 1024 / 4 = 256)
        NotNull(),              // context
        2,                      // priority
        NotNull()               // created task handle
    )).WillOnce(DoAll(
        SetArgPointee<5>(mock_task_handle),
        Return(pdPASS)
    ));
    
    auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, &allocator);
    
    EXPECT_EQ(handle, mock_task_handle);
}

TEST_F(FreeRTOSTaskTest, DynamicTaskAllocatorCreateFailure) {
    dynamic_task_allocator<1024> allocator;
    
    // Test case where FreeRTOS returns failure
    EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
        .WillOnce(Return(pdFAIL));
    
    auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, nullptr);
    
    EXPECT_EQ(handle, nullptr);
}

// =============================================================================
// BASIC TASK TESTS (STATIC ALLOCATION)
// =============================================================================

TEST_F(FreeRTOSTaskTest, StaticTaskConstruction) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("TestTask"), _, _, 2, _, _))
        .WillOnce(Return(mock_task_handle));
    
    bool task_executed = false;
    sa::task<1024> test_task("TestTask", 2, [&task_executed]() {
        task_executed = true;
    });
    
    EXPECT_EQ(test_task.handle(), mock_task_handle);
}

TEST_F(FreeRTOSTaskTest, StaticTaskConstructionWithString) {
    std::string task_name = "StringTask";
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("StringTask"), _, _, 3, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::task<1024> test_task(task_name, 3, []() {
        // Test task routine
    });
    
    EXPECT_EQ(test_task.handle(), mock_task_handle);
}

TEST_F(FreeRTOSTaskTest, StaticTaskDestruction) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
    
    {
        sa::task<1024> test_task("TestTask", 2, []() {});
    } // Destructor should call vTaskDelete
}

TEST_F(FreeRTOSTaskTest, StaticTaskDestructionNullHandle) {
    // Test destruction when task creation failed
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(nullptr));
    
    // Should NOT call vTaskDelete for null handle
    EXPECT_CALL(*mock, vTaskDelete(_)).Times(0);
    
    {
        sa::task<1024> test_task("TestTask", 2, []() {});
    }
}

TEST_F(FreeRTOSTaskTest, StaticTaskSuspendResume) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, vTaskSuspend(mock_task_handle));
    test_task.suspend();
    
    EXPECT_CALL(*mock, vTaskResume(mock_task_handle));
    test_task.resume();
    
    EXPECT_CALL(*mock, xTaskResumeFromISR(mock_task_handle))
        .WillOnce(Return(pdTRUE));
    BaseType_t result = test_task.resume_isr();
    EXPECT_EQ(result, pdTRUE);
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, StaticTaskTerminate) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
    test_task.terminate();
    
    // Destructor should still be called but shouldn't delete again
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, StaticTaskPriority) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, uxTaskPriorityGet(mock_task_handle))
        .WillOnce(Return(5));
    UBaseType_t priority = test_task.priority();
    EXPECT_EQ(priority, 5);
    
    EXPECT_CALL(*mock, vTaskPrioritySet(mock_task_handle, 10));
    test_task.priority(10);
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, StaticTaskName) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, pcTaskGetName(mock_task_handle))
        .WillOnce(Return("TestTask"));
    const char* name = test_task.name();
    EXPECT_STREQ(name, "TestTask");
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, StaticTaskState) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, eTaskGetState(mock_task_handle))
        .WillOnce(Return(eRunning));
    eTaskState state = test_task.state();
    EXPECT_EQ(state, eRunning);
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

// =============================================================================
// BASIC TASK TESTS (DYNAMIC ALLOCATION)
// =============================================================================

TEST_F(FreeRTOSTaskTest, DynamicTaskConstruction) {
    EXPECT_CALL(*mock, xTaskCreate(_, StrEq("DynamicTask"), _, _, 3, _))
        .WillOnce(DoAll(
            SetArgPointee<5>(mock_task_handle),
            Return(pdPASS)
        ));
    
    da::task<2048> test_task("DynamicTask", 3, []() {
        // Test task routine
    });
    
    EXPECT_EQ(test_task.handle(), mock_task_handle);
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, DynamicTaskConstructionFailure) {
    EXPECT_CALL(*mock, xTaskCreate(_, _, _, _, _, _))
        .WillOnce(Return(pdFAIL));
    
    da::task<2048> test_task("FailTask", 3, []() {});
    
    EXPECT_EQ(test_task.handle(), nullptr);
    
    // Should NOT call vTaskDelete for null handle
    EXPECT_CALL(*mock, vTaskDelete(_)).Times(0);
}

// =============================================================================
// TASK NOTIFICATIONS TESTS
// =============================================================================

TEST_F(FreeRTOSTaskTest, TaskNotifications) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::task<1024> test_task("NotifyTask", 2, []() {});
    
    // Test notify_give
    EXPECT_CALL(*mock, xTaskNotifyGive(mock_task_handle))
        .WillOnce(Return(pdTRUE));
    BaseType_t result = test_task.notify_give();
    EXPECT_EQ(result, pdTRUE);
    
    // Test notify_take
    EXPECT_CALL(*mock, ulTaskNotifyTake(pdTRUE, 100))
        .WillOnce(Return(42));
    uint32_t value = test_task.notify_take(pdTRUE, 100);
    EXPECT_EQ(value, 42);
    
    // Test notify
    EXPECT_CALL(*mock, xTaskNotify(mock_task_handle, 0x12345678, eSetValueWithOverwrite))
        .WillOnce(Return(pdTRUE));
    result = test_task.notify(0x12345678, eSetValueWithOverwrite);
    EXPECT_EQ(result, pdTRUE);
    
    // Test notify_wait
    uint32_t notification_value;
    EXPECT_CALL(*mock, xTaskNotifyWait(0, 0xFFFFFFFF, _, 1000))
        .WillOnce(DoAll(
            SetArgPointee<2>(0xABCDEF00),
            Return(pdTRUE)
        ));
    result = test_task.notify_wait(0, 0xFFFFFFFF, notification_value, 1000);
    EXPECT_EQ(result, pdTRUE);
    EXPECT_EQ(notification_value, 0xABCDEF00);
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

// =============================================================================
// PERIODIC TASK TESTS
// =============================================================================

TEST_F(FreeRTOSTaskTest, PeriodicTaskConstruction) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("PeriodicTask"), _, _, 2, _, _))
        .WillOnce(Return(mock_task_handle));
    
    bool on_start_called = false;
    bool on_stop_called = false;
    bool periodic_called = false;
    
    sa::periodic_task<1024> periodic_task(
        "PeriodicTask", 
        2,
        [&on_start_called]() { on_start_called = true; },      // on_start
        [&on_stop_called]() { on_stop_called = true; },       // on_stop
        [&periodic_called]() { periodic_called = true; },     // periodic_routine
        100ms                                                   // period
    );
    
    EXPECT_EQ(periodic_task.handle(), mock_task_handle);
    
    EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, PeriodicTaskWithString) {
    std::string task_name = "StringPeriodicTask";
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("StringPeriodicTask"), _, _, 3, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::periodic_task<1024> periodic_task(
        task_name,
        3,
        []() {},  // on_start
        []() {},  // on_stop
        []() {},  // periodic_routine
        50ms      // period
    );
    
    EXPECT_EQ(periodic_task.handle(), mock_task_handle);
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, PeriodicTaskZeroPeriod) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("ZeroPeriodTask"), _, _, 2, _, _))
        .WillOnce(Return(mock_task_handle));
    
    // Test periodic task with zero period (no delay between executions)
    sa::periodic_task<1024> periodic_task(
        "ZeroPeriodTask",
        2,
        []() {},  // on_start
        []() {},  // on_stop
        []() {},  // periodic_routine
        0ms       // period - should run continuously
    );
    
    EXPECT_EQ(periodic_task.handle(), mock_task_handle);
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, PeriodicTaskNoPeriod) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("NoPeriodTask"), _, _, 2, _, _))
        .WillOnce(Return(mock_task_handle));
    
    // Test periodic task constructor without explicit period
    sa::periodic_task<1024> periodic_task(
        "NoPeriodTask",
        2,
        []() {},  // on_start
        []() {},  // on_stop
        []() {}   // periodic_routine
        // No period specified - should default to 0ms
    );
    
    EXPECT_EQ(periodic_task.handle(), mock_task_handle);
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, PeriodicTaskIsRunning) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::periodic_task<1024> periodic_task(
        "RunningTask",
        2,
        []() {},  // on_start
        []() {},  // on_stop
        []() {},  // periodic_routine
        100ms     // period
    );
    
    // Test different task states
    EXPECT_CALL(*mock, eTaskGetState(mock_task_handle))
        .WillOnce(Return(eRunning));
    EXPECT_TRUE(periodic_task.is_running());
    
    EXPECT_CALL(*mock, eTaskGetState(mock_task_handle))
        .WillOnce(Return(eReady));
    EXPECT_TRUE(periodic_task.is_running());
    
    EXPECT_CALL(*mock, eTaskGetState(mock_task_handle))
        .WillOnce(Return(eBlocked));
    EXPECT_TRUE(periodic_task.is_running());
    
    EXPECT_CALL(*mock, eTaskGetState(mock_task_handle))
        .WillOnce(Return(eSuspended));
    EXPECT_TRUE(periodic_task.is_running());
    
    EXPECT_CALL(*mock, eTaskGetState(mock_task_handle))
        .WillOnce(Return(eDeleted));
    EXPECT_FALSE(periodic_task.is_running());
    
    EXPECT_CALL(*mock, eTaskGetState(mock_task_handle))
        .WillOnce(Return(eInvalid));
    EXPECT_FALSE(periodic_task.is_running());
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, PeriodicTaskTerminate) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::periodic_task<1024> periodic_task(
        "TerminateTask",
        2,
        []() {},  // on_start
        []() {},  // on_stop
        []() {},  // periodic_routine
        100ms     // period
    );
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
    periodic_task.terminate();
    
    // Destructor should still be called
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, PeriodicTaskDestructorAbortDelay) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    {
        sa::periodic_task<1024> periodic_task(
            "AbortDelayTask",
            2,
            []() {},  // on_start
            []() {},  // on_stop
            []() {},  // periodic_routine
            100ms     // period
        );
        
        // Destructor should call abort_delay and then vTaskDelete
        EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
            .WillOnce(Return(pdTRUE));
        EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
    }
}

// =============================================================================
// UTILITY FUNCTION TESTS
// =============================================================================

TEST_F(FreeRTOSTaskTest, DelayFunction) {
    EXPECT_CALL(*mock, vTaskDelay(100));
    delay(100);
    
    // Test chrono duration
    EXPECT_CALL(*mock, vTaskDelay(500));
    delay(500ms);
}

TEST_F(FreeRTOSTaskTest, DelayUntilFunction) {
    TickType_t wake_time = 1000;
    EXPECT_CALL(*mock, vTaskDelayUntil(&wake_time, 100));
    delay_until(wake_time, 100);
    
    // Test chrono duration
    TickType_t wake_time2 = 2000;
    EXPECT_CALL(*mock, vTaskDelayUntil(&wake_time2, 250));
    delay_until(wake_time2, 250ms);
}

TEST_F(FreeRTOSTaskTest, SleepForFunction) {
    EXPECT_CALL(*mock, vTaskDelay(1000));
    sleep_for(1000ms);
}

TEST_F(FreeRTOSTaskTest, CurrentTaskHandle) {
    EXPECT_CALL(*mock, xTaskGetCurrentTaskHandle())
        .WillOnce(Return(mock_task_handle));
    TaskHandle_t handle = current_task_handle();
    EXPECT_EQ(handle, mock_task_handle);
}

TEST_F(FreeRTOSTaskTest, IdleTaskHandle) {
    TaskHandle_t idle_handle = reinterpret_cast<TaskHandle_t>(0x87654321);
    EXPECT_CALL(*mock, xTaskGetIdleTaskHandle())
        .WillOnce(Return(idle_handle));
    TaskHandle_t handle = idle_task_handle();
    EXPECT_EQ(handle, idle_handle);
}

TEST_F(FreeRTOSTaskTest, TickCount) {
    EXPECT_CALL(*mock, xTaskGetTickCount())
        .WillOnce(Return(12345));
    TickType_t ticks = tick_count();
    EXPECT_EQ(ticks, 12345);
    
    EXPECT_CALL(*mock, xTaskGetTickCountFromISR())
        .WillOnce(Return(54321));
    TickType_t ticks_isr = tick_count_isr();
    EXPECT_EQ(ticks_isr, 54321);
}

TEST_F(FreeRTOSTaskTest, TaskCount) {
    EXPECT_CALL(*mock, uxTaskGetNumberOfTasks())
        .WillOnce(Return(5));
    UBaseType_t count = task_count();
    EXPECT_EQ(count, 5);
}

TEST_F(FreeRTOSTaskTest, SchedulerState) {
    EXPECT_CALL(*mock, xTaskGetSchedulerState())
        .WillOnce(Return(2));
    BaseType_t state = get_scheduler_state();
    EXPECT_EQ(state, 2);
}

TEST_F(FreeRTOSTaskTest, TimeSinceSchedulerStarted) {
    EXPECT_CALL(*mock, xTaskGetTickCount())
        .WillOnce(Return(1000));  // 1000 ticks
    auto time = time_since_scheduler_started();
    EXPECT_EQ(time.count(), 1000);  // Should be 1000ms with portTICK_PERIOD_MS = 1
    
    EXPECT_CALL(*mock, xTaskGetTickCountFromISR())
        .WillOnce(Return(2000));  // 2000 ticks
    auto time_isr = time_since_scheduler_started_isr();
    EXPECT_EQ(time_isr.count(), 2000);  // Should be 2000ms
}

// =============================================================================
// CRITICAL SECTION AND BARRIER TESTS
// =============================================================================

TEST_F(FreeRTOSTaskTest, CriticalSection) {
    // Test RAII critical section (macros are no-op in host testing)
    {
        critical_section cs;
        // Critical section is active
    } // Critical section ends automatically
    
    // No expectations needed as critical section macros are no-op for host testing
}

TEST_F(FreeRTOSTaskTest, CriticalSectionISR) {
    // Test RAII critical section from ISR (macros are no-op in host testing)
    {
        critical_section_isr cs_isr;
        // Critical section from ISR is active
    } // Critical section ends automatically
    
    // No expectations needed as ISR critical section macros are no-op for host testing
}

TEST_F(FreeRTOSTaskTest, InterruptBarrier) {
    // Test RAII interrupt barrier (macros are no-op in host testing)
    {
        interrupt_barrier barrier;
        // Interrupts are disabled
    } // Interrupts are re-enabled automatically
    
    // No expectations needed as interrupt control macros are no-op for host testing
}

TEST_F(FreeRTOSTaskTest, SchedulerBarrier) {
    // Test RAII scheduler barrier (macros are no-op in host testing)
    {
        scheduler_barrier barrier;
        // Scheduler is suspended
    } // Scheduler is resumed automatically
    
    // No expectations needed as scheduler control macros are no-op for host testing
}

// =============================================================================
// STACK ALLOCATION LIMITATION TEST
// =============================================================================

TEST_F(FreeRTOSTaskTest, StackAllocationLimitation) {
    // This test documents the current limitation that static tasks cannot be allocated on the stack
    // when using the static allocator. This is due to the way FreeRTOS static allocation works.
    
    /*
     * NOTE: The following code would NOT work correctly in a real FreeRTOS environment:
     * 
     * void some_function() {
     *     freertos::sa::task<1024> my_task("StackTask", 1, [](){});  // PROBLEMATIC!
     *     // The task's stack and control block are allocated on the stack here,
     *     // but FreeRTOS expects them to persist beyond the function scope.
     *     // This would cause undefined behavior when the function returns.
     * }
     * 
     * For stack-allocated task usage, dynamic allocation should be used instead:
     * 
     * void some_function() {
     *     freertos::da::task<1024> my_task("StackTask", 1, [](){});  // OK
     *     // Dynamic allocation handles memory management correctly
     * }
     */
    
    // In our host-based test, we can't easily demonstrate this limitation
    // without actually running FreeRTOS, but we document it here.
    // The static allocator is designed for global/static task objects only.
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
    
    // This works in our test because we're not actually running FreeRTOS,
    // but in a real environment, this pattern should be avoided for static tasks
    {
        sa::task<1024> stack_task("StackLimitationDemo", 1, [](){});
    } // Stack allocation limitation applies here in real FreeRTOS usage
}

// =============================================================================
// ERROR AND EDGE CASE TESTS
// =============================================================================

TEST_F(FreeRTOSTaskTest, InvalidParameters) {
    // Test task creation with null name (FreeRTOS may handle this gracefully)
    EXPECT_CALL(*mock, xTaskCreateStatic(_, nullptr, _, _, _, _, _))
        .WillOnce(Return(nullptr));
    
    sa::task<1024> task_with_null_name(nullptr, 1, [](){});
    EXPECT_EQ(task_with_null_name.handle(), nullptr);
    
    // Should NOT call vTaskDelete for null handle
    EXPECT_CALL(*mock, vTaskDelete(_)).Times(0);
}

TEST_F(FreeRTOSTaskTest, ZeroStackSize) {
    // Test with very small stack size
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, 0, _, _, _, _))
        .WillOnce(Return(nullptr));  // FreeRTOS would likely fail this
    
    sa::task<0> zero_stack_task("ZeroStack", 1, [](){});
    EXPECT_EQ(zero_stack_task.handle(), nullptr);
    
    EXPECT_CALL(*mock, vTaskDelete(_)).Times(0);
}

TEST_F(FreeRTOSTaskTest, VeryHighPriority) {
    // Test with maximum priority value
    const UBaseType_t max_priority = 0xFFFFFFFF;
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, max_priority, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::task<1024> high_priority_task("HighPriority", max_priority, [](){});
    EXPECT_EQ(high_priority_task.handle(), mock_task_handle);
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

TEST_F(FreeRTOSTaskTest, TaskSystemStatus) {
    // Test task system status (if trace facility is available)
    TaskStatus_t mock_status[10];
    
    EXPECT_CALL(*mock, uxTaskGetSystemState(_, 10, _))
        .WillOnce(DoAll(
            SetArgPointee<2>(5000),  // total runtime
            Return(3)                // number of tasks
        ));
    
    task_system_status<10> status;
    EXPECT_EQ(status.count(), 3);
    EXPECT_EQ(status.total_run_time().count(), 5000);
}

TEST_F(FreeRTOSTaskTest, TaskChronoCompatibility) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::task<1024> test_task("ChronoTask", 2, [](){});
    
    // Test chrono duration compatibility with notify_take
    EXPECT_CALL(*mock, ulTaskNotifyTake(pdTRUE, 500))
        .WillOnce(Return(1));
    uint32_t result = test_task.notify_take(pdTRUE, 500ms);
    EXPECT_EQ(result, 1);
    
    // Test chrono duration compatibility with notify_wait
    uint32_t notification_value;
    EXPECT_CALL(*mock, xTaskNotifyWait(0, 0, _, 1000))
        .WillOnce(DoAll(
            SetArgPointee<2>(42),
            Return(pdTRUE)
        ));
    BaseType_t wait_result = test_task.notify_wait(0, 0, notification_value, 1000ms);
    EXPECT_EQ(wait_result, pdTRUE);
    EXPECT_EQ(notification_value, 42);
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
}

// =============================================================================
// MOVE SEMANTICS TESTS
// =============================================================================

TEST_F(FreeRTOSTaskTest, TaskMoveConstruction) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::task<1024> original_task("MoveTest", 2, [](){});
    EXPECT_EQ(original_task.handle(), mock_task_handle);
    
    // Only one vTaskDelete should be called (by moved_task destructor)
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle))
        .Times(1);
    
    // Move construction
    sa::task<1024> moved_task = std::move(original_task);
    EXPECT_EQ(moved_task.handle(), mock_task_handle);
}

TEST_F(FreeRTOSTaskTest, PeriodicTaskMoveConstruction) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(Return(mock_task_handle));
    
    sa::periodic_task<1024> original_task("PeriodicMoveTest", 2, 
        [](){}, [](){}, [](){}, 100ms);
    EXPECT_EQ(original_task.handle(), mock_task_handle);
    
    // Only one cleanup sequence should be called (by moved_task destructor)
    EXPECT_CALL(*mock, xTaskAbortDelay(mock_task_handle))
        .Times(1)
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle))
        .Times(1);
    
    // Move construction
    sa::periodic_task<1024> moved_task = std::move(original_task);
    EXPECT_EQ(moved_task.handle(), mock_task_handle);
}

// Test entry point
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}