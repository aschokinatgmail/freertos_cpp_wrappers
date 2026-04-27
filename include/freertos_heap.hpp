/**********************************************************************************
@file freertos_heap.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS heap integration: new/delete redirect and static memory callbacks
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
************************************************************************************/

#pragma once

#ifndef __cplusplus
#error "This header is for C++ only"
#endif

#include <cstddef>
#include <cstdint>
#include <new>
#include <FreeRTOS.h>
#include <task.h>

namespace freertos {

namespace heap {

namespace detail {

inline bool &redirect_flag() noexcept {
    static bool flag{true};
    return flag;
}

} // namespace detail

inline void enable_redirect() noexcept { detail::redirect_flag() = true; }
inline void disable_redirect() noexcept { detail::redirect_flag() = false; }
[[nodiscard]] inline bool is_redirect_enabled() noexcept {
    return detail::redirect_flag();
}

inline void *allocate(size_t size) noexcept { return pvPortMalloc(size); }

inline void *allocate_with_alignment_check(size_t alignment, size_t size) noexcept {
#if defined(portBYTE_ALIGNMENT)
    configASSERT(alignment <= static_cast<size_t>(portBYTE_ALIGNMENT));
#else
    configASSERT(alignment <= sizeof(size_t));
#endif
    (void)alignment;
    return pvPortMalloc(size);
}

inline void deallocate(void *ptr) noexcept { vPortFree(ptr); }

#if (tskKERNEL_VERSION_MAJOR >= 10 && tskKERNEL_VERSION_MINOR >= 5) ||              \
    (tskKERNEL_VERSION_MAJOR > 10)
inline void *calloc(size_t num, size_t size) noexcept {
    return pvPortCalloc(num, size);
}
#endif

} // namespace heap

#if configSUPPORT_STATIC_ALLOCATION

namespace static_memory {

constexpr UBaseType_t default_idle_task_stack_size = configMINIMAL_STACK_SIZE;

#ifndef configTIMER_TASK_STACK_DEPTH
constexpr UBaseType_t default_timer_task_stack_size = configMINIMAL_STACK_SIZE;
#else
constexpr UBaseType_t default_timer_task_stack_size =
    configTIMER_TASK_STACK_DEPTH;
#endif

namespace detail {

inline StaticTask_t &idle_task_tcb() {
    static StaticTask_t tcb{};
    return tcb;
}

inline StackType_t *idle_task_stack() {
    static StackType_t stack[default_idle_task_stack_size]{};
    return stack;
}

inline StaticTask_t &timer_task_tcb() {
    static StaticTask_t tcb{};
    return tcb;
}

inline StackType_t *timer_task_stack() {
    static StackType_t stack[default_timer_task_stack_size]{};
    return stack;
}

} // namespace detail

inline void vApplicationGetIdleTaskMemory(
    StaticTask_t **ppxIdleTaskTCBBuffer,
    StackType_t **ppxIdleTaskStackBuffer,
    uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &detail::idle_task_tcb();
    *ppxIdleTaskStackBuffer = detail::idle_task_stack();
    *pulIdleTaskStackSize = default_idle_task_stack_size;
}

inline void vApplicationGetTimerTaskMemory(
    StaticTask_t **ppxTimerTaskTCBBuffer,
    StackType_t **ppxTimerTaskStackBuffer,
    uint32_t *pulTimerTaskStackSize) {
    *ppxTimerTaskTCBBuffer = &detail::timer_task_tcb();
    *ppxTimerTaskStackBuffer = detail::timer_task_stack();
    *pulTimerTaskStackSize = default_timer_task_stack_size;
}

} // namespace static_memory

#endif // configSUPPORT_STATIC_ALLOCATION

} // namespace freertos

#ifdef FREERTOS_CPP_WRAPPERS_REDIRECT_NEW_DELETE

inline void *operator new(size_t size) noexcept {
    return freertos::heap::allocate(size);
}

inline void operator delete(void *ptr) noexcept {
    freertos::heap::deallocate(ptr);
}

inline void *operator new[](size_t size) noexcept {
    return freertos::heap::allocate(size);
}

inline void operator delete[](void *ptr) noexcept {
    freertos::heap::deallocate(ptr);
}

inline void *operator new(size_t size, const std::nothrow_t &) noexcept {
    return freertos::heap::allocate(size);
}

inline void operator delete(void *ptr, const std::nothrow_t &) noexcept {
    freertos::heap::deallocate(ptr);
}

inline void operator delete(void *ptr, size_t) noexcept {
    freertos::heap::deallocate(ptr);
}

inline void operator delete[](void *ptr, size_t) noexcept {
    freertos::heap::deallocate(ptr);
}

#if __cplusplus >= 201703L

inline void *operator new(size_t size, std::align_val_t align) noexcept {
    return freertos::heap::allocate_with_alignment_check(static_cast<size_t>(align), size);
}

inline void *operator new[](size_t size, std::align_val_t align) noexcept {
    return freertos::heap::allocate_with_alignment_check(static_cast<size_t>(align), size);
}

inline void *operator new(size_t size, std::align_val_t align, std::nothrow_t &) noexcept {
    return freertos::heap::allocate_with_alignment_check(static_cast<size_t>(align), size);
}

inline void *operator new[](size_t size, std::align_val_t align, std::nothrow_t &) noexcept {
    return freertos::heap::allocate_with_alignment_check(static_cast<size_t>(align), size);
}

inline void operator delete(void *ptr, std::align_val_t) noexcept {
    freertos::heap::deallocate(ptr);
}

inline void operator delete[](void *ptr, std::align_val_t) noexcept {
    freertos::heap::deallocate(ptr);
}

inline void operator delete(void *ptr, std::align_val_t, std::nothrow_t &) noexcept {
    freertos::heap::deallocate(ptr);
}

inline void operator delete[](void *ptr, std::align_val_t, std::nothrow_t &) noexcept {
    freertos::heap::deallocate(ptr);
}

#endif // __cplusplus >= 201703L

#endif // FREERTOS_CPP_WRAPPERS_REDIRECT_NEW_DELETE