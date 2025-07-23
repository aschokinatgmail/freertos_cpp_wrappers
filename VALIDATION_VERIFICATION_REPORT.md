# Validation and Verification Report

## Executive Summary

This report provides comprehensive validation and verification results for the FreeRTOS C++ Wrappers project. The analysis focuses exclusively on the main project code (src/ and include/ directories) and excludes test harness code to maintain focus on the library implementation.

**Key Areas Covered:**
- Static code analysis (MISRA C++ 2012 compliance and clang-tidy)
- Test execution results  
- Code coverage analysis
- Validation conclusions

## Static Code Analysis

### Overview

**Static Analysis Tools**: clang-tidy + cppcheck (MISRA C++ 2012)
**Analysis Scope**: Main project code only (src/ and include/ directories)
**Excluded**: Test harness, mocks, and external dependencies

### Combined Static Analysis Results

#### Summary

- **MISRA C++ 2012 Violations**: 0
0
- **Style Issues**: 0
0
- **Warnings**: 0
0
- **Errors**: 0
0

#### Detailed Violations

### Summary

- **Total MISRA Violations**: 0
- **Unique Rules Violated**: 0
- **Files Analyzed**: 0
- **Other Issues**: 0

### Analysis Notes

- **Standard**: MISRA C 2012 (applicable rules for C++)
- **Tool**: cppcheck with MISRA addon
- **Scope**: Library modules only (src/, include/)
- **Rule texts**: Cannot be displayed due to MISRA licensing restrictions
- **Compatibility**: Many MISRA C 2012 rules overlap with MISRA C++ 2008 requirements
- **Integration**: This analysis complements the existing clang-tidy static analysis


#### clang-tidy Analysis Results

- **Tool**: clang-tidy
- **Warnings**: 94
- **Errors**: 9

##### Detailed clang-tidy Violations

### Detailed clang-tidy Analysis

#### bugprone-easily-swappable-parameters (28 violation(s))

**Violation 1**: freertos_event_group.hpp:163:25
*Severity*: Warning
*Message*: 4 adjacent parameters of 'wait_bits' of similar type ('const int') are easily swapped by mistake

```cpp
     160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
>>>  163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
     164:                         const BaseType_t clear_on_exit,
     165:                         const BaseType_t wait_for_all_bits,
     166:                         const TickType_t ticks_to_wait) {
```

**Violation 2**: freertos_event_group.hpp:182:25
*Severity*: Warning
*Message*: 3 adjacent parameters of 'wait_bits' of similar type ('const int') are easily swapped by mistake

```cpp
     179:    * @return EventBits_t value of the event group after the call returns
     180:    */
     181:   template <typename Rep, typename Period>
>>>  182:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
     183:                         const BaseType_t clear_on_exit,
     184:                         const BaseType_t wait_for_all_bits,
     185:                         const std::chrono::duration<Rep, Period> &timeout) {
```

**Violation 3**: freertos_event_group.hpp:217:20
*Severity*: Warning
*Message*: 3 adjacent parameters of 'sync' of similar type ('const int') are easily swapped by mistake

```cpp
     214:    * @param ticks_to_wait timeout in ticks to wait for the bits
     215:    * @return EventBits_t value of the event group after the call returns
     216:    */
>>>  217:   EventBits_t sync(const EventBits_t bits_to_set,
     218:                    const EventBits_t bits_to_wait_for,
     219:                    const TickType_t ticks_to_wait) {
     220:     return xEventGroupSync(m_event_group, bits_to_set, bits_to_wait_for,
```

**Violation 4**: freertos_event_group.hpp:224:20
*Severity*: Warning
*Message*: 2 adjacent parameters of 'sync' of similar type ('const int') are easily swapped by mistake

```cpp
     221:                            ticks_to_wait);
     222:   }
     223:   template <typename Rep, typename Period>
>>>  224:   EventBits_t sync(const EventBits_t bits_to_set,
     225:                    const EventBits_t bits_to_wait_for,
     226:                    const std::chrono::duration<Rep, Period> &timeout) {
     227:     return sync(
```

**Violation 5**: freertos_message_buffer.hpp:133:37
*Severity*: Warning
*Message*: 2 adjacent parameters of 'send' of convertible types are easily swapped by mistake

```cpp
     130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
>>>  133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
     134:               TickType_t xTicksToWait) {
     135:     return xMessageBufferSend(m_message_buffer, pvTxData, xDataLengthBytes,
     136:                               xTicksToWait);
```

**Violation 6**: freertos_message_buffer.hpp:165:34
*Severity*: Warning
*Message*: 2 adjacent parameters of 'receive' of convertible types are easily swapped by mistake

```cpp
     162:    * become available
     163:    * @return size_t number of bytes received
     164:    */
>>>  165:   size_t receive(void *pvRxData, size_t xBufferLengthBytes,
     166:                  TickType_t xTicksToWait) {
     167:     return xMessageBufferReceive(m_message_buffer, pvRxData, xBufferLengthBytes,
     168:                                  xTicksToWait);
```

