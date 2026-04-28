/**********************************************************************************
@file freertos_message_buffer.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS message buffer wrapper
@version 3.2.0
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
#include <FreeRTOS.h>
#include <array>
#include <chrono>
#include <message_buffer.h>
#include <type_traits>
#include <utility>

namespace freertos {

#if configUSE_SB_COMPLETED_CALLBACK
/**
 * @brief An allocator for the message buffer that uses a dynamic memory
 * allocation with send/receive completed callbacks.
 *
 */
template <size_t MessageBufferSize>
class dynamic_message_buffer_allocator_with_callback {
  StreamBufferCallbackFunction_t m_send_callback;
  void *m_send_context;
  StreamBufferCallbackFunction_t m_receive_callback;
  void *m_receive_context;

public:
  static constexpr bool is_static = false;

  dynamic_message_buffer_allocator_with_callback(
      StreamBufferCallbackFunction_t send_callback, void *send_context,
      StreamBufferCallbackFunction_t receive_callback, void *receive_context)
      : m_send_callback{send_callback}, m_send_context{send_context},
        m_receive_callback{receive_callback},
        m_receive_context{receive_context} {}

  void swap(dynamic_message_buffer_allocator_with_callback &other) noexcept {
    using std::swap;
    swap(m_send_callback, other.m_send_callback);
    swap(m_send_context, other.m_send_context);
    swap(m_receive_callback, other.m_receive_callback);
    swap(m_receive_context, other.m_receive_context);
  }

  MessageBufferHandle_t create() {
    return xMessageBufferCreateWithCallback(
        MessageBufferSize, m_send_callback, m_send_context,
        m_receive_callback, m_receive_context);
  }
};
#endif

#if configSUPPORT_STATIC_ALLOCATION
#if configUSE_SB_COMPLETED_CALLBACK
/**
 * @brief An allocator for the message buffer that uses a static memory
 * allocation with send/receive completed callbacks.
 *
 */
template <size_t MessageBufferSize>
class static_message_buffer_allocator_with_callback {
  StreamBufferCallbackFunction_t m_send_callback;
  void *m_send_context;
  StreamBufferCallbackFunction_t m_receive_callback;
  void *m_receive_context;
  StaticMessageBuffer_t m_message_buffer_placeholder{};
  std::array<uint8_t, MessageBufferSize> m_storage;

public:
  static constexpr bool is_static = true;

  static_message_buffer_allocator_with_callback(
      StreamBufferCallbackFunction_t send_callback, void *send_context,
      StreamBufferCallbackFunction_t receive_callback, void *receive_context)
      : m_send_callback{send_callback}, m_send_context{send_context},
        m_receive_callback{receive_callback},
        m_receive_context{receive_context} {}
  ~static_message_buffer_allocator_with_callback() = default;
  static_message_buffer_allocator_with_callback(
      const static_message_buffer_allocator_with_callback &) = delete;
  static_message_buffer_allocator_with_callback(
      static_message_buffer_allocator_with_callback &&) = default;

  static_message_buffer_allocator_with_callback &
  operator=(const static_message_buffer_allocator_with_callback &) = delete;
  static_message_buffer_allocator_with_callback &
  operator=(static_message_buffer_allocator_with_callback &&) = delete;

  void swap(static_message_buffer_allocator_with_callback &other) noexcept {
    using std::swap;
    swap(m_send_callback, other.m_send_callback);
    swap(m_send_context, other.m_send_context);
    swap(m_receive_callback, other.m_receive_callback);
    swap(m_receive_context, other.m_receive_context);
    swap(m_message_buffer_placeholder, other.m_message_buffer_placeholder);
    swap(m_storage, other.m_storage);
  }

  MessageBufferHandle_t create() {
    return xMessageBufferCreateStaticWithCallback(
        MessageBufferSize, m_storage.data(), &m_message_buffer_placeholder,
        m_send_callback, m_send_context, m_receive_callback,
        m_receive_context);
  }
};
#endif
/**
 * @brief An allocator for the message buffer that uses a static memory
 * allocation.
 *
 */
