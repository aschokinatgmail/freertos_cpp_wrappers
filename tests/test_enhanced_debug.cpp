/**
 * @file test_enhanced_debug.cpp
 * @brief Simple debug test for enhanced timer mocks
 */

#include <gtest/gtest.h>
#include <iostream>

// Include mocks first
#include "FreeRTOS.h"
#include "enhanced_timer_mocks.hpp"

using namespace testing;
using namespace freertos_mocks;

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