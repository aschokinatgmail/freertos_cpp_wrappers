/**********************************************************************************
@file freertos_external_threading.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief libc++ __external_threading backend for Clang/LLVM standard library
@version 3.1.0
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

#if FREERTOS_CPP_WRAPPERS_ENABLE_EXTERNAL_THREADING

#include "freertos_config.hpp"
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#if __STDC_HOSTED__
#include <chrono>
#include <cstring>
#else
#include <stdint.h>
#include <string.h>
#endif

#define _LIBCPP_HAS_THREADS 1
#define _LIBCPP_HAS_THREAD_API_EXTERNAL
#define _LIBCPP_HAS_MONOTONIC_CLOCK 1

#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
#define _LIBCPP_HAS_TRIVIAL_MUTEX_DESTRUCTION 0
#define _LIBCPP_HAS_TRIVIAL_CONDVAR_DESTRUCTION 0
#else
#define _LIBCPP_HAS_TRIVIAL_MUTEX_DESTRUCTION 1
#define _LIBCPP_HAS_TRIVIAL_CONDVAR_DESTRUCTION 1
#endif

namespace std {

typedef struct __libcpp_mutex_t {
  StaticSemaphore_t storage;
  SemaphoreHandle_t handle;
  bool initialized;
} __libcpp_mutex_t;

typedef struct __libcpp_recursive_mutex_t {
  StaticSemaphore_t storage;
  SemaphoreHandle_t handle;
  bool initialized;
} __libcpp_recursive_mutex_t;

#if FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL == 1

typedef struct __libcpp_condvar_t {
  SemaphoreHandle_t signal;
  volatile int waiter_count;
} __libcpp_condvar_t;

#define _LIBCPP_CONDVAR_INITIALIZER \
  { nullptr, 0 }

#elif FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL == 2

typedef struct __libcpp_condvar_t {
  TaskHandle_t waiting_task;
  volatile uint8_t signaled;
} __libcpp_condvar_t;

#define _LIBCPP_CONDVAR_INITIALIZER \
  { nullptr, 0 }

#else

typedef struct __libcpp_condvar_t {
  SemaphoreHandle_t internal_mutex;
  struct waiter_node {
    TaskHandle_t task;
    SemaphoreHandle_t semaphore;
    struct waiter_node *next;
  } * waiters;
  int max_waiters;
} __libcpp_condvar_t;

#define _LIBCPP_CONDVAR_INITIALIZER \
  { nullptr, nullptr, 0 }

#endif

typedef volatile uint8_t __libcpp_exec_once_flag;

#define _LIBCPP_EXEC_ONCE_INITIALIZER 0

typedef TaskHandle_t __libcpp_thread_id;

typedef TaskHandle_t __libcpp_thread_t;

typedef uint32_t __libcpp_tls_key;

typedef struct timespec __libcpp_timespec_t;

#define _LIBCPP_MUTEX_INITIALIZER \
  { {}, nullptr, false }
#define _LIBCPP_NULL_THREAD nullptr

inline int __libcpp_mutex_lock(__libcpp_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle = xSemaphoreCreateMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateMutex();
#endif
    mutex->initialized = true;
  }
  return xSemaphoreTake(mutex->handle, portMAX_DELAY) == pdTRUE ? 0 : 1;
}

inline bool __libcpp_mutex_trylock(__libcpp_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle = xSemaphoreCreateMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateMutex();
#endif
    mutex->initialized = true;
  }
  return xSemaphoreTake(mutex->handle, 0) == pdTRUE;
}

inline int __libcpp_mutex_unlock(__libcpp_mutex_t *mutex) {
  return xSemaphoreGive(mutex->handle) == pdTRUE ? 0 : 1;
}

inline int __libcpp_mutex_destroy(__libcpp_mutex_t *mutex) {
  if (mutex->initialized && mutex->handle) {
    vSemaphoreDelete(mutex->handle);
    mutex->handle = nullptr;
    mutex->initialized = false;
  }
  return 0;
}

inline int
__libcpp_recursive_mutex_init(__libcpp_recursive_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle =
        xSemaphoreCreateRecursiveMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateRecursiveMutex();
#endif
    mutex->initialized = true;
  }
  return 0;
}

inline int
__libcpp_recursive_mutex_lock(__libcpp_recursive_mutex_t *mutex) {
  if (!mutex->initialized) {
    __libcpp_recursive_mutex_init(mutex);
  }
  return xSemaphoreTakeRecursive(mutex->handle, portMAX_DELAY) == pdTRUE
             ? 0
             : 1;
}

inline bool
__libcpp_recursive_mutex_trylock(__libcpp_recursive_mutex_t *mutex) {
  if (!mutex->initialized) {
    __libcpp_recursive_mutex_init(mutex);
  }
  return xSemaphoreTakeRecursive(mutex->handle, 0) == pdTRUE;
}

inline int
__libcpp_recursive_mutex_unlock(__libcpp_recursive_mutex_t *mutex) {
  return xSemaphoreGiveRecursive(mutex->handle) == pdTRUE ? 0 : 1;
}

inline int
__libcpp_recursive_mutex_destroy(__libcpp_recursive_mutex_t *mutex) {
  if (mutex->initialized && mutex->handle) {
    vSemaphoreDelete(mutex->handle);
    mutex->handle = nullptr;
    mutex->initialized = false;
  }
  return 0;
}

#if FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL == 1

inline int __libcpp_condvar_signal(__libcpp_condvar_t *cv) {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  if (cv->waiter_count > 0) {
    xSemaphoreGive(cv->signal);
  }
  return 0;
}

inline int __libcpp_condvar_broadcast(__libcpp_condvar_t *cv) {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  int count = cv->waiter_count;
  for (int i = 0; i < count; i++) {
    xSemaphoreGive(cv->signal);
  }
  return 0;
}

inline int __libcpp_condvar_wait(__libcpp_condvar_t *cv,
                                  __libcpp_mutex_t *mutex) {
  cv->waiter_count++;
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  __libcpp_mutex_unlock(mutex);
  xSemaphoreTake(cv->signal, portMAX_DELAY);
  cv->waiter_count--;
  __libcpp_mutex_lock(mutex);
  return 0;
}

inline int
__libcpp_condvar_timedwait(__libcpp_condvar_t *cv,
                            __libcpp_mutex_t *mutex,
                            const struct timespec *ts) {
  (void)ts;
  return __libcpp_condvar_wait(cv, mutex);
}

inline int __libcpp_condvar_destroy(__libcpp_condvar_t *cv) {
  if (cv->signal) {
    vSemaphoreDelete(cv->signal);
    cv->signal = nullptr;
  }
  return 0;
}

#elif FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL == 2

inline int __libcpp_condvar_signal(__libcpp_condvar_t *cv) {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  if (cv->waiting_task) {
    vTaskNotifyGiveFromISR(cv->waiting_task, nullptr);
    cv->waiting_task = nullptr;
  }
  return 0;
}

inline int __libcpp_condvar_broadcast(__libcpp_condvar_t *cv) {
  return __libcpp_condvar_signal(cv);
}

inline int __libcpp_condvar_wait(__libcpp_condvar_t *cv,
                                  __libcpp_mutex_t *mutex) {
  cv->waiting_task = xTaskGetCurrentTaskHandle();
  cv->signaled = 0;
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  __libcpp_mutex_unlock(mutex);
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  __libcpp_mutex_lock(mutex);
  return 0;
}

inline int
__libcpp_condvar_timedwait(__libcpp_condvar_t *cv,
                            __libcpp_mutex_t *mutex,
                            const struct timespec *ts) {
  (void)ts;
  return __libcpp_condvar_wait(cv, mutex);
}

inline int __libcpp_condvar_destroy(__libcpp_condvar_t *cv) {
  cv->waiting_task = nullptr;
  cv->signaled = 0;
  return 0;
}

#else

inline int __libcpp_condvar_signal(__libcpp_condvar_t *cv) {
  (void)cv;
  return 0;
}

inline int __libcpp_condvar_broadcast(__libcpp_condvar_t *cv) {
  (void)cv;
  return 0;
}

inline int __libcpp_condvar_wait(__libcpp_condvar_t *cv,
                                  __libcpp_mutex_t *mutex) {
  (void)cv;
  __libcpp_mutex_unlock(mutex);
  vTaskDelay(1);
  __libcpp_mutex_lock(mutex);
  return 0;
}

inline int
__libcpp_condvar_timedwait(__libcpp_condvar_t *cv,
                            __libcpp_mutex_t *mutex,
                            const struct timespec *ts) {
  (void)ts;
  return __libcpp_condvar_wait(cv, mutex);
}

inline int __libcpp_condvar_destroy(__libcpp_condvar_t *cv) {
  (void)cv;
  return 0;
}

#endif

inline void __libcpp_execute_once(__libcpp_exec_once_flag *flag,
                                   void (*func)(void *)) {
  if (__atomic_load_n(flag, __ATOMIC_ACQUIRE) == 2) {
    return;
  }
  taskENTER_CRITICAL();
  if (*flag == 0) {
    *flag = 1;
    taskEXIT_CRITICAL();
    func(nullptr);
    __atomic_store_n(flag, 2, __ATOMIC_RELEASE);
  } else {
    taskEXIT_CRITICAL();
  }
}

namespace __et_detail {
struct __thread_trampoline {
  void *(*func)(void *);
  void *args;
};
static void __thread_entry(void *arg) {
  auto *t = static_cast<__thread_trampoline *>(arg);
  t->func(t->args);
  vPortFree(t);
  vTaskDelete(nullptr);
}
} // namespace __et_detail

inline bool __libcpp_thread_create(__libcpp_thread_t *thread,
                                    void *(*func)(void *), void *args) {
  auto *t = static_cast<__et_detail::__thread_trampoline *>(
      pvPortMalloc(sizeof(__et_detail::__thread_trampoline)));
  if (!t)
    return false;
  t->func = func;
  t->args = args;
  return xTaskCreate(__et_detail::__thread_entry, "lcxx",
                     configMINIMAL_STACK_SIZE, t, tskIDLE_PRIORITY + 1,
                     thread) == pdPASS;
}

inline bool __libcpp_thread_join(__libcpp_thread_t thread, void **value_ptr) {
  (void)value_ptr;
  eTaskState state;
  do {
    state = eTaskGetState(thread);
    if (state == eDeleted) {
      break;
    }
    vTaskDelay(1);
  } while (1);
  return true;
}

inline bool __libcpp_thread_detach(__libcpp_thread_t thread) {
  (void)thread;
  return true;
}

inline void __libcpp_thread_yield() { taskYIELD(); }

#if __STDC_HOSTED__
inline void
__libcpp_thread_sleep_for(const std::chrono::nanoseconds &ns) {
  TickType_t ticks = static_cast<TickType_t>(
      ns.count() * configTICK_RATE_HZ / 1000000000LL);
  if (ticks == 0)
    ticks = 1;
  vTaskDelay(ticks);
}
#else
inline void __libcpp_thread_sleep_for(uint32_t ms) {
  TickType_t ticks = static_cast<TickType_t>(
      ms * configTICK_RATE_HZ / 1000);
  if (ticks == 0)
    ticks = 1;
  vTaskDelay(ticks);
}
#endif

inline __libcpp_thread_id __libcpp_thread_get_id(__libcpp_thread_t thread) {
  return thread;
}

inline __libcpp_thread_id __libcpp_thread_get_current_id() {
  return xTaskGetCurrentTaskHandle();
}

inline bool __libcpp_thread_id_equal(__libcpp_thread_id t1,
                                      __libcpp_thread_id t2) {
  return t1 == t2;
}

inline bool __libcpp_thread_id_less(__libcpp_thread_id t1,
                                     __libcpp_thread_id t2) {
  return t1 < t2;
}

inline bool __libcpp_thread_isnull(__libcpp_thread_t thread) {
  return thread == nullptr;
}

inline int __libcpp_tls_create(__libcpp_tls_key *key,
                                void (*dtor)(void *)) {
  (void)dtor;
  static __libcpp_tls_key next_key = 0;
  if (next_key >= configNUM_THREAD_LOCAL_STORAGE_POINTERS) {
    return 1;
  }
  *key = next_key++;
  return 0;
}

inline void *__libcpp_tls_get(__libcpp_tls_key key) {
  return pvTaskGetThreadLocalStoragePointer(nullptr,
                                            static_cast<BaseType_t>(key));
}

inline void __libcpp_tls_set(__libcpp_tls_key key, void *ptr) {
  vTaskSetThreadLocalStoragePointer(nullptr, static_cast<BaseType_t>(key),
                                     ptr);
}

} // namespace std

#endif