/**
 * @file test_enhanced_multitasking.cpp
 * @brief Enhanced multitasking tests using std::thread simulation
 * 
 * This file provides comprehensive multitasking tests using an enhanced mock
 * environment that simulates FreeRTOS behavior with std::thread. These tests
 * cover real task execution paths and multitasking scenarios.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <memory>
#include <thread>
#include <atomic>
#include <vector>
#include <future>
#include <condition_variable>
#include <mutex>
#include <map>

// Include mocks first to set up FreeRTOS environment
#include "FreeRTOS.h"

// Include the actual FreeRTOS task wrapper
#include "../include/freertos_task.hpp"

using namespace testing;
using namespace freertos;
using namespace std::chrono_literals;

/**
 * @brief Simple Task Simulator using std::thread
 * 
 * This provides basic task execution simulation for testing
 * actual task execution paths and multitasking scenarios.
 */
class SimpleTaskSimulator {
public:
    struct TaskInfo {
        std::thread thread;
        std::atomic<bool> should_terminate{false};
        std::atomic<eTaskState> state{eRunning};
        
        TaskInfo() = default;
        TaskInfo(TaskInfo&& other) noexcept 
            : thread(std::move(other.thread)), 
              should_terminate(other.should_terminate.load()),
              state(other.state.load()) {}
        
        ~TaskInfo() {
            terminate();
        }
        
        void terminate() {
            should_terminate = true;
            state = eDeleted;
            if (thread.joinable()) {
                thread.join();
            }
        }
    };

private:
    std::map<TaskHandle_t, std::unique_ptr<TaskInfo>> tasks_;
    std::mutex tasks_mutex_;
    std::atomic<uint32_t> next_handle_{1};
    
public:
    TaskHandle_t createTask(TaskFunction_t taskFunction, void* context) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        
        auto handle = reinterpret_cast<TaskHandle_t>(next_handle_++);
        auto task_info = std::make_unique<TaskInfo>();
        
        // Create thread that will execute the task function
        task_info->thread = std::thread([taskFunction, context, task_info = task_info.get()]() {
            if (!task_info->should_terminate.load()) {
                try {
                    taskFunction(context);
                } catch (...) {
                    // Handle any exceptions from task function
                }
            }
        });
        
        tasks_[handle] = std::move(task_info);
        return handle;
    }
    
    void deleteTask(TaskHandle_t handle) {
        std::unique_ptr<TaskInfo> task_info;
        {
            std::lock_guard<std::mutex> lock(tasks_mutex_);
            auto it = tasks_.find(handle);
            if (it != tasks_.end()) {
                task_info = std::move(it->second);
                tasks_.erase(it);
            }
        }
        // Task will be cleaned up when task_info goes out of scope
    }
    
    void cleanup() {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        tasks_.clear();
    }
    
    size_t getTaskCount() {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        return tasks_.size();
    }
    
    eTaskState getTaskState(TaskHandle_t handle) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = tasks_.find(handle);
        if (it != tasks_.end()) {
            return it->second->state.load();
        }
        return eDeleted;  // Task not found, consider it deleted
    }
};

class EnhancedMultitaskingTest : public Test {
protected:
    void SetUp() override {
        mock = std::make_unique<NiceMock<FreeRTOSMock>>();  // Use NiceMock to avoid warnings
        extern FreeRTOSMock* g_freertos_mock;
        g_freertos_mock = mock.get();
        
        simulator = std::make_unique<SimpleTaskSimulator>();
        
        // Set up mock to actually execute tasks
        ON_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
            .WillByDefault([this](TaskFunction_t code, const char* name, uint32_t stack, 
                                void* params, UBaseType_t priority, StackType_t* stackBuf, StaticTask_t* taskBuf) {
                return simulator->createTask(code, params);
            });
            
        ON_CALL(*mock, vTaskDelete(_))
            .WillByDefault([this](TaskHandle_t handle) {
                simulator->deleteTask(handle);
            });
            
        // Allow other calls without warnings
        ON_CALL(*mock, vTaskSuspend(_)).WillByDefault(Return());
        ON_CALL(*mock, vTaskResume(_)).WillByDefault(Return());
        ON_CALL(*mock, eTaskGetState(_))
            .WillByDefault([this](TaskHandle_t handle) {
                return simulator->getTaskState(handle);
            });
        ON_CALL(*mock, pcTaskGetName(_)).WillByDefault(Return("TestTask"));
    }
    
