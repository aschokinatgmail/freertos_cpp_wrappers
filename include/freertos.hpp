/**********************************************************************************
@file freertos.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS C++ Wrappers Library main header
@version 3.1.0
@date 2024-04-12

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

#include "freertos_version.hpp"

#include "freertos_config.hpp"

#include "freertos_clock.hpp"
#include "freertos_condition_variable.hpp"
#include "freertos_event_group.hpp"
#include "freertos_expected.hpp"
#include "freertos_external_allocator.hpp"
#include "freertos_fixed_function.hpp"
#include "freertos_heap.hpp"
#include "freertos_isr_context.hpp"
#include "freertos_isr_result.hpp"
#include "freertos_latch.hpp"
#include "freertos_lock.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_once_flag.hpp"
#include "freertos_pend_call.hpp"
#include "freertos_queue.hpp"
#include "freertos_queue_set.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_shared_mutex.hpp"
#include "freertos_shared_data.hpp"
#include "freertos_span.hpp"
#include "freertos_stream_batching_buffer.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_strong_types.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_task.hpp"
#include "freertos_thread_safety.hpp"

#include "freertos_atomic.hpp"
#include "freertos_atomic_wait.hpp"
#if !defined(FREERTOS_CPP_WRAPPERS_SIMULATION) && !defined(UNIT_TESTING)
#include "freertos_gthr.hpp"
#include "freertos_external_threading.hpp"
#endif

namespace freertos {

namespace ea {

#if configSUPPORT_STATIC_ALLOCATION

template <typename Region>
using binary_semaphore =
    freertos::binary_semaphore<freertos::external_semaphore_allocator<Region>>;

template <typename Region>
using counting_semaphore = freertos::counting_semaphore<
    freertos::external_semaphore_allocator<Region>>;

template <typename Region>
using mutex = freertos::mutex<freertos::external_semaphore_allocator<Region>>;

template <typename Region>
using recursive_mutex =
    freertos::recursive_mutex<freertos::external_semaphore_allocator<Region>>;

#if FREERTOS_CPP_WRAPPERS_HAS_MUTEX && FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE
template <typename Region>
using shared_mutex = freertos::shared_mutex<
    freertos::external_shared_mutex_allocator<Region>>;
#endif

template <typename Region, size_t QueueLength, typename T>
using queue =
    freertos::queue<QueueLength, T,
                    freertos::external_queue_allocator<Region, QueueLength, T>>;

template <typename Region, size_t StreamBufferSize>
using stream_buffer = freertos::stream_buffer<
    StreamBufferSize,
    freertos::external_stream_buffer_allocator<Region, StreamBufferSize>>;

template <typename Region, size_t MessageBufferSize>
using message_buffer = freertos::message_buffer<
    MessageBufferSize,
    freertos::external_message_buffer_allocator<Region, MessageBufferSize>>;

template <typename Region>
using event_group =
    freertos::event_group<freertos::external_event_group_allocator<Region>>;

#if configUSE_TIMERS
template <typename Region>
using timer = freertos::timer<freertos::external_sw_timer_allocator<Region>>;
#endif

template <typename Region, size_t StackSize>
using task =
    freertos::task<freertos::external_task_allocator<Region, StackSize>>;

template <typename Region, size_t StackSize>
using periodic_task = freertos::periodic_task<
    freertos::external_task_allocator<Region, StackSize>>;

#endif // configSUPPORT_STATIC_ALLOCATION

} // namespace ea

} // namespace freertos
