/**********************************************************************************
@file freertos_stream_buffer.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS stream buffer interface wrapper
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

#include "freertos_expected.hpp"
#include "freertos_isr_result.hpp"
#include <FreeRTOS.h>
#include <array>
#include <chrono>
#include <optional>
#include <stream_buffer.h>
#include <type_traits>
#include <utility>

namespace freertos {

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief An allocator for the stream buffer that uses a static memory
 * allocation.
 *
 */
template <size_t StreamBufferSize> class static_stream_buffer_allocator {
  StaticStreamBuffer_t m_stream_buffer_placeholder{};
  std::array<uint8_t, StreamBufferSize> m_storage;

public:
  static_stream_buffer_allocator() = default;
  ~static_stream_buffer_allocator() = default;
  static_stream_buffer_allocator(const static_stream_buffer_allocator &) =
      delete;
  static_stream_buffer_allocator(static_stream_buffer_allocator &&) = delete;

  static_stream_buffer_allocator &
  operator=(const static_stream_buffer_allocator &) = delete;
  static_stream_buffer_allocator &
  operator=(static_stream_buffer_allocator &&) = delete;

  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferCreateStatic(StreamBufferSize, trigger_level_bytes,
                                     m_storage.data(),
                                     &m_stream_buffer_placeholder);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief An allocator for the stream buffer that uses a dynamic memory
 * allocation.
 *
 */
template <size_t StreamBufferSize> class dynamic_stream_buffer_allocator {
public:
  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferCreate(StreamBufferSize, trigger_level_bytes);
  }
};
#endif

/**
 * @brief A wrapper for the FreeRTOS stream buffer.
 *
 * @tparam StreamBufferSize size of the stream buffer
 * @tparam StreamBufferAllocator type of the stream buffer allocator
 */
template <size_t StreamBufferSize, typename StreamBufferAllocator>
class stream_buffer {
  StreamBufferAllocator m_allocator{};
  StreamBufferHandle_t m_stream_buffer{nullptr};

public:
  /**
   * @brief Construct a new stream buffer object
   *
   * @param trigger_level_bytes the number of bytes that must be in the stream
   * buffer before a task that is blocked on a read operation will be unblocked
   */
  explicit stream_buffer(size_t trigger_level_bytes = 1)
      : m_stream_buffer{m_allocator.create(trigger_level_bytes)} {
    configASSERT(m_stream_buffer);
  }
  template <typename... AllocatorArgs,
            typename std::enable_if_t<(sizeof...(AllocatorArgs) > 0), int> = 0>
  explicit stream_buffer(size_t trigger_level_bytes, AllocatorArgs &&...args)
      : m_allocator{std::forward<AllocatorArgs>(args)...},
        m_stream_buffer{m_allocator.create(trigger_level_bytes)} {
    configASSERT(m_stream_buffer);
  }
  stream_buffer(const stream_buffer &) = delete;
  stream_buffer(stream_buffer &&src) noexcept
      : m_stream_buffer(src.m_stream_buffer) {
    src.m_stream_buffer = nullptr;
  }
  /**
   * @brief Destruct the stream buffer object and delete the stream buffer
   * instance if it was created.
   *
   */
  ~stream_buffer(void) {
    if (m_stream_buffer) {
      vStreamBufferDelete(m_stream_buffer);
    }
  }

  stream_buffer &operator=(const stream_buffer &) = delete;
  stream_buffer &operator=(stream_buffer &&src) noexcept {
    if (this != &src) {
      swap(src);
    }
    return *this;
  }

  void swap(stream_buffer &other) noexcept {
    using std::swap;
    swap(m_stream_buffer, other.m_stream_buffer);
  }

  friend void swap(stream_buffer &a, stream_buffer &b) noexcept { a.swap(b); }

