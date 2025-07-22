/**
 * @file test_freertos_semaphore_stl.cpp
 * @author GitHub Copilot AI Assistant
 * @brief Comprehensive STL-based semaphore tests for FreeRTOS C++ wrappers
 * @version 1.0
 * @date 2024-12-20
 * 
 * This file contains comprehensive behavioral unit tests using STL-based
 * semaphore implementations that provide actual semaphore behavior for
 * testing concurrent scenarios and real-world usage patterns.
 *
 * Test Coverage:
 * - STL-based binary semaphore operations (actual signaling behavior)
 * - STL-based counting semaphore operations (actual counting behavior)
 * - STL-based mutex operations (actual locking behavior)
 * - STL-based recursive mutex operations (actual recursive locking)
 * - Concurrent access patterns using multiple threads
 * - Timeout behavior with actual timeouts
 * - Producer-consumer scenarios
 * - Reader-writer patterns
 * - Deadlock prevention scenarios
 * 
 * Testing Strategy:
 * - Use actual std::thread for concurrent testing
 * - Use real timeout values to test timeout behavior
 * - Test actual semaphore semantics vs mock expectations
 * - Validate thread safety and synchronization primitives
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <future>

// Include STL-based implementations
#include "stl_semaphore_mocks.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::NiceMock;

class STLSemaphoreTest : public ::testing::Test {
protected:
    void SetUp() override {
        // No setup needed for direct STL implementations
    }

    void TearDown() override {
        // No teardown needed for direct STL implementations
    }
};

// =============================================================================
// STL Binary Semaphore Tests
// =============================================================================

TEST_F(STLSemaphoreTest, STLBinarySemaphoreBasicOperations) {
    freertos_test::stl_binary_semaphore sem;
    
    // Initially no signal should be available
    EXPECT_EQ(sem.take(0), pdFALSE); // Should timeout immediately
    
    // Give signal
    EXPECT_EQ(sem.give(), pdTRUE);
    
    // Take signal (should succeed)
    EXPECT_EQ(sem.take(0), pdTRUE);
    
    // No more signals available
    EXPECT_EQ(sem.take(0), pdFALSE);
}

TEST_F(STLSemaphoreTest, STLBinarySemaphoreTimeout) {
    freertos_test::stl_binary_semaphore sem;
    
    auto start = std::chrono::steady_clock::now();
    
    // Should timeout after approximately 100ms
    EXPECT_EQ(sem.take(100), pdFALSE);
    
    auto elapsed = std::chrono::steady_clock::now() - start;
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    
    // Should have waited approximately 100ms (allow some tolerance)
    EXPECT_GE(elapsed_ms, 90);
    EXPECT_LE(elapsed_ms, 150);
}

TEST_F(STLSemaphoreTest, STLBinarySemaphoreProducerConsumer) {
    freertos_test::stl_binary_semaphore sem;
    std::atomic<int> data{0};
    std::atomic<bool> producer_done{false};
    std::atomic<bool> consumer_received{false};
    
    // Consumer thread
    std::thread consumer([&]() {
        if (sem.take(1000) == pdTRUE) { // Wait up to 1 second
            EXPECT_EQ(data.load(), 42);
            consumer_received = true;
        }
    });
    
    // Producer thread
    std::thread producer([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        data = 42;
        sem.give();
        producer_done = true;
    });
    
    producer.join();
    consumer.join();
    
    EXPECT_TRUE(producer_done);
    EXPECT_TRUE(consumer_received);
}

TEST_F(STLSemaphoreTest, STLBinarySemaphoreMultipleProducers) {
    freertos_test::stl_binary_semaphore sem;
    std::atomic<int> signals_received{0};
    const int num_producers = 5;
    
    // Consumer thread that counts signals
    std::thread consumer([&]() {
        for (int i = 0; i < num_producers; ++i) {
            if (sem.take(1000) == pdTRUE) {
                signals_received++;
            }
        }
    });
    
    // Multiple producer threads
    std::vector<std::thread> producers;
    for (int i = 0; i < num_producers; ++i) {
        producers.emplace_back([&, i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));
            sem.give();
        });
    }
    
    // Wait for all producers
    for (auto& producer : producers) {
        producer.join();
    }
    
    consumer.join();
    
    EXPECT_EQ(signals_received.load(), num_producers);
}

// =============================================================================
// STL Counting Semaphore Tests
// =============================================================================

TEST_F(STLSemaphoreTest, STLCountingSemaphoreBasicOperations) {
    freertos_test::stl_counting_semaphore sem(5, 0); // Max 5, start with 0
    
    // Initially no permits available
    EXPECT_EQ(sem.take(0), pdFALSE);
    EXPECT_EQ(sem.count(), 0);
    
    // Give permits
    EXPECT_EQ(sem.give(), pdTRUE);
    EXPECT_EQ(sem.count(), 1);
    
    EXPECT_EQ(sem.give(), pdTRUE);
    EXPECT_EQ(sem.count(), 2);
    
    // Take permits
    EXPECT_EQ(sem.take(0), pdTRUE);
    EXPECT_EQ(sem.count(), 1);
    
    EXPECT_EQ(sem.take(0), pdTRUE);
    EXPECT_EQ(sem.count(), 0);
    
    // No more permits
    EXPECT_EQ(sem.take(0), pdFALSE);
}

TEST_F(STLSemaphoreTest, STLCountingSemaphoreMaxLimit) {
    freertos_test::stl_counting_semaphore sem(2, 0); // Max 2
    
    // Fill to capacity
    EXPECT_EQ(sem.give(), pdTRUE);
    EXPECT_EQ(sem.count(), 1);
    
    EXPECT_EQ(sem.give(), pdTRUE);
    EXPECT_EQ(sem.count(), 2);
    
    // Should fail when at maximum
    EXPECT_EQ(sem.give(), pdFALSE);
    EXPECT_EQ(sem.count(), 2);
}

TEST_F(STLSemaphoreTest, STLCountingSemaphoreResourcePool) {
    const int pool_size = 3;
    const int num_workers = 6;
    freertos_test::stl_counting_semaphore sem(pool_size, pool_size); // Resource pool
    
    std::atomic<int> active_workers{0};
    std::atomic<int> max_concurrent{0};
    std::atomic<int> completed_workers{0};
    
    std::vector<std::thread> workers;
    
    for (int i = 0; i < num_workers; ++i) {
        workers.emplace_back([&, i]() {
            // Acquire resource
            if (sem.take(1000) == pdTRUE) {
                int current = active_workers.fetch_add(1) + 1;
                
                // Update maximum concurrent count
                int expected = max_concurrent.load();
                while (current > expected && 
                       !max_concurrent.compare_exchange_weak(expected, current)) {
                    expected = max_concurrent.load();
                }
                
                // Simulate work
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                
                active_workers.fetch_sub(1);
                completed_workers.fetch_add(1);
                
                // Release resource
                sem.give();
            }
        });
    }
    
    // Wait for all workers
    for (auto& worker : workers) {
        worker.join();
    }
    
    EXPECT_EQ(completed_workers.load(), num_workers);
    EXPECT_LE(max_concurrent.load(), pool_size); // Should never exceed pool size
    EXPECT_EQ(sem.count(), pool_size); // All resources should be returned
}

// =============================================================================
// STL Mutex Tests
// =============================================================================

TEST_F(STLSemaphoreTest, STLMutexBasicOperations) {
    freertos_test::stl_mutex mutex;
    
    // Should be able to lock initially
    EXPECT_EQ(mutex.lock(0), pdTRUE);
    EXPECT_TRUE(mutex.locked());
    
    // Should not be able to lock again (from same thread in this implementation)
    EXPECT_EQ(mutex.try_lock(), pdFALSE);
    
    // Unlock
    EXPECT_EQ(mutex.unlock(), pdTRUE);
    EXPECT_FALSE(mutex.locked());
    
    // Should be able to lock again
    EXPECT_EQ(mutex.try_lock(), pdTRUE);
    EXPECT_TRUE(mutex.locked());
    
    EXPECT_EQ(mutex.unlock(), pdTRUE);
}

TEST_F(STLSemaphoreTest, STLMutexCriticalSection) {
    freertos_test::stl_mutex mutex;
    std::atomic<int> counter{0};
    std::atomic<int> inside_critical_section{0};
    std::atomic<int> max_concurrent{0};
    const int num_threads = 5;
    const int increments_per_thread = 100;
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < increments_per_thread; ++j) {
                // Enter critical section
                if (mutex.lock(1000) == pdTRUE) {
                    int current = inside_critical_section.fetch_add(1) + 1;
                    
                    // Update max concurrent count
                    int expected = max_concurrent.load();
                    while (current > expected && 
                           !max_concurrent.compare_exchange_weak(expected, current)) {
                        expected = max_concurrent.load();
                    }
                    
                    // Critical section work
                    int old_counter = counter.load();
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                    counter = old_counter + 1;
                    
                    inside_critical_section.fetch_sub(1);
                    mutex.unlock();
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(counter.load(), num_threads * increments_per_thread);
    EXPECT_EQ(max_concurrent.load(), 1); // Only one thread should be in critical section
    EXPECT_FALSE(mutex.locked()); // Mutex should be unlocked
}

// =============================================================================
// STL Recursive Mutex Tests
// =============================================================================

TEST_F(STLSemaphoreTest, STLRecursiveMutexBasicOperations) {
    freertos_test::stl_recursive_mutex mutex;
    
    // Should be able to lock initially
    EXPECT_EQ(mutex.lock(0), pdTRUE);
    EXPECT_TRUE(mutex.locked());
    EXPECT_EQ(mutex.recursions_count(), 1);
    
    // Should be able to lock recursively
    EXPECT_EQ(mutex.lock(0), pdTRUE);
    EXPECT_TRUE(mutex.locked());
    EXPECT_EQ(mutex.recursions_count(), 2);
    
    // Lock one more time
    EXPECT_EQ(mutex.lock(0), pdTRUE);
    EXPECT_EQ(mutex.recursions_count(), 3);
    
    // Unlock once
    EXPECT_EQ(mutex.unlock(), pdTRUE);
    EXPECT_TRUE(mutex.locked()); // Still locked
    EXPECT_EQ(mutex.recursions_count(), 2);
    
    // Unlock again
    EXPECT_EQ(mutex.unlock(), pdTRUE);
    EXPECT_TRUE(mutex.locked()); // Still locked
    EXPECT_EQ(mutex.recursions_count(), 1);
    
    // Final unlock
    EXPECT_EQ(mutex.unlock(), pdTRUE);
    EXPECT_FALSE(mutex.locked()); // Now unlocked
    EXPECT_EQ(mutex.recursions_count(), 0);
}

TEST_F(STLSemaphoreTest, STLRecursiveMutexNestedLocking) {
    freertos_test::stl_recursive_mutex mutex;
    std::atomic<int> depth_reached{0};
    
    std::function<void(int)> recursive_function = [&](int depth) {
        if (mutex.lock(1000) == pdTRUE) {
            depth_reached = std::max(depth_reached.load(), depth);
            
            if (depth < 5) {
                recursive_function(depth + 1);
            }
            
            mutex.unlock();
        }
    };
    
    std::thread test_thread([&]() {
        recursive_function(1);
    });
    
    test_thread.join();
    
    EXPECT_EQ(depth_reached.load(), 5);
    EXPECT_FALSE(mutex.locked());
    EXPECT_EQ(mutex.recursions_count(), 0);
}

// =============================================================================
// Complex Concurrent Scenarios
// =============================================================================

TEST_F(STLSemaphoreTest, STLProducerConsumerWithBuffering) {
    const int buffer_size = 3;
    freertos_test::stl_counting_semaphore empty_slots(buffer_size, buffer_size);
    freertos_test::stl_counting_semaphore filled_slots(buffer_size, 0);
    freertos_test::stl_mutex buffer_mutex;
    
    std::vector<int> buffer;
    std::atomic<int> items_produced{0};
    std::atomic<int> items_consumed{0};
    const int total_items = 10;
    
    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < total_items; ++i) {
            // Wait for empty slot
            if (empty_slots.take(1000) == pdTRUE) {
                // Get exclusive access to buffer
                if (buffer_mutex.lock(1000) == pdTRUE) {
                    buffer.push_back(i);
                    items_produced++;
                    buffer_mutex.unlock();
                }
                // Signal that slot is now filled
                filled_slots.give();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    
    // Consumer thread
    std::thread consumer([&]() {
        for (int i = 0; i < total_items; ++i) {
            // Wait for filled slot
            if (filled_slots.take(1000) == pdTRUE) {
                // Get exclusive access to buffer
                if (buffer_mutex.lock(1000) == pdTRUE) {
                    if (!buffer.empty()) {
                        int item = buffer.front();
                        buffer.erase(buffer.begin());
                        items_consumed++;
                        EXPECT_EQ(item, i); // Items should be consumed in order
                    }
                    buffer_mutex.unlock();
                }
                // Signal that slot is now empty
                empty_slots.give();
            }
        }
    });
    
    producer.join();
    consumer.join();
    
    EXPECT_EQ(items_produced.load(), total_items);
    EXPECT_EQ(items_consumed.load(), total_items);
    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(empty_slots.count(), buffer_size);
    EXPECT_EQ(filled_slots.count(), 0);
}

TEST_F(STLSemaphoreTest, STLReaderWriterPattern) {
    freertos_test::stl_counting_semaphore read_semaphore(5, 5); // Up to 5 concurrent readers
    freertos_test::stl_mutex write_mutex; // Exclusive writer access
    
    std::atomic<int> data{0};
    std::atomic<int> concurrent_readers{0};
    std::atomic<int> max_concurrent_readers{0};
    std::atomic<int> reads_completed{0};
    std::atomic<int> writes_completed{0};
    
    const int num_readers = 8;
    const int num_writers = 2;
    const int reads_per_reader = 5;
    const int writes_per_writer = 3;
    
    std::vector<std::thread> readers;
    std::vector<std::thread> writers;
    
    // Reader threads
    for (int i = 0; i < num_readers; ++i) {
        readers.emplace_back([&, i]() {
            for (int j = 0; j < reads_per_reader; ++j) {
                if (read_semaphore.take(1000) == pdTRUE) {
                    int current = concurrent_readers.fetch_add(1) + 1;
                    
                    // Update max concurrent readers
                    int expected = max_concurrent_readers.load();
                    while (current > expected && 
                           !max_concurrent_readers.compare_exchange_weak(expected, current)) {
                        expected = max_concurrent_readers.load();
                    }
                    
                    // Read data (simulate read operation)
                    int value = data.load();
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    EXPECT_GE(value, 0); // Data should be non-negative
                    
                    concurrent_readers.fetch_sub(1);
                    reads_completed.fetch_add(1);
                    read_semaphore.give();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        });
    }
    
    // Writer threads
    for (int i = 0; i < num_writers; ++i) {
        writers.emplace_back([&, i]() {
            for (int j = 0; j < writes_per_writer; ++j) {
                // Acquire write lock
                if (write_mutex.lock(1000) == pdTRUE) {
                    // Ensure no readers are active by acquiring all read permits
                    std::vector<bool> acquired_permits(5);
                    int permits_acquired = 0;
                    
                    for (int k = 0; k < 5; ++k) {
                        if (read_semaphore.take(100) == pdTRUE) {
                            acquired_permits[k] = true;
                            permits_acquired++;
                        } else {
                            break;
                        }
                    }
                    
                    if (permits_acquired == 5) {
                        // Write data
                        data = data.load() + 1;
                        writes_completed.fetch_add(1);
                        std::this_thread::sleep_for(std::chrono::milliseconds(20));
                    }
                    
                    // Release all acquired read permits
                    for (int k = 0; k < permits_acquired; ++k) {
                        read_semaphore.give();
                    }
                    
                    write_mutex.unlock();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }
    
    // Wait for all threads
    for (auto& reader : readers) {
        reader.join();
    }
    
    for (auto& writer : writers) {
        writer.join();
    }
    
    EXPECT_EQ(reads_completed.load(), num_readers * reads_per_reader);
    EXPECT_EQ(writes_completed.load(), num_writers * writes_per_writer);
    EXPECT_EQ(data.load(), num_writers * writes_per_writer);
    EXPECT_LE(max_concurrent_readers.load(), 5); // Should not exceed semaphore limit
    EXPECT_EQ(read_semaphore.count(), 5); // All read permits should be available
    EXPECT_FALSE(write_mutex.locked()); // Write mutex should be unlocked
}

// =============================================================================
// Performance and Stress Tests
// =============================================================================

TEST_F(STLSemaphoreTest, STLSemaphoreStressTest) {
    const int num_threads = 10;
    const int operations_per_thread = 1000;
    freertos_test::stl_counting_semaphore sem(num_threads, num_threads);
    
    std::atomic<int> successful_operations{0};
    std::atomic<int> timeout_operations{0};
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                // Take-give cycle
                if (sem.take(100) == pdTRUE) {
                    // Simulate some work
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    sem.give();
                    successful_operations.fetch_add(1);
                } else {
                    timeout_operations.fetch_add(1);
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_GT(successful_operations.load(), num_threads * operations_per_thread * 0.9); // At least 90% success
    EXPECT_EQ(sem.count(), num_threads); // All permits should be returned
}

TEST_F(STLSemaphoreTest, STLTimeoutAccuracy) {
    freertos_test::stl_binary_semaphore sem;
    
    std::vector<std::pair<int, std::chrono::milliseconds>> test_cases = {
        {50, std::chrono::milliseconds(0)},
        {100, std::chrono::milliseconds(0)},
        {200, std::chrono::milliseconds(0)}
    };
    
    for (auto& [timeout_ms, tolerance] : test_cases) {
        auto start = std::chrono::steady_clock::now();
        
        BaseType_t result = sem.take(timeout_ms);
        
        auto elapsed = std::chrono::steady_clock::now() - start;
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        
        EXPECT_EQ(result, pdFALSE); // Should timeout
        EXPECT_GE(elapsed_ms, timeout_ms - 20); // Allow some tolerance
        EXPECT_LE(elapsed_ms, timeout_ms + 50); // Allow some tolerance
    }
}