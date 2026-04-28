/**********************************************************************************
@file freertos_stream_buffer.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS stream buffer interface wrapper
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
#include <cstdint>
#include <iterator>
#include <optional>
#include <stream_buffer.h>
#include <type_traits>
#include <utility>

namespace freertos {

namespace detail {

// Trait detecting iterators that are guaranteed to point at contiguous
// storage. `&*begin + N` is only well-defined when the iterator is truly
// contiguous; random-access alone is not sufficient (e.g. std::deque is
// random-access but its elements live in non-contiguous chunks, so taking a
// pointer and offsetting it is undefined behavior).
//
// **C++20**: std::contiguous_iterator is the canonical concept and is used
// directly when the concepts library is available. This correctly accepts
// pointers, vector/array/string iterators, span::iterator, etc., and rejects
// deque, list, map, set, etc.
//
// **C++17**: there is no concept-based way to detect contiguity from just
// the iterator type. To avoid pulling heavy standard-library headers
// (<vector>, <string>, etc.) into a public header — the library is intended
// for embedded targets where dynamic memory is constrained — the C++17
// fallback recognizes ONLY raw pointers as contiguous. Users with
// `std::vector<uint8_t>` or `std::array<uint8_t, N>` should pass
// `(container.data(), container.size())` to the byte-pointer overload of
// `send()` / `receive()` rather than the iterator overload.
//
// Iterator pairs that don't satisfy `is_contiguous_iterator_v` fall through
// to `send_iter_fallback`, which uses a small chunked stack buffer — slower
// than the contiguous fast-path but correct for any contiguous OR
// non-contiguous range.
//
// The trait lives in `detail` so tests and follow-up wrappers can pin its
// values down with `static_assert` without poking into private members of
// the buffer templates that consume it.
#if defined(__cpp_lib_concepts) && __cpp_lib_concepts >= 201907L
template <typename Iterator>
inline constexpr bool is_contiguous_iterator_v =
    std::contiguous_iterator<Iterator>;
#else
template <typename Iterator>
inline constexpr bool is_contiguous_iterator_v =
    std::is_pointer<Iterator>::value;
#endif

}  // namespace detail

#if configUSE_SB_COMPLETED_CALLBACK
/**
 * @brief An allocator for the stream buffer that uses a dynamic memory
 * allocation with send/receive completed callbacks.
 *
 */
template <size_t StreamBufferSize>
class dynamic_stream_buffer_allocator_with_callback {
  StreamBufferCallbackFunction_t m_send_callback;
  void *m_send_context;
  StreamBufferCallbackFunction_t m_receive_callback;
  void *m_receive_context;

public:
  static constexpr bool is_static = false;

  dynamic_stream_buffer_allocator_with_callback(
      StreamBufferCallbackFunction_t send_callback, void *send_context,
      StreamBufferCallbackFunction_t receive_callback, void *receive_context)
      : m_send_callback{send_callback}, m_send_context{send_context},
        m_receive_callback{receive_callback},
        m_receive_context{receive_context} {}

  void swap(dynamic_stream_buffer_allocator_with_callback &other) noexcept {
    using std::swap;
    swap(m_send_callback, other.m_send_callback);
    swap(m_send_context, other.m_send_context);
    swap(m_receive_callback, other.m_receive_callback);
    swap(m_receive_context, other.m_receive_context);
  }

  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferCreateWithCallback(
        StreamBufferSize, trigger_level_bytes, m_send_callback, m_send_context,
        m_receive_callback, m_receive_context);
  }
};
#endif

#if configSUPPORT_STATIC_ALLOCATION
#if configUSE_SB_COMPLETED_CALLBACK
/**
 * @brief An allocator for the stream buffer that uses a static memory
 * allocation with send/receive completed callbacks.
 *
 */
template <size_t StreamBufferSize>
class static_stream_buffer_allocator_with_callback {
  StreamBufferCallbackFunction_t m_send_callback;
  void *m_send_context;
  StreamBufferCallbackFunction_t m_receive_callback;
  void *m_receive_context;
  StaticStreamBuffer_t m_stream_buffer_placeholder{};
  std::array<uint8_t, StreamBufferSize> m_storage;

public:
  static constexpr bool is_static = true;

