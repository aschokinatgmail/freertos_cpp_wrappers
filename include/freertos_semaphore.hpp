/**********************************************************************************
@file freertos_semaphore.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS semaphore wrapper
@version 3.1.0
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

#include "freertos_expected.hpp"
#include "freertos_isr_result.hpp"
#include "freertos_thread_safety.hpp"
#include <FreeRTOS.h>
#include <chrono>
#include <ctime>
#include <semphr.h>
#include <task.h>
#include <type_traits>
#include <utility>

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
  static_semaphore_allocator(static_semaphore_allocator &&) = default;

  static_semaphore_allocator &
  operator=(const static_semaphore_allocator &) = delete;
  static_semaphore_allocator &operator=(static_semaphore_allocator &&) = delete;

  void swap(static_semaphore_allocator &other) noexcept {
    using std::swap;
    swap(m_semaphore_placeholder, other.m_semaphore_placeholder);
  }

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
  void swap(dynamic_semaphore_allocator &other) noexcept { (void)other; }

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
 * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
 *
 * Binary semaphores are used for both mutual exclusion and task
 * synchronization. Unlike mutexes, binary semaphores do not implement priority
 * inheritance.
 *
 * @tparam SemaphoreAllocator Type of allocator (static or dynamic)
 *
 * ## Features:
 * - RAII automatic cleanup
 * - std::chrono timeout support
 * - ISR-safe operations
 * - Exception-safe design
 * - No copy/move to prevent accidental resource duplication
 *
 * ## Usage Examples:
 *
 * ### Basic Signaling:
 * ```cpp
 * freertos::binary_semaphore signal_sem;
 *
 * // Producer task
 * freertos::task<512> producer("Producer", 3, [&]() {
 *     while (true) {
 *         produce_data();
 *         signal_sem.give();  // Signal consumer
 *         vTaskDelay(pdMS_TO_TICKS(1000));
 *     }
 * });
 *
 * // Consumer task
 * freertos::task<512> consumer("Consumer", 2, [&]() {
 *     while (true) {
 *         if (signal_sem.take(std::chrono::seconds(5))) {
 *             consume_data();
 *         } else {
 *             printf("Timeout waiting for signal\\n");
 *         }
 *     }
 * });
 * ```
 *
 * ### ISR Communication:
 * ```cpp
 * freertos::binary_semaphore isr_sem;
 *
 * void timer_interrupt_handler() {
 *     BaseType_t task_woken = pdFALSE;
 *     isr_sem.give_isr(task_woken);
 *     portYIELD_FROM_ISR(task_woken);
 * }
 *
 * freertos::task<1024> handler_task("Handler", 5, [&]() {
 *     while (true) {
 *         if (isr_sem.take()) {
 *             handle_timer_event();
 *         }
 *     }
 * });
 * ```
 *
 * ### Static Allocation:
 * ```cpp
 * freertos::binary_semaphore<freertos::static_semaphore_allocator> static_sem;
 * ```
 */
template <typename SemaphoreAllocator>
class FREERTOS_CAPABILITY("binary_semaphore") binary_semaphore {
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
  template <typename... AllocatorArgs,
            typename std::enable_if_t<(sizeof...(AllocatorArgs) > 0), int> = 0>
  explicit binary_semaphore(AllocatorArgs &&...args)
      : m_allocator{std::forward<AllocatorArgs>(args)...},
        m_semaphore{m_allocator.create_binary()} {
    configASSERT(m_semaphore);
  }
  binary_semaphore(const binary_semaphore &) = delete;
  binary_semaphore(binary_semaphore &&src) noexcept
      : m_allocator(std::move(src.m_allocator)), m_semaphore(src.m_semaphore) {
    src.m_semaphore = nullptr;
  }
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
  binary_semaphore &operator=(binary_semaphore &&src) noexcept {
    if (this != &src) {
      swap(src);
    }
    return *this;
  }

  void swap(binary_semaphore &other) noexcept {
    using std::swap;
    m_allocator.swap(other.m_allocator);
    swap(m_semaphore, other.m_semaphore);
  }

  friend void swap(binary_semaphore &a, binary_semaphore &b) noexcept {
    a.swap(b);
  }

