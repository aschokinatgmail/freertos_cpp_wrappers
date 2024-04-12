/**
 * @file freertos_semaphore.hpp
 * @author Andrey V. Shchekin <aschokin@gmail.com>
 * @brief FreeRTOS semaphore wrapper
 * @version 0.1
 * @date 2024-04-07
 *
 * @copyright Copyright (c) 2024 Andrey V. Shchekin
 *
 */

#pragma once

#include <FreeRTOS.h>
#include <chrono>
#include <semphr.h>
#include <task.h>
#include <time.h>

namespace freertos {

#if configSUPPORT_STATIC_ALLOCATION
class static_semaphore_allocator {
  StaticSemaphore_t m_semaphore_placeholder;

public:
  static_semaphore_allocator() = default;
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

template <typename SemaphoreAllocator> class binary_semaphore {
  SemaphoreAllocator m_allocator;
  SemaphoreHandle_t m_semaphore;

public:
  binary_semaphore() : m_allocator{}, m_semaphore{nullptr} {
    m_semaphore = m_allocator.create_binary();
    configASSERT(m_semaphore);
  }
  binary_semaphore(const binary_semaphore &) = delete;
  binary_semaphore(binary_semaphore &&src) = delete;
  ~binary_semaphore(void) {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  binary_semaphore &operator=(const binary_semaphore &) = delete;
  binary_semaphore &operator=(binary_semaphore &&src) = delete;

  void give() { xSemaphoreGive(m_semaphore); }
  void give_isr(BaseType_t &high_priority_task_woken) {
    xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
  }
  BaseType_t take(const TickType_t ticks_to_wait = portMAX_DELAY) {
    return xSemaphoreTake(m_semaphore, ticks_to_wait);
  }
  BaseType_t take_isr(BaseType_t &high_priority_task_woken) {
    return xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
  }
  template <typename Rep, typename Period>
  BaseType_t take(const std::chrono::duration<Rep, Period> &timeout) {
    return take(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
};

template <typename SemaphoreAllocator> class counting_semaphore {
  SemaphoreAllocator m_allocator;
  SemaphoreHandle_t m_semaphore;

public:
  explicit counting_semaphore(UBaseType_t max_count = 1)
      : m_allocator{}, m_semaphore{nullptr} {
    m_semaphore = m_allocator.create_counting(max_count, max_count);
    configASSERT(m_semaphore);
  }
  counting_semaphore(const counting_semaphore &) = delete;
  counting_semaphore(counting_semaphore &&src) = delete;
  ~counting_semaphore(void) {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  counting_semaphore &operator=(const counting_semaphore &) = delete;
  counting_semaphore &operator=(counting_semaphore &&src) = delete;

  void give() { xSemaphoreGive(m_semaphore); }
  void give_isr(BaseType_t &high_priority_task_woken) {
    xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
  }
  BaseType_t take(const TickType_t ticks_to_wait = portMAX_DELAY) {
    return xSemaphoreTake(m_semaphore, ticks_to_wait);
  }
  BaseType_t take_isr(BaseType_t &high_priority_task_woken) {
    return xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
  }
  template <typename Rep, typename Period>
  BaseType_t take(const std::chrono::duration<Rep, Period> &timeout) {
    return take(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  counting_semaphore &operator++() {
    give();
    return *this;
  }
  counting_semaphore &operator--() {
    take();
    return *this;
  }
  counting_semaphore &operator++(int) {
    give();
    return *this;
  }
  counting_semaphore &operator--(int) {
    take();
    return *this;
  }
  UBaseType_t count() { return uxSemaphoreGetCount(m_semaphore); }
  counting_semaphore &operator+=(UBaseType_t count) {
    while (count--) {
      give();
    }
    return *this;
  }
};

template <typename SemaphoreAllocator> class mutex {
  SemaphoreAllocator m_allocator;
  SemaphoreHandle_t m_semaphore;
  uint8_t m_locked : 1;

public:
  mutex() : m_allocator{}, m_semaphore{nullptr}, m_locked{false} {
    m_semaphore = m_allocator.create_mutex();
    configASSERT(m_semaphore);
  }
  mutex(const mutex &) = delete;
  mutex(mutex &&src) = delete;
  ~mutex(void) {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  mutex &operator=(const mutex &) = delete;
  mutex &operator=(mutex &&src) = delete;

  void unlock() {
    xSemaphoreGive(m_semaphore);
    m_locked = false;
  }
  void unlock_isr(BaseType_t &high_priority_task_woken) {
    xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
    m_locked = false;
  }
  BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
    auto rc = xSemaphoreTake(m_semaphore, ticks_to_wait);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }
  BaseType_t lock_isr(BaseType_t &high_priority_task_woken) {
    auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }
  template <typename Rep, typename Period>
  BaseType_t lock(const std::chrono::duration<Rep, Period> &timeout) {
    return lock(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  BaseType_t try_lock() {
    auto rc = xSemaphoreTake(m_semaphore, 0);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }
  BaseType_t locked(void) const { return m_locked; }
};

template <typename SemaphoreAllocator> class recursive_mutex {
  SemaphoreAllocator m_allocator;
  SemaphoreHandle_t m_semaphore;
  uint8_t m_locked : 1;

public:
  recursive_mutex() : m_allocator{}, m_semaphore{nullptr}, m_locked{false} {
    m_semaphore = m_allocator.create_recursive_mutex();
    configASSERT(m_semaphore);
  }
  recursive_mutex(const recursive_mutex &) = delete;
  recursive_mutex(recursive_mutex &&src) = delete;
  ~recursive_mutex(void) {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  recursive_mutex &operator=(const recursive_mutex &) = delete;
  recursive_mutex &operator=(recursive_mutex &&src) = delete;

  void unlock() {
    xSemaphoreGive(m_semaphore);
    m_locked = false;
  }
  void unlock_isr(BaseType_t &high_priority_task_woken) {
    xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
    m_locked = false;
  }
  BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
    auto rc = xSemaphoreTake(m_semaphore, ticks_to_wait);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }
  BaseType_t lock_isr(BaseType_t &high_priority_task_woken) {
    auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }
  template <typename Rep, typename Period>
  BaseType_t lock(const std::chrono::duration<Rep, Period> &timeout) {
    return lock(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  BaseType_t try_lock() {
    auto rc = xSemaphoreTake(m_semaphore, 0);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }
  BaseType_t locked(void) const { return m_locked; }
};

template <typename Mutex> class lock_guard {
  Mutex &m_mutex;

public:
  explicit lock_guard(Mutex &mutex) : m_mutex{mutex} { m_mutex.lock(); }
  ~lock_guard(void) { m_mutex.unlock(); }
};

template <typename Mutex> class try_lock_guard {
  Mutex &m_mutex;

public:
  explicit try_lock_guard(Mutex &mutex) : m_mutex{mutex} { m_mutex.try_lock(); }
  ~try_lock_guard(void) { m_mutex.unlock(); }

  bool locked(void) const { return m_mutex.locked(); }
};

template <typename Mutex> class lock_guard_isr {
  Mutex &m_mutex;
  BaseType_t m_high_priority_task_woken;

public:
  explicit lock_guard_isr(Mutex &mutex)
      : m_mutex{mutex}, m_high_priority_task_woken{pdFALSE} {
    m_mutex.lock_isr(m_high_priority_task_woken);
  }
  ~lock_guard_isr(void) { m_mutex.unlock_isr(m_high_priority_task_woken); }

  BaseType_t high_priority_task_woken(void) const {
    return m_high_priority_task_woken;
  }
};

template <typename Mutex> class timeout_lock_guard {
  Mutex &m_mutex;

public:
  timeout_lock_guard(Mutex &mutex, TickType_t ticks_to_wait) : m_mutex{mutex} {
    m_mutex.lock(ticks_to_wait);
  }
  template <typename Rep, typename Period>
  timeout_lock_guard(Mutex &mutex,
                     const std::chrono::duration<Rep, Period> &timeout)
      : m_mutex{mutex} {
    m_mutex.lock(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  ~timeout_lock_guard(void) { m_mutex.unlock(); }

  bool locked(void) const { return m_mutex.locked(); }
};

} // namespace freertos
