# Static Analysis Report

## Overview

**Static Analysis Tools**: clang-tidy + MISRA C++ (cppcheck)
**Analysis Scope**: Library modules only - src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-* + MISRA C 2012 (applicable to C++)

## clang-tidy Analysis

## Statistics

### Summary

- **Total Issues Found**: 34438
- **Warnings**: 34437
- **Errors**: 1
- **Files Analyzed**: 18
- **Unique Check Types**: 2

### Issues by Check Category

- **cppcoreguidelines**: 16 issues
- **cert**: 2 issues

### Files with Issues

- **freertos_task.hpp**: 14 issues
- **freertos_semaphore.hpp**: 2 issues
- **freertos_sw_timer.hpp**: 2 issues

### Analyzed Files

- freertos.hpp
- freertos.hpp.
- freertos_event_group.hpp
- freertos_event_group.hpp.
- freertos_message_buffer.hpp
- freertos_message_buffer.hpp.
- freertos_queue.hpp
- freertos_queue.hpp.
- freertos_semaphore.hpp
- freertos_semaphore.hpp.
- freertos_stream_buffer.hpp
- freertos_stream_buffer.hpp.
- freertos_sw_timer.hpp
- freertos_sw_timer.hpp.
- freertos_task.cc
- freertos_task.cc.
- freertos_task.hpp
- freertos_task.hpp.

## MISRA C++ Analysis

### Summary

- **Total MISRA Violations**: 259
- **Unique Rules Violated**: 17
- **Files Analyzed**: 8
- **Analysis Errors**: 0
- **Other Issues**: 0

### Violations by Severity

- **Style**: 259

### Violations by Rule

- **MISRA C 2012 Rule 10.4**: 4 violation(s) - Both operands of an operator in which the usual arithmetic conversions are performed shall have the same essential type
- **MISRA C 2012 Rule 12.3**: 36 violation(s) - The comma operator should not be used
- **MISRA C 2012 Rule 13.4**: 1 violation(s) - The result of an assignment operator should not be used
- **MISRA C 2012 Rule 15.5**: 17 violation(s) - A function should have a single point of exit at the end
- **MISRA C 2012 Rule 16.3**: 13 violation(s) - An unconditional break statement shall terminate every switch-clause
- **MISRA C 2012 Rule 17.3**: 7 violation(s) - A function shall not be declared implicitly
- **MISRA C 2012 Rule 17.8**: 14 violation(s) - A function parameter should not be modified
- **MISRA C 2012 Rule 2.3**: 2 violation(s) - A project should not contain unused type declarations
- **MISRA C 2012 Rule 2.7**: 15 violation(s) - There should be no unused parameters in functions
- **MISRA C 2012 Rule 20.9**: 25 violation(s) - All identifiers used in the controlling expression of #if or #elif preprocessing directives shall be #define'd before evaluation
- **MISRA C 2012 Rule 21.2**: 1 violation(s) - A reserved identifier or reserved macro name shall not be declared
- **MISRA C 2012 Rule 3.1**: 2 violation(s) - The character sequences /* and // shall not appear in /* comment */
- **MISRA C 2012 Rule 5.8**: 16 violation(s) - Identifiers that define objects or functions with external linkage shall be unique
- **MISRA C 2012 Rule 8.2**: 45 violation(s) - Function types shall be in prototype form with named parameters
- **MISRA C 2012 Rule 8.5**: 6 violation(s) - An external object or function shall be declared once in one and only one file
- **MISRA C 2012 Rule 8.6**: 40 violation(s) - An identifier with external linkage shall have exactly one external definition
- **MISRA C 2012 Rule 8.7**: 15 violation(s) - Functions and objects should not be defined with external linkage if they are referenced from only one translation unit

### Violations by File

- **freertos_event_group.hpp**: 28 violation(s)
- **freertos_message_buffer.hpp**: 34 violation(s)
- **freertos_queue.hpp**: 36 violation(s)
- **freertos_semaphore.hpp**: 40 violation(s)
- **freertos_stream_buffer.hpp**: 32 violation(s)
- **freertos_sw_timer.hpp**: 30 violation(s)
- **freertos_task.cc**: 25 violation(s)
- **freertos_task.hpp**: 34 violation(s)

### Detailed Violations with Code Context

#### MISRA C 2012 Rule 10.4 (4 violation(s))

**Both operands of an operator in which the usual arithmetic conversions are performed shall have the same essential type**

Arithmetic operations should be performed on operands of compatible types to avoid unexpected type conversions and potential loss of precision.

*Rationale: Mixed-type arithmetic can lead to unexpected results due to implicit type conversions.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

#### MISRA C 2012 Rule 12.3 (36 violation(s))

**The comma operator should not be used**

The comma operator can make code difficult to understand and maintain. Its use should be avoided except in specific contexts like for-loop expressions.

*Rationale: The comma operator can obscure control flow and make code harder to read and debug.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

**Violation 8**: freertos_task.cc:170:27
*Style*: misra violation

```cpp

```

**Violation 9**: freertos_semaphore.hpp:180:28
*Style*: misra violation

```cpp
     177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
>>>  180:    * @brief Destruct the binary semaphore object and
     181:    * delete the binary semaphore instance if it was created.
     182:    *
     183:    */
```

**Violation 10**: freertos_queue.hpp:190:29
*Style*: misra violation

```cpp
     187:  */
     188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
>>>  190:   QueueHandle_t m_queue{nullptr};
     191: 
     192: public:
     193:   /**
```

**Violation 11**: freertos_task.hpp:200:30
*Style*: misra violation

```cpp
     197:         m_start_suspended{start_suspended},
     198:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     199:   /**
>>>  200:    * @brief Construct a new task object
     201:    *
     202:    * @param name name of the task
     203:    * @param priority priority of the task
```

**Violation 12**: freertos_message_buffer.hpp:210:31
*Style*: misra violation

```cpp
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
     209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
>>>  210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
     213:    *
```

**Violation 13**: freertos_stream_buffer.hpp:220:32
*Style*: misra violation

```cpp
     217:   /**
     218:    * @brief Send data to the stream buffer from an ISR.
     219:    * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
>>>  220:    *
     221:    * @param data A pointer to the data to be copied into the stream buffer.
     222:    * @param data_size Maximum number of bytes to copy into the stream buffer.
     223:    * @return BaseType_t pdPASS if the data
```

**Violation 14**: freertos_sw_timer.hpp:230:33
*Style*: misra violation

```cpp
     227: 
     228:   /**
     229:    * @brief Method to start the timer.
>>>  230:    * @ref https://www.freertos.org/xTimerStart.html
     231:    *
     232:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     233:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
```

**Violation 15**: freertos_event_group.hpp:240:34
*Style*: misra violation

```cpp
     237:  * @brief Namespace for aliases of RTOS kernel objects that use a static memory
     238:  *
     239:  */
>>>  240: namespace sa {
     241: /**
     242:  * @brief Alias for the event group that uses a static memory allocation.
     243:  *
```

**Violation 16**: freertos_task.cc:250:35
*Style*: misra violation

```cpp

```

**Violation 17**: freertos_semaphore.hpp:260:36
*Style*: misra violation

```cpp
     257:    *
     258:    */
     259:   BaseType_t take_isr(void) {
>>>  260:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     261:     return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     262:   }
     263:   /**
```

**Violation 18**: freertos_queue.hpp:270:37
*Style*: misra violation

