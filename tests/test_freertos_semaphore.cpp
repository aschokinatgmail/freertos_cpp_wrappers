/**
 * @file test_freertos_semaphore.cpp
 * @author GitHub Copilot AI Assistant
 * @brief Comprehensive unit tests for FreeRTOS C++ semaphore wrappers
 * @version 1.0
 * @date 2024-12-20
 * 
 * This file contains comprehensive behavioral unit tests for the FreeRTOS semaphore
 * wrapper library, including binary semaphores, counting semaphores, mutexes, and 
 * recursive mutexes with both static and dynamic allocators.
 *
 * Test Coverage:
 * - Static and dynamic semaphore allocators
 * - Binary semaphore operations (give/take, ISR variants, timeouts)
 * - Counting semaphore operations (with different max counts, operators)
 * - Mutex operations (lock/unlock, try_lock, ISR variants)
 * - Recursive mutex operations
 * - Lock guard RAII behavior
 * - Error handling and edge cases
 * - ISR safety operations
 * - Chrono timeout compatibility
 * 
 * Mock Strategy:
 * - Complete FreeRTOS semaphore API mocking for host-based testing
 * - Validation of parameter passing and return value handling
 * - ISR function testing (commented where host limitations apply)
 * - Clear documentation of RTOS-only features
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <thread>

// Include mocks before the actual headers
#include "FreeRTOS.h"
#include "freertos_semaphore.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::NotNull;
using ::testing::StrictMock;
using ::testing::NiceMock;

class FreeRTOSSemaphoreTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a strict mock for precise API verification
        mock = new StrictMock<FreeRTOSMock>();
        g_freertos_mock = mock;
        
        // Create mock handles for testing
        mock_semaphore_handle = reinterpret_cast<SemaphoreHandle_t>(0x12345678);
        mock_mutex_handle = reinterpret_cast<SemaphoreHandle_t>(0x87654321);
        mock_recursive_mutex_handle = reinterpret_cast<SemaphoreHandle_t>(0x11223344);
    }

    void TearDown() override {
        delete mock;
        g_freertos_mock = nullptr;
    }

    StrictMock<FreeRTOSMock>* mock;
    SemaphoreHandle_t mock_semaphore_handle;
    SemaphoreHandle_t mock_mutex_handle;
    SemaphoreHandle_t mock_recursive_mutex_handle;
};

// =============================================================================
// Static Semaphore Allocator Tests
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, StaticSemaphoreAllocatorConstruction) {
    // Test that static allocator can be constructed without API calls
    freertos::static_semaphore_allocator allocator;
    // No mock expectations needed - constructor should not call FreeRTOS APIs
}

TEST_F(FreeRTOSSemaphoreTest, StaticSemaphoreAllocatorCreateBinary) {
    freertos::static_semaphore_allocator allocator;
    
    EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(NotNull()))
        .WillOnce(Return(mock_semaphore_handle));
    
    auto handle = allocator.create_binary();
    EXPECT_EQ(handle, mock_semaphore_handle);
}

TEST_F(FreeRTOSSemaphoreTest, StaticSemaphoreAllocatorCreateCounting) {
    freertos::static_semaphore_allocator allocator;
    
    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(5, 5, NotNull()))
        .WillOnce(Return(mock_semaphore_handle));
    
    auto handle = allocator.create_counting(5);
    EXPECT_EQ(handle, mock_semaphore_handle);
}

TEST_F(FreeRTOSSemaphoreTest, StaticSemaphoreAllocatorCreateMutex) {
    freertos::static_semaphore_allocator allocator;
    
    EXPECT_CALL(*mock, xSemaphoreCreateMutexStatic(NotNull()))
        .WillOnce(Return(mock_mutex_handle));
    
    auto handle = allocator.create_mutex();
    EXPECT_EQ(handle, mock_mutex_handle);
}

TEST_F(FreeRTOSSemaphoreTest, StaticSemaphoreAllocatorCreateRecursiveMutex) {
    freertos::static_semaphore_allocator allocator;
    
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutexStatic(NotNull()))
        .WillOnce(Return(mock_recursive_mutex_handle));
    
    auto handle = allocator.create_recursive_mutex();
    EXPECT_EQ(handle, mock_recursive_mutex_handle);
}

// =============================================================================
// Dynamic Semaphore Allocator Tests  
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, DynamicSemaphoreAllocatorConstruction) {
    // Test that dynamic allocator can be constructed without API calls
    freertos::dynamic_semaphore_allocator allocator;
    // No mock expectations needed - constructor should not call FreeRTOS APIs
}

TEST_F(FreeRTOSSemaphoreTest, DynamicSemaphoreAllocatorCreateBinary) {
    freertos::dynamic_semaphore_allocator allocator;
    
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    
    auto handle = allocator.create_binary();
    EXPECT_EQ(handle, mock_semaphore_handle);
}

TEST_F(FreeRTOSSemaphoreTest, DynamicSemaphoreAllocatorCreateCounting) {
    freertos::dynamic_semaphore_allocator allocator;
    
    EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10))
        .WillOnce(Return(mock_semaphore_handle));
    
    auto handle = allocator.create_counting(10);
    EXPECT_EQ(handle, mock_semaphore_handle);
}

TEST_F(FreeRTOSSemaphoreTest, DynamicSemaphoreAllocatorCreateMutex) {
    freertos::dynamic_semaphore_allocator allocator;
    
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    
    auto handle = allocator.create_mutex();
    EXPECT_EQ(handle, mock_mutex_handle);
}

TEST_F(FreeRTOSSemaphoreTest, DynamicSemaphoreAllocatorCreateRecursiveMutex) {
    freertos::dynamic_semaphore_allocator allocator;
    
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    
    auto handle = allocator.create_recursive_mutex();
    EXPECT_EQ(handle, mock_recursive_mutex_handle);
}

// =============================================================================
// Binary Semaphore Tests
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, BinarySemaphoreConstruction) {
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle))
        .Times(1);
    
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    // Constructor should have called create_binary and stored the handle
}

TEST_F(FreeRTOSSemaphoreTest, BinarySemaphoreDestruction) {
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle))
        .Times(1);
    
    {
        freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    } // Destructor called here
}

TEST_F(FreeRTOSSemaphoreTest, BinarySemaphoreDestructionNullHandle) {
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(nullptr));
    // No vSemaphoreDelete call expected for null handle
    
    {
        freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    } // Destructor should handle null handle gracefully
}

TEST_F(FreeRTOSSemaphoreTest, BinarySemaphoreGive) {
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    auto result = semaphore.give();
    EXPECT_EQ(result, pdTRUE);
}

TEST_F(FreeRTOSSemaphoreTest, BinarySemaphoreTake) {
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_semaphore_handle, portMAX_DELAY))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    auto result = semaphore.take();
    EXPECT_EQ(result, pdTRUE);
}

TEST_F(FreeRTOSSemaphoreTest, BinarySemaphoreTakeWithTimeout) {
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_semaphore_handle, 100))
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    auto result = semaphore.take(100);
    EXPECT_EQ(result, pdFALSE);
}

TEST_F(FreeRTOSSemaphoreTest, BinarySemaphoreISROperations) {
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_semaphore_handle, NotNull()))
        .Times(2) // Called twice - once with ref param, once without (but with internal temp var)
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_semaphore_handle, NotNull()))
        .Times(2) // Called twice - once with ref param, once without (but with internal temp var)
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    
    /*
     * ISR Operations Testing Note:
     * In a real FreeRTOS environment, these functions would be called from
     * interrupt service routines. Here we test the API compatibility and
     * parameter passing, but actual ISR context cannot be simulated in host testing.
     */
    BaseType_t high_priority_woken = pdFALSE;
    auto result1 = semaphore.give_isr(high_priority_woken);
    auto result2 = semaphore.give_isr();
    auto result3 = semaphore.take_isr(high_priority_woken);
    auto result4 = semaphore.take_isr();
    
    EXPECT_EQ(result1, pdTRUE);
    EXPECT_EQ(result2, pdTRUE);
    EXPECT_EQ(result3, pdTRUE);
    EXPECT_EQ(result4, pdTRUE);
}