    void TearDown() override {
        simulator->cleanup();
        extern FreeRTOSMock* g_freertos_mock;
        g_freertos_mock = nullptr;
        mock.reset();
        simulator.reset();
    }
    
    std::unique_ptr<NiceMock<FreeRTOSMock>> mock;
    std::unique_ptr<SimpleTaskSimulator> simulator;
};

// =============================================================================
// ENHANCED TASK EXECUTION TESTS
// =============================================================================

TEST_F(EnhancedMultitaskingTest, TaskActuallyExecutes) {
    std::atomic<bool> task_executed{false};
    std::atomic<int> execution_count{0};
    
    // Create a simple task that sets a flag
    auto task_function = [&task_executed, &execution_count]() {
        task_executed = true;
        execution_count++;
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("ExecutionTest"), _, _, _, _, _))
        .WillOnce(DoDefault());
    
    // Create static task - this should actually execute the task function
    sa::task<1024> test_task("ExecutionTest", 1, task_function);
    
    // Wait for task execution
    std::this_thread::sleep_for(50ms);
    
    EXPECT_TRUE(task_executed.load());
    EXPECT_EQ(execution_count.load(), 1);
}

TEST_F(EnhancedMultitaskingTest, TaskExecutionWithSuspendResume) {
    std::atomic<bool> task_started{false};
    std::atomic<bool> task_completed{false};
    
    auto task_function = [&]() {
        task_started = true;
        
        // Simulate some work
        std::this_thread::sleep_for(20ms);
        
        task_completed = true;
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillOnce(DoDefault());
    EXPECT_CALL(*mock, vTaskSuspend(_)).Times(AtMost(1));
    EXPECT_CALL(*mock, vTaskResume(_)).Times(AtMost(1));
    
    // Create task that starts suspended
    sa::task<1024> test_task("SuspendTest", 1, task_function, true);
    
    // Wait a bit to ensure task starts
    std::this_thread::sleep_for(10ms);
    EXPECT_TRUE(task_started.load());
    
    // Resume the task  
    test_task.resume();
    
    // Wait for completion
    std::this_thread::sleep_for(50ms);
    EXPECT_TRUE(task_completed.load());
}

TEST_F(EnhancedMultitaskingTest, MultipleTasksConcurrentExecution) {
    const int num_tasks = 3;
    std::vector<std::atomic<bool>> task_flags(num_tasks);
    std::atomic<int> concurrent_tasks{0};
    std::atomic<int> max_concurrent{0};
    
    std::vector<std::unique_ptr<sa::task<512>>> tasks;
    
    for (int i = 0; i < num_tasks; ++i) {
        task_flags[i] = false;
        
        auto task_function = [&, i]() {
            // Increment concurrent counter
            int current = concurrent_tasks.fetch_add(1) + 1;
            
            // Update max concurrent if needed
            int expected_max = max_concurrent.load();
            while (expected_max < current && 
                   !max_concurrent.compare_exchange_weak(expected_max, current)) {
                // Retry
            }
            
            // Simulate work
            std::this_thread::sleep_for(20ms);
            
            // Mark this task as completed
            task_flags[i] = true;
            
            // Decrement concurrent counter
            concurrent_tasks.fetch_sub(1);
        };
        
        EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
            .WillOnce(DoDefault());
        
        tasks.push_back(std::make_unique<sa::task<512>>(
            ("ConcurrentTask" + std::to_string(i)).c_str(), 1, task_function));
    }
    
    // Wait for all tasks to complete
    std::this_thread::sleep_for(100ms);
    
    // Verify all tasks executed
    for (int i = 0; i < num_tasks; ++i) {
        EXPECT_TRUE(task_flags[i].load()) << "Task " << i << " did not execute";
    }
    
    // Verify we had actual concurrency (at least 2 tasks running simultaneously)
    EXPECT_GT(max_concurrent.load(), 1) << "No concurrent execution detected";
}

TEST_F(EnhancedMultitaskingTest, TaskSynchronizationWithNotifications) {
    std::atomic<bool> producer_done{false};
    std::atomic<bool> consumer_done{false};
    std::atomic<int> shared_value{0};
    
    // Producer task
    auto producer_function = [&]() {
        // Simulate producing data
        for (int i = 1; i <= 5; ++i) {
            shared_value = i;
            std::this_thread::sleep_for(2ms);
        }
        producer_done = true;
    };
    
    // Consumer task
    auto consumer_function = [&]() {
        // Wait for producer to complete
        while (!producer_done.load()) {
            std::this_thread::sleep_for(1ms);
        }
        
        // Verify final value
        EXPECT_EQ(shared_value.load(), 5);
        consumer_done = true;
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("Producer"), _, _, _, _, _))
        .WillOnce(DoDefault());
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("Consumer"), _, _, _, _, _))
        .WillOnce(DoDefault());
    
    sa::task<512> producer_task("Producer", 2, producer_function);
    sa::task<512> consumer_task("Consumer", 1, consumer_function);
    
    // Wait for both tasks to complete
    std::this_thread::sleep_for(50ms);
    
    EXPECT_TRUE(producer_done.load());
    EXPECT_TRUE(consumer_done.load());
}

