
#include <FreeRTOS.h>

#include <array>
#include <semphr.h>
#include <utility>

#pragma once

namespace freertos {

template <typename T, size_t N> class pool {
  StaticSemaphore_t m_mtx_ph;
  SemaphoreHandle_t m_mtx;
  std::array<std::pair<bool, T>, N> m_pool;

public:
  pool() : m_mtx_ph{}, m_mtx{xSemaphoreCreateRecursiveMutexStatic(&m_mtx_ph)} {
    for (auto &p : m_pool) {
      p.first = false;
    }
  }
  pool(const pool &) = delete;
  pool(pool &&) = delete;
  ~pool() { vSemaphoreDelete(m_mtx); }

  pool &operator=(const pool &) = delete;
  pool &operator=(pool &&) = delete;

  T *allocate() {
    if (xSemaphoreTake(m_mtx, portMAX_DELAY) != pdTRUE) {
      return nullptr;
    }
    T *ptr = nullptr;
    for (auto &p : m_pool) {
      if (!p.first) {
        p.first = true;
        ptr = &p.second;
        break;
      }
    }
    xSemaphoreGive(m_mtx);
    return ptr;
  }
  bool deallocate(T *ptr) {
    if (xSemaphoreTake(m_mtx, portMAX_DELAY) != pdTRUE) {
      return false;
    }
    bool ret = false;
    for (auto &p : m_pool) {
      if (&p.second == ptr) {
        p.first = false;
        ret = true;
        break;
      }
    }
    xSemaphoreGive(m_mtx);
    return ret;
  }
};

} // namespace freertos
