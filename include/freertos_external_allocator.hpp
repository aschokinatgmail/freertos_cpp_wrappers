/**********************************************************************************
@file freertos_external_allocator.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief External memory allocation for FreeRTOS C++ Wrappers
@version 3.0.0
@date 2026-04-16

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
************************************************************************************/

#pragma once

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#if !configSUPPORT_STATIC_ALLOCATION
#error                                                                         \
    "External allocators require configSUPPORT_STATIC_ALLOCATION to be enabled"
#endif

#include "freertos_config.hpp"

#include <FreeRTOS.h>
#include <array>
#include <cstddef>
#include <event_groups.h>
#include <message_buffer.h>
#include <queue.h>
#include <semphr.h>
#include <stream_buffer.h>
#include <task.h>
#include <timers.h>

namespace freertos {

/** @brief Interface for external memory regions used by external allocators.
 *
 * Provide an implementation with allocate() and deallocate() functions
 * to supply user-controlled memory to FreeRTOS wrapper objects.
 */
struct external_memory_region {
  void *(*allocate)(size_t size);
  void (*deallocate)(void *ptr);
};

/** @brief External allocator for semaphore objects (binary, counting, mutex, recursive mutex).
 *
 * Allocates StaticSemaphore_t from the supplied memory Region.
 * Requires configSUPPORT_STATIC_ALLOCATION.
 *
 * @tparam Region A type implementing external_memory_region interface (allocate/deallocate)
 */
template <typename Region> class external_semaphore_allocator {
  Region *m_region{nullptr};
  void *m_memory{nullptr};

  external_semaphore_allocator(const external_semaphore_allocator &) = delete;
  external_semaphore_allocator &
  operator=(const external_semaphore_allocator &) = delete;
  external_semaphore_allocator &
  operator=(external_semaphore_allocator &&) = delete;

public:
  explicit external_semaphore_allocator(Region &region) : m_region(&region) {}
  external_semaphore_allocator(external_semaphore_allocator &&other) noexcept
      : m_region(other.m_region), m_memory(other.m_memory) {
    other.m_memory = nullptr;
    other.m_region = nullptr;
  }

  void swap(external_semaphore_allocator &other) noexcept {
    using std::swap;
    swap(m_region, other.m_region);
    swap(m_memory, other.m_memory);
  }

  ~external_semaphore_allocator() {
    if (m_memory) {
      m_region->deallocate(m_memory);
    }
  }

  SemaphoreHandle_t create_binary() {
    m_memory = m_region->allocate(sizeof(StaticSemaphore_t));
    if (!m_memory) {
      return nullptr;
    }
    return xSemaphoreCreateBinaryStatic(
        static_cast<StaticSemaphore_t *>(m_memory)); // NOLINT(clang-tidy:cppcoreguidelines-pro-type-static-cast-downcast)
  }
  SemaphoreHandle_t create_counting(UBaseType_t max_count) {
    m_memory = m_region->allocate(sizeof(StaticSemaphore_t));
    if (!m_memory) {
      return nullptr;
    }
    return xSemaphoreCreateCountingStatic(
        max_count, max_count, static_cast<StaticSemaphore_t *>(m_memory)); // NOLINT(clang-tidy:cppcoreguidelines-pro-type-static-cast-downcast)
  }
  SemaphoreHandle_t create_mutex() {
    m_memory = m_region->allocate(sizeof(StaticSemaphore_t));
    if (!m_memory) {
      return nullptr;
    }
    return xSemaphoreCreateMutexStatic(
        static_cast<StaticSemaphore_t *>(m_memory)); // NOLINT(clang-tidy:cppcoreguidelines-pro-type-static-cast-downcast)
  }
  SemaphoreHandle_t create_recursive_mutex() {
    m_memory = m_region->allocate(sizeof(StaticSemaphore_t));
    if (!m_memory) {
      return nullptr;
    }
    return xSemaphoreCreateRecursiveMutexStatic(
        static_cast<StaticSemaphore_t *>(m_memory)); // NOLINT(clang-tidy:cppcoreguidelines-pro-type-static-cast-downcast)
  }
};

/** @brief External allocator for queue objects.
 *
 * Allocates both the StaticQueue_t structure and the item storage array
 * from the supplied memory Region. Requires configSUPPORT_STATIC_ALLOCATION.
 *
 * @tparam Region      A type implementing external_memory_region interface
 * @tparam QueueLength Maximum number of items in the queue
 * @tparam T           Type of items stored in the queue
 */
template <typename Region, size_t QueueLength, typename T>
class external_queue_allocator {
  Region *m_region{nullptr};
  void *m_struct_memory{nullptr};
  void *m_storage_memory{nullptr};

