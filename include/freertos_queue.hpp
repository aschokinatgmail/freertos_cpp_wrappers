/**
 * @file freertos_queue.hpp
 * @author Andrey V. Shchekin <aschokin@gmail.com>
 * @brief FreeRTOS queue wrapper
 * @version 0.1
 * @date 2024-04-07
 *
 * @copyright Copyright (c) 2024 Andrey V. Shchekin
 *
 */

#pragma once

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#include <FreeRTOS.h>
#include <chrono>
#include <optional>
#include <queue.h>

namespace freertos {

using std::optional;

#if configSUPPORT_STATIC_ALLOCATION
template <size_t QueueLength, typename T> class static_queue_allocator {
  StaticQueue_t m_queue_placeholder;
  uint8_t m_storage[QueueLength * sizeof(T)];

public:
  static_queue_allocator() = default;
  static_queue_allocator(const static_queue_allocator &) = delete;
  static_queue_allocator(static_queue_allocator &&) = delete;

  static_queue_allocator &operator=(const static_queue_allocator &) = delete;
  static_queue_allocator &operator=(static_queue_allocator &&) = delete;

  QueueHandle_t create() {
    return xQueueCreateStatic(QueueLength, sizeof(T), m_storage,
                              &m_queue_placeholder);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
template <size_t QueueLength, typename T> class dynamic_queue_allocator {
public:
  QueueHandle_t create() { return xQueueCreate(QueueLength, sizeof(T)); }
};
#endif

template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
  QueueAllocator m_allocator;
  QueueHandle_t m_queue;

public:
  explicit queue(const char *registred_name = nullptr)
      : m_allocator{}, m_queue{nullptr} {
    m_queue = m_allocator.create();
    configASSERT(m_queue);
    if (registred_name != nullptr) {
      vQueueAddToRegistry(m_queue, registred_name);
    }
  }
  queue(const queue &) = delete;
  queue(queue &&src) : m_allocator{src.m_allocator}, m_queue{src.m_queue} {
    src.m_queue = nullptr;
  }
  ~queue(void) {
    if (m_queue) {
      auto name = pcQueueGetName(m_queue);
      if (name) {
        vQueueUnregisterQueue(m_queue);
      }
      vQueueDelete(m_queue);
    }
  }

  queue &operator=(const queue &) = delete;
  queue &operator=(queue &&src) {
    if (this == &src) {
      return *this;
    }
    if (m_queue) {
      vQueueDelete(m_queue);
    }
    m_allocator = src.m_allocator;
    m_queue = src.m_queue;
    src.m_queue = nullptr;
    return *this;
  }

  BaseType_t send(const T &item, TickType_t ticks_to_wait) {
    return xQueueSend(m_queue, &item, ticks_to_wait);
  }
  template <typename Rep, typename Period>
  BaseType_t send(const T &item,
                  const std::chrono::duration<Rep, Period> &timeout) {
    return send(item, pdMS_TO_TICKS(timeout.count()));
  }
  BaseType_t send_isr(const T &item, BaseType_t &higher_priority_task_woken) {
    return xQueueSendFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  template <typename Rep, typename Period>
  BaseType_t send_isr(const T &item,
                      const std::chrono::duration<Rep, Period> &timeout,
                      BaseType_t &higher_priority_task_woken) {
    return send_isr(item, higher_priority_task_woken);
  }
  BaseType_t send_back(const T &item, TickType_t ticks_to_wait) {
    return xQueueSendToBack(m_queue, &item, ticks_to_wait);
  }
  template <typename Rep, typename Period>
  BaseType_t send_back(const T &item,
                       const std::chrono::duration<Rep, Period> &timeout) {
    return send_back(item, pdMS_TO_TICKS(timeout.count()));
  }
  BaseType_t send_back_isr(const T &item,
                           BaseType_t &higher_priority_task_woken) {
    return xQueueSendToBackFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  template <typename Rep, typename Period>
  BaseType_t send_back_isr(const T &item,
                           const std::chrono::duration<Rep, Period> &timeout,
                           BaseType_t &higher_priority_task_woken) {
    return send_back_isr(item, higher_priority_task_woken);
  }
  BaseType_t send_front(const T &item, TickType_t ticks_to_wait) {
    return xQueueSendToFront(m_queue, &item, ticks_to_wait);
  }
  template <typename Rep, typename Period>
  BaseType_t send_front(const T &item,
                        const std::chrono::duration<Rep, Period> &timeout) {
    return send_front(item, pdMS_TO_TICKS(timeout.count()));
  }
  BaseType_t send_front_isr(const T &item,
                            BaseType_t &higher_priority_task_woken) {
    return xQueueSendToFrontFromISR(m_queue, &item,
                                    &higher_priority_task_woken);
  }
  BaseType_t receive(T &item, TickType_t ticks_to_wait) {
    return xQueueReceive(m_queue, &item, ticks_to_wait);
  }
  optional<T> receive(TickType_t ticks_to_wait) {
    T item;
    if (xQueueReceive(m_queue, &item, ticks_to_wait) == pdPASS) {
      return item;
    }
    return {};
  }
  template <typename Rep, typename Period>
  BaseType_t receive(T &item,
                     const std::chrono::duration<Rep, Period> &timeout) {
    return receive(item, pdMS_TO_TICKS(timeout.count()));
  }
  template <typename Rep, typename Period>
  optional<T> receive(const std::chrono::duration<Rep, Period> &timeout) {
    return receive(pdMS_TO_TICKS(timeout.count()));
  }
  BaseType_t receive_isr(T &item, BaseType_t &higher_priority_task_woken) {
    return xQueueReceiveFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  optional<T> receive_isr(BaseType_t &higher_priority_task_woken) {
    T item;
    if (xQueueReceiveFromISR(m_queue, &item, &higher_priority_task_woken) ==
        pdPASS) {
      return item;
    }
    return {};
  }
  UBaseType_t messages_waiting(void) { return uxQueueMessagesWaiting(m_queue); }
  UBaseType_t messages_waiting_isr(void) {
    return uxQueueMessagesWaitingFromISR(m_queue);
  }
  UBaseType_t spaces_available(void) { return uxQueueSpacesAvailable(m_queue); }
  BaseType_t reset(void) { return xQueueReset(m_queue); }
  BaseType_t overwrite(const T &item) {
    return xQueueOverwrite(m_queue, &item);
  }
  BaseType_t overwrite_isr(const T &item,
                           BaseType_t &higher_priority_task_woken) {
    return xQueueOverwriteFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  BaseType_t peek(T &item, TickType_t ticks_to_wait) {
    return xQueuePeek(m_queue, &item, ticks_to_wait);
  }
  template <typename Rep, typename Period>
  BaseType_t peek(T &item, const std::chrono::duration<Rep, Period> &timeout) {
    return peek(item, pdMS_TO_TICKS(timeout.count()));
  }
  BaseType_t peek_isr(T &item, BaseType_t &higher_priority_task_woken) {
    return xQueuePeekFromISR(m_queue, &item, &higher_priority_task_woken);
  }
  const char *name(void) const { return pcQueueGetName(m_queue); }
  BaseType_t full_isr(void) const { return xQueueIsQueueFullFromISR(m_queue); }
  BaseType_t empty_isr(void) const {
    return xQueueIsQueueEmptyFromISR(m_queue);
  }
};

} // namespace freertos
