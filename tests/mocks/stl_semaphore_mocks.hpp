/**
 * @file stl_semaphore_mocks.hpp
 * @author GitHub Copilot AI Assistant
 * @brief STL-based semaphore implementations for FreeRTOS semaphore testing
 * @version 1.0
 * @date 2024-12-20
 * 
 * This file provides STL-based implementations of FreeRTOS semaphore functionality
 * using standard C++ library primitives like std::mutex, std::condition_variable,
 * and std::counting_semaphore. These implementations provide actual semaphore
 * behavior for comprehensive testing of concurrent scenarios.
 *
 * Features:
 * - STL binary semaphore using std::mutex and std::condition_variable
 * - STL counting semaphore using std::counting_semaphore (C++20) or custom implementation
 * - STL mutex using std::mutex
 * - STL recursive mutex using std::recursive_mutex
 * - Timeout support using std::chrono
 * - Thread-safe operations
 * - Compatible with FreeRTOS semaphore wrapper API
 */

#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <thread>
#include "FreeRTOS.h"

namespace freertos_test {

/**
 * @brief STL-based binary semaphore implementation
 * 
 * Provides actual binary semaphore behavior using std::mutex and std::condition_variable.
 * Unlike pure mocks, this implementation allows testing of real concurrent scenarios.
 */
class stl_binary_semaphore {
private:
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_available{false};
    
public:
    stl_binary_semaphore() = default;
    ~stl_binary_semaphore() = default;
    
    // Non-copyable, non-movable
    stl_binary_semaphore(const stl_binary_semaphore&) = delete;
    stl_binary_semaphore& operator=(const stl_binary_semaphore&) = delete;
    stl_binary_semaphore(stl_binary_semaphore&&) = delete;
    stl_binary_semaphore& operator=(stl_binary_semaphore&&) = delete;
    
    /**
     * @brief Give (signal) the binary semaphore
     * @return BaseType_t pdTRUE if successful, pdFALSE otherwise
     */
    BaseType_t give() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_available = true;
        m_condition.notify_one();
        return pdTRUE;
    }
    
    /**
     * @brief Take (wait for) the binary semaphore with timeout
     * @param timeout_ms Timeout in milliseconds (portMAX_DELAY for infinite)
     * @return BaseType_t pdTRUE if acquired, pdFALSE on timeout
     */
    BaseType_t take(TickType_t timeout_ms = portMAX_DELAY) {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        if (timeout_ms == portMAX_DELAY) {
            // Infinite wait
            m_condition.wait(lock, [this] { return m_available; });
            m_available = false;
            return pdTRUE;
        } else {
            // Timed wait
            auto timeout = std::chrono::milliseconds(timeout_ms);
            if (m_condition.wait_for(lock, timeout, [this] { return m_available; })) {
                m_available = false;
                return pdTRUE;
            } else {
                return pdFALSE; // Timeout
            }
        }
    }
    
    /**
     * @brief ISR-safe give operation (simplified for host testing)
     * @param higher_priority_task_woken Not used in host testing
     * @return BaseType_t pdTRUE if successful
     */
    BaseType_t give_isr(BaseType_t* higher_priority_task_woken = nullptr) {
        // For host testing, just call regular give
        return give();
    }
    
    /**
     * @brief ISR-safe take operation (simplified for host testing)
     * @param higher_priority_task_woken Not used in host testing
     * @return BaseType_t pdTRUE if acquired immediately, pdFALSE otherwise
     */
    BaseType_t take_isr(BaseType_t* higher_priority_task_woken = nullptr) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_available) {
            m_available = false;
            return pdTRUE;
        }
        return pdFALSE;
    }
};

/**
 * @brief STL-based counting semaphore implementation
 * 
 * Provides actual counting semaphore behavior using std::mutex and std::condition_variable.
 */
class stl_counting_semaphore {
private:
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    UBaseType_t m_count{0};
    UBaseType_t m_max_count;
    
public:
    explicit stl_counting_semaphore(UBaseType_t max_count, UBaseType_t initial_count = 0)
        : m_count(initial_count), m_max_count(max_count) {}
    
    ~stl_counting_semaphore() = default;
    
    // Non-copyable, non-movable
    stl_counting_semaphore(const stl_counting_semaphore&) = delete;
    stl_counting_semaphore& operator=(const stl_counting_semaphore&) = delete;
    stl_counting_semaphore(stl_counting_semaphore&&) = delete;
    stl_counting_semaphore& operator=(stl_counting_semaphore&&) = delete;
    