  /**
   * @brief Give the binary semaphore.
   * @ref https://www.freertos.org/a00123.html
   *
   * @return BaseType_t pdTRUE if the semaphore was successfully given,
   * otherwise pdFALSE.
   *
   */
  BaseType_t give() FREERTOS_RELEASE() { return xSemaphoreGive(m_semaphore); }
  /**
   * @brief Give the binary semaphore from an ISR.
   * @ref https://www.freertos.org/a00124.html
   *
   * @return isr_result<BaseType_t> containing the result and whether a
   * higher priority task was woken.
   */
  isr_result<BaseType_t> give_isr(void) {
    isr_result<BaseType_t> result{pdFALSE, pdFALSE};
    result.result =
        xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
    return result;
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
  BaseType_t take(const TickType_t ticks_to_wait = portMAX_DELAY)
      FREERTOS_ACQUIRE() {
    return xSemaphoreTake(m_semaphore, ticks_to_wait);
  }
  /**
   * @brief Take the binary semaphore from an ISR.
   * @ref https://www.freertos.org/xSemaphoreTakeFromISR.html
   *
   * @return isr_result<BaseType_t> containing the result and whether a
   * higher priority task was woken.
   */
  isr_result<BaseType_t> take_isr(void) {
    isr_result<BaseType_t> result{pdFALSE, pdFALSE};
    result.result =
        xSemaphoreTakeFromISR(m_semaphore, &result.higher_priority_task_woken);
    return result;
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
    return take(pdMS_TO_TICKS(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
            .count()));
  }

  [[nodiscard]] expected<void, error> give_ex() {
    auto rc = give();
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(error::semaphore_not_owned);
  }
  [[nodiscard]] isr_result<expected<void, error>> give_ex_isr() {
    auto result = give_isr();
    isr_result<expected<void, error>> ret{
        unexpected<error>(error::semaphore_not_owned),
        result.higher_priority_task_woken};
    if (result.result == pdTRUE) {
      ret.result = {};
    }
    return ret;
  }
  [[nodiscard]] expected<void, error>
  take_ex(const TickType_t ticks_to_wait = portMAX_DELAY) {
    auto rc = take(ticks_to_wait);
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(ticks_to_wait == 0 ? error::would_block
                                                : error::timeout);
  }
  [[nodiscard]] isr_result<expected<void, error>> take_ex_isr() {
    auto result = take_isr();
    isr_result<expected<void, error>> ret{unexpected<error>(error::would_block),
                                          result.higher_priority_task_woken};
    if (result.result == pdTRUE) {
      ret.result = {};
    }
    return ret;
  }
  template <typename Rep, typename Period>
  [[nodiscard]] expected<void, error>
  take_ex(const std::chrono::duration<Rep, Period> &timeout) {
    return take_ex(pdMS_TO_TICKS(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
            .count()));
  }

  void acquire(const TickType_t ticks_to_wait = portMAX_DELAY)
      FREERTOS_ACQUIRE() {
    take(ticks_to_wait);
  }
  template <typename Rep, typename Period>
  void acquire(const std::chrono::duration<Rep, Period> &timeout)
      FREERTOS_ACQUIRE() {
    take(timeout);
  }
  [[nodiscard]] bool try_acquire() FREERTOS_TRY_ACQUIRE(true) {
    return take(0) == pdTRUE;
  }
  void release() FREERTOS_RELEASE() { give(); }
  isr_result<BaseType_t> release_isr() { return give_isr(); }
};

/**
 * @brief A wrapper for the FreeRTOS counting semaphore.
 *
 * @tparam SemaphoreAllocator type of the semaphore allocator to use for memory
 * allocation.
 */
template <typename SemaphoreAllocator>
class FREERTOS_CAPABILITY("counting_semaphore") counting_semaphore {
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
  template <typename... AllocatorArgs,
            typename std::enable_if_t<(sizeof...(AllocatorArgs) > 0), int> = 0>
  explicit counting_semaphore(UBaseType_t max_count, AllocatorArgs &&...args)
      : m_allocator{std::forward<AllocatorArgs>(args)...},
        m_semaphore{m_allocator.create_counting(max_count)} {
    configASSERT(m_semaphore);
  }
  counting_semaphore(const counting_semaphore &) = delete;
  counting_semaphore(counting_semaphore &&src) noexcept
      : m_allocator(std::move(src.m_allocator)),
        m_semaphore(src.m_semaphore) {
    src.m_semaphore = nullptr;
  }
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
  counting_semaphore &operator=(counting_semaphore &&src) noexcept {
    if (this != &src) {
      swap(src);
    }
    return *this;
  }