  external_queue_allocator(const external_queue_allocator &) = delete;
  external_queue_allocator &
  operator=(const external_queue_allocator &) = delete;
  external_queue_allocator &operator=(external_queue_allocator &&) = delete;

public:
  explicit external_queue_allocator(Region &region) : m_region(&region) {}
  external_queue_allocator(external_queue_allocator &&other) noexcept
      : m_region(other.m_region), m_struct_memory(other.m_struct_memory),
        m_storage_memory(other.m_storage_memory) {
    other.m_struct_memory = nullptr;
    other.m_storage_memory = nullptr;
    other.m_region = nullptr;
  }

  void swap(external_queue_allocator &other) noexcept {
    using std::swap;
    swap(m_region, other.m_region);
    swap(m_struct_memory, other.m_struct_memory);
    swap(m_storage_memory, other.m_storage_memory);
  }

  ~external_queue_allocator() {
    if (m_struct_memory) {
      m_region->deallocate(m_struct_memory);
    }
    if (m_storage_memory) {
      m_region->deallocate(m_storage_memory);
    }
  }

  QueueHandle_t create() {
    m_struct_memory = m_region->allocate(sizeof(StaticQueue_t));
    if (!m_struct_memory) {
      return nullptr;
    }
    m_storage_memory = m_region->allocate(QueueLength * sizeof(T));
    if (!m_storage_memory) {
      m_region->deallocate(m_struct_memory);
      m_struct_memory = nullptr;
      return nullptr;
    }
    return xQueueCreateStatic(QueueLength, sizeof(T),
                              static_cast<uint8_t *>(m_storage_memory),
                              static_cast<StaticQueue_t *>(m_struct_memory)); // NOLINT(clang-tidy:cppcoreguidelines-pro-type-static-cast-downcast)
  }
};

/** @brief External allocator for event group objects.
 *
 * Allocates StaticEventGroup_t from the supplied memory Region.
 * Requires configSUPPORT_STATIC_ALLOCATION.
 *
 * @tparam Region A type implementing external_memory_region interface
 */
template <typename Region> class external_event_group_allocator {
  Region *m_region{nullptr};
  void *m_memory{nullptr};

  external_event_group_allocator(const external_event_group_allocator &) =
      delete;
  external_event_group_allocator &
  operator=(const external_event_group_allocator &) = delete;
  external_event_group_allocator &
  operator=(external_event_group_allocator &&) = delete;

public:
  explicit external_event_group_allocator(Region &region) : m_region(&region) {}
  external_event_group_allocator(
      external_event_group_allocator &&other) noexcept
      : m_region(other.m_region), m_memory(other.m_memory) {
    other.m_memory = nullptr;
    other.m_region = nullptr;
  }

  void swap(external_event_group_allocator &other) noexcept {
    using std::swap;
    swap(m_region, other.m_region);
    swap(m_memory, other.m_memory);
  }

  ~external_event_group_allocator() {
    if (m_memory) {
      m_region->deallocate(m_memory);
    }
  }

  EventGroupHandle_t create() {
    m_memory = m_region->allocate(sizeof(StaticEventGroup_t));
    if (!m_memory) {
      return nullptr;
    }
    return xEventGroupCreateStatic(static_cast<StaticEventGroup_t *>(m_memory)); // NOLINT(clang-tidy:cppcoreguidelines-pro-type-static-cast-downcast)
  }
};

/** @brief External allocator for stream buffer objects.
 *
 * Allocates both the StaticStreamBuffer_t structure and the data buffer
 * from the supplied memory Region. Requires configSUPPORT_STATIC_ALLOCATION.
 *
 * @tparam Region           A type implementing external_memory_region interface
 * @tparam StreamBufferSize Size of the stream buffer in bytes
 */
template <typename Region, size_t StreamBufferSize>
class external_stream_buffer_allocator {
  Region *m_region{nullptr};
  void *m_struct_memory{nullptr};
  void *m_storage_memory{nullptr};