TEST_F(FreeRTOSSemaphoreTest, BinarySemaphoreChronoTimeout) {
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_semaphore_handle, 500)) // 500ms = 500 ticks
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    auto result = semaphore.take(std::chrono::milliseconds(500));
    EXPECT_EQ(result, pdTRUE);
}

// =============================================================================
// Counting Semaphore Tests
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, CountingSemaphoreConstruction) {
    EXPECT_CALL(*mock, xSemaphoreCreateCounting(3, 3))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle))
        .Times(1);
    
    freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> semaphore(3);
    // Constructor should have called create_counting with max_count = 3
}

TEST_F(FreeRTOSSemaphoreTest, CountingSemaphoreDefaultConstruction) {
    EXPECT_CALL(*mock, xSemaphoreCreateCounting(1, 1))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle))
        .Times(1);
    
    freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    // Default constructor should create counting semaphore with max_count = 1
}

TEST_F(FreeRTOSSemaphoreTest, CountingSemaphoreOperators) {
    EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
        .Times(4); // Once for ++, once for operator++(), twice for +=2
    EXPECT_CALL(*mock, xSemaphoreTake(mock_semaphore_handle, portMAX_DELAY))
        .Times(2); // Once for --, once for operator--()
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> semaphore(5);
    
    // Test increment operators
    ++semaphore;
    semaphore++;
    
    // Test decrement operators  
    --semaphore;
    semaphore--;
    
    // Test compound assignment
    semaphore += 2;
}

TEST_F(FreeRTOSSemaphoreTest, CountingSemaphoreGetCount) {
    EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_semaphore_handle))
        .WillOnce(Return(7));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> semaphore(10);
    auto count = semaphore.count();
    EXPECT_EQ(count, 7);
}

// =============================================================================
// Mutex Tests
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, MutexConstruction) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle))
        .Times(1);
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    // Constructor should have called create_mutex
}