    /**
     * @brief Give (increment) the counting semaphore
     * @return BaseType_t pdTRUE if successful, pdFALSE if already at max
     */
    BaseType_t give() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_count < m_max_count) {
            m_count++;
            m_condition.notify_one();
            return pdTRUE;
        }
        return pdFALSE; // Already at maximum count
    }
    
    /**
     * @brief Take (decrement) the counting semaphore with timeout
     * @param timeout_ms Timeout in milliseconds (portMAX_DELAY for infinite)
     * @return BaseType_t pdTRUE if acquired, pdFALSE on timeout
     */
    BaseType_t take(TickType_t timeout_ms = portMAX_DELAY) {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        if (timeout_ms == portMAX_DELAY) {
            // Infinite wait
            m_condition.wait(lock, [this] { return m_count > 0; });
            m_count--;
            return pdTRUE;
        } else {
            // Timed wait
            auto timeout = std::chrono::milliseconds(timeout_ms);
            if (m_condition.wait_for(lock, timeout, [this] { return m_count > 0; })) {
                m_count--;
                return pdTRUE;
            } else {
                return pdFALSE; // Timeout
            }
        }
    }
    
    /**
     * @brief Get current count of the semaphore
     * @return UBaseType_t Current count
     */
    UBaseType_t count() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_count;
    }
    
    /**
     * @brief ISR-safe give operation
     */
    BaseType_t give_isr(BaseType_t* higher_priority_task_woken = nullptr) {
        return give();
    }
    
    /**
     * @brief ISR-safe take operation
     */
    BaseType_t take_isr(BaseType_t* higher_priority_task_woken = nullptr) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_count > 0) {
            m_count--;
            return pdTRUE;
        }
        return pdFALSE;
    }
};

/**
 * @brief STL-based mutex implementation
 * 
 * Provides actual mutex behavior using std::mutex.
 */
class stl_mutex {
private:
    mutable std::mutex m_mutex;
    std::atomic<bool> m_locked{false};
    std::thread::id m_owner_thread;
    
public:
    stl_mutex() = default;
    ~stl_mutex() = default;
    
    // Non-copyable, non-movable
    stl_mutex(const stl_mutex&) = delete;
    stl_mutex& operator=(const stl_mutex&) = delete;
    stl_mutex(stl_mutex&&) = delete;
    stl_mutex& operator=(stl_mutex&&) = delete;
    
