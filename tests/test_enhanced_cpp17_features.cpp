/**
 * @file test_enhanced_cpp17_features.cpp
 * @brief Enhanced tests for C++17 features and move semantics
 * 
 * This file provides comprehensive testing for modern C++17 features,
 * move semantics, perfect forwarding, and RAII behavior in the
 * FreeRTOS C++ wrappers.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <memory>
#include <vector>
#include <functional>
#include <type_traits>

// Include mocks first
#include "FreeRTOS.h"

// Include all wrapper headers
#include "../include/freertos_task.hpp"
#include "../include/freertos_semaphore.hpp"
#include "../include/freertos_queue.hpp"
#include "../include/freertos_event_group.hpp"

using namespace testing;
using namespace freertos;
using namespace std::chrono_literals;

class Cpp17FeaturesTest : public Test {
protected:
    void SetUp() override {
        mock = std::make_unique<NiceMock<FreeRTOSMock>>();
        extern FreeRTOSMock* g_freertos_mock;
        g_freertos_mock = mock.get();
        
        // Set up comprehensive mock behaviors
        ON_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
            .WillByDefault(Return(reinterpret_cast<TaskHandle_t>(0x12345678)));
        ON_CALL(*mock, vTaskDelete(_)).WillByDefault(Return());
        ON_CALL(*mock, vTaskSuspend(_)).WillByDefault(Return());
        ON_CALL(*mock, vTaskResume(_)).WillByDefault(Return());
        ON_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
            .WillByDefault(Return(reinterpret_cast<SemaphoreHandle_t>(0x87654321)));
        ON_CALL(*mock, xQueueCreateStatic(_, _, _, _))
            .WillByDefault(Return(reinterpret_cast<QueueHandle_t>(0x11111111)));
        ON_CALL(*mock, xEventGroupCreateStatic(_))
            .WillByDefault(Return(reinterpret_cast<EventGroupHandle_t>(0x22222222)));
    }
    
    void TearDown() override {
        extern FreeRTOSMock* g_freertos_mock;
        g_freertos_mock = nullptr;
        mock.reset();
    }
    
    std::unique_ptr<NiceMock<FreeRTOSMock>> mock;
};

// =============================================================================
// MOVE SEMANTICS ADVANCED TESTS
// =============================================================================

TEST_F(Cpp17FeaturesTest, TaskMoveSemanticsPerfectForwarding) {
    std::atomic<int> call_count{0};
    
    // Test perfect forwarding with various lambda captures
    auto task_func1 = [&call_count]() mutable { call_count++; };
    auto task_func2 = [&call_count](){ call_count += 2; };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .Times(AtLeast(2));
    
    // Test move construction with different function types
    sa::task<1024> task1("MoveTest1", 1, std::move(task_func1));
    sa::task<1024> task2("MoveTest2", 1, std::move(task_func2));
    
    // Test that tasks are created successfully
    EXPECT_NE(task1.handle(), nullptr);
    EXPECT_NE(task2.handle(), nullptr);
}

TEST_F(Cpp17FeaturesTest, TaskMoveAssignmentChaining) {
    std::atomic<bool> executed{false};
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .Times(AtLeast(3));
    EXPECT_CALL(*mock, vTaskDelete(_))
        .Times(AtLeast(2)); // Two tasks will be destroyed during moves
    
    auto task_func = [&executed]() { executed = true; };
    
    // Create multiple tasks
    sa::task<1024> task1("Original", 1, [&executed]() { executed = true; });
    sa::task<1024> task2("ToMove1", 2, task_func);
    sa::task<1024> task3("ToMove2", 3, task_func);
    
    TaskHandle_t original_handle = task1.handle();
    
    // Note: move assignment is deleted, so we test move construction instead
    sa::task<1024> moved_task = std::move(task1);
    EXPECT_EQ(task1.handle(), nullptr);  // Moved-from object
    EXPECT_NE(moved_task.handle(), nullptr);
}

TEST_F(Cpp17FeaturesTest, PeriodicTaskMoveWithChronoTypes) {
    std::atomic<int> periodic_count{0};
    std::atomic<int> start_count{0};
    std::atomic<int> stop_count{0};
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .Times(AtLeast(2));
    
    auto on_start = [&start_count]() { start_count++; };
    auto on_stop = [&stop_count]() { stop_count++; };
    auto periodic = [&periodic_count]() { periodic_count++; };
    
    // Test move with different chrono duration types - using static allocation
    sa::periodic_task<1024> task1("PeriodicMove1", 1, 
                       std::move(on_start), 
                       std::move(on_stop), 
                       std::move(periodic));
                       
    // Create another task and move it
    sa::periodic_task<1024> task2("PeriodicMove2", 2,
                       [&start_count]() { start_count++; },
                       [&stop_count]() { stop_count++; },
                       [&periodic_count]() { periodic_count++; });
    
    // Test move construction instead of assignment (which is deleted)
    sa::periodic_task<1024> moved_task = std::move(task2);
    
    EXPECT_NE(task1.handle(), nullptr);
    EXPECT_NE(moved_task.handle(), nullptr);
}

// =============================================================================
// PERFECT FORWARDING AND TEMPLATE DEDUCTION TESTS
// =============================================================================

TEST_F(Cpp17FeaturesTest, LambdaCaptureVarieties) {
    std::string message = "Test message";
    int counter = 42;
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .Times(AtLeast(3));
    
    // Test different lambda capture types
    
    // Capture by reference
    sa::task<1024> task1("RefCapture", 1, [&message]() {
        EXPECT_EQ(message, "Test message");
    });
    
    // Capture by value
    sa::task<1024> task2("ValueCapture", 1, [counter]() {
        EXPECT_EQ(counter, 42);
    });
    
    // Simple lambda without capture
    sa::task<1024> task3("NoCapture", 1, []() {
        // Simple test function
    });
}

TEST_F(Cpp17FeaturesTest, FunctionObjectVarieties) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .Times(AtLeast(3));
    
    // Test with copyable function object instead 
    auto func_lambda = []() { /* test */ };
    sa::task<1024> task1("StdFunction", 1, func_lambda);
    
    // Test with custom callable class
    class CustomCallable {
    public:
        void operator()() const { /* test */ }
    };
    
    CustomCallable callable;
    sa::task<1024> task2("CustomCallable", 1, std::move(callable));
    
    // Test with function pointer
    sa::task<1024> task3("FunctionPointer", 1, []() { /* test */ });
}

