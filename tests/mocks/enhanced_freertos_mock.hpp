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

// Static instance
TaskSimulator* TaskSimulator::instance_ = nullptr;

/**
 * @brief Enhanced FreeRTOS mock class that uses TaskSimulator
 * 
 * This class extends the basic FreeRTOS mock to provide realistic
 * task execution simulation using std::thread.
 */
class EnhancedFreeRTOSMock : public FreeRTOSMock {
private:
    TaskSimulator& simulator_;
    
public:
    EnhancedFreeRTOSMock() : simulator_(TaskSimulator::getInstance()) {}
    
    ~EnhancedFreeRTOSMock() {
        simulator_.terminateAllTasks();
    }
    
    // Enhanced task creation that actually executes tasks
    MOCK_METHOD(TaskHandle_t, xTaskCreateStatic, 
                (TaskFunction_t pxTaskCode, const char* pcName, uint32_t ulStackDepth,
                 void* pvParameters, UBaseType_t uxPriority, StackType_t* puxStackBuffer,
                 StaticTask_t* pxTaskBuffer));
    
    MOCK_METHOD(BaseType_t, xTaskCreate,
                (TaskFunction_t pxTaskCode, const char* pcName, uint16_t usStackDepth,
                 void* pvParameters, UBaseType_t uxPriority, TaskHandle_t* pxCreatedTask));
    
    MOCK_METHOD(void, vTaskDelete, (TaskHandle_t xTaskToDelete));
    MOCK_METHOD(void, vTaskSuspend, (TaskHandle_t xTaskToSuspend));
    MOCK_METHOD(void, vTaskResume, (TaskHandle_t xTaskToResume));
    MOCK_METHOD(eTaskState, eTaskGetState, (TaskHandle_t xTask));
    MOCK_METHOD(const char*, pcTaskGetName, (TaskHandle_t xTaskToQuery));
    MOCK_METHOD(UBaseType_t, uxTaskGetNumberOfTasks, ());
    
    // Actual task creation with simulation
    TaskHandle_t createTaskWithSimulation(TaskFunction_t pxTaskCode, const char* pcName, uint32_t ulStackDepth,
                                         void* pvParameters, UBaseType_t uxPriority, StackType_t* puxStackBuffer,
                                         StaticTask_t* pxTaskBuffer) {
        return simulator_.createTask(pxTaskCode, pcName, pvParameters, uxPriority);
    }
    
    BaseType_t createDynamicTaskWithSimulation(TaskFunction_t pxTaskCode, const char* pcName, uint16_t usStackDepth,
                                              void* pvParameters, UBaseType_t uxPriority, TaskHandle_t* pxCreatedTask) {
        if (pxCreatedTask) {
            *pxCreatedTask = simulator_.createTask(pxTaskCode, pcName, pvParameters, uxPriority);
            return pdPASS;
        }
        return pdFAIL;
    }
    
    void deleteTaskWithSimulation(TaskHandle_t xTaskToDelete) {
        simulator_.deleteTask(xTaskToDelete);
    }
    
    void suspendTaskWithSimulation(TaskHandle_t xTaskToSuspend) {
        simulator_.suspendTask(xTaskToSuspend);
    }
    
    void resumeTaskWithSimulation(TaskHandle_t xTaskToResume) {
        simulator_.resumeTask(xTaskToResume);
    }
    
    eTaskState getTaskStateWithSimulation(TaskHandle_t xTask) {
        return simulator_.getTaskState(xTask);
    }
    
    const char* getTaskNameWithSimulation(TaskHandle_t xTaskToQuery) {
        return simulator_.getTaskName(xTaskToQuery);
    }
    
    UBaseType_t getNumberOfTasksWithSimulation() {
        return static_cast<UBaseType_t>(simulator_.getTaskCount());
    }
    
    // Utility methods for testing
    void waitForTask(TaskHandle_t handle, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
        simulator_.waitForTask(handle, timeout);
    }
    
    size_t getActiveTaskCount() {
        return simulator_.getTaskCount();
    }
};

/**
 * @brief Enhanced mock instance manager that uses existing global mock
 */
class EnhancedMockInstance {
public:
    static std::shared_ptr<EnhancedFreeRTOSMock> getInstance() {
        if (!instance_) {
            instance_ = std::make_shared<EnhancedFreeRTOSMock>();
            extern FreeRTOSMock* g_freertos_mock;
            g_freertos_mock = instance_.get();
        }
        return instance_;
    }
    
    static void resetInstance() {
        TaskSimulator::resetInstance();
        extern FreeRTOSMock* g_freertos_mock;
        g_freertos_mock = nullptr;
        instance_.reset();
    }
    
private:
    static std::shared_ptr<EnhancedFreeRTOSMock> instance_;
};

std::shared_ptr<EnhancedFreeRTOSMock> EnhancedMockInstance::instance_;

} // namespace enhanced_mock