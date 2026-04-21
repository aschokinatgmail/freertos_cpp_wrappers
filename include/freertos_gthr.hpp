/**********************************************************************************
@file freertos_gthr.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief GCC libstdc++ gthr-FreeRTOS threading backend
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

#if FREERTOS_CPP_WRAPPERS_ENABLE_GTHR_BACKEND

#include "freertos_config.hpp"
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include <cstring>

#define __GTHREADS 1

#if FREERTOS_CPP_WRAPPERS_ENABLE_STD_THREAD
#define __GTHREADS_CXX0X 1
#else
#define __GTHREADS_CXX0X 0
#endif

#if FREERTOS_CPP_WRAPPERS_ENABLE_STD_MUTEX
#define __GTHREAD_HAS_COND 1
#else
#define __GTHREAD_HAS_COND 0
#endif

#if FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 2 ||                               \
    FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 3
#define __GTHREAD_MUTEX_INIT_FUNCTION __gthread_mutex_init_function
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION                                \
  __gthread_recursive_mutex_init_function
#else
#define __GTHREAD_MUTEX_INIT { {}, nullptr, false }
#define __GTHREAD_RECURSIVE_MUTEX_INIT { {}, nullptr, false }
#endif

#define __GTHREAD_ONCE_INIT 0

typedef struct __gthread_mutex_t {
  StaticSemaphore_t storage;
  SemaphoreHandle_t handle;
  bool initialized;
} __gthread_mutex_t;

typedef struct __gthread_recursive_mutex_t {
  StaticSemaphore_t storage;
  SemaphoreHandle_t handle;
  bool initialized;
} __gthread_recursive_mutex_t;

#if FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL == 1

typedef struct __gthread_cond_t {
  SemaphoreHandle_t signal;
  volatile int waiter_count;
} __gthread_cond_t;

#define __GTHREAD_COND_INIT { nullptr, 0 }

#elif FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL == 2

typedef struct __gthread_cond_t {
  TaskHandle_t waiting_task;
  volatile uint8_t signaled;
} __gthread_cond_t;

#define __GTHREAD_COND_INIT { nullptr, 0 }

#else

typedef struct __gthread_cond_t {
  SemaphoreHandle_t internal_mutex;
  struct __gthread_cond_waiter {
    TaskHandle_t task;
    SemaphoreHandle_t semaphore;
    struct __gthread_cond_waiter *next;
  } *waiters;
  int max_waiters;
} __gthread_cond_t;

#define __GTHREAD_COND_INIT { nullptr, nullptr, 0 }

#endif

typedef volatile uint8_t __gthread_once_t;

typedef uint32_t __gthread_key_t;

typedef TaskHandle_t __gthread_t;

struct timespec;

inline int __gthread_active_p() {
  return xTaskGetSchedulerState() == taskSCHEDULER_RUNNING ? 1 : 0;
}

#if FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 2 ||                               \
    FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 3

inline void __gthread_mutex_init_function(__gthread_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle =
        xSemaphoreCreateMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateMutex();
#endif
    mutex->initialized = true;
  }
}

inline void
__gthread_recursive_mutex_init_function(__gthread_recursive_mutex_t *mutex) {
  if (!mutex->initialized) {
#if FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION
    mutex->handle =
        xSemaphoreCreateRecursiveMutexStatic(&mutex->storage);
#else
    mutex->handle = xSemaphoreCreateRecursiveMutex();
#endif
    mutex->initialized = true;
  }
}

#endif

inline int __gthread_mutex_lock(__gthread_mutex_t *mutex) {
#if FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 2 ||                               \
    FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 3
  if (!mutex->initialized) {
    __gthread_mutex_init_function(mutex);
  }
#else
  if (!mutex->initialized) {
    mutex->handle = xSemaphoreCreateMutex();
    mutex->initialized = true;
  }
#endif
  return xSemaphoreTake(mutex->handle, portMAX_DELAY) == pdTRUE ? 0 : 1;
}

inline int __gthread_mutex_trylock(__gthread_mutex_t *mutex) {
#if FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 2 ||                               \
    FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 3
  if (!mutex->initialized) {
    __gthread_mutex_init_function(mutex);
  }
#else
  if (!mutex->initialized) {
    mutex->handle = xSemaphoreCreateMutex();
    mutex->initialized = true;
  }
#endif
  return xSemaphoreTake(mutex->handle, 0) == pdTRUE ? 0 : 1;
}

inline int __gthread_mutex_unlock(__gthread_mutex_t *mutex) {
  return xSemaphoreGive(mutex->handle) == pdTRUE ? 0 : 1;
}

inline int __gthread_mutex_destroy(__gthread_mutex_t *mutex) {
  if (mutex->initialized && mutex->handle) {
    vSemaphoreDelete(mutex->handle);
    mutex->handle = nullptr;
    mutex->initialized = false;
  }
  return 0;
}

inline int
__gthread_recursive_mutex_lock(__gthread_recursive_mutex_t *mutex) {
#if FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 2 ||                               \
    FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 3
  if (!mutex->initialized) {
    __gthread_recursive_mutex_init_function(mutex);
  }
#else
  if (!mutex->initialized) {
    mutex->handle = xSemaphoreCreateRecursiveMutex();
    mutex->initialized = true;
  }
#endif
  return xSemaphoreTakeRecursive(mutex->handle, portMAX_DELAY) == pdTRUE
             ? 0
             : 1;
}

inline int
__gthread_recursive_mutex_trylock(__gthread_recursive_mutex_t *mutex) {
#if FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 2 ||                               \
    FREERTOS_CPP_WRAPPERS_ALLOC_STRATEGY == 3
  if (!mutex->initialized) {
    __gthread_recursive_mutex_init_function(mutex);
  }
#else
  if (!mutex->initialized) {
    mutex->handle = xSemaphoreCreateRecursiveMutex();
    mutex->initialized = true;
  }
#endif
  return xSemaphoreTakeRecursive(mutex->handle, 0) == pdTRUE ? 0 : 1;
}

inline int
__gthread_recursive_mutex_unlock(__gthread_recursive_mutex_t *mutex) {
  return xSemaphoreGiveRecursive(mutex->handle) == pdTRUE ? 0 : 1;
}

inline int
__gthread_recursive_mutex_destroy(__gthread_recursive_mutex_t *mutex) {
  if (mutex->initialized && mutex->handle) {
    vSemaphoreDelete(mutex->handle);
    mutex->handle = nullptr;
    mutex->initialized = false;
  }
  return 0;
}

inline void __gthread_once_callback(void) {}

inline int __gthread_once(__gthread_once_t *once, void (*func)(void)) {
  if (__atomic_load_n(once, __ATOMIC_ACQUIRE) == 2) {
    return 0;
  }
  taskENTER_CRITICAL();
  if (*once == 0) {
    *once = 1;
    taskEXIT_CRITICAL();
    func();
    __atomic_store_n(once, 2, __ATOMIC_RELEASE);
  } else {
    taskEXIT_CRITICAL();
  }
  return 0;
}

inline int __gthread_key_create(__gthread_key_t *key,
                                void (*dtor)(void *)) {
  (void)dtor;
  static __gthread_key_t next_key = 0;
  if (next_key >= configNUM_THREAD_LOCAL_STORAGE_POINTERS) {
    return 1;
  }
  *key = next_key++;
  return 0;
}

inline int __gthread_key_delete(__gthread_key_t key) {
  (void)key;
  return 0;
}

inline void *__gthread_getspecific(__gthread_key_t key) {
  return pvTaskGetThreadLocalStoragePointer(nullptr,
                                             static_cast<BaseType_t>(key));
}

inline int __gthread_setspecific(__gthread_key_t key, const void *ptr) {
  vTaskSetThreadLocalStoragePointer(nullptr, static_cast<BaseType_t>(key),
                                    const_cast<void *>(ptr));
  return 0;
}

#if FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL == 1

inline int __gthread_cond_wait(__gthread_cond_t *cond,
                                __gthread_mutex_t *mutex) {
  cond->waiter_count++;
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  __gthread_mutex_unlock(mutex);
  xSemaphoreTake(cond->signal, portMAX_DELAY);
  cond->waiter_count--;
  __gthread_mutex_lock(mutex);
  return 0;
}

inline int __gthread_cond_signal(__gthread_cond_t *cond) {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  if (cond->waiter_count > 0) {
    xSemaphoreGive(cond->signal);
  }
  return 0;
}

inline int __gthread_cond_broadcast(__gthread_cond_t *cond) {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  int count = cond->waiter_count;
  for (int i = 0; i < count; i++) {
    xSemaphoreGive(cond->signal);
  }
  return 0;
}

inline int __gthread_cond_timedwait(__gthread_cond_t *cond,
                                     __gthread_mutex_t *mutex,
                                     const struct timespec *ts) {
  (void)ts;
  return __gthread_cond_wait(cond, mutex);
}

inline int __gthread_cond_destroy(__gthread_cond_t *cond) {
  if (cond->signal) {
    vSemaphoreDelete(cond->signal);
    cond->signal = nullptr;
  }
  return 0;
}

#elif FREERTOS_CPP_WRAPPERS_CONDVAR_IMPL == 2

inline int __gthread_cond_wait(__gthread_cond_t *cond,
                                __gthread_mutex_t *mutex) {
  cond->waiting_task = xTaskGetCurrentTaskHandle();
  cond->signaled = 0;
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  __gthread_mutex_unlock(mutex);
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  __gthread_mutex_lock(mutex);
  return 0;
}

inline int __gthread_cond_signal(__gthread_cond_t *cond) {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
  if (cond->waiting_task) {
    vTaskNotifyGiveFromISR(cond->waiting_task, nullptr);
    cond->waiting_task = nullptr;
  }
  return 0;
}

inline int __gthread_cond_broadcast(__gthread_cond_t *cond) {
  return __gthread_cond_signal(cond);
}

inline int __gthread_cond_timedwait(__gthread_cond_t *cond,
                                     __gthread_mutex_t *mutex,
                                     const struct timespec *ts) {
  (void)ts;
  return __gthread_cond_wait(cond, mutex);
}

inline int __gthread_cond_destroy(__gthread_cond_t *cond) {
  cond->waiting_task = nullptr;
  cond->signaled = 0;
  return 0;
}

#else

inline int __gthread_cond_wait(__gthread_cond_t *cond,
                                __gthread_mutex_t *mutex) {
  (void)cond;
  __gthread_mutex_unlock(mutex);
  vTaskDelay(1);
  __gthread_mutex_lock(mutex);
  return 0;
}

inline int __gthread_cond_signal(__gthread_cond_t *cond) {
  (void)cond;
  return 0;
}

inline int __gthread_cond_broadcast(__gthread_cond_t *cond) {
  (void)cond;
  return 0;
}

inline int __gthread_cond_timedwait(__gthread_cond_t *cond,
                                     __gthread_mutex_t *mutex,
                                     const struct timespec *ts) {
  (void)ts;
  return __gthread_cond_wait(cond, mutex);
}

inline int __gthread_cond_destroy(__gthread_cond_t *cond) {
  (void)cond;
  return 0;
}

#endif

#if __GTHREADS_CXX0X

inline int __gthread_create(__gthread_t *thread, void (*func)(void *),
                             void *args) {
  return xTaskCreate(static_cast<void (*)(void *)>(func), "gthr",
                     configMINIMAL_STACK_SIZE, args, tskIDLE_PRIORITY + 1,
                     thread) == pdPASS
             ? 0
             : 1;
}

inline int __gthread_join(__gthread_t thread, void **value_ptr) {
  (void)value_ptr;
  eTaskState state;
  do {
    state = eTaskGetState(thread);
    if (state == eDeleted) {
      break;
    }
    vTaskDelay(1);
  } while (1);
  vTaskDelete(thread);
  return 0;
}

inline int __gthread_detach(__gthread_t thread) {
  (void)thread;
  return 0;
}

inline int __gthread_equal(__gthread_t t1, __gthread_t t2) {
  return t1 == t2;
}

inline __gthread_t __gthread_self() {
  return xTaskGetCurrentTaskHandle();
}

inline void __gthread_yield() {
  taskYIELD();
}

#endif

#endif