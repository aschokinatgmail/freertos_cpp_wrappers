/**********************************************************************************
@file freertos_queue.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS queue wrapper
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
#include <array>
#include <chrono>
#include <optional>
#include <queue.h>

namespace freertos {

using std::optional;

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief An allocator for the queue that uses a static memory allocation.
 *
 */
template <size_t QueueLength, typename T> class static_queue_allocator {
  StaticQueue_t m_queue_placeholder{};
  std::array<uint8_t, QueueLength * sizeof(T)> m_storage;

public:
  static_queue_allocator() = default;
  ~static_queue_allocator() = default;
  static_queue_allocator(const static_queue_allocator &) = delete;
  static_queue_allocator(static_queue_allocator &&) = delete;

  static_queue_allocator &operator=(const static_queue_allocator &) = delete;
  static_queue_allocator &operator=(static_queue_allocator &&) = delete;

  QueueHandle_t create() {
    return xQueueCreateStatic(QueueLength, sizeof(T), m_storage.data(),
                              &m_queue_placeholder);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief An allocator for the queue that uses a dynamic memory allocation.
 *
 */
template <size_t QueueLength, typename T> class dynamic_queue_allocator {
public:
  QueueHandle_t create() { return xQueueCreate(QueueLength, sizeof(T)); }
};
#endif

/**
 * @brief A modern C++ wrapper for FreeRTOS queues with type safety.
 * 
 * This class provides a type-safe, RAII wrapper around FreeRTOS queues for 
 * inter-task communication. It supports compile-time type checking, automatic
 * resource management, and std::chrono timeout support.
 * 
 * @tparam QueueLength The maximum number of items that the queue can hold
 * @tparam T The type of items stored in the queue (must be copyable)
 * @tparam QueueAllocator The allocator type (static or dynamic)
 * 
 * ## Features:
 * - Type-safe item storage and retrieval
 * - RAII automatic resource management
 * - std::chrono timeout support
 * - ISR-safe operations
 * - Optional queue registry integration
 * - Move semantics prevention for safety
 * 
 * ## Usage Examples:
 * 
 * ### Basic Message Passing:
 * ```cpp
 * // Create a queue for 10 integers
 * freertos::queue<10, int> int_queue;
 * 
 * // Producer task
 * freertos::task<512> producer("Producer", 3, [&]() {
 *     for (int i = 0; i < 20; ++i) {
 *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
 *             printf("Sent: %d\\n", i);
 *         } else {
 *             printf("Failed to send: %d\\n", i);
 *         }
 *         vTaskDelay(pdMS_TO_TICKS(500));
 *     }
 * });
 * 
 * // Consumer task
 * freertos::task<512> consumer("Consumer", 2, [&]() {
 *     while (true) {
 *         int value;
 *         if (int_queue.receive(value, std::chrono::seconds(1))) {
 *             printf("Received: %d\\n", value);
 *         } else {
 *             printf("Timeout - no data\\n");
 *         }
 *     }
 * });
 * ```
 * 
 * ### Complex Data Types:
 * ```cpp
 * struct SensorData {
 *     float temperature;
 *     float humidity;
 *     uint32_t timestamp;
 * };
 * 
 * freertos::queue<5, SensorData> sensor_queue("SensorQueue");
 * 
 * // Send sensor data
 * SensorData data{25.5f, 60.2f, xTaskGetTickCount()};
 * if (sensor_queue.send(data, 100ms)) {
 *     printf("Sensor data sent\\n");
 * }
 * 
 * // Receive sensor data
 * SensorData received;
 * if (sensor_queue.receive(received, 1s)) {
 *     printf("Temperature: %.1f°C, Humidity: %.1f%%\\n", 
 *            received.temperature, received.humidity);
 * }
 * ```
 * 
 * ### ISR Communication:
 * ```cpp
 * freertos::queue<32, uint8_t> isr_queue;
 * 
 * void uart_interrupt_handler() {
 *     uint8_t received_byte = read_uart_register();
 *     BaseType_t task_woken = pdFALSE;
 *     
 *     if (isr_queue.send_isr(received_byte, task_woken)) {
 *         portYIELD_FROM_ISR(task_woken);
 *     }
 * }
 * 
 * freertos::task<1024> uart_handler("UartHandler", 4, [&]() {
 *     while (true) {
 *         uint8_t byte;
 *         if (isr_queue.receive(byte)) {
 *             process_received_byte(byte);
 *         }
 *     }
 * });
 * ```
 * 
 * ### Static Allocation:
 * ```cpp
 * freertos::queue<16, float, freertos::static_queue_allocator<16, float>> static_queue;
 * ```
 */
template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
  QueueAllocator m_allocator{};
  QueueHandle_t m_queue{nullptr};

public:
  /**
   * @brief Construct a new queue object
   *
   * @param registred_name Name of the queue, which will be used to register it
   * in the queue registry. If nullptr, the queue will not be registered.
   */
  explicit queue(const char *registred_name = nullptr)
      : m_queue{m_allocator.create()} {
    configASSERT(m_queue);
    if (registred_name != nullptr) {
      vQueueAddToRegistry(m_queue, registred_name);
    }
  }
  queue(const queue &) = delete;
  queue(queue &&src) = delete;
  ~queue(void) {
    if (m_queue) {
      auto n = pcQueueGetName(m_queue);
      if (n) {
        vQueueUnregisterQueue(m_queue);
      }
      vQueueDelete(m_queue);
    }
  }

  queue &operator=(const queue &) = delete;
  queue &operator=(queue &&src) = delete;

  /**
   * @brief Posts an item to the back of a queue.
   * @ref https://www.freertos.org/a00117.html
   *
   * @param item An item to be posted on the queue.
   * @param ticks_to_wait Timeout in ticks to wait for the space to become
   * available.
   * @return BaseType_t pdPASS if the item was successfully posted, otherwise
   * errQUEUE_FULL.
   */
  BaseType_t send(const T &item, TickType_t ticks_to_wait) {
    return xQueueSend(m_queue, &item, ticks_to_wait);
  }
  /**
   * @brief Posts an item to the back of a queue.
   * @ref https://www.freertos.org/a00117.html
   *
   * @param item An item to be posted on the queue.
   * @param timeout Timeout to wait for the space to become available.
   * @return BaseType_t pdPASS if the item was successfully posted, otherwise
   * errQUEUE_FULL.
   */
  template <typename Rep, typename Period>
  BaseType_t send(const T &item,
                  const std::chrono::duration<Rep, Period> &timeout) {
    return send(item, pdMS_TO_TICKS(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()));
  }
  /**
   * @brief Posts an item to the back of a queue from an ISR.
   * @ref https://www.freertos.org/a00119.html
   *
   * @param item An item to be posted on the queue.
   * @param higher_priority_task_woken A variable to set to pdTRUE if the action
   * unblocked a task of higher priority.
   * @return BaseType_t pdPASS if the item was successfully posted, otherwise
   * errQUEUE_FULL.
   */
  BaseType_t send_isr(const T &item, BaseType_t &higher_priority_task_woken) {
    return xQueueSendFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  /**
   * @brief Posts an item to the back of a queue from an ISR.
   * @ref https://www.freertos.org/a00119.html
   *
   * @param item  An item to be posted on the queue.
   * @return BaseType_t  pdPASS if the item was successfully posted, otherwise
   * errQUEUE_FULL.
   */
  BaseType_t send_isr(const T &item) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    return xQueueSendFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  /**
   * @brief Posts an item to the back of a queue.
   * @ref https://www.freertos.org/xQueueSendToBack.html
   *
   * @param item An item to be posted on the queue.
   * @param ticks_to_wait Timeout in ticks to wait for the space to become
   * available.
   * @return BaseType_t pdPASS if the item
   * was successfully posted, otherwise errQUEUE_FULL.
   */
  BaseType_t send_back(const T &item, TickType_t ticks_to_wait) {
    return xQueueSendToBack(m_queue, &item, ticks_to_wait);
  }
  /**
   * @brief Posts an item to the back of a queue.
   * @ref https://www.freertos.org/xQueueSendToBack.html
   *
   * @param item An item to be posted on the queue.
   * @param timeout Timeout to wait for the space to become available.
   * @return BaseType_t pdPASS if the item was successfully posted, otherwise
   * errQUEUE_FULL.
   */
  template <typename Rep, typename Period>
  BaseType_t send_back(const T &item,
                       const std::chrono::duration<Rep, Period> &timeout) {
    return send_back(item, pdMS_TO_TICKS(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()));
  }
  /**
   * @brief Posts an item to the back of a queue from an ISR.
   * @ref https://www.freertos.org/xQueueSendToBackFromISR.html
   *
   * @param item An item to be posted on the queue.
   * @param higher_priority_task_woken A variable to set to pdTRUE if the action
   * unblocked a task of higher priority.
   * @return BaseType_t pdPASS if the item
   * was successfully posted, otherwise errQUEUE_FULL.
   */
  BaseType_t send_back_isr(const T &item,
                           BaseType_t &higher_priority_task_woken) {
    return xQueueSendToBackFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  /**
   * @brief Posts an item to the back of a queue from an ISR.
   * @ref https://www.freertos.org/xQueueSendToBackFromISR.html
   *
   * @param item An item to be posted on the queue.
   * @return BaseType_t pdPASS if the item was successfully posted, otherwise
   * errQUEUE_FULL.
   */
  BaseType_t send_back_isr(const T &item) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    return xQueueSendToBackFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  /**
   * @brief Posts an item to the front of a queue.
   * @ref https://www.freertos.org/xQueueSendToFront.html
   *
   * @param item An item to be posted on the queue.
   * @param ticks_to_wait Timeout in ticks to wait for the space to become
   * available.
   * @return BaseType_t pdPASS if the item
   * was successfully posted, otherwise errQUEUE_FULL.
   */
  BaseType_t send_front(const T &item, TickType_t ticks_to_wait) {
    return xQueueSendToFront(m_queue, &item, ticks_to_wait);
  }
  /**
   * @brief Posts an item to the front of a queue.
   * @ref https://www.freertos.org/xQueueSendToFront.html
   *
   * @param item An item to be posted on the queue.
   * @param timeout Timeout to wait for the space to become available.
   * @return BaseType_t pdPASS if the item was successfully posted, otherwise
   * errQUEUE_FULL.
   */
  template <typename Rep, typename Period>
  BaseType_t send_front(const T &item,
                        const std::chrono::duration<Rep, Period> &timeout) {
    return send_front(item, pdMS_TO_TICKS(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()));
  }
  /**
   * @brief Posts an item to the front of a queue from an ISR.
   * @ref https://www.freertos.org/xQueueSendToFrontFromISR.html
   *
   * @param item An item to be posted on the queue.
   * @param higher_priority_task_woken A variable to set to pdTRUE if the action
   * unblocked a task of higher priority.
   * @return BaseType_t pdPASS if the item
   * was successfully posted, otherwise errQUEUE_FULL.
   */
  BaseType_t send_front_isr(const T &item,
                            BaseType_t &higher_priority_task_woken) {
    return xQueueSendToFrontFromISR(m_queue, &item,
                                    &higher_priority_task_woken);
  }
  /**
   * @brief Posts an item to the front of a queue from an ISR.
   * @ref https://www.freertos.org/xQueueSendToFrontFromISR.html
   *
   * @param item An item to be posted on the queue.
   * @return BaseType_t pdPASS if the item
   * was successfully posted, otherwise errQUEUE_FULL.
   */
  BaseType_t send_front_isr(const T &item) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    return xQueueSendToFrontFromISR(m_queue, &item,
                                    &higher_priority_task_woken);
  }
  /**
   * @brief Receive an item from a queue.
   * @ref https://www.freertos.org/a00118.html
   *
   * @param item An item placeholder for the item to be received from the queue.
   * @param ticks_to_wait Timeout in ticks to wait for the item to become
   * available.
   * @return BaseType_t pdPASS if the item was successfully received, otherwise
   * pdFALSE.
   */
  BaseType_t receive(T &item, TickType_t ticks_to_wait) {
    return xQueueReceive(m_queue, &item, ticks_to_wait);
  }
  /**
   * @brief Receive an item from a queue.
   * @ref https://www.freertos.org/a00118.html
   *
   * @param timeout Timeout to wait for the item to become available.
   * @return optional<T> The item received from the queue or std::nullopt if the
   * queue is empty.
   */
  optional<T> receive(TickType_t ticks_to_wait) {
    T item;
    if (xQueueReceive(m_queue, &item, ticks_to_wait) == pdPASS) {
      return item;
    }
    return {};
  }
  /**
   * @brief Receive an item from a queue.
   * @ref https://www.freertos.org/a00118.html
   *
   * @param item An item placeholder for the item to be received from the queue.
   * @param timeout Timeout to wait for the item to become available.
   * @return BaseType_t pdPASS if the item
   * was successfully received, otherwise pdFALSE.
   */
  template <typename Rep, typename Period>
  BaseType_t receive(T &item,
                     const std::chrono::duration<Rep, Period> &timeout) {
    return receive(item, pdMS_TO_TICKS(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()));
  }
  /**
   * @brief Receive an item from a queue.
   * @ref https://www.freertos.org/a00118.html
   *
   * @param timeout Timeout to wait for the item to become available.
   * @return optional<T> The item received from the queue or std::nullopt if the
   * queue is empty.
   */
  template <typename Rep, typename Period>
  optional<T> receive(const std::chrono::duration<Rep, Period> &timeout) {
    return receive(pdMS_TO_TICKS(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()));
  }
  /**
   * @brief Receive an item from a queue from an ISR.
   * @ref https://www.freertos.org/a00120.html
   *
   * @param item An item placeholder for the item to be received from the queue.
   * @param higher_priority_task_woken  A variable to set to pdTRUE if the
   * action unblocked a task of higher priority.
   * @return BaseType_t pdPASS if the item was successfully received, otherwise
   * pdFALSE.
   */
  BaseType_t receive_isr(T &item, BaseType_t &higher_priority_task_woken) {
    return xQueueReceiveFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  /**
   * @brief Receive an item from a queue from an ISR.
   * @ref https://www.freertos.org/a00120.html
   *
   * @param item An item placeholder for the item to be received from the queue.
   * @return BaseType_t pdPASS if the item was successfully received, otherwise
   * pdFALSE.
   */
  BaseType_t receive_isr(T &item) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    return xQueueReceiveFromISR(m_queue, &item, &higher_priority_task_woken);
  }

  /**
   * @brief Receive an item from a queue from an ISR.
   * @ref https://www.freertos.org/a00120.html
   *
   * @return optional<T> The item received from the queue or std::nullopt if the
   * queue is empty.
   */
  optional<T> receive_isr(void) {
    T item;
    BaseType_t higher_priority_task_woken = pdFALSE;
    if (xQueueReceiveFromISR(m_queue, &item, &higher_priority_task_woken) ==
        pdPASS) {
      return item;
    }
    return {};
  }
  /**
   * @brief Return the number of items stored in the queue.
   * @ref https://www.freertos.org/a00018.html#uxQueueMessagesWaiting
   *
   * @return UBaseType_t The number of items stored in the queue.
   */
  UBaseType_t messages_waiting(void) { return uxQueueMessagesWaiting(m_queue); }
  /**
   * @brief Return the number of items stored in the queue from an ISR.
   * @ref https://www.freertos.org/a00018.html#uxQueueMessagesWaitingFromISR
   *
   * @return UBaseType_t The number of items stored in the queue.
   */
  UBaseType_t messages_waiting_isr(void) {
    return uxQueueMessagesWaitingFromISR(m_queue);
  }
  /**
   * @brief Return the number of spaces available in the queue.
   * @ref https://www.freertos.org/a00018.html#uxQueueSpacesAvailable
   *
   * @return UBaseType_t The number of spaces available in the queue.
   */
  UBaseType_t spaces_available(void) { return uxQueueSpacesAvailable(m_queue); }
  /**
   * @brief Resets the queue to the empty state.
   * @ref https://www.freertos.org/a00018.html#xQueueReset
   *
   * @return BaseType_t pdPASS if the queue was reset, pdFAIL otherwise.
   */
  BaseType_t reset(void) { return xQueueReset(m_queue); }
  /**
   * @brief A version of send_back method that overwrites the items in the queue
   * if it is full.
   * @ref https://www.freertos.org/xQueueOverwrite.html
   *
   * @param item An item to be posted on the queue.
   * @return BaseType_t pdPASS returned always.
   */
  BaseType_t overwrite(const T &item) {
    return xQueueOverwrite(m_queue, &item);
  }
  /**
   * @brief A version of send_back method that overwrites the items in the queue
   * from an ISR.
   * @ref https://www.freertos.org/xQueueOverwriteFromISR.html
   *
   * @param item An item to be posted on the queue.
   * @param higher_priority_task_woken A variable to set to pdTRUE if the action
   * unblocked a task of higher priority.
   * @return BaseType_t pdPASS returned always.
   */
  BaseType_t overwrite_isr(const T &item,
                           BaseType_t &higher_priority_task_woken) {
    return xQueueOverwriteFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  /**
   * @brief A version of send_back method that overwrites the items in the queue
   * from an ISR.
   * @ref https://www.freertos.org/xQueueOverwriteFromISR.html
   *
   * @param item An item to be posted on the queue.
   * @return BaseType_t pdPASS returned always.
   */
  BaseType_t overwrite_isr(const T &item) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    return xQueueOverwriteFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  /**
   * @brief Peek an item from a queue. The item is not removed from the queue.
   * @ref https://www.freertos.org/xQueuePeek.html
   *
   * @param item An item placeholder for the item to be peeked from the queue.
   * @param ticks_to_wait Timeout in ticks to wait for the item to become
   * available.
   * @return BaseType_t pdPASS if the item was successfully peeked, otherwise
   * pdFALSE.
   */
  BaseType_t peek(T &item, TickType_t ticks_to_wait) {
    return xQueuePeek(m_queue, &item, ticks_to_wait);
  }
  /**
   * @brief Peek an item from a queue. The item is not removed from the queue.
   * @ref https://www.freertos.org/xQueuePeek.html
   *
   * @param item An item placeholder for the item to be peeked from the queue.
   * @param timeout Timeout to wait for the item to become available.
   * @return BaseType_t pdPASS if the item was successfully peeked, otherwise
   * pdFALSE.
   */
  template <typename Rep, typename Period>
  BaseType_t peek(T &item, const std::chrono::duration<Rep, Period> &timeout) {
    return peek(item, pdMS_TO_TICKS(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()));
  }
  /**
   * @brief Peek an item from a queue from an ISR. The item is not removed from
   * the queue.
   * @ref https://www.freertos.org/xQueuePeekFromISR.html
   *
   * @param item An item placeholder for the item to be peeked from the queue.
   * @param higher_priority_task_woken A variable to set to pdTRUE if the action
   * unblocked a task of higher priority.
   * @return BaseType_t pdPASS if the item
   * was successfully peeked, otherwise pdFALSE.
   */
  BaseType_t peek_isr(T &item, BaseType_t &higher_priority_task_woken) {
    return xQueuePeekFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  /**
   * @brief Peek an item from a queue from an ISR. The item is not removed from
   * the queue.
   * @ref https://www.freertos.org/xQueuePeekFromISR.html
   *
   * @param item An item placeholder for the item to be peeked from the queue.
   * @return BaseType_t pdPASS if the item was successfully peeked, otherwise
   * pdFALSE.
   */
  BaseType_t peek_isr(T &item) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    return xQueuePeekFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  /**
   * @brief Peek an item from a queue. The item is not removed from the queue.
   * @ref https://www.freertos.org/xQueuePeek.html
   *
   * @param ticks_to_wait Timeout in ticks to wait for the item to become
   * available.
   * @return optional<T> The item peeked from the queue or std::nullopt if the
   * queue is empty.
   */
  optional<T> peek(TickType_t ticks_to_wait) {
    T item;
    if (xQueuePeek(m_queue, &item, ticks_to_wait) == pdPASS) {
      return item;
    }
    return {};
  }
  /**
   * @brief Peek an item from a queue. The item is not removed from the queue.
   * @ref https://www.freertos.org/xQueuePeek.html
   *
   * @param timeout Timeout to wait for the item to become available.
   * @return optional<T> The item peeked from the queue or std::nullopt if the
   * queue is empty.
   */
  template <typename Rep, typename Period>
  optional<T> peek(const std::chrono::duration<Rep, Period> &timeout) {
    return peek(pdMS_TO_TICKS(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()));
  }

  /**
   * @brief Peek an item from a queue from an ISR. The item is not removed from
   * the queue.
   * @ref https://www.freertos.org/xQueuePeekFromISR.html
   *
   * @return optional<T> The item peeked from the queue or std::nullopt if the
   * queue is empty.
   */
  optional<T> peek_isr(void) {
    T item;
    BaseType_t higher_priority_task_woken = pdFALSE;
    if (xQueuePeekFromISR(m_queue, &item, &higher_priority_task_woken) ==
        pdPASS) {
      return item;
    }
    return {};
  }
  /**
   * @brief Return the name of the queue.
   * @ref https://www.freertos.org/a00018.html#pcQueueGetName
   *
   * @return const char* The name of the queue.
   */
  const char *name(void) const { return pcQueueGetName(m_queue); }
  /**
   * @brief Method checking if the queue is full from an ISR.
   * @ref https://www.freertos.org/a00018.html#xQueueIsQueueFullFromISR
   *
   * @return BaseType_t pdTRUE if the queue is full, pdFALSE otherwise.
   */
  BaseType_t full_isr(void) const { return xQueueIsQueueFullFromISR(m_queue); }
  /**
   * @brief Method checking if the queue is empty from an ISR.
   * @ref https://www.freertos.org/a00018.html#xQueueIsQueueEmptyFromISR
   *
   * @return BaseType_t pdTRUE if the queue is empty, pdFALSE otherwise.
   */
  BaseType_t empty_isr(void) const {
    return xQueueIsQueueEmptyFromISR(m_queue);
  }
};

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief A namespace for the FreeRTOS queue with a static memory allocation.
 *
 */
namespace sa {
/**
 * @brief Alias for the queue that uses a static memory allocation.
 *
 */
template <size_t QueueLength, typename T>
using queue =
    freertos::queue<QueueLength, T, static_queue_allocator<QueueLength, T>>;
} // namespace sa
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief A namespace for the FreeRTOS queue with a dynamic memory allocation.
 *
 */
namespace da {
/**
 * @brief Alias for the queue that uses a dynamic memory allocation.
 *
 */
template <size_t QueueLength, typename T>
using queue =
    freertos::queue<QueueLength, T, dynamic_queue_allocator<QueueLength, T>>;
} // namespace da
#endif

} // namespace freertos