    /**
     * @brief Lock the mutex with timeout
     * @param timeout_ms Timeout in milliseconds (portMAX_DELAY for infinite)
     * @return BaseType_t pdTRUE if locked, pdFALSE on timeout
     */
    BaseType_t lock(TickType_t timeout_ms = portMAX_DELAY) {
        if (timeout_ms == 0) {
            // Non-blocking try lock
            if (m_mutex.try_lock()) {
                m_locked = true;
                m_owner_thread = std::this_thread::get_id();
                return pdTRUE;
            }
            return pdFALSE;
        } else if (timeout_ms == portMAX_DELAY) {
            m_mutex.lock();
            m_locked = true;
            m_owner_thread = std::this_thread::get_id();
            return pdTRUE;
        } else {
            // Simulate timeout behavior with try_lock attempts
            auto end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
            while (std::chrono::steady_clock::now() < end_time) {
                if (m_mutex.try_lock()) {
                    m_locked = true;
                    m_owner_thread = std::this_thread::get_id();
                    return pdTRUE;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return pdFALSE;
        }
    }
    
    /**
     * @brief Try to lock the mutex without blocking
     * @return BaseType_t pdTRUE if locked, pdFALSE if already locked
     */
    BaseType_t try_lock() {
        if (m_mutex.try_lock()) {
            m_locked = true;
            m_owner_thread = std::this_thread::get_id();
            return pdTRUE;
        }
        return pdFALSE;
    }
    
    /**
     * @brief Unlock the mutex
     * @return BaseType_t pdTRUE if unlocked successfully
     */
    BaseType_t unlock() {
        if (m_locked && m_owner_thread == std::this_thread::get_id()) {
            m_locked = false;
            m_mutex.unlock();
            return pdTRUE;
        }
        return pdFALSE; // Not owned by current thread
    }
    
    /**
     * @brief Check if mutex is locked
     * @return bool true if locked, false otherwise
     */
    bool locked() const {
        return m_locked.load();
    }
    
    /**
     * @brief ISR-safe lock operation (simplified for host testing)
     */
    BaseType_t lock_isr(BaseType_t* higher_priority_task_woken = nullptr) {
        return try_lock(); // ISR operations can't block
    }
    
    /**
     * @brief ISR-safe unlock operation
     */
    BaseType_t unlock_isr(BaseType_t* higher_priority_task_woken = nullptr) {
        return unlock();
    }
};

/**
 * @brief STL-based recursive mutex implementation
 * 
 * Provides actual recursive mutex behavior using std::recursive_mutex.
 */
class stl_recursive_mutex {
private:
    mutable std::recursive_mutex m_mutex;
    std::atomic<UBaseType_t> m_recursion_count{0};
    std::thread::id m_owner_thread;
    
public:
    stl_recursive_mutex() = default;
    ~stl_recursive_mutex() = default;
    
    // Non-copyable, non-movable
    stl_recursive_mutex(const stl_recursive_mutex&) = delete;
    stl_recursive_mutex& operator=(const stl_recursive_mutex&) = delete;
    stl_recursive_mutex(stl_recursive_mutex&&) = delete;
    stl_recursive_mutex& operator=(stl_recursive_mutex&&) = delete;
    
    /**
     * @brief Lock the recursive mutex with timeout
     * @param timeout_ms Timeout in milliseconds (portMAX_DELAY for infinite)
     * @return BaseType_t pdTRUE if locked, pdFALSE on timeout
     */
    BaseType_t lock(TickType_t timeout_ms = portMAX_DELAY) {
        if (timeout_ms == 0) {
            // Non-blocking try lock
            if (m_mutex.try_lock()) {
                m_recursion_count++;
                m_owner_thread = std::this_thread::get_id();
                return pdTRUE;
            }
            return pdFALSE;
        } else if (timeout_ms == portMAX_DELAY) {
            m_mutex.lock();
            m_recursion_count++;
            m_owner_thread = std::this_thread::get_id();
            return pdTRUE;
        } else {
            // Simulate timeout behavior with try_lock attempts
            auto end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
            while (std::chrono::steady_clock::now() < end_time) {
                if (m_mutex.try_lock()) {
                    m_recursion_count++;
                    m_owner_thread = std::this_thread::get_id();
                    return pdTRUE;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return pdFALSE;
        }
    }
    
    /**
     * @brief Try to lock the recursive mutex without blocking
     * @return BaseType_t pdTRUE if locked, pdFALSE if cannot acquire
     */
    BaseType_t try_lock() {
        if (m_mutex.try_lock()) {
            m_recursion_count++;
            m_owner_thread = std::this_thread::get_id();
            return pdTRUE;
        }
        return pdFALSE;
    }
    
    /**
     * @brief Unlock the recursive mutex
     * @return BaseType_t pdTRUE if unlocked successfully
     */
    BaseType_t unlock() {
        if (m_recursion_count > 0 && m_owner_thread == std::this_thread::get_id()) {
            m_recursion_count--;
            m_mutex.unlock();
            return pdTRUE;
        }
        return pdFALSE; // Not owned by current thread or not locked
    }
    
    /**
     * @brief Check if recursive mutex is locked
     * @return bool true if locked, false otherwise
     */
    bool locked() const {
        return m_recursion_count.load() > 0;
    }
    
    /**
     * @brief Get recursion count
     * @return UBaseType_t Current recursion count
     */
    UBaseType_t recursions_count() const {
        return m_recursion_count.load();
    }
    
    /**
     * @brief ISR-safe lock operation (simplified for host testing)
     */
    BaseType_t lock_isr(BaseType_t* higher_priority_task_woken = nullptr) {
        return try_lock(); // ISR operations can't block
    }
    
    /**
     * @brief ISR-safe unlock operation
     */
    BaseType_t unlock_isr(BaseType_t* higher_priority_task_woken = nullptr) {
        return unlock();
    }
};

/**
 * @brief STL-based semaphore allocator that creates STL implementations
 * 
 * This allocator creates STL-based semaphore implementations instead of 
 * FreeRTOS handles, allowing for actual semaphore behavior in tests.
 */
class stl_semaphore_allocator {
public:
    stl_semaphore_allocator() = default;
    ~stl_semaphore_allocator() = default;
    
    /**
     * @brief Create STL binary semaphore
     * @return SemaphoreHandle_t Handle to STL binary semaphore
     */
    SemaphoreHandle_t create_binary() {
        return reinterpret_cast<SemaphoreHandle_t>(new stl_binary_semaphore());
    }
    
    /**
     * @brief Create STL counting semaphore
     * @param max_count Maximum count for the semaphore
     * @return SemaphoreHandle_t Handle to STL counting semaphore
     */
    SemaphoreHandle_t create_counting(UBaseType_t max_count) {
        return reinterpret_cast<SemaphoreHandle_t>(new stl_counting_semaphore(max_count, max_count));
    }
    
    /**
     * @brief Create STL mutex
     * @return SemaphoreHandle_t Handle to STL mutex
     */
    SemaphoreHandle_t create_mutex() {
        return reinterpret_cast<SemaphoreHandle_t>(new stl_mutex());
    }
    
    /**
     * @brief Create STL recursive mutex
     * @return SemaphoreHandle_t Handle to STL recursive mutex
     */
    SemaphoreHandle_t create_recursive_mutex() {
        return reinterpret_cast<SemaphoreHandle_t>(new stl_recursive_mutex());
    }
};

} // namespace freertos_test