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
 *  Mirrors `pdMS_TO_TICKS()` but clamps to `portMAX_DELAY` when the
 *  multiplication `ms * configTICK_RATE_HZ / 1000` would overflow
 *  `TickType_t`. `pdMS_TO_TICKS` itself is a macro that performs the
 *  multiplication in the underlying type without overflow checks; for
 *  durations longer than ~24 days at 1 kHz on a 32-bit `TickType_t` it
 *  silently wraps and produces an unexpectedly short delay.
 */
template <typename Rep, typename Period>
[[nodiscard]] constexpr TickType_t
to_ticks_saturating(const std::chrono::duration<Rep, Period> &d) noexcept {
  const auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
  if (ms <= 0) {
    return 0;
  }
  using ms_t = decltype(ms);
  constexpr ms_t max_ms =
      static_cast<ms_t>(portMAX_DELAY) * 1000 / configTICK_RATE_HZ;
  if (ms >= max_ms) {
    return portMAX_DELAY;
  }
  return pdMS_TO_TICKS(ms);
}

} // namespace freertos