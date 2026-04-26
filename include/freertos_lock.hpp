/**********************************************************************************
@file freertos_lock.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS shared_lock and Lockable concept compliance utilities
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
***********************************************************************************/

#pragma once

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#include "freertos_config.hpp"
#include "freertos_thread_safety.hpp"
#include <chrono>
#include <mutex>
#include <type_traits>
#include <utility>

namespace freertos {

template <typename Mutex> class FREERTOS_SCOPED_CAPABILITY shared_lock {
  Mutex *m_mutex{nullptr};
  bool m_owns{false};

public:
  using mutex_type = Mutex;

  shared_lock() noexcept = default;

  explicit shared_lock(Mutex &m) FREERTOS_ACQUIRE_SHARED()
      : m_mutex{std::addressof(m)}, m_owns{true} {
    m_mutex->lock_shared();
  }

  shared_lock(Mutex &m, std::defer_lock_t) noexcept
      : m_mutex{std::addressof(m)}, m_owns{false} {}

  shared_lock(Mutex &m, std::try_to_lock_t)
      : m_mutex{std::addressof(m)},
        m_owns{m.try_lock_shared()} {}

  shared_lock(Mutex &m, std::adopt_lock_t) noexcept
      : m_mutex{std::addressof(m)}, m_owns{true} {}

  template <typename Rep, typename Period>
  shared_lock(Mutex &m, const std::chrono::duration<Rep, Period> &timeout)
      : m_mutex{std::addressof(m)},
        m_owns{m.try_lock_shared_for(timeout)} {}

  shared_lock(shared_lock &&other) noexcept
      : m_mutex{other.m_mutex}, m_owns{other.m_owns} {
    other.m_mutex = nullptr;
    other.m_owns = false;
  }

  shared_lock &operator=(shared_lock &&other) noexcept {
    if (this != &other) {
      if (m_owns) {
        m_mutex->unlock_shared();
      }
      m_mutex = other.m_mutex;
      m_owns = other.m_owns;
      other.m_mutex = nullptr;
      other.m_owns = false;
    }
    return *this;
  }

  shared_lock(const shared_lock &) = delete;
  shared_lock &operator=(const shared_lock &) = delete;

  ~shared_lock() FREERTOS_RELEASE_SHARED() {
    if (m_owns && m_mutex) {
      m_mutex->unlock_shared();
    }
  }

  void lock() FREERTOS_ACQUIRE_SHARED() {
    configASSERT(m_mutex != nullptr);
    if (m_mutex && !m_owns) {
      m_mutex->lock_shared();
      m_owns = true;
    }
  }

  bool try_lock() FREERTOS_TRY_ACQUIRE_SHARED(true) {
    if (m_mutex && !m_owns) {
      m_owns = m_mutex->try_lock_shared();
    }
    return m_owns;
  }

  template <typename Rep, typename Period>
  bool try_lock_for(const std::chrono::duration<Rep, Period> &timeout) {
    if (m_mutex && !m_owns) {
      m_owns = m_mutex->try_lock_shared_for(timeout);
    }
    return m_owns;
  }

  template <typename Clock, typename Duration>
  bool
  try_lock_until(const std::chrono::time_point<Clock, Duration> &tp) {
    if (m_mutex && !m_owns) {
      m_owns = m_mutex->try_lock_shared_until(tp);
    }
    return m_owns;
  }

  void unlock() FREERTOS_RELEASE_SHARED() {
    if (m_owns && m_mutex) {
      m_mutex->unlock_shared();
      m_owns = false;
    }
  }

  bool owns_lock() const noexcept { return m_owns; }

  explicit operator bool() const noexcept { return m_owns; }

  Mutex *mutex() const noexcept { return m_mutex; }

  Mutex *release() noexcept {
    auto *m = m_mutex;
    m_mutex = nullptr;
    m_owns = false;
    return m;
  }

  void swap(shared_lock &other) noexcept {
    using std::swap;
    swap(m_mutex, other.m_mutex);
    swap(m_owns, other.m_owns);
  }
};

template <typename Mutex>
void swap(shared_lock<Mutex> &a, shared_lock<Mutex> &b) noexcept {
  a.swap(b);
}

template <typename Mutex> class FREERTOS_SCOPED_CAPABILITY unique_lock {
  Mutex *m_mutex{nullptr};
  bool m_owns{false};

public:
  using mutex_type = Mutex;

  unique_lock() noexcept = default;

  explicit unique_lock(Mutex &m) FREERTOS_ACQUIRE()
      : m_mutex{std::addressof(m)}, m_owns{true} {
    m_mutex->lock();
  }

  unique_lock(Mutex &m, std::defer_lock_t) noexcept
      : m_mutex{std::addressof(m)}, m_owns{false} {}

  unique_lock(Mutex &m, std::try_to_lock_t)
      : m_mutex{std::addressof(m)},
        m_owns{static_cast<bool>(m.try_lock())} {}

  unique_lock(Mutex &m, std::adopt_lock_t) noexcept
      : m_mutex{std::addressof(m)}, m_owns{true} {}

  template <typename Rep, typename Period>
  unique_lock(Mutex &m, const std::chrono::duration<Rep, Period> &timeout)
      : m_mutex{std::addressof(m)},
        m_owns{m.try_lock_for(timeout)} {}

  template <typename Clock, typename Duration>
  unique_lock(Mutex &m, const std::chrono::time_point<Clock, Duration> &tp)
      : m_mutex{std::addressof(m)},
        m_owns{m.try_lock_until(tp)} {}

  unique_lock(unique_lock &&other) noexcept
      : m_mutex{other.m_mutex}, m_owns{other.m_owns} {
    other.m_mutex = nullptr;
    other.m_owns = false;
  }

  unique_lock &operator=(unique_lock &&other) noexcept {
    if (this != &other) {
      if (m_owns && m_mutex) {
        m_mutex->unlock();
      }
      m_mutex = other.m_mutex;
      m_owns = other.m_owns;
      other.m_mutex = nullptr;
      other.m_owns = false;
    }
    return *this;
  }

  unique_lock(const unique_lock &) = delete;
  unique_lock &operator=(const unique_lock &) = delete;

  ~unique_lock() FREERTOS_RELEASE() {
    if (m_owns && m_mutex) {
      m_mutex->unlock();
    }
  }

  void lock() FREERTOS_ACQUIRE() {
    configASSERT(m_mutex != nullptr);
    if (m_mutex && !m_owns) {
      m_mutex->lock();
      m_owns = true;
    }
  }

  bool try_lock() FREERTOS_TRY_ACQUIRE(true) {
    if (m_mutex && !m_owns) {
      m_owns = static_cast<bool>(m_mutex->try_lock());
    }
    return m_owns;
  }

  template <typename Rep, typename Period>
  bool try_lock_for(const std::chrono::duration<Rep, Period> &timeout) {
    if (m_mutex && !m_owns) {
      m_owns = m_mutex->try_lock_for(timeout);
    }
    return m_owns;
  }

  template <typename Clock, typename Duration>
  bool
  try_lock_until(const std::chrono::time_point<Clock, Duration> &tp) {
    if (m_mutex && !m_owns) {
      m_owns = m_mutex->try_lock_until(tp);
    }
    return m_owns;
  }

  void unlock() FREERTOS_RELEASE() {
    if (m_owns && m_mutex) {
      m_mutex->unlock();
      m_owns = false;
    }
  }

  bool owns_lock() const noexcept { return m_owns; }

  explicit operator bool() const noexcept { return m_owns; }

  Mutex *mutex() const noexcept { return m_mutex; }

  Mutex *release() noexcept {
    auto *m = m_mutex;
    m_mutex = nullptr;
    m_owns = false;
    return m;
  }

  void swap(unique_lock &other) noexcept {
    using std::swap;
    swap(m_mutex, other.m_mutex);
    swap(m_owns, other.m_owns);
  }
};

template <typename Mutex>
void swap(unique_lock<Mutex> &a, unique_lock<Mutex> &b) noexcept {
  a.swap(b);
}

#if FREERTOS_CPP_WRAPPERS_CPP20

template <typename M>
concept BasicLockable = requires(M &m) {
  { m.lock() } -> std::same_as<void>;
  { m.unlock() } -> std::same_as<void>;
};

template <typename M>
concept Lockable = BasicLockable<M> && requires(M &m) {
  { m.try_lock() } -> std::convertible_to<bool>;
};

template <typename M>
concept TimedLockable = Lockable<M> && requires(M &m) {
  { m.try_lock_for(std::declval<std::chrono::milliseconds>()) } -> std::convertible_to<bool>;
  { m.try_lock_until(std::declval<std::chrono::steady_clock::time_point>()) } -> std::convertible_to<bool>;
};

template <typename M>
concept SharedLockable = requires(M &m) {
  { m.lock_shared() } -> std::same_as<void>;
  { m.unlock_shared() } -> std::same_as<void>;
  { m.try_lock_shared() } -> std::convertible_to<bool>;
};

template <typename M>
concept SharedTimedLockable = SharedLockable<M> && requires(M &m) {
  { m.try_lock_shared_for(std::declval<std::chrono::milliseconds>()) } -> std::convertible_to<bool>;
  { m.try_lock_shared_until(std::declval<std::chrono::steady_clock::time_point>()) } -> std::convertible_to<bool>;
};

#if configSUPPORT_DYNAMIC_ALLOCATION
static_assert(BasicLockable<freertos::mutex<freertos::dynamic_semaphore_allocator>>);
static_assert(Lockable<freertos::mutex<freertos::dynamic_semaphore_allocator>>);
static_assert(TimedLockable<freertos::mutex<freertos::dynamic_semaphore_allocator>>);
static_assert(BasicLockable<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>>);
static_assert(Lockable<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>>);
static_assert(TimedLockable<freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>>);

#if FREERTOS_CPP_WRAPPERS_HAS_MUTEX && FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE
static_assert(BasicLockable<freertos::shared_mutex<freertos::dynamic_shared_mutex_allocator>>);
static_assert(Lockable<freertos::shared_mutex<freertos::dynamic_shared_mutex_allocator>>);
static_assert(TimedLockable<freertos::shared_mutex<freertos::dynamic_shared_mutex_allocator>>);
static_assert(SharedLockable<freertos::shared_mutex<freertos::dynamic_shared_mutex_allocator>>);
static_assert(SharedTimedLockable<freertos::shared_mutex<freertos::dynamic_shared_mutex_allocator>>);
#endif
#endif

#endif

} // namespace freertos