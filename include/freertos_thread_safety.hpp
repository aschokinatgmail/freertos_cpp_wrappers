/**********************************************************************************
@file freertos_thread_safety.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief Thread safety annotation macros for FreeRTOS C++ Wrappers
@version 3.2.0
@date 2026-04-17

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

/** @file freertos_thread_safety.hpp
 *  @brief Thread safety annotation macros for FreeRTOS C++ Wrappers.
 *
 * When FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS is enabled and
 * compiled with Clang, these macros expand to __attribute__((capability(...)))
 * annotations that enable Clang's Thread Safety Analysis. On other compilers
 * or when disabled, they expand to nothing.
 *
 * Usage: Define FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS=1 before
 * including this header, or pass -DFREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS=1
 * to the compiler.
 */

#pragma once

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#if defined(FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS) &&                \
    FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS

#if defined(__clang__)
#define FREERTOS_CAPABILITY(...) __attribute__((capability(__VA_ARGS__)))
#define FREERTOS_SCOPED_CAPABILITY __attribute__((scoped_lockable))
#define FREERTOS_GUARDED_BY(...) __attribute__((guarded_by(__VA_ARGS__)))
#define FREERTOS_PT_GUARDED_BY(...) __attribute__((pt_guarded_by(__VA_ARGS__)))
#define FREERTOS_ACQUIRED_BY(...) __attribute__((acquired_by(__VA_ARGS__)))
#define FREERTOS_REQUIRES(...) __attribute__((requires_capability(__VA_ARGS__)))
#define FREERTOS_REQUIRES_SHARED(...)                                          \
  __attribute__((requires_shared_capability(__VA_ARGS__)))
#define FREERTOS_ACQUIRE(...) __attribute__((acquire_capability(__VA_ARGS__)))
#define FREERTOS_ACQUIRE_SHARED(...)                                           \
  __attribute__((acquire_shared_capability(__VA_ARGS__)))
#define FREERTOS_RELEASE(...) __attribute__((release_capability(__VA_ARGS__)))
#define FREERTOS_RELEASE_SHARED(...)                                           \
  __attribute__((release_shared_capability(__VA_ARGS__)))
#define FREERTOS_TRY_ACQUIRE(...)                                              \
  __attribute__((try_acquire_capability(__VA_ARGS__)))
#define FREERTOS_TRY_ACQUIRE_SHARED(...)                                       \
  __attribute__((try_acquire_shared_capability(__VA_ARGS__)))
#define FREERTOS_EXCLUDES(...) __attribute__((locks_excluded(__VA_ARGS__)))
#define FREERTOS_ASSERT_CAPABILITY(...)                                        \
  __attribute__((assert_capability(__VA_ARGS__)))
#define FREERTOS_ASSERT_SHARED_CAPABILITY(...)                                 \
  __attribute__((assert_shared_capability(__VA_ARGS__)))
#define FREERTOS_RETURN_CAPABILITY(...)                                        \
  __attribute__((lock_returned(__VA_ARGS__)))
#define FREERTOS_NO_THREAD_SAFETY_ANALYSIS                                     \
  __attribute__((no_thread_safety_analysis))
#else
#define FREERTOS_CAPABILITY(...)
#define FREERTOS_SCOPED_CAPABILITY
#define FREERTOS_GUARDED_BY(...)
#define FREERTOS_PT_GUARDED_BY(...)
#define FREERTOS_ACQUIRED_BY(...)
#define FREERTOS_REQUIRES(...)
#define FREERTOS_REQUIRES_SHARED(...)
#define FREERTOS_ACQUIRE(...)
#define FREERTOS_ACQUIRE_SHARED(...)
#define FREERTOS_RELEASE(...)
#define FREERTOS_RELEASE_SHARED(...)
#define FREERTOS_TRY_ACQUIRE(...)
#define FREERTOS_TRY_ACQUIRE_SHARED(...)
#define FREERTOS_EXCLUDES(...)
#define FREERTOS_ASSERT_CAPABILITY(...)
#define FREERTOS_ASSERT_SHARED_CAPABILITY(...)
#define FREERTOS_RETURN_CAPABILITY(...)
#define FREERTOS_NO_THREAD_SAFETY_ANALYSIS
#endif

#else

#define FREERTOS_CAPABILITY(...)
#define FREERTOS_SCOPED_CAPABILITY
#define FREERTOS_GUARDED_BY(...)
#define FREERTOS_PT_GUARDED_BY(...)
#define FREERTOS_ACQUIRED_BY(...)
#define FREERTOS_REQUIRES(...)
#define FREERTOS_REQUIRES_SHARED(...)
#define FREERTOS_ACQUIRE(...)
#define FREERTOS_ACQUIRE_SHARED(...)
#define FREERTOS_RELEASE(...)
#define FREERTOS_RELEASE_SHARED(...)
#define FREERTOS_TRY_ACQUIRE(...)
#define FREERTOS_TRY_ACQUIRE_SHARED(...)
#define FREERTOS_EXCLUDES(...)
#define FREERTOS_ASSERT_CAPABILITY(...)
#define FREERTOS_ASSERT_SHARED_CAPABILITY(...)
#define FREERTOS_RETURN_CAPABILITY(...)
#define FREERTOS_NO_THREAD_SAFETY_ANALYSIS

#endif