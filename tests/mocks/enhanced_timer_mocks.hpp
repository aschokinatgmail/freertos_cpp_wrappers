#pragma once

/**
 * @file enhanced_timer_mocks.hpp
 * @brief Enhanced FreeRTOS software timer mocks with realistic timer service simulation
 * 
 * This header provides an enhanced timer mock system that simulates realistic
 * FreeRTOS timer behavior including:
 * - Timer service task simulation
 * - Time-based callback execution
 * - Timer state management over time
 * - Command queue simulation
 * - Multi-timer coordination
 * 
 * The enhanced mocks enable testing of:
 * - Actual callback invocation when timers expire
 * - Single-shot vs auto-reload timer behavior
 * - Timer interactions and coordination
 * - Realistic timing scenarios
 * - Error conditions and edge cases
 */

#include "FreeRTOS.h"
#include <unordered_map>
#include <functional>
#include <chrono>
#include <queue>
#include <memory>
#include <thread>

namespace freertos_mocks {

/**
 * @brief Enhanced timer information structure
 * 
 * Tracks complete timer state for realistic simulation
 */
struct TimerInfo {
    const char* name;
    TickType_t period_ticks;
    UBaseType_t auto_reload;
    void* timer_id;
    TimerCallbackFunction_t callback;
    bool is_active;
    TickType_t expiry_time;
    TickType_t creation_time;
    
    TimerInfo(const char* n, TickType_t p, UBaseType_t ar, void* id, TimerCallbackFunction_t cb)
        : name(n), period_ticks(p), auto_reload(ar), timer_id(id), callback(cb),
          is_active(false), expiry_time(0), creation_time(0) {}
};

/**
 * @brief Timer command types for queue simulation
 */
enum class TimerCommand {
    Start,
    Stop,
    Reset,
    ChangePeriod,
    Delete
};

/**
 * @brief Timer command structure for queue simulation
 */
struct TimerCommandInfo {
    TimerHandle_t timer;
    TimerCommand command;
    TickType_t parameter;  // Used for period changes
    TickType_t command_time;
    
    TimerCommandInfo(TimerHandle_t t, TimerCommand c, TickType_t p = 0)
        : timer(t), command(c), parameter(p), command_time(0) {}
};

/**
 * @brief Enhanced timer service simulator
 * 
 * Provides realistic simulation of FreeRTOS timer service task behavior
 */
class TimerServiceSimulator {
public:
    TimerServiceSimulator();
    ~TimerServiceSimulator() = default;
    
    // Timer management
    TimerHandle_t createTimer(const char* name, TickType_t period, UBaseType_t auto_reload,
                             void* timer_id, TimerCallbackFunction_t callback, bool is_static = false);
    BaseType_t deleteTimer(TimerHandle_t timer, TickType_t ticks_to_wait);
    
    // Timer control
    BaseType_t startTimer(TimerHandle_t timer, TickType_t ticks_to_wait);
    BaseType_t stopTimer(TimerHandle_t timer, TickType_t ticks_to_wait);
    BaseType_t resetTimer(TimerHandle_t timer, TickType_t ticks_to_wait);
    BaseType_t changePeriod(TimerHandle_t timer, TickType_t new_period, TickType_t ticks_to_wait);
    
    // Timer state queries
    BaseType_t isTimerActive(TimerHandle_t timer);
    TickType_t getTimerPeriod(TimerHandle_t timer);
    TickType_t getTimerExpiryTime(TimerHandle_t timer);
    UBaseType_t getReloadMode(TimerHandle_t timer);
    void setReloadMode(TimerHandle_t timer, UBaseType_t auto_reload);
    const char* getTimerName(TimerHandle_t timer);
    void* getTimerID(TimerHandle_t timer);
    
    // Time simulation
    void advanceTime(TickType_t ticks);
    TickType_t getCurrentTime() const { return current_time_; }
    void reset();
    
    // Timer service task simulation
    void processTimerCommands();
    void checkTimerExpirations();
    void serviceTimers();  // Combined command processing and expiration checking
    
    // Test utilities
    size_t getActiveTimerCount() const;
    size_t getTotalTimerCount() const;
    size_t getPendingCommandCount() const;
    bool hasTimer(TimerHandle_t timer) const;
    
    // Callback execution tracking (for testing)
    void enableCallbackTracking(bool enable) { track_callbacks_ = enable; }
    size_t getCallbackCount(TimerHandle_t timer) const;
    void clearCallbackHistory();
    
    // Command processing control
    void setImmediateProcessing(bool immediate) { immediate_processing_ = immediate; }
    bool isImmediateProcessing() const { return immediate_processing_; }

private:
    std::unordered_map<TimerHandle_t, std::unique_ptr<TimerInfo>> timers_;
    std::queue<TimerCommandInfo> command_queue_;
    TickType_t current_time_;
    TimerHandle_t next_handle_;
    bool track_callbacks_;
    bool immediate_processing_;
    std::unordered_map<TimerHandle_t, size_t> callback_counts_;
    
    TimerHandle_t generateHandle();
    bool isValidTimer(TimerHandle_t timer) const;
    void executeCallback(TimerHandle_t timer);
    void scheduleTimerExpiry(TimerHandle_t timer, TickType_t delay = 0, TickType_t base_time = 0);
};

/**
 * @brief Enhanced timer mock interface
 * 
 * Singleton that provides access to the timer service simulator
 * for use by the mock functions
 */
class EnhancedTimerMock {
public:
    static EnhancedTimerMock& instance();
    
    TimerServiceSimulator& getSimulator() { return simulator_; }
    
    // Enable/disable enhanced simulation
    void enableSimulation(bool enable) { simulation_enabled_ = enable; }
    bool isSimulationEnabled() const { return simulation_enabled_; }
    
    void reset() { simulator_.reset(); }
    
    // vTaskDelay simulation - works with timer time advancement
    void simulateTaskDelay(TickType_t ticks) {
        if (simulation_enabled_) {
            // In enhanced simulation mode, vTaskDelay just advances simulated time
            simulator_.advanceTime(ticks);
        } else {
            // In regular mode, simulate with actual sleep (much shorter for testing)
            std::this_thread::sleep_for(std::chrono::microseconds(ticks * 10));
        }
    }

private:
    EnhancedTimerMock() : simulation_enabled_(false) {}  // Disabled by default
    TimerServiceSimulator simulator_;
    bool simulation_enabled_;
};

} // namespace freertos_mocks