```cpp
     267:    *
     268:    * @param item  An item to be posted on the queue.
     269:    * @return BaseType_t  pdPASS if the item was successfully posted, otherwise
>>>  270:    * errQUEUE_FULL.
     271:    */
     272:   BaseType_t send_isr(const T &item) {
     273:     BaseType_t higher_priority_task_woken = pdFALSE;
```

**Violation 19**: freertos_task.hpp:280:38
*Style*: misra violation

```cpp
     277:    */
     278:   void resume(void) { vTaskResume(m_hTask); }
     279:   /**
>>>  280:    * @brief Resume the task from an ISR.
     281:    *
     282:    * @return BaseType_t pdTRUE if the task was resumed, pdFALSE otherwise
     283:    */
```

**Violation 20**: freertos_message_buffer.hpp:290:39
*Style*: misra violation

```cpp

```

**Violation 21**: freertos_stream_buffer.hpp:300:40
*Style*: misra violation

```cpp
     297:     return receive(
     298:         data, data_size,
     299:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
>>>  300:   }
     301:   /**
     302:    * @brief Receive data from the stream buffer from an ISR.
     303:    * @ref https://www.freertos.org/xStreamBufferReceiveFromISR.html
```

**Violation 22**: freertos_sw_timer.hpp:310:41
*Style*: misra violation

```cpp
     307:    *
     308:    * @tparam Rep duration representation type
     309:    * @tparam Period duration period type
>>>  310:    * @param timeout timeout to wait for the timer to stop
     311:    * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
     312:    */
     313:   template <typename Rep, typename Period>
```

**Violation 23**: freertos_event_group.hpp:320:42
*Style*: misra violation

```cpp

```

**Violation 24**: freertos_task.cc:330:43
*Style*: misra violation

```cpp

```

**Violation 25**: freertos_semaphore.hpp:340:44
*Style*: misra violation

```cpp
     337:    * @brief Give the counting semaphore from an ISR.
     338:    * @ref https://www.freertos.org/a00124.html
     339:    *
>>>  340:    * @return BaseType_t pdTRUE if the semaphore was successfully given,
     341:    *
     342:    */
     343:   BaseType_t give_isr(void) {
```

**Violation 26**: freertos_queue.hpp:350:45
*Style*: misra violation

```cpp
     347:    * @ref https://www.freertos.org/xQueueSendToFront.html
     348:    *
     349:    * @param item An item to be posted on the queue.
>>>  350:    * @param timeout Timeout to wait for the space to become available.
     351:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     352:    * errQUEUE_FULL.
     353:    */
```

**Violation 27**: freertos_task.hpp:360:46
*Style*: misra violation

```cpp
     357:     return status;
     358:   }
     359: #endif
>>>  360: #if configUSE_APPLICATION_TASK_TAG
     361:   /**
     362:    * @brief Set the tag of the task.
     363:    *
```

**Violation 28**: freertos_message_buffer.hpp:370:47
*Style*: misra violation

```cpp

```

**Violation 29**: freertos_stream_buffer.hpp:380:48
*Style*: misra violation

```cpp
     377:   BaseType_t empty(void) { return xStreamBufferIsEmpty(m_stream_buffer); }
     378:   /**
     379:    * @brief Check if the stream buffer is full.
>>>  380:    * @ref https://www.freertos.org/xStreamBufferIsFull.html
     381:    *
     382:    * @return BaseType_t pdTRUE if the stream buffer is full, pdFALSE otherwise.
     383:    */
```

**Violation 30**: freertos_sw_timer.hpp:390:49
*Style*: misra violation

```cpp
     387:   /**
     388:    * @brief Method to reset the timer from an ISR.
     389:    * @ref https://www.freertos.org/xTimerResetFromISR.html
>>>  390:    *
     391:    * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
     392:    */
     393:   BaseType_t reset_isr(void) {
```

**Violation 31**: freertos_event_group.hpp:400:50
*Style*: misra violation

```cpp

```

**Violation 32**: freertos_task.cc:410:51
*Style*: misra violation

```cpp

```

**Violation 33**: freertos_semaphore.hpp:420:52
*Style*: misra violation

```cpp
     417:    * @brief Give the counting semaphore.
     418:    * Note: Post-increment returns reference instead of copy for RAII safety
     419:    *
>>>  420:    * @return counting_semaphore& reference to the counting semaphore.
     421:    */
     422:   // NOLINTNEXTLINE(cert-dcl21-cpp): RAII class, copy is deleted -
     423:   // post-increment returns reference instead of copy
```

**Violation 34**: freertos_queue.hpp:430:53
*Style*: misra violation

```cpp
     427:   template <typename Rep, typename Period>
     428:   BaseType_t receive(T &item,
     429:                      const std::chrono::duration<Rep, Period> &timeout) {
>>>  430:     return receive(
     431:         item, pdMS_TO_TICKS(
     432:                   std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
     433:                       .count()));
```

**Violation 35**: freertos_task.hpp:440:54
*Style*: misra violation

```cpp
     437:   uint32_t notify_take(BaseType_t clearCountOnExit = pdTRUE,
     438:                        TickType_t ticksToWait = portMAX_DELAY) {
     439:     return ulTaskNotifyTake(clearCountOnExit, ticksToWait);
>>>  440:   }
     441:   /**
     442:    * @brief Take a notification from the task.
     443:    *
```

**Violation 36**: freertos_message_buffer.hpp:450:55
*Style*: misra violation

```cpp

```

#### MISRA C 2012 Rule 13.4 (1 violation(s))

**The result of an assignment operator should not be used**

Assignment expressions should not be used as sub-expressions. Assignments should be separate statements to improve readability and avoid confusion.

*Rationale: Using assignment results as expressions can make code harder to read and may hide logical errors.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

#### MISRA C 2012 Rule 15.5 (17 violation(s))

**A function should have a single point of exit at the end**

Functions should have one return statement at the end rather than multiple return points throughout the function body. This improves code clarity and makes debugging easier.

*Rationale: Single exit points make functions easier to understand, debug, and maintain.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

**Violation 8**: freertos_task.cc:170:27
*Style*: misra violation

```cpp

```

**Violation 9**: freertos_semaphore.hpp:180:28
*Style*: misra violation

```cpp
     177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
>>>  180:    * @brief Destruct the binary semaphore object and
     181:    * delete the binary semaphore instance if it was created.
     182:    *
     183:    */
```

**Violation 10**: freertos_queue.hpp:190:29
*Style*: misra violation

```cpp
     187:  */
     188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
>>>  190:   QueueHandle_t m_queue{nullptr};
     191: 
     192: public:
     193:   /**
```

**Violation 11**: freertos_task.hpp:200:30
*Style*: misra violation

```cpp
     197:         m_start_suspended{start_suspended},
     198:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     199:   /**
>>>  200:    * @brief Construct a new task object
     201:    *
     202:    * @param name name of the task
     203:    * @param priority priority of the task
```

**Violation 12**: freertos_message_buffer.hpp:210:31
*Style*: misra violation

```cpp
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
     209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
>>>  210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
     213:    *
```

**Violation 13**: freertos_stream_buffer.hpp:220:32
*Style*: misra violation

```cpp
     217:   /**
     218:    * @brief Send data to the stream buffer from an ISR.
     219:    * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
>>>  220:    *
     221:    * @param data A pointer to the data to be copied into the stream buffer.
     222:    * @param data_size Maximum number of bytes to copy into the stream buffer.
     223:    * @return BaseType_t pdPASS if the data
```