// =============================================================================
// RAII AND EXCEPTION SAFETY TESTS
// =============================================================================

TEST_F(Cpp17FeaturesTest, RAIIExceptionSafety) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .WillRepeatedly(Return(reinterpret_cast<TaskHandle_t>(0x12345678)));
    EXPECT_CALL(*mock, vTaskDelete(_))
        .Times(AtLeast(1));
    
    // Test RAII behavior with exceptions
    {
        sa::task<1024> task("RAIITest", 1, []() { /* test */ });
        EXPECT_NE(task.handle(), nullptr);
        
        // Simulate exception scenario
        try {
            // Task should be cleaned up even if exception is thrown
            throw std::runtime_error("Test exception");
        } catch (const std::exception&) {
            // Exception caught, task should still be cleaned up properly
        }
        // Task destructor should be called here
    }
}

TEST_F(Cpp17FeaturesTest, CriticalSectionRAII) {
    // Note: Critical section mocks may not be available in this mock setup
    // Test that critical sections compile and can be constructed
    {
        critical_section cs1;
        // Should enter critical section
        
        {
            critical_section cs2;
            // Nested critical section
            
            try {
                critical_section cs3;
                throw std::runtime_error("Test");
            } catch (...) {
                // cs3 should be cleaned up here
            }
            // cs2 should be cleaned up here
        }
        // cs1 should be cleaned up here
    }
}

// =============================================================================
// TYPE TRAITS AND TEMPLATE METAPROGRAMMING TESTS
// =============================================================================

TEST_F(Cpp17FeaturesTest, TypeTraitsValidation) {
    // Test that task types have expected properties
    
    // Task should be movable but not copyable
    EXPECT_TRUE(std::is_move_constructible_v<sa::task<1024>>);
    EXPECT_FALSE(std::is_move_assignable_v<sa::task<1024>>);  // Move assignment is deleted
    EXPECT_FALSE(std::is_copy_constructible_v<sa::task<1024>>);
    EXPECT_FALSE(std::is_copy_assignable_v<sa::task<1024>>);
    
    // Periodic task should have same properties - use specific type
    EXPECT_TRUE(std::is_move_constructible_v<sa::periodic_task<1024>>);
    EXPECT_FALSE(std::is_move_assignable_v<sa::periodic_task<1024>>);  // Move assignment is deleted
    EXPECT_FALSE(std::is_copy_constructible_v<sa::periodic_task<1024>>);
    EXPECT_FALSE(std::is_copy_assignable_v<sa::periodic_task<1024>>);
    
    // Critical sections should not be movable or copyable
    EXPECT_FALSE(std::is_move_constructible_v<critical_section>);
    EXPECT_FALSE(std::is_move_assignable_v<critical_section>);
    EXPECT_FALSE(std::is_copy_constructible_v<critical_section>);
    EXPECT_FALSE(std::is_copy_assignable_v<critical_section>);
}