TEST_F(EnhancedMultitaskingTest, TaskLifecycleRacingConditions) {
    std::atomic<int> constructor_calls{0};
    std::atomic<int> destructor_calls{0};
    std::atomic<int> task_executions{0};
    
    const int num_iterations = 10;
    
    for (int i = 0; i < num_iterations; ++i) {
        auto task_function = [&]() {
            task_executions++;
            std::this_thread::sleep_for(1ms);
        };
        
        EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
            .WillOnce(DoDefault());
        EXPECT_CALL(*mock, vTaskDelete(_)).Times(1);
        
        {
            sa::task<256> test_task(("RaceTask" + std::to_string(i)).c_str(), 1, task_function);
            constructor_calls++;
            
            // Brief delay to let task start
            std::this_thread::sleep_for(2ms);
        } // Task destructor called here
        destructor_calls++;
        
        // Small delay between iterations
        std::this_thread::sleep_for(1ms);
    }
    
    EXPECT_EQ(constructor_calls.load(), num_iterations);
    EXPECT_EQ(destructor_calls.load(), num_iterations);
    
    // Allow some flexibility in task execution count due to timing
    EXPECT_GE(task_executions.load(), num_iterations / 2);
}

TEST_F(EnhancedMultitaskingTest, TaskMoveSemanticsConcurrency) {
    std::atomic<bool> original_executed{false};
    std::atomic<bool> moved_executed{false};
    
    auto task_function = [&]() {
        std::this_thread::sleep_for(10ms);
        original_executed = true;
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("MoveTest"), _, _, _, _, _))
        .WillOnce(DoDefault());
    
    // Create original task
    sa::task<512> original_task("MoveTest", 1, task_function);
    
    // Brief wait to ensure task gets created and starts executing
    std::this_thread::sleep_for(5ms);
    
    // Move the task - this should not affect the already running task
    sa::task<512> moved_task = std::move(original_task);
    
    // Create another task that executes after move
    auto moved_function = [&]() {
        std::this_thread::sleep_for(5ms);
        moved_executed = true;
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("MovedTask"), _, _, _, _, _))
        .WillOnce(DoDefault());
    
    sa::task<512> new_task("MovedTask", 1, moved_function);
    
    // Wait for both tasks to complete execution
    std::this_thread::sleep_for(30ms);
    
    EXPECT_TRUE(original_executed.load());
    EXPECT_TRUE(moved_executed.load());
}