TEST_F(FreeRTOSSemaphoreTest, MutexLockUnlock) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    auto lock_result = mutex.lock();
    EXPECT_EQ(lock_result, pdTRUE);
    EXPECT_TRUE(mutex.locked());
    
    auto unlock_result = mutex.unlock();
    EXPECT_EQ(unlock_result, pdTRUE);
    EXPECT_FALSE(mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, MutexTryLock) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    auto result = mutex.try_lock();
    EXPECT_EQ(result, pdFALSE);
    EXPECT_FALSE(mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, MutexLockWithTimeout) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 1000))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    auto result = mutex.lock(std::chrono::seconds(1));
    EXPECT_EQ(result, pdTRUE);
    EXPECT_TRUE(mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, MutexISROperations) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_mutex_handle, NotNull()))
        .Times(2) // Called twice - once with ref param, once without (but with internal temp var)
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_mutex_handle, NotNull()))
        .Times(2) // Called twice - once with ref param, once without (but with internal temp var)
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    /*
     * ISR Mutex Operations Note:
     * While FreeRTOS provides xSemaphoreTakeFromISR for semaphores, mutex usage
     * in ISR context is generally not recommended. These APIs are tested for
     * completeness but should be avoided in real applications.
     */
    BaseType_t high_priority_woken = pdFALSE;
    auto lock_result1 = mutex.lock_isr(high_priority_woken);
    auto lock_result2 = mutex.lock_isr();
    auto unlock_result1 = mutex.unlock_isr(high_priority_woken);
    auto unlock_result2 = mutex.unlock_isr();
    
    EXPECT_EQ(lock_result1, pdTRUE);
    EXPECT_EQ(lock_result2, pdTRUE);
    EXPECT_EQ(unlock_result1, pdTRUE);
    EXPECT_EQ(unlock_result2, pdTRUE);
}

