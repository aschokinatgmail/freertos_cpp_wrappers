/**
 * @file enhanced_timer_mocks.cpp
 * @brief Implementation of enhanced FreeRTOS software timer mocks
 */

#include "enhanced_timer_mocks.hpp"
#include <algorithm>
#include <iostream>

namespace freertos_mocks {

// TimerServiceSimulator implementation

TimerServiceSimulator::TimerServiceSimulator()
    : current_time_(0), next_handle_(reinterpret_cast<TimerHandle_t>(0x1000)), track_callbacks_(false), immediate_processing_(true) {
}

TimerHandle_t TimerServiceSimulator::generateHandle() {
    TimerHandle_t handle = next_handle_;
    next_handle_ = reinterpret_cast<TimerHandle_t>(
        reinterpret_cast<uintptr_t>(next_handle_) + 1);
    return handle;
}

bool TimerServiceSimulator::isValidTimer(TimerHandle_t timer) const {
    return timer != nullptr && timers_.find(timer) != timers_.end();
}

TimerHandle_t TimerServiceSimulator::createTimer(const char* name, TickType_t period, 
                                               UBaseType_t auto_reload, void* timer_id, 
                                               TimerCallbackFunction_t callback, bool is_static) {
    if (!callback || period == 0) {
        return nullptr;  // Invalid parameters
    }
    
    TimerHandle_t handle = generateHandle();
    auto timer_info = std::make_unique<TimerInfo>(name, period, auto_reload, timer_id, callback);
    timer_info->creation_time = current_time_;
    
    timers_[handle] = std::move(timer_info);
    
    if (track_callbacks_) {
        callback_counts_[handle] = 0;
    }
    
    return handle;
}

BaseType_t TimerServiceSimulator::deleteTimer(TimerHandle_t timer, TickType_t ticks_to_wait) {
    if (!isValidTimer(timer)) {
        return pdFAIL;
    }
    
    // For delete operations, process immediately to avoid destructor hanging
    auto& info = timers_[timer];
    info->is_active = false;
    info->expiry_time = 0;
    
    if (track_callbacks_) {
        callback_counts_.erase(timer);
    }
    
    timers_.erase(timer);
    
    return pdPASS;
}

BaseType_t TimerServiceSimulator::startTimer(TimerHandle_t timer, TickType_t ticks_to_wait) {
    if (!isValidTimer(timer)) {
        return pdFAIL;
    }
    
    // Add start command to queue
    command_queue_.emplace(timer, TimerCommand::Start);
    command_queue_.back().command_time = current_time_;
    
    // Process commands immediately if enabled (default for unit testing)
    if (immediate_processing_) {
        processTimerCommands();
    }
    
    return pdPASS;
}

BaseType_t TimerServiceSimulator::stopTimer(TimerHandle_t timer, TickType_t ticks_to_wait) {
    if (!isValidTimer(timer)) {
        return pdFAIL;
    }
    
    // Add stop command to queue
    command_queue_.emplace(timer, TimerCommand::Stop);
    command_queue_.back().command_time = current_time_;
    
    // Process commands immediately if enabled (default for unit testing)
    if (immediate_processing_) {
        processTimerCommands();
    }
    
    return pdPASS;
}

BaseType_t TimerServiceSimulator::resetTimer(TimerHandle_t timer, TickType_t ticks_to_wait) {
    if (!isValidTimer(timer)) {
        return pdFAIL;
    }
    
    // Add reset command to queue
    command_queue_.emplace(timer, TimerCommand::Reset);
    command_queue_.back().command_time = current_time_;
    
    // Process commands immediately if enabled (default for unit testing)
    if (immediate_processing_) {
        processTimerCommands();
    }
    
    return pdPASS;
}

BaseType_t TimerServiceSimulator::changePeriod(TimerHandle_t timer, TickType_t new_period, 
                                              TickType_t ticks_to_wait) {
    if (!isValidTimer(timer) || new_period == 0) {
        return pdFAIL;
    }
    
    // Add change period command to queue
    command_queue_.emplace(timer, TimerCommand::ChangePeriod, new_period);
    command_queue_.back().command_time = current_time_;
    
    // Process commands immediately if enabled (default for unit testing)
    if (immediate_processing_) {
        processTimerCommands();
    }
    
    return pdPASS;
}

// ISR Functions - behave the same as normal functions but simulate interrupt context
BaseType_t TimerServiceSimulator::startTimerFromISR(TimerHandle_t timer, BaseType_t* higher_priority_task_woken) {
    if (higher_priority_task_woken) {
        *higher_priority_task_woken = pdFALSE;  // Simulate no high priority task woken for testing
    }
    return startTimer(timer, 0);  // ISR functions typically don't wait
}

BaseType_t TimerServiceSimulator::stopTimerFromISR(TimerHandle_t timer, BaseType_t* higher_priority_task_woken) {
    if (higher_priority_task_woken) {
        *higher_priority_task_woken = pdFALSE;  // Simulate no high priority task woken for testing
    }
    return stopTimer(timer, 0);  // ISR functions typically don't wait
}

BaseType_t TimerServiceSimulator::resetTimerFromISR(TimerHandle_t timer, BaseType_t* higher_priority_task_woken) {
    if (higher_priority_task_woken) {
        *higher_priority_task_woken = pdFALSE;  // Simulate no high priority task woken for testing
    }
    return resetTimer(timer, 0);  // ISR functions typically don't wait
}

BaseType_t TimerServiceSimulator::changePeriodFromISR(TimerHandle_t timer, TickType_t new_period, BaseType_t* higher_priority_task_woken) {
    if (higher_priority_task_woken) {
        *higher_priority_task_woken = pdFALSE;  // Simulate no high priority task woken for testing
    }
    return changePeriod(timer, new_period, 0);  // ISR functions typically don't wait
}

BaseType_t TimerServiceSimulator::isTimerActive(TimerHandle_t timer) {
    if (!isValidTimer(timer)) {
        return pdFALSE;
    }
    
    return timers_[timer]->is_active ? pdTRUE : pdFALSE;
}

TickType_t TimerServiceSimulator::getTimerPeriod(TimerHandle_t timer) {
    if (!isValidTimer(timer)) {
        return 0;
    }
    
    return timers_[timer]->period_ticks;
}

TickType_t TimerServiceSimulator::getTimerExpiryTime(TimerHandle_t timer) {
    if (!isValidTimer(timer)) {
        return 0;
    }
    
    return timers_[timer]->expiry_time;
}

UBaseType_t TimerServiceSimulator::getReloadMode(TimerHandle_t timer) {
    if (!isValidTimer(timer)) {
        return pdFALSE;
    }
    
    return timers_[timer]->auto_reload;
}

void TimerServiceSimulator::setReloadMode(TimerHandle_t timer, UBaseType_t auto_reload) {
    if (isValidTimer(timer)) {
        timers_[timer]->auto_reload = auto_reload;
    }
}

const char* TimerServiceSimulator::getTimerName(TimerHandle_t timer) {
    if (!isValidTimer(timer)) {
        return nullptr;
    }
    
    return timers_[timer]->name;
}

void* TimerServiceSimulator::getTimerID(TimerHandle_t timer) {
    if (!isValidTimer(timer)) {
        return nullptr;
    }
    
    return timers_[timer]->timer_id;
}

void TimerServiceSimulator::scheduleTimerExpiry(TimerHandle_t timer, TickType_t delay, TickType_t base_time) {
    if (isValidTimer(timer)) {
        auto& info = timers_[timer];
        TickType_t time_base = (base_time > 0) ? base_time : current_time_;
        info->expiry_time = time_base + (delay > 0 ? delay : info->period_ticks);
    }
}

void TimerServiceSimulator::executeCallback(TimerHandle_t timer) {
    if (!isValidTimer(timer)) {
        return;
    }
    
    auto& info = timers_[timer];
    if (info->callback) {
        // Execute the callback
        info->callback(timer);
        
        // Track callback execution for testing
        if (track_callbacks_) {
            callback_counts_[timer]++;
        }
    }
}

void TimerServiceSimulator::processTimerCommands() {
    while (!command_queue_.empty()) {
        auto command = command_queue_.front();
        command_queue_.pop();
        
        if (!isValidTimer(command.timer)) {
            continue;  // Timer was deleted
        }
        
        auto& info = timers_[command.timer];
        
        switch (command.command) {
        case TimerCommand::Start:
            if (!info->is_active) {
                info->is_active = true;
                scheduleTimerExpiry(command.timer, 0, command.command_time);
            }
            break;
            
        case TimerCommand::Stop:
            info->is_active = false;
            info->expiry_time = 0;
            break;
            
        case TimerCommand::Reset:
            if (info->is_active) {
                scheduleTimerExpiry(command.timer);
            } else {
                info->is_active = true;
                scheduleTimerExpiry(command.timer);
            }
            break;
            
        case TimerCommand::ChangePeriod:
            info->period_ticks = command.parameter;
            if (info->is_active) {
                scheduleTimerExpiry(command.timer);
            }
            break;
            
        case TimerCommand::Delete:
            info->is_active = false;
            if (track_callbacks_) {
                callback_counts_.erase(command.timer);
            }
            timers_.erase(command.timer);
            break;
        }
    }
}

void TimerServiceSimulator::checkTimerExpirations() {
    std::vector<TimerHandle_t> expired_timers;
    
    // Find expired timers
    for (auto& pair : timers_) {
        auto& info = pair.second;
        if (info->is_active && info->expiry_time <= current_time_) {
            expired_timers.push_back(pair.first);
        }
    }
    
    // Process expired timers
    for (TimerHandle_t timer : expired_timers) {
        if (!isValidTimer(timer)) {
            continue;  // Timer was deleted during processing
        }
        
        auto& info = timers_[timer];
        
        // Execute callback
        executeCallback(timer);
        
        // Handle auto-reload vs single-shot
        if (info->auto_reload == pdTRUE) {
            // Auto-reload: schedule next expiry
            scheduleTimerExpiry(timer);
        } else {
            // Single-shot: stop the timer
            info->is_active = false;
            info->expiry_time = 0;
        }
    }
}

void TimerServiceSimulator::serviceTimers() {
    processTimerCommands();
    checkTimerExpirations();
}

void TimerServiceSimulator::advanceTime(TickType_t ticks) {
    TickType_t target_time = current_time_ + ticks;
    
    // Advance time step by step to catch all expirations
    while (current_time_ < target_time) {
        current_time_++;
        serviceTimers();
    }
}

void TimerServiceSimulator::reset() {
    timers_.clear();
    while (!command_queue_.empty()) {
        command_queue_.pop();
    }
    current_time_ = 0;
    next_handle_ = reinterpret_cast<TimerHandle_t>(0x1000);
    callback_counts_.clear();
}

size_t TimerServiceSimulator::getTotalTimerCount() const {
    return timers_.size();
}

size_t TimerServiceSimulator::getActiveTimerCount() const {
    size_t count = 0;
    for (const auto& pair : timers_) {
        if (pair.second->is_active) {
            count++;
        }
    }
    return count;
}

size_t TimerServiceSimulator::getPendingCommandCount() const {
    return command_queue_.size();
}

bool TimerServiceSimulator::hasTimer(TimerHandle_t timer) const {
    return isValidTimer(timer);
}

size_t TimerServiceSimulator::getCallbackCount(TimerHandle_t timer) const {
    auto it = callback_counts_.find(timer);
    return it != callback_counts_.end() ? it->second : 0;
}

void TimerServiceSimulator::clearCallbackHistory() {
    callback_counts_.clear();
}

// EnhancedTimerMock implementation

EnhancedTimerMock& EnhancedTimerMock::instance() {
    static EnhancedTimerMock instance;
    return instance;
}

} // namespace freertos_mocks