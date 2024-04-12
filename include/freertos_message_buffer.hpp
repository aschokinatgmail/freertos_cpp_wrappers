/**********************************************************************************
@file freertos_message_buffer.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS message buffer wrapper
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
#include <message_buffer.h>

namespace freertos {

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief An allocator for the message buffer that uses a static memory
 * allocation.
 *
 */
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
/**
 * @brief An allocator for the message buffer that uses a dynamic memory
 * allocation.
 *
 */
template <size_t MessageBufferSize> class dynamic_message_buffer_allocator {
public:
  MessageBufferHandle_t create() {
    return xMessageBufferCreate(MessageBufferSize);
  }
};
#endif

/**
 * @brief A wrapper for the FreeRTOS message buffer.
 *
 * @tparam MessageBufferSize size of the message buffer
 * @tparam MessageBufferAllocator type of the message buffer allocator
 */
template <size_t MessageBufferSize, typename MessageBufferAllocator>
class message_buffer {
  MessageBufferAllocator m_allocator;
  MessageBufferHandle_t m_message_buffer;

public:
  /**
   * @brief Construct a new message buffer object
   *
   */
  explicit message_buffer() : m_allocator{}, m_message_buffer{nullptr} {
    m_message_buffer = m_allocator.create();
    configASSERT(m_message_buffer);
  }
  message_buffer(const message_buffer &) = delete;
  message_buffer(message_buffer &&src) = delete;
  /**
   * @brief Destruct the message buffer object and delete the message buffer
   * kernel object instance if it was created.
   *
   */
  ~message_buffer(void) {
    if (m_message_buffer) {
      vMessageBufferDelete(m_message_buffer);
    }
  }

  message_buffer &operator=(const message_buffer &) = delete;
  message_buffer &operator=(message_buffer &&src) = delete;

  /**
   * @brief Method sends a discret message to the message buffer.
   * @ref https://www.freertos.org/xMessageBufferSend.html
   *
   * @param pvTxData pointer to the message data
   * @param xDataLengthBytes length of the message data
   * @param xTicksToWait timeout in ticks to wait for the message buffer to
   * become available
   * @return size_t number of bytes sent
   */
  size_t send(const void *pvTxData, size_t xDataLengthBytes,
              TickType_t xTicksToWait) {
    return xMessageBufferSend(m_message_buffer, pvTxData, xDataLengthBytes,
                              xTicksToWait);
  }
  /**
   * @brief Method sends a discret message to the message buffer.
   * @ref https://www.freertos.org/xMessageBufferSend.html
   *
   * @param pvTxData pointer to the message data
   * @param xDataLengthBytes length of the message data
   * @param xTicksToWait timeout in ticks to wait for the message buffer to
   * @return size_t number of bytes sent
   */
  template <typename Rep, typename Period>
  size_t send(const void *pvTxData, size_t xDataLengthBytes,
              const std::chrono::duration<Rep, Period> &xTicksToWait) {
    return send(
        pvTxData, xDataLengthBytes,
        std::chrono::duration_cast<std::chrono::milliseconds>(xTicksToWait)
            .count());
  }
  /**
   * @brief Method receives a discret message from the message buffer.
   * @ref https://www.freertos.org/xMessageBufferReceive.html
   *
   * @param pvRxData pointer to the message data
   * @param xBufferLengthBytes length of the message data buffer
   * @param xTicksToWait timeout in ticks to wait for the message buffer to
   * become available
   * @return size_t number of bytes received
   */
  size_t receive(void *pvRxData, size_t xBufferLengthBytes,
                 TickType_t xTicksToWait) {
    return xMessageBufferReceive(m_message_buffer, pvRxData, xBufferLengthBytes,
                                 xTicksToWait);
  }
  /**
   * @brief Method receives a discret message from the message buffer.
   * @ref https://www.freertos.org/xMessageBufferReceive.html
   *
   * @param pvRxData pointer to the message data
   * @param xBufferLengthBytes length of the message data buffer
   * @param timeout timeout to wait for the message buffer to
   * become available
   * @return size_t number of bytes received
   */
  template <typename Rep, typename Period>
  size_t receive(void *pvRxData, size_t xBufferLengthBytes,
                 const std::chrono::duration<Rep, Period> &timeout) {
    return receive(
        pvRxData, xBufferLengthBytes,
        std::chrono::duration_cast<std::chrono::milliseconds>(xTicksToWait)
            .count());
  }
  /**
   * @brief Method returning the number of bytes available in the buffer.
   * @ref https://www.freertos.org/xMessageBufferSpaceAvailable.html
   *
   * @return size_t the number of bytes available in the buffer
   */
  size_t available(void) const {
    return xMessageBufferSpaceAvailable(m_message_buffer);
  }
  /**
   * @brief Method resetting the message buffer to the cleared state.
   * @ref https://www.freertos.org/xMessageBufferReset.html
   *
   * @return BaseType_t pdPass if the message buffer was reset, pdFAIL otherwise
   */
  BaseType_t reset(void) { return xMessageBufferReset(m_message_buffer); }
  /**
   * @brief Method checking if the message buffer is empty.
   * @ref https://www.freertos.org/xMessageBufferIsEmpty.html
   *
   * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
   */
  BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
  /**
   * @brief Method checking if the message buffer is full.
   * @ref https://www.freertos.org/xMessageBufferIsFull.html
   *
   * @return BaseType_t pdTRUE if the message buffer is full, pdFALSE otherwise
   */
  BaseType_t full(void) { return xMessageBufferIsFull(m_message_buffer); }
};

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief Namespace for the FreeRTOS message buffer with static memory
 * allocation
 *
 */
namespace sa {
/**
 * @brief Alias for the message buffer that uses a static memory allocation.
 *
 * @tparam MessageBufferSize size of the message buffer
 */
template <size_t MessageBufferSize>
using message_buffer = freertos::message_buffer<
    MessageBufferSize,
    freertos::static_message_buffer_allocator<MessageBufferSize>>;
} // namespace sa
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief Namespace for the FreeRTOS message buffer with dynamic memory
 * allocation
 *
 */
namespace da {
/**
 * @brief Alias for the message buffer that uses a dynamic memory allocation.
 *
 * @tparam MessageBufferSize size of the message buffer
 */
template <size_t MessageBufferSize>
using message_buffer = freertos::message_buffer<
    MessageBufferSize,
    freertos::dynamic_message_buffer_allocator<MessageBufferSize>>;
} // namespace da
#endif

} // namespace freertos
