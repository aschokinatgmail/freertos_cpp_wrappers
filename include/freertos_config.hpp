/**********************************************************************************
@file freertos_config.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief Feature detection header for FreeRTOS C++ Wrappers
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

#include <FreeRTOS.h>

#if __cplusplus >= 202002L
#define FREERTOS_CPP_WRAPPERS_CPP20 1
#define FREERTOS_CPP_WRAPPERS_CPP17 1
#elif __cplusplus >= 201703L
#define FREERTOS_CPP_WRAPPERS_CPP17 1
#define FREERTOS_CPP_WRAPPERS_CPP20 0
#else
#define FREERTOS_CPP_WRAPPERS_CPP17 0
#define FREERTOS_CPP_WRAPPERS_CPP20 0
#endif

#if defined(configUSE_MUTEXES) && configUSE_MUTEXES == 1
#define FREERTOS_CPP_WRAPPERS_HAS_MUTEX 1
#else
#define FREERTOS_CPP_WRAPPERS_HAS_MUTEX 0
#endif

#if defined(configUSE_COUNTING_SEMAPHORES) && configUSE_COUNTING_SEMAPHORES == 1
#define FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE 1
#else
#define FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE 0
#endif

#if defined(configUSE_RECURSIVE_MUTEXES) && configUSE_RECURSIVE_MUTEXES == 1
#define FREERTOS_CPP_WRAPPERS_HAS_RECURSIVE_MUTEX 1
#else
#define FREERTOS_CPP_WRAPPERS_HAS_RECURSIVE_MUTEX 0
#endif

#if defined(configUSE_TIMERS) && configUSE_TIMERS == 1
#define FREERTOS_CPP_WRAPPERS_HAS_SW_TIMER 1
#else
#define FREERTOS_CPP_WRAPPERS_HAS_SW_TIMER 0
#endif

#if defined(configUSE_TASK_NOTIFICATIONS) && configUSE_TASK_NOTIFICATIONS == 1
#define FREERTOS_CPP_WRAPPERS_HAS_TASK_NOTIFICATIONS 1
#else
#define FREERTOS_CPP_WRAPPERS_HAS_TASK_NOTIFICATIONS 0
#endif

#if defined(configUSE_STREAM_BUFFERS) && configUSE_STREAM_BUFFERS == 1
#define FREERTOS_CPP_WRAPPERS_HAS_STREAM_BUFFER 1
#else
#define FREERTOS_CPP_WRAPPERS_HAS_STREAM_BUFFER 0
#endif

#if defined(configUSE_EVENT_GROUPS) && configUSE_EVENT_GROUPS == 1
#define FREERTOS_CPP_WRAPPERS_HAS_EVENT_GROUP 1
#else
#define FREERTOS_CPP_WRAPPERS_HAS_EVENT_GROUP 0
#endif

#if defined(configSUPPORT_STATIC_ALLOCATION) &&                                \
    configSUPPORT_STATIC_ALLOCATION == 1
#define FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION 1
#else
#define FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION 0
#endif

#if defined(configSUPPORT_DYNAMIC_ALLOCATION) &&                               \
    configSUPPORT_DYNAMIC_ALLOCATION == 1
#define FREERTOS_CPP_WRAPPERS_HAS_DYNAMIC_ALLOCATION 1
#else
#define FREERTOS_CPP_WRAPPERS_HAS_DYNAMIC_ALLOCATION 0
#endif

#if FREERTOS_CPP_WRAPPERS_HAS_RECURSIVE_MUTEX &&                               \
    !FREERTOS_CPP_WRAPPERS_HAS_MUTEX
static_assert(
    false,
    "configUSE_RECURSIVE_MUTEXES requires configUSE_MUTEXES to be enabled");
#endif

#if !FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION &&                            \
    !FREERTOS_CPP_WRAPPERS_HAS_DYNAMIC_ALLOCATION
static_assert(false, "At least one of configSUPPORT_STATIC_ALLOCATION or "
                     "configSUPPORT_DYNAMIC_ALLOCATION must be enabled");
#endif

#if !FREERTOS_CPP_WRAPPERS_CPP17
static_assert(false, "FreeRTOS C++ Wrappers require C++17 or later");
#endif