  static_stream_buffer_allocator_with_callback(
      StreamBufferCallbackFunction_t send_callback, void *send_context,
      StreamBufferCallbackFunction_t receive_callback, void *receive_context)
      : m_send_callback{send_callback}, m_send_context{send_context},
        m_receive_callback{receive_callback},
        m_receive_context{receive_context} {}
  ~static_stream_buffer_allocator_with_callback() = default;
  static_stream_buffer_allocator_with_callback(
      const static_stream_buffer_allocator_with_callback &) = delete;
  static_stream_buffer_allocator_with_callback(
      static_stream_buffer_allocator_with_callback &&) = default;

  static_stream_buffer_allocator_with_callback &
  operator=(const static_stream_buffer_allocator_with_callback &) = delete;
  static_stream_buffer_allocator_with_callback &
  operator=(static_stream_buffer_allocator_with_callback &&) = delete;

  void swap(static_stream_buffer_allocator_with_callback &other) noexcept {
    using std::swap;
    swap(m_send_callback, other.m_send_callback);
    swap(m_send_context, other.m_send_context);
    swap(m_receive_callback, other.m_receive_callback);
    swap(m_receive_context, other.m_receive_context);
    swap(m_stream_buffer_placeholder, other.m_stream_buffer_placeholder);
    swap(m_storage, other.m_storage);
  }

  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferCreateStaticWithCallback(
        StreamBufferSize, trigger_level_bytes, m_storage.data(),
        &m_stream_buffer_placeholder, m_send_callback, m_send_context,
        m_receive_callback, m_receive_context);
  }
};
#endif
/**
 * @brief An allocator for the stream buffer that uses a static memory
 * allocation.
 *
 */