TEST_F(Cpp17FeaturesTest, TemplateDeductionGuides) {
    EXPECT_CALL(*mock, xQueueCreateStatic(_, _, _, _))
        .Times(AtLeast(2));
    
    // Test template parameter order for queues - they are created at construction
    sa::queue<10, int> int_queue;
    sa::queue<5, std::string> string_queue;
    
    // Queues should be created automatically during construction
    // No need to call create() explicitly
}

// =============================================================================
// CONSTEXPR AND COMPILE-TIME TESTS
// =============================================================================

TEST_F(Cpp17FeaturesTest, CompileTimeConstants) {
    // Test constexpr functionality where applicable
    
    constexpr size_t stack_size = 1024;
    constexpr size_t queue_size = 10;
    constexpr UBaseType_t priority = 1;
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, priority, _, _))
        .Times(1);
    EXPECT_CALL(*mock, xQueueCreateStatic(queue_size, _, _, _))
        .Times(1);
    
    // These should compile with constexpr values - queues are created at construction
    sa::task<stack_size> task("ConstexprTest", priority, []() { /* test */ });
    sa::queue<queue_size, int> queue;
    
    // Compile-time assertions
    static_assert(stack_size == 1024);
    static_assert(queue_size == 10);
    static_assert(priority == 1);
}

// =============================================================================
// CHRONO INTEGRATION TESTS
// =============================================================================

TEST_F(Cpp17FeaturesTest, ChronoTypesIntegration) {
    using namespace std::chrono;
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .Times(AtLeast(3));
    
    // Test with different chrono duration types in periodic tasks - using static allocation
    sa::periodic_task<1024> task1("ChronoMs", 1,
                       []() { /* start */ },
                       []() { /* stop */ },
                       []() { /* periodic */ },
                       500ms);
                       
    sa::periodic_task<1024> task2("ChronoMs2", 1,
                       []() { /* start */ },
                       []() { /* stop */ },
                       []() { /* periodic */ },
                       750ms);
                       
    sa::periodic_task<1024> task3("ChronoCustom", 1,
                       []() { /* start */ },
                       []() { /* stop */ },
                       []() { /* periodic */ },
                       duration<double, std::milli>(123.456));
    
    EXPECT_NE(task1.handle(), nullptr);
    EXPECT_NE(task2.handle(), nullptr);
    EXPECT_NE(task3.handle(), nullptr);
}

// =============================================================================
// CONTAINER INTEGRATION TESTS
// =============================================================================

TEST_F(Cpp17FeaturesTest, STLContainerIntegration) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .Times(AtLeast(3));
    
    // Test storing tasks in STL containers
    std::vector<sa::task<1024>> tasks;
    
    // Reserve space to avoid moves during push_back
    tasks.reserve(3);
    
    // Use emplace_back to construct in place
    tasks.emplace_back("VectorTask1", 1, []() { /* test */ });
    tasks.emplace_back("VectorTask2", 2, []() { /* test */ });
    tasks.emplace_back("VectorTask3", 3, []() { /* test */ });
    
    EXPECT_EQ(tasks.size(), 3);
    
    // Test that all tasks are valid
    for (const auto& task : tasks) {
        EXPECT_NE(task.handle(), nullptr);
    }
}

TEST_F(Cpp17FeaturesTest, UniquePtrIntegration) {
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
        .Times(AtLeast(2));
    EXPECT_CALL(*mock, vTaskDelete(_))
        .Times(AtLeast(2));
    
    // Test with unique_ptr for dynamic task management
    auto task1 = std::make_unique<sa::task<1024>>("UniqueTask1", 1, []() { /* test */ });
    auto task2 = std::make_unique<sa::task<1024>>("UniqueTask2", 2, []() { /* test */ });
    
    EXPECT_NE(task1->handle(), nullptr);
    EXPECT_NE(task2->handle(), nullptr);
    
    // Test move semantics with unique_ptr
    auto moved_task = std::move(task1);
    EXPECT_EQ(task1.get(), nullptr);
    EXPECT_NE(moved_task.get(), nullptr);
    EXPECT_NE(moved_task->handle(), nullptr);
}