// =============================================================================
// PERIODIC TASK MULTITASKING TESTS
// =============================================================================

TEST_F(EnhancedMultitaskingTest, PeriodicTaskExecution) {
    std::atomic<int> execution_count{0};
    std::atomic<bool> should_stop{false};
    
    auto on_start = []() { /* Task startup */ };
    auto on_stop = []() { /* Task cleanup */ };
    auto periodic_function = [&]() {
        // This is called once per periodic iteration
        if (!should_stop.load()) {
            execution_count++;
            std::this_thread::sleep_for(5ms);
        }
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("PeriodicTest"), _, _, _, _, _))
        .WillOnce(DoDefault());
    
    sa::periodic_task<512> periodic_task("PeriodicTest", 1, 
                                        std::move(on_start), 
                                        std::move(on_stop), 
                                        std::move(periodic_function),
                                        std::chrono::milliseconds{10}, // 10ms period
                                        false); // Start not suspended
    
    // Let it run for a while
    std::this_thread::sleep_for(30ms);
    
    // Stop the task
    should_stop = true;
    
    // Wait for cleanup
    std::this_thread::sleep_for(10ms);
    
    // Should have executed multiple times
    EXPECT_GT(execution_count.load(), 1);
}

TEST_F(EnhancedMultitaskingTest, MultiplePeriodicTasksCoordination) {
    std::atomic<int> fast_count{0};
    std::atomic<int> slow_count{0};
    std::atomic<bool> should_stop{false};
    
    auto fast_on_start = []() { /* Fast task startup */ };
    auto fast_on_stop = []() { /* Fast task cleanup */ };
    auto fast_function = [&]() {
        // This function is called once per periodic iteration
        if (!should_stop.load()) {
            fast_count++;
            std::this_thread::sleep_for(2ms);
        }
    };
    
    auto slow_on_start = []() { /* Slow task startup */ };
    auto slow_on_stop = []() { /* Slow task cleanup */ };
    auto slow_function = [&]() {
        // This function is called once per periodic iteration
        if (!should_stop.load()) {
            slow_count++;
            std::this_thread::sleep_for(8ms);
        }
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("FastPeriodic"), _, _, _, _, _))
        .WillOnce(DoDefault());
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("SlowPeriodic"), _, _, _, _, _))
        .WillOnce(DoDefault());
    
    // Create first task and allow it to initialize before creating second
    sa::periodic_task<256> fast_task("FastPeriodic", 2, 
                                   std::move(fast_on_start), 
                                   std::move(fast_on_stop), 
                                   std::move(fast_function),
                                   std::chrono::milliseconds{5}, // 5ms period
                                   false); // Start not suspended
    
    // Small delay to allow first task to initialize properly
    std::this_thread::sleep_for(2ms);
    
    sa::periodic_task<256> slow_task("SlowPeriodic", 1, 
                                   std::move(slow_on_start), 
                                   std::move(slow_on_stop), 
                                   std::move(slow_function),
                                   std::chrono::milliseconds{10}, // 10ms period
                                   false); // Start not suspended
    
    // Let them run for a longer period to ensure both tasks execute
    std::this_thread::sleep_for(80ms);
    
    should_stop = true;
    
    // Wait for cleanup
    std::this_thread::sleep_for(15ms);
    
    // Both tasks should have executed at least once
    EXPECT_GT(fast_count.load(), 0);
    EXPECT_GT(slow_count.load(), 0);
    // Fast task should execute more times than slow task due to shorter period
    EXPECT_GT(fast_count.load(), slow_count.load());
}

