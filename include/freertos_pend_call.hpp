/**********************************************************************************
@file freertos_pend_call.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS pend_call (deferred function execution from ISR) wrapper
@version 3.2.0
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
************************************************************************************/

#pragma once

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#include "freertos_config.hpp"
#include "freertos_expected.hpp"

#if FREERTOS_CPP_WRAPPERS_HAS_SW_TIMER

#include <FreeRTOS.h>
#include <timers.h>
#include <type_traits>
#include <utility>

namespace freertos {

inline bool pend_call(void (*function)(void *, uint32_t), void *arg1,
                       uint32_t arg2,
                       TickType_t timeout = portMAX_DELAY) {
  return xTimerPendFunctionCall(function, arg1, arg2, timeout) == pdPASS;
}

inline expected<void, error>
pend_call_ex(void (*function)(void *, uint32_t), void *arg1, uint32_t arg2,
             TickType_t timeout = portMAX_DELAY) {
  auto rc = xTimerPendFunctionCall(function, arg1, arg2, timeout);
  if (rc == pdPASS) {
    return {};
  }
  return unexpected<error>(error::timeout);
}

template <typename Callable,
          std::enable_if_t<!std::is_convertible_v<Callable, void (*)(void *, uint32_t)>,
                           int> = 0>
bool pend_call(Callable &&callable, TickType_t timeout = portMAX_DELAY) {
  using CallableType = std::remove_reference_t<Callable>;
  auto *ptr = new (std::nothrow) CallableType(std::forward<Callable>(callable));
  if (!ptr) {
    return false;
  }
  void (*adapter)(void *, uint32_t) = [](void *p, uint32_t) {
    auto *c = static_cast<CallableType *>(p);
    (*c)();
    delete c;
  };
  auto rc = xTimerPendFunctionCall(adapter, ptr, 0, timeout);
  if (rc != pdPASS) {
    delete ptr;
    return false;
  }
  return true;
}

} // namespace freertos

#endif // FREERTOS_CPP_WRAPPERS_HAS_SW_TIMER