  void swap(counting_semaphore &other) noexcept {
    using std::swap;
    m_allocator.swap(other.m_allocator);
    swap(m_semaphore, other.m_semaphore);
  }

  friend void swap(counting_semaphore &a, counting_semaphore &b) noexcept {
    a.swap(b);
  }

  /**
   * @brief Give the counting semaphore.
   * @ref https://www.freertos.org/a00123.html
   *
   * @return BaseType_t pdTRUE if the semaphore was successfully given,
   * otherwise pdFALSE.
   *
   */
  BaseType_t give() FREERTOS_RELEASE() { return xSemaphoreGive(m_semaphore); }
  /**
   * @brief Give the counting semaphore from an ISR.
   * @ref https://www.freertos.org/a00124.html
   *
   * @return isr_result<BaseType_t> containing the result and whether a
   * higher priority task was woken.
   */
  isr_result<BaseType_t> give_isr(void) {
    isr_result<BaseType_t> result{pdFALSE, pdFALSE};
    result.result =
        xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
    return result;
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
  BaseType_t take(const TickType_t ticks_to_wait = portMAX_DELAY)
      FREERTOS_ACQUIRE() {
    return xSemaphoreTake(m_semaphore, ticks_to_wait);
  }
  /**
   * @brief Take the counting semaphore from an ISR.
   * @ref https://www.freertos.org/xSemaphoreTakeFromISR.html
   *
   * @return isr_result<BaseType_t> containing the result and whether a
   * higher priority task was woken.
   */
  isr_result<BaseType_t> take_isr(void) {
    isr_result<BaseType_t> result{pdFALSE, pdFALSE};
    result.result =
        xSemaphoreTakeFromISR(m_semaphore, &result.higher_priority_task_woken);
    return result;
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
    return take(pdMS_TO_TICKS(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
            .count()));
  }

  [[nodiscard]] expected<void, error> give_ex() {
    auto rc = give();
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(error::semaphore_not_owned);
  }
  [[nodiscard]] isr_result<expected<void, error>> give_ex_isr() {
    auto result = give_isr();
    isr_result<expected<void, error>> ret{
        unexpected<error>(error::semaphore_not_owned),
        result.higher_priority_task_woken};
    if (result.result == pdTRUE) {
      ret.result = {};
    }
    return ret;
  }
  [[nodiscard]] expected<void, error>
  take_ex(const TickType_t ticks_to_wait = portMAX_DELAY) {
    auto rc = take(ticks_to_wait);
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(ticks_to_wait == 0 ? error::would_block
                                                : error::timeout);
  }
  [[nodiscard]] isr_result<expected<void, error>> take_ex_isr() {
    auto result = take_isr();
    isr_result<expected<void, error>> ret{unexpected<error>(error::would_block),
                                          result.higher_priority_task_woken};
    if (result.result == pdTRUE) {
      ret.result = {};
    }
    return ret;
  }
  template <typename Rep, typename Period>
  [[nodiscard]] expected<void, error>
  take_ex(const std::chrono::duration<Rep, Period> &timeout) {
    return take_ex(pdMS_TO_TICKS(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
            .count()));
  }

  void acquire(const TickType_t ticks_to_wait = portMAX_DELAY)
      FREERTOS_ACQUIRE() {
    take(ticks_to_wait);
  }
  template <typename Rep, typename Period>
  void acquire(const std::chrono::duration<Rep, Period> &timeout)
      FREERTOS_ACQUIRE() {
    take(timeout);
  }
  [[nodiscard]] bool try_acquire() FREERTOS_TRY_ACQUIRE(true) {
    return take(0) == pdTRUE;
  }
  void release() FREERTOS_RELEASE() { give(); }
  isr_result<BaseType_t> release_isr() { return give_isr(); }

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
  // NOLINTNEXTLINE(cert-dcl21-cpp): RAII class, copy is deleted -
  // post-increment returns reference instead of copy
  counting_semaphore &operator++(int) {
    give();
    return *this;
  }
  /**
   * @brief Take the counting semaphore.
   * Note: Post-decrement returns reference instead of copy for RAII safety
   *
   * @return counting_semaphore& reference to the counting semaphore.
   */
  // NOLINTNEXTLINE(cert-dcl21-cpp): RAII class, copy is deleted -
  // post-decrement returns reference instead of copy
  counting_semaphore &operator--(int) {
    take();
    return *this;
  }
  /**
   * @brief Get the count of the counting semaphore.
   *
   * @return UBaseType_t count of the counting semaphore.
   */
  [[nodiscard]] UBaseType_t count() const { return uxSemaphoreGetCount(m_semaphore); }
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
template <typename SemaphoreAllocator>
class FREERTOS_CAPABILITY("mutex") mutex {
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
  template <typename... AllocatorArgs,
            typename std::enable_if_t<(sizeof...(AllocatorArgs) > 0), int> = 0>
  explicit mutex(AllocatorArgs &&...args)
      : m_allocator{std::forward<AllocatorArgs>(args)...},
        m_semaphore{m_allocator.create_mutex()}, m_locked{false} {
    configASSERT(m_semaphore);
  }
  mutex(const mutex &) = delete;
  mutex(mutex &&src) noexcept
      : m_allocator(std::move(src.m_allocator)), m_semaphore(src.m_semaphore),
        m_locked(src.m_locked) {
    configASSERT(!src.m_locked);
    src.m_semaphore = nullptr;
    src.m_locked = false;
  }
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
  mutex &operator=(mutex &&src) noexcept {
    configASSERT(!m_locked);
    configASSERT(!src.m_locked);
    if (this != &src) {
      swap(src);
    }
    return *this;
  }

  void swap(mutex &other) noexcept {
    using std::swap;
    m_allocator.swap(other.m_allocator);
    swap(m_semaphore, other.m_semaphore);
    const auto locked_tmp = static_cast<uint8_t>(m_locked);
    m_locked = other.m_locked ? 1 : 0;
    other.m_locked = locked_tmp ? 1 : 0;
  }

  friend void swap(mutex &a, mutex &b) noexcept { a.swap(b); }

  /**
   * @brief Unlock the mutex.
   * @ref https://www.freertos.org/a00123.html
   *
   * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
   */
  BaseType_t unlock() FREERTOS_RELEASE() {
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
   * WARNING: Mutexes must not be used from ISR context — FreeRTOS mutexes
   * have priority inheritance that is incompatible with ISR. Use
   * binary_semaphore instead.
   *
   * @return isr_result<BaseType_t> containing the result and whether a
   * higher priority task was woken.
   */
  isr_result<BaseType_t> unlock_isr(void) {
    isr_result<BaseType_t> result{pdFALSE, pdFALSE};
    result.result =
        xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
    if (result.result) {
      m_locked = false;
    }
    return result;
  }
  /**
   * @brief Lock the mutex.
   * @ref https://www.freertos.org/a00122.html
   *
   * @param ticks_to_wait timeout in ticks to wait for the mutex.
   * @return BaseType_t pdTRUE if the mutex was successfully locked,
   */
  BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY)
      FREERTOS_ACQUIRE() {
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
   * WARNING: Mutexes must not be used from ISR context — FreeRTOS mutexes
   * have priority inheritance that is incompatible with ISR. Use
   * binary_semaphore instead.
   *
   * @return isr_result<BaseType_t> containing the result and whether a
   * higher priority task was woken.
   */
  isr_result<BaseType_t> lock_isr(void) {
    isr_result<BaseType_t> result{pdFALSE, pdFALSE};
    result.result =
        xSemaphoreTakeFromISR(m_semaphore, &result.higher_priority_task_woken);
    if (result.result) {
      m_locked = true;
    }
    return result;
  }
  /**
   * @brief Lock the mutex.
   *
   * @param timeout timeout to wait for the mutex.
   * @return BaseType_t pdTRUE if the mutex was successfully locked,
   */
  template <typename Rep, typename Period>
  BaseType_t lock(const std::chrono::duration<Rep, Period> &timeout) {
    return lock(pdMS_TO_TICKS(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
            .count()));
  }
  /**
   * @brief Try to lock the mutex.
   * @ref https://www.freertos.org/xSemaphoreTake.html
   *
   * @return BaseType_t pdTRUE if the mutex was successfully locked,
   */
  BaseType_t try_lock() FREERTOS_TRY_ACQUIRE(true) {
    auto rc = xSemaphoreTake(m_semaphore, 0);
    if (rc) {
      m_locked = true;
    }
    return rc;
  }

  [[nodiscard]] expected<void, error>
  lock_ex(const TickType_t ticks_to_wait = portMAX_DELAY) {
    auto rc = lock(ticks_to_wait);
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(ticks_to_wait == 0 ? error::would_block
                                                : error::timeout);
  }
  // WARNING: Mutexes must not be used from ISR context — FreeRTOS mutexes
  // have priority inheritance that is incompatible with ISR. Use
  // binary_semaphore instead.
  [[nodiscard]] isr_result<expected<void, error>> lock_ex_isr() {
    auto result = lock_isr();
    isr_result<expected<void, error>> ret{unexpected<error>(error::would_block),
                                          result.higher_priority_task_woken};
    if (result.result == pdTRUE) {
      ret.result = {};
    }
    return ret;
  }
  [[nodiscard]] expected<void, error> unlock_ex() {
    auto rc = unlock();
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(error::semaphore_not_owned);
  }
  // WARNING: Mutexes must not be used from ISR context — FreeRTOS mutexes
  // have priority inheritance that is incompatible with ISR. Use
  // binary_semaphore instead.
  [[nodiscard]] isr_result<expected<void, error>> unlock_ex_isr() {
    auto result = unlock_isr();
    isr_result<expected<void, error>> ret{
        unexpected<error>(error::semaphore_not_owned),
        result.higher_priority_task_woken};
    if (result.result == pdTRUE) {
      ret.result = {};
    }
    return ret;
  }
  [[nodiscard]] expected<void, error> try_lock_ex() {
    auto rc = try_lock();
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(error::would_block);
  }
  template <typename Rep, typename Period>
  [[nodiscard]] expected<void, error>
  lock_ex(const std::chrono::duration<Rep, Period> &timeout) {
    return lock_ex(pdMS_TO_TICKS(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
            .count()));
  }

  template <typename Rep, typename Period>
  bool try_lock_for(const std::chrono::duration<Rep, Period> &timeout) {
    return lock(timeout) == pdTRUE;
  }
  template <typename Clock, typename Duration>
  bool try_lock_until(const std::chrono::time_point<Clock, Duration> &tp) {
    auto now = Clock::now();
    if (tp <= now) {
      return try_lock();
    }
    return lock(std::chrono::duration_cast<std::chrono::milliseconds>(
               tp - now)) == pdTRUE;
  }

  /**
   * @brief Get the lock status of the mutex.
   *
   * @return bool true if the mutex is locked, otherwise false.
   */
  bool locked(void) const { return m_locked; }

  template <typename Fn>
  auto claim(Fn &&fn) -> decltype(fn()) {
    lock();
    try {
      if constexpr (std::is_void_v<decltype(fn())>) {
        fn();
        unlock();
      } else {
        auto result = fn();
        unlock();
        return result;
      }
    } catch (...) {
      unlock();
      throw;
    }
  }

  template <typename Fn>
  auto with_lock(Fn &&fn) -> decltype(fn()) {
    return claim(std::forward<Fn>(fn));
  }
};

/**
 * @brief A wrapper for the FreeRTOS recursive mutex.
 *
 * @tparam SemaphoreAllocator type of the semaphore allocator to use for memory
 * allocation.
 */
template <typename SemaphoreAllocator>
class FREERTOS_SCOPED_CAPABILITY recursive_mutex {
  SemaphoreAllocator m_allocator{};
  SemaphoreHandle_t m_semaphore{nullptr};
  uint8_t m_recursions_count{0};

public:
  /**
   * @brief Construct a new recursive mutex object
   *
   */
  recursive_mutex() : m_semaphore{m_allocator.create_recursive_mutex()} {
    configASSERT(m_semaphore);
  }
  template <typename... AllocatorArgs,
            typename std::enable_if_t<(sizeof...(AllocatorArgs) > 0), int> = 0>
  explicit recursive_mutex(AllocatorArgs &&...args)
      : m_allocator{std::forward<AllocatorArgs>(args)...},
        m_semaphore{m_allocator.create_recursive_mutex()} {
    configASSERT(m_semaphore);
  }
  recursive_mutex(const recursive_mutex &) = delete;
  recursive_mutex(recursive_mutex &&src) noexcept
      : m_allocator(std::move(src.m_allocator)),
        m_semaphore(src.m_semaphore),
        m_recursions_count(src.m_recursions_count) {
    configASSERT(src.m_recursions_count == 0);
    src.m_semaphore = nullptr;
    src.m_recursions_count = 0;
  }
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
  recursive_mutex &operator=(recursive_mutex &&src) noexcept {
    configASSERT(m_recursions_count == 0);
    configASSERT(src.m_recursions_count == 0);
    if (this != &src) {
      swap(src);
    }
    return *this;
  }

  void swap(recursive_mutex &other) noexcept {
    using std::swap;
    m_allocator.swap(other.m_allocator);
    swap(m_semaphore, other.m_semaphore);
    swap(m_recursions_count, other.m_recursions_count);
  }

  friend void swap(recursive_mutex &a, recursive_mutex &b) noexcept {
    a.swap(b);
  }

  /**
   * @brief Unlock the recursive mutex.
   * @ref https://www.freertos.org/a00123.html
   *
   * @return BaseType_t pdTRUE if the recursive mutex was successfully unlocked,
   * otherwise pdFALSE.
   */
  BaseType_t unlock() FREERTOS_RELEASE() {
    auto rc = xSemaphoreGiveRecursive(m_semaphore);
    if (rc && m_recursions_count > 0) {
      m_recursions_count--;
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
  BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY)
      FREERTOS_ACQUIRE() {
    auto rc = xSemaphoreTakeRecursive(m_semaphore, ticks_to_wait);
    if (rc) {
      m_recursions_count++;
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
  BaseType_t lock(const std::chrono::duration<Rep, Period> &timeout)
      FREERTOS_ACQUIRE() {
    return lock(pdMS_TO_TICKS(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
            .count()));
  }
  /**
   * @brief Try to lock the recursive mutex.
   * @ref https://www.freertos.org/xSemaphoreTake.html
   *
   * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
   */
  BaseType_t try_lock() FREERTOS_TRY_ACQUIRE(true) {
    auto rc = xSemaphoreTakeRecursive(m_semaphore, 0);
    if (rc) {
      m_recursions_count++;
    }
    return rc;
  }

  [[nodiscard]] expected<void, error>
  lock_ex(const TickType_t ticks_to_wait = portMAX_DELAY) {
    auto rc = lock(ticks_to_wait);
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(ticks_to_wait == 0 ? error::would_block
                                                : error::timeout);
  }
  [[nodiscard]] expected<void, error> unlock_ex() {
    auto rc = unlock();
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(error::semaphore_not_owned);
  }
  [[nodiscard]] expected<void, error> try_lock_ex() {
    auto rc = try_lock();
    if (rc == pdTRUE) {
      return {};
    }
    return unexpected<error>(error::would_block);
  }
  template <typename Rep, typename Period>
  [[nodiscard]] expected<void, error>
  lock_ex(const std::chrono::duration<Rep, Period> &timeout) {
    return lock_ex(pdMS_TO_TICKS(
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
            .count()));
  }

  template <typename Rep, typename Period>
  bool try_lock_for(const std::chrono::duration<Rep, Period> &timeout) {
    return lock(timeout) == pdTRUE;
  }
  template <typename Clock, typename Duration>
  bool try_lock_until(const std::chrono::time_point<Clock, Duration> &tp) {
    auto now = Clock::now();
    if (tp <= now) {
      return try_lock();
    }
    return lock(std::chrono::duration_cast<std::chrono::milliseconds>(
               tp - now)) == pdTRUE;
  }

  /**
   * @brief Get the lock status of the recursive mutex.
   *
   * @return bool true if the recursive mutex is locked, otherwise false.
   */
  bool locked(void) const { return m_recursions_count > 0; }
  /**
   * @brief Get the number of recursions of the recursive mutex.
   *
   * @return uint8_t number of recursions of the recursive mutex.
   */
  uint8_t recursions_count(void) const { return m_recursions_count; }

  template <typename Fn>
  auto claim(Fn &&fn) -> decltype(fn()) {
    lock();
    try {
      if constexpr (std::is_void_v<decltype(fn())>) {
        fn();
        unlock();
      } else {
        auto result = fn();
        unlock();
        return result;
      }
    } catch (...) {
      unlock();
      throw;
    }
  }

  template <typename Fn>
  auto with_lock(Fn &&fn) -> decltype(fn()) {
    return claim(std::forward<Fn>(fn));
  }
};

/**
 * @brief Lock guard for the mutex. The mutex is locked in the constructor and
 * unlocked in the destructor (RAII).
 *
 * @tparam Mutex type of the mutex to guard.
 */
template <typename Mutex> class FREERTOS_SCOPED_CAPABILITY lock_guard {
  Mutex &m_mutex; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members):
                  // RAII design requires reference

public:
  /**
   * @brief Construct a new lock guard object
   *
   * @param mutex mutex to guard
   */
  explicit lock_guard(Mutex &mutex) FREERTOS_ACQUIRE("mutex") : m_mutex{mutex} {
    m_mutex.lock();
  }

  /**
   * @brief Destruct the lock guard object and unlock the mutex.
   *
   */
  ~lock_guard(void) FREERTOS_RELEASE() { m_mutex.unlock(); }

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
template <typename Mutex> class FREERTOS_SCOPED_CAPABILITY try_lock_guard {
  Mutex &m_mutex; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members):
                  // RAII design requires reference
  bool m_lock_acquired{false};

public:
  /**
   * @brief Construct a new try lock guard object
   *
   * @param mutex mutex to guard
   */
  explicit try_lock_guard(Mutex &mutex) FREERTOS_TRY_ACQUIRE(true)
      : m_mutex{mutex}, m_lock_acquired{static_cast<bool>(m_mutex.try_lock())} {
  }
  /**
   * @brief Destruct the try lock guard object and unlock the mutex.
   *
   */
  ~try_lock_guard(void) FREERTOS_RELEASE() {
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
template <typename Mutex> class FREERTOS_SCOPED_CAPABILITY lock_guard_isr {
  Mutex &m_mutex; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members):
                  // RAII design requires reference
  BaseType_t m_high_priority_task_woken{pdFALSE};
  bool m_lock_acquired{false};

public:
  /**
   * @brief Construct a new lock guard object
   *
   * @param mutex mutex to guard
   */
  explicit lock_guard_isr(Mutex &mutex) FREERTOS_ACQUIRE("mutex")
      : m_mutex{mutex} {
    auto result = m_mutex.lock_isr();
    m_lock_acquired = static_cast<bool>(result.result);
    m_high_priority_task_woken = result.higher_priority_task_woken;
  }
  /**
   * @brief Destruct the lock guard object and unlock the mutex.
   *
   */
  ~lock_guard_isr(void) FREERTOS_RELEASE() {
    if (m_lock_acquired) {
      auto result = m_mutex.unlock_isr();
      m_high_priority_task_woken = result.higher_priority_task_woken;
    }
  }

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
  bool locked(void) const { return m_lock_acquired && m_mutex.locked(); }
};

/**
 * @brief Lock guard for the mutex with timeout. The mutex is locked in the
 * constructor and unlocked in the destructor (RAII).
 *
 * @tparam Mutex type of the mutex to guard.
 */
template <typename Mutex> class FREERTOS_SCOPED_CAPABILITY timeout_lock_guard {
  Mutex &m_mutex; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members):
                  // RAII design requires reference
  bool m_lock_acquired{false};

public:
  /**
   * @brief Construct a new timeout lock guard object
   *
   * @param mutex mutex to guard
   * @param ticks_to_wait timeout in ticks to wait for the mutex.
   */
  timeout_lock_guard(Mutex &mutex, TickType_t ticks_to_wait)
      FREERTOS_TRY_ACQUIRE(true)
      : m_mutex{mutex},
        m_lock_acquired{static_cast<bool>(m_mutex.lock(ticks_to_wait))} {}
  /**
   * @brief Construct a new timeout lock guard object
   *
   * @param mutex mutex to guard
   * @param timeout timeout to wait for the mutex.
   */
  template <typename Rep, typename Period>
  timeout_lock_guard(Mutex &mutex,
                     const std::chrono::duration<Rep, Period> &timeout)
      FREERTOS_TRY_ACQUIRE(true)
      : m_mutex{mutex},
        m_lock_acquired{static_cast<bool>(m_mutex.lock(pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count())))} {}
  /**
   * @brief Destruct the timeout lock guard object and unlock the mutex.
   *
   */
  ~timeout_lock_guard(void) FREERTOS_RELEASE() {
    if (m_lock_acquired) {
      m_mutex.unlock();
    }
  }

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
  bool locked(void) const { return m_lock_acquired && m_mutex.locked(); }
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