template <size_t StreamBufferSize> class static_stream_buffer_allocator {
  StaticStreamBuffer_t m_stream_buffer_placeholder{};
  std::array<uint8_t, StreamBufferSize> m_storage;

public:
  static constexpr bool is_static = true;

  static_stream_buffer_allocator() = default;
  ~static_stream_buffer_allocator() = default;
  static_stream_buffer_allocator(const static_stream_buffer_allocator &) =
      delete;
  static_stream_buffer_allocator(static_stream_buffer_allocator &&) = default;

  static_stream_buffer_allocator &
  operator=(const static_stream_buffer_allocator &) = delete;
  static_stream_buffer_allocator &
  operator=(static_stream_buffer_allocator &&) = delete;

  void swap(static_stream_buffer_allocator &other) noexcept {
    using std::swap;
    swap(m_stream_buffer_placeholder, other.m_stream_buffer_placeholder);
    swap(m_storage, other.m_storage);
  }

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
  static constexpr bool is_static = false;

  void swap(dynamic_stream_buffer_allocator &other) noexcept { (void)other; }

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
      : m_allocator(std::move(src.m_allocator)),
        m_stream_buffer(src.m_stream_buffer) {
    configASSERT(!StreamBufferAllocator::is_static);
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
    configASSERT(!StreamBufferAllocator::is_static);
    if (this != &src) {
      swap(src);
    }
    return *this;
  }

  void swap(stream_buffer &other) noexcept {
    configASSERT(!StreamBufferAllocator::is_static);
    using std::swap;
    m_allocator.swap(other.m_allocator);
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
private:
  // Re-export the namespace-scope contiguous-iterator trait so the rest of
  // this class body can keep using the unqualified name. The actual trait
  // definition lives in `freertos::detail` (above) so it is testable from
  // outside without needing access to private members.
  template <typename Iterator>
  static constexpr bool is_contiguous_iterator_v =
      detail::is_contiguous_iterator_v<Iterator>;

  // Fallback path for non-contiguous iterators: copy element-by-element into
  // a small stack buffer and forward to the byte-pointer overload of send().
  template <typename Iterator, typename TimeoutT>
  size_t send_iter_fallback(Iterator begin, Iterator end, TimeoutT timeout) {
    using value_type =
        typename std::iterator_traits<Iterator>::value_type;
    static_assert(std::is_trivially_copyable<value_type>::value,
                  "stream_buffer::send requires trivially copyable elements");
    constexpr size_t kChunkBytes = 64;
    constexpr size_t kElemSize = sizeof(value_type);
    static_assert(kElemSize > 0, "element size must be non-zero");
    constexpr size_t kChunkElems =
        (kChunkBytes / kElemSize) > 0 ? (kChunkBytes / kElemSize) : 1;
    uint8_t buffer[kChunkElems * kElemSize];
    size_t total_bytes_sent = 0;
    while (begin != end) {
      size_t count = 0;
      while (count < kChunkElems && begin != end) {
        value_type tmp = *begin;
        // memcpy to avoid aliasing/alignment surprises with the byte buffer.
        for (size_t i = 0; i < kElemSize; ++i) {
          buffer[count * kElemSize + i] =
              reinterpret_cast<const uint8_t *>(&tmp)[i];
        }
        ++count;
        ++begin;
      }
      size_t bytes = count * kElemSize;
      size_t sent = send(static_cast<const void *>(buffer), bytes, timeout);
      total_bytes_sent += sent;
      if (sent < bytes) {
        // Underlying stream buffer could not accept the full chunk; stop to
        // avoid silently dropping data that would otherwise be queued.
        break;
      }
    }
    return total_bytes_sent;
  }

public:
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
    if constexpr (is_contiguous_iterator_v<Iterator>) {
      using value_type =
          typename std::iterator_traits<Iterator>::value_type;
      return send(&*begin,
                  static_cast<size_t>(std::distance(begin, end)) *
                      sizeof(value_type),
                  timeout);
    } else {
      return send_iter_fallback(begin, end, timeout);
    }
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
    if constexpr (is_contiguous_iterator_v<Iterator>) {
      using value_type =
          typename std::iterator_traits<Iterator>::value_type;
      return send(&*begin,
                  static_cast<size_t>(std::distance(begin, end)) *
                      sizeof(value_type),
                  timeout);
    } else {
      return send_iter_fallback(begin, end, timeout);
    }
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
    static_assert(
        is_contiguous_iterator_v<Iterator>,
        "stream_buffer::send_isr requires a contiguous iterator (raw pointer "
        "or random-access iterator over contiguous storage). Non-contiguous "
        "iterators are not supported in ISR context because a fallback copy "
        "buffer would not be ISR-safe for arbitrary input ranges.");
    using value_type =
        typename std::iterator_traits<Iterator>::value_type;
    return send_isr(&*begin,
                    static_cast<size_t>(std::distance(begin, end)) *
                        sizeof(value_type));
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
  [[nodiscard]] size_t available(void) const {
    return xStreamBufferBytesAvailable(m_stream_buffer);
  }
  /**
   * @brief Number of bytes free in the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferSpacesAvailable.html
   *
   * @return size_t Number of bytes free in the stream buffer.
   */
  [[nodiscard]] size_t free(void) const { return xStreamBufferSpacesAvailable(m_stream_buffer); }
  /**
   * @brief Reset the stream buffer to the cleared state.
   * @ref https://www.freertos.org/xStreamBufferReset.html
   *
   * @return BaseType_t pdPass if the stream buffer was reset, pdFAIL otherwise.
   */
  [[nodiscard]] BaseType_t reset(void) { return xStreamBufferReset(m_stream_buffer); }
  /**
   * @brief Set the trigger level of the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferSetTriggerLevel.html
   *
   * @param trigger_level_bytes The number of bytes that must be in the stream
   * buffer before a task that is blocked on a read operation will be unblocked.
   * @return BaseType_t pdPass if the trigger level was set, pdFAIL otherwise.
   */
  [[nodiscard]] BaseType_t set_trigger_level(size_t trigger_level_bytes) {
    return xStreamBufferSetTriggerLevel(m_stream_buffer, trigger_level_bytes);
  }
  /**
   * @brief Get the handle of the stream buffer.
   *
   * @return StreamBufferHandle_t Handle of the stream buffer.
   */
  [[nodiscard]] StreamBufferHandle_t handle(void) const { return m_stream_buffer; }
  /**
   * @brief Check if the stream buffer is empty.
   * @ref https://www.freertos.org/xStreamBufferIsEmpty.html
   *
   * @return BaseType_t pdTRUE if the stream buffer is empty, pdFALSE otherwise.
   */
  [[nodiscard]] BaseType_t empty(void) const { return xStreamBufferIsEmpty(m_stream_buffer); }
  /**
   * @brief Check if the stream buffer is full.
   * @ref https://www.freertos.org/xStreamBufferIsFull.html
   *
   * @return BaseType_t pdTRUE if the stream buffer is full, pdFALSE otherwise.
   */
  [[nodiscard]] BaseType_t full(void) const { return xStreamBufferIsFull(m_stream_buffer); }

  [[nodiscard]] expected<size_t, error>
  send_ex(const void *data, size_t data_size,
          TickType_t timeout = portMAX_DELAY) {
    auto rc = send(data, data_size, timeout);
    if (rc > 0) {
      return rc;
    }
    if (timeout == 0) {
      // Non-blocking call returned 0 bytes. Distinguish "buffer full at call
      // time" (buffer_full) from a transient inability to make progress
      // (would_block).
      if (xStreamBufferIsFull(m_stream_buffer) == pdTRUE) {
        return unexpected<error>(error::buffer_full);
      }
      return unexpected<error>(error::would_block);
    }
    return unexpected<error>(error::timeout);
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
    if constexpr (is_contiguous_iterator_v<Iterator>) {
      using value_type =
          typename std::iterator_traits<Iterator>::value_type;
      return send_ex(&*begin,
                     static_cast<size_t>(std::distance(begin, end)) *
                         sizeof(value_type),
                     timeout);
    } else {
      auto rc = send(begin, end, timeout);
      if (rc > 0) {
        return rc;
      }
      if (timeout == 0) {
        if (xStreamBufferIsFull(m_stream_buffer) == pdTRUE) {
          return unexpected<error>(error::buffer_full);
        }
        return unexpected<error>(error::would_block);
      }
      return unexpected<error>(error::timeout);
    }
  }
  template <typename Iterator, typename Rep, typename Period>
  [[nodiscard]] expected<size_t, error>
  send_ex(Iterator begin, Iterator end,
          const std::chrono::duration<Rep, Period> &timeout) {
    return send_ex(
        begin, end,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
  [[nodiscard]] isr_result<expected<size_t, error>>
  send_ex_isr(const void *data, size_t data_size) {
    auto result = send_isr(data, data_size);
    // ISR send is implicitly non-blocking. If 0 bytes were transferred and
    // the buffer is full, surface buffer_full; otherwise fall back to
    // would_block. xStreamBufferIsFull is callable from ISR context.
    isr_result<expected<size_t, error>> ret{
        unexpected<error>(xStreamBufferIsFull(m_stream_buffer) == pdTRUE
                              ? error::buffer_full
                              : error::would_block),
        result.higher_priority_task_woken};
    if (result.result > 0) {
      ret.result = result.result;
    }
    return ret;
  }
  template <typename Iterator>
  [[nodiscard]] isr_result<expected<size_t, error>> send_ex_isr(Iterator begin,
                                                                Iterator end) {
    static_assert(
        is_contiguous_iterator_v<Iterator>,
        "stream_buffer::send_ex_isr requires a contiguous iterator (raw "
        "pointer or random-access iterator over contiguous storage). "
        "Non-contiguous iterators are not supported in ISR context.");
    using value_type =
        typename std::iterator_traits<Iterator>::value_type;
    return send_ex_isr(&*begin,
                       static_cast<size_t>(std::distance(begin, end)) *
                           sizeof(value_type));
  }
  [[nodiscard]] expected<size_t, error>
  receive_ex(void *data, size_t data_size, TickType_t timeout = portMAX_DELAY) {
    auto rc = receive(data, data_size, timeout);
    if (rc > 0) {
      return rc;
    }
    if (timeout == 0) {
      // Non-blocking call returned 0 bytes. Distinguish "buffer empty at
      // call time" (buffer_empty) from a transient inability to make
      // progress (would_block).
      if (xStreamBufferIsEmpty(m_stream_buffer) == pdTRUE) {
        return unexpected<error>(error::buffer_empty);
      }
      return unexpected<error>(error::would_block);
    }
    return unexpected<error>(error::timeout);
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
    // ISR receive is implicitly non-blocking. If 0 bytes were transferred
    // and the buffer is empty, surface buffer_empty; otherwise would_block.
    isr_result<expected<size_t, error>> ret{
        unexpected<error>(xStreamBufferIsEmpty(m_stream_buffer) == pdTRUE
                              ? error::buffer_empty
                              : error::would_block),
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
   * @brief Reset the stream buffer from an ISR context.
   * @ref https://www.freertos.org/xStreamBufferResetFromISR.html
   *
   * @return isr_result<bool> result containing true if the stream buffer was
   * reset, and the higher_priority_task_woken flag.
   */
  isr_result<bool> reset_isr() {
    // xStreamBufferResetFromISR signature differs across FreeRTOS versions.
    // V10.6+ takes only the handle; pre-V10.6 takes a higher-priority-task-
    // woken out parameter. Guard the call so the wrapper compiles and behaves
    // correctly on both. The simulation layer also uses the new (1-arg) form.
    isr_result<bool> result{false, pdFALSE};
#if (tskKERNEL_VERSION_MAJOR > 10) ||                                          \
    (tskKERNEL_VERSION_MAJOR == 10 && tskKERNEL_VERSION_MINOR >= 6) ||         \
    defined(FREERTOS_CPP_WRAPPERS_SIMULATION)
    result.result = xStreamBufferResetFromISR(m_stream_buffer) == pdPASS;
#else
    BaseType_t hpw = pdFALSE;
    result.result = xStreamBufferResetFromISR(m_stream_buffer, &hpw) == pdPASS;
    result.higher_priority_task_woken = hpw;
#endif
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
   * @brief Set the notification index for the stream buffer.
   * @ref https://www.freertos.org/vStreamBufferSetStreamBufferNotificationIndex.html
   *
   * @param index The notification index to set (0 to
   * configTASK_NOTIFICATION_ARRAY_ENTRIES-1).
   */
  void set_notification_index(uint8_t index) {
    vStreamBufferSetStreamBufferNotificationIndex(m_stream_buffer, index);
  }
#endif
#if configSUPPORT_STATIC_ALLOCATION
  /**
   * @brief Retrieve the static buffers used by the stream buffer.
   * @ref https://www.freertos.org/xStreamBufferGetStaticBuffers.html
   *
   * @param storage Pointer to receive the storage area pointer.
   * @param static_buffer Pointer to receive the static stream buffer pointer.
   * @return true if the stream buffer was created statically and the buffers
   * were retrieved, false otherwise.
   */
  [[nodiscard]] bool get_static_buffers(uint8_t **storage,
                                        StaticStreamBuffer_t **static_buffer) {
    return xStreamBufferGetStaticBuffers(m_stream_buffer, storage,
                                         static_buffer) == pdPASS;
  }
#endif
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
#if configUSE_SB_COMPLETED_CALLBACK
namespace sa_cb {
template <size_t StreamBufferSize>
using stream_buffer = freertos::stream_buffer<
    StreamBufferSize,
    freertos::static_stream_buffer_allocator_with_callback<StreamBufferSize>>;
} // namespace sa_cb
#endif
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
#if configUSE_SB_COMPLETED_CALLBACK
namespace da_cb {
template <size_t StreamBufferSize>
using stream_buffer = freertos::stream_buffer<
    StreamBufferSize,
    freertos::dynamic_stream_buffer_allocator_with_callback<StreamBufferSize>>;
} // namespace da_cb
#endif
#endif

} // namespace freertos