**Violation 7**: freertos_stream_buffer.hpp:140:37
*Severity*: Warning
*Message*: 2 adjacent parameters of 'send' of convertible types are easily swapped by mistake

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 8**: freertos_stream_buffer.hpp:280:30
*Severity*: Warning
*Message*: 2 adjacent parameters of 'receive' of convertible types are easily swapped by mistake

```cpp
     277:    * buffer.
     278:    * @return size_t Number of bytes received.
     279:    */
>>>  280:   size_t receive(void *data, size_t data_size,
     281:                  TickType_t timeout = portMAX_DELAY) {
     282:     return xStreamBufferReceive(m_stream_buffer, data, data_size, timeout);
     283:   }
```

**Violation 9**: freertos_task.hpp:229:8
*Severity*: Warning
*Message*: 2 adjacent parameters of 'task<TaskAllocator>' of similar type are easily swapped by mistake

```cpp
     226:    * @param priority  priority of the task
     227:    * @param task_routine  task routine function
     228:    */
>>>  229:   task(const std::string &name, UBaseType_t priority,
     230:        task_routine_t &&task_routine)
     231:       : task{name.c_str(), priority,
     232:              std::forward<std::function<void()>>(task_routine)} {}
```

**Violation 10**: freertos_task.hpp:604:17
*Severity*: Warning
*Message*: 3 adjacent parameters of 'periodic_task<TaskAllocator>' of similar type ('task_routine_t &&') are easily swapped by mistake

```cpp
     601:    */
     602:   template <typename Rep, typename Period>
     603:   periodic_task(const char *name, UBaseType_t priority,
>>>  604:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     605:                 task_routine_t &&periodic_routine,
     606:                 const std::chrono::duration<Rep, Period> &period,
     607:                 bool start_suspended = true)
```

**Violation 11**: freertos_task.hpp:626:17
*Severity*: Warning
*Message*: 2 adjacent parameters of 'periodic_task<TaskAllocator>' of similar type are easily swapped by mistake

```cpp
     623:    * @param start_suspended  start the task in the suspended state
     624:    */
     625:   template <typename Rep, typename Period>
>>>  626:   periodic_task(const std::string &name, UBaseType_t priority,
     627:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     628:                 task_routine_t &&periodic_routine,
     629:                 const std::chrono::duration<Rep, Period> &period,
```

**Violation 12**: freertos_task.hpp:668:17
*Severity*: Warning
*Message*: 2 adjacent parameters of 'periodic_task<TaskAllocator>' of similar type are easily swapped by mistake

```cpp
     665:    * @param periodic_routine  periodic task routine function
     666:    * @param start_suspended  start the task in the suspended state
     667:    */
>>>  668:   periodic_task(const std::string &name, UBaseType_t priority,
     669:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     670:                 task_routine_t &&periodic_routine, bool start_suspended = true)
     671:       : periodic_task{name.c_str(),
```

**Violation 13**: freertos_event_group.hpp:163:25
*Severity*: Warning
*Message*: 4 adjacent parameters of 'wait_bits' of similar type ('const int') are easily swapped by mistake

```cpp
     160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
>>>  163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
     164:                         const BaseType_t clear_on_exit,
     165:                         const BaseType_t wait_for_all_bits,
     166:                         const TickType_t ticks_to_wait) {
```

**Violation 14**: freertos_event_group.hpp:182:25
*Severity*: Warning
*Message*: 3 adjacent parameters of 'wait_bits' of similar type ('const int') are easily swapped by mistake

```cpp
     179:    * @return EventBits_t value of the event group after the call returns
     180:    */
     181:   template <typename Rep, typename Period>
>>>  182:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
     183:                         const BaseType_t clear_on_exit,
     184:                         const BaseType_t wait_for_all_bits,
     185:                         const std::chrono::duration<Rep, Period> &timeout) {
```

**Violation 15**: freertos_event_group.hpp:217:20
*Severity*: Warning
*Message*: 3 adjacent parameters of 'sync' of similar type ('const int') are easily swapped by mistake

```cpp
     214:    * @param ticks_to_wait timeout in ticks to wait for the bits
     215:    * @return EventBits_t value of the event group after the call returns
     216:    */
>>>  217:   EventBits_t sync(const EventBits_t bits_to_set,
     218:                    const EventBits_t bits_to_wait_for,
     219:                    const TickType_t ticks_to_wait) {
     220:     return xEventGroupSync(m_event_group, bits_to_set, bits_to_wait_for,
```

**Violation 16**: freertos_event_group.hpp:224:20
*Severity*: Warning
*Message*: 2 adjacent parameters of 'sync' of similar type ('const int') are easily swapped by mistake

```cpp
     221:                            ticks_to_wait);
     222:   }
     223:   template <typename Rep, typename Period>
>>>  224:   EventBits_t sync(const EventBits_t bits_to_set,
     225:                    const EventBits_t bits_to_wait_for,
     226:                    const std::chrono::duration<Rep, Period> &timeout) {
     227:     return sync(
```

**Violation 17**: freertos_message_buffer.hpp:133:37
*Severity*: Warning
*Message*: 2 adjacent parameters of 'send' of convertible types are easily swapped by mistake