**Violation 14**: freertos_sw_timer.hpp:230:33
*Style*: misra violation

```cpp
     227: 
     228:   /**
     229:    * @brief Method to start the timer.
>>>  230:    * @ref https://www.freertos.org/xTimerStart.html
     231:    *
     232:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     233:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
```

**Violation 15**: freertos_event_group.hpp:240:34
*Style*: misra violation

```cpp
     237:  * @brief Namespace for aliases of RTOS kernel objects that use a static memory
     238:  *
     239:  */
>>>  240: namespace sa {
     241: /**
     242:  * @brief Alias for the event group that uses a static memory allocation.
     243:  *
```

**Violation 16**: freertos_task.cc:250:35
*Style*: misra violation

```cpp

```

**Violation 17**: freertos_semaphore.hpp:260:36
*Style*: misra violation

```cpp
     257:    *
     258:    */
     259:   BaseType_t take_isr(void) {
>>>  260:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     261:     return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     262:   }
     263:   /**
```

#### MISRA C 2012 Rule 16.3 (13 violation(s))

**An unconditional break statement shall terminate every switch-clause**

Every case in a switch statement should end with a break statement to prevent fall-through behavior, unless fall-through is explicitly intended and documented.

*Rationale: Missing break statements can cause unintended fall-through behavior that leads to bugs.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

**Violation 8**: freertos_task.cc:170:27
*Style*: misra violation

```cpp

```

**Violation 9**: freertos_semaphore.hpp:180:28
*Style*: misra violation

```cpp
     177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
>>>  180:    * @brief Destruct the binary semaphore object and
     181:    * delete the binary semaphore instance if it was created.
     182:    *
     183:    */
```

**Violation 10**: freertos_queue.hpp:190:29
*Style*: misra violation

```cpp
     187:  */
     188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
>>>  190:   QueueHandle_t m_queue{nullptr};
     191: 
     192: public:
     193:   /**
```

**Violation 11**: freertos_task.hpp:200:30
*Style*: misra violation

```cpp
     197:         m_start_suspended{start_suspended},
     198:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     199:   /**
>>>  200:    * @brief Construct a new task object
     201:    *
     202:    * @param name name of the task
     203:    * @param priority priority of the task
```

**Violation 12**: freertos_message_buffer.hpp:210:31
*Style*: misra violation

```cpp
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
     209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
>>>  210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
     213:    *
```

**Violation 13**: freertos_stream_buffer.hpp:220:32
*Style*: misra violation

```cpp
     217:   /**
     218:    * @brief Send data to the stream buffer from an ISR.
     219:    * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
>>>  220:    *
     221:    * @param data A pointer to the data to be copied into the stream buffer.
     222:    * @param data_size Maximum number of bytes to copy into the stream buffer.
     223:    * @return BaseType_t pdPASS if the data
```

#### MISRA C 2012 Rule 17.3 (7 violation(s))

**A function shall not be declared implicitly**

All functions should be explicitly declared before use. Implicit function declarations can lead to type mismatches and undefined behavior.

*Rationale: Implicit function declarations can cause type mismatches and make code harder to understand and maintain.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

#### MISRA C 2012 Rule 17.8 (14 violation(s))

**A function parameter should not be modified**

Function parameters should not be modified within the function body. If modification is needed, use a local copy of the parameter.

*Rationale: Modifying parameters can make function behavior less predictable and harder to understand.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

**Violation 8**: freertos_task.cc:170:27
*Style*: misra violation

```cpp

```

**Violation 9**: freertos_semaphore.hpp:180:28
*Style*: misra violation

```cpp
     177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
>>>  180:    * @brief Destruct the binary semaphore object and
     181:    * delete the binary semaphore instance if it was created.
     182:    *
     183:    */
```

**Violation 10**: freertos_queue.hpp:190:29
*Style*: misra violation

```cpp
     187:  */
     188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
>>>  190:   QueueHandle_t m_queue{nullptr};
     191: 
     192: public:
     193:   /**
```

**Violation 11**: freertos_task.hpp:200:30
*Style*: misra violation

```cpp
     197:         m_start_suspended{start_suspended},
     198:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     199:   /**
>>>  200:    * @brief Construct a new task object
     201:    *
     202:    * @param name name of the task
     203:    * @param priority priority of the task
```

**Violation 12**: freertos_message_buffer.hpp:210:31
*Style*: misra violation

```cpp
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
     209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
>>>  210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
     213:    *
```

**Violation 13**: freertos_stream_buffer.hpp:220:32
*Style*: misra violation

```cpp
     217:   /**
     218:    * @brief Send data to the stream buffer from an ISR.
     219:    * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
>>>  220:    *
     221:    * @param data A pointer to the data to be copied into the stream buffer.
     222:    * @param data_size Maximum number of bytes to copy into the stream buffer.
     223:    * @return BaseType_t pdPASS if the data
```

**Violation 14**: freertos_sw_timer.hpp:230:33
*Style*: misra violation

```cpp
     227: 
     228:   /**
     229:    * @brief Method to start the timer.
>>>  230:    * @ref https://www.freertos.org/xTimerStart.html
     231:    *
     232:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     233:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
```

#### MISRA C 2012 Rule 2.3 (2 violation(s))

**A project should not contain unused type declarations**

Unused type declarations may indicate incomplete code or can be removed to simplify the codebase. This rule helps identify dead code and improves maintainability.

*Rationale: Unused declarations can confuse developers and increase compilation time unnecessarily.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

#### MISRA C 2012 Rule 2.7 (15 violation(s))

**There should be no unused parameters in functions**

Function parameters that are not used may indicate an error in the implementation or can be removed to clarify the interface. If a parameter must remain for interface compatibility, it should be explicitly marked as unused.

*Rationale: Unused parameters can indicate incomplete implementations or interface issues and should be explicitly handled.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

**Violation 8**: freertos_task.cc:170:27
*Style*: misra violation

```cpp

```

**Violation 9**: freertos_semaphore.hpp:180:28
*Style*: misra violation

```cpp
     177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
>>>  180:    * @brief Destruct the binary semaphore object and
     181:    * delete the binary semaphore instance if it was created.
     182:    *
     183:    */
```

**Violation 10**: freertos_queue.hpp:190:29
*Style*: misra violation

```cpp
     187:  */
     188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
>>>  190:   QueueHandle_t m_queue{nullptr};
     191: 
     192: public:
     193:   /**
```

**Violation 11**: freertos_task.hpp:200:30
*Style*: misra violation

```cpp
     197:         m_start_suspended{start_suspended},
     198:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     199:   /**
>>>  200:    * @brief Construct a new task object
     201:    *
     202:    * @param name name of the task
     203:    * @param priority priority of the task
```

**Violation 12**: freertos_message_buffer.hpp:210:31
*Style*: misra violation

```cpp
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
     209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
>>>  210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
     213:    *
```

**Violation 13**: freertos_stream_buffer.hpp:220:32
*Style*: misra violation

```cpp
     217:   /**
     218:    * @brief Send data to the stream buffer from an ISR.
     219:    * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
>>>  220:    *
     221:    * @param data A pointer to the data to be copied into the stream buffer.
     222:    * @param data_size Maximum number of bytes to copy into the stream buffer.
     223:    * @return BaseType_t pdPASS if the data
```

**Violation 14**: freertos_sw_timer.hpp:230:33
*Style*: misra violation

