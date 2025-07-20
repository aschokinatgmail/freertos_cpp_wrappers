/**********************************************************************************
@file freertos_semaphore.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS semaphore wrapper
@version 0.1
@date 2024-04-07

The MIT License (MIT)

FreeRTOS C++ Wrappers Library
https://github.com/aschokinatgmail/freertos_cpp_wrappers/

Copyright(c) 2024 Andrey V. Shchekin <aschokin@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***********************************************************************************/

#pragma once

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#include <FreeRTOS.h>
#include <chrono>
#include <semphr.h>
#include <task.h>
#include <ctime>

namespace freertos {

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief An allocator for the semaphore that uses a static memory allocation.
 *
 */
class static_semaphore_allocator {
  StaticSemaphore_t m_semaphore_placeholder{};

public:
  static_semaphore_allocator() = default;
  ~static_semaphore_allocator() = default;
  static_semaphore_allocator(const static_semaphore_allocator &) = delete;
  static_semaphore_allocator(static_semaphore_allocator &&) = delete;

  static_semaphore_allocator &
  operator=(const static_semaphore_allocator &) = delete;
  static_semaphore_allocator &operator=(static_semaphore_allocator &&) = delete;

  SemaphoreHandle_t create_binary() {
    return xSemaphoreCreateBinaryStatic(&m_semaphore_placeholder);
  }
  SemaphoreHandle_t create_counting(UBaseType_t max_count) {
    return xSemaphoreCreateCountingStatic(max_count, max_count,
                                          &m_semaphore_placeholder);
  }
  SemaphoreHandle_t create_mutex() {
    return xSemaphoreCreateMutexStatic(&m_semaphore_placeholder);
  }
  SemaphoreHandle_t create_recursive_mutex() {
    return xSemaphoreCreateRecursiveMutexStatic(&m_semaphore_placeholder);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief An allocator for the semaphore that uses a dynamic memory allocation.
 *
 */
class dynamic_semaphore_allocator {
public:
  SemaphoreHandle_t create_binary() { return xSemaphoreCreateBinary(); }
  SemaphoreHandle_t create_counting(UBaseType_t max_count) {
    return xSemaphoreCreateCounting(max_count, max_count);
  }
  SemaphoreHandle_t create_mutex() { return xSemaphoreCreateMutex(); }
  SemaphoreHandle_t create_recursive_mutex() {
    return xSemaphoreCreateRecursiveMutex();
  }
};
#endif

/**
 * @brief A wrapper for the FreeRTOS binary semaphore.
 *
 * @tparam SemaphoreAllocator type of the semaphore allocator to use for memory
 * allocation.
 */
template <typename SemaphoreAllocator> class binary_semaphore {
  SemaphoreAllocator m_allocator{};
  SemaphoreHandle_t m_semaphore{nullptr};

public:
  /**
   * @brief Construct a new binary semaphore object
   *
   */
  binary_semaphore() : m_semaphore{m_allocator.create_binary()} {
    configASSERT(m_semaphore);
  }
  binary_semaphore(const binary_semaphore &) = delete;
  binary_semaphore(binary_semaphore &&src) = delete;
  /**
   * @brief Destruct the binary semaphore object and
   * delete the binary semaphore instance if it was created.
   *
   */
  ~binary_semaphore(void) {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  binary_semaphore &operator=(const binary_semaphore &) = delete;
  binary_semaphore &operator=(binary_semaphore &&src) = delete;

  /**
   * @brief Give the binary semaphore.
   * @ref https://www.freertos.org/a00123.html
   *
   * @return BaseType_t pdTRUE if the semaphore was successfully given,
   * otherwise pdFALSE.
   *
   */
  BaseType_t give() { return xSemaphoreGive(m_semaphore); }
  /**
   * @brief Give the binary semaphore from an ISR.
   * @ref https://www.freertos.org/a00124.html
   *
   * @param high_priority_task_woken pdTRUE if the high priority task was woken
   * by the semaphore give.
   * @return BaseType_t pdTRUE if the semaphore was successfully given,
   *
   */
  BaseType_t give_isr(BaseType_t &high_priority_task_woken) {
    return xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
  }
  /**
   * @brief Give the binary semaphore from an ISR.
   * @ref https://www.freertos.org/a00124.html
   *
   * @return BaseType_t pdTRUE if the semaphore was successfully given,
   *
   */
  BaseType_t give_isr(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    return xSemaphoreGiveFromISR(m_semaphore, &xHigherPriorityTaskWoken);
  }
  /**
   * @brief Take the binary semaphore.
   * @ref https://www.freertos.org/a00122.html
   *
   * @param ticks_to_wait timeout in ticks to wait for the semaphore.
   * @return BaseType_t pdTRUE if the semaphore was successfully taken,
   * otherwise pdFALSE.
   *
   */
  BaseType_t take(const TickType_t ticks_to_wait = portMAX_DELAY) {
    return xSemaphoreTake(m_semaphore, ticks_to_wait);
  }
  /**
   * @brief Take the binary semaphore from an ISR.
   * @ref https://www.freertos.org/xSemaphoreTakeFromISR.html
   *
   * @param high_priority_task_woken pdTRUE if the high priority task was woken
   * by the semaphore take.
   *
   * @return BaseType_t pdTRUE if the semaphore was successfully taken,
   * otherwise pdFALSE.
   *
   */
  BaseType_t take_isr(BaseType_t &high_priority_task_woken) {
    return xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
  }
  /**
   * @brief Take the binary semaphore from an ISR.
   * @ref https://www.freertos.org/xSemaphoreTakeFromISR.html
   *
   * @return BaseType_t pdTRUE if the semaphore was successfully taken,
   * otherwise pdFALSE.
   *
   */
  BaseType_t take_isr(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
  }
  /**
   * @brief Take the binary semaphore.
   *
   * @param timeout timeout to wait for the semaphore.
   * @return BaseType_t pdTRUE if the semaphore was successfully taken,
   * otherwise pdFALSE.
   *
   */
  template <typename Rep, typename Period>
  BaseType_t take(const std::chrono::duration<Rep, Period> &timeout) {
    return take(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
};

/**
 * @brief A wrapper for the FreeRTOS counting semaphore.
 *
 * @tparam SemaphoreAllocator type of the semaphore allocator to use for memory
 * allocation.
 */
template <typename SemaphoreAllocator> class counting_semaphore {
  SemaphoreAllocator m_allocator{};
  SemaphoreHandle_t m_semaphore{nullptr};

public:
  /**
   * @brief Construct a new counting semaphore object
   *
   * @param max_count maximum count of the counting semaphore.
   *
   */
  explicit counting_semaphore(UBaseType_t max_count = 1)
      : m_semaphore{m_allocator.create_counting(max_count)} {
    configASSERT(m_semaphore);
  }
  counting_semaphore(const counting_semaphore &) = delete;
  counting_semaphore(counting_semaphore &&src) = delete;
  /**
   * @brief Destruct the counting semaphore object and
   * delete the counting semaphore instance if it was created.
   *
   */
  ~counting_semaphore(void) {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  counting_semaphore &operator=(const counting_semaphore &) = delete;
  counting_semaphore &operator=(counting_semaphore &&src) = delete;

  /**
   * @brief Give the counting semaphore.
   * @ref https://www.freertos.org/a00123.html
   *
   * @return BaseType_t pdTRUE if the semaphore was successfully given,
   * otherwise pdFALSE.
   *
   */
  BaseType_t give() { return xSemaphoreGive(m_semaphore); }
  /**
   * @brief Give the counting semaphore from an ISR.
   * @ref https://www.freertos.org/a00124.html
   *
   * @param high_priority_task_woken pdTRUE if the high priority task was woken
   * by the semaphore give.
   * @return BaseType_t pdTRUE if the semaphore was successfully given,
   *
   */
  BaseType_t give_isr(BaseType_t &high_priority_task_woken) {
    return xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
  }
  /**
   * @brief Give the counting semaphore from an ISR.
   * @ref https://www.freertos.org/a00124.html
   *
   * @return BaseType_t pdTRUE if the semaphore was successfully given,
   *
   */
  BaseType_t give_isr(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    return xSemaphoreGiveFromISR(m_semaphore, &xHigherPriorityTaskWoken);
  }
  /**
   * @brief Take the counting semaphore.
   * @ref https://www.freertos.org/a00122.html
   *
   * @param ticks_to_wait timeout in ticks to wait for the semaphore.
   * @return BaseType_t pdTRUE if the semaphore was successfully taken,
   * otherwise pdFALSE.
   *
   */
  BaseType_t take(const TickType_t ticks_to_wait = portMAX_DELAY) {
    return xSemaphoreTake(m_semaphore, ticks_to_wait);
  }
  /**
   * @brief Take the counting semaphore from an ISR.
   * @ref https://www.freertos.org/xSemaphoreTakeFromISR.html
   *
   * @param high_priority_task_woken pdTRUE if the high priority task was woken
   * by the semaphore take.
   *
   * @return BaseType_t pdTRUE if the semaphore was successfully taken,
   * otherwise pdFALSE.
   *
   */
  BaseType_t take_isr(BaseType_t &high_priority_task_woken) {
    return xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
  }
  /**
   * @brief Take the counting semaphore from an ISR.
   * @ref https://www.freertos.org/xSemaphoreTakeFromISR.html
   *
   * @return BaseType_t pdTRUE if the semaphore was successfully taken,
   * otherwise pdFALSE.
   *
   */
  BaseType_t take_isr(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
  }
  /**
   * @brief Take the counting semaphore.
   *
   * @param timeout timeout to wait for the semaphore.
   * @return BaseType_t pdTRUE if the semaphore was successfully taken,
   * otherwise pdFALSE.
   *
   */
  template <typename Rep, typename Period>
  BaseType_t take(const std::chrono::duration<Rep, Period> &timeout) {
    return take(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  /**
   * @brief Give the counting semaphore.
   *
   * @return counting_semaphore& reference to the counting semaphore.
   */
  counting_semaphore &operator++() {
    give();
    return *this;
  }
  /**
   * @brief Take the counting semaphore.
   *
   * @return counting_semaphore& reference to the counting semaphore.
   */
  counting_semaphore &operator--() {
    take();
    return *this;
  }
  /**
   * @brief Give the counting semaphore.
   * Note: Post-increment returns reference instead of copy for RAII safety
   *
   * @return counting_semaphore& reference to the counting semaphore.
   */
  counting_semaphore &operator++(int) { // NOLINT(cert-dcl21-cpp): RAII class, copy is deleted
    give();
    return *this;
  }
  /**
   * @brief Take the counting semaphore.
   * Note: Post-decrement returns reference instead of copy for RAII safety
   *
   * @return counting_semaphore& reference to the counting semaphore.
   */
  counting_semaphore &operator--(int) { // NOLINT(cert-dcl21-cpp): RAII class, copy is deleted
    take();
    return *this;
  }
  /**
   * @brief Get the count of the counting semaphore.
   *
   * @return UBaseType_t count of the counting semaphore.
   */
  UBaseType_t count() { return uxSemaphoreGetCount(m_semaphore); }
  /**
   * @brief Give the counting semaphore.
   *
   * @param count the number of times to give the counting semaphore.
   * @return counting_semaphore& reference to the counting semaphore.
   */
  counting_semaphore &operator+=(UBaseType_t count) {
    while (count--) {
      give();
    }
    return *this;
  }
};

/**
 * @brief A wrapper for the FreeRTOS mutex.
 *
 * @tparam SemaphoreAllocator type of the semaphore allocator to use for memory
 * allocation.
 */
template <typename SemaphoreAllocator> class mutex {
  SemaphoreAllocator m_allocator{};
  SemaphoreHandle_t m_semaphore{nullptr};
  uint8_t m_locked : 1;

public:
  /**
   * @brief Construct a new mutex object
   *
   */
  mutex() : m_semaphore{m_allocator.create_mutex()}, m_locked{false} {
    configASSERT(m_semaphore);
  }
  mutex(const mutex &) = delete;
  mutex(mutex &&src) = delete;
  /**
   * @brief Destruct the mutex object and delete the mutex instance if it was
   * created.
   *
   */
  ~mutex(void) {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  mutex &operator=(const mutex &) = delete;
  mutex &operator=(mutex &&src) = delete;

  /**
   * @brief Unlock the mutex.
   * @ref https://www.freertos.org/a00123.html
   *
   * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
   */
  BaseType_t unlock() {
    auto rc = xSemaphoreGive(m_semaphore);
    if (rc) {
      m_locked = false;
    }
    return rc;
  }
  /**
   * @brief Unlock the mutex from an ISR.
   * @ref https://www.freertos.org/a00124.html
   *
   * @param high_priority_task_woken pdTRUE if the high priority task was woken
   * by the mutex unlock.
   * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
   */
  BaseType_t unlock_isr(BaseType_t &high_priority_task_woken) {
    auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
    if (rc) {
      m_locked = false;
    }
    return rc;
  }
  /**
   * @brief Unlock the mutex from an ISR.
   * @ref https://www.freertos.org/a00124.html
   *
   * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
   */
  BaseType_t unlock_isr(void) {
    BaseType_t high_priority_task_woken = pdFALSE;
    auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
    if (rc) {
      m_locked = false;
    }
    return rc;
  }
  /**
   * @brief Lock the mutex.
   * @ref https://www.freertos.org/a00122.html
   *
   * @param ticks_to_wait timeout in ticks to wait for the mutex.
   * @return BaseType_t pdTRUE if the mutex was successfully locked,
   */
  BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
    auto rc = xSemaphoreTake(m_semaphore, ticks_to_wait);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }
  /**
   * @brief Lock the mutex from an ISR.
   * @ref https://www.freertos.org/xSemaphoreTakeFromISR.html
   *
   * @param high_priority_task_woken pdTRUE if the high priority task was woken
   * by the mutex lock.
   * @return BaseType_t pdTRUE if the mutex was successfully locked,
   */
  BaseType_t lock_isr(BaseType_t &high_priority_task_woken) {
    auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }
  /**
   * @brief Lock the mutex from an ISR.
   * @ref https://www.freertos.org/xSemaphoreTakeFromISR.html
   *
   * @return BaseType_t pdTRUE if the mutex was successfully locked,
   */
  BaseType_t lock_isr(void) {
    BaseType_t high_priority_task_woken = pdFALSE;
    auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }
  /**
   * @brief Lock the mutex.
   *
   * @param timeout timeout to wait for the mutex.
   * @return BaseType_t pdTRUE if the mutex was successfully locked,
   */
  template <typename Rep, typename Period>
  BaseType_t lock(const std::chrono::duration<Rep, Period> &timeout) {
    return lock(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  /**
   * @brief Try to lock the mutex.
   * @ref https://www.freertos.org/xSemaphoreTake.html
   *
   * @return BaseType_t pdTRUE if the mutex was successfully locked,
   */
  BaseType_t try_lock() {
    auto rc = xSemaphoreTake(m_semaphore, 0);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }
  /**
   * @brief Get the lock status of the mutex.
   *
   * @return bool true if the mutex is locked, otherwise false.
   */
  bool locked(void) const { return m_locked; }
};

/**
 * @brief A wrapper for the FreeRTOS recursive mutex.
 *
 * @tparam SemaphoreAllocator type of the semaphore allocator to use for memory
 * allocation.
 */
template <typename SemaphoreAllocator> class recursive_mutex {
  SemaphoreAllocator m_allocator{};
  SemaphoreHandle_t m_semaphore{nullptr};
  uint8_t m_locked : 1;
  uint8_t m_lock_count{0};

public:
  /**
   * @brief Construct a new recursive mutex object
   *
   */
  recursive_mutex() : m_semaphore{m_allocator.create_recursive_mutex()}, m_locked{false} {
    configASSERT(m_semaphore);
  }
  recursive_mutex(const recursive_mutex &) = delete;
  recursive_mutex(recursive_mutex &&src) = delete;
  /**
   * @brief Destruct the recursive mutex object and delete the recursive mutex
   * instance if it was created.
   *
   */
  ~recursive_mutex(void) {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  recursive_mutex &operator=(const recursive_mutex &) = delete;
  recursive_mutex &operator=(recursive_mutex &&src) = delete;

  /**
   * @brief Unlock the recursive mutex.
   * @ref https://www.freertos.org/a00123.html
   *
   * @return BaseType_t pdTRUE if the recursive mutex was successfully unlocked,
   */
  BaseType_t unlock() {
    auto rc = xSemaphoreGive(m_semaphore);
    if (rc && m_lock_count > 0) {
      m_lock_count--;
      if (m_lock_count == 0) {
        m_locked = false;
      }
    }
    return rc;
  }
  /**
   * @brief Unlock the recursive mutex from an ISR.
   * @ref https://www.freertos.org/a00124.html
   *
   * @param high_priority_task_woken pdTRUE if the high priority task was woken
   * by the recursive mutex unlock.
   * @return BaseType_t pdTRUE if the recursive mutex was successfully unlocked,
   */
  BaseType_t unlock_isr(BaseType_t &high_priority_task_woken) {
    auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
    if (rc && m_lock_count > 0) {
      m_lock_count--;
      if (m_lock_count == 0) {
        m_locked = false;
      }
    }
    return rc;
  }
  /**
   * @brief Unlock the recursive mutex from an ISR.
   * @ref https://www.freertos.org/a00124.html
   *
   * @return BaseType_t pdTRUE if the recursive mutex was successfully unlocked,
   */
  BaseType_t unlock_isr(void) {
    BaseType_t high_priority_task_woken = pdFALSE;
    auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
    if (rc && m_lock_count > 0) {
      m_lock_count--;
      if (m_lock_count == 0) {
        m_locked = false;
      }
    }
    return rc;
  }
  /**
   * @brief Lock the recursive mutex.
   * @ref https://www.freertos.org/a00122.html
   *
   * @param ticks_to_wait timeout in ticks to wait for the recursive mutex.
   * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
   */
  BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
    auto rc = xSemaphoreTake(m_semaphore, ticks_to_wait);
    if (rc) {
      m_locked = true;
      m_lock_count++;
    }
    return rc;
  }
  /**
   * @brief Lock the recursive mutex from an ISR.
   * @ref https://www.freertos.org/xSemaphoreTakeFromISR.html
   *
   * @param high_priority_task_woken pdTRUE if the high priority task was woken
   * by the recursive mutex lock.
   * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
   */
  BaseType_t lock_isr(BaseType_t &high_priority_task_woken) {
    auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
    if (rc) {
      m_locked = true;
      m_lock_count++;
    }
    return rc;
  }
  /**
   * @brief Lock the recursive mutex from an ISR.
   * @ref https://www.freertos.org/xSemaphoreTakeFromISR.html
   *
   * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
   */
  BaseType_t lock_isr(void) {
    BaseType_t high_priority_task_woken = pdFALSE;
    auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
    if (rc) {
      m_locked = true;
      m_lock_count++;
    }
    return rc;
  }
  /**
   * @brief Lock the recursive mutex.
   *
   * @param timeout timeout to wait for the recursive mutex.
   * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
   */
  template <typename Rep, typename Period>
  BaseType_t lock(const std::chrono::duration<Rep, Period> &timeout) {
    return lock(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  /**
   * @brief Try to lock the recursive mutex.
   * @ref https://www.freertos.org/xSemaphoreTake.html
   *
   * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
   */
  BaseType_t try_lock() {
    auto rc = xSemaphoreTake(m_semaphore, 0);
    if (rc) {
      m_locked = true;
      m_lock_count++;
    }
    return rc;
  }
  /**
   * @brief Get the lock status of the recursive mutex.
   *
   * @return bool true if the recursive mutex is locked, otherwise false.
   */
  bool locked(void) const { return m_locked; }
};

/**
 * @brief Lock guard for the mutex. The mutex is locked in the constructor and
 * unlocked in the destructor (RAII).
 *
 * @tparam Mutex type of the mutex to guard.
 */
template <typename Mutex> class lock_guard {
  Mutex &m_mutex; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members): RAII design requires reference

public:
  /**
   * @brief Construct a new lock guard object
   *
   * @param mutex mutex to guard
   */
  explicit lock_guard(Mutex &mutex) : m_mutex{mutex} { m_mutex.lock(); }
  
  /**
   * @brief Destruct the lock guard object and unlock the mutex.
   *
   */
  ~lock_guard(void) { m_mutex.unlock(); }
  
  // Delete copy and move operations for RAII safety
  lock_guard(const lock_guard &) = delete;
  lock_guard(lock_guard &&) = delete;
  lock_guard &operator=(const lock_guard &) = delete;
  lock_guard &operator=(lock_guard &&) = delete;

  /**
   * @brief Checks if the mutex is locked.
   *
   * @return  true if the mutex is locked, otherwise false.
   */
  bool locked(void) const { return m_mutex.locked(); }
};

/**
 * @brief Lock guard for the mutex. The mutex is locked in the constructor and
 * unlocked in the destructor (RAII).
 *
 * @tparam Mutex type of the mutex to guard.
 */
template <typename Mutex> class try_lock_guard {
  Mutex &m_mutex; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members): RAII design requires reference
  bool m_lock_acquired{false};

public:
  /**
   * @brief Construct a new try lock guard object
   *
   * @param mutex mutex to guard
   */
  explicit try_lock_guard(Mutex &mutex) : m_mutex{mutex}, m_lock_acquired{static_cast<bool>(m_mutex.try_lock())} {
  }
  /**
   * @brief Destruct the try lock guard object and unlock the mutex.
   *
   */
  ~try_lock_guard(void) { 
    if (m_lock_acquired) {
      m_mutex.unlock(); 
    }
  }
  
  // Delete copy and move operations for RAII safety
  try_lock_guard(const try_lock_guard &) = delete;
  try_lock_guard(try_lock_guard &&) = delete;
  try_lock_guard &operator=(const try_lock_guard &) = delete;
  try_lock_guard &operator=(try_lock_guard &&) = delete;

  /**
   * @brief Checks if the mutex is locked.
   *
   * @return  true if the mutex is locked, otherwise false.
   */
  bool locked(void) const { return m_lock_acquired && m_mutex.locked(); }
};

/**
 * @brief Lock guard for the mutex. The mutex is locked in the constructor and
 * unlocked in the destructor (RAII).
 *
 * @tparam Mutex type of the mutex to guard.
 */
template <typename Mutex> class lock_guard_isr {
  Mutex &m_mutex; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members): RAII design requires reference
  BaseType_t m_high_priority_task_woken{pdFALSE};

public:
  /**
   * @brief Construct a new lock guard object
   *
   * @param mutex mutex to guard
   */
  explicit lock_guard_isr(Mutex &mutex)
      : m_mutex{mutex} {
    m_mutex.lock_isr(m_high_priority_task_woken);
  }
  /**
   * @brief Destruct the lock guard object and unlock the mutex.
   *
   */
  ~lock_guard_isr(void) { m_mutex.unlock_isr(m_high_priority_task_woken); }
  
  // Delete copy and move operations for RAII safety
  lock_guard_isr(const lock_guard_isr &) = delete;
  lock_guard_isr(lock_guard_isr &&) = delete;
  lock_guard_isr &operator=(const lock_guard_isr &) = delete;
  lock_guard_isr &operator=(lock_guard_isr &&) = delete;

  /**
   * @brief Checks if high priority task was woken.
   *
   * @return BaseType_t pdTRUE if the high priority task was woken, otherwise
   * pdFALSE.
   */
  BaseType_t high_priority_task_woken(void) const {
    return m_high_priority_task_woken;
  }
  /**
   * @brief Checks if the mutex is locked.
   *
   * @return  true if the mutex is locked, otherwise false.
   */
  bool locked(void) const { return m_mutex.locked(); }
};

/**
 * @brief Lock guard for the mutex with timeout. The mutex is locked in the
 * constructor and unlocked in the destructor (RAII).
 *
 * @tparam Mutex type of the mutex to guard.
 */
template <typename Mutex> class timeout_lock_guard {
  Mutex &m_mutex; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members): RAII design requires reference

public:
  /**
   * @brief Construct a new timeout lock guard object
   *
   * @param mutex mutex to guard
   * @param ticks_to_wait timeout in ticks to wait for the mutex.
   */
  timeout_lock_guard(Mutex &mutex, TickType_t ticks_to_wait) : m_mutex{mutex} {
    m_mutex.lock(ticks_to_wait);
  }
  /**
   * @brief Construct a new timeout lock guard object
   *
   * @param mutex mutex to guard
   * @param timeout timeout to wait for the mutex.
   */
  template <typename Rep, typename Period>
  timeout_lock_guard(Mutex &mutex,
                     const std::chrono::duration<Rep, Period> &timeout)
      : m_mutex{mutex} {
    m_mutex.lock(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  /**
   * @brief Destruct the timeout lock guard object and unlock the mutex.
   *
   */
  ~timeout_lock_guard(void) { m_mutex.unlock(); }
  
  // Delete copy and move operations for RAII safety
  timeout_lock_guard(const timeout_lock_guard &) = delete;
  timeout_lock_guard(timeout_lock_guard &&) = delete;
  timeout_lock_guard &operator=(const timeout_lock_guard &) = delete;
  timeout_lock_guard &operator=(timeout_lock_guard &&) = delete;

  /**
   * @brief Checks if the mutex is locked.
   *
   * @return  true if the mutex is locked, otherwise false.
   */
  bool locked(void) const { return m_mutex.locked(); }
};

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief A wrapper for the FreeRTOS binary semaphore with static memory
 * allocation.
 *
 */
namespace sa {
/**
 * @brief Alias for the binary semaphore that uses a static memory allocation.
 *
 */
using binary_semaphore =
    freertos::binary_semaphore<freertos::static_semaphore_allocator>;
/**
 * @brief Alias for the counting semaphore that uses a static memory allocation.
 *
 */
using counting_semaphore =
    freertos::counting_semaphore<freertos::static_semaphore_allocator>;
/**
 * @brief Alias for the mutex that uses a static memory allocation.
 *
 */
using mutex = freertos::mutex<freertos::static_semaphore_allocator>;
/**
 * @brief Alias for the recursive mutex that uses a static memory allocation.
 *
 */
using recursive_mutex =
    freertos::recursive_mutex<freertos::static_semaphore_allocator>;
} // namespace sa
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief A wrapper for the FreeRTOS binary semaphore with dynamic memory
 * allocation.
 *
 */
namespace da {
/**
 * @brief Alias for the binary semaphore that uses a dynamic memory allocation.
 *
 */
using binary_semaphore =
    freertos::binary_semaphore<freertos::dynamic_semaphore_allocator>;
/**
 * @brief Alias for the counting semaphore that uses a dynamic memory
 *
 */
using counting_semaphore =
    freertos::counting_semaphore<freertos::dynamic_semaphore_allocator>;
/**
 * @brief Alias for the mutex that uses a dynamic memory allocation.
 *
 */
using mutex = freertos::mutex<freertos::dynamic_semaphore_allocator>;
/**
 * @brief Alias for the recursive mutex that uses a dynamic memory allocation.
 *
 */
using recursive_mutex =
    freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>;
} // namespace da
#endif

} // namespace freertos
