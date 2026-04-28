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
 *  @note The data members `result` and `higher_priority_task_woken` are
 *        intentionally kept public for backward compatibility with existing
 *        code. New code should prefer the accessor methods
 *        `result_value()`, `higher_priority_task_woken_value()`, and
 *        `should_yield()` for safer, encapsulated access. The
 *        `portYIELD_FROM_ISR()` macro is intentionally NOT wrapped in a
 *        helper because some FreeRTOS ports define it in ways that require
 *        expansion at the call site (e.g., inline assembly). Invoke it
 *        directly when `should_yield()` returns true.
 *
 *  @tparam T The type of the operation result value
 */
template <typename T> struct isr_result {
  T result;
  BaseType_t higher_priority_task_woken;

  /** @brief Read-only accessor for the operation result value.
   *  @return const reference to the underlying `result` member.
   */
  const T &result_value() const { return result; }

  /** @brief Mutable accessor for the operation result value.
   *  @return reference to the underlying `result` member.
   */
  T &result_value() { return result; }

  /** @brief Read-only accessor for the higher_priority_task_woken flag.
   *  @return const reference to the underlying flag member.
   */
  const BaseType_t &higher_priority_task_woken_value() const {
    return higher_priority_task_woken;
  }

  /** @brief Mutable accessor for the higher_priority_task_woken flag.
   *  @return reference to the underlying flag member.
   */
  BaseType_t &higher_priority_task_woken_value() {
    return higher_priority_task_woken;
  }

  /** @brief Whether the ISR should yield to a higher-priority task.
   *  @return true if `higher_priority_task_woken` indicates a yield is needed.
   *
   *  At the end of the ISR, callers should invoke
   *  `portYIELD_FROM_ISR(higher_priority_task_woken)` directly when this
   *  returns true. The port-specific macro is intentionally not wrapped
   *  here because some FreeRTOS ports define it in ways that require
   *  expansion at the call site (e.g., as inline assembly).
   */
  bool should_yield() const { return higher_priority_task_woken == pdTRUE; }
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
 *
 *  @note The data member `higher_priority_task_woken` remains public for
 *        backward compatibility. New code should prefer the accessor methods
 *        `higher_priority_task_woken_value()` and `should_yield()`. As with
 *        the primary template, `portYIELD_FROM_ISR()` is intentionally not
 *        wrapped — invoke it directly at the end of the ISR when
 *        `should_yield()` returns true.
 */
template <> struct isr_result<void> {
  BaseType_t higher_priority_task_woken;

  /** @brief Read-only accessor for the higher_priority_task_woken flag. */
  const BaseType_t &higher_priority_task_woken_value() const {
    return higher_priority_task_woken;
  }

  /** @brief Mutable accessor for the higher_priority_task_woken flag. */
  BaseType_t &higher_priority_task_woken_value() {
    return higher_priority_task_woken;
  }

  /** @brief Whether the ISR should yield to a higher-priority task.
   *
   *  At the end of the ISR, callers should invoke
   *  `portYIELD_FROM_ISR(higher_priority_task_woken)` directly when this
   *  returns true. The port-specific macro is intentionally not wrapped
   *  here because some FreeRTOS ports define it in ways that require
   *  expansion at the call site (e.g., as inline assembly).
   */
  bool should_yield() const { return higher_priority_task_woken == pdTRUE; }
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
