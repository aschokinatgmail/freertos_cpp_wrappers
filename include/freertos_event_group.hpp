/**********************************************************************************
@file freertos_event_group.hpp
@author Andrey V. Shchekin <aschokin@gmail.com>
@brief FreeRTOS event group wrapper
@version 0.1
@date 2024-04-07

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

#include <FreeRTOS.h>
#include <chrono>
#include <event_groups.h>

namespace freertos {

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief An allocator for the event group that uses a static memory allocation.
 *
 */
class static_event_group_allocator {
  StaticEventGroup_t m_event_group_placeholder;

public:
  static_event_group_allocator() = default;
  static_event_group_allocator(const static_event_group_allocator &) = delete;
  static_event_group_allocator(static_event_group_allocator &&) = delete;

  static_event_group_allocator &
  operator=(const static_event_group_allocator &) = delete;
  static_event_group_allocator &
  operator=(static_event_group_allocator &&) = delete;
  EventGroupHandle_t create() {
    return xEventGroupCreateStatic(&m_event_group_placeholder);
  }
};
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief An allocator for the event group that uses a dynamic memory
 * allocation.
 *
 */
class dynamic_event_group_allocator {
public:
  EventGroupHandle_t create() { return xEventGroupCreate(); }
};
#endif

/**
 * @brief A wrapper for the FreeRTOS event group.
 *
 * @tparam EventGroupAllocator type of the event group allocator
 */
template <typename EventGroupAllocator> class event_group {
  EventGroupAllocator m_allocator;
  EventGroupHandle_t m_event_group;

public:
  /**
   * @brief Construct a new event group object
   *
   */
  event_group(void) : m_event_group(m_allocator.create()) {}
  event_group(const event_group &) = delete;
  event_group(event_group &&other) = delete;
  /**
   * @brief Destruct the event group object and delete the event group instance
   * if it was created.
   *
   */
  ~event_group(void) {
    if (m_event_group) {
      vEventGroupDelete(m_event_group);
    }
  }

  event_group &operator=(const event_group &) = delete;
  event_group &operator=(event_group &&other) = delete;

  /**
   * @brief Method to get the handle of the event group.
   * @ref https://www.freertos.org/xEventGroupGetHandle.html
   *
   * @return EventGroupHandle_t event group handle
   */
  EventGroupHandle_t handle(void) const { return m_event_group; }

  /**
   * @brief Method to set bits in the event group.
   * @ref https://www.freertos.org/xEventGroupSetBits.html
   *
   * @param bits_to_set bits to set
   * @return EventBits_t bits set
   */
  EventBits_t set_bits(const EventBits_t bits_to_set) {
    return xEventGroupSetBits(m_event_group, bits_to_set);
  }
  /**
   * @brief Method to set bits in the event group from an ISR.
   * @ref https://www.freertos.org/xEventGroupSetBitsFromISR.html
   *
   * @param bits_to_set bits to set
   * @return EventBits_t bits set
   */
  EventBits_t set_bits_isr(const EventBits_t bits_to_set) {
    BaseType_t higher_priority_task_woken = pdFALSE;
    const EventBits_t bits_set = xEventGroupSetBitsFromISR(
        m_event_group, bits_to_set, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);
    return bits_set;
  }
  /**
   * @brief Method to clear bits in the event group.
   * @ref https://www.freertos.org/xEventGroupClearBits.html
   *
   * @param bits_to_clear bits to clear
   * @return EventBits_t bits cleared
   */
  EventBits_t clear_bits(const EventBits_t bits_to_clear) {
    return xEventGroupClearBits(m_event_group, bits_to_clear);
  }
  /**
   * @brief Method waiting for the bits or group of bits to be set.
   * @ref https://www.freertos.org/xEventGroupWaitBits.html
   *
   * @param bits_to_wait_for bits to wait for
   * @param clear_on_exit clear bits on exit
   * @param wait_for_all_bits wait for all bits
   * @param ticks_to_wait timeout in ticks to wait for the bits
   * @return EventBits_t value of the event group after the call returns
   */
  EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
                        const BaseType_t clear_on_exit,
                        const BaseType_t wait_for_all_bits,
                        const TickType_t ticks_to_wait) {
    return xEventGroupWaitBits(m_event_group, bits_to_wait_for, clear_on_exit,
                               wait_for_all_bits, ticks_to_wait);
  }
  /**
   * @brief Method waiting for the bits or group of bits to be set compliant to
   * the C++ std::chrono library.
   * @ref https://www.freertos.org/xEventGroupWaitBits.html
   *
   * @param bits_to_wait_for bits to wait for
   * @param clear_on_exit clear bits on exit
   * @param wait_for_all_bits wait for all bits
   * @param timeout timeout to wait for the bits
   * @return EventBits_t value of the event group after the call returns
   */
  template <typename Rep, typename Period>
  EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
                        const BaseType_t clear_on_exit,
                        const BaseType_t wait_for_all_bits,
                        const std::chrono::duration<Rep, Period> &timeout) {
    return wait_bits(
        bits_to_wait_for, clear_on_exit, wait_for_all_bits,
        static_cast<TickType_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
  /**
   * @brief Method to get the bits of the event group.
   * @ref https://www.freertos.org/xEventGroupGetBits.html
   *
   * @return EventBits_t Current value of the event group bits
   */
  EventBits_t get_bits(void) const { return xEventGroupGetBits(m_event_group); }
  /**
   * @brief Method to get the bits of the event group from an ISR.
   * @ref https://www.freertos.org/xEventGroupGetBitsFromISR.html
   *
   * @return EventBits_t Current value of the event group bits
   */
  EventBits_t get_bits_isr(void) const {
    return xEventGroupGetBitsFromISR(m_event_group);
  }
  /**
   * @brief Method to synchronize the event group.
   * @ref https://www.freertos.org/xEventGroupSync.html
   *
   * @param bits_to_set bits to set
   * @param bits_to_wait_for bits to wait for
   * @param ticks_to_wait timeout in ticks to wait for the bits
   * @return EventBits_t value of the event group after the call returns
   */
  EventBits_t sync(const EventBits_t bits_to_set,
                   const EventBits_t bits_to_wait_for,
                   const TickType_t ticks_to_wait) {
    return xEventGroupSync(m_event_group, bits_to_set, bits_to_wait_for,
                           ticks_to_wait);
  }
  template <typename Rep, typename Period>
  EventBits_t sync(const EventBits_t bits_to_set,
                   const EventBits_t bits_to_wait_for,
                   const std::chrono::duration<Rep, Period> &timeout) {
    return sync(
        bits_to_set, bits_to_wait_for,
        static_cast<TickType_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
                .count()));
  }
};

#if configSUPPORT_STATIC_ALLOCATION
/**
 * @brief Namespace for aliases of RTOS kernel objects that use a static memory
 *
 */
namespace sa {
/**
 * @brief Alias for the event group that uses a static memory allocation.
 *
 */
using event_group =
    freertos::event_group<freertos::static_event_group_allocator>;
} // namespace sa
#endif
#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief Namespace for aliases of RTOS kernel objects that use a dynamic memory
 *
 */
namespace da {
/**
 * @brief Alias for the event group that uses a dynamic memory allocation.
 *
 */
using event_group =
    freertos::event_group<freertos::dynamic_event_group_allocator>;
} // namespace da
#endif

} // namespace freertos