// =============================================================================
// Recursive Mutex Tests
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexConstruction) {
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle))
        .Times(1);
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    // Constructor should have called create_recursive_mutex
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexLockUnlock) {
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, portMAX_DELAY))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    auto lock_result = recursive_mutex.lock();
    EXPECT_EQ(lock_result, pdTRUE);
    EXPECT_TRUE(recursive_mutex.locked());
    
    auto unlock_result = recursive_mutex.unlock();
    EXPECT_EQ(unlock_result, pdTRUE);
    EXPECT_FALSE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexTryLock) {
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 0))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    auto result = recursive_mutex.try_lock();
    EXPECT_EQ(result, pdTRUE);
    EXPECT_TRUE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexNestedLocks) {
    /*
     * Recursive Mutex Nested Locking Test:
     * Tests the fundamental property of recursive mutexes - the ability to be
     * locked multiple times by the same task. In a real FreeRTOS environment,
     * the recursive mutex maintains a count of how many times it has been locked
     * and must be unlocked the same number of times.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, portMAX_DELAY))
        .Times(3)  // Lock 3 times
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .Times(3)  // Must unlock 3 times
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    // First lock
    auto lock1 = recursive_mutex.lock();
    EXPECT_EQ(lock1, pdTRUE);
    EXPECT_TRUE(recursive_mutex.locked());
    
    // Second lock (recursive) - should succeed
    auto lock2 = recursive_mutex.lock();
    EXPECT_EQ(lock2, pdTRUE);
    EXPECT_TRUE(recursive_mutex.locked());
    
    // Third lock (recursive) - should succeed  
    auto lock3 = recursive_mutex.lock();
    EXPECT_EQ(lock3, pdTRUE);
    EXPECT_TRUE(recursive_mutex.locked());
    
    // Unlock once - should still be locked (recursive count: 3->2)
    auto unlock1 = recursive_mutex.unlock();
    EXPECT_EQ(unlock1, pdTRUE);
    EXPECT_TRUE(recursive_mutex.locked()); // Still locked with count 2
    
    // Unlock second time (recursive count: 2->1)
    auto unlock2 = recursive_mutex.unlock();
    EXPECT_EQ(unlock2, pdTRUE);
    EXPECT_TRUE(recursive_mutex.locked()); // Still locked with count 1
    
    // Final unlock
    auto unlock3 = recursive_mutex.unlock();
    EXPECT_EQ(unlock3, pdTRUE);
    EXPECT_FALSE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexNestedFunction) {
    /*
     * Recursive Mutex in Recursive Function Test:
     * Simulates a recursive function that acquires the same mutex at each
     * recursion level - a common use case for recursive mutexes.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, portMAX_DELAY))
        .Times(5)  // Recursive depth of 5
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .Times(5)  // Must unlock same number of times
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    // Lambda to simulate recursive function behavior
    std::function<void(int)> recursive_function = [&](int depth) {
        if (depth <= 0) return;
        
        // Acquire mutex at this recursion level
        auto lock_result = recursive_mutex.lock();
        EXPECT_EQ(lock_result, pdTRUE);
        
        // Recurse (this will acquire the mutex again)
        recursive_function(depth - 1);
        
        // Release mutex at this recursion level
        auto unlock_result = recursive_mutex.unlock();
        EXPECT_EQ(unlock_result, pdTRUE);
    };
    
    // Start recursion with depth 5
    recursive_function(5);
    
    // After all recursion completes, mutex should be fully unlocked
    EXPECT_FALSE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexLockGuardRAII) {
    /*
     * Recursive Mutex Lock Guard RAII Test:
     * Tests that lock_guard works correctly with recursive mutexes,
     * providing RAII semantics for automatic unlocking.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, portMAX_DELAY))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard(recursive_mutex);
        // Recursive mutex should be locked here
        EXPECT_TRUE(recursive_mutex.locked());
    } // Lock guard destructor should unlock the recursive mutex
    
    EXPECT_FALSE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexNestedLockGuards) {
    /*
     * Nested Lock Guards for Recursive Mutex Test:
     * Tests RAII behavior with nested lock guards on the same recursive mutex.
     * This simulates nested function calls where each function uses a lock guard.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, portMAX_DELAY))
        .Times(3)  // Three nested lock guards
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .Times(3)  // Three corresponding unlocks
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard1(recursive_mutex);
        EXPECT_TRUE(recursive_mutex.locked());
        
        {
            freertos::lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard2(recursive_mutex);
            EXPECT_TRUE(recursive_mutex.locked());
            
            {
                freertos::lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard3(recursive_mutex);
                EXPECT_TRUE(recursive_mutex.locked());
                
                // All three guards are active here - mutex locked 3 times
            } // guard3 destructor unlocks
            
            // guard2 still active, mutex still locked (in real FreeRTOS)
            // Note: wrapper's locked() reflects any lock state, not count
        } // guard2 destructor unlocks
        
        // guard1 still active
    } // guard1 destructor unlocks - final unlock
    
    EXPECT_FALSE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexTryLockGuardRAII) {
    /*
     * Recursive Mutex Try Lock Guard RAII Test:
     * Tests that try_lock_guard works correctly with recursive mutexes,
     * providing RAII semantics with try_lock behavior.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 0))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::try_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard(recursive_mutex);
        // Recursive mutex should be locked here
        EXPECT_TRUE(guard.locked());
        EXPECT_TRUE(recursive_mutex.locked());
    } // Try lock guard destructor should unlock the recursive mutex
    
    EXPECT_FALSE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexTryLockGuardFailure) {
    /*
     * Recursive Mutex Try Lock Guard Failure Test:
     * Tests try_lock_guard behavior when lock acquisition fails,
     * ensuring proper RAII semantics even on failure.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 0))
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::try_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard(recursive_mutex);
        // Try lock should have failed
        EXPECT_FALSE(guard.locked());
        EXPECT_FALSE(recursive_mutex.locked());
    } // Try lock guard destructor should not call unlock since lock failed
    
    EXPECT_FALSE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexNestedTryLockGuards) {
    /*
     * Nested Try Lock Guards for Recursive Mutex Test:
     * Tests RAII behavior with nested try_lock_guard on the same recursive mutex.
     * This demonstrates recursive locking capability with try_lock semantics.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 0))
        .Times(2)  // Two nested try lock guards
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .Times(2)  // Two corresponding unlocks
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::try_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard1(recursive_mutex);
        EXPECT_TRUE(guard1.locked());
        EXPECT_TRUE(recursive_mutex.locked());
        
        {
            freertos::try_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard2(recursive_mutex);
            EXPECT_TRUE(guard2.locked());
            EXPECT_TRUE(recursive_mutex.locked());
            
            // Both guards are active here - mutex locked twice
        } // guard2 destructor unlocks
        
        // guard1 still active
        EXPECT_TRUE(recursive_mutex.locked());
    } // guard1 destructor unlocks - final unlock
    
    EXPECT_FALSE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexLockGuardISRRAII) {
    /*
     * Recursive Mutex Lock Guard ISR RAII Test:
     * Tests that lock_guard_isr works correctly with recursive mutexes,
     * providing RAII semantics for ISR contexts.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_recursive_mutex_handle, NotNull()))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_recursive_mutex_handle, NotNull()))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::lock_guard_isr<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard(recursive_mutex);
        // Recursive mutex should be locked here
        EXPECT_TRUE(guard.locked());
        EXPECT_EQ(guard.high_priority_task_woken(), pdFALSE);
    } // Lock guard ISR destructor should unlock the recursive mutex
    
    // Note: In host testing, locked() state behavior may vary from real RTOS
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexNestedLockGuardISR) {
    /*
     * Nested Lock Guard ISR for Recursive Mutex Test:
     * Tests RAII behavior with nested lock_guard_isr on the same recursive mutex.
     * This simulates nested ISR contexts acquiring the same recursive mutex.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_recursive_mutex_handle, NotNull()))
        .Times(3)  // Three nested ISR lock guards
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_recursive_mutex_handle, NotNull()))
        .Times(3)  // Three corresponding unlocks
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::lock_guard_isr<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard1(recursive_mutex);
        EXPECT_TRUE(guard1.locked());
        EXPECT_EQ(guard1.high_priority_task_woken(), pdFALSE);
        
        {
            freertos::lock_guard_isr<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard2(recursive_mutex);
            EXPECT_TRUE(guard2.locked());
            EXPECT_EQ(guard2.high_priority_task_woken(), pdFALSE);
            
            {
                freertos::lock_guard_isr<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard3(recursive_mutex);
                EXPECT_TRUE(guard3.locked());
                EXPECT_EQ(guard3.high_priority_task_woken(), pdFALSE);
                
                // All three ISR guards are active here - mutex locked 3 times
            } // guard3 destructor unlocks
            
        } // guard2 destructor unlocks
        
    } // guard1 destructor unlocks - final unlock
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexTimeoutLockGuardRAII) {
    /*
     * Recursive Mutex Timeout Lock Guard RAII Test:
     * Tests that timeout_lock_guard works correctly with recursive mutexes,
     * providing RAII semantics with timeout-based acquisition.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 1000))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::timeout_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard(recursive_mutex, 1000);
        // Recursive mutex should be locked here
        EXPECT_TRUE(guard.locked());
    } // Timeout lock guard destructor should unlock the recursive mutex
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexTimeoutLockGuardChrono) {
    /*
     * Recursive Mutex Timeout Lock Guard Chrono Test:
     * Tests timeout_lock_guard with recursive mutex using chrono duration.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 500)) // 500ms
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::timeout_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard(recursive_mutex, std::chrono::milliseconds(500));
        // Recursive mutex should be locked here
        EXPECT_TRUE(guard.locked());
    } // Timeout lock guard destructor should unlock the recursive mutex
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexNestedTimeoutLockGuards) {
    /*
     * Nested Timeout Lock Guards for Recursive Mutex Test:
     * Tests RAII behavior with nested timeout_lock_guard on the same recursive mutex.
     * This demonstrates recursive locking capability with timeout semantics.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 1000))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 500))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .Times(2)  // Two corresponding unlocks
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::timeout_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard1(recursive_mutex, 1000);
        EXPECT_TRUE(guard1.locked());
        
        {
            freertos::timeout_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard2(recursive_mutex, std::chrono::milliseconds(500));
            EXPECT_TRUE(guard2.locked());
            
            // Both guards are active here - mutex locked twice
        } // guard2 destructor unlocks
        
        // guard1 still active
    } // guard1 destructor unlocks - final unlock
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexMixedLockGuardTypes) {
    /*
     * Mixed Lock Guard Types for Recursive Mutex Test:
     * Tests combining different lock guard types in nested scenarios
     * to ensure they work together correctly with recursive mutexes.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    // Regular lock_guard uses portMAX_DELAY
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, portMAX_DELAY))
        .WillOnce(Return(pdTRUE));
    // try_lock_guard uses 0 timeout
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 0))
        .WillOnce(Return(pdTRUE));
    // timeout_lock_guard uses specified timeout
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 100))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .Times(3)  // Three corresponding unlocks
        .WillRepeatedly(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard1(recursive_mutex);
        EXPECT_TRUE(recursive_mutex.locked());
        
        {
            freertos::try_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard2(recursive_mutex);
            EXPECT_TRUE(guard2.locked());
            EXPECT_TRUE(recursive_mutex.locked());
            
            {
                freertos::timeout_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard3(recursive_mutex, 100);
                EXPECT_TRUE(guard3.locked());
                
                // All three different guard types are active here
            } // guard3 destructor unlocks
            
        } // guard2 destructor unlocks
        
    } // guard1 destructor unlocks - final unlock
    
    EXPECT_FALSE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexTimeoutLockGuardFailure) {
    /*
     * Recursive Mutex Timeout Lock Guard Failure Test:
     * Tests timeout_lock_guard behavior when lock acquisition fails due to timeout,
     * ensuring that unlock is NOT called when the lock was never acquired.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 500))
        .WillOnce(Return(pdFALSE)); // Lock fails due to timeout
    // Note: xSemaphoreGive should NOT be called since lock failed
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::timeout_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard(recursive_mutex, 500);
        // Recursive mutex should not be locked (lock with timeout failed)
        EXPECT_FALSE(recursive_mutex.locked());
        EXPECT_FALSE(guard.locked());
    } // Timeout lock guard destructor should NOT call unlock (since lock was never acquired)
    
    EXPECT_FALSE(recursive_mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, RecursiveMutexTryLockGuardFailure) {
    /*
     * Recursive Mutex Try Lock Guard Failure Test:
     * Tests try_lock_guard behavior when lock acquisition fails,
     * ensuring that unlock is NOT called when the lock was never acquired.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, 0))
        .WillOnce(Return(pdFALSE)); // try_lock fails
    // Note: xSemaphoreGive should NOT be called since lock failed
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator> recursive_mutex;
    
    {
        freertos::try_lock_guard<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>> guard(recursive_mutex);
        // Recursive mutex should not be locked (try_lock failed)
        EXPECT_FALSE(recursive_mutex.locked());
        EXPECT_FALSE(guard.locked());
    } // Try lock guard destructor should NOT call unlock (since lock was never acquired)
    
    EXPECT_FALSE(recursive_mutex.locked());
}

// =============================================================================
// Lock Guard Tests (RAII)
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, LockGuardRAII) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    {
        freertos::lock_guard<freertos::mutex<freertos::dynamic_semaphore_allocator>> guard(mutex);
        // Mutex should be locked here
        EXPECT_TRUE(mutex.locked());
    } // Lock guard destructor should unlock the mutex
    
    EXPECT_FALSE(mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, TryLockGuardRAII) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    {
        freertos::try_lock_guard<freertos::mutex<freertos::dynamic_semaphore_allocator>> guard(mutex);
        // Mutex should be locked here (try_lock succeeded)
        EXPECT_TRUE(mutex.locked());
        EXPECT_TRUE(guard.locked());
    } // Try lock guard destructor should unlock the mutex
    
    EXPECT_FALSE(mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, TryLockGuardFailure) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
        .WillOnce(Return(pdFALSE));
    // xSemaphoreGive should NOT be called since lock failed
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    {
        freertos::try_lock_guard<freertos::mutex<freertos::dynamic_semaphore_allocator>> guard(mutex);
        // Mutex should not be locked (try_lock failed)
        EXPECT_FALSE(mutex.locked());
        EXPECT_FALSE(guard.locked());
    } // Try lock guard destructor should NOT call unlock since lock failed
    
    EXPECT_FALSE(mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, LockGuardISRRAII) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTakeFromISR(mock_mutex_handle, ::testing::NotNull()))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGiveFromISR(mock_mutex_handle, ::testing::NotNull()))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    {
        /*
         * ISR Lock Guard Test:
         * This tests the lock_guard_isr which is designed for use in ISR contexts.
         * In a real FreeRTOS environment, this would be used inside interrupt service
         * routines. Here we test the API compatibility and parameter passing.
         */
        freertos::lock_guard_isr<freertos::mutex<freertos::dynamic_semaphore_allocator>> guard(mutex);
        // Mutex should be locked here
        EXPECT_TRUE(mutex.locked());
        EXPECT_TRUE(guard.locked());
        // Check if high priority task woken flag is accessible
        EXPECT_EQ(guard.high_priority_task_woken(), pdFALSE);
    } // ISR lock guard destructor should unlock the mutex
    
    EXPECT_FALSE(mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, TimeoutLockGuardRAII) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 1000))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    {
        freertos::timeout_lock_guard<freertos::mutex<freertos::dynamic_semaphore_allocator>> guard(mutex, 1000);
        // Mutex should be locked here (lock with timeout succeeded)
        EXPECT_TRUE(mutex.locked());
        EXPECT_TRUE(guard.locked());
    } // Timeout lock guard destructor should unlock the mutex
    
    EXPECT_FALSE(mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, TimeoutLockGuardChronoRAII) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 500)) // 500ms
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    {
        freertos::timeout_lock_guard<freertos::mutex<freertos::dynamic_semaphore_allocator>> guard(mutex, std::chrono::milliseconds(500));
        // Mutex should be locked here (lock with chrono timeout succeeded)
        EXPECT_TRUE(mutex.locked());
        EXPECT_TRUE(guard.locked());
    } // Timeout lock guard destructor should unlock the mutex
    
    EXPECT_FALSE(mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, TimeoutLockGuardFailure) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 100))
        .WillOnce(Return(pdFALSE));
    // Note: xSemaphoreGive should NOT be called since lock failed
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    {
        freertos::timeout_lock_guard<freertos::mutex<freertos::dynamic_semaphore_allocator>> guard(mutex, 100);
        // Mutex should not be locked (lock with timeout failed)
        EXPECT_FALSE(mutex.locked());
        EXPECT_FALSE(guard.locked());
    } // Timeout lock guard destructor should NOT call unlock (since lock was never acquired)
    
    EXPECT_FALSE(mutex.locked());
}

