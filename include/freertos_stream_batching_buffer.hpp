/**********************************************************************************
@file freertos_stream_batching_buffer.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS stream batching buffer interface wrapper (V11.1.0+)
@version 3.1.0
@date 2026-04-22

The MIT License (MIT)

FreeRTOS C++ Wrappers Library
https://github.com/aschokinatgmail/freertos_cpp_wrappers/

Copyright(c) 2024 Andrey V. Shchekin <aschokin@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
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

#if configUSE_STREAM_BUFFERS == 1

#include "freertos_expected.hpp"
#include "freertos_isr_result.hpp"
#include <FreeRTOS.h>
#include <array>
#include <chrono>
#include <optional>
#include <stream_buffer.h>
#include <type_traits>
#include <utility>

#ifndef sbTYPE_STREAM_BATCHING_BUFFER
#define sbTYPE_STREAM_BATCHING_BUFFER 2
#endif

namespace freertos {

#if configUSE_SB_COMPLETED_CALLBACK
template <size_t StreamBufferSize>
class dynamic_stream_batching_buffer_allocator_with_callback {
  StreamBufferCallbackFunction_t m_send_callback;
  void *m_send_context;
  StreamBufferCallbackFunction_t m_receive_callback;
  void *m_receive_context;

public:
  dynamic_stream_batching_buffer_allocator_with_callback(
      StreamBufferCallbackFunction_t send_callback, void *send_context,
      StreamBufferCallbackFunction_t receive_callback, void *receive_context)
      : m_send_callback{send_callback}, m_send_context{send_context},
        m_receive_callback{receive_callback},
        m_receive_context{receive_context} {}

  void swap(dynamic_stream_batching_buffer_allocator_with_callback &other) noexcept {
    using std::swap;
    swap(m_send_callback, other.m_send_callback);
    swap(m_send_context, other.m_send_context);
    swap(m_receive_callback, other.m_receive_callback);
    swap(m_receive_context, other.m_receive_context);
  }

  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferGenericCreateWithCallback(
        StreamBufferSize, trigger_level_bytes, sbTYPE_STREAM_BATCHING_BUFFER,
        m_send_callback, m_send_context, m_receive_callback,
        m_receive_context);
  }
};
#endif

#if configSUPPORT_STATIC_ALLOCATION
#if configUSE_SB_COMPLETED_CALLBACK
template <size_t StreamBufferSize>
class static_stream_batching_buffer_allocator_with_callback {
  StreamBufferCallbackFunction_t m_send_callback;
  void *m_send_context;
  StreamBufferCallbackFunction_t m_receive_callback;
  void *m_receive_context;
  StaticStreamBuffer_t m_stream_buffer_placeholder{};
  std::array<uint8_t, StreamBufferSize> m_storage;

public:
  static_stream_batching_buffer_allocator_with_callback(
      StreamBufferCallbackFunction_t send_callback, void *send_context,
      StreamBufferCallbackFunction_t receive_callback, void *receive_context)
      : m_send_callback{send_callback}, m_send_context{send_context},
        m_receive_callback{receive_callback},
        m_receive_context{receive_context} {}
  ~static_stream_batching_buffer_allocator_with_callback() = default;
  static_stream_batching_buffer_allocator_with_callback(
      const static_stream_batching_buffer_allocator_with_callback &) = delete;
  static_stream_batching_buffer_allocator_with_callback(
      static_stream_batching_buffer_allocator_with_callback &&) = default;

  static_stream_batching_buffer_allocator_with_callback &
  operator=(const static_stream_batching_buffer_allocator_with_callback &) = delete;
  static_stream_batching_buffer_allocator_with_callback &
  operator=(static_stream_batching_buffer_allocator_with_callback &&) = delete;

  void swap(static_stream_batching_buffer_allocator_with_callback &other) noexcept {
    using std::swap;
    swap(m_send_callback, other.m_send_callback);
    swap(m_send_context, other.m_send_context);
    swap(m_receive_callback, other.m_receive_callback);
    swap(m_receive_context, other.m_receive_context);
    swap(m_stream_buffer_placeholder, other.m_stream_buffer_placeholder);
    swap(m_storage, other.m_storage);
  }

  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferGenericCreateStaticWithCallback(
        StreamBufferSize, trigger_level_bytes, sbTYPE_STREAM_BATCHING_BUFFER,
        m_storage.data(), &m_stream_buffer_placeholder, m_send_callback,
        m_send_context, m_receive_callback, m_receive_context);
  }
};
#endif
template <size_t StreamBufferSize> class static_stream_batching_buffer_allocator {
  StaticStreamBuffer_t m_stream_buffer_placeholder{};
  std::array<uint8_t, StreamBufferSize> m_storage;

public:
  static_stream_batching_buffer_allocator() = default;
  ~static_stream_batching_buffer_allocator() = default;
  static_stream_batching_buffer_allocator(const static_stream_batching_buffer_allocator &) =
      delete;
  static_stream_batching_buffer_allocator(static_stream_batching_buffer_allocator &&) = default;

  static_stream_batching_buffer_allocator &
  operator=(const static_stream_batching_buffer_allocator &) = delete;
  static_stream_batching_buffer_allocator &
  operator=(static_stream_batching_buffer_allocator &&) = delete;

  void swap(static_stream_batching_buffer_allocator &other) noexcept {
    using std::swap;
    swap(m_stream_buffer_placeholder, other.m_stream_buffer_placeholder);
    swap(m_storage, other.m_storage);
  }

  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferGenericCreateStatic(StreamBufferSize, trigger_level_bytes,
                                            sbTYPE_STREAM_BATCHING_BUFFER,
                                            m_storage.data(),
                                            &m_stream_buffer_placeholder);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
template <size_t StreamBufferSize> class dynamic_stream_batching_buffer_allocator {
public:
  void swap(dynamic_stream_batching_buffer_allocator &other) noexcept { (void)other; }

  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferGenericCreate(StreamBufferSize, trigger_level_bytes,
                                      sbTYPE_STREAM_BATCHING_BUFFER);
  }
};
#endif

template <size_t StreamBufferSize, typename StreamBatchingBufferAllocator>
class stream_batching_buffer {
  StreamBatchingBufferAllocator m_allocator{};
  StreamBufferHandle_t m_stream_buffer{nullptr};

public:
  explicit stream_batching_buffer(size_t trigger_level_bytes = 1)
      : m_stream_buffer{m_allocator.create(trigger_level_bytes)} {
    configASSERT(m_stream_buffer);
  }
  template <typename... AllocatorArgs,
            typename std::enable_if_t<(sizeof...(AllocatorArgs) > 0), int> = 0>
  explicit stream_batching_buffer(size_t trigger_level_bytes, AllocatorArgs &&...args)
      : m_allocator{std::forward<AllocatorArgs>(args)...},
        m_stream_buffer{m_allocator.create(trigger_level_bytes)} {
    configASSERT(m_stream_buffer);
  }
  stream_batching_buffer(const stream_batching_buffer &) = delete;
  stream_batching_buffer(stream_batching_buffer &&src) noexcept
      : m_allocator(std::move(src.m_allocator)),
        m_stream_buffer(src.m_stream_buffer) {
    src.m_stream_buffer = nullptr;
  }
  ~stream_batching_buffer(void) {
    if (m_stream_buffer) {
      vStreamBufferDelete(m_stream_buffer);
    }
  }

  stream_batching_buffer &operator=(const stream_batching_buffer &) = delete;
  stream_batching_buffer &operator=(stream_batching_buffer &&src) noexcept {
    if (this != &src) {
      swap(src);
    }
    return *this;
  }

  void swap(stream_batching_buffer &other) noexcept {
    using std::swap;
    m_allocator.swap(other.m_allocator);
    swap(m_stream_buffer, other.m_stream_buffer);
  }

  friend void swap(stream_batching_buffer &a, stream_batching_buffer &b) noexcept { a.swap(b); }

  size_t send(const void *data, size_t data_size,
              TickType_t timeout = portMAX_DELAY) {
    return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
  }
  template <typename Rep, typename Period>
  size_t send(const void *data, size_t data_size,
              const std::chrono::duration<Rep, Period> &timeout) {
    return send(
        data, data_size,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
  template <typename Iterator>
  size_t send(Iterator begin, Iterator end,
              TickType_t timeout = portMAX_DELAY) {
    return send(&*begin, std::distance(begin, end), timeout);
  }
  template <typename Iterator, typename Rep, typename Period>
  size_t send(Iterator begin, Iterator end,
              const std::chrono::duration<Rep, Period> &timeout) {
    return send(&*begin, std::distance(begin, end), timeout);
  }
  isr_result<size_t> send_isr(const void *data, size_t data_size) {
    isr_result<size_t> result{0, pdFALSE};
    result.result = xStreamBufferSendFromISR(
        m_stream_buffer, data, data_size, &result.higher_priority_task_woken);
    return result;
  }
  template <typename Iterator>
  isr_result<size_t> send_isr(Iterator begin, Iterator end) {
    return send_isr(&*begin, std::distance(begin, end));
  }
  size_t receive(void *data, size_t data_size,
                 TickType_t timeout = portMAX_DELAY) {
    return xStreamBufferReceive(m_stream_buffer, data, data_size, timeout);
  }
  template <typename Rep, typename Period>
  size_t receive(void *data, size_t data_size,
                 const std::chrono::duration<Rep, Period> &timeout) {
    return receive(
        data, data_size,
        pdMS_TO_TICKS(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
  isr_result<size_t> receive_isr(void *data, size_t data_size) {
    isr_result<size_t> result{0, pdFALSE};
    result.result = xStreamBufferReceiveFromISR(
        m_stream_buffer, data, data_size, &result.higher_priority_task_woken);
    return result;
  }
  [[nodiscard]] size_t available(void) const {
    return xStreamBufferBytesAvailable(m_stream_buffer);
  }
  [[nodiscard]] size_t free(void) const { return xStreamBufferSpacesAvailable(m_stream_buffer); }
  BaseType_t reset(void) { return xStreamBufferReset(m_stream_buffer); }
  BaseType_t set_trigger_level(size_t trigger_level_bytes) {
    return xStreamBufferSetTriggerLevel(m_stream_buffer, trigger_level_bytes);
  }
  [[nodiscard]] StreamBufferHandle_t handle(void) const { return m_stream_buffer; }
  [[nodiscard]] BaseType_t empty(void) const { return xStreamBufferIsEmpty(m_stream_buffer); }
  [[nodiscard]] BaseType_t full(void) const { return xStreamBufferIsFull(m_stream_buffer); }

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
  isr_result<bool> reset_isr() {
    isr_result<bool> result{false, pdFALSE};
    result.result =
        xStreamBufferResetFromISR(m_stream_buffer,
                                  &result.higher_priority_task_woken) == pdPASS;
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
  void set_notification_index(uint8_t index) {
    vStreamBufferSetStreamBufferNotificationIndex(m_stream_buffer, index);
  }
#endif
#if configSUPPORT_STATIC_ALLOCATION
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
namespace sa {
template <size_t StreamBufferSize>
using stream_batching_buffer = freertos::stream_batching_buffer<
    StreamBufferSize,
    freertos::static_stream_batching_buffer_allocator<StreamBufferSize>>;
} // namespace sa
#if configUSE_SB_COMPLETED_CALLBACK
namespace sa_cb {
template <size_t StreamBufferSize>
using stream_batching_buffer = freertos::stream_batching_buffer<
    StreamBufferSize,
    freertos::static_stream_batching_buffer_allocator_with_callback<StreamBufferSize>>;
} // namespace sa_cb
#endif
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
namespace da {
template <size_t StreamBufferSize>
using stream_batching_buffer = freertos::stream_batching_buffer<
    StreamBufferSize,
    freertos::dynamic_stream_batching_buffer_allocator<StreamBufferSize>>;
} // namespace da
#if configUSE_SB_COMPLETED_CALLBACK
namespace da_cb {
template <size_t StreamBufferSize>
using stream_batching_buffer = freertos::stream_batching_buffer<
    StreamBufferSize,
    freertos::dynamic_stream_batching_buffer_allocator_with_callback<StreamBufferSize>>;
} // namespace da_cb
#endif
#endif

} // namespace freertos

#endif // configUSE_STREAM_BUFFERS == 1