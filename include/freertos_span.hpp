/**********************************************************************************
@file freertos_span.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief std::span polyfill for FreeRTOS C++ Wrappers
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

#if defined(__cpp_lib_span) && __cpp_lib_span >= 202002L

#include <span>

namespace freertos {

inline constexpr size_t dynamic_extent = std::dynamic_extent;

template <typename T, size_t Extent = std::dynamic_extent>
using span = std::span<T, Extent>;

} // namespace freertos

#else

#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace freertos {

inline constexpr size_t dynamic_extent = static_cast<size_t>(-1);

template <typename T, size_t Extent = dynamic_extent> class span {
public:
  using element_type = T;
  using value_type = typename std::remove_cv<T>::type;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using iterator = pointer;

  constexpr span() noexcept : _data(nullptr), _size(0) {}

  constexpr span(pointer ptr, size_type count) noexcept
      : _data(ptr), _size(count) {}

  template <size_t N>
  constexpr span(T (&arr)[N]) noexcept : _data(arr), _size(N) {}

  template <size_t N>
  constexpr span(std::array<T, N> &arr) noexcept
      : _data(arr.data()), _size(N) {}

  template <size_t N>
  constexpr span(
      const std::array<typename std::remove_cv<T>::type, N> &arr) noexcept
      : _data(arr.data()), _size(N) {}

  template <
      typename U,
      typename = typename std::enable_if<
          std::is_const<T>::value &&
          std::is_same<typename std::remove_const<T>::type, U>::value>::type>
  constexpr span(const span<U> &other) noexcept
      : _data(other.data()), _size(other.size()) {}

  constexpr pointer data() const noexcept { return _data; }
  constexpr size_type size() const noexcept { return _size; }
  constexpr size_type size_bytes() const noexcept {
    return _size * sizeof(element_type);
  }
  constexpr bool empty() const noexcept { return _size == 0; }

  constexpr reference operator[](size_type idx) const { return _data[idx]; }
  constexpr reference front() const { return _data[0]; }
  constexpr reference back() const { return _data[_size - 1]; }

  constexpr iterator begin() const noexcept { return _data; }
  constexpr iterator end() const noexcept { return _data + _size; }

  constexpr span<T> first(size_type count) const {
    return span<T>(_data, count);
  }
  constexpr span<T> last(size_type count) const {
    return span<T>(_data + _size - count, count);
  }
  constexpr span<T> subspan(size_type offset,
                            size_type count = dynamic_extent) const {
    return span<T>(_data + offset,
                   count == dynamic_extent ? _size - offset : count);
  }

private:
  pointer _data;
  size_type _size;
};

template <typename T, size_t N> span(std::array<T, N> &) -> span<T, N>;

template <typename T, size_t N> span(T (&)[N]) -> span<T, N>;

} // namespace freertos

#endif