```cpp
     227: 
     228:   /**
     229:    * @brief Method to start the timer.
>>>  230:    * @ref https://www.freertos.org/xTimerStart.html
     231:    *
     232:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     233:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
```

**Violation 15**: freertos_event_group.hpp:240:34
*Style*: misra violation

```cpp
     237:  * @brief Namespace for aliases of RTOS kernel objects that use a static memory
     238:  *
     239:  */
>>>  240: namespace sa {
     241: /**
     242:  * @brief Alias for the event group that uses a static memory allocation.
     243:  *
```

#### MISRA C 2012 Rule 20.9 (25 violation(s))

**All identifiers used in the controlling expression of #if or #elif preprocessing directives shall be #define'd before evaluation**

Preprocessor identifiers used in conditional compilation should be explicitly defined to avoid undefined behavior in preprocessor evaluation.

*Rationale: Undefined preprocessor identifiers evaluate to 0, which may not be the intended behavior.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

**Violation 8**: freertos_task.cc:170:27
*Style*: misra violation

```cpp

```

**Violation 9**: freertos_semaphore.hpp:180:28
*Style*: misra violation

```cpp
     177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
>>>  180:    * @brief Destruct the binary semaphore object and
     181:    * delete the binary semaphore instance if it was created.
     182:    *
     183:    */
```

**Violation 10**: freertos_queue.hpp:190:29
*Style*: misra violation

```cpp
     187:  */
     188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
>>>  190:   QueueHandle_t m_queue{nullptr};
     191: 
     192: public:
     193:   /**
```

**Violation 11**: freertos_task.hpp:200:30
*Style*: misra violation

```cpp
     197:         m_start_suspended{start_suspended},
     198:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     199:   /**
>>>  200:    * @brief Construct a new task object
     201:    *
     202:    * @param name name of the task
     203:    * @param priority priority of the task
```

**Violation 12**: freertos_message_buffer.hpp:210:31
*Style*: misra violation

```cpp
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
     209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
>>>  210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
     213:    *
```

**Violation 13**: freertos_stream_buffer.hpp:220:32
*Style*: misra violation

```cpp
     217:   /**
     218:    * @brief Send data to the stream buffer from an ISR.
     219:    * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
>>>  220:    *
     221:    * @param data A pointer to the data to be copied into the stream buffer.
     222:    * @param data_size Maximum number of bytes to copy into the stream buffer.
     223:    * @return BaseType_t pdPASS if the data
```

**Violation 14**: freertos_sw_timer.hpp:230:33
*Style*: misra violation

```cpp
     227: 
     228:   /**
     229:    * @brief Method to start the timer.
>>>  230:    * @ref https://www.freertos.org/xTimerStart.html
     231:    *
     232:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     233:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
```

**Violation 15**: freertos_event_group.hpp:240:34
*Style*: misra violation

```cpp
     237:  * @brief Namespace for aliases of RTOS kernel objects that use a static memory
     238:  *
     239:  */
>>>  240: namespace sa {
     241: /**
     242:  * @brief Alias for the event group that uses a static memory allocation.
     243:  *
```

**Violation 16**: freertos_task.cc:250:35
*Style*: misra violation

```cpp

```

**Violation 17**: freertos_semaphore.hpp:260:36
*Style*: misra violation

```cpp
     257:    *
     258:    */
     259:   BaseType_t take_isr(void) {
>>>  260:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     261:     return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     262:   }
     263:   /**
```

**Violation 18**: freertos_queue.hpp:270:37
*Style*: misra violation

```cpp
     267:    *
     268:    * @param item  An item to be posted on the queue.
     269:    * @return BaseType_t  pdPASS if the item was successfully posted, otherwise
>>>  270:    * errQUEUE_FULL.
     271:    */
     272:   BaseType_t send_isr(const T &item) {
     273:     BaseType_t higher_priority_task_woken = pdFALSE;
```

**Violation 19**: freertos_task.hpp:280:38
*Style*: misra violation

```cpp
     277:    */
     278:   void resume(void) { vTaskResume(m_hTask); }
     279:   /**
>>>  280:    * @brief Resume the task from an ISR.
     281:    *
     282:    * @return BaseType_t pdTRUE if the task was resumed, pdFALSE otherwise
     283:    */
```

**Violation 20**: freertos_message_buffer.hpp:290:39
*Style*: misra violation

```cpp

```

**Violation 21**: freertos_stream_buffer.hpp:300:40
*Style*: misra violation

```cpp
     297:     return receive(
     298:         data, data_size,
     299:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
>>>  300:   }
     301:   /**
     302:    * @brief Receive data from the stream buffer from an ISR.
     303:    * @ref https://www.freertos.org/xStreamBufferReceiveFromISR.html
```

**Violation 22**: freertos_sw_timer.hpp:310:41
*Style*: misra violation

```cpp
     307:    *
     308:    * @tparam Rep duration representation type
     309:    * @tparam Period duration period type
>>>  310:    * @param timeout timeout to wait for the timer to stop
     311:    * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
     312:    */
     313:   template <typename Rep, typename Period>
```

**Violation 23**: freertos_event_group.hpp:320:42
*Style*: misra violation

```cpp

```

**Violation 24**: freertos_task.cc:330:43
*Style*: misra violation

```cpp

```

**Violation 25**: freertos_semaphore.hpp:340:44
*Style*: misra violation

```cpp
     337:    * @brief Give the counting semaphore from an ISR.
     338:    * @ref https://www.freertos.org/a00124.html
     339:    *
>>>  340:    * @return BaseType_t pdTRUE if the semaphore was successfully given,
     341:    *
     342:    */
     343:   BaseType_t give_isr(void) {
```

#### MISRA C 2012 Rule 21.2 (1 violation(s))

**A reserved identifier or reserved macro name shall not be declared**

Identifiers reserved by the C standard (such as those beginning with underscore) should not be used in user code to avoid conflicts with implementation-defined behavior.

*Rationale: Using reserved identifiers can conflict with compiler and standard library implementations.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

#### MISRA C 2012 Rule 3.1 (2 violation(s))

**The character sequences /* and // shall not appear in /* comment */**

Comment delimiters should not appear within comments to avoid confusion about comment boundaries and prevent accidental code commenting.

*Rationale: Nested comment sequences can lead to unexpected comment boundaries and code being accidentally commented out.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

#### MISRA C 2012 Rule 5.8 (16 violation(s))

**Identifiers that define objects or functions with external linkage shall be unique**

External identifiers must be unique to avoid linking errors and undefined behavior when linking multiple translation units.

*Rationale: Non-unique external identifiers can cause linking errors and unpredictable behavior in multi-file projects.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

**Violation 8**: freertos_task.cc:170:27
*Style*: misra violation

```cpp

```

**Violation 9**: freertos_semaphore.hpp:180:28
*Style*: misra violation

```cpp
     177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
>>>  180:    * @brief Destruct the binary semaphore object and
     181:    * delete the binary semaphore instance if it was created.
     182:    *
     183:    */
```

**Violation 10**: freertos_queue.hpp:190:29
*Style*: misra violation

```cpp
     187:  */
     188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
>>>  190:   QueueHandle_t m_queue{nullptr};
     191: 
     192: public:
     193:   /**
```

**Violation 11**: freertos_task.hpp:200:30
*Style*: misra violation

```cpp
     197:         m_start_suspended{start_suspended},
     198:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     199:   /**
>>>  200:    * @brief Construct a new task object
     201:    *
     202:    * @param name name of the task
     203:    * @param priority priority of the task
```