```cpp
     130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
>>>  133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
     134:               TickType_t xTicksToWait) {
     135:     return xMessageBufferSend(m_message_buffer, pvTxData, xDataLengthBytes,
     136:                               xTicksToWait);
```

**Violation 18**: freertos_message_buffer.hpp:165:34
*Severity*: Warning
*Message*: 2 adjacent parameters of 'receive' of convertible types are easily swapped by mistake

```cpp
     162:    * become available
     163:    * @return size_t number of bytes received
     164:    */
>>>  165:   size_t receive(void *pvRxData, size_t xBufferLengthBytes,
     166:                  TickType_t xTicksToWait) {
     167:     return xMessageBufferReceive(m_message_buffer, pvRxData, xBufferLengthBytes,
     168:                                  xTicksToWait);
```

**Violation 19**: freertos_stream_buffer.hpp:140:37
*Severity*: Warning
*Message*: 2 adjacent parameters of 'send' of convertible types are easily swapped by mistake

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 20**: freertos_stream_buffer.hpp:280:30
*Severity*: Warning
*Message*: 2 adjacent parameters of 'receive' of convertible types are easily swapped by mistake

```cpp
     277:    * buffer.
     278:    * @return size_t Number of bytes received.
     279:    */
>>>  280:   size_t receive(void *data, size_t data_size,
     281:                  TickType_t timeout = portMAX_DELAY) {
     282:     return xStreamBufferReceive(m_stream_buffer, data, data_size, timeout);
     283:   }
```

**Violation 21**: freertos_task.hpp:229:8
*Severity*: Warning
*Message*: 2 adjacent parameters of 'task<TaskAllocator>' of similar type are easily swapped by mistake

```cpp
     226:    * @param priority  priority of the task
     227:    * @param task_routine  task routine function
     228:    */
>>>  229:   task(const std::string &name, UBaseType_t priority,
     230:        task_routine_t &&task_routine)
     231:       : task{name.c_str(), priority,
     232:              std::forward<std::function<void()>>(task_routine)} {}
```

**Violation 22**: freertos_task.hpp:604:17
*Severity*: Warning
*Message*: 3 adjacent parameters of 'periodic_task<TaskAllocator>' of similar type ('task_routine_t &&') are easily swapped by mistake

```cpp
     601:    */
     602:   template <typename Rep, typename Period>
     603:   periodic_task(const char *name, UBaseType_t priority,
>>>  604:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     605:                 task_routine_t &&periodic_routine,
     606:                 const std::chrono::duration<Rep, Period> &period,
     607:                 bool start_suspended = true)
```

**Violation 23**: freertos_task.hpp:626:17
*Severity*: Warning
*Message*: 2 adjacent parameters of 'periodic_task<TaskAllocator>' of similar type are easily swapped by mistake

```cpp
     623:    * @param start_suspended  start the task in the suspended state
     624:    */
     625:   template <typename Rep, typename Period>
>>>  626:   periodic_task(const std::string &name, UBaseType_t priority,
     627:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     628:                 task_routine_t &&periodic_routine,
     629:                 const std::chrono::duration<Rep, Period> &period,
```

**Violation 24**: freertos_task.hpp:668:17
*Severity*: Warning
*Message*: 2 adjacent parameters of 'periodic_task<TaskAllocator>' of similar type are easily swapped by mistake

```cpp
     665:    * @param periodic_routine  periodic task routine function
     666:    * @param start_suspended  start the task in the suspended state
     667:    */
>>>  668:   periodic_task(const std::string &name, UBaseType_t priority,
     669:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     670:                 task_routine_t &&periodic_routine, bool start_suspended = true)
     671:       : periodic_task{name.c_str(),
```

**Violation 25**: freertos_task.hpp:229:8
*Severity*: Warning
*Message*: 2 adjacent parameters of 'task<TaskAllocator>' of similar type are easily swapped by mistake

```cpp
     226:    * @param priority  priority of the task
     227:    * @param task_routine  task routine function
     228:    */
>>>  229:   task(const std::string &name, UBaseType_t priority,
     230:        task_routine_t &&task_routine)
     231:       : task{name.c_str(), priority,
     232:              std::forward<std::function<void()>>(task_routine)} {}
```

**Violation 26**: freertos_task.hpp:604:17
*Severity*: Warning
*Message*: 3 adjacent parameters of 'periodic_task<TaskAllocator>' of similar type ('task_routine_t &&') are easily swapped by mistake

```cpp
     601:    */
     602:   template <typename Rep, typename Period>
     603:   periodic_task(const char *name, UBaseType_t priority,
>>>  604:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     605:                 task_routine_t &&periodic_routine,
     606:                 const std::chrono::duration<Rep, Period> &period,
     607:                 bool start_suspended = true)
```

**Violation 27**: freertos_task.hpp:626:17
*Severity*: Warning
*Message*: 2 adjacent parameters of 'periodic_task<TaskAllocator>' of similar type are easily swapped by mistake

