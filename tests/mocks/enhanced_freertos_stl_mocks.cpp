/**
 * @file enhanced_freertos_stl_mocks.cpp
 * @author GitHub Copilot AI Assistant
 * @brief Enhanced FreeRTOS mocks that can use STL implementations
 * @version 1.0
 * @date 2024-12-20
 * 
 * This file provides enhanced mock implementations that can optionally use
 * STL-based semaphore implementations for more realistic testing scenarios.
 */

#include "enhanced_freertos_mock.hpp"
#include "stl_semaphore_mocks.hpp"
#include <unordered_map>
#include <memory>

// Static instance definitions
namespace enhanced_mock {
TaskSimulator* TaskSimulator::instance_ = nullptr;
std::shared_ptr<EnhancedFreeRTOSMock> EnhancedMockInstance::instance_;
}

namespace freertos_test {

// Global registry for STL semaphore implementations
static std::unordered_map<SemaphoreHandle_t, std::unique_ptr<stl_binary_semaphore>> g_stl_binary_semaphores;
static std::unordered_map<SemaphoreHandle_t, std::unique_ptr<stl_counting_semaphore>> g_stl_counting_semaphores;
static std::unordered_map<SemaphoreHandle_t, std::unique_ptr<stl_mutex>> g_stl_mutexes;
static std::unordered_map<SemaphoreHandle_t, std::unique_ptr<stl_recursive_mutex>> g_stl_recursive_mutexes;
static std::mutex g_registry_mutex;

// Enhanced mock mode control
static bool g_use_stl_implementations = false;

void enable_stl_implementations(bool enable) {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    g_use_stl_implementations = enable;
}

bool is_stl_implementations_enabled() {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    return g_use_stl_implementations;
}

void clear_stl_registries() {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    g_stl_binary_semaphores.clear();
    g_stl_counting_semaphores.clear();
    g_stl_mutexes.clear();
    g_stl_recursive_mutexes.clear();
}

// Enhanced semaphore creation functions
SemaphoreHandle_t create_stl_binary_semaphore() {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    auto semaphore = std::make_unique<stl_binary_semaphore>();
    SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(semaphore.get());
    g_stl_binary_semaphores[handle] = std::move(semaphore);
    return handle;
}

SemaphoreHandle_t create_stl_counting_semaphore(UBaseType_t max_count, UBaseType_t initial_count) {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    auto semaphore = std::make_unique<stl_counting_semaphore>(max_count, initial_count);
    SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(semaphore.get());
    g_stl_counting_semaphores[handle] = std::move(semaphore);
    return handle;
}

SemaphoreHandle_t create_stl_mutex() {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    auto mutex = std::make_unique<stl_mutex>();
    SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(mutex.get());
    g_stl_mutexes[handle] = std::move(mutex);
    return handle;
}

SemaphoreHandle_t create_stl_recursive_mutex() {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    auto mutex = std::make_unique<stl_recursive_mutex>();
    SemaphoreHandle_t handle = reinterpret_cast<SemaphoreHandle_t>(mutex.get());
    g_stl_recursive_mutexes[handle] = std::move(mutex);
    return handle;
}

// Enhanced semaphore operation functions
BaseType_t stl_semaphore_give(SemaphoreHandle_t handle) {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    
    // Check if it's a binary semaphore
    auto bin_it = g_stl_binary_semaphores.find(handle);
    if (bin_it != g_stl_binary_semaphores.end()) {
        return bin_it->second->give();
    }
    
    // Check if it's a counting semaphore
    auto count_it = g_stl_counting_semaphores.find(handle);
    if (count_it != g_stl_counting_semaphores.end()) {
        return count_it->second->give();
    }
    
    // Check if it's a mutex
    auto mutex_it = g_stl_mutexes.find(handle);
    if (mutex_it != g_stl_mutexes.end()) {
        return mutex_it->second->unlock();
    }
    
    // Check if it's a recursive mutex
    auto rec_mutex_it = g_stl_recursive_mutexes.find(handle);
    if (rec_mutex_it != g_stl_recursive_mutexes.end()) {
        return rec_mutex_it->second->unlock();
    }
    
    return pdFALSE;
}

BaseType_t stl_semaphore_take(SemaphoreHandle_t handle, TickType_t timeout) {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    
    // Check if it's a binary semaphore
    auto bin_it = g_stl_binary_semaphores.find(handle);
    if (bin_it != g_stl_binary_semaphores.end()) {
        return bin_it->second->take(timeout);
    }
    
    // Check if it's a counting semaphore
    auto count_it = g_stl_counting_semaphores.find(handle);
    if (count_it != g_stl_counting_semaphores.end()) {
        return count_it->second->take(timeout);
    }
    
    // Check if it's a mutex
    auto mutex_it = g_stl_mutexes.find(handle);
    if (mutex_it != g_stl_mutexes.end()) {
        return mutex_it->second->lock(timeout);
    }
    
    // Check if it's a recursive mutex
    auto rec_mutex_it = g_stl_recursive_mutexes.find(handle);
    if (rec_mutex_it != g_stl_recursive_mutexes.end()) {
        return rec_mutex_it->second->lock(timeout);
    }
    
    return pdFALSE;
}

BaseType_t stl_semaphore_take_recursive(SemaphoreHandle_t handle, TickType_t timeout) {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    
    auto rec_mutex_it = g_stl_recursive_mutexes.find(handle);
    if (rec_mutex_it != g_stl_recursive_mutexes.end()) {
        return rec_mutex_it->second->lock(timeout);
    }
    
    return pdFALSE;
}

BaseType_t stl_semaphore_give_recursive(SemaphoreHandle_t handle) {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    
    auto rec_mutex_it = g_stl_recursive_mutexes.find(handle);
    if (rec_mutex_it != g_stl_recursive_mutexes.end()) {
        return rec_mutex_it->second->unlock();
    }
    
    return pdFALSE;
}

UBaseType_t stl_semaphore_get_count(SemaphoreHandle_t handle) {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    
    auto count_it = g_stl_counting_semaphores.find(handle);
    if (count_it != g_stl_counting_semaphores.end()) {
        return count_it->second->count();
    }
    
    return 0;
}

void stl_semaphore_delete(SemaphoreHandle_t handle) {
    std::lock_guard<std::mutex> lock(g_registry_mutex);
    
    // Remove from all registries
    g_stl_binary_semaphores.erase(handle);
    g_stl_counting_semaphores.erase(handle);
    g_stl_mutexes.erase(handle);
    g_stl_recursive_mutexes.erase(handle);
}

} // namespace freertos_test