**Violation 12**: freertos_message_buffer.hpp:210:31
*Style*: misra violation

```cpp
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
     209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
>>>  210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
     213:    *
```

**Violation 13**: freertos_stream_buffer.hpp:220:32
*Style*: misra violation

```cpp
     217:   /**
     218:    * @brief Send data to the stream buffer from an ISR.
     219:    * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
>>>  220:    *
     221:    * @param data A pointer to the data to be copied into the stream buffer.
     222:    * @param data_size Maximum number of bytes to copy into the stream buffer.
     223:    * @return BaseType_t pdPASS if the data
```

**Violation 14**: freertos_sw_timer.hpp:230:33
*Style*: misra violation

```cpp
     227: 
     228:   /**
     229:    * @brief Method to start the timer.
>>>  230:    * @ref https://www.freertos.org/xTimerStart.html
     231:    *
     232:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     233:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
```

**Violation 15**: freertos_event_group.hpp:240:34
*Style*: misra violation

```cpp
     237:  * @brief Namespace for aliases of RTOS kernel objects that use a static memory
     238:  *
     239:  */
>>>  240: namespace sa {
     241: /**
     242:  * @brief Alias for the event group that uses a static memory allocation.
     243:  *
```

**Violation 16**: freertos_task.cc:250:35
*Style*: misra violation

```cpp

```

#### MISRA C 2012 Rule 8.2 (45 violation(s))

**Function types shall be in prototype form with named parameters**

Function declarations should include parameter names in addition to types. This improves code readability and helps with documentation.

*Rationale: Named parameters in function prototypes make the interface clearer and improve code documentation.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

**Violation 8**: freertos_task.cc:170:27
*Style*: misra violation

```cpp

```

**Violation 9**: freertos_semaphore.hpp:180:28
*Style*: misra violation

```cpp
     177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
>>>  180:    * @brief Destruct the binary semaphore object and
     181:    * delete the binary semaphore instance if it was created.
     182:    *
     183:    */
```

**Violation 10**: freertos_queue.hpp:190:29
*Style*: misra violation

```cpp
     187:  */
     188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
>>>  190:   QueueHandle_t m_queue{nullptr};
     191: 
     192: public:
     193:   /**
```

**Violation 11**: freertos_task.hpp:200:30
*Style*: misra violation

```cpp
     197:         m_start_suspended{start_suspended},
     198:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     199:   /**
>>>  200:    * @brief Construct a new task object
     201:    *
     202:    * @param name name of the task
     203:    * @param priority priority of the task
```

**Violation 12**: freertos_message_buffer.hpp:210:31
*Style*: misra violation

```cpp
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
     209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
>>>  210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
     213:    *
```

**Violation 13**: freertos_stream_buffer.hpp:220:32
*Style*: misra violation

```cpp
     217:   /**
     218:    * @brief Send data to the stream buffer from an ISR.
     219:    * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
>>>  220:    *
     221:    * @param data A pointer to the data to be copied into the stream buffer.
     222:    * @param data_size Maximum number of bytes to copy into the stream buffer.
     223:    * @return BaseType_t pdPASS if the data
```

**Violation 14**: freertos_sw_timer.hpp:230:33
*Style*: misra violation

```cpp
     227: 
     228:   /**
     229:    * @brief Method to start the timer.
>>>  230:    * @ref https://www.freertos.org/xTimerStart.html
     231:    *
     232:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     233:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
```

**Violation 15**: freertos_event_group.hpp:240:34
*Style*: misra violation

```cpp
     237:  * @brief Namespace for aliases of RTOS kernel objects that use a static memory
     238:  *
     239:  */
>>>  240: namespace sa {
     241: /**
     242:  * @brief Alias for the event group that uses a static memory allocation.
     243:  *
```

**Violation 16**: freertos_task.cc:250:35
*Style*: misra violation

```cpp

```

**Violation 17**: freertos_semaphore.hpp:260:36
*Style*: misra violation

```cpp
     257:    *
     258:    */
     259:   BaseType_t take_isr(void) {
>>>  260:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     261:     return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     262:   }
     263:   /**
```

**Violation 18**: freertos_queue.hpp:270:37
*Style*: misra violation

```cpp
     267:    *
     268:    * @param item  An item to be posted on the queue.
     269:    * @return BaseType_t  pdPASS if the item was successfully posted, otherwise
>>>  270:    * errQUEUE_FULL.
     271:    */
     272:   BaseType_t send_isr(const T &item) {
     273:     BaseType_t higher_priority_task_woken = pdFALSE;
```

**Violation 19**: freertos_task.hpp:280:38
*Style*: misra violation

```cpp
     277:    */
     278:   void resume(void) { vTaskResume(m_hTask); }
     279:   /**
>>>  280:    * @brief Resume the task from an ISR.
     281:    *
     282:    * @return BaseType_t pdTRUE if the task was resumed, pdFALSE otherwise
     283:    */
```

**Violation 20**: freertos_message_buffer.hpp:290:39
*Style*: misra violation

```cpp

```

**Violation 21**: freertos_stream_buffer.hpp:300:40
*Style*: misra violation

```cpp
     297:     return receive(
     298:         data, data_size,
     299:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
>>>  300:   }
     301:   /**
     302:    * @brief Receive data from the stream buffer from an ISR.
     303:    * @ref https://www.freertos.org/xStreamBufferReceiveFromISR.html
```

**Violation 22**: freertos_sw_timer.hpp:310:41
*Style*: misra violation

```cpp
     307:    *
     308:    * @tparam Rep duration representation type
     309:    * @tparam Period duration period type
>>>  310:    * @param timeout timeout to wait for the timer to stop
     311:    * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
     312:    */
     313:   template <typename Rep, typename Period>
```

**Violation 23**: freertos_event_group.hpp:320:42
*Style*: misra violation

```cpp

```

**Violation 24**: freertos_task.cc:330:43
*Style*: misra violation

```cpp

```

**Violation 25**: freertos_semaphore.hpp:340:44
*Style*: misra violation

```cpp
     337:    * @brief Give the counting semaphore from an ISR.
     338:    * @ref https://www.freertos.org/a00124.html
     339:    *
>>>  340:    * @return BaseType_t pdTRUE if the semaphore was successfully given,
     341:    *
     342:    */
     343:   BaseType_t give_isr(void) {
```

**Violation 26**: freertos_queue.hpp:350:45
*Style*: misra violation

```cpp
     347:    * @ref https://www.freertos.org/xQueueSendToFront.html
     348:    *
     349:    * @param item An item to be posted on the queue.
>>>  350:    * @param timeout Timeout to wait for the space to become available.
     351:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     352:    * errQUEUE_FULL.
     353:    */
```

**Violation 27**: freertos_task.hpp:360:46
*Style*: misra violation

```cpp
     357:     return status;
     358:   }
     359: #endif
>>>  360: #if configUSE_APPLICATION_TASK_TAG
     361:   /**
     362:    * @brief Set the tag of the task.
     363:    *
```

**Violation 28**: freertos_message_buffer.hpp:370:47
*Style*: misra violation

```cpp

```

**Violation 29**: freertos_stream_buffer.hpp:380:48
*Style*: misra violation

```cpp
     377:   BaseType_t empty(void) { return xStreamBufferIsEmpty(m_stream_buffer); }
     378:   /**
     379:    * @brief Check if the stream buffer is full.
>>>  380:    * @ref https://www.freertos.org/xStreamBufferIsFull.html
     381:    *
     382:    * @return BaseType_t pdTRUE if the stream buffer is full, pdFALSE otherwise.
     383:    */
```