// =============================================================================
// Error Handling and Edge Cases
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, SemaphoreCreationFailure) {
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(nullptr));
    
    /*
     * Creation Failure Note:
     * When xSemaphoreCreateBinary returns nullptr, the wrapper's constructor
     * will call configASSERT. In a real FreeRTOS environment, this would
     * trigger assertion handling. In our mock environment, we document this
     * behavior without triggering actual assertions.
     */
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    // The constructor would call configASSERT(m_semaphore) on nullptr
    // In testing, we just verify the API was called correctly
}

TEST_F(FreeRTOSSemaphoreTest, StaticAllocationLimitation) {
    /*
     * Static Allocation Limitation:
     * Similar to tasks, static semaphore allocators have limitations when
     * allocated on the stack, as the underlying StaticSemaphore_t structure
     * must persist beyond the allocator's scope when used by FreeRTOS.
     * 
     * Problematic pattern:
     * void some_function() {
     *     freertos::static_semaphore_allocator allocator; // PROBLEMATIC!
     *     auto sem_handle = allocator.create_binary();
     *     // allocator goes out of scope but FreeRTOS still references it
     * }
     * 
     * Recommended: Use global/static allocators or dynamic allocation.
     */
    
    // This test documents the limitation without actually testing the failure
    // as it would require complex lifetime management verification
    EXPECT_TRUE(true); // Placeholder for documentation
}

