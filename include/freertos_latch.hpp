/**********************************************************************************
@file freertos_latch.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS latch wrapper providing C++20 std::latch semantics
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
***********************************************************************************/

#pragma once

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#include "freertos_expected.hpp"
#include "freertos_isr_result.hpp"
#include <FreeRTOS.h>
#include <atomic>
#include <cstddef>
#include <semphr.h>

namespace freertos {

#ifndef FREERTOS_CPP_WRAPPERS_LATCH_MAX_WAITERS
#define FREERTOS_CPP_WRAPPERS_LATCH_MAX_WAITERS 8
#endif

class latch {
  static_assert(std::atomic<ptrdiff_t>::is_always_lock_free,
                "latch requires lock-free atomic for ISR safety");

public:
  static constexpr ptrdiff_t max() noexcept { return PTRDIFF_MAX; }

  explicit latch(ptrdiff_t expected)
      : m_semaphore{xSemaphoreCreateBinary()},
        m_counter{expected} {
    configASSERT(m_semaphore);
  }

  [[nodiscard]] bool valid() const noexcept { return m_semaphore != nullptr; }

  latch(const latch &) = delete;
  latch &operator=(const latch &) = delete;

  ~latch() noexcept {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  void count_down(ptrdiff_t update = 1) {
    if (update <= 0) {
      return;
    }
    ptrdiff_t prev;
    ptrdiff_t next;
    do {
      prev = m_counter.load(std::memory_order_acquire);
      if (prev <= 0) {
        return;
      }
      next = prev > update ? prev - update : 0;
    } while (!m_counter.compare_exchange_weak(prev, next,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire));
    if (next == 0 && prev > 0) {
      if (m_semaphore) {
        xSemaphoreGive(m_semaphore);
      }
    }
  }

  [[nodiscard]] bool try_wait() const noexcept {
    return m_counter.load(std::memory_order_acquire) == 0;
  }

  void wait() const {
    if (m_counter.load(std::memory_order_acquire) == 0) {
      return;
    }
    if (!m_semaphore) {
      return;
    }
    xSemaphoreTake(m_semaphore, portMAX_DELAY);
    xSemaphoreGive(m_semaphore);
  }

  void arrive_and_wait(ptrdiff_t update = 1) {
    count_down(update);
    wait();
  }

  isr_result<void> count_down_isr(ptrdiff_t update = 1) {
    if (update <= 0) {
      return {pdFALSE};
    }
    ptrdiff_t prev;
    ptrdiff_t next;
    do {
      prev = m_counter.load(std::memory_order_acquire);
      if (prev <= 0) {
        return {pdFALSE};
      }
      next = prev > update ? prev - update : 0;
    } while (!m_counter.compare_exchange_weak(prev, next,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire));
    isr_result<void> result{pdFALSE};
    if (next == 0 && prev > 0) {
      if (m_semaphore) {
        xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
      }
    }
    return result;
  }

  [[nodiscard]] expected<void, error> count_down_ex(ptrdiff_t update = 1) {
    if (update <= 0) {
      return {};
    }
    ptrdiff_t prev;
    ptrdiff_t next;
    do {
      prev = m_counter.load(std::memory_order_acquire);
      if (prev <= 0) {
        return {};
      }
      next = prev > update ? prev - update : 0;
    } while (!m_counter.compare_exchange_weak(prev, next,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire));
    if (next == 0 && prev > 0) {
      if (!m_semaphore) {
        return unexpected<error>(error::invalid_handle);
      }
      if (xSemaphoreGive(m_semaphore) != pdTRUE) {
        return unexpected<error>(error::semaphore_not_owned);
      }
    }
    return {};
  }

  [[nodiscard]] isr_result<expected<void, error>>
  count_down_ex_isr(ptrdiff_t update = 1) {
    if (update <= 0) {
      return {{}, pdFALSE};
    }
    ptrdiff_t prev;
    ptrdiff_t next;
    do {
      prev = m_counter.load(std::memory_order_acquire);
      if (prev <= 0) {
        return {{}, pdFALSE};
      }
      next = prev > update ? prev - update : 0;
    } while (!m_counter.compare_exchange_weak(prev, next,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire));
    if (next == 0 && prev > 0) {
      if (!m_semaphore) {
        return {unexpected<error>(error::invalid_handle), pdFALSE};
      }
      BaseType_t woken = pdFALSE;
      if (xSemaphoreGiveFromISR(m_semaphore, &woken) != pdTRUE) {
        return {unexpected<error>(error::semaphore_not_owned), woken};
      }
      return {{}, woken};
    }
    return {{}, pdFALSE};
  }

private:
  mutable SemaphoreHandle_t m_semaphore{nullptr};
  std::atomic<ptrdiff_t> m_counter{0};
};

#if configSUPPORT_STATIC_ALLOCATION
namespace sa {

template <ptrdiff_t N = 1> class latch_static {
  static_assert(N > 0, "latch expected count must be positive");
  static_assert(std::atomic<ptrdiff_t>::is_always_lock_free,
                "latch requires lock-free atomic for ISR safety");

  StaticSemaphore_t m_semaphore_placeholder{};
  SemaphoreHandle_t m_semaphore{nullptr};
  std::atomic<ptrdiff_t> m_counter{N};

public:
  static constexpr ptrdiff_t max() noexcept { return PTRDIFF_MAX; }

  latch_static()
      : m_semaphore{xSemaphoreCreateBinaryStatic(&m_semaphore_placeholder)} {
    configASSERT(m_semaphore);
  }

  [[nodiscard]] bool valid() const noexcept { return m_semaphore != nullptr; }

  latch_static(const latch_static &) = delete;
  latch_static &operator=(const latch_static &) = delete;

  ~latch_static() noexcept {
    if (m_semaphore) {
      vSemaphoreDelete(m_semaphore);
    }
  }

  void count_down(ptrdiff_t update = 1) {
    if (update <= 0) {
      return;
    }
    ptrdiff_t prev;
    ptrdiff_t next;
    do {
      prev = m_counter.load(std::memory_order_acquire);
      if (prev <= 0) {
        return;
      }
      next = prev > update ? prev - update : 0;
    } while (!m_counter.compare_exchange_weak(prev, next,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire));
    if (next == 0 && prev > 0) {
      xSemaphoreGive(m_semaphore);
    }
  }

  [[nodiscard]] bool try_wait() const noexcept {
    return m_counter.load(std::memory_order_acquire) == 0;
  }

  void wait() const {
    if (m_counter.load(std::memory_order_acquire) == 0) {
      return;
    }
    xSemaphoreTake(m_semaphore, portMAX_DELAY);
    xSemaphoreGive(m_semaphore);
  }

  void arrive_and_wait(ptrdiff_t update = 1) {
    count_down(update);
    wait();
  }

  isr_result<void> count_down_isr(ptrdiff_t update = 1) {
    if (update <= 0) {
      return {pdFALSE};
    }
    ptrdiff_t prev;
    ptrdiff_t next;
    do {
      prev = m_counter.load(std::memory_order_acquire);
      if (prev <= 0) {
        return {pdFALSE};
      }
      next = prev > update ? prev - update : 0;
    } while (!m_counter.compare_exchange_weak(prev, next,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire));
    isr_result<void> result{pdFALSE};
    if (next == 0 && prev > 0) {
      xSemaphoreGiveFromISR(m_semaphore, &result.higher_priority_task_woken);
    }
    return result;
  }

  [[nodiscard]] expected<void, error> count_down_ex(ptrdiff_t update = 1) {
    if (update <= 0) {
      return {};
    }
    ptrdiff_t prev;
    ptrdiff_t next;
    do {
      prev = m_counter.load(std::memory_order_acquire);
      if (prev <= 0) {
        return {};
      }
      next = prev > update ? prev - update : 0;
    } while (!m_counter.compare_exchange_weak(prev, next,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire));
    if (next == 0 && prev > 0) {
      if (xSemaphoreGive(m_semaphore) != pdTRUE) {
        return unexpected<error>(error::semaphore_not_owned);
      }
    }
    return {};
  }

  [[nodiscard]] isr_result<expected<void, error>>
  count_down_ex_isr(ptrdiff_t update = 1) {
    if (update <= 0) {
      return {{}, pdFALSE};
    }
    ptrdiff_t prev;
    ptrdiff_t next;
    do {
      prev = m_counter.load(std::memory_order_acquire);
      if (prev <= 0) {
        return {{}, pdFALSE};
      }
      next = prev > update ? prev - update : 0;
    } while (!m_counter.compare_exchange_weak(prev, next,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire));
    if (next == 0 && prev > 0) {
      BaseType_t woken = pdFALSE;
      if (xSemaphoreGiveFromISR(m_semaphore, &woken) != pdTRUE) {
        return {unexpected<error>(error::semaphore_not_owned), woken};
      }
      return {{}, woken};
    }
    return {{}, pdFALSE};
  }
};

using latch = latch_static<1>;

} // namespace sa
#endif

#if configSUPPORT_DYNAMIC_ALLOCATION
namespace da {

using latch = freertos::latch;

} // namespace da
#endif

} // namespace freertos
