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

#include <FreeRTOS.h>
#include <chrono>
#include <optional>
#include <stream_buffer.h>

namespace freertos {

#if configSUPPORT_STATIC_ALLOCATION
template <size_t StreamBufferSize> class static_stream_buffer_allocator {
  StaticStreamBuffer_t m_stream_buffer_placeholder;
  uint8_t m_storage[StreamBufferSize];

public:
  static_stream_buffer_allocator() = default;
  static_stream_buffer_allocator(const static_stream_buffer_allocator &) =
      delete;
  static_stream_buffer_allocator(static_stream_buffer_allocator &&) = delete;

  static_stream_buffer_allocator &
  operator=(const static_stream_buffer_allocator &) = delete;
  static_stream_buffer_allocator &
  operator=(static_stream_buffer_allocator &&) = delete;

  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferCreateStatic(StreamBufferSize, trigger_level_bytes,
                                     m_storage, &m_stream_buffer_placeholder);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
template <size_t StreamBufferSize> class dynamic_stream_buffer_allocator {
public:
  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    return xStreamBufferCreate(StreamBufferSize, trigger_level_bytes);
  }
};
#endif

template <size_t StreamBufferSize, typename StreamBufferAllocator>
class stream_buffer {
  StreamBufferAllocator m_allocator;
  StreamBufferHandle_t m_stream_buffer;

public:
  explicit stream_buffer(size_t trigger_level_bytes = 1)
      : m_allocator{}, m_stream_buffer{nullptr} {
    m_stream_buffer = m_allocator.create(trigger_level_bytes);
    configASSERT(m_stream_buffer);
  }
  stream_buffer(const stream_buffer &) = delete;
  stream_buffer(stream_buffer &&src)
      : m_allocator{src.m_allocator}, m_stream_buffer{src.m_stream_buffer} {
    src.m_stream_buffer = nullptr;
  }
  ~stream_buffer(void) {
    if (m_stream_buffer) {
      vStreamBufferDelete(m_stream_buffer);
    }
  }

  stream_buffer &operator=(const stream_buffer &) = delete;
  stream_buffer &operator=(stream_buffer &&src) {
    if (this != &src) {
      if (m_stream_buffer) {
        vStreamBufferDelete(m_stream_buffer);
      }
      m_allocator = src.m_allocator;
      m_stream_buffer = src.m_stream_buffer;
      src.m_stream_buffer = nullptr;
    }
    return *this;
  }

  BaseType_t send(const void *data, size_t data_size,
                  TickType_t timeout = portMAX_DELAY) {
    return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
  }
  template <typename Rep, typename Period>
  BaseType_t send(const void *data, size_t data_size,
                  const std::chrono::duration<Rep, Period> &timeout) {
    return send(
        data, data_size,
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  template <typename Iterator>
  BaseType_t send(Iterator begin, Iterator end,
                  TickType_t timeout = portMAX_DELAY) {
    return send(&*begin, std::distance(begin, end), timeout);
  }
  template <typename Iterator, typename Rep, typename Period>
  BaseType_t send(Iterator begin, Iterator end,
                  const std::chrono::duration<Rep, Period> &timeout) {
    return send(&*begin, std::distance(begin, end), timeout);
  }
  BaseType_t send_isr(const void *data, size_t data_size,
                      BaseType_t &higher_priority_task_woken) {
    return xStreamBufferSendFromISR(m_stream_buffer, data, data_size,
                                    &higher_priority_task_woken);
  }
  template <typename Iterator>
  BaseType_t send_isr(Iterator begin, Iterator end,
                      BaseType_t &higher_priority_task_woken) {
    return send_isr(&*begin, std::distance(begin, end),
                    higher_priority_task_woken);
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
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
  }
  size_t receive_isr(void *data, size_t data_size,
                     BaseType_t &higher_priority_task_woken) {
    return xStreamBufferReceiveFromISR(m_stream_buffer, data, data_size,
                                       &higher_priority_task_woken);
  }
  size_t available(void) {
    return xStreamBufferBytesAvailable(m_stream_buffer);
  }
  size_t free(void) { return xStreamBufferSpacesAvailable(m_stream_buffer); }
  BaseType_t reset(void) { return xStreamBufferReset(m_stream_buffer); }
  BaseType_t set_trigger_level(size_t trigger_level_bytes) {
    return xStreamBufferSetTriggerLevel(m_stream_buffer, trigger_level_bytes);
  }
  StreamBufferHandle_t handle(void) const { return m_stream_buffer; }
  BaseType_t empty(void) { return xStreamBufferIsEmpty(m_stream_buffer); }
  BaseType_t full(void) { return xStreamBufferIsFull(m_stream_buffer); }
};

#if configSUPPORT_STATIC_ALLOCATION
namespace sa {
template <size_t StreamBufferSize>
using stream_buffer = freertos::stream_buffer<
    StreamBufferSize,
    freertos::static_stream_buffer_allocator<StreamBufferSize>>;
} // namespace sa
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
namespace da {
template <size_t StreamBufferSize>
using stream_buffer = freertos::stream_buffer<
    StreamBufferSize,
    freertos::dynamic_stream_buffer_allocator<StreamBufferSize>>;
} // namespace da
#endif

} // namespace freertos
