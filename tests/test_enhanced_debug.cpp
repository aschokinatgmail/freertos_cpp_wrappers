/**
 * @file test_enhanced_debug.cpp
 * @brief Simple debug test for enhanced timer mocks
 */

#include <gtest/gtest.h>
#include <iostream>

// Include mocks first
#include "FreeRTOS.h"
#include "enhanced_timer_mocks.hpp"

// Include the actual timer wrapper
#include "../include/freertos_sw_timer.hpp"

using namespace testing;
using namespace freertos_mocks;
using namespace freertos;

TEST(EnhancedDebugTest, BasicSimulatorTest) {
    std::cout << "Starting basic simulator test..." << std::endl;
    
    // Get enhanced mock instance
    auto& enhanced_mock = EnhancedTimerMock::instance();
    enhanced_mock.enableSimulation(true);
    enhanced_mock.reset();
    
    std::cout << "Enhanced simulation enabled and reset" << std::endl;
    
    // Try to create a timer directly via simulator
    auto& sim = enhanced_mock.getSimulator();
    auto callback = [](TimerHandle_t timer) {
        std::cout << "Callback executed!" << std::endl;
    };
    
    std::cout << "Creating timer..." << std::endl;
    TimerHandle_t handle = sim.createTimer("TestTimer", 1000, pdTRUE, nullptr, callback);
    
    std::cout << "Timer handle: " << handle << std::endl;
    EXPECT_NE(handle, nullptr);
    
    std::cout << "Total timer count: " << sim.getTotalTimerCount() << std::endl;
    EXPECT_EQ(sim.getTotalTimerCount(), 1);
    
    std::cout << "Active timer count: " << sim.getActiveTimerCount() << std::endl;
    EXPECT_EQ(sim.getActiveTimerCount(), 0);  // Not started yet
    
    std::cout << "Starting timer..." << std::endl;
    EXPECT_EQ(sim.startTimer(handle, 100), pdPASS);
    
    std::cout << "Active timer count after start: " << sim.getActiveTimerCount() << std::endl;
    EXPECT_EQ(sim.getActiveTimerCount(), 1);
    
    std::cout << "Advancing time..." << std::endl;
    sim.advanceTime(1000);
    
    std::cout << "Test completed successfully!" << std::endl;
    
    enhanced_mock.enableSimulation(false);
}

TEST(EnhancedDebugTest, CppWrapperTest) {
    std::cout << "Starting C++ wrapper test..." << std::endl;
    
    // Get enhanced mock instance
    auto& enhanced_mock = EnhancedTimerMock::instance();
    enhanced_mock.enableSimulation(true);
    enhanced_mock.reset();
    
    std::cout << "Enhanced simulation enabled and reset" << std::endl;
    
    int callback_count = 0;
    auto callback = [&callback_count]() {
        callback_count++;
        std::cout << "C++ wrapper callback executed! Count: " << callback_count << std::endl;
    };
    
    std::cout << "Creating C++ timer..." << std::endl;
    
    // Create timer using static allocator
    sa::timer test_timer("TestTimer", 500, pdTRUE, std::move(callback));
    
    std::cout << "C++ timer created" << std::endl;
    std::cout << "Total timer count: " << enhanced_mock.getSimulator().getTotalTimerCount() << std::endl;
    
    // Start the timer
    std::cout << "Starting C++ timer..." << std::endl;
    BaseType_t result = test_timer.start();
    std::cout << "Start result: " << result << std::endl;
    
    std::cout << "Is running: " << test_timer.running() << std::endl;
    std::cout << "Active timer count: " << enhanced_mock.getSimulator().getActiveTimerCount() << std::endl;
    
    // Advance time to trigger callback
    std::cout << "Advancing time..." << std::endl;
    enhanced_mock.getSimulator().advanceTime(500);
    
    std::cout << "Callback count: " << callback_count << std::endl;
    
    enhanced_mock.enableSimulation(false);
}