  /**
   * @brief Send data to the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferSend.html
   *
   * @param data A pointer to the data to be copied into the stream buffer.
   * @param data_size Maximum number of bytes to copy into the stream buffer.
   * @param timeout Number of ticks to wait for the data to be copied into the
   * stream buffer.
   * @return size_t Number of bytes written to the stream buffer.
   */
  size_t send(const void *data, size_t data_size,
              TickType_t timeout = portMAX_DELAY) {
    return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
  }
  /**
   * @brief Send data to the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferSend.html
   *
   * @param data A pointer to the data to be copied into the stream buffer.
   * @param data_size Maximum number of bytes to copy into the stream buffer.
   * @param timeout Duration to wait for the data to be copied into the stream
   * buffer.
   * @return size_t Number of bytes written to the stream buffer.
   */
  template <typename Rep, typename Period>
  size_t send(const void *data, size_t data_size,
              const std::chrono::duration<Rep, Period> &timeout) {
    return send(
        data, data_size,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
  /**
   * @brief Send data to the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferSend.html
   *
   * @tparam Iterator Const iterator type
   * @param begin Iterator to the beginning of the data
   * @param end Iterator to the end of the data
   * @param timeout Number of ticks to wait for the data to be copied into the
   * stream buffer.
   * @return size_t Number of bytes written to the stream buffer.
   */
  template <typename Iterator>
  size_t send(Iterator begin, Iterator end,
              TickType_t timeout = portMAX_DELAY) {
    return send(&*begin, std::distance(begin, end), timeout);
  }
  /**
   * @brief Send data to the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferSend.html
   *
   * @tparam Iterator Const iterator type
   * @param begin Iterator to the beginning of the data
   * @param end Iterator to the end of the data
   * @param timeout Duration to wait for the data to be copied into the stream
   * buffer.
   * @return size_t Number of bytes written to the stream buffer.
   */
  template <typename Iterator, typename Rep, typename Period>
  size_t send(Iterator begin, Iterator end,
              const std::chrono::duration<Rep, Period> &timeout) {
    return send(&*begin, std::distance(begin, end), timeout);
  }
  /**
   * @brief Send data to the stream buffer from an ISR.
   * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
   *
   * @param data A pointer to the data to be copied into the stream buffer.
   * @param data_size Maximum number of bytes to copy into the stream buffer.
   * @return isr_result<size_t> result containing the number of bytes written
   * and the higher_priority_task_woken flag.
   */
  isr_result<size_t> send_isr(const void *data, size_t data_size) {
    isr_result<size_t> result{0, pdFALSE};
    result.result = xStreamBufferSendFromISR(
        m_stream_buffer, data, data_size, &result.higher_priority_task_woken);
    return result;
  }
  /**
   * @brief Send data to the stream buffer from an ISR.
   * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
   *
   * @tparam Iterator Const iterator type
   * @param begin Iterator to the beginning of the data
   * @param end Iterator to the end of the data
   * @return isr_result<size_t> result containing the number of bytes written
   * and the higher_priority_task_woken flag.
   */
  template <typename Iterator>
  isr_result<size_t> send_isr(Iterator begin, Iterator end) {
    return send_isr(&*begin, std::distance(begin, end));
  }
  /**
   * @brief Receive data from the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferReceive.html
   *
   * @param data A pointer to the buffer into which the received data will be
   * copied.
   * @param data_size Maximum number of bytes to copy into the buffer.
   * @param timeout Number of ticks to wait for the data to be copied into the
   * buffer.
   * @return size_t Number of bytes received.
   */
  size_t receive(void *data, size_t data_size,
                 TickType_t timeout = portMAX_DELAY) {
    return xStreamBufferReceive(m_stream_buffer, data, data_size, timeout);
  }
  /**
   * @brief Receive data from the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferReceive.html
   *
   * @param data A pointer to the buffer into which the received data will be
   * copied.
   * @param data_size Maximum number of bytes to copy into the buffer.
   * @param timeout Duration to wait for the data to be copied into the buffer.
   * @return size_t Number of bytes received.
   */
  template <typename Rep, typename Period>
  size_t receive(void *data, size_t data_size,
                 const std::chrono::duration<Rep, Period> &timeout) {
    return receive(
        data, data_size,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
  /**
   * @brief Receive data from the stream buffer from an ISR.
   * @ref https://www.freertos.org/xStreamBufferReceiveFromISR.html
   *
   * @param data A pointer to the buffer into which the received data will be
   * copied.
   * @param data_size Maximum number of bytes to copy into the buffer.
   * @return isr_result<size_t> result containing the number of bytes received
   * and the higher_priority_task_woken flag.
   */
  isr_result<size_t> receive_isr(void *data, size_t data_size) {
    isr_result<size_t> result{0, pdFALSE};
    result.result = xStreamBufferReceiveFromISR(
        m_stream_buffer, data, data_size, &result.higher_priority_task_woken);
    return result;
  }
  /**
   * @brief Number of bytes available in the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferBytesAvailable.html
   *
   * @return size_t Number of bytes available in the stream buffer.
   */
  size_t available(void) {
    return xStreamBufferBytesAvailable(m_stream_buffer);
  }
  /**
   * @brief Number of bytes free in the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferSpacesAvailable.html
   *
   * @return size_t Number of bytes free in the stream buffer.
   */
  size_t free(void) { return xStreamBufferSpacesAvailable(m_stream_buffer); }
  /**
   * @brief Reset the stream buffer to the cleared state.
   * @ref https://www.freertos.org/xStreamBufferReset.html
   *
   * @return BaseType_t pdPass if the stream buffer was reset, pdFAIL otherwise.
   */
  BaseType_t reset(void) { return xStreamBufferReset(m_stream_buffer); }
  /**
   * @brief Set the trigger level of the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferSetTriggerLevel.html
   *
   * @param trigger_level_bytes The number of bytes that must be in the stream
   * buffer before a task that is blocked on a read operation will be unblocked.
   * @return BaseType_t pdPass if the trigger level was set, pdFAIL otherwise.
   */
  BaseType_t set_trigger_level(size_t trigger_level_bytes) {
    return xStreamBufferSetTriggerLevel(m_stream_buffer, trigger_level_bytes);
  }
  /**
   * @brief Get the handle of the stream buffer.
   *
   * @return StreamBufferHandle_t Handle of the stream buffer.
   */
  StreamBufferHandle_t handle(void) const { return m_stream_buffer; }
  /**
   * @brief Check if the stream buffer is empty.
   * @ref https://www.freertos.org/xStreamBufferIsEmpty.html
   *
   * @return BaseType_t pdTRUE if the stream buffer is empty, pdFALSE otherwise.
   */
  BaseType_t empty(void) { return xStreamBufferIsEmpty(m_stream_buffer); }
  /**
   * @brief Check if the stream buffer is full.
   * @ref https://www.freertos.org/xStreamBufferIsFull.html
   *
   * @return BaseType_t pdTRUE if the stream buffer is full, pdFALSE otherwise.
   */
  BaseType_t full(void) { return xStreamBufferIsFull(m_stream_buffer); }

  [[nodiscard]] expected<size_t, error>
  send_ex(const void *data, size_t data_size,
          TickType_t timeout = portMAX_DELAY) {
    auto rc = send(data, data_size, timeout);
    if (rc > 0) {
      return rc;
    }
    return unexpected<error>(timeout == 0 ? error::would_block
                                          : error::timeout);
  }
  template <typename Rep, typename Period>
  [[nodiscard]] expected<size_t, error>
  send_ex(const void *data, size_t data_size,
          const std::chrono::duration<Rep, Period> &timeout) {
    return send_ex(
        data, data_size,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
  template <typename Iterator>
  [[nodiscard]] expected<size_t, error>
  send_ex(Iterator begin, Iterator end, TickType_t timeout = portMAX_DELAY) {
    return send_ex(&*begin, std::distance(begin, end), timeout);
  }
  template <typename Iterator, typename Rep, typename Period>
  [[nodiscard]] expected<size_t, error>
  send_ex(Iterator begin, Iterator end,
          const std::chrono::duration<Rep, Period> &timeout) {
    return send_ex(&*begin, std::distance(begin, end), timeout);
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
  template <typename Iterator>
  [[nodiscard]] isr_result<expected<size_t, error>> send_ex_isr(Iterator begin,
                                                                Iterator end) {
    return send_ex_isr(&*begin, std::distance(begin, end));
  }
  [[nodiscard]] expected<size_t, error>
  receive_ex(void *data, size_t data_size, TickType_t timeout = portMAX_DELAY) {
    auto rc = receive(data, data_size, timeout);
    if (rc > 0) {
      return rc;
    }
    return unexpected<error>(timeout == 0 ? error::would_block
                                          : error::timeout);
  }
  template <typename Rep, typename Period>
  [[nodiscard]] expected<size_t, error>
  receive_ex(void *data, size_t data_size,
             const std::chrono::duration<Rep, Period> &timeout) {
    return receive_ex(
        data, data_size,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
  [[nodiscard]] isr_result<expected<size_t, error>>
  receive_ex_isr(void *data, size_t data_size) {
    auto result = receive_isr(data, data_size);
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
  [[nodiscard]] expected<void, error>
  set_trigger_level_ex(size_t trigger_level_bytes) {
    auto rc = set_trigger_level(trigger_level_bytes);
    if (rc == pdPASS) {
      return {};
    }
    return unexpected<error>(error::invalid_parameter);
  }
};

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief Namespace for the FreeRTOS stream buffer with static memory allocation
 *
 */
namespace sa {
/**
 * @brief Alias for the stream buffer that uses a static memory allocation.
 *
 * @tparam StreamBufferSize size of the stream buffer
 */
template <size_t StreamBufferSize>
using stream_buffer = freertos::stream_buffer<
    StreamBufferSize,
    freertos::static_stream_buffer_allocator<StreamBufferSize>>;
} // namespace sa
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief Namespace for the FreeRTOS stream buffer with dynamic memory
 * allocation
 *
 */
namespace da {
/**
 * @brief Alias for the stream buffer that uses a dynamic memory allocation.
 *
 * @tparam StreamBufferSize size of the stream buffer
 */
template <size_t StreamBufferSize>
using stream_buffer = freertos::stream_buffer<
    StreamBufferSize,
    freertos::dynamic_stream_buffer_allocator<StreamBufferSize>>;
} // namespace da
#endif

} // namespace freertos