  external_stream_buffer_allocator(const external_stream_buffer_allocator &) =
      delete;
  external_stream_buffer_allocator &
  operator=(const external_stream_buffer_allocator &) = delete;
  external_stream_buffer_allocator &
  operator=(external_stream_buffer_allocator &&) = delete;

public:
  explicit external_stream_buffer_allocator(Region &region)
      : m_region(&region) {}
  external_stream_buffer_allocator(
      external_stream_buffer_allocator &&other) noexcept
      : m_region(other.m_region), m_struct_memory(other.m_struct_memory),
        m_storage_memory(other.m_storage_memory) {
    other.m_struct_memory = nullptr;
    other.m_storage_memory = nullptr;
    other.m_region = nullptr;
  }

  void swap(external_stream_buffer_allocator &other) noexcept {
    using std::swap;
    swap(m_region, other.m_region);
    swap(m_struct_memory, other.m_struct_memory);
    swap(m_storage_memory, other.m_storage_memory);
  }

  ~external_stream_buffer_allocator() {
    if (m_struct_memory) {
      m_region->deallocate(m_struct_memory);
    }
    if (m_storage_memory) {
      m_region->deallocate(m_storage_memory);
    }
  }

  StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
    m_struct_memory = m_region->allocate(sizeof(StaticStreamBuffer_t));
    if (!m_struct_memory) {
      return nullptr;
    }
    m_storage_memory = m_region->allocate(StreamBufferSize);
    if (!m_storage_memory) {
      m_region->deallocate(m_struct_memory);
      m_struct_memory = nullptr;
      return nullptr;
    }
    return xStreamBufferCreateStatic(
        StreamBufferSize, trigger_level_bytes,
        static_cast<uint8_t *>(m_storage_memory),
        static_cast<StaticStreamBuffer_t *>(m_struct_memory)); // NOLINT(clang-tidy:cppcoreguidelines-pro-type-static-cast-downcast)
  }
};

/** @brief External allocator for message buffer objects.
 *
 * Allocates both the StaticMessageBuffer_t structure and the data buffer
 * from the supplied memory Region. Requires configSUPPORT_STATIC_ALLOCATION.
 *
 * @tparam Region            A type implementing external_memory_region interface
 * @tparam MessageBufferSize Size of the message buffer in bytes
 */
template <typename Region, size_t MessageBufferSize>
class external_message_buffer_allocator {
  Region *m_region{nullptr};
  void *m_struct_memory{nullptr};
  void *m_storage_memory{nullptr};

  external_message_buffer_allocator(const external_message_buffer_allocator &) =
      delete;
  external_message_buffer_allocator &
  operator=(const external_message_buffer_allocator &) = delete;
  external_message_buffer_allocator &
  operator=(external_message_buffer_allocator &&) = delete;

public:
  explicit external_message_buffer_allocator(Region &region)
      : m_region(&region) {}
  external_message_buffer_allocator(
      external_message_buffer_allocator &&other) noexcept
      : m_region(other.m_region), m_struct_memory(other.m_struct_memory),
        m_storage_memory(other.m_storage_memory) {
    other.m_struct_memory = nullptr;
    other.m_storage_memory = nullptr;
    other.m_region = nullptr;
  }

  void swap(external_message_buffer_allocator &other) noexcept {
    using std::swap;
    swap(m_region, other.m_region);
    swap(m_struct_memory, other.m_struct_memory);
    swap(m_storage_memory, other.m_storage_memory);
  }

  ~external_message_buffer_allocator() {
    if (m_struct_memory) {
      m_region->deallocate(m_struct_memory);
    }
    if (m_storage_memory) {
      m_region->deallocate(m_storage_memory);
    }
  }

  MessageBufferHandle_t create() {
    m_struct_memory = m_region->allocate(sizeof(StaticMessageBuffer_t));
    if (!m_struct_memory) {
      return nullptr;
    }
    m_storage_memory = m_region->allocate(MessageBufferSize);
    if (!m_storage_memory) {
      m_region->deallocate(m_struct_memory);
      m_struct_memory = nullptr;
      return nullptr;
    }
    return xMessageBufferCreateStatic(
        MessageBufferSize, static_cast<uint8_t *>(m_storage_memory),
        static_cast<StaticMessageBuffer_t *>(m_struct_memory)); // NOLINT(clang-tidy:cppcoreguidelines-pro-type-static-cast-downcast)
  }
};

