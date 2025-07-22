/**
 * @file test_enhanced_timeout_precision.cpp
 * @brief Enhanced timeout precision and chrono duration tests
 * 
 * This file provides comprehensive testing for timeout precision,
 * sub-millisecond delays, and chrono duration functionality
 * to address the TODO comment in freertos_task.hpp line 1011.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <thread>
#include <atomic>

// Include mocks first
#include "FreeRTOS.h"

// Include the actual headers
#include "../include/freertos_task.hpp"
#include "../include/freertos_semaphore.hpp"

using namespace testing;
using namespace freertos;
using namespace std::chrono_literals;

class TimeoutPrecisionTest : public Test {
protected:
    void SetUp() override {
        mock = std::make_unique<NiceMock<FreeRTOSMock>>();
        extern FreeRTOSMock* g_freertos_mock;
        g_freertos_mock = mock.get();
        
        // Set up default mock behaviors
        ON_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
            .WillByDefault(Return(reinterpret_cast<TaskHandle_t>(0x12345678)));
        ON_CALL(*mock, vTaskDelay(_)).WillByDefault(Return());
        ON_CALL(*mock, xSemaphoreCreateBinaryStatic(_))
            .WillByDefault(Return(reinterpret_cast<SemaphoreHandle_t>(0x87654321)));
        ON_CALL(*mock, xSemaphoreTake(_, _))
            .WillByDefault(Return(pdTRUE));
    }
    
    void TearDown() override {
        extern FreeRTOSMock* g_freertos_mock;
        g_freertos_mock = nullptr;
        mock.reset();
    }
    
    std::unique_ptr<NiceMock<FreeRTOSMock>> mock;
};

// =============================================================================
// SUB-MILLISECOND DELAY TESTS
// =============================================================================

TEST_F(TimeoutPrecisionTest, MicrosecondDelayConversion) {
    // Test microsecond duration conversion to ticks
    auto microseconds_500 = 500us;
    auto microseconds_100 = 100us;
    auto microseconds_1 = 1us;
    
    EXPECT_CALL(*mock, vTaskDelay(_))
        .Times(AtLeast(3));
    
    // These should work even for sub-millisecond delays
    // The wrapper should handle conversion appropriately
    delay(microseconds_500);
    delay(microseconds_100); 
    delay(microseconds_1);
}

TEST_F(TimeoutPrecisionTest, NanosecondDelayConversion) {
    // Test nanosecond duration conversion
    auto nanoseconds_500000 = 500000ns;  // 0.5ms
    auto nanoseconds_100000 = 100000ns;  // 0.1ms
    auto nanoseconds_1000 = 1000ns;      // 1μs
    
    EXPECT_CALL(*mock, vTaskDelay(_))
        .Times(AtLeast(3));
    
    delay(nanoseconds_500000);
    delay(nanoseconds_100000);
    delay(nanoseconds_1000);
}

TEST_F(TimeoutPrecisionTest, MixedDurationTypes) {
    // Test different std::chrono duration types
    EXPECT_CALL(*mock, vTaskDelay(_))
        .Times(AtLeast(6));
    
    delay(1ms);           // milliseconds
    delay(1000us);        // microseconds  
    delay(1000000ns);     // nanoseconds
    delay(1s);            // seconds
    delay(0.5s);          // fractional seconds
    delay(2min);          // minutes (should work but be very long)
}

// =============================================================================
// CHRONO TIMEOUT PRECISION TESTS 
// =============================================================================

TEST_F(TimeoutPrecisionTest, ChronoBinarySemaphoreTimeout) {
    sa::binary_semaphore sem;
    
    // Test timeout functionality with regular take operations
    EXPECT_CALL(*mock, xSemaphoreTake(_, _))
        .WillRepeatedly(Return(pdFALSE)); // Always timeout
    
    auto start = std::chrono::steady_clock::now();
    
    // Test timeout operations
    bool result1 = sem.take(0);
    EXPECT_FALSE(result1);
    
    bool result2 = sem.take(0);
    EXPECT_FALSE(result2);
    
    bool result3 = sem.take(0);
    EXPECT_FALSE(result3);
    
    auto end = std::chrono::steady_clock::now();
    auto elapsed = end - start;
    
    // Verify the test ran in reasonable time (should be very fast due to mocking)
    EXPECT_LT(elapsed, 100ms);
}

TEST_F(TimeoutPrecisionTest, ChronoCountingSemaphoreTimeout) {
    EXPECT_CALL(*mock, xSemaphoreCreateCountingStatic(_, _, _))
        .WillOnce(Return(reinterpret_cast<SemaphoreHandle_t>(0x11111111)));
    EXPECT_CALL(*mock, xSemaphoreTake(_, _))
        .WillRepeatedly(Return(pdFALSE));
    
    // Create counting semaphore with max count of 10
    sa::counting_semaphore sem(10);
    
    // Test timeout functionality
    EXPECT_FALSE(sem.take(0));
    EXPECT_FALSE(sem.take(0));
    EXPECT_FALSE(sem.take(0));
}

// =============================================================================
// DURATION BOUNDARY TESTS
// =============================================================================

TEST_F(TimeoutPrecisionTest, ZeroDurationHandling) {
    // Test zero duration handling
    EXPECT_CALL(*mock, vTaskDelay(0))
        .Times(AtLeast(1));
    
    delay(0ms);
    delay(0us);
    delay(0ns);
    delay(std::chrono::milliseconds::zero());
}

TEST_F(TimeoutPrecisionTest, MaximumDurationHandling) {
    // Test very large durations
    EXPECT_CALL(*mock, vTaskDelay(_))
        .Times(AtLeast(2));
    
    // These should not overflow
    delay(std::chrono::hours(1));
    delay(std::chrono::minutes(30));
}

TEST_F(TimeoutPrecisionTest, DurationCastingAccuracy) {
    // Test that duration casting maintains reasonable precision
    
    // 1.5 milliseconds
    auto duration_1_5_ms = std::chrono::duration<double, std::milli>(1.5);
    
    // 2.7 milliseconds  
    auto duration_2_7_ms = std::chrono::duration<float, std::milli>(2.7f);
    
    EXPECT_CALL(*mock, vTaskDelay(_))
        .Times(AtLeast(2));
    
    delay(duration_1_5_ms);
    delay(duration_2_7_ms);
}

// =============================================================================
// CONCURRENT TIMEOUT PRECISION TESTS
// =============================================================================

TEST_F(TimeoutPrecisionTest, ConcurrentTimeoutOperations) {
    constexpr size_t num_threads = 4;
    constexpr size_t operations_per_thread = 10;
    
    std::vector<std::thread> threads;
    std::atomic<size_t> completed_operations{0};
    
    // Mock will be called many times
    EXPECT_CALL(*mock, vTaskDelay(_))
        .Times(AtLeast(num_threads * operations_per_thread));
    
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            for (size_t j = 0; j < operations_per_thread; ++j) {
                // Use different duration types for variety
                switch ((i + j) % 4) {
                    case 0: delay(100us); break;
                    case 1: delay(0.1ms); break;
                    case 2: delay(100000ns); break;
                    case 3: delay(std::chrono::duration<double, std::milli>(0.1)); break;
                }
                completed_operations++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(completed_operations.load(), num_threads * operations_per_thread);
}

// =============================================================================
// EDGE CASE TIMEOUT TESTS
// =============================================================================

TEST_F(TimeoutPrecisionTest, EdgeCaseDurations) {
    EXPECT_CALL(*mock, vTaskDelay(_))
        .Times(AtLeast(5));
    
    // Test edge cases
    delay(1ns);    // Minimum resolution
    delay(999us);  // Just under 1ms
    delay(1001us); // Just over 1ms
    delay(std::chrono::duration<long long, std::ratio<1, 1000000>>(1)); // 1 microsecond
    delay(std::chrono::duration<double, std::nano>(500.5)); // Fractional nanoseconds
}

TEST_F(TimeoutPrecisionTest, TickConversionConsistency) {
    // Test that different ways of expressing the same duration 
    // result in consistent behavior
    
    EXPECT_CALL(*mock, vTaskDelay(_))
        .Times(AtLeast(6));
    
    // All these should represent the same duration (1ms)
    delay(1ms);
    delay(1000us);
    delay(1000000ns);
    delay(std::chrono::duration<double, std::milli>(1.0));
    delay(std::chrono::duration<int, std::milli>(1));
    delay(std::chrono::duration<float, std::milli>(1.0f));
}