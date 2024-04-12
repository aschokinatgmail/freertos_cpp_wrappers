/**
 * @file freertos_message_buffer.hpp
 * @author Andrey V. Shchekin <aschokin@gmail.com>
 * @brief  FreeRTOS message buffer wrapper
 * @version 0.1
 * @date 2024-04-07
 *
 * @copyright Copyright (c) 2024 Andrey V. Shchekin
 *
 */

#pragma once

#include <FreeRTOS.h>
#include <chrono>
#include <message_buffer.h>
#include <optional>

namespace freertos {

using std::optional;

#if configSUPPORT_STATIC_ALLOCATION
template <size_t MessageBufferSize> class static_message_buffer_allocator {
  StaticMessageBuffer_t m_message_buffer_placeholder;
  uint8_t m_storage[MessageBufferSize];

public:
  static_message_buffer_allocator() = default;
  static_message_buffer_allocator(const static_message_buffer_allocator &) =
      delete;
  static_message_buffer_allocator(static_message_buffer_allocator &&) = delete;

  static_message_buffer_allocator &
  operator=(const static_message_buffer_allocator &) = delete;
  static_message_buffer_allocator &
  operator=(static_message_buffer_allocator &&) = delete;

  MessageBufferHandle_t create() {
    return xMessageBufferCreateStatic(MessageBufferSize, m_storage,
                                      &m_message_buffer_placeholder);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
template <size_t MessageBufferSize> class dynamic_message_buffer_allocator {
public:
  MessageBufferHandle_t create() {
    return xMessageBufferCreate(MessageBufferSize);
  }
};
#endif

template <size_t MessageBufferSize, typename MessageBufferAllocator>
class message_buffer {
  MessageBufferAllocator m_allocator;
  MessageBufferHandle_t m_message_buffer;

public:
  explicit message_buffer() : m_allocator{}, m_message_buffer{nullptr} {
    m_message_buffer = m_allocator.create();
    configASSERT(m_message_buffer);
  }
  message_buffer(const message_buffer &) = delete;
  message_buffer(message_buffer &&src)
      : m_allocator{src.m_allocator}, m_message_buffer{src.m_message_buffer} {
    src.m_message_buffer = nullptr;
  }
  ~message_buffer(void) {
    if (m_message_buffer) {
      vMessageBufferDelete(m_message_buffer);
    }
  }

  message_buffer &operator=(const message_buffer &) = delete;
  message_buffer &operator=(message_buffer &&src) {
    if (this != &src) {
      if (m_message_buffer) {
        vMessageBufferDelete(m_message_buffer);
      }
      m_allocator = src.m_allocator;
      m_message_buffer = src.m_message_buffer;
      src.m_message_buffer = nullptr;
    }
    return *this;
  }

  BaseType_t send(const void *pvTxData, size_t xDataLengthBytes,
                  TickType_t xTicksToWait) {
    return xMessageBufferSend(m_message_buffer, pvTxData, xDataLengthBytes,
                              xTicksToWait);
  }
  template <typename Rep, typename Period>
  BaseType_t send(const void *pvTxData, size_t xDataLengthBytes,
                  const std::chrono::duration<Rep, Period> &xTicksToWait) {
    return send(
        pvTxData, xDataLengthBytes,
        std::chrono::duration_cast<std::chrono::milliseconds>(xTicksToWait)
            .count());
  }
  size_t receive(void *pvRxData, size_t xBufferLengthBytes,
                 TickType_t xTicksToWait) {
    return xMessageBufferReceive(m_message_buffer, pvRxData, xBufferLengthBytes,
                                 xTicksToWait);
  }
  template <typename Rep, typename Period>
  size_t receive(void *pvRxData, size_t xBufferLengthBytes,
                 const std::chrono::duration<Rep, Period> &xTicksToWait) {
    return receive(
        pvRxData, xBufferLengthBytes,
        std::chrono::duration_cast<std::chrono::milliseconds>(xTicksToWait)
            .count());
  }
  size_t available(void) const {
    return xMessageBufferSpaceAvailable(m_message_buffer);
  }
  BaseType_t reset(void) { return xMessageBufferReset(m_message_buffer); }
  BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
  BaseType_t full(void) { return xMessageBufferIsFull(m_message_buffer); }
};

} // namespace freertos
