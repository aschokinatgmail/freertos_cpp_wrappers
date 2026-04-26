/**********************************************************************************
@file freertos_fixed_function.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief Fixed-capacity callback (SBO delegate) for heap-free task/timer callbacks
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

#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <exception>
#include <type_traits>

#ifndef configASSERT
#define configASSERT(expr) assert(expr)
#endif
#include <utility>

namespace freertos {

template<typename Signature, size_t Capacity = 4 * sizeof(void*)>
class fixed_function;

template<typename ReturnType, typename... Args, size_t Capacity>
class fixed_function<ReturnType(Args...), Capacity> {
    using invoke_fn = ReturnType(*)(std::byte*, Args...);
    using copy_fn = void(*)(std::byte* dst, const std::byte* src);
    using move_fn = void(*)(std::byte* dst, std::byte* src);
    using destroy_fn = void(*)(std::byte*);

    alignas(std::max_align_t) std::array<std::byte, Capacity> m_storage{};
    invoke_fn m_invoker = nullptr;
    copy_fn m_copier = nullptr;
    move_fn m_mover = nullptr;
    destroy_fn m_destroyer = nullptr;

    template<typename Callable>
    static ReturnType invoke_callable(std::byte* data, Args... args) {
        Callable& c = *reinterpret_cast<Callable*>(data);
        return c(static_cast<Args>(args)...);
    }

    template<typename Callable,
             typename = std::enable_if_t<std::is_copy_constructible_v<Callable>>>
    static void copy_callable(std::byte* dst, const std::byte* src) {
        new (dst) Callable(*reinterpret_cast<const Callable*>(src));
    }

    template<typename Callable>
    static void move_callable_impl(std::byte* dst, std::byte* src) {
        new (dst) Callable(std::move(*reinterpret_cast<Callable*>(src)));
    }

    template<typename Callable>
    static void destroy_callable(std::byte* data) {
        reinterpret_cast<Callable*>(data)->~Callable();
    }

    static ReturnType invoke_fn_ptr(std::byte* data, Args... args) {
        using FnPtr = ReturnType(*)(Args...);
        FnPtr fn;
        std::memcpy(&fn, data, sizeof(FnPtr));
        return fn(static_cast<Args>(args)...);
    }

    static void copy_fn_ptr(std::byte* dst, const std::byte* src) {
        using FnPtr = ReturnType(*)(Args...);
        std::memcpy(dst, src, sizeof(FnPtr));
    }

    static void move_fn_ptr(std::byte* dst, std::byte* src) {
        using FnPtr = ReturnType(*)(Args...);
        std::memcpy(dst, src, sizeof(FnPtr));
    }

    void clear() noexcept {
        if (m_destroyer) {
            m_destroyer(m_storage.data());
            m_destroyer = nullptr;
        }
        m_invoker = nullptr;
        m_copier = nullptr;
        m_mover = nullptr;
    }

public:
    fixed_function() noexcept = default;

    fixed_function(std::nullptr_t) noexcept {}

    fixed_function(ReturnType(*function)(Args...)) {
        if (function) {
            using FnPtr = ReturnType(*)(Args...);
            std::memcpy(m_storage.data(), &function, sizeof(FnPtr));
            m_invoker = &invoke_fn_ptr;
            m_copier = &copy_fn_ptr;
            m_mover = &move_fn_ptr;
            m_destroyer = nullptr;
        }
    }

    template<typename Callable,
             typename = std::enable_if_t<
                 !std::is_same_v<std::decay_t<Callable>, fixed_function> &&
                 !std::is_pointer_v<std::decay_t<Callable>> &&
                 sizeof(std::decay_t<Callable>) <= Capacity &&
                 std::is_invocable_r_v<ReturnType, std::decay_t<Callable>&, Args...>>>
    fixed_function(Callable&& callable) {
        using Decayed = std::decay_t<Callable>;
        new (m_storage.data()) Decayed(std::forward<Callable>(callable));
        m_invoker = &invoke_callable<Decayed>;
        if constexpr (std::is_copy_constructible_v<Decayed>) {
            m_copier = &copy_callable<Decayed>;
        } else {
            m_copier = nullptr;
        }
        m_mover = &move_callable_impl<Decayed>;
        m_destroyer = &destroy_callable<Decayed>;
    }

    fixed_function(const fixed_function& other)
        : m_invoker(other.m_invoker), m_copier(other.m_copier),
          m_mover(other.m_mover), m_destroyer(other.m_destroyer) {
        if (m_copier) {
            m_copier(m_storage.data(), other.m_storage.data());
        } else if (!m_invoker) {
        } else {
            configASSERT(!"cannot copy fixed_function wrapping non-copyable callable");
            m_invoker = nullptr;
            m_copier = nullptr;
            m_mover = nullptr;
            m_destroyer = nullptr;
        }
    }

    fixed_function(fixed_function&& other) noexcept
        : m_invoker(other.m_invoker), m_copier(other.m_copier),
          m_mover(other.m_mover), m_destroyer(other.m_destroyer) {
        if (m_mover) {
            try {
                m_mover(m_storage.data(), other.m_storage.data());
            } catch (...) {
                configASSERT(!"move of contained callable threw in noexcept context");
                std::terminate();
            }
        }
        if (other.m_destroyer) {
            other.m_destroyer(other.m_storage.data());
        }
        other.m_invoker = nullptr;
        other.m_copier = nullptr;
        other.m_mover = nullptr;
        other.m_destroyer = nullptr;
    }

    fixed_function& operator=(const fixed_function& other) {
        if (this != &other) {
            clear();
            m_invoker = other.m_invoker;
            m_copier = other.m_copier;
            m_mover = other.m_mover;
            m_destroyer = other.m_destroyer;
            if (m_copier) {
                m_copier(m_storage.data(), other.m_storage.data());
            } else if (m_invoker) {
                configASSERT(!"cannot copy fixed_function wrapping non-copyable callable");
                m_invoker = nullptr;
                m_copier = nullptr;
                m_mover = nullptr;
                m_destroyer = nullptr;
            }
        }
        return *this;
    }

    fixed_function& operator=(fixed_function&& other) noexcept {
        if (this != &other) {
            clear();
            m_invoker = other.m_invoker;
            m_copier = other.m_copier;
            m_mover = other.m_mover;
            m_destroyer = other.m_destroyer;
            if (m_mover) {
                try {
                    m_mover(m_storage.data(), other.m_storage.data());
                } catch (...) {
                    configASSERT(!"move of contained callable threw in noexcept context");
                    std::terminate();
                }
            }
            if (other.m_destroyer) {
                other.m_destroyer(other.m_storage.data());
            }
            other.m_invoker = nullptr;
            other.m_copier = nullptr;
            other.m_mover = nullptr;
            other.m_destroyer = nullptr;
        }
        return *this;
    }

    fixed_function& operator=(std::nullptr_t) noexcept {
        clear();
        return *this;
    }

    ~fixed_function() {
        if (m_destroyer) {
            m_destroyer(m_storage.data());
        }
    }

    ReturnType operator()(Args... args) {
        configASSERT(m_invoker != nullptr && "cannot call empty fixed_function");
        return m_invoker(m_storage.data(), static_cast<Args>(args)...);
    }

    ReturnType operator()(Args... args) const {
        configASSERT(m_invoker != nullptr && "cannot call empty fixed_function");
        return const_cast<fixed_function*>(this)->m_invoker(
            const_cast<std::byte*>(m_storage.data()), static_cast<Args>(args)...);
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return m_invoker != nullptr;
    }

    [[nodiscard]] bool operator!() const noexcept {
        return m_invoker == nullptr;
    }

    void swap(fixed_function& other) noexcept {
        if (this == &other) return;

        fixed_function tmp(std::move(*this));
        *this = std::move(other);
        other = std::move(tmp);
    }

    friend void swap(fixed_function& a, fixed_function& b) noexcept {
        a.swap(b);
    }

    friend bool operator==(const fixed_function& f, std::nullptr_t) noexcept {
        return !f;
    }

    friend bool operator==(std::nullptr_t, const fixed_function& f) noexcept {
        return !f;
    }

    friend bool operator!=(const fixed_function& f, std::nullptr_t) noexcept {
        return static_cast<bool>(f);
    }

    friend bool operator!=(std::nullptr_t, const fixed_function& f) noexcept {
        return static_cast<bool>(f);
    }
};

} // namespace freertos