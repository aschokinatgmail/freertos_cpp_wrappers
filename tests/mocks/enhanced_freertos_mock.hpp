#pragma once

// Enhanced FreeRTOS mock with std::thread simulation
// This provides more realistic multitasking simulation for comprehensive testing

#include "FreeRTOS.h"
#include "freertos_mocks.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <functional>

namespace enhanced_mock {

/**
 * @brief Enhanced task simulator using std::thread
 * 
 * This class provides realistic task execution simulation using std::thread
 * to enable testing of actual task execution paths and multitasking scenarios.
 */
class TaskSimulator {
public:
    struct TaskInfo {
        std::thread thread;
        TaskFunction_t function;
        void* context;
        std::string name;
        UBaseType_t priority;
        std::atomic<bool> suspended{false};
        std::atomic<bool> should_terminate{false};
        std::atomic<eTaskState> state{eReady};
        std::mutex state_mutex;
        std::condition_variable resume_cv;
        
        TaskInfo(TaskFunction_t func, const char* task_name, UBaseType_t prio, void* ctx)
            : function(func), context(ctx), name(task_name ? task_name : ""), priority(prio) {}
    };

private:
    std::map<TaskHandle_t, std::unique_ptr<TaskInfo>> tasks_;
    std::mutex tasks_mutex_;
    std::atomic<uint32_t> next_handle_{1};
    
    static TaskSimulator* instance_;
    
public:
    static TaskSimulator& getInstance() {
        if (!instance_) {
            instance_ = new TaskSimulator();
        }
        return *instance_;
    }
    
    static void resetInstance() {
        if (instance_) {
            instance_->terminateAllTasks();
            delete instance_;
            instance_ = nullptr;
        }
    }
    
    // Simulate task creation with actual thread execution
    TaskHandle_t createTask(TaskFunction_t taskFunction, 
                           const char* name,
                           void* context,
                           UBaseType_t priority,
                           bool startSuspended = false) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        
        auto handle = reinterpret_cast<TaskHandle_t>(next_handle_++);
        auto task_info = std::make_unique<TaskInfo>(taskFunction, name, priority, context);
        
        if (startSuspended) {
            task_info->suspended = true;
            task_info->state = eSuspended;
        }
        
        // Create thread that will execute the task function
        task_info->thread = std::thread([this, handle, taskFunction, context, startSuspended]() {
            executeTask(handle, taskFunction, context, startSuspended);
        });
        
        tasks_[handle] = std::move(task_info);
        return handle;
    }
    
    // Simulate task suspension
    void suspendTask(TaskHandle_t handle) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = tasks_.find(handle);
        if (it != tasks_.end()) {
            it->second->suspended = true;
            it->second->state = eSuspended;
        }
    }
    
    // Simulate task resumption
    void resumeTask(TaskHandle_t handle) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = tasks_.find(handle);
        if (it != tasks_.end()) {
            it->second->suspended = false;
            it->second->state = eReady;
            it->second->resume_cv.notify_all();
        }
    }
    
    // Simulate task deletion
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
        
        if (task_info) {
            task_info->should_terminate = true;
            task_info->suspended = false;
            task_info->resume_cv.notify_all();
            
            if (task_info->thread.joinable()) {
                task_info->thread.join();
            }
        }
    }
    
    // Get task state
    eTaskState getTaskState(TaskHandle_t handle) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = tasks_.find(handle);
        if (it != tasks_.end()) {
            return it->second->state.load();
        }
        return eInvalid;
    }
    
    // Get task name
    const char* getTaskName(TaskHandle_t handle) {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        auto it = tasks_.find(handle);
        if (it != tasks_.end()) {
            return it->second->name.c_str();
        }
        return "InvalidTask";
    }
    
    // Get number of active tasks
    size_t getTaskCount() {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        return tasks_.size();
    }
    
    // Terminate all tasks (cleanup)
    void terminateAllTasks() {
        std::map<TaskHandle_t, std::unique_ptr<TaskInfo>> tasks_to_cleanup;
        {
            std::lock_guard<std::mutex> lock(tasks_mutex_);
            tasks_to_cleanup = std::move(tasks_);
            tasks_.clear();
        }
        
        // Terminate all tasks
        for (auto& [handle, task_info] : tasks_to_cleanup) {
            task_info->should_terminate = true;
            task_info->suspended = false;
            task_info->resume_cv.notify_all();
            
            if (task_info->thread.joinable()) {
                task_info->thread.join();
            }
        }
    }
    
    // Wait for a task to complete (for testing)
    void waitForTask(TaskHandle_t handle, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
        std::unique_ptr<TaskInfo> task_info;
        {
            std::lock_guard<std::mutex> lock(tasks_mutex_);
            auto it = tasks_.find(handle);
            if (it != tasks_.end()) {
                // Move the task info temporarily
                task_info = std::move(it->second);
                tasks_.erase(it);
            }
        }
        
        if (task_info && task_info->thread.joinable()) {
            // Create a detached thread to handle timeout
            std::thread timeout_thread([&task_info, timeout]() {
                std::this_thread::sleep_for(timeout);
                if (task_info->thread.joinable()) {
                    task_info->should_terminate = true;
                    task_info->resume_cv.notify_all();
                }
            });
            timeout_thread.detach();
            
            task_info->thread.join();
        }
    }

