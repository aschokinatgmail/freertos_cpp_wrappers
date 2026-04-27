/**********************************************************************************
@file freertos_strong_types.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief Strong typedefs for FreeRTOS primitive types
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

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#include <FreeRTOS.h>
#include <type_traits>

namespace freertos {

/** @brief Strong typedef for FreeRTOS task priority values.
 *
 * Prevents accidental mixing of task priorities with other integer types.
 * Supports comparison operators and above()/below() for relative priority navigation.
 */
class priority {
public:
  explicit constexpr priority(UBaseType_t value = 0) : m_value(value) {}

  constexpr UBaseType_t value() const noexcept { return m_value; }

  constexpr bool operator==(const priority &other) const noexcept {
    return m_value == other.m_value;
  }
  constexpr bool operator!=(const priority &other) const noexcept {
    return m_value != other.m_value;
  }
  constexpr bool operator<(const priority &other) const noexcept {
    return m_value < other.m_value;
  }
  constexpr bool operator<=(const priority &other) const noexcept {
    return m_value <= other.m_value;
  }
  constexpr bool operator>(const priority &other) const noexcept {
    return m_value > other.m_value;
  }
  constexpr bool operator>=(const priority &other) const noexcept {
    return m_value >= other.m_value;
  }

  constexpr priority above() const noexcept { return priority{m_value + 1}; }
  constexpr priority below() const noexcept {
    return priority{m_value > 0 ? m_value - 1 : 0};
  }

private:
  UBaseType_t m_value;
};

/** @brief Strong typedef for FreeRTOS event group bits.
 *
 * Prevents accidental mixing of event bit values with other integer types.
 * Supports bitwise OR, AND, XOR, and NOT operations for combining and masking.
 */
class event_bits {
public:
  explicit constexpr event_bits(EventBits_t value = 0) : m_value(value) {}

  constexpr EventBits_t value() const noexcept { return m_value; }

  constexpr event_bits operator|(const event_bits &other) const noexcept {
    return event_bits{m_value | other.m_value};
  }
  constexpr event_bits operator&(const event_bits &other) const noexcept {
    return event_bits{m_value & other.m_value};
  }
  constexpr event_bits operator^(const event_bits &other) const noexcept {
    return event_bits{m_value ^ other.m_value};
  }
  constexpr event_bits operator~() const noexcept {
    return event_bits{~m_value};
  }

  constexpr event_bits &operator|=(const event_bits &other) noexcept {
    m_value |= other.m_value;
    return *this;
  }
  constexpr event_bits &operator&=(const event_bits &other) noexcept {
    m_value &= other.m_value;
    return *this;
  }
  constexpr event_bits &operator^=(const event_bits &other) noexcept {
    m_value ^= other.m_value;
    return *this;
  }

  constexpr bool operator==(const event_bits &other) const noexcept {
    return m_value == other.m_value;
  }
  constexpr bool operator!=(const event_bits &other) const noexcept {
    return m_value != other.m_value;
  }

  constexpr bool operator!() const noexcept { return m_value == 0; }
  constexpr bool any() const noexcept { return m_value != 0; }
  constexpr bool none() const noexcept { return m_value == 0; }

private:
  EventBits_t m_value;
};

/** @brief Strong typedef for FreeRTOS tick count values.
 *
 * Prevents accidental mixing of tick counts with other integer types.
 * Supports arithmetic (addition, subtraction) and relational comparisons.
 */
class tick_count {
public:
  explicit constexpr tick_count(TickType_t value = 0) : m_value(value) {}

  constexpr TickType_t value() const noexcept { return m_value; }

  constexpr tick_count operator+(const tick_count &other) const noexcept {
    return tick_count{m_value + other.m_value};
  }
  constexpr tick_count operator-(const tick_count &other) const noexcept {
    return tick_count{m_value - other.m_value};
  }

  constexpr tick_count &operator+=(const tick_count &other) noexcept {
    m_value += other.m_value;
    return *this;
  }
  constexpr tick_count &operator-=(const tick_count &other) noexcept {
    m_value -= other.m_value;
    return *this;
  }

  constexpr bool operator==(const tick_count &other) const noexcept {
    return m_value == other.m_value;
  }
  constexpr bool operator!=(const tick_count &other) const noexcept {
    return m_value != other.m_value;
  }
  constexpr bool operator<(const tick_count &other) const noexcept {
    return m_value < other.m_value;
  }
  constexpr bool operator<=(const tick_count &other) const noexcept {
    return m_value <= other.m_value;
  }
  constexpr bool operator>(const tick_count &other) const noexcept {
    return m_value > other.m_value;
  }
  constexpr bool operator>=(const tick_count &other) const noexcept {
    return m_value >= other.m_value;
  }

private:
  TickType_t m_value;
};

/** @brief Strong typedef for SMP core affinity mask values.
 *
 * Used with FreeRTOS SMP builds (configNUMBER_OF_CORES > 1) to pin tasks
 * to specific CPU cores. Supports bitwise operations for combining masks.
 * Use core(uint8_t) factory method to create single-core masks.
 */
class core_affinity_mask {
public:
  explicit constexpr core_affinity_mask(UBaseType_t value = 0)
      : m_value(value) {}

  constexpr UBaseType_t value() const noexcept { return m_value; }

  constexpr core_affinity_mask
  operator|(const core_affinity_mask &other) const noexcept {
    return core_affinity_mask{m_value | other.m_value};
  }
  constexpr core_affinity_mask
  operator&(const core_affinity_mask &other) const noexcept {
    return core_affinity_mask{m_value & other.m_value};
  }
  constexpr core_affinity_mask
  operator^(const core_affinity_mask &other) const noexcept {
    return core_affinity_mask{m_value ^ other.m_value};
  }
  constexpr core_affinity_mask operator~() const noexcept {
    return core_affinity_mask{~m_value};
  }

  constexpr core_affinity_mask &
  operator|=(const core_affinity_mask &other) noexcept {
    m_value |= other.m_value;
    return *this;
  }
  constexpr core_affinity_mask &
  operator&=(const core_affinity_mask &other) noexcept {
    m_value &= other.m_value;
    return *this;
  }
  constexpr core_affinity_mask &
  operator^=(const core_affinity_mask &other) noexcept {
    m_value ^= other.m_value;
    return *this;
  }

  constexpr bool operator==(const core_affinity_mask &other) const noexcept {
    return m_value == other.m_value;
  }
  constexpr bool operator!=(const core_affinity_mask &other) const noexcept {
    return m_value != other.m_value;
  }

  constexpr bool operator!() const noexcept { return m_value == 0; }

  static constexpr core_affinity_mask core(uint8_t core_id) noexcept {
    return core_affinity_mask{static_cast<UBaseType_t>(1U) << core_id};
  }

private:
  UBaseType_t m_value;
};

} // namespace freertos