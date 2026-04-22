/**********************************************************************************
@file freertos_shared_data.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief Thread-safe value wrapper for FreeRTOS C++ Wrappers
@version 3.1.0
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
#include "freertos_semaphore.hpp"
#include <type_traits>
#include <utility>

namespace freertos {

template <typename T, typename Mutex = freertos::mutex<freertos::dynamic_semaphore_allocator>>
class shared_data {
  T m_data{};
  mutable Mutex m_mutex;

public:
  shared_data() = default;

  explicit shared_data(const T &value) : m_data(value) {}

  explicit shared_data(T &&value) : m_data(std::move(value)) {}

  T get() const {
    m_mutex.lock();
    T result = m_data;
    m_mutex.unlock();
    return result;
  }

  void set(const T &value) {
    m_mutex.lock();
    m_data = value;
    m_mutex.unlock();
  }

  void set(T &&value) {
    m_mutex.lock();
    m_data = std::move(value);
    m_mutex.unlock();
  }

  template <typename Fn>
  auto use(Fn &&fn) -> decltype(fn(std::declval<T &>())) {
    m_mutex.lock();
    auto result = fn(m_data);
    m_mutex.unlock();
    return result;
  }

  [[nodiscard]] expected<T, error> get_ex() const {
    auto rc = m_mutex.lock_ex();
    if (!rc.has_value()) {
      return unexpected<error>(rc.error());
    }
    T result = m_data;
    m_mutex.unlock();
    return result;
  }

  [[nodiscard]] expected<void, error> set_ex(const T &value) {
    auto rc = m_mutex.lock_ex();
    if (!rc.has_value()) {
      return unexpected<error>(rc.error());
    }
    m_data = value;
    m_mutex.unlock();
    return {};
  }

  [[nodiscard]] expected<void, error> set_ex(T &&value) {
    auto rc = m_mutex.lock_ex();
    if (!rc.has_value()) {
      return unexpected<error>(rc.error());
    }
    m_data = std::move(value);
    m_mutex.unlock();
    return {};
  }
};

} // namespace freertos