**Violation 30**: freertos_sw_timer.hpp:390:49
*Style*: misra violation

```cpp
     387:   /**
     388:    * @brief Method to reset the timer from an ISR.
     389:    * @ref https://www.freertos.org/xTimerResetFromISR.html
>>>  390:    *
     391:    * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
     392:    */
     393:   BaseType_t reset_isr(void) {
```

**Violation 31**: freertos_event_group.hpp:400:50
*Style*: misra violation

```cpp

```

**Violation 32**: freertos_task.cc:410:51
*Style*: misra violation

```cpp

```

**Violation 33**: freertos_semaphore.hpp:420:52
*Style*: misra violation

```cpp
     417:    * @brief Give the counting semaphore.
     418:    * Note: Post-increment returns reference instead of copy for RAII safety
     419:    *
>>>  420:    * @return counting_semaphore& reference to the counting semaphore.
     421:    */
     422:   // NOLINTNEXTLINE(cert-dcl21-cpp): RAII class, copy is deleted -
     423:   // post-increment returns reference instead of copy
```

**Violation 34**: freertos_queue.hpp:430:53
*Style*: misra violation

```cpp
     427:   template <typename Rep, typename Period>
     428:   BaseType_t receive(T &item,
     429:                      const std::chrono::duration<Rep, Period> &timeout) {
>>>  430:     return receive(
     431:         item, pdMS_TO_TICKS(
     432:                   std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
     433:                       .count()));
```

**Violation 35**: freertos_task.hpp:440:54
*Style*: misra violation

```cpp
     437:   uint32_t notify_take(BaseType_t clearCountOnExit = pdTRUE,
     438:                        TickType_t ticksToWait = portMAX_DELAY) {
     439:     return ulTaskNotifyTake(clearCountOnExit, ticksToWait);
>>>  440:   }
     441:   /**
     442:    * @brief Take a notification from the task.
     443:    *
```

**Violation 36**: freertos_message_buffer.hpp:450:55
*Style*: misra violation

```cpp

```

**Violation 37**: freertos_stream_buffer.hpp:460:56
*Style*: misra violation

```cpp

```

**Violation 38**: freertos_sw_timer.hpp:470:57
*Style*: misra violation

```cpp
     467:   template <typename Rep, typename Period>
     468:   BaseType_t period_isr(const std::chrono::duration<Rep, Period> &new_period,
     469:                         BaseType_t &high_priority_task_woken) {
>>>  470:     return period_isr(
     471:         std::chrono::duration_cast<std::chrono::milliseconds>(new_period)
     472:             .count(),
     473:         high_priority_task_woken);
```

**Violation 39**: freertos_event_group.hpp:480:58
*Style*: misra violation

```cpp

```

**Violation 40**: freertos_task.cc:490:59
*Style*: misra violation

```cpp

```

**Violation 41**: freertos_semaphore.hpp:500:60
*Style*: misra violation

```cpp
     497:    * @ref https://www.freertos.org/a00123.html
     498:    *
     499:    * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
>>>  500:    */
     501:   BaseType_t unlock() {
     502:     auto rc = xSemaphoreGive(m_semaphore);
     503:     if (rc) {
```

**Violation 42**: freertos_queue.hpp:510:61
*Style*: misra violation

```cpp
     507:   /**
     508:    * @brief Return the number of spaces available in the queue.
     509:    * @ref https://www.freertos.org/a00018.html#uxQueueSpacesAvailable
>>>  510:    *
     511:    * @return UBaseType_t The number of spaces available in the queue.
     512:    */
     513:   UBaseType_t spaces_available(void) { return uxQueueSpacesAvailable(m_queue); }
```

**Violation 43**: freertos_task.hpp:520:62
*Style*: misra violation

```cpp
     517:                          uint32_t &notification_value,
     518:                          TickType_t xTicksToWait) {
     519:     return xTaskNotifyWait(ulBitsToClearOnEntry, ulBitsToClearOnExit,
>>>  520:                            &notification_value, xTicksToWait);
     521:   }
     522:   /**
     523:    * @brief Wait for the notification.
```

**Violation 44**: freertos_message_buffer.hpp:530:63
*Style*: misra violation

```cpp

```

**Violation 45**: freertos_stream_buffer.hpp:540:64
*Style*: misra violation

```cpp

```

#### MISRA C 2012 Rule 8.5 (6 violation(s))

**An external object or function shall be declared once in one and only one file**

External objects and functions should have exactly one declaration to avoid multiple definition errors and ensure consistent interfaces.

*Rationale: Multiple declarations can lead to inconsistencies and linking errors.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

#### MISRA C 2012 Rule 8.6 (40 violation(s))

**An identifier with external linkage shall have exactly one external definition**

Each external identifier should be defined exactly once across the entire program to avoid multiple definition errors.

*Rationale: Multiple definitions of the same external identifier violate the one definition rule and can cause linking errors.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

**Violation 8**: freertos_task.cc:170:27
*Style*: misra violation

```cpp

```

**Violation 9**: freertos_semaphore.hpp:180:28
*Style*: misra violation

```cpp
     177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
>>>  180:    * @brief Destruct the binary semaphore object and
     181:    * delete the binary semaphore instance if it was created.
     182:    *
     183:    */
```

**Violation 10**: freertos_queue.hpp:190:29
*Style*: misra violation

```cpp
     187:  */
     188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
>>>  190:   QueueHandle_t m_queue{nullptr};
     191: 
     192: public:
     193:   /**
```

**Violation 11**: freertos_task.hpp:200:30
*Style*: misra violation

```cpp
     197:         m_start_suspended{start_suspended},
     198:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     199:   /**
>>>  200:    * @brief Construct a new task object
     201:    *
     202:    * @param name name of the task
     203:    * @param priority priority of the task
```

**Violation 12**: freertos_message_buffer.hpp:210:31
*Style*: misra violation

```cpp
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
     209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
>>>  210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
     213:    *
```

**Violation 13**: freertos_stream_buffer.hpp:220:32
*Style*: misra violation

```cpp
     217:   /**
     218:    * @brief Send data to the stream buffer from an ISR.
     219:    * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
>>>  220:    *
     221:    * @param data A pointer to the data to be copied into the stream buffer.
     222:    * @param data_size Maximum number of bytes to copy into the stream buffer.
     223:    * @return BaseType_t pdPASS if the data
```

**Violation 14**: freertos_sw_timer.hpp:230:33
*Style*: misra violation

```cpp
     227: 
     228:   /**
     229:    * @brief Method to start the timer.
>>>  230:    * @ref https://www.freertos.org/xTimerStart.html
     231:    *
     232:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     233:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
```

**Violation 15**: freertos_event_group.hpp:240:34
*Style*: misra violation

```cpp
     237:  * @brief Namespace for aliases of RTOS kernel objects that use a static memory
     238:  *
     239:  */
>>>  240: namespace sa {
     241: /**
     242:  * @brief Alias for the event group that uses a static memory allocation.
     243:  *
```

**Violation 16**: freertos_task.cc:250:35
*Style*: misra violation

```cpp

```

**Violation 17**: freertos_semaphore.hpp:260:36
*Style*: misra violation

```cpp
     257:    *
     258:    */
     259:   BaseType_t take_isr(void) {
>>>  260:     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     261:     return xSemaphoreTakeFromISR(m_semaphore, &xHigherPriorityTaskWoken);
     262:   }
     263:   /**
```

