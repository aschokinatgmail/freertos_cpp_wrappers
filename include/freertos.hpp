/**********************************************************************************
@file freertos.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS C++ Wrappers Library main header
@version 0.1
@date 2024-04-12

The MIT License (MIT)

FreeRTOS C++ Wrappers Library
https://github.com/aschokinatgmail/freertos_cpp_wrappers/

Copyright(c) 2024 Andrey V. Shchekin <aschokin@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
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

#include "freertos_event_group.hpp"
#include "freertos_message_buffer.hpp"
#include "freertos_queue.hpp"
#include "freertos_semaphore.hpp"
#include "freertos_stream_buffer.hpp"
#include "freertos_sw_timer.hpp"
#include "freertos_task.hpp"
