/**
 * @file test_enhanced_freertos_sw_timer.cpp
 * @brief Enhanced unit tests for FreeRTOS C++ software timer wrappers with realistic timer service simulation
 * 
 * This file provides enhanced tests that demonstrate realistic FreeRTOS timer behavior using
 * the enhanced timer service simulator. Unlike the basic timer tests, these tests actually
 * simulate timer service task behavior including:
 * 
 * - Real callback execution when timers expire
 * - Time-based timer behavior simulation
 * - Single-shot vs auto-reload timer verification
 * - Multi-timer coordination and interaction
 * - Timer command queue processing
 * - Realistic error conditions and edge cases
 * 
 * These tests provide comprehensive coverage of timer behavior that closely mirrors
 * what would happen in a real FreeRTOS environment with an actual timer service task.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <memory>
#include <functional>
#include <atomic>
#include <vector>

// Include mocks first to set up FreeRTOS environment
#include "FreeRTOS.h"
#include "enhanced_timer_mocks.hpp"

// Include the actual FreeRTOS software timer wrapper
#include "../include/freertos_sw_timer.hpp"

using namespace testing;
using namespace freertos;
using namespace freertos_mocks;
using namespace std::chrono_literals;

class EnhancedFreeRTOSSwTimerTest : public Test {
protected:
    void SetUp() override {
        // Enable enhanced simulation for these tests
        enhanced_mock = &EnhancedTimerMock::instance();
        enhanced_mock->enableSimulation(true);
        enhanced_mock->reset();
        
        // Enable callback tracking
        enhanced_mock->getSimulator().enableCallbackTracking(true);
        
        // Reset callback counters
        callback_count = 0;
        callback_values.clear();
    }
    
    void TearDown() override {
        // Disable enhanced simulation after tests
        enhanced_mock->enableSimulation(false);
        enhanced_mock->reset();
        enhanced_mock = nullptr;
    }
    
    // Helper function to create a test callback that increments counter
    std::function<void()> createCountingCallback() {
        return [this]() { 
            callback_count++; 
            callback_values.push_back(getCurrentTime());
        };
    }
    
    // Helper function to create a test callback with custom action
    std::function<void()> createCustomCallback(std::function<void()> action) {
        return [this, action]() { 
            callback_count++; 
            callback_values.push_back(getCurrentTime());
            if (action) action();
        };
    }
    
    // Helper function to advance time and process timers
    void advanceTime(TickType_t ticks) {
        enhanced_mock->getSimulator().advanceTime(ticks);
    }
    
    // Helper function to get current simulated time
    TickType_t getCurrentTime() const {
        return enhanced_mock->getSimulator().getCurrentTime();
    }
    
    // Helper function to get callback count for a timer
    size_t getCallbackCount(TimerHandle_t timer) const {
        return enhanced_mock->getSimulator().getCallbackCount(timer);
    }
    
    EnhancedTimerMock* enhanced_mock;
    std::atomic<int> callback_count{0};
    std::vector<TickType_t> callback_values;
};

// =============================================================================
// BASIC ENHANCED TIMER FUNCTIONALITY TESTS
// =============================================================================

TEST_F(EnhancedFreeRTOSSwTimerTest, TimerCreationWithEnhancedMocks) {
    auto callback = createCountingCallback();
    
    // Create timer using static allocator
    sa::timer test_timer("TestTimer", 1000, pdTRUE, std::move(callback));
    
    // Verify timer was created in the simulator  
    EXPECT_GT(enhanced_mock->getSimulator().getTotalTimerCount(), 0);
    
    // Timer should not be active initially
    EXPECT_FALSE(test_timer.running());
    
    // Start the timer
    EXPECT_EQ(test_timer.start(), pdPASS);
    EXPECT_TRUE(test_timer.running());
    
    // No callbacks should have been executed yet
    EXPECT_EQ(callback_count, 0);
}

TEST_F(EnhancedFreeRTOSSwTimerTest, SingleShotTimerRealBehavior) {
    auto callback = createCountingCallback();
    
    // Create single-shot timer (auto-reload = false)
    sa::timer test_timer("SingleShot", 500, pdFALSE, std::move(callback));
    
    // Start the timer
    EXPECT_EQ(test_timer.start(), pdPASS);
    EXPECT_TRUE(test_timer.running());
    
    // Advance time but not enough to expire
    advanceTime(400);
    EXPECT_EQ(callback_count, 0);
    EXPECT_TRUE(test_timer.running());
    
    // Advance time to expiry
    advanceTime(100);  // Total: 500 ticks
    
    // Callback should have been executed once
    EXPECT_EQ(callback_count, 1);
    // Timer should have stopped (single-shot behavior)
    EXPECT_FALSE(test_timer.running());
    
    // Advance more time - no additional callbacks
    advanceTime(1000);
    EXPECT_EQ(callback_count, 1);
    EXPECT_FALSE(test_timer.running());
}

TEST_F(EnhancedFreeRTOSSwTimerTest, AutoReloadTimerRealBehavior) {
    auto callback = createCountingCallback();
    
    // Create auto-reload timer
    sa::timer test_timer("AutoReload", 300, pdTRUE, std::move(callback));
    
    // Start the timer
    EXPECT_EQ(test_timer.start(), pdPASS);
    EXPECT_TRUE(test_timer.running());
    
    // Advance time to first expiry
    advanceTime(300);
    EXPECT_EQ(callback_count, 1);
    EXPECT_TRUE(test_timer.running());  // Should still be running
    
    // Advance to second expiry
    advanceTime(300);  // Total: 600 ticks
    EXPECT_EQ(callback_count, 2);
    EXPECT_TRUE(test_timer.running());
    
    // Advance to third expiry
    advanceTime(300);  // Total: 900 ticks
    EXPECT_EQ(callback_count, 3);
    EXPECT_TRUE(test_timer.running());
    
    // Stop the timer
    EXPECT_EQ(test_timer.stop(), pdPASS);
    EXPECT_FALSE(test_timer.running());
    
    // Advance more time - no additional callbacks
    advanceTime(1000);
    EXPECT_EQ(callback_count, 3);
}

TEST_F(EnhancedFreeRTOSSwTimerTest, TimerResetRealBehavior) {
    auto callback = createCountingCallback();
    
    sa::timer test_timer("ResetTest", 500, pdFALSE, std::move(callback));
    
    // Start the timer
    EXPECT_EQ(test_timer.start(), pdPASS);
    
    // Advance time partially
    advanceTime(300);
    EXPECT_EQ(callback_count, 0);
    
    // Reset the timer - should restart the period
    EXPECT_EQ(test_timer.reset(), pdPASS);
    
    // Advance the original remaining time (200 ticks) - should not expire
    advanceTime(200);
    EXPECT_EQ(callback_count, 0);
    
    // Advance to complete the reset period
    advanceTime(300);  // Total from reset: 500 ticks
    EXPECT_EQ(callback_count, 1);
}

// =============================================================================
// PERIOD CHANGE TESTS WITH REAL BEHAVIOR
// =============================================================================

TEST_F(EnhancedFreeRTOSSwTimerTest, PeriodChangeWithRealBehavior) {
    auto callback = createCountingCallback();
    
    sa::timer test_timer("PeriodChange", 1000, pdTRUE, std::move(callback));
    
    // Start the timer
    EXPECT_EQ(test_timer.start(), pdPASS);
    
    // Advance to first expiry
    advanceTime(1000);
    EXPECT_EQ(callback_count, 1);
    
    // Change period to 500 ticks
    EXPECT_EQ(test_timer.period(500), pdPASS);
    
    // Next expiry should be at new period
    advanceTime(500);
    EXPECT_EQ(callback_count, 2);
    
    // And continue with new period
    advanceTime(500);
    EXPECT_EQ(callback_count, 3);
}

TEST_F(EnhancedFreeRTOSSwTimerTest, PeriodChangeWithChronoDurations) {
    auto callback = createCountingCallback();
    
    // Create timer with chrono duration
    sa::timer test_timer("ChronoTimer", 1s, pdTRUE, std::move(callback));
    
    EXPECT_EQ(test_timer.start(), pdPASS);
    
    // First expiry at 1000ms
    advanceTime(1000);
    EXPECT_EQ(callback_count, 1);
    
    // Change period to 500ms using chrono
    EXPECT_EQ(test_timer.period(500ms, 100ms), pdPASS);
    
    // Next expiry at new period
    advanceTime(500);
    EXPECT_EQ(callback_count, 2);
}

// =============================================================================
// MULTI-TIMER COORDINATION TESTS
// =============================================================================

TEST_F(EnhancedFreeRTOSSwTimerTest, MultipleTimersCoordination) {
    std::atomic<int> timer1_count{0};
    std::atomic<int> timer2_count{0};
    std::atomic<int> timer3_count{0};
    
    auto callback1 = [&timer1_count]() { timer1_count++; };
    auto callback2 = [&timer2_count]() { timer2_count++; };
    auto callback3 = [&timer3_count]() { timer3_count++; };
    
    // Create multiple timers with different periods
    sa::timer timer1("Fast", 100, pdTRUE, std::move(callback1));
    sa::timer timer2("Medium", 300, pdTRUE, std::move(callback2));
    sa::timer timer3("Slow", 700, pdFALSE, std::move(callback3));  // Single-shot
    
    // Start all timers
    EXPECT_EQ(timer1.start(), pdPASS);
    EXPECT_EQ(timer2.start(), pdPASS);
    EXPECT_EQ(timer3.start(), pdPASS);
    
    // Advance time and check coordination
    advanceTime(100);  // Timer1 fires
    EXPECT_EQ(timer1_count, 1);
    EXPECT_EQ(timer2_count, 0);
    EXPECT_EQ(timer3_count, 0);
    
    advanceTime(200);  // Total: 300 - Timer1 fires 2 more times, Timer2 fires once
    EXPECT_EQ(timer1_count, 3);
    EXPECT_EQ(timer2_count, 1);
    EXPECT_EQ(timer3_count, 0);
    
    advanceTime(400);  // Total: 700 - Multiple fires, Timer3 fires once and stops
    EXPECT_EQ(timer1_count, 7);  // Every 100 ticks
    EXPECT_EQ(timer2_count, 2);  // Every 300 ticks
    EXPECT_EQ(timer3_count, 1);  // Single-shot at 700 ticks
    
    // Timer3 should be stopped, others still running
    EXPECT_TRUE(timer1.running());
    EXPECT_TRUE(timer2.running());
    EXPECT_FALSE(timer3.running());
    
    // Continue - Timer3 should not fire again
    advanceTime(1000);
    EXPECT_GT(timer1_count.load(), 7);
    EXPECT_GT(timer2_count.load(), 2);
    EXPECT_EQ(timer3_count, 1);  // Still 1 (single-shot)
}

TEST_F(EnhancedFreeRTOSSwTimerTest, TimerInteraction) {
    bool timer1_stopped_timer2 = false;
    std::atomic<int> timer2_count{0};
    
    // Timer2 callback just counts
    auto callback2 = [&timer2_count]() { timer2_count++; };
    
    // Create timer2 first (it will be captured by timer1's callback)
    sa::timer timer2("Timer2", 200, pdTRUE, std::move(callback2));
    
    // Timer1 callback stops timer2 after first execution
    auto callback1 = [&timer1_stopped_timer2, &timer2](){ 
        if (!timer1_stopped_timer2) {
            timer2.stop();
            timer1_stopped_timer2 = true;
        }
    };
    
    sa::timer timer1("Timer1", 500, pdFALSE, std::move(callback1));
    
    // Start both timers
    EXPECT_EQ(timer1.start(), pdPASS);
    EXPECT_EQ(timer2.start(), pdPASS);
    
    // Advance time - Timer2 should fire twice before Timer1 stops it
    advanceTime(400);
    EXPECT_EQ(timer2_count, 2);  // At 200 and 400 ticks
    EXPECT_FALSE(timer1_stopped_timer2);
    
    // Timer1 fires and stops Timer2
    advanceTime(100);  // Total: 500 ticks
    EXPECT_TRUE(timer1_stopped_timer2);
    EXPECT_FALSE(timer2.running());
    
    // Timer2 should not fire anymore
    advanceTime(1000);
    EXPECT_EQ(timer2_count, 2);  // Still 2
}

// =============================================================================
// ERROR CONDITIONS AND EDGE CASES
// =============================================================================

TEST_F(EnhancedFreeRTOSSwTimerTest, InvalidTimerOperations) {
    // Try to operate on non-existent timer handle
    TimerHandle_t fake_handle = reinterpret_cast<TimerHandle_t>(0xDEADBEEF);
    
    auto& sim = enhanced_mock->getSimulator();
    
    EXPECT_EQ(sim.startTimer(fake_handle, 100), pdFAIL);
    EXPECT_EQ(sim.stopTimer(fake_handle, 100), pdFAIL);
    EXPECT_EQ(sim.resetTimer(fake_handle, 100), pdFAIL);
    EXPECT_EQ(sim.changePeriod(fake_handle, 200, 100), pdFAIL);
    EXPECT_EQ(sim.deleteTimer(fake_handle, 100), pdFAIL);
    
    EXPECT_EQ(sim.isTimerActive(fake_handle), pdFALSE);
    EXPECT_EQ(sim.getTimerPeriod(fake_handle), 0);
    EXPECT_EQ(sim.getTimerExpiryTime(fake_handle), 0);
    EXPECT_EQ(sim.getReloadMode(fake_handle), pdFALSE);
    EXPECT_EQ(sim.getTimerName(fake_handle), nullptr);
    EXPECT_EQ(sim.getTimerID(fake_handle), nullptr);
}

TEST_F(EnhancedFreeRTOSSwTimerTest, ZeroPeriodTimer) {
    auto callback = createCountingCallback();
    
    // Try to create timer with zero period - should fail
    auto& sim = enhanced_mock->getSimulator();
    TimerHandle_t handle = sim.createTimer("ZeroPeriod", 0, pdTRUE, nullptr, 
                                          [](TimerHandle_t){});
    EXPECT_EQ(handle, nullptr);
}

TEST_F(EnhancedFreeRTOSSwTimerTest, NullCallbackTimer) {
    // Try to create timer with null callback - should fail
    auto& sim = enhanced_mock->getSimulator();
    TimerHandle_t handle = sim.createTimer("NullCallback", 1000, pdTRUE, nullptr, nullptr);
    EXPECT_EQ(handle, nullptr);
}

// =============================================================================
// TIMING ACCURACY TESTS
// =============================================================================

TEST_F(EnhancedFreeRTOSSwTimerTest, TimingAccuracyVerification) {
    auto callback = [this]() {
        callback_values.push_back(getCurrentTime());
        callback_count++;
    };
    
    sa::timer test_timer("AccuracyTest", 250, pdTRUE, std::move(callback));
    
    EXPECT_EQ(test_timer.start(), pdPASS);
    
    // Let timer run for multiple periods
    advanceTime(1000);
    
    // Should have fired at: 250, 500, 750, 1000
    EXPECT_EQ(callback_count, 4);
    EXPECT_EQ(callback_values.size(), 4);
    
    // Verify exact timing
    EXPECT_EQ(callback_values[0], 250);
    EXPECT_EQ(callback_values[1], 500);
    EXPECT_EQ(callback_values[2], 750);
    EXPECT_EQ(callback_values[3], 1000);
}

TEST_F(EnhancedFreeRTOSSwTimerTest, ExpiryTimeCalculation) {
    auto callback = createCountingCallback();
    
    sa::timer test_timer("ExpiryTest", 300, pdTRUE, std::move(callback));
    
    // Timer not started - expiry time should be 0
    EXPECT_EQ(test_timer.remaining_ticks(), 0);
    
    // Start timer at time 100
    advanceTime(100);
    EXPECT_EQ(test_timer.start(), pdPASS);
    
    // Expiry time should be current_time + period = 100 + 300 = 400
    EXPECT_EQ(xTimerGetExpiryTime(reinterpret_cast<TimerHandle_t>(&test_timer)), 400);
    
    // Advance time and check remaining time calculation
    advanceTime(150);  // Now at time 250
    TickType_t remaining = test_timer.remaining_ticks();
    EXPECT_EQ(remaining, 150);  // 400 - 250 = 150
}

// =============================================================================
// DYNAMIC ALLOCATION TESTS
// =============================================================================

#if configSUPPORT_DYNAMIC_ALLOCATION

TEST_F(EnhancedFreeRTOSSwTimerTest, DynamicTimerEnhancedBehavior) {
    auto callback = createCountingCallback();
    
    // Create dynamic timer
    da::timer test_timer("DynamicTimer", 400, pdTRUE, std::move(callback));
    
    EXPECT_EQ(test_timer.start(), pdPASS);
    
    // Test auto-reload behavior
    advanceTime(1200);  // Should fire 3 times: at 400, 800, 1200
    
    EXPECT_EQ(callback_count, 3);
    EXPECT_TRUE(test_timer.running());
}

#endif // configSUPPORT_DYNAMIC_ALLOCATION

// =============================================================================
// ISR FUNCTION TESTS WITH ENHANCED BEHAVIOR
// =============================================================================

TEST_F(EnhancedFreeRTOSSwTimerTest, ISRFunctionsEnhancedBehavior) {
    auto callback = createCountingCallback();
    
    sa::timer test_timer("ISRTest", 300, pdTRUE, std::move(callback));
    
    // Test ISR start
    BaseType_t higher_priority_woken = pdFALSE;
    EXPECT_EQ(test_timer.start_isr(higher_priority_woken), pdPASS);
    EXPECT_TRUE(test_timer.running());
    
    // Timer should behave normally after ISR start
    advanceTime(300);
    EXPECT_EQ(callback_count, 1);
    
    // Test ISR stop
    EXPECT_EQ(test_timer.stop_isr(higher_priority_woken), pdPASS);
    EXPECT_FALSE(test_timer.running());
    
    // Timer should not fire after being stopped
    advanceTime(600);
    EXPECT_EQ(callback_count, 1);  // Still 1
    
    // Test ISR reset
    EXPECT_EQ(test_timer.reset_isr(higher_priority_woken), pdPASS);
    EXPECT_TRUE(test_timer.running());
    
    // Timer should fire after reset
    advanceTime(300);
    EXPECT_EQ(callback_count, 2);
}

// =============================================================================
// COMMAND QUEUE SIMULATION TESTS
// =============================================================================

TEST_F(EnhancedFreeRTOSSwTimerTest, CommandQueueProcessing) {
    auto callback = createCountingCallback();
    
    sa::timer test_timer("QueueTest", 500, pdTRUE, std::move(callback));
    
    auto& sim = enhanced_mock->getSimulator();
    
    // Commands are queued but not processed until time advances
    EXPECT_EQ(test_timer.start(), pdPASS);
    EXPECT_EQ(sim.getPendingCommandCount(), 1);
    
    // Process commands by advancing time
    advanceTime(1);
    EXPECT_EQ(sim.getPendingCommandCount(), 0);
    EXPECT_TRUE(test_timer.running());
    
    // Queue multiple commands
    EXPECT_EQ(test_timer.stop(), pdPASS);
    EXPECT_EQ(test_timer.period(300), pdPASS);
    EXPECT_EQ(test_timer.start(), pdPASS);
    EXPECT_EQ(sim.getPendingCommandCount(), 3);
    
    // Process all commands
    advanceTime(1);
    EXPECT_EQ(sim.getPendingCommandCount(), 0);
    EXPECT_TRUE(test_timer.running());
    EXPECT_EQ(test_timer.period_ticks(), 300);  // Period should be changed
}

// =============================================================================
// COMPREHENSIVE INTEGRATION TEST
// =============================================================================

TEST_F(EnhancedFreeRTOSSwTimerTest, ComprehensiveIntegrationTest) {
    // This test demonstrates a realistic scenario with multiple timers
    // performing different tasks in coordination
    
    struct TimerStats {
        std::atomic<int> heartbeat_count{0};
        std::atomic<int> status_count{0};
        std::atomic<int> cleanup_count{0};
        bool cleanup_triggered{false};
    } stats;
    
    // Heartbeat timer - fast, continuous
    auto heartbeat_callback = [&stats]() { stats.heartbeat_count++; };
    sa::timer heartbeat_timer("Heartbeat", 50, pdTRUE, std::move(heartbeat_callback));
    
    // Status timer - medium speed, continuous
    auto status_callback = [&stats]() { stats.status_count++; };
    sa::timer status_timer("Status", 200, pdTRUE, std::move(status_callback));
    
    // Cleanup timer - slow, single-shot
    auto cleanup_callback = [&stats]() { 
        stats.cleanup_count++; 
        stats.cleanup_triggered = true;
    };
    sa::timer cleanup_timer("Cleanup", 1000, pdFALSE, std::move(cleanup_callback));
    
    // Start all timers
    EXPECT_EQ(heartbeat_timer.start(), pdPASS);
    EXPECT_EQ(status_timer.start(), pdPASS);
    EXPECT_EQ(cleanup_timer.start(), pdPASS);
    
    // Run system for a while
    advanceTime(500);
    
    // Check intermediate state
    EXPECT_EQ(stats.heartbeat_count, 10);  // Every 50 ticks: 50,100,150...500
    EXPECT_EQ(stats.status_count, 2);      // Every 200 ticks: 200,400
    EXPECT_EQ(stats.cleanup_count, 0);     // Not yet fired
    EXPECT_FALSE(stats.cleanup_triggered);
    
    // All timers except cleanup should still be running
    EXPECT_TRUE(heartbeat_timer.running());
    EXPECT_TRUE(status_timer.running());
    EXPECT_TRUE(cleanup_timer.running());
    
    // Continue to cleanup trigger
    advanceTime(500);  // Total: 1000
    
    // Final state
    EXPECT_EQ(stats.heartbeat_count, 20);  // 20 * 50 = 1000
    EXPECT_EQ(stats.status_count, 5);      // 5 * 200 = 1000
    EXPECT_EQ(stats.cleanup_count, 1);     // Single-shot fired once
    EXPECT_TRUE(stats.cleanup_triggered);
    
    // Cleanup timer should have stopped, others continue
    EXPECT_TRUE(heartbeat_timer.running());
    EXPECT_TRUE(status_timer.running());
    EXPECT_FALSE(cleanup_timer.running());  // Single-shot stopped
    
    // Continue running - cleanup should not fire again
    advanceTime(1000);  // Total: 2000
    EXPECT_GT(stats.heartbeat_count.load(), 20);
    EXPECT_GT(stats.status_count.load(), 5);
    EXPECT_EQ(stats.cleanup_count, 1);  // Still 1
}

// =============================================================================
// PERFORMANCE AND STRESS TESTS
// =============================================================================

TEST_F(EnhancedFreeRTOSSwTimerTest, ManyTimersStressTest) {
    const int NUM_TIMERS = 20;
    std::vector<std::unique_ptr<sa::timer>> timers;
    std::vector<std::atomic<int>> counters(NUM_TIMERS);
    
    // Create many timers with different periods
    for (int i = 0; i < NUM_TIMERS; ++i) {
        auto callback = [&counters, i]() { counters[i]++; };
        TickType_t period = 100 + (i * 50);  // 100, 150, 200, 250, ...
        
        timers.push_back(std::make_unique<sa::timer>(
            ("Timer" + std::to_string(i)).c_str(), 
            period, 
            pdTRUE, 
            std::move(callback)
        ));
        
        EXPECT_EQ(timers[i]->start(), pdPASS);
    }
    
    // Let all timers run
    advanceTime(2000);
    
    // Verify all timers fired appropriately
    int total_callbacks = 0;
    for (int i = 0; i < NUM_TIMERS; ++i) {
        int expected_count = 2000 / (100 + (i * 50));
        EXPECT_EQ(counters[i].load(), expected_count);
        total_callbacks += counters[i].load();
        EXPECT_TRUE(timers[i]->running());
    }
    
    // Should have had many callback executions
    EXPECT_GT(total_callbacks, 50);
}