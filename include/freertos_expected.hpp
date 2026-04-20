/**********************************************************************************
@file freertos_expected.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief std::expected polyfill for FreeRTOS C++ Wrappers
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

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L

#include <expected>

namespace freertos {

template <typename T, typename E> using expected = std::expected<T, E>;

template <typename E> using unexpected = std::unexpected<E>;

} // namespace freertos

#else

#include <FreeRTOS.h>
#include <type_traits>
#include <utility>

namespace freertos {

/** @brief Unified error enumeration for all FreeRTOS wrapper operations.
 *
 * Used as the error type in expected<T, error> return values from
 * _ex-suffixed wrapper methods, providing type-safe error handling
 * instead of bare error codes.
 */
enum class error : uint8_t {
  ok = 0,
  timeout,
  would_block,
  queue_full,
  queue_empty,
  semaphore_not_owned,
  invalid_handle,
  out_of_memory,
  invalid_parameter,
};

/** @brief Wrapper for an unexpected error value, used to construct error states in expected.
 *
 * Analogous to std::unexpected from C++23. Holds an error value of type E
 * and is implicitly convertible from it.
 */
template <typename E> class unexpected {
public:
  constexpr unexpected(const E &err) : m_error(err) {}
  constexpr unexpected(E &&err) : m_error(std::move(err)) {}

  constexpr const E &value() const & noexcept { return m_error; }
  constexpr E &value() & noexcept { return m_error; }
  constexpr const E &&value() const && noexcept { return std::move(m_error); }
  constexpr E &&value() && noexcept { return std::move(m_error); }

private:
  E m_error;
};

template <typename E>
constexpr bool operator==(const unexpected<E> &lhs, const unexpected<E> &rhs) {
  return lhs.value() == rhs.value();
}

template <typename E>
constexpr bool operator!=(const unexpected<E> &lhs, const unexpected<E> &rhs) {
  return !(lhs == rhs);
}

/** @brief Tag type for in-place error construction of expected values.
 *
 * Used as expected(unexpect, error_value) to explicitly construct an error state.
 * Analogous to std::unexpect_t from C++23.
 */
struct unexpect_t {
  explicit unexpect_t() = default;
};

inline constexpr unexpect_t unexpect{};

/** @brief A type-safe return type that holds either a value of type T or an error of type E.
 *
 * Polyfill for std::expected (C++23). Provides value(), error(), has_value(),
 * and_then(), or_else(), transform(), and transform_error() for monadic chaining.
 * When C++23 std::expected is available, it is used directly via an alias.
 *
 * @tparam T The value type on success
 * @tparam E The error type on failure
 */