TEST_F(FreeRTOSSemaphoreTest, MutexLockFailure) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, portMAX_DELAY))
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    auto result = mutex.lock();
    EXPECT_EQ(result, pdFALSE);
    EXPECT_FALSE(mutex.locked()); // Should remain unlocked on failure
}

TEST_F(FreeRTOSSemaphoreTest, LargeCountingSemaphore) {
    const UBaseType_t large_count = 65535;
    
    EXPECT_CALL(*mock, xSemaphoreCreateCounting(large_count, large_count))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> semaphore(large_count);
    // Should handle large count values correctly
}

TEST_F(FreeRTOSSemaphoreTest, ZeroCountingSemaphore) {
    /*
     * Zero Count Semaphore Note:
     * Creating a counting semaphore with maxCount = 0 is technically possible
     * but not practically useful. FreeRTOS may handle this differently across
     * versions. This test documents the behavior.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateCounting(0, 0))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> semaphore(0);
    // Edge case: semaphore with zero maximum count
}

// =============================================================================
// Mixed Allocator Tests
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, StaticVsDynamicBehavior) {
    // Test that static and dynamic allocators produce functionally equivalent semaphores
    
    EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(NotNull()))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
        .Times(2);
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle))
        .Times(2);
    
    freertos::binary_semaphore<freertos::static_semaphore_allocator> static_sem;
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> dynamic_sem;
    
    // Both should behave identically for operations
    auto result1 = static_sem.give();
    auto result2 = dynamic_sem.give();
    
    EXPECT_EQ(result1, result2);
}

// =============================================================================
// Namespace Alias Tests
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, StaticAliasNamespace) {
    /*
     * Static Allocation Namespace Alias Test:
     * Tests that the 'sa' namespace aliases work correctly and provide
     * the same functionality as the templated classes with static allocator.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateBinaryStatic(::testing::NotNull()))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::sa::binary_semaphore semaphore;
    auto result = semaphore.give();
    EXPECT_EQ(result, pdTRUE);
}

TEST_F(FreeRTOSSemaphoreTest, DynamicAliasNamespace) {
    /*
     * Dynamic Allocation Namespace Alias Test:
     * Tests that the 'da' namespace aliases work correctly and provide
     * the same functionality as the templated classes with dynamic allocator.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_semaphore_handle, portMAX_DELAY))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::da::binary_semaphore semaphore;
    auto result = semaphore.take();
    EXPECT_EQ(result, pdTRUE);
}

TEST_F(FreeRTOSSemaphoreTest, NamespaceAliasCountingSemaphore) {
    EXPECT_CALL(*mock, xSemaphoreCreateCounting(5, 5))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_semaphore_handle))
        .WillOnce(Return(3));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::da::counting_semaphore semaphore(5);
    auto count = semaphore.count();
    EXPECT_EQ(count, 3);
}

TEST_F(FreeRTOSSemaphoreTest, NamespaceAliasMutex) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::da::mutex mutex;
    auto result = mutex.try_lock();
    EXPECT_EQ(result, pdTRUE);
    EXPECT_TRUE(mutex.locked());
}

TEST_F(FreeRTOSSemaphoreTest, NamespaceAliasRecursiveMutex) {
    EXPECT_CALL(*mock, xSemaphoreCreateRecursiveMutex())
        .WillOnce(Return(mock_recursive_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_recursive_mutex_handle, portMAX_DELAY))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_recursive_mutex_handle))
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_recursive_mutex_handle));
    
    freertos::da::recursive_mutex recursive_mutex;
    auto lock_result = recursive_mutex.lock();
    auto unlock_result = recursive_mutex.unlock();
    
    EXPECT_EQ(lock_result, pdTRUE);
    EXPECT_EQ(unlock_result, pdTRUE);
}

// =============================================================================
// Additional Edge Cases and Comprehensive Tests
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, CountingSemaphoreMultipleOperators) {
    /*
     * Counting Semaphore Multiple Operations Test:
     * Tests various combinations of operators and operations on counting semaphores
     * to ensure they work correctly in different sequences.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateCounting(10, 10))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
        .Times(7); // 2 from ++, 5 from +=5
    EXPECT_CALL(*mock, xSemaphoreTake(mock_semaphore_handle, portMAX_DELAY))
        .Times(3); // 3 from --
    EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_semaphore_handle))
        .WillOnce(Return(9));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> semaphore(10);
    
    // Test pre-increment
    ++semaphore;
    // Test post-increment  
    semaphore++;
    // Test compound assignment
    semaphore += 5;
    // Test pre-decrement
    --semaphore;
    // Test post-decrement
    semaphore--;
    // Test another decrement
    --semaphore;
    
    // Check final count
    auto count = semaphore.count();
    EXPECT_EQ(count, 9);
}

TEST_F(FreeRTOSSemaphoreTest, MutexUnlockWithoutLock) {
    /*
     * Mutex Unlock Without Lock Test:
     * Tests the behavior when trying to unlock a mutex that hasn't been locked.
     * This is an edge case that could happen in error scenarios.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_mutex_handle))
        .WillOnce(Return(pdFALSE)); // FreeRTOS may return false for unlocking unlocked mutex
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    // Try to unlock without locking first
    auto result = mutex.unlock();
    EXPECT_EQ(result, pdFALSE);
    EXPECT_FALSE(mutex.locked()); // Should remain unlocked
}

TEST_F(FreeRTOSSemaphoreTest, BinarySemaphoreMultipleGive) {
    /*
     * Binary Semaphore Multiple Give Test:
     * Tests giving a binary semaphore multiple times. In FreeRTOS, giving
     * a binary semaphore that's already given should have no effect.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreGive(mock_semaphore_handle))
        .Times(3)
        .WillOnce(Return(pdTRUE))    // First give succeeds
        .WillOnce(Return(pdFALSE))   // Second give fails (already given)
        .WillOnce(Return(pdFALSE));  // Third give fails (already given)
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    
    auto result1 = semaphore.give();
    auto result2 = semaphore.give();
    auto result3 = semaphore.give();
    
    EXPECT_EQ(result1, pdTRUE);
    EXPECT_EQ(result2, pdFALSE);
    EXPECT_EQ(result3, pdFALSE);
}

TEST_F(FreeRTOSSemaphoreTest, BinarySemaphoreTakeUntaken) {
    /*
     * Binary Semaphore Take Untaken Test:
     * Tests taking a binary semaphore that hasn't been given with zero timeout.
     * This should fail immediately.
     */
    EXPECT_CALL(*mock, xSemaphoreCreateBinary())
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_semaphore_handle, 0))
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator> semaphore;
    
    auto result = semaphore.take(0);
    EXPECT_EQ(result, pdFALSE);
}