**Violation 18**: freertos_queue.hpp:270:37
*Style*: misra violation

```cpp
     267:    *
     268:    * @param item  An item to be posted on the queue.
     269:    * @return BaseType_t  pdPASS if the item was successfully posted, otherwise
>>>  270:    * errQUEUE_FULL.
     271:    */
     272:   BaseType_t send_isr(const T &item) {
     273:     BaseType_t higher_priority_task_woken = pdFALSE;
```

**Violation 19**: freertos_task.hpp:280:38
*Style*: misra violation

```cpp
     277:    */
     278:   void resume(void) { vTaskResume(m_hTask); }
     279:   /**
>>>  280:    * @brief Resume the task from an ISR.
     281:    *
     282:    * @return BaseType_t pdTRUE if the task was resumed, pdFALSE otherwise
     283:    */
```

**Violation 20**: freertos_message_buffer.hpp:290:39
*Style*: misra violation

```cpp

```

**Violation 21**: freertos_stream_buffer.hpp:300:40
*Style*: misra violation

```cpp
     297:     return receive(
     298:         data, data_size,
     299:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
>>>  300:   }
     301:   /**
     302:    * @brief Receive data from the stream buffer from an ISR.
     303:    * @ref https://www.freertos.org/xStreamBufferReceiveFromISR.html
```

**Violation 22**: freertos_sw_timer.hpp:310:41
*Style*: misra violation

```cpp
     307:    *
     308:    * @tparam Rep duration representation type
     309:    * @tparam Period duration period type
>>>  310:    * @param timeout timeout to wait for the timer to stop
     311:    * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
     312:    */
     313:   template <typename Rep, typename Period>
```

**Violation 23**: freertos_event_group.hpp:320:42
*Style*: misra violation

```cpp

```

**Violation 24**: freertos_task.cc:330:43
*Style*: misra violation

```cpp

```

**Violation 25**: freertos_semaphore.hpp:340:44
*Style*: misra violation

```cpp
     337:    * @brief Give the counting semaphore from an ISR.
     338:    * @ref https://www.freertos.org/a00124.html
     339:    *
>>>  340:    * @return BaseType_t pdTRUE if the semaphore was successfully given,
     341:    *
     342:    */
     343:   BaseType_t give_isr(void) {
```

**Violation 26**: freertos_queue.hpp:350:45
*Style*: misra violation

```cpp
     347:    * @ref https://www.freertos.org/xQueueSendToFront.html
     348:    *
     349:    * @param item An item to be posted on the queue.
>>>  350:    * @param timeout Timeout to wait for the space to become available.
     351:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     352:    * errQUEUE_FULL.
     353:    */
```

**Violation 27**: freertos_task.hpp:360:46
*Style*: misra violation

```cpp
     357:     return status;
     358:   }
     359: #endif
>>>  360: #if configUSE_APPLICATION_TASK_TAG
     361:   /**
     362:    * @brief Set the tag of the task.
     363:    *
```

**Violation 28**: freertos_message_buffer.hpp:370:47
*Style*: misra violation

```cpp

```

**Violation 29**: freertos_stream_buffer.hpp:380:48
*Style*: misra violation

```cpp
     377:   BaseType_t empty(void) { return xStreamBufferIsEmpty(m_stream_buffer); }
     378:   /**
     379:    * @brief Check if the stream buffer is full.
>>>  380:    * @ref https://www.freertos.org/xStreamBufferIsFull.html
     381:    *
     382:    * @return BaseType_t pdTRUE if the stream buffer is full, pdFALSE otherwise.
     383:    */
```

**Violation 30**: freertos_sw_timer.hpp:390:49
*Style*: misra violation

```cpp
     387:   /**
     388:    * @brief Method to reset the timer from an ISR.
     389:    * @ref https://www.freertos.org/xTimerResetFromISR.html
>>>  390:    *
     391:    * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
     392:    */
     393:   BaseType_t reset_isr(void) {
```

**Violation 31**: freertos_event_group.hpp:400:50
*Style*: misra violation

```cpp

```

**Violation 32**: freertos_task.cc:410:51
*Style*: misra violation

```cpp

```

**Violation 33**: freertos_semaphore.hpp:420:52
*Style*: misra violation

```cpp
     417:    * @brief Give the counting semaphore.
     418:    * Note: Post-increment returns reference instead of copy for RAII safety
     419:    *
>>>  420:    * @return counting_semaphore& reference to the counting semaphore.
     421:    */
     422:   // NOLINTNEXTLINE(cert-dcl21-cpp): RAII class, copy is deleted -
     423:   // post-increment returns reference instead of copy
```

**Violation 34**: freertos_queue.hpp:430:53
*Style*: misra violation

```cpp
     427:   template <typename Rep, typename Period>
     428:   BaseType_t receive(T &item,
     429:                      const std::chrono::duration<Rep, Period> &timeout) {
>>>  430:     return receive(
     431:         item, pdMS_TO_TICKS(
     432:                   std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
     433:                       .count()));
```

**Violation 35**: freertos_task.hpp:440:54
*Style*: misra violation

```cpp
     437:   uint32_t notify_take(BaseType_t clearCountOnExit = pdTRUE,
     438:                        TickType_t ticksToWait = portMAX_DELAY) {
     439:     return ulTaskNotifyTake(clearCountOnExit, ticksToWait);
>>>  440:   }
     441:   /**
     442:    * @brief Take a notification from the task.
     443:    *
```

**Violation 36**: freertos_message_buffer.hpp:450:55
*Style*: misra violation

```cpp

```

**Violation 37**: freertos_stream_buffer.hpp:460:56
*Style*: misra violation

```cpp

```

**Violation 38**: freertos_sw_timer.hpp:470:57
*Style*: misra violation

```cpp
     467:   template <typename Rep, typename Period>
     468:   BaseType_t period_isr(const std::chrono::duration<Rep, Period> &new_period,
     469:                         BaseType_t &high_priority_task_woken) {
>>>  470:     return period_isr(
     471:         std::chrono::duration_cast<std::chrono::milliseconds>(new_period)
     472:             .count(),
     473:         high_priority_task_woken);
```

**Violation 39**: freertos_event_group.hpp:480:58
*Style*: misra violation

```cpp

```

**Violation 40**: freertos_task.cc:490:59
*Style*: misra violation

```cpp

```

#### MISRA C 2012 Rule 8.7 (15 violation(s))

**Functions and objects should not be defined with external linkage if they are referenced from only one translation unit**

Functions and objects used only within a single file should be declared static to limit their scope and avoid namespace pollution.

*Rationale: Internal linkage prevents namespace pollution and allows for better optimization by the compiler.*

**Violation 1**: freertos_semaphore.hpp:100:20
*Style*: misra violation

```cpp
      97: 
      98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
>>>  100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
     102:  * synchronization. Unlike mutexes, binary semaphores do not implement priority
     103:  * inheritance.
```

**Violation 2**: freertos_queue.hpp:110:21
*Style*: misra violation

```cpp
     107:  * // Create a queue for 10 integers
     108:  * freertos::queue<10, int> int_queue;
     109:  *
>>>  110:  * // Producer task
     111:  * freertos::task<512> producer("Producer", 3, [&]() {
     112:  *     for (int i = 0; i < 20; ++i) {
     113:  *         if (int_queue.send(i, std::chrono::milliseconds(100))) {
```

**Violation 3**: freertos_task.hpp:120:22
*Style*: misra violation