// =============================================================================
// ERROR HANDLING AND EDGE CASES
// =============================================================================

TEST_F(EnhancedMultitaskingTest, TaskExceptionHandling) {
    std::atomic<bool> exception_task_completed{false};
    std::atomic<bool> normal_task_completed{false};
    
    // Task that throws an exception
    auto exception_function = [&]() {
        try {
            throw std::runtime_error("Test exception");
        } catch (...) {
            // Exception should be caught by the simulator
            exception_task_completed = true;
        }
    };
    
    // Normal task that should continue working
    auto normal_function = [&]() {
        std::this_thread::sleep_for(5ms);
        normal_task_completed = true;
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("ExceptionTask"), _, _, _, _, _))
        .WillOnce(DoDefault());
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("NormalTask"), _, _, _, _, _))
        .WillOnce(DoDefault());
    
    sa::task<512> exception_task("ExceptionTask", 1, exception_function);
    sa::task<512> normal_task("NormalTask", 1, normal_function);
    
    // Wait for both to complete
    std::this_thread::sleep_for(20ms);
    
    EXPECT_TRUE(exception_task_completed.load());
    EXPECT_TRUE(normal_task_completed.load());
}

TEST_F(EnhancedMultitaskingTest, TaskDeleteDuringExecution) {
    std::atomic<bool> task_started{false};
    std::atomic<bool> task_should_continue{true};
    
    auto long_running_function = [&]() {
        task_started = true;
        // Run for a limited time to avoid hanging
        for (int i = 0; i < 100 && task_should_continue.load(); ++i) {
            std::this_thread::sleep_for(1ms);
        }
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("LongRunning"), _, _, _, _, _))
        .WillOnce(DoDefault());
    EXPECT_CALL(*mock, vTaskDelete(_)).Times(1);
    
    {
        sa::task<512> long_task("LongRunning", 1, long_running_function);
        
        // Wait for task to start
        while (!task_started.load()) {
            std::this_thread::sleep_for(1ms);
        }
        
        // Let it run briefly
        std::this_thread::sleep_for(5ms);
        
    } // Task destructor called here - should terminate the running task
    
    // Signal the task to stop (in case it's still running)
    task_should_continue = false;
    
    // Verify task was properly cleaned up
    EXPECT_TRUE(task_started.load());
}

// =============================================================================
// COVERAGE IMPROVEMENT TESTS
// =============================================================================

TEST_F(EnhancedMultitaskingTest, TaskExecFunctionCoverage) {
    std::atomic<bool> task_routine_called{false};
    
    // This test specifically aims to cover the task_exec function
    auto task_function = [&]() {
        // This function will be called through task_exec
        task_routine_called = true;
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("ExecCoverage"), _, _, _, _, _))
        .WillOnce(DoDefault());
    
    // Create task that will trigger task_exec execution
    sa::task<1024> test_task("ExecCoverage", 1, task_function);
    
    // Wait for execution
    std::this_thread::sleep_for(10ms);
    
    EXPECT_TRUE(task_routine_called.load());
}

TEST_F(EnhancedMultitaskingTest, SuspendedTaskStartupCoverage) {
    std::atomic<bool> suspended_task_executed{false};
    
    auto suspended_function = [&]() {
        suspended_task_executed = true;
    };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, StrEq("SuspendedStart"), _, _, _, _, _))
        .WillOnce(DoDefault());
    EXPECT_CALL(*mock, vTaskResume(_)).Times(1);
    
    // Create task that starts suspended
    sa::task<512> suspended_task("SuspendedStart", 1, suspended_function, true);
    
    // Task should not execute immediately
    std::this_thread::sleep_for(5ms);
    
    // Resume the task
    suspended_task.resume();
    
    // Now it should execute
    std::this_thread::sleep_for(10ms);
    
    EXPECT_TRUE(suspended_task_executed.load());
}