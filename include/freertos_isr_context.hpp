/**********************************************************************************
@file freertos_isr_context.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief ISR context detection utilities for FreeRTOS C++ Wrappers
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
#error "This header is C++ only"
#endif

#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>

namespace freertos {

enum class execution_context : uint8_t {
    task,
    isr,
};

[[nodiscard]] inline bool in_isr() noexcept {
#ifdef FREERTOS_CPP_WRAPPERS_SIMULATION
    // The POSIX/simulation port does not provide xPortIsInsideInterrupt().
    // Tasks under the simulation are real POSIX threads and never run in
    // hardware ISR context, so always return false.
    return false;
#else
    return xPortIsInsideInterrupt() != 0;
#endif
}

[[nodiscard]] inline execution_context current_execution_context() noexcept {
    return in_isr() ? execution_context::isr : execution_context::task;
}

} // namespace freertos