template <typename T, typename E> class expected {
  constexpr expected(const T &val) : m_has_value(true) {
    new (&m_storage) T(val);
  }

  constexpr expected(T &&val) : m_has_value(true) {
    new (&m_storage) T(std::move(val));
  }

  constexpr expected(const unexpected<E> &err) : m_has_value(false) {
    new (&m_error_storage) E(err.value());
  }

  constexpr expected(unexpected<E> &&err) : m_has_value(false) {
    new (&m_error_storage) E(std::move(err.value()));
  }

  constexpr expected(unexpect_t, const E &err) : m_has_value(false) {
    new (&m_error_storage) E(err);
  }

  constexpr expected(unexpect_t, E &&err) : m_has_value(false) {
    new (&m_error_storage) E(std::move(err));
  }

  ~expected() {
    if (m_has_value) {
      reinterpret_cast<T *>(&m_storage)->~T();
    } else {
      reinterpret_cast<E *>(&m_error_storage)->~E();
    }
  }

  constexpr expected(const expected &other) : m_has_value(other.m_has_value) {
    if (m_has_value) {
      new (&m_storage) T(*reinterpret_cast<const T *>(&other.m_storage));
    } else {
      new (&m_error_storage)
          E(*reinterpret_cast<const E *>(&other.m_error_storage));
    }
  }

  constexpr expected(expected &&other) noexcept
      : m_has_value(other.m_has_value) {
    if (m_has_value) {
      new (&m_storage) T(std::move(*reinterpret_cast<T *>(&other.m_storage)));
    } else {
      new (&m_error_storage)
          E(std::move(*reinterpret_cast<E *>(&other.m_error_storage)));
    }
  }

  constexpr expected &operator=(const expected &other) {
    if (this != &other) {
      this->~expected();
      m_has_value = other.m_has_value;
      if (m_has_value) {
        new (&m_storage) T(*reinterpret_cast<const T *>(&other.m_storage));
      } else {
        new (&m_error_storage)
            E(*reinterpret_cast<const E *>(&other.m_error_storage));
      }
    }
    return *this;
  }

  constexpr expected &operator=(expected &&other) noexcept {
    if (this != &other) {
      this->~expected();
      m_has_value = other.m_has_value;
      if (m_has_value) {
        new (&m_storage) T(std::move(*reinterpret_cast<T *>(&other.m_storage)));
      } else {
        new (&m_error_storage)
            E(std::move(*reinterpret_cast<E *>(&other.m_error_storage)));
      }
    }
    return *this;
  }

  constexpr bool has_value() const noexcept { return m_has_value; }
  constexpr explicit operator bool() const noexcept { return m_has_value; }

  constexpr T &value() & {
    configASSERT(m_has_value);
    return *reinterpret_cast<T *>(&m_storage);
  }

  constexpr const T &value() const & {
    configASSERT(m_has_value);
    return *reinterpret_cast<const T *>(&m_storage);
  }

  constexpr T &&value() && {
    configASSERT(m_has_value);
    return std::move(*reinterpret_cast<T *>(&m_storage));
  }

  constexpr E &error() & {
    configASSERT(!m_has_value);
    return *reinterpret_cast<E *>(&m_error_storage);
  }

  constexpr const E &error() const & {
    configASSERT(!m_has_value);
    return *reinterpret_cast<const E *>(&m_error_storage);
  }

  constexpr E &&error() && {
    configASSERT(!m_has_value);
    return std::move(*reinterpret_cast<E *>(&m_error_storage));
  }

  constexpr T &operator*() & {
    configASSERT(m_has_value);
    return *reinterpret_cast<T *>(&m_storage);
  }

  constexpr const T &operator*() const & {
    configASSERT(m_has_value);
    return *reinterpret_cast<const T *>(&m_storage);
  }

  constexpr T *operator->() {
    configASSERT(m_has_value);
    return reinterpret_cast<T *>(&m_storage);
  }

  constexpr const T *operator->() const {
    configASSERT(m_has_value);
    return reinterpret_cast<const T *>(&m_storage);
  }

  template <typename F>
  constexpr auto and_then(F &&f) & -> decltype(f(std::declval<T &>())) {
    if (m_has_value) {
      return f(*reinterpret_cast<T *>(&m_storage));
    }
    return decltype(f(std::declval<T &>()))(
        unexpected<E>(std::move(*reinterpret_cast<E *>(&m_error_storage))));
  }

  template <typename F>
  constexpr auto
  and_then(F &&f) const & -> decltype(f(std::declval<const T &>())) {
    if (m_has_value) {
      return f(*reinterpret_cast<const T *>(&m_storage));
    }
    return decltype(f(std::declval<const T &>()))(
        unexpected<E>(*reinterpret_cast<const E *>(&m_error_storage)));
  }

  template <typename F>
  constexpr auto or_else(F &&f) & -> decltype(f(std::declval<E &>())) {
    if (!m_has_value) {
      return f(*reinterpret_cast<E *>(&m_error_storage));
    }
    return decltype(f(std::declval<E &>()))(
        std::move(*reinterpret_cast<T *>(&m_storage)));
  }

  template <typename F>
  constexpr auto
  or_else(F &&f) const & -> decltype(f(std::declval<const E &>())) {
    if (!m_has_value) {
      return f(*reinterpret_cast<const E *>(&m_error_storage));
    }
    return decltype(f(std::declval<const E &>()))(
        *reinterpret_cast<const T *>(&m_storage));
  }

  template <typename F>
  constexpr auto
  transform(F &&f) & -> expected<decltype(f(std::declval<T &>())), E> {
    if (m_has_value) {
      return expected<decltype(f(std::declval<T &>())), E>(
          f(*reinterpret_cast<T *>(&m_storage)));
    }
    return expected<decltype(f(std::declval<T &>())), E>(
        unexpected<E>(std::move(*reinterpret_cast<E *>(&m_error_storage))));
  }

  template <typename F>
  constexpr auto transform(
      F &&f) const & -> expected<decltype(f(std::declval<const T &>())), E> {
    if (m_has_value) {
      return expected<decltype(f(std::declval<const T &>())), E>(
          f(*reinterpret_cast<const T *>(&m_storage)));
    }
    return expected<decltype(f(std::declval<const T &>())), E>(
        unexpected<E>(*reinterpret_cast<const E *>(&m_error_storage)));
  }

  template <typename F>
  constexpr auto
  transform_error(F &&f) & -> expected<T, decltype(f(std::declval<E &>()))> {
    if (!m_has_value) {
      return expected<T, decltype(f(std::declval<E &>()))>(
          unexpect, f(*reinterpret_cast<E *>(&m_error_storage)));
    }
    return expected<T, decltype(f(std::declval<E &>()))>(
        std::move(*reinterpret_cast<T *>(&m_storage)));
  }

  template <typename F>
  constexpr auto transform_error(
      F &&f) const & -> expected<T, decltype(f(std::declval<const E &>()))> {
    if (!m_has_value) {
      return expected<T, decltype(f(std::declval<const E &>()))>(
          unexpect, f(*reinterpret_cast<const E *>(&m_error_storage)));
    }
    return expected<T, decltype(f(std::declval<const E &>()))>(
        *reinterpret_cast<const T *>(&m_storage));
  }

private:
  alignas(T) unsigned char m_storage[sizeof(T)];
  alignas(E) unsigned char m_error_storage[sizeof(E)];
  bool m_has_value;
};