private:
    void executeTask(TaskHandle_t handle, TaskFunction_t taskFunction, void* context, bool startSuspended) {
        // Wait if task should start suspended
        if (startSuspended) {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            auto it = tasks_.find(handle);
            if (it != tasks_.end()) {
                it->second->resume_cv.wait(lock, [this, handle]() {
                    auto task_it = tasks_.find(handle);
                    return task_it == tasks_.end() || 
                           !task_it->second->suspended.load() ||
                           task_it->second->should_terminate.load();
                });
            }
        }
        
        // Check if task should be terminated before execution
        {
            std::lock_guard<std::mutex> lock(tasks_mutex_);
            auto it = tasks_.find(handle);
            if (it == tasks_.end() || it->second->should_terminate.load()) {
                return;
            }
            it->second->state = eRunning;
        }
        
        // Execute the actual task function
        try {
            taskFunction(context);
        } catch (...) {
            // Handle any exceptions from task function
        }
        
        // Mark task as completed
        {
            std::lock_guard<std::mutex> lock(tasks_mutex_);
            auto it = tasks_.find(handle);
            if (it != tasks_.end()) {
                it->second->state = eDeleted;
            }
        }
    }
};

} // namespace enhanced_mock

// STL-based semaphore testing extensions
namespace freertos_test {

/**
 * @brief Enable or disable STL implementations for enhanced testing
 * @param enable true to use STL implementations, false to use normal mocks
 */
void enable_stl_implementations(bool enable);

/**
 * @brief Check if STL implementations are enabled
 * @return bool true if STL implementations are enabled
 */
bool is_stl_implementations_enabled();

/**
 * @brief Clear all STL implementation registries
 */
void clear_stl_registries();

// Enhanced semaphore creation functions
SemaphoreHandle_t create_stl_binary_semaphore();
SemaphoreHandle_t create_stl_counting_semaphore(UBaseType_t max_count, UBaseType_t initial_count);
SemaphoreHandle_t create_stl_mutex();
SemaphoreHandle_t create_stl_recursive_mutex();

// Enhanced semaphore operation functions
BaseType_t stl_semaphore_give(SemaphoreHandle_t handle);
BaseType_t stl_semaphore_take(SemaphoreHandle_t handle, TickType_t timeout);
BaseType_t stl_semaphore_take_recursive(SemaphoreHandle_t handle, TickType_t timeout);
BaseType_t stl_semaphore_give_recursive(SemaphoreHandle_t handle);
UBaseType_t stl_semaphore_get_count(SemaphoreHandle_t handle);
void stl_semaphore_delete(SemaphoreHandle_t handle);

} // namespace freertos_test