/**********************************************************************************
@file freertos_isr_result.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief ISR result type for FreeRTOS C++ Wrappers
@version 3.2.0
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

namespace freertos {

/** @brief Result type for ISR-safe FreeRTOS wrapper methods.
 *
 *  Bundles the operation return value with the higher_priority_task_woken flag
 *  required for proper ISR context handling. After all ISR operations, check
 *  higher_priority_task_woken and call portYIELD_FROM_ISR() if set.
 *
 *  @tparam T The type of the operation result value
 */
template <typename T> struct isr_result {
  T result;
  BaseType_t higher_priority_task_woken;
};

/** @brief Specialization for ISR wrappers with no return value.
 *
 *  This specialization omits the `result` field — only
 *  `higher_priority_task_woken` is available. Callers that need
 *  a result should use `isr_result<T>` instead.
 *
 *  Currently used only by the simulation test suite. A production
 *  consumer will be added when a void-returning ISR wrapper is
 *  introduced (e.g., event_group::clear_bits_isr).
 */
template <> struct isr_result<void> {
  BaseType_t higher_priority_task_woken;
};

template <typename T>
bool operator==(const isr_result<T> &lhs, const isr_result<T> &rhs) {
  return lhs.result == rhs.result &&
         lhs.higher_priority_task_woken == rhs.higher_priority_task_woken;
}

inline bool operator==(const isr_result<void> &lhs,
                       const isr_result<void> &rhs) {
  return lhs.higher_priority_task_woken == rhs.higher_priority_task_woken;
}

template <typename T>
bool operator!=(const isr_result<T> &lhs, const isr_result<T> &rhs) {
  return !(lhs == rhs);
}

inline bool operator!=(const isr_result<void> &lhs,
                       const isr_result<void> &rhs) {
  return !(lhs == rhs);
}

} // namespace freertos