```cpp
     623:    * @param start_suspended  start the task in the suspended state
     624:    */
     625:   template <typename Rep, typename Period>
>>>  626:   periodic_task(const std::string &name, UBaseType_t priority,
     627:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     628:                 task_routine_t &&periodic_routine,
     629:                 const std::chrono::duration<Rep, Period> &period,
```

**Violation 28**: freertos_task.hpp:668:17
*Severity*: Warning
*Message*: 2 adjacent parameters of 'periodic_task<TaskAllocator>' of similar type are easily swapped by mistake

```cpp
     665:    * @param periodic_routine  periodic task routine function
     666:    * @param start_suspended  start the task in the suspended state
     667:    */
>>>  668:   periodic_task(const std::string &name, UBaseType_t priority,
     669:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     670:                 task_routine_t &&periodic_routine, bool start_suspended = true)
     671:       : periodic_task{name.c_str(),
```

#### bugprone-infinite-loop (2 violation(s))

**Violation 1**: freertos_semaphore.hpp:453:5
*Severity*: Warning
*Message*: this loop is infinite; none of its condition variables (count) are updated in the loop body

```cpp
     450:    * @return counting_semaphore& reference to the counting semaphore.
     451:    */
     452:   counting_semaphore &operator+=(UBaseType_t count) {
>>>  453:     while (count--) {
     454:       give();
     455:     }
     456:     return *this;
```

**Violation 2**: freertos_semaphore.hpp:453:5
*Severity*: Warning
*Message*: this loop is infinite; none of its condition variables (count) are updated in the loop body

```cpp
     450:    * @return counting_semaphore& reference to the counting semaphore.
     451:    */
     452:   counting_semaphore &operator+=(UBaseType_t count) {
>>>  453:     while (count--) {
     454:       give();
     455:     }
     456:     return *this;
```

#### misc-include-cleaner (21 violation(s))

**Violation 1**: freertos.hpp:39:1
*Severity*: Warning
*Message*: included header freertos_event_group.hpp is not used directly

```cpp
      36: #error "This header is for C++ only"
      37: #endif
      38: 
>>>   39: #include "freertos_event_group.hpp"
      40: #include "freertos_message_buffer.hpp"
      41: #include "freertos_queue.hpp"
      42: #include "freertos_semaphore.hpp"
```

**Violation 2**: freertos.hpp:40:1
*Severity*: Warning
*Message*: included header freertos_message_buffer.hpp is not used directly

```cpp
      37: #endif
      38: 
      39: #include "freertos_event_group.hpp"
>>>   40: #include "freertos_message_buffer.hpp"
      41: #include "freertos_queue.hpp"
      42: #include "freertos_semaphore.hpp"
      43: #include "freertos_stream_buffer.hpp"
```

**Violation 3**: freertos.hpp:41:1
*Severity*: Warning
*Message*: included header freertos_queue.hpp is not used directly

```cpp
      38: 
      39: #include "freertos_event_group.hpp"
      40: #include "freertos_message_buffer.hpp"
>>>   41: #include "freertos_queue.hpp"
      42: #include "freertos_semaphore.hpp"
      43: #include "freertos_stream_buffer.hpp"
      44: #include "freertos_sw_timer.hpp"
```

**Violation 4**: freertos.hpp:42:1
*Severity*: Warning
*Message*: included header freertos_semaphore.hpp is not used directly

```cpp
      39: #include "freertos_event_group.hpp"
      40: #include "freertos_message_buffer.hpp"
      41: #include "freertos_queue.hpp"
>>>   42: #include "freertos_semaphore.hpp"
      43: #include "freertos_stream_buffer.hpp"
      44: #include "freertos_sw_timer.hpp"
      45: #include "freertos_task.hpp"
```

**Violation 5**: freertos.hpp:43:1
*Severity*: Warning
*Message*: included header freertos_stream_buffer.hpp is not used directly

```cpp
      40: #include "freertos_message_buffer.hpp"
      41: #include "freertos_queue.hpp"
      42: #include "freertos_semaphore.hpp"
>>>   43: #include "freertos_stream_buffer.hpp"
      44: #include "freertos_sw_timer.hpp"
      45: #include "freertos_task.hpp"
```

**Violation 6**: freertos.hpp:44:1
*Severity*: Warning
*Message*: included header freertos_sw_timer.hpp is not used directly

```cpp
      41: #include "freertos_queue.hpp"
      42: #include "freertos_semaphore.hpp"
      43: #include "freertos_stream_buffer.hpp"
>>>   44: #include "freertos_sw_timer.hpp"
      45: #include "freertos_task.hpp"
```

**Violation 7**: freertos.hpp:45:1
*Severity*: Warning
*Message*: included header freertos_task.hpp is not used directly

```cpp
      42: #include "freertos_semaphore.hpp"
      43: #include "freertos_stream_buffer.hpp"
      44: #include "freertos_sw_timer.hpp"
>>>   45: #include "freertos_task.hpp"
```

**Violation 8**: freertos_message_buffer.hpp:40:1
*Severity*: Warning
*Message*: included header array is not used directly

```cpp
      37: #endif
      38: 
      39: #include <FreeRTOS.h>
>>>   40: #include <array>
      41: #include <chrono>
      42: #include <message_buffer.h>
      43: 
```