/** @brief External allocator for software timer objects.
 *
 * Allocates StaticTimer_t from the supplied memory Region.
 * Requires configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS.
 *
 * @tparam Region A type implementing external_memory_region interface
 */
template <typename Region> class external_sw_timer_allocator {
  Region *m_region{nullptr};
  void *m_memory{nullptr};

  external_sw_timer_allocator(const external_sw_timer_allocator &) = delete;
  external_sw_timer_allocator &
  operator=(const external_sw_timer_allocator &) = delete;
  external_sw_timer_allocator &
  operator=(external_sw_timer_allocator &&) = delete;

public:
  explicit external_sw_timer_allocator(Region &region) : m_region(&region) {}
  external_sw_timer_allocator(external_sw_timer_allocator &&other) noexcept
      : m_region(other.m_region), m_memory(other.m_memory) {
    other.m_memory = nullptr;
    other.m_region = nullptr;
  }

  void swap(external_sw_timer_allocator &other) noexcept {
    using std::swap;
    swap(m_region, other.m_region);
    swap(m_memory, other.m_memory);
  }

  ~external_sw_timer_allocator() {
    if (m_memory) {
      m_region->deallocate(m_memory);
    }
  }

  TimerHandle_t create(const char *name, const TickType_t period_ticks,
                       UBaseType_t auto_reload, void *const timer_id,
                       TimerCallbackFunction_t callback) {
    m_memory = m_region->allocate(sizeof(StaticTimer_t));
    if (!m_memory) {
      return nullptr;
    }
    return xTimerCreateStatic(name, period_ticks, auto_reload, timer_id,
                              callback, static_cast<StaticTimer_t *>(m_memory)); // NOLINT(clang-tidy:cppcoreguidelines-pro-type-static-cast-downcast)
  }
};

/** @brief External allocator for task objects.
 *
 * Allocates both StaticTask_t (TCB) and the task stack from the supplied
 * memory Region. Requires configSUPPORT_STATIC_ALLOCATION.
 *
 * @tparam Region    A type implementing external_memory_region interface
 * @tparam StackSize Stack size in bytes for the task
 */
template <typename Region, size_t StackSize> class external_task_allocator {
  Region *m_region{nullptr};
  void *m_task_memory{nullptr};
  void *m_stack_memory{nullptr};

  external_task_allocator(const external_task_allocator &) = delete;
  external_task_allocator &operator=(const external_task_allocator &) = delete;
  external_task_allocator &operator=(external_task_allocator &&) = delete;

public:
  explicit external_task_allocator(Region &region) : m_region(&region) {}
  external_task_allocator(external_task_allocator &&other) noexcept
      : m_region(other.m_region), m_task_memory(other.m_task_memory),
        m_stack_memory(other.m_stack_memory) {
    other.m_task_memory = nullptr;
    other.m_stack_memory = nullptr;
    other.m_region = nullptr;
  }

  void swap(external_task_allocator &other) noexcept {
    using std::swap;
    swap(m_region, other.m_region);
    swap(m_task_memory, other.m_task_memory);
    swap(m_stack_memory, other.m_stack_memory);
  }

  ~external_task_allocator() {
    if (m_task_memory) {
      m_region->deallocate(m_task_memory);
    }
    if (m_stack_memory) {
      m_region->deallocate(m_stack_memory);
    }
  }

  TaskHandle_t create(TaskFunction_t taskFunction, const char *name,
                      UBaseType_t priority, void *context) {
    m_task_memory = m_region->allocate(sizeof(StaticTask_t));
    if (!m_task_memory) {
      return nullptr;
    }
    m_stack_memory = m_region->allocate(StackSize);
    if (!m_stack_memory) {
      m_region->deallocate(m_task_memory);
      m_task_memory = nullptr;
      return nullptr;
    }
    return xTaskCreateStatic(taskFunction, name,
                             StackSize / sizeof(StackType_t), context, priority,
                             static_cast<StackType_t *>(m_stack_memory),
                             static_cast<StaticTask_t *>(m_task_memory)); // NOLINT(clang-tidy:cppcoreguidelines-pro-type-static-cast-downcast)
  }
};

} // namespace freertos