```cpp
     117:  *
     118:  * ## Usage Examples:
     119:  *
>>>  120:  * ### Basic Task Creation:
     121:  * ```cpp
     122:  * // Create a simple task with dynamic allocation
     123:  * freertos::task<1024> worker_task("WorkerTask", 3, []() {
```

**Violation 4**: freertos_message_buffer.hpp:130:23
*Style*: misra violation

```cpp
     127:    * @param pvTxData pointer to the message data
     128:    * @param xDataLengthBytes length of the message data
     129:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
>>>  130:    * become available
     131:    * @return size_t number of bytes sent
     132:    */
     133:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
```

**Violation 5**: freertos_stream_buffer.hpp:140:24
*Style*: misra violation

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 6**: freertos_sw_timer.hpp:150:25
*Style*: misra violation

```cpp
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
>>>  150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
     152:                       .count()),
     153:               auto_reload, std::move(callback)} {}
```

**Violation 7**: freertos_event_group.hpp:160:26
*Style*: misra violation

```cpp
     157:    * @param bits_to_wait_for bits to wait for
     158:    * @param clear_on_exit clear bits on exit
     159:    * @param wait_for_all_bits wait for all bits
>>>  160:    * @param ticks_to_wait timeout in ticks to wait for the bits
     161:    * @return EventBits_t value of the event group after the call returns
     162:    */
     163:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
```

**Violation 8**: freertos_task.cc:170:27
*Style*: misra violation

```cpp

```

**Violation 9**: freertos_semaphore.hpp:180:28
*Style*: misra violation

```cpp
     177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
>>>  180:    * @brief Destruct the binary semaphore object and
     181:    * delete the binary semaphore instance if it was created.
     182:    *
     183:    */
```

**Violation 10**: freertos_queue.hpp:190:29
*Style*: misra violation

```cpp
     187:  */
     188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
>>>  190:   QueueHandle_t m_queue{nullptr};
     191: 
     192: public:
     193:   /**
```

**Violation 11**: freertos_task.hpp:200:30
*Style*: misra violation

```cpp
     197:         m_start_suspended{start_suspended},
     198:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     199:   /**
>>>  200:    * @brief Construct a new task object
     201:    *
     202:    * @param name name of the task
     203:    * @param priority priority of the task
```

**Violation 12**: freertos_message_buffer.hpp:210:31
*Style*: misra violation

```cpp
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
     209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
>>>  210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
     213:    *
```

**Violation 13**: freertos_stream_buffer.hpp:220:32
*Style*: misra violation

```cpp
     217:   /**
     218:    * @brief Send data to the stream buffer from an ISR.
     219:    * @ref https://www.freertos.org/xStreamBufferSendFromISR.html
>>>  220:    *
     221:    * @param data A pointer to the data to be copied into the stream buffer.
     222:    * @param data_size Maximum number of bytes to copy into the stream buffer.
     223:    * @return BaseType_t pdPASS if the data
```

**Violation 14**: freertos_sw_timer.hpp:230:33
*Style*: misra violation

```cpp
     227: 
     228:   /**
     229:    * @brief Method to start the timer.
>>>  230:    * @ref https://www.freertos.org/xTimerStart.html
     231:    *
     232:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     233:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
```

**Violation 15**: freertos_event_group.hpp:240:34
*Style*: misra violation

```cpp
     237:  * @brief Namespace for aliases of RTOS kernel objects that use a static memory
     238:  *
     239:  */
>>>  240: namespace sa {
     241: /**
     242:  * @brief Alias for the event group that uses a static memory allocation.
     243:  *
```

### Analysis Notes

- **Standard**: MISRA C 2012 (applicable rules for C++)
- **Tool**: cppcheck with MISRA addon
- **Scope**: Library modules only (src/, include/)
- **Rule texts**: Cannot be displayed due to MISRA licensing restrictions
- **Compatibility**: Many MISRA C 2012 rules overlap with MISRA C++ 2008 requirements
- **Integration**: This analysis complements the existing clang-tidy static analysis



## Detailed clang-tidy Analysis

```
[1/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc.
34437 warnings and 1 error generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc.
[2/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos.hpp.
69922 warnings and 2 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos.hpp.
[3/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_event_group.hpp.
76750 warnings and 3 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_event_group.hpp.
[4/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp.
88645 warnings and 4 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp.
[5/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp.
102507 warnings and 5 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp.
[6/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp.
109335 warnings and 6 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp.
[7/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp.
123186 warnings and 7 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp.
[8/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp.
149713 warnings and 8 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp.
[9/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp.
184150 warnings and 9 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp.
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:424:3: warning: overloaded 'operator++' returns a reference instead of a constant object type [cert-dcl21-cpp]
  424 |   counting_semaphore &operator++(int) {
      |   ^~~~~~~~~~~~~~~~~~~~
      |   const counting_semaphore<SemaphoreAllocator> 
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:436:3: warning: overloaded 'operator--' returns a reference instead of a constant object type [cert-dcl21-cpp]
  436 |   counting_semaphore &operator--(int) {
      |   ^~~~~~~~~~~~~~~~~~~~
      |   const counting_semaphore<SemaphoreAllocator> 
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp:129:62: warning: rvalue reference parameter 'callback' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  129 |                  UBaseType_t auto_reload, timer_callback_t &&callback)
      |                                                              ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp:148:62: warning: rvalue reference parameter 'callback' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  148 |                  UBaseType_t auto_reload, timer_callback_t &&callback)
      |                                                              ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:194:65: warning: rvalue reference parameter 'task_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  194 |   task(const char *name, UBaseType_t priority, task_routine_t &&task_routine,
      |                                                                 ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:208:25: warning: rvalue reference parameter 'task_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  208 |        task_routine_t &&task_routine, bool start_suspended = true)
      |                         ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:604:34: warning: rvalue reference parameter 'on_start' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  604 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:604:61: warning: rvalue reference parameter 'on_stop' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  604 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                                             ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:605:34: warning: rvalue reference parameter 'periodic_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  605 |                 task_routine_t &&periodic_routine,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:627:34: warning: rvalue reference parameter 'on_start' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  627 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:627:61: warning: rvalue reference parameter 'on_stop' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  627 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                                             ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:628:34: warning: rvalue reference parameter 'periodic_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  628 |                 task_routine_t &&periodic_routine,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:649:34: warning: rvalue reference parameter 'on_start' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  649 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:649:61: warning: rvalue reference parameter 'on_stop' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  649 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                                             ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:650:34: warning: rvalue reference parameter 'periodic_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  650 |                 task_routine_t &&periodic_routine, bool start_suspended = true)
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:669:34: warning: rvalue reference parameter 'on_start' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  669 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:669:61: warning: rvalue reference parameter 'on_stop' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  669 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                                             ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:670:34: warning: rvalue reference parameter 'periodic_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  670 |                 task_routine_t &&periodic_routine, bool start_suspended = true)
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/tests/mocks/FreeRTOS.h:6:10: error: 'gmock/gmock.h' file not found [clang-diagnostic-error]
    6 | #include <gmock/gmock.h>
      |          ^~~~~~~~~~~~~~~
Suppressed 184114 warnings (184114 in non-user code).
Use -header-filter=.* to display errors from all non-system headers. Use -system-headers to display errors from system headers as well.
Found compiler error(s).
```

---
*Generated: July 23, 2025*
*Tools: clang-tidy + MISRA C++ (cppcheck)*
*Scope: Library modules only*