/** @brief Specialization of expected for void value type.
 *
 * Represents an operation that either succeeds (void) or fails with error E.
 * Used for operations that do not return a value but can still fail,
 * such as semaphore give or queue send.
 *
 * @tparam E The error type on failure (typically freertos::error)
 */
template <typename E> class expected<void, E> {
public:
  constexpr expected() noexcept : m_has_value(true) {}

  constexpr expected(const unexpected<E> &err) : m_has_value(false) {
    new (&m_error_storage) E(err.value());
  }

  constexpr expected(unexpected<E> &&err) : m_has_value(false) {
    new (&m_error_storage) E(std::move(err.value()));
  }

  constexpr expected(unexpect_t, const E &err) : m_has_value(false) {
    new (&m_error_storage) E(err);
  }

  constexpr expected(unexpect_t, E &&err) : m_has_value(false) {
    new (&m_error_storage) E(std::move(err));
  }

  ~expected() {
    if (!m_has_value) {
      reinterpret_cast<E *>(&m_error_storage)->~E();
    }
  }

  constexpr expected(const expected &other) : m_has_value(other.m_has_value) {
    if (!m_has_value) {
      new (&m_error_storage)
          E(*reinterpret_cast<const E *>(&other.m_error_storage));
    }
  }

  constexpr expected(expected &&other) noexcept
      : m_has_value(other.m_has_value) {
    if (!m_has_value) {
      new (&m_error_storage)
          E(std::move(*reinterpret_cast<E *>(&other.m_error_storage)));
    }
  }

  constexpr expected &operator=(const expected &other) {
    if (this != &other) {
      if (!m_has_value) {
        reinterpret_cast<E *>(&m_error_storage)->~E();
      }
      m_has_value = other.m_has_value;
      if (!m_has_value) {
        new (&m_error_storage)
            E(*reinterpret_cast<const E *>(&other.m_error_storage));
      }
    }
    return *this;
  }

  constexpr expected &operator=(expected &&other) noexcept {
    if (this != &other) {
      if (!m_has_value) {
        reinterpret_cast<E *>(&m_error_storage)->~E();
      }
      m_has_value = other.m_has_value;
      if (!m_has_value) {
        new (&m_error_storage)
            E(std::move(*reinterpret_cast<E *>(&other.m_error_storage)));
      }
    }
    return *this;
  }

  constexpr bool has_value() const noexcept { return m_has_value; }
  constexpr explicit operator bool() const noexcept { return m_has_value; }

  constexpr E &error() & {
    configASSERT(!m_has_value);
    return *reinterpret_cast<E *>(&m_error_storage);
  }

  constexpr const E &error() const & {
    configASSERT(!m_has_value);
    return *reinterpret_cast<const E *>(&m_error_storage);
  }

  constexpr E &&error() && {
    configASSERT(!m_has_value);
    return std::move(*reinterpret_cast<E *>(&m_error_storage));
  }

  template <typename F> constexpr auto and_then(F &&f) & -> decltype(f()) {
    if (m_has_value) {
      return f();
    }
    return decltype(f())(
        unexpected<E>(std::move(*reinterpret_cast<E *>(&m_error_storage))));
  }

  template <typename F>
  constexpr auto and_then(F &&f) const & -> decltype(f()) {
    if (m_has_value) {
      return f();
    }
    return decltype(f())(
        unexpected<E>(*reinterpret_cast<const E *>(&m_error_storage)));
  }

  template <typename F>
  constexpr auto or_else(F &&f) & -> decltype(f(std::declval<E &>())) {
    if (!m_has_value) {
      return f(*reinterpret_cast<E *>(&m_error_storage));
    }
    return decltype(f(std::declval<E &>()))();
  }

  template <typename F>
  constexpr auto
  or_else(F &&f) const & -> decltype(f(std::declval<const E &>())) {
    if (!m_has_value) {
      return f(*reinterpret_cast<const E *>(&m_error_storage));
    }
    return decltype(f(std::declval<const E &>()))();
  }

  template <typename F>
  constexpr auto transform(F &&f) & -> expected<decltype(f()), E> {
    if (m_has_value) {
      return expected<decltype(f()), E>(f());
    }
    return expected<decltype(f()), E>(
        unexpected<E>(std::move(*reinterpret_cast<E *>(&m_error_storage))));
  }

  template <typename F>
  constexpr auto transform(F &&f) const & -> expected<decltype(f()), E> {
    if (m_has_value) {
      return expected<decltype(f()), E>(f());
    }
    return expected<decltype(f()), E>(
        unexpected<E>(*reinterpret_cast<const E *>(&m_error_storage)));
  }

  template <typename F>
  constexpr auto
  transform_error(F &&f) & -> expected<void, decltype(f(std::declval<E &>()))> {
    if (!m_has_value) {
      return expected<void, decltype(f(std::declval<E &>()))>(
          unexpect, f(*reinterpret_cast<E *>(&m_error_storage)));
    }
    return expected<void, decltype(f(std::declval<E &>()))>();
  }

  template <typename F>
  constexpr auto transform_error(
      F &&f) const & -> expected<void, decltype(f(std::declval<const E &>()))> {
    if (!m_has_value) {
      return expected<void, decltype(f(std::declval<const E &>()))>(
          unexpect, f(*reinterpret_cast<const E *>(&m_error_storage)));
    }
    return expected<void, decltype(f(std::declval<const E &>()))>();
  }

private:
  alignas(E) unsigned char m_error_storage[sizeof(E)];
  bool m_has_value;
};

} // namespace freertos

#endif