**Violation 9**: freertos_message_buffer.hpp:94:11
*Severity*: Warning
*Message*: no header providing "size_t" is directly included

```cpp
      91:  * @tparam MessageBufferSize size of the message buffer
      92:  * @tparam MessageBufferAllocator type of the message buffer allocator
      93:  */
>>>   94: template <size_t MessageBufferSize, typename MessageBufferAllocator>
      95: class message_buffer {
      96:   MessageBufferAllocator m_allocator{};
      97:   MessageBufferHandle_t m_message_buffer{nullptr};
```

**Violation 10**: freertos_queue.hpp:40:1
*Severity*: Warning
*Message*: included header array is not used directly

```cpp
      37: #endif
      38: 
      39: #include <FreeRTOS.h>
>>>   40: #include <array>
      41: #include <chrono>
      42: #include <optional>
      43: #include <queue.h>
```

**Violation 11**: freertos_queue.hpp:188:11
*Severity*: Warning
*Message*: no header providing "size_t" is directly included

```cpp
     185:  * static_queue;
     186:  * ```
     187:  */
>>>  188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
     190:   QueueHandle_t m_queue{nullptr};
     191: 
```

**Violation 12**: freertos_semaphore.hpp:469:3
*Severity*: Warning
*Message*: no header providing "uint8_t" is directly included

```cpp
     466: template <typename SemaphoreAllocator> class mutex {
     467:   SemaphoreAllocator m_allocator{};
     468:   SemaphoreHandle_t m_semaphore{nullptr};
>>>  469:   uint8_t m_locked : 1;
     470: 
     471: public:
     472:   /**
```

**Violation 13**: freertos_stream_buffer.hpp:40:1
*Severity*: Warning
*Message*: included header array is not used directly

```cpp
      37: #endif
      38: 
      39: #include <FreeRTOS.h>
>>>   40: #include <array>
      41: #include <chrono>
      42: #include <optional>
      43: #include <stream_buffer.h>
```

**Violation 14**: freertos_stream_buffer.hpp:42:1
*Severity*: Warning
*Message*: included header optional is not used directly

```cpp
      39: #include <FreeRTOS.h>
      40: #include <array>
      41: #include <chrono>
>>>   42: #include <optional>
      43: #include <stream_buffer.h>
      44: 
      45: namespace freertos {
```

**Violation 15**: freertos_stream_buffer.hpp:96:11
*Severity*: Warning
*Message*: no header providing "size_t" is directly included

```cpp
      93:  * @tparam StreamBufferSize size of the stream buffer
      94:  * @tparam StreamBufferAllocator type of the stream buffer allocator
      95:  */
>>>   96: template <size_t StreamBufferSize, typename StreamBufferAllocator>
      97: class stream_buffer {
      98:   StreamBufferAllocator m_allocator{};
      99:   StreamBufferHandle_t m_stream_buffer{nullptr};
```

**Violation 16**: freertos_sw_timer.hpp:40:1
*Severity*: Warning
*Message*: included header chrono is not used directly

```cpp
      37: #endif
      38: 
      39: #include <FreeRTOS.h>
>>>   40: #include <chrono>
      41: #include <cstdbool>
      42: #include <ctime>
      43: #include <functional>
```

**Violation 17**: freertos_sw_timer.hpp:43:1
*Severity*: Warning
*Message*: included header functional is not used directly

```cpp
      40: #include <chrono>
      41: #include <cstdbool>
      42: #include <ctime>
>>>   43: #include <functional>
      44: #include <task.h>
      45: #include <timers.h>
      46: 
```

**Violation 18**: freertos_task.hpp:40:1
*Severity*: Warning
*Message*: included header array is not used directly

```cpp
      37: #endif
      38: 
      39: #include <FreeRTOS.h>
>>>   40: #include <array>
      41: #include <cassert>
      42: #include <chrono>
      43: #include <functional>
```

**Violation 19**: freertos_task.cc:41:37
*Severity*: Warning
*Message*: no header providing "std::chrono::system_clock" is directly included

```cpp
      38: void delay_until(TickType_t &previousWakeTime, TickType_t period) {
      39:   vTaskDelayUntil(&previousWakeTime, period);
      40: }
>>>   41: void delay_until(const std::chrono::system_clock::time_point &wakeTime) {
      42:   auto now = std::chrono::system_clock::now();
      43:   if (wakeTime > now) {
      44:     delay(wakeTime - now);
```

**Violation 20**: freertos_task.cc:47:37
*Severity*: Warning
*Message*: no header providing "std::chrono::steady_clock" is directly included

```cpp
      44:     delay(wakeTime - now);
      45:   }
      46: }
>>>   47: void delay_until(const std::chrono::steady_clock::time_point &wakeTime) {
      48:   auto now = std::chrono::steady_clock::now();
      49:   if (wakeTime > now) {
      50:     delay(wakeTime - now);
```

**Violation 21**: freertos_task.cc:61:14
*Severity*: Warning
*Message*: no header providing "std::chrono::milliseconds" is directly included

```cpp
      58: #endif
      59: TickType_t tick_count(void) { return xTaskGetTickCount(); }
      60: TickType_t tick_count_isr(void) { return xTaskGetTickCountFromISR(); }
>>>   61: std::chrono::milliseconds time_since_scheduler_started(void) {
      62:   return std::chrono::milliseconds{tick_count() * portTICK_PERIOD_MS};
      63: }
      64: std::chrono::milliseconds time_since_scheduler_started_isr(void) {
```

#### readability-identifier-length (30 violation(s))

**Violation 1**: freertos_queue.hpp:210:12
*Severity*: Warning
*Message*: variable name 'n' is too short, expected at least 3 characters

```cpp
     207:   queue(queue &&src) = delete;
     208:   ~queue(void) {
     209:     if (m_queue) {
>>>  210:       auto n = pcQueueGetName(m_queue);
     211:       if (n) {
     212:         vQueueUnregisterQueue(m_queue);
     213:       }
```

**Violation 2**: freertos_semaphore.hpp:502:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     499:    * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
     500:    */
     501:   BaseType_t unlock() {
>>>  502:     auto rc = xSemaphoreGive(m_semaphore);
     503:     if (rc) {
     504:       m_locked = false;
     505:     }
```

**Violation 3**: freertos_semaphore.hpp:517:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     514:    * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
     515:    */
     516:   BaseType_t unlock_isr(BaseType_t &high_priority_task_woken) {
>>>  517:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     518:     if (rc) {
     519:       m_locked = false;
     520:     }
```

**Violation 4**: freertos_semaphore.hpp:531:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     528:    */
     529:   BaseType_t unlock_isr(void) {
     530:     BaseType_t high_priority_task_woken = pdFALSE;
>>>  531:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     532:     if (rc) {
     533:       m_locked = false;
     534:     }
```

**Violation 5**: freertos_semaphore.hpp:545:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     542:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     543:    */
     544:   BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
>>>  545:     auto rc = xSemaphoreTake(m_semaphore, ticks_to_wait);
     546:     if (rc) {
     547:       m_locked = true;
     548:     }
```

**Violation 6**: freertos_semaphore.hpp:560:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     557:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     558:    */
     559:   BaseType_t lock_isr(BaseType_t &high_priority_task_woken) {
>>>  560:     auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
     561:     if (rc) {
     562:       m_locked = true;
     563:     }
```

**Violation 7**: freertos_semaphore.hpp:574:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     571:    */
     572:   BaseType_t lock_isr(void) {
     573:     BaseType_t high_priority_task_woken = pdFALSE;
>>>  574:     auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
     575:     if (rc) {
     576:       m_locked = true;
     577:     }
```

**Violation 8**: freertos_semaphore.hpp:598:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     595:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     596:    */
     597:   BaseType_t try_lock() {
>>>  598:     auto rc = xSemaphoreTake(m_semaphore, 0);
     599:     if (rc) {
     600:       m_locked = true;
     601:     }
```

**Violation 9**: freertos_semaphore.hpp:655:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     652:    * otherwise pdFALSE.
     653:    */
     654:   BaseType_t unlock() {
>>>  655:     auto rc = xSemaphoreGiveRecursive(m_semaphore);
     656:     if (rc && m_recursions_count > 0) {
     657:       m_recursions_count--;
     658:     }
```

**Violation 10**: freertos_semaphore.hpp:670:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     667:    * @return BaseType_t pdTRUE if the recursive mutex was successfully unlocked,
     668:    */
     669:   BaseType_t unlock_isr(BaseType_t &high_priority_task_woken) {
>>>  670:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     671:     if (rc && m_recursions_count > 0) {
     672:       m_recursions_count--;
     673:     }
```

**Violation 11**: freertos_semaphore.hpp:684:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     681:    */
     682:   BaseType_t unlock_isr(void) {
     683:     BaseType_t high_priority_task_woken = pdFALSE;
>>>  684:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     685:     if (rc && m_recursions_count > 0) {
     686:       m_recursions_count--;
     687:     }
```

**Violation 12**: freertos_semaphore.hpp:698:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     695:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     696:    */
     697:   BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
>>>  698:     auto rc = xSemaphoreTakeRecursive(m_semaphore, ticks_to_wait);
     699:     if (rc) {
     700:       m_recursions_count++;
     701:     }
```

**Violation 13**: freertos_semaphore.hpp:713:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     710:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     711:    */
     712:   BaseType_t lock_isr(BaseType_t &high_priority_task_woken) {
>>>  713:     auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
     714:     if (rc) {
     715:       m_recursions_count++;
     716:     }
```

**Violation 14**: freertos_semaphore.hpp:727:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     724:    */
     725:   BaseType_t lock_isr(void) {
     726:     BaseType_t high_priority_task_woken = pdFALSE;
>>>  727:     auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
     728:     if (rc) {
     729:       m_recursions_count++;
     730:     }
```

**Violation 15**: freertos_semaphore.hpp:751:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     748:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     749:    */
     750:   BaseType_t try_lock() {
>>>  751:     auto rc = xSemaphoreTakeRecursive(m_semaphore, 0);
     752:     if (rc) {
     753:       m_recursions_count++;
     754:     }
```

**Violation 16**: freertos_queue.hpp:210:12
*Severity*: Warning
*Message*: variable name 'n' is too short, expected at least 3 characters

```cpp
     207:   queue(queue &&src) = delete;
     208:   ~queue(void) {
     209:     if (m_queue) {
>>>  210:       auto n = pcQueueGetName(m_queue);
     211:       if (n) {
     212:         vQueueUnregisterQueue(m_queue);
     213:       }
```

**Violation 17**: freertos_semaphore.hpp:502:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     499:    * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
     500:    */
     501:   BaseType_t unlock() {
>>>  502:     auto rc = xSemaphoreGive(m_semaphore);
     503:     if (rc) {
     504:       m_locked = false;
     505:     }
```

**Violation 18**: freertos_semaphore.hpp:517:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     514:    * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
     515:    */
     516:   BaseType_t unlock_isr(BaseType_t &high_priority_task_woken) {
>>>  517:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     518:     if (rc) {
     519:       m_locked = false;
     520:     }
```

**Violation 19**: freertos_semaphore.hpp:531:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     528:    */
     529:   BaseType_t unlock_isr(void) {
     530:     BaseType_t high_priority_task_woken = pdFALSE;
>>>  531:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     532:     if (rc) {
     533:       m_locked = false;
     534:     }
```

**Violation 20**: freertos_semaphore.hpp:545:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     542:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     543:    */
     544:   BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
>>>  545:     auto rc = xSemaphoreTake(m_semaphore, ticks_to_wait);
     546:     if (rc) {
     547:       m_locked = true;
     548:     }
```

**Violation 21**: freertos_semaphore.hpp:560:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     557:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     558:    */
     559:   BaseType_t lock_isr(BaseType_t &high_priority_task_woken) {
>>>  560:     auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
     561:     if (rc) {
     562:       m_locked = true;
     563:     }
```

**Violation 22**: freertos_semaphore.hpp:574:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     571:    */
     572:   BaseType_t lock_isr(void) {
     573:     BaseType_t high_priority_task_woken = pdFALSE;
>>>  574:     auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
     575:     if (rc) {
     576:       m_locked = true;
     577:     }
```

**Violation 23**: freertos_semaphore.hpp:598:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     595:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     596:    */
     597:   BaseType_t try_lock() {
>>>  598:     auto rc = xSemaphoreTake(m_semaphore, 0);
     599:     if (rc) {
     600:       m_locked = true;
     601:     }
```

**Violation 24**: freertos_semaphore.hpp:655:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     652:    * otherwise pdFALSE.
     653:    */
     654:   BaseType_t unlock() {
>>>  655:     auto rc = xSemaphoreGiveRecursive(m_semaphore);
     656:     if (rc && m_recursions_count > 0) {
     657:       m_recursions_count--;
     658:     }
```

**Violation 25**: freertos_semaphore.hpp:670:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     667:    * @return BaseType_t pdTRUE if the recursive mutex was successfully unlocked,
     668:    */
     669:   BaseType_t unlock_isr(BaseType_t &high_priority_task_woken) {
>>>  670:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     671:     if (rc && m_recursions_count > 0) {
     672:       m_recursions_count--;
     673:     }
```

**Violation 26**: freertos_semaphore.hpp:684:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     681:    */
     682:   BaseType_t unlock_isr(void) {
     683:     BaseType_t high_priority_task_woken = pdFALSE;
>>>  684:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     685:     if (rc && m_recursions_count > 0) {
     686:       m_recursions_count--;
     687:     }
```

**Violation 27**: freertos_semaphore.hpp:698:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     695:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     696:    */
     697:   BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
>>>  698:     auto rc = xSemaphoreTakeRecursive(m_semaphore, ticks_to_wait);
     699:     if (rc) {
     700:       m_recursions_count++;
     701:     }
```

**Violation 28**: freertos_semaphore.hpp:713:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     710:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     711:    */
     712:   BaseType_t lock_isr(BaseType_t &high_priority_task_woken) {
>>>  713:     auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
     714:     if (rc) {
     715:       m_recursions_count++;
     716:     }
```

**Violation 29**: freertos_semaphore.hpp:727:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     724:    */
     725:   BaseType_t lock_isr(void) {
     726:     BaseType_t high_priority_task_woken = pdFALSE;
>>>  727:     auto rc = xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
     728:     if (rc) {
     729:       m_recursions_count++;
     730:     }
```

**Violation 30**: freertos_semaphore.hpp:751:10
*Severity*: Warning
*Message*: variable name 'rc' is too short, expected at least 3 characters

```cpp
     748:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     749:    */
     750:   BaseType_t try_lock() {
>>>  751:     auto rc = xSemaphoreTakeRecursive(m_semaphore, 0);
     752:     if (rc) {
     753:       m_recursions_count++;
     754:     }
```

#### readability-identifier-naming (11 violation(s))

**Description**: Follow consistent naming conventions for identifiers

**Violation 1**: freertos_semaphore.hpp:222:16
*Severity*: Warning
*Message*: invalid case style for variable 'xHigherPriorityTaskWoken'

```cpp
     219:    *
     220:    */
     221:   BaseType_t give_isr(void) {
>>>  222:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     223:     return xSemaphoreGiveFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     224:   }
     225:   /**
```

**Violation 2**: freertos_semaphore.hpp:260:16
*Severity*: Warning
*Message*: invalid case style for variable 'xHigherPriorityTaskWoken'

```cpp
     257:    *
     258:    */
     259:   BaseType_t take_isr(void) {
>>>  260:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     261:     return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     262:   }
     263:   /**
```

**Violation 3**: freertos_semaphore.hpp:344:16
*Severity*: Warning
*Message*: invalid case style for variable 'xHigherPriorityTaskWoken'

```cpp
     341:    *
     342:    */
     343:   BaseType_t give_isr(void) {
>>>  344:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     345:     return xSemaphoreGiveFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     346:   }
     347:   /**
```

**Violation 4**: freertos_semaphore.hpp:382:16
*Severity*: Warning
*Message*: invalid case style for variable 'xHigherPriorityTaskWoken'

```cpp
     379:    *
     380:    */
     381:   BaseType_t take_isr(void) {
>>>  382:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     383:     return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     384:   }
     385:   /**
```

**Violation 5**: freertos_task.hpp:174:10
*Severity*: Warning
*Message*: invalid case style for variable 'pThis'

```cpp
     171:   TaskHandle_t m_hTask;
     172: 
     173:   static void task_exec(void *context) {
>>>  174:     auto pThis = static_cast<task *>(context);
     175:     assert(nullptr != pThis);
     176: #if INCLUDE_vTaskSuspend
     177:     if (pThis->m_start_suspended) {
```

**Violation 6**: freertos_semaphore.hpp:222:16
*Severity*: Warning
*Message*: invalid case style for variable 'xHigherPriorityTaskWoken'

```cpp
     219:    *
     220:    */
     221:   BaseType_t give_isr(void) {
>>>  222:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     223:     return xSemaphoreGiveFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     224:   }
     225:   /**
```

**Violation 7**: freertos_semaphore.hpp:260:16
*Severity*: Warning
*Message*: invalid case style for variable 'xHigherPriorityTaskWoken'

```cpp
     257:    *
     258:    */
     259:   BaseType_t take_isr(void) {
>>>  260:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     261:     return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     262:   }
     263:   /**
```

**Violation 8**: freertos_semaphore.hpp:344:16
*Severity*: Warning
*Message*: invalid case style for variable 'xHigherPriorityTaskWoken'

```cpp
     341:    *
     342:    */
     343:   BaseType_t give_isr(void) {
>>>  344:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     345:     return xSemaphoreGiveFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     346:   }
     347:   /**
```

**Violation 9**: freertos_semaphore.hpp:382:16
*Severity*: Warning
*Message*: invalid case style for variable 'xHigherPriorityTaskWoken'

```cpp
     379:    *
     380:    */
     381:   BaseType_t take_isr(void) {
>>>  382:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     383:     return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     384:   }
     385:   /**
```

**Violation 10**: freertos_task.hpp:174:10
*Severity*: Warning
*Message*: invalid case style for variable 'pThis'

```cpp
     171:   TaskHandle_t m_hTask;
     172: 
     173:   static void task_exec(void *context) {
>>>  174:     auto pThis = static_cast<task *>(context);
     175:     assert(nullptr != pThis);
     176: #if INCLUDE_vTaskSuspend
     177:     if (pThis->m_start_suspended) {
```

**Violation 11**: freertos_task.hpp:174:10
*Severity*: Warning
*Message*: invalid case style for variable 'pThis'

```cpp
     171:   TaskHandle_t m_hTask;
     172: 
     173:   static void task_exec(void *context) {
>>>  174:     auto pThis = static_cast<task *>(context);
     175:     assert(nullptr != pThis);
     176: #if INCLUDE_vTaskSuspend
     177:     if (pThis->m_start_suspended) {
```

#### readability-implicit-bool-conversion (2 violation(s))

**Violation 1**: freertos_semaphore.hpp:476:63
*Severity*: Warning
*Message*: implicit conversion 'bool' -> 'uint8_t' (aka 'unsigned char')

```cpp
     473:    * @brief Construct a new mutex object
     474:    *
     475:    */
>>>  476:   mutex() : m_semaphore{m_allocator.create_mutex()}, m_locked{false} {
     477:     configASSERT(m_semaphore);
     478:   }
     479:   mutex(const mutex &) = delete;
```

**Violation 2**: freertos_semaphore.hpp:476:63
*Severity*: Warning
*Message*: implicit conversion 'bool' -> 'uint8_t' (aka 'unsigned char')

```cpp
     473:    * @brief Construct a new mutex object
     474:    *
     475:    */
>>>  476:   mutex() : m_semaphore{m_allocator.create_mutex()}, m_locked{false} {
     477:     configASSERT(m_semaphore);
     478:   }
     479:   mutex(const mutex &) = delete;
```