template <size_t MessageBufferSize> class static_message_buffer_allocator {
  StaticMessageBuffer_t m_message_buffer_placeholder{};
  std::array<uint8_t, MessageBufferSize> m_storage;

public:
  static constexpr bool is_static = true;

  static_message_buffer_allocator() = default;
  ~static_message_buffer_allocator() = default;
  static_message_buffer_allocator(const static_message_buffer_allocator &) =
      delete;
  static_message_buffer_allocator(static_message_buffer_allocator &&) = default;

  static_message_buffer_allocator &
  operator=(const static_message_buffer_allocator &) = delete;
  static_message_buffer_allocator &
  operator=(static_message_buffer_allocator &&) = delete;

  void swap(static_message_buffer_allocator &other) noexcept {
    using std::swap;
    swap(m_message_buffer_placeholder, other.m_message_buffer_placeholder);
    swap(m_storage, other.m_storage);
  }

  MessageBufferHandle_t create() {
    return xMessageBufferCreateStatic(MessageBufferSize, m_storage.data(),
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
  static constexpr bool is_static = false;

  void swap(dynamic_message_buffer_allocator &other) noexcept { (void)other; }

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
  MessageBufferAllocator m_allocator{};
  MessageBufferHandle_t m_message_buffer{nullptr};

public:
  /**
   * @brief Construct a new message buffer object
   *
   */
  message_buffer() : m_message_buffer{m_allocator.create()} {
    configASSERT(m_message_buffer);
  }
  template <typename... AllocatorArgs,
            typename std::enable_if_t<(sizeof...(AllocatorArgs) > 0), int> = 0>
  explicit message_buffer(AllocatorArgs &&...args)
      : m_allocator{std::forward<AllocatorArgs>(args)...},
        m_message_buffer{m_allocator.create()} {
    configASSERT(m_message_buffer);
  }
  message_buffer(const message_buffer &) = delete;
  message_buffer(message_buffer &&src) noexcept
      : m_allocator(std::move(src.m_allocator)),
        m_message_buffer(src.m_message_buffer) {
    configASSERT(!MessageBufferAllocator::is_static);
    src.m_message_buffer = nullptr;
  }
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
  message_buffer &operator=(message_buffer &&src) noexcept {
    configASSERT(!MessageBufferAllocator::is_static);
    if (this != &src) {
      swap(src);
    }
    return *this;
  }

  void swap(message_buffer &other) noexcept {
    configASSERT(!MessageBufferAllocator::is_static);
    using std::swap;
    m_allocator.swap(other.m_allocator);
    swap(m_message_buffer, other.m_message_buffer);
  }

  friend void swap(message_buffer &a, message_buffer &b) noexcept { a.swap(b); }

  /**
   * @brief Method sends a discret message to the message buffer.
   * @ref https://www.freertos.org/xMessageBufferSend.html
   *
   * @param data pointer to the message data
   * @param data_size length of the message data
   * @param ticks_to_wait timeout in ticks to wait for the message buffer to
   * become available
   * @return size_t number of bytes sent
   */
  size_t send(const void *data, size_t data_size,
              TickType_t ticks_to_wait) {
    return xMessageBufferSend(m_message_buffer, data, data_size,
                              ticks_to_wait);
  }
  /**
   * @brief Method sends a discret message to the message buffer.
   * @ref https://www.freertos.org/xMessageBufferSend.html
   *
   * @param data pointer to the message data
   * @param data_size length of the message data
   * @param ticks_to_wait timeout in ticks to wait for the message buffer to
   * @return size_t number of bytes sent
   */
  template <typename Rep, typename Period>
  size_t send(const void *data, size_t data_size,
              const std::chrono::duration<Rep, Period> &ticks_to_wait) {
    return send(
        data, data_size,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(ticks_to_wait)
                .count()));
  }
  /**
   * @brief Method receives a discret message from the message buffer.
   * @ref https://www.freertos.org/xMessageBufferReceive.html
   *
   * @param data pointer to the message data
   * @param buffer_size length of the message data buffer
   * @param ticks_to_wait timeout in ticks to wait for the message buffer to
   * become available
   * @return size_t number of bytes received
   */
  size_t receive(void *data, size_t buffer_size,
                 TickType_t ticks_to_wait) {
    return xMessageBufferReceive(m_message_buffer, data, buffer_size,
                                 ticks_to_wait);
  }
  /**
   * @brief Method receives a discret message from the message buffer.
   * @ref https://www.freertos.org/xMessageBufferReceive.html
   *
   * @param data pointer to the message data
   * @param buffer_size length of the message data buffer
   * @param timeout timeout to wait for the message buffer to
   * become available
   * @return size_t number of bytes received
   */
  template <typename Rep, typename Period>
  size_t receive(void *data, size_t buffer_size,
                 const std::chrono::duration<Rep, Period> &timeout) {
    return receive(
        data, buffer_size,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
  /**
   * @brief Send a discrete message to the message buffer from an ISR.
   * @ref https://www.freertos.org/xMessageBufferSendFromISR.html
   *
   * @param data A pointer to the message data to be copied into the message
   * buffer.
   * @param data_size The number of bytes to copy into the message buffer.
   * @return isr_result<size_t> result containing the number of bytes sent and
   * the higher_priority_task_woken flag.
   */
  isr_result<size_t> send_isr(const void *data, size_t data_size) {
    isr_result<size_t> result{0, pdFALSE};
    result.result = xMessageBufferSendFromISR(
        m_message_buffer, data, data_size, &result.higher_priority_task_woken);
    return result;
  }
  /**
   * @brief Receive a discrete message from the message buffer from an ISR.
   * @ref https://www.freertos.org/xMessageBufferReceiveFromISR.html
   *
   * @param data A pointer to the buffer into which the received message will be
   * copied.
   * @param buffer_size The size of the buffer pointed to by the data parameter.
   * @return isr_result<size_t> result containing the number of bytes received
   * and the higher_priority_task_woken flag.
   */
  isr_result<size_t> receive_isr(void *data, size_t buffer_size) {
    isr_result<size_t> result{0, pdFALSE};
    result.result =
        xMessageBufferReceiveFromISR(m_message_buffer, data, buffer_size,
                                     &result.higher_priority_task_woken);
    return result;
  }
  /**
   * @brief Method returning the number of bytes available in the buffer.
   * @ref https://www.freertos.org/xMessageBufferSpaceAvailable.html
   *
   * @return size_t the number of bytes available in the buffer
   */
  [[nodiscard]] size_t available(void) const {
    return xMessageBufferSpaceAvailable(m_message_buffer);
  }
  /**
   * @brief Method resetting the message buffer to the cleared state.
   * @ref https://www.freertos.org/xMessageBufferReset.html
   *
   * @return BaseType_t pdPass if the message buffer was reset, pdFAIL otherwise
   */
  [[nodiscard]] BaseType_t reset(void) { return xMessageBufferReset(m_message_buffer); }
  /**
   * @brief Method checking if the message buffer is empty.
   * @ref https://www.freertos.org/xMessageBufferIsEmpty.html
   *
   * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
   */
  [[nodiscard]] BaseType_t empty(void) const { return xMessageBufferIsEmpty(m_message_buffer); }
  /**
   * @brief Method checking if the message buffer is full.
   * @ref https://www.freertos.org/xMessageBufferIsFull.html
   *
   * @return BaseType_t pdTRUE if the message buffer is full, pdFALSE otherwise
   */
  [[nodiscard]] BaseType_t full(void) const { return xMessageBufferIsFull(m_message_buffer); }

  [[nodiscard]] expected<size_t, error> send_ex(const void *data,
                                                size_t data_size,
                                                TickType_t ticks_to_wait) {
    auto rc = send(data, data_size, ticks_to_wait);
    if (rc > 0) {
      return rc;
    }
    return unexpected<error>(ticks_to_wait == 0 ? error::would_block
                                               : error::timeout);
  }
  template <typename Rep, typename Period>
  [[nodiscard]] expected<size_t, error>
  send_ex(const void *data, size_t data_size,
          const std::chrono::duration<Rep, Period> &ticks_to_wait) {
    return send_ex(
        data, data_size,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(ticks_to_wait)
                .count()));
  }
  [[nodiscard]] isr_result<expected<size_t, error>>
  send_ex_isr(const void *data, size_t data_size) {
    auto result = send_isr(data, data_size);
    isr_result<expected<size_t, error>> ret{
        unexpected<error>(error::would_block),
        result.higher_priority_task_woken};
    if (result.result > 0) {
      ret.result = result.result;
    }
    return ret;
  }
  [[nodiscard]] expected<size_t, error> receive_ex(void *data,
                                                   size_t buffer_size,
                                                   TickType_t ticks_to_wait) {
    auto rc = receive(data, buffer_size, ticks_to_wait);
    if (rc > 0) {
      return rc;
    }
    return unexpected<error>(ticks_to_wait == 0 ? error::would_block
                                               : error::timeout);
  }
  template <typename Rep, typename Period>
  [[nodiscard]] expected<size_t, error>
  receive_ex(void *data, size_t buffer_size,
             const std::chrono::duration<Rep, Period> &timeout) {
    return receive_ex(
        data, buffer_size,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
  [[nodiscard]] isr_result<expected<size_t, error>>
  receive_ex_isr(void *data, size_t buffer_size) {
    auto result = receive_isr(data, buffer_size);
    isr_result<expected<size_t, error>> ret{
        unexpected<error>(error::would_block),
        result.higher_priority_task_woken};
    if (result.result > 0) {
      ret.result = result.result;
    }
    return ret;
  }
  [[nodiscard]] expected<void, error> reset_ex() {
    auto rc = reset();
    if (rc == pdPASS) {
      return {};
    }
    return unexpected<error>(error::invalid_handle);
  }
  /**
   * @brief Reset the message buffer from an ISR context.
   * @ref https://www.freertos.org/xMessageBufferResetFromISR.html
   *
   * @return isr_result<bool> result containing true if the message buffer was
   * reset, and the higher_priority_task_woken flag.
   */
  isr_result<bool> reset_isr() {
    // xMessageBufferResetFromISR takes only the buffer handle in production
    // FreeRTOS — it does not have a higher_priority_task_woken out-parameter.
    // The flag is left at pdFALSE in the returned result.
    isr_result<bool> result{false, pdFALSE};
    result.result = xMessageBufferResetFromISR(m_message_buffer) == pdPASS;
    return result;
  }
  [[nodiscard]] isr_result<expected<void, error>> reset_ex_isr() {
    auto result = reset_isr();
    isr_result<expected<void, error>> ret{{}, result.higher_priority_task_woken};
    if (!result.result) {
      ret.result = unexpected<error>(error::invalid_handle);
    }
    return ret;
  }
#if configUSE_STREAM_BUFFERS == 1
  /**
   * @brief Set the notification index for the message buffer.
   * @ref https://www.freertos.org/vStreamBufferSetStreamBufferNotificationIndex.html
   *
   * @param index The notification index to set (0 to
   * configTASK_NOTIFICATION_ARRAY_ENTRIES-1).
   */
  void set_notification_index(uint8_t index) {
    vStreamBufferSetStreamBufferNotificationIndex(m_message_buffer, index);
  }
#endif
#if configSUPPORT_STATIC_ALLOCATION
  /**
   * @brief Retrieve the static buffers used by the message buffer.
   * @ref https://www.freertos.org/xMessageBufferGetStaticBuffers.html
   *
   * @param storage Pointer to receive the storage area pointer.
   * @param static_buffer Pointer to receive the static message buffer pointer.
   * @return true if the message buffer was created statically and the buffers
   * were retrieved, false otherwise.
   */
  [[nodiscard]] bool get_static_buffers(uint8_t **storage,
                                        StaticMessageBuffer_t **static_buffer) {
    return xMessageBufferGetStaticBuffers(m_message_buffer, storage,
                                          static_buffer) == pdPASS;
  }
#endif
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
#if configUSE_SB_COMPLETED_CALLBACK
namespace sa_cb {
template <size_t MessageBufferSize>
using message_buffer = freertos::message_buffer<
    MessageBufferSize,
    freertos::static_message_buffer_allocator_with_callback<MessageBufferSize>>;
} // namespace sa_cb
#endif
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
#if configUSE_SB_COMPLETED_CALLBACK
namespace da_cb {
template <size_t MessageBufferSize>
using message_buffer = freertos::message_buffer<
    MessageBufferSize,
    freertos::dynamic_message_buffer_allocator_with_callback<MessageBufferSize>>;
} // namespace da_cb
#endif
#endif

} // namespace freertos