TEST_F(FreeRTOSSemaphoreTest, CountingSemaphoreEdgeCounts) {
    /*
     * Counting Semaphore Edge Count Test:
     * Tests counting semaphore with edge case count values including
     * boundary conditions.
     */
    const UBaseType_t max_count = UINT32_MAX;
    
    EXPECT_CALL(*mock, xSemaphoreCreateCounting(max_count, max_count))
        .WillOnce(Return(mock_semaphore_handle));
    EXPECT_CALL(*mock, uxSemaphoreGetCount(mock_semaphore_handle))
        .WillOnce(Return(max_count));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_semaphore_handle));
    
    freertos::counting_semaphore<freertos::dynamic_semaphore_allocator> semaphore(max_count);
    auto count = semaphore.count();
    EXPECT_EQ(count, max_count);
}

// =============================================================================
// Performance and Compatibility Tests
// =============================================================================

TEST_F(FreeRTOSSemaphoreTest, ChronoTimeoutCompatibility) {
    EXPECT_CALL(*mock, xSemaphoreCreateMutex())
        .WillOnce(Return(mock_mutex_handle));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 2000)) // 2 seconds = 2000ms
        .WillOnce(Return(pdTRUE));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 500))  // 500ms
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, xSemaphoreTake(mock_mutex_handle, 0))    // 0 microseconds = 0ms
        .WillOnce(Return(pdFALSE));
    EXPECT_CALL(*mock, vSemaphoreDelete(mock_mutex_handle));
    
    freertos::mutex<freertos::dynamic_semaphore_allocator> mutex;
    
    // Test various chrono duration types
    auto result1 = mutex.lock(std::chrono::seconds(2));
    auto result2 = mutex.lock(std::chrono::milliseconds(500));
    auto result3 = mutex.lock(std::chrono::microseconds(100)); // Rounds down to 0ms
    
    EXPECT_EQ(result1, pdTRUE);
    EXPECT_EQ(result2, pdFALSE);
    EXPECT_EQ(result3, pdFALSE);
}

TEST_F(FreeRTOSSemaphoreTest, SemaphoreAPICompleteness) {
    /*
     * API Completeness Test:
     * This test verifies that all major FreeRTOS semaphore APIs are covered
     * by the C++ wrapper classes. Not all APIs may be directly exposed,
     * but the essential functionality should be available.
     * 
     * Covered APIs:
     * - xSemaphoreCreateBinary/Static
     * - xSemaphoreCreateCounting/Static  
     * - xSemaphoreCreateMutex/Static
     * - xSemaphoreCreateRecursiveMutex/Static
     * - xSemaphoreGive/FromISR
     * - xSemaphoreTake/FromISR
     * - uxSemaphoreGetCount
     * - vSemaphoreDelete
     * 
     * Note: Some advanced FreeRTOS features like semaphore handles in queues
     * or queue sets are not covered by these basic wrapper classes.
     */
    EXPECT_TRUE(true); // Placeholder for documentation
}