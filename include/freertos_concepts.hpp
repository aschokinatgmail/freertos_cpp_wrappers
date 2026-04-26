/**********************************************************************************
@file freertos_concepts.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief C++20 concepts for FreeRTOS wrapper template constraints
@version 3.2.0
@date 2026-04-21

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

#include "freertos_config.hpp"

#include <FreeRTOS.h>
#include <chrono>
#include <concepts>
#include <cstddef>

namespace freertos {

#if __cpp_concepts >= 201907L

template <typename A>
concept FreeRTOSAllocator = requires(size_t n) {
  { A::allocate(n) } -> std::same_as<void *>;
  { A::deallocate(std::declval<void *>(), n) } -> std::same_as<void>;
};

template <typename T>
concept Waitable = requires(T t, TickType_t ticks) {
  { t.acquire(ticks) } -> std::same_as<void>;
};

template <typename T>
concept Synchronizable = Waitable<T> && requires(T t) {
  { t.release() } -> std::same_as<void>;
};

template <typename T>
concept Lockable = requires(T t) {
  { t.lock() } -> std::same_as<BaseType_t>;
  { t.unlock() } -> std::same_as<BaseType_t>;
};

template <typename T>
concept TimedLockable =
    Lockable<T> && requires(T t, std::chrono::milliseconds dur) {
      { t.try_lock_for(dur) } -> std::same_as<bool>;
    };

template <typename F>
concept TaskCallback = std::invocable<F>;

template <typename F>
concept TimerCallback = std::invocable<F>;

#endif

} // namespace freertos