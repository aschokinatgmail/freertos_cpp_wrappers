/**********************************************************************************
@file freertos_clock.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS tick-based TrivialClock and tick conversion utilities
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

#include <FreeRTOS.h>
#include <task.h>
#include <chrono>
#include <cstdint>
#include <ratio>
#include <type_traits>

namespace freertos {

/** @brief TrivialClock using FreeRTOS tick count.
 *
 *  Meets the C++ TrivialClock requirements, using `configTICK_RATE_HZ`
 *  as the tick period. The clock is steady since tick counts are
 *  monotonically non-decreasing.
 */
class tick_timer {
public:
  using rep = TickType_t;
  using period = std::ratio<1, configTICK_RATE_HZ>;
  using duration = std::chrono::duration<rep, period>;
  using time_point = std::chrono::time_point<tick_timer>;

  static constexpr bool is_steady = true;

  [[nodiscard]] static time_point now() noexcept {
    return time_point{duration{xTaskGetTickCount()}};
  }
};

/** @brief Convert a std::chrono::duration to FreeRTOS ticks.
 *
 *  Uses `std::chrono::duration_cast` to convert to the clock's native
 *  duration (tick-based) and returns the tick count.
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr TickType_t
to_ticks(const std::chrono::duration<Rep, Period> &d) noexcept {
  return static_cast<TickType_t>(
      std::chrono::duration_cast<tick_timer::duration>(d).count());
}

/** @brief Convert FreeRTOS ticks to a std::chrono::duration.
 *
 *  Constructs a `tick_timer::duration` from the tick count and converts
 *  to the target duration type using `std::chrono::duration_cast`.
 */
template <typename Duration = std::chrono::milliseconds>
[[nodiscard]] constexpr Duration
to_duration(TickType_t ticks) noexcept {
  return std::chrono::duration_cast<Duration>(tick_timer::duration{ticks});
}

/** @brief Saturating millisecond-to-tick conversion.
 *
 *  Converts a `std::chrono::duration` to FreeRTOS ticks while clamping the
 *  result to `portMAX_DELAY`. Unlike `pdMS_TO_TICKS()`, which expands to a
 *  macro that performs `ms * configTICK_RATE_HZ / 1000` in the operand's
 *  native type and silently wraps on overflow, this helper performs the
 *  multiplication in 64-bit and only narrows once the value is known to fit.
 *
 *  This bypasses `pdMS_TO_TICKS` deliberately: at a 1 kHz tick rate with a
 *  32-bit `TickType_t`, the macro starts wrapping when the millisecond count
 *  exceeds ~4.3 million (≈ 71 minutes), but `portMAX_DELAY` only saturates at
 *  ~4.29 billion ms (≈ 49.7 days). That gap is a silent-corruption window
 *  where a long timeout becomes an unexpectedly short one. Using 64-bit
 *  intermediate math closes it.
 *
 *  Negative durations and zero return 0; values that would exceed
 *  `portMAX_DELAY` saturate to `portMAX_DELAY`.
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr TickType_t
to_ticks_saturating(const std::chrono::duration<Rep, Period> &d) noexcept {
  const auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
  if (ms <= 0) {
    return 0;
  }
  const auto raw = (static_cast<uint64_t>(ms) * configTICK_RATE_HZ) / 1000U;
  return raw > portMAX_DELAY ? portMAX_DELAY : static_cast<TickType_t>(raw);
}

} // namespace freertos