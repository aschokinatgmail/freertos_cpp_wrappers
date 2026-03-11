# Validation and Verification Report

## Executive Summary

This report provides comprehensive validation and verification results for the FreeRTOS C++ Wrappers project, including detailed test execution outcomes and code coverage analysis.

### Test Execution Summary
- **Total Tests Executed**: 439
- **✅ Passed**: 439 tests
- **❌ Failed**: 0 tests
- **Success Rate**: 100.0% (All tests passed!)
- **Total Execution Time**: 2.04 seconds
- **Average Test Time**: 0.0046 seconds per test

### Code Coverage Summary
- **Line Coverage**: 97.0% (7395/7620 lines)
- **Function Coverage**: 98.2% (2902/2955 functions)
- **Coverage Scope**: Main library modules only (src/ and include/ directories)

---

# Static Code Analysis

**Static Analysis Tools**: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)
**Analysis Scope**: Library modules only - src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-* + All cppcheck rules (style, performance, portability, security, etc.) + MISRA C 2012 (applicable to C++)

## clang-tidy Analysis

## Statistics

### Summary

- **Total Issues Found**: 0
- **Warnings**: 0
- **Errors**: 0
- **Files Analyzed**: 9
- **Unique Check Types**: 1

### Suppressed Issues

- **Total Suppressed**: 706552
- **Non-user Code**: 705535
- **NOLINT Directives**: 1017

### Issues by Check Category

- **cert**: 2 issues

### Files with Issues

- **freertos_semaphore.hpp**: 2 issues

### Analyzed Files

- freertos.hpp
- freertos_event_group.hpp
- freertos_message_buffer.hpp
- freertos_queue.hpp
- freertos_semaphore.hpp
- freertos_stream_buffer.hpp
- freertos_sw_timer.hpp
- freertos_task.cc
- freertos_task.hpp

## MISRA C++ Analysis

### Summary

- **Total MISRA Violations**: 259
- **Unique Rules Violated**: 17
- **Files Analyzed**: 9
- **Other Issues**: 100

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

- **freertos_event_group.hpp**: 12 violation(s)
- **freertos_message_buffer.hpp**: 21 violation(s)
- **freertos_queue.hpp**: 39 violation(s)
- **freertos_semaphore.hpp**: 81 violation(s)
- **freertos_stream_buffer.hpp**: 35 violation(s)
- **freertos_sw_timer.hpp**: 41 violation(s)
- **freertos_task.cc**: 4 violation(s)
- **freertos_task.hpp**: 26 violation(s)

### Detailed Violations with Code Context

#### MISRA C 2012 Rule 10.4 (4 violation(s))

**Both operands of an operator in which the usual arithmetic conversions are performed shall have the same essential type**

Arithmetic operations should be performed on operands of compatible types to avoid unexpected type conversions and potential loss of precision.

*Rationale: Mixed-type arithmetic can lead to unexpected results due to implicit type conversions.*

**Violation 1**: freertos_semaphore.hpp:656:34
*Reason: Coding Style*

```cpp
     653:    */
     654:   BaseType_t unlock() {
     655:     auto rc = xSemaphoreGiveRecursive(m_semaphore);
>>>  656:     if (rc && m_recursions_count > 0) {
     657:       m_recursions_count--;
     658:     }
     659:     return rc;
```

**Violation 2**: freertos_semaphore.hpp:671:34
*Reason: Coding Style*

```cpp
     668:    */
     669:   BaseType_t unlock_isr(BaseType_t &high_priority_task_woken) {
     670:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
>>>  671:     if (rc && m_recursions_count > 0) {
     672:       m_recursions_count--;
     673:     }
     674:     return rc;
```

**Violation 3**: freertos_semaphore.hpp:685:34
*Reason: Coding Style*

```cpp
     682:   BaseType_t unlock_isr(void) {
     683:     BaseType_t high_priority_task_woken = pdFALSE;
     684:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
>>>  685:     if (rc && m_recursions_count > 0) {
     686:       m_recursions_count--;
     687:     }
     688:     return rc;
```

**Violation 4**: freertos_semaphore.hpp:762:55
*Reason: Coding Style*

```cpp
     759:    *
     760:    * @return bool true if the recursive mutex is locked, otherwise false.
     761:    */
>>>  762:   bool locked(void) const { return m_recursions_count > 0; }
     763:   /**
     764:    * @brief Get the number of recursions of the recursive mutex.
     765:    *
```

#### MISRA C 2012 Rule 12.3 (36 violation(s))

**The comma operator should not be used**

The comma operator can make code difficult to understand and maintain. Its use should be avoided except in specific contexts like for-loop expressions.

*Rationale: The comma operator can obscure control flow and make code harder to read and debug.*

**Violation 1**: freertos_queue.hpp:243:25
*Reason: Coding Style*

```cpp
     240:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     241:    * errQUEUE_FULL.
     242:    */
>>>  243:   template <typename Rep, typename Period>
     244:   BaseType_t send(const T &item,
     245:                   const std::chrono::duration<Rep, Period> &timeout) {
     246:     return send(
```

**Violation 2**: freertos_queue.hpp:298:25
*Reason: Coding Style*

```cpp
     295:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     296:    * errQUEUE_FULL.
     297:    */
>>>  298:   template <typename Rep, typename Period>
     299:   BaseType_t send_back(const T &item,
     300:                        const std::chrono::duration<Rep, Period> &timeout) {
     301:     return send_back(
```

**Violation 3**: freertos_queue.hpp:354:25
*Reason: Coding Style*

```cpp
     351:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     352:    * errQUEUE_FULL.
     353:    */
>>>  354:   template <typename Rep, typename Period>
     355:   BaseType_t send_front(const T &item,
     356:                         const std::chrono::duration<Rep, Period> &timeout) {
     357:     return send_front(
```

**Violation 4**: freertos_queue.hpp:427:25
*Reason: Coding Style*

```cpp
     424:    * @return BaseType_t pdPASS if the item
     425:    * was successfully received, otherwise pdFALSE.
     426:    */
>>>  427:   template <typename Rep, typename Period>
     428:   BaseType_t receive(T &item,
     429:                      const std::chrono::duration<Rep, Period> &timeout) {
     430:     return receive(
```

**Violation 5**: freertos_queue.hpp:443:25
*Reason: Coding Style*

```cpp
     440:    * @return optional<T> The item received from the queue or std::nullopt if the
     441:    * queue is empty.
     442:    */
>>>  443:   template <typename Rep, typename Period>
     444:   optional<T> receive(const std::chrono::duration<Rep, Period> &timeout) {
     445:     return receive(pdMS_TO_TICKS(
     446:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
```

**Violation 6**: freertos_queue.hpp:580:25
*Reason: Coding Style*

```cpp
     577:    * @return BaseType_t pdPASS if the item was successfully peeked, otherwise
     578:    * pdFALSE.
     579:    */
>>>  580:   template <typename Rep, typename Period>
     581:   BaseType_t peek(T &item, const std::chrono::duration<Rep, Period> &timeout) {
     582:     return peek(
     583:         item, pdMS_TO_TICKS(
```

**Violation 7**: freertos_queue.hpp:638:25
*Reason: Coding Style*

```cpp
     635:    * @return optional<T> The item peeked from the queue or std::nullopt if the
     636:    * queue is empty.
     637:    */
>>>  638:   template <typename Rep, typename Period>
     639:   optional<T> peek(const std::chrono::duration<Rep, Period> &timeout) {
     640:     return peek(pdMS_TO_TICKS(
     641:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
```

**Violation 8**: freertos_queue.hpp:188:29
*Reason: Coding Style*

```cpp
     185:  * static_queue;
     186:  * ```
     187:  */
>>>  188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
     190:   QueueHandle_t m_queue{nullptr};
     191: 
```

**Violation 9**: freertos_queue.hpp:56:21
*Reason: Coding Style*

```cpp
      53:  */
      54: template <size_t QueueLength, typename T> class static_queue_allocator {
      55:   StaticQueue_t m_queue_placeholder{};
>>>   56:   std::array<uint8_t, QueueLength * sizeof(T)> m_storage;
      57: 
      58: public:
      59:   static_queue_allocator() = default;
```

**Violation 10**: freertos_semaphore.hpp:271:25
*Reason: Coding Style*

```cpp
     268:    * otherwise pdFALSE.
     269:    *
     270:    */
>>>  271:   template <typename Rep, typename Period>
     272:   BaseType_t take(const std::chrono::duration<Rep, Period> &timeout) {
     273:     return take(
     274:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 11**: freertos_semaphore.hpp:393:25
*Reason: Coding Style*

```cpp
     390:    * otherwise pdFALSE.
     391:    *
     392:    */
>>>  393:   template <typename Rep, typename Period>
     394:   BaseType_t take(const std::chrono::duration<Rep, Period> &timeout) {
     395:     return take(
     396:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 12**: freertos_semaphore.hpp:586:25
*Reason: Coding Style*

```cpp
     583:    * @param timeout timeout to wait for the mutex.
     584:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     585:    */
>>>  586:   template <typename Rep, typename Period>
     587:   BaseType_t lock(const std::chrono::duration<Rep, Period> &timeout) {
     588:     return lock(
     589:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 13**: freertos_semaphore.hpp:739:25
*Reason: Coding Style*

```cpp
     736:    * @param timeout timeout to wait for the recursive mutex.
     737:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     738:    */
>>>  739:   template <typename Rep, typename Period>
     740:   BaseType_t lock(const std::chrono::duration<Rep, Period> &timeout) {
     741:     return lock(
     742:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 14**: freertos_semaphore.hpp:827:23
*Reason: Coding Style*

```cpp
     824:    * @param mutex mutex to guard
     825:    */
     826:   explicit try_lock_guard(Mutex &mutex)
>>>  827:       : m_mutex{mutex}, m_lock_acquired{static_cast<bool>(m_mutex.try_lock())} {
     828:   }
     829:   /**
     830:    * @brief Destruct the try lock guard object and unlock the mutex.
```

**Violation 15**: freertos_semaphore.hpp:921:23
*Reason: Coding Style*

```cpp
     918:    * @param ticks_to_wait timeout in ticks to wait for the mutex.
     919:    */
     920:   timeout_lock_guard(Mutex &mutex, TickType_t ticks_to_wait)
>>>  921:       : m_mutex{mutex},
     922:         m_lock_acquired{static_cast<bool>(m_mutex.lock(ticks_to_wait))} {}
     923:   /**
     924:    * @brief Construct a new timeout lock guard object
```

**Violation 16**: freertos_semaphore.hpp:929:25
*Reason: Coding Style*

```cpp
     926:    * @param mutex mutex to guard
     927:    * @param timeout timeout to wait for the mutex.
     928:    */
>>>  929:   template <typename Rep, typename Period>
     930:   timeout_lock_guard(Mutex &mutex,
     931:                      const std::chrono::duration<Rep, Period> &timeout)
     932:       : m_mutex{mutex},
```

**Violation 17**: freertos_semaphore.hpp:932:23
*Reason: Coding Style*

```cpp
     929:   template <typename Rep, typename Period>
     930:   timeout_lock_guard(Mutex &mutex,
     931:                      const std::chrono::duration<Rep, Period> &timeout)
>>>  932:       : m_mutex{mutex},
     933:         m_lock_acquired{static_cast<bool>(m_mutex.lock(
     934:             std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
     935:                 .count()))} {}
```

**Violation 18**: freertos_sw_timer.hpp:132:58
*Reason: Coding Style*

```cpp
     129:    */
     130:   explicit timer(const char *name, const TickType_t period_ticks,
     131:                  UBaseType_t auto_reload, timer_callback_t &&callback)
>>>  132:       : m_callback{std::move(callback)}, m_started{false},
     133:         m_timer{m_allocator.create(name, period_ticks, auto_reload, this,
     134:                                    callback_wrapper)} {
     135:     configASSERT(m_timer);
```

**Violation 19**: freertos_sw_timer.hpp:147:25
*Reason: Coding Style*

```cpp
     144:    * @param auto_reload auto-reload flag
     145:    * @param callback callback routine
     146:    */
>>>  147:   template <typename Rep, typename Period>
     148:   explicit timer(const char *name,
     149:                  const std::chrono::duration<Rep, Period> &period,
     150:                  UBaseType_t auto_reload, timer_callback_t &&callback)
```

**Violation 20**: freertos_sw_timer.hpp:256:25
*Reason: Coding Style*

```cpp
     253:    * @param timeout timeout to wait for the timer to start
     254:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
     255:    */
>>>  256:   template <typename Rep, typename Period>
     257:   BaseType_t start(const std::chrono::duration<Rep, Period> &timeout) {
     258:     return start(
     259:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 21**: freertos_sw_timer.hpp:315:25
*Reason: Coding Style*

```cpp
     312:    * @param timeout timeout to wait for the timer to stop
     313:    * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
     314:    */
>>>  315:   template <typename Rep, typename Period>
     316:   BaseType_t stop(const std::chrono::duration<Rep, Period> &timeout) {
     317:     return stop(
     318:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 22**: freertos_sw_timer.hpp:370:25
*Reason: Coding Style*

```cpp
     367:    * @param timeout timeout to wait for the timer to reset
     368:    * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
     369:    */
>>>  370:   template <typename Rep, typename Period>
     371:   BaseType_t reset(const std::chrono::duration<Rep, Period> &timeout) {
     372:     return reset(
     373:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 23**: freertos_sw_timer.hpp:429:31
*Reason: Coding Style*

```cpp
     426:    * @return BaseType_t pdPASS if the timer period was changed successfully else
     427:    * pdFAIL
     428:    */
>>>  429:   template <typename RepPeriod, typename PeriodPeriod, typename RepTimeout,
     430:             typename PeriodTimeout>
     431:   BaseType_t
     432:   period(const std::chrono::duration<RepPeriod, PeriodPeriod> &new_period,
```

**Violation 24**: freertos_sw_timer.hpp:469:25
*Reason: Coding Style*

```cpp
     466:    * @return BaseType_t pdPASS if the timer period was changed successfully else
     467:    * pdFAIL
     468:    */
>>>  469:   template <typename Rep, typename Period>
     470:   BaseType_t period_isr(const std::chrono::duration<Rep, Period> &new_period,
     471:                         BaseType_t &high_priority_task_woken) {
     472:     return period_isr(
```

**Violation 25**: freertos_sw_timer.hpp:499:25
*Reason: Coding Style*

```cpp
     496:    * @return BaseType_t pdPASS if the timer period was changed successfully else
     497:    * pdFAIL
     498:    */
>>>  499:   template <typename Rep, typename Period>
     500:   BaseType_t period_isr(const std::chrono::duration<Rep, Period> &new_period) {
     501:     return period_isr(
     502:         std::chrono::duration_cast<std::chrono::milliseconds>(new_period)
```

**Violation 26**: freertos_stream_buffer.hpp:156:25
*Reason: Coding Style*

```cpp
     153:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     154:    * insufficient memory available to copy the data into the stream buffer.
     155:    */
>>>  156:   template <typename Rep, typename Period>
     157:   BaseType_t send(const void *data, size_t data_size,
     158:                   const std::chrono::duration<Rep, Period> &timeout) {
     159:     return send(
```

**Violation 27**: freertos_stream_buffer.hpp:194:30
*Reason: Coding Style*

```cpp
     191:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     192:    * insufficient memory available to copy the data into the stream buffer.
     193:    */
>>>  194:   template <typename Iterator, typename Rep, typename Period>
     195:   BaseType_t send(Iterator begin, Iterator end,
     196:                   const std::chrono::duration<Rep, Period> &timeout) {
     197:     return send(&*begin, std::distance(begin, end), timeout);
```

**Violation 28**: freertos_stream_buffer.hpp:294:25
*Reason: Coding Style*

```cpp
     291:    * @param timeout Duration to wait for the data to be copied into the buffer.
     292:    * @return size_t Number of bytes received.
     293:    */
>>>  294:   template <typename Rep, typename Period>
     295:   size_t receive(void *data, size_t data_size,
     296:                  const std::chrono::duration<Rep, Period> &timeout) {
     297:     return receive(
```

**Violation 29**: freertos_stream_buffer.hpp:96:34
*Reason: Coding Style*

```cpp
      93:  * @tparam StreamBufferSize size of the stream buffer
      94:  * @tparam StreamBufferAllocator type of the stream buffer allocator
      95:  */
>>>   96: template <size_t StreamBufferSize, typename StreamBufferAllocator>
      97: class stream_buffer {
      98:   StreamBufferAllocator m_allocator{};
      99:   StreamBufferHandle_t m_stream_buffer{nullptr};
```

**Violation 30**: freertos_stream_buffer.hpp:55:21
*Reason: Coding Style*

```cpp
      52:  */
      53: template <size_t StreamBufferSize> class static_stream_buffer_allocator {
      54:   StaticStreamBuffer_t m_stream_buffer_placeholder{};
>>>   55:   std::array<uint8_t, StreamBufferSize> m_storage;
      56: 
      57: public:
      58:   static_stream_buffer_allocator() = default;
```

**Violation 31**: freertos_event_group.hpp:181:25
*Reason: Coding Style*

```cpp
     178:    * @param timeout timeout to wait for the bits
     179:    * @return EventBits_t value of the event group after the call returns
     180:    */
>>>  181:   template <typename Rep, typename Period>
     182:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
     183:                         const BaseType_t clear_on_exit,
     184:                         const BaseType_t wait_for_all_bits,
```

**Violation 32**: freertos_event_group.hpp:223:25
*Reason: Coding Style*

```cpp
     220:     return xEventGroupSync(m_event_group, bits_to_set, bits_to_wait_for,
     221:                            ticks_to_wait);
     222:   }
>>>  223:   template <typename Rep, typename Period>
     224:   EventBits_t sync(const EventBits_t bits_to_set,
     225:                    const EventBits_t bits_to_wait_for,
     226:                    const std::chrono::duration<Rep, Period> &timeout) {
```

**Violation 33**: freertos_message_buffer.hpp:147:25
*Reason: Coding Style*

```cpp
     144:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
     145:    * @return size_t number of bytes sent
     146:    */
>>>  147:   template <typename Rep, typename Period>
     148:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
     149:               const std::chrono::duration<Rep, Period> &xTicksToWait) {
     150:     return send(
```

**Violation 34**: freertos_message_buffer.hpp:180:25
*Reason: Coding Style*

```cpp
     177:    * become available
     178:    * @return size_t number of bytes received
     179:    */
>>>  180:   template <typename Rep, typename Period>
     181:   size_t receive(void *pvRxData, size_t xBufferLengthBytes,
     182:                  const std::chrono::duration<Rep, Period> &timeout) {
     183:     return receive(
```

**Violation 35**: freertos_message_buffer.hpp:94:35
*Reason: Coding Style*

```cpp
      91:  * @tparam MessageBufferSize size of the message buffer
      92:  * @tparam MessageBufferAllocator type of the message buffer allocator
      93:  */
>>>   94: template <size_t MessageBufferSize, typename MessageBufferAllocator>
      95: class message_buffer {
      96:   MessageBufferAllocator m_allocator{};
      97:   MessageBufferHandle_t m_message_buffer{nullptr};
```

**Violation 36**: freertos_message_buffer.hpp:54:21
*Reason: Coding Style*

```cpp
      51:  */
      52: template <size_t MessageBufferSize> class static_message_buffer_allocator {
      53:   StaticMessageBuffer_t m_message_buffer_placeholder{};
>>>   54:   std::array<uint8_t, MessageBufferSize> m_storage;
      55: 
      56: public:
      57:   static_message_buffer_allocator() = default;
```

#### MISRA C 2012 Rule 13.4 (1 violation(s))

**The result of an assignment operator should not be used**

Assignment expressions should not be used as sub-expressions. Assignments should be separate statements to improve readability and avoid confusion.

*Rationale: Using assignment results as expressions can make code harder to read and may hide logical errors.*

**Violation 1**: freertos_queue.hpp:199:45
*Reason: Coding Style*

```cpp
     196:    * @param registred_name Name of the queue, which will be used to register it
     197:    * in the queue registry. If nullptr, the queue will not be registered.
     198:    */
>>>  199:   explicit queue(const char *registred_name = nullptr)
     200:       : m_queue{m_allocator.create()} {
     201:     configASSERT(m_queue);
     202:     if (registred_name != nullptr) {
```

#### MISRA C 2012 Rule 15.5 (17 violation(s))

**A function should have a single point of exit at the end**

Functions should have one return statement at the end rather than multiple return points throughout the function body. This improves code clarity and makes debugging easier.

*Rationale: Single exit points make functions easier to understand, debug, and maintain.*

**Violation 1**: freertos_queue.hpp:414:7
*Reason: Coding Style*

```cpp
     411:   optional<T> receive(TickType_t ticks_to_wait) {
     412:     T item;
     413:     if (xQueueReceive(m_queue, &item, ticks_to_wait) == pdPASS) {
>>>  414:       return item;
     415:     }
     416:     return {};
     417:   }
```

**Violation 2**: freertos_queue.hpp:487:7
*Reason: Coding Style*

```cpp
     484:     BaseType_t higher_priority_task_woken = pdFALSE;
     485:     if (xQueueReceiveFromISR(m_queue, &item, &higher_priority_task_woken) ==
     486:         pdPASS) {
>>>  487:       return item;
     488:     }
     489:     return {};
     490:   }
```

**Violation 3**: freertos_queue.hpp:626:7
*Reason: Coding Style*

```cpp
     623:   optional<T> peek(TickType_t ticks_to_wait) {
     624:     T item;
     625:     if (xQueuePeek(m_queue, &item, ticks_to_wait) == pdPASS) {
>>>  626:       return item;
     627:     }
     628:     return {};
     629:   }
```

**Violation 4**: freertos_queue.hpp:658:7
*Reason: Coding Style*

```cpp
     655:     BaseType_t higher_priority_task_woken = pdFALSE;
     656:     if (xQueuePeekFromISR(m_queue, &item, &higher_priority_task_woken) ==
     657:         pdPASS) {
>>>  658:       return item;
     659:     }
     660:     return {};
     661:   }
```

**Violation 5**: freertos_sw_timer.hpp:239:7
*Reason: Coding Style*

```cpp
     236:    */
     237:   BaseType_t start(const TickType_t ticks_to_wait = portMAX_DELAY) {
     238:     if (!m_timer) {
>>>  239:       return pdFAIL;
     240:     }
     241:     auto rc = xTimerStart(m_timer, ticks_to_wait);
     242:     if (rc) {
```

**Violation 6**: freertos_sw_timer.hpp:271:7
*Reason: Coding Style*

```cpp
     268:    */
     269:   BaseType_t start_isr(BaseType_t &high_priority_task_woken) {
     270:     if (!m_timer) {
>>>  271:       return pdFAIL;
     272:     }
     273:     auto rc = xTimerStartFromISR(m_timer, &high_priority_task_woken);
     274:     if (rc) {
```

**Violation 7**: freertos_sw_timer.hpp:298:7
*Reason: Coding Style*

```cpp
     295:    */
     296:   BaseType_t stop(const TickType_t ticks_to_wait = portMAX_DELAY) {
     297:     if (!m_timer) {
>>>  298:       return pdFAIL;
     299:     }
     300:     auto rc = xTimerStop(m_timer, ticks_to_wait);
     301:     if (rc) {
```

**Violation 8**: freertos_sw_timer.hpp:330:7
*Reason: Coding Style*

```cpp
     327:    */
     328:   BaseType_t stop_isr(BaseType_t &high_priority_task_woken) {
     329:     if (!m_timer) {
>>>  330:       return pdFAIL;
     331:     }
     332:     auto rc = xTimerStopFromISR(m_timer, &high_priority_task_woken);
     333:     if (rc) {
```

**Violation 9**: freertos_sw_timer.hpp:357:7
*Reason: Coding Style*

```cpp
     354:    */
     355:   BaseType_t reset(const TickType_t ticks_to_wait = portMAX_DELAY) {
     356:     if (!m_timer) {
>>>  357:       return pdFAIL;
     358:     }
     359:     return xTimerReset(m_timer, ticks_to_wait);
     360:   }
```

**Violation 10**: freertos_sw_timer.hpp:385:7
*Reason: Coding Style*

```cpp
     382:    */
     383:   BaseType_t reset_isr(BaseType_t &high_priority_task_woken) {
     384:     if (!m_timer) {
>>>  385:       return pdFAIL;
     386:     }
     387:     return xTimerResetFromISR(m_timer, &high_priority_task_woken);
     388:   }
```

**Violation 11**: freertos_sw_timer.hpp:412:7
*Reason: Coding Style*

```cpp
     409:   BaseType_t period(const TickType_t new_period_ticks,
     410:                     const TickType_t ticks_to_wait = portMAX_DELAY) {
     411:     if (!m_timer) {
>>>  412:       return pdFAIL;
     413:     }
     414:     return xTimerChangePeriod(m_timer, new_period_ticks, ticks_to_wait);
     415:   }
```

**Violation 12**: freertos_sw_timer.hpp:452:7
*Reason: Coding Style*

```cpp
     449:   BaseType_t period_isr(const TickType_t new_period_ticks,
     450:                         BaseType_t &high_priority_task_woken) {
     451:     if (!m_timer) {
>>>  452:       return pdFAIL;
     453:     }
     454:     return xTimerChangePeriodFromISR(m_timer, new_period_ticks,
     455:                                      &high_priority_task_woken);
```

**Violation 13**: freertos_sw_timer.hpp:518:7
*Reason: Coding Style*

```cpp
     515:    */
     516:   TickType_t period_ticks(void) const {
     517:     if (!m_timer) {
>>>  518:       return 0;
     519:     }
     520:     return xTimerGetPeriod(m_timer);
     521:   }
```

**Violation 14**: freertos_sw_timer.hpp:558:7
*Reason: Coding Style*

```cpp
     555:    */
     556:   TickType_t remaining_ticks(void) const {
     557:     if (m_timer) {
>>>  558:       return xTimerGetExpiryTime(m_timer) - xTaskGetTickCount();
     559:     } else {
     560:       return 0;
     561:     }
```

**Violation 15**: freertos_sw_timer.hpp:560:7
*Reason: Coding Style*

```cpp
     557:     if (m_timer) {
     558:       return xTimerGetExpiryTime(m_timer) - xTaskGetTickCount();
     559:     } else {
>>>  560:       return 0;
     561:     }
     562:   }
     563:   /**
```

**Violation 16**: freertos_sw_timer.hpp:580:7
*Reason: Coding Style*

```cpp
     577:    */
     578:   BaseType_t running(void) const {
     579:     if (!m_timer) {
>>>  580:       return pdFALSE;
     581:     }
     582:     return xTimerIsTimerActive(m_timer);
     583:   }
```

**Violation 17**: freertos_sw_timer.hpp:591:7
*Reason: Coding Style*

```cpp
     588:    */
     589:   const char *name(void) const {
     590:     if (!m_timer) {
>>>  591:       return nullptr;
     592:     }
     593:     return pcTimerGetName(m_timer);
     594:   }
```

#### MISRA C 2012 Rule 16.3 (13 violation(s))

**An unconditional break statement shall terminate every switch-clause**

Every case in a switch statement should end with a break statement to prevent fall-through behavior, unless fall-through is explicitly intended and documented.

*Rationale: Missing break statements can cause unintended fall-through behavior that leads to bugs.*

**Violation 1**: freertos_queue.hpp:59:30
*Reason: Coding Style*

```cpp
      56:   std::array<uint8_t, QueueLength * sizeof(T)> m_storage;
      57: 
      58: public:
>>>   59:   static_queue_allocator() = default;
      60:   ~static_queue_allocator() = default;
      61:   static_queue_allocator(const static_queue_allocator &) = delete;
      62:   static_queue_allocator(static_queue_allocator &&) = delete;
```

**Violation 2**: freertos_queue.hpp:60:31
*Reason: Coding Style*

```cpp
      57: 
      58: public:
      59:   static_queue_allocator() = default;
>>>   60:   ~static_queue_allocator() = default;
      61:   static_queue_allocator(const static_queue_allocator &) = delete;
      62:   static_queue_allocator(static_queue_allocator &&) = delete;
      63: 
```

**Violation 3**: freertos_semaphore.hpp:56:34
*Reason: Coding Style*

```cpp
      53:   StaticSemaphore_t m_semaphore_placeholder{};
      54: 
      55: public:
>>>   56:   static_semaphore_allocator() = default;
      57:   ~static_semaphore_allocator() = default;
      58:   static_semaphore_allocator(const static_semaphore_allocator &) = delete;
      59:   static_semaphore_allocator(static_semaphore_allocator &&) = delete;
```

**Violation 4**: freertos_semaphore.hpp:57:35
*Reason: Coding Style*

```cpp
      54: 
      55: public:
      56:   static_semaphore_allocator() = default;
>>>   57:   ~static_semaphore_allocator() = default;
      58:   static_semaphore_allocator(const static_semaphore_allocator &) = delete;
      59:   static_semaphore_allocator(static_semaphore_allocator &&) = delete;
      60: 
```

**Violation 5**: freertos_sw_timer.hpp:63:33
*Reason: Coding Style*

```cpp
      60:   StaticTimer_t m_timer_placeholder{};
      61: 
      62: public:
>>>   63:   static_sw_timer_allocator() = default;
      64:   ~static_sw_timer_allocator() = default;
      65:   static_sw_timer_allocator(const static_sw_timer_allocator &) = delete;
      66:   static_sw_timer_allocator(static_sw_timer_allocator &&) = default;
```

**Violation 6**: freertos_sw_timer.hpp:64:34
*Reason: Coding Style*

```cpp
      61: 
      62: public:
      63:   static_sw_timer_allocator() = default;
>>>   64:   ~static_sw_timer_allocator() = default;
      65:   static_sw_timer_allocator(const static_sw_timer_allocator &) = delete;
      66:   static_sw_timer_allocator(static_sw_timer_allocator &&) = default;
      67: 
```

**Violation 7**: freertos_sw_timer.hpp:66:61
*Reason: Coding Style*

```cpp
      63:   static_sw_timer_allocator() = default;
      64:   ~static_sw_timer_allocator() = default;
      65:   static_sw_timer_allocator(const static_sw_timer_allocator &) = delete;
>>>   66:   static_sw_timer_allocator(static_sw_timer_allocator &&) = default;
      67: 
      68:   static_sw_timer_allocator &
      69:   operator=(const static_sw_timer_allocator &) = delete;
```

**Violation 8**: freertos_stream_buffer.hpp:58:38
*Reason: Coding Style*

```cpp
      55:   std::array<uint8_t, StreamBufferSize> m_storage;
      56: 
      57: public:
>>>   58:   static_stream_buffer_allocator() = default;
      59:   ~static_stream_buffer_allocator() = default;
      60:   static_stream_buffer_allocator(const static_stream_buffer_allocator &) =
      61:       delete;
```

**Violation 9**: freertos_stream_buffer.hpp:59:39
*Reason: Coding Style*

```cpp
      56: 
      57: public:
      58:   static_stream_buffer_allocator() = default;
>>>   59:   ~static_stream_buffer_allocator() = default;
      60:   static_stream_buffer_allocator(const static_stream_buffer_allocator &) =
      61:       delete;
      62:   static_stream_buffer_allocator(static_stream_buffer_allocator &&) = delete;
```

**Violation 10**: freertos_event_group.hpp:54:36
*Reason: Coding Style*

```cpp
      51:   StaticEventGroup_t m_event_group_placeholder{};
      52: 
      53: public:
>>>   54:   static_event_group_allocator() = default;
      55:   ~static_event_group_allocator() = default;
      56:   static_event_group_allocator(const static_event_group_allocator &) = delete;
      57:   static_event_group_allocator(static_event_group_allocator &&) = delete;
```

**Violation 11**: freertos_event_group.hpp:55:37
*Reason: Coding Style*

```cpp
      52: 
      53: public:
      54:   static_event_group_allocator() = default;
>>>   55:   ~static_event_group_allocator() = default;
      56:   static_event_group_allocator(const static_event_group_allocator &) = delete;
      57:   static_event_group_allocator(static_event_group_allocator &&) = delete;
      58: 
```

**Violation 12**: freertos_message_buffer.hpp:57:39
*Reason: Coding Style*

```cpp
      54:   std::array<uint8_t, MessageBufferSize> m_storage;
      55: 
      56: public:
>>>   57:   static_message_buffer_allocator() = default;
      58:   ~static_message_buffer_allocator() = default;
      59:   static_message_buffer_allocator(const static_message_buffer_allocator &) =
      60:       delete;
```

**Violation 13**: freertos_message_buffer.hpp:58:40
*Reason: Coding Style*

```cpp
      55: 
      56: public:
      57:   static_message_buffer_allocator() = default;
>>>   58:   ~static_message_buffer_allocator() = default;
      59:   static_message_buffer_allocator(const static_message_buffer_allocator &) =
      60:       delete;
      61:   static_message_buffer_allocator(static_message_buffer_allocator &&) = delete;
```

#### MISRA C 2012 Rule 17.3 (7 violation(s))

**A function shall not be declared implicitly**

All functions should be explicitly declared before use. Implicit function declarations can lead to type mismatches and undefined behavior.

*Rationale: Implicit function declarations can cause type mismatches and make code harder to understand and maintain.*

**Violation 1**: freertos_queue.hpp:413:9
*Reason: Coding Style*

```cpp
     410:    */
     411:   optional<T> receive(TickType_t ticks_to_wait) {
     412:     T item;
>>>  413:     if (xQueueReceive(m_queue, &item, ticks_to_wait) == pdPASS) {
     414:       return item;
     415:     }
     416:     return {};
```

**Violation 2**: freertos_queue.hpp:485:9
*Reason: Coding Style*

```cpp
     482:   optional<T> receive_isr(void) {
     483:     T item;
     484:     BaseType_t higher_priority_task_woken = pdFALSE;
>>>  485:     if (xQueueReceiveFromISR(m_queue, &item, &higher_priority_task_woken) ==
     486:         pdPASS) {
     487:       return item;
     488:     }
```

**Violation 3**: freertos_queue.hpp:625:9
*Reason: Coding Style*

```cpp
     622:    */
     623:   optional<T> peek(TickType_t ticks_to_wait) {
     624:     T item;
>>>  625:     if (xQueuePeek(m_queue, &item, ticks_to_wait) == pdPASS) {
     626:       return item;
     627:     }
     628:     return {};
```

**Violation 4**: freertos_queue.hpp:656:9
*Reason: Coding Style*

```cpp
     653:   optional<T> peek_isr(void) {
     654:     T item;
     655:     BaseType_t higher_priority_task_woken = pdFALSE;
>>>  656:     if (xQueuePeekFromISR(m_queue, &item, &higher_priority_task_woken) ==
     657:         pdPASS) {
     658:       return item;
     659:     }
```

**Violation 5**: freertos_sw_timer.hpp:186:16
*Reason: Coding Style*

```cpp
     183:     if (m_timer) {
     184:       auto rc = xTimerDelete(m_timer, portMAX_DELAY);
     185:       if (rc == pdPASS) {
>>>  186:         while (xTimerIsTimerActive(m_timer) != pdFALSE) {
     187:           vTaskDelay(pdMS_TO_TICKS(1));
     188:         }
     189:       }
```

**Violation 6**: freertos_sw_timer.hpp:201:16
*Reason: Coding Style*

```cpp
     198:       }
     199:       auto rc = xTimerStop(src.m_timer, portMAX_DELAY);
     200:       if (rc == pdPASS) {
>>>  201:         while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
     202:           vTaskDelay(pdMS_TO_TICKS(1));
     203:         }
     204:         auto name = pcTimerGetName(src.m_timer);
```

**Violation 7**: freertos_sw_timer.hpp:209:18
*Reason: Coding Style*

```cpp
     206:         auto auto_reload = uxTimerGetReloadMode(src.m_timer);
     207:         rc = xTimerDelete(src.m_timer, portMAX_DELAY);
     208:         if (rc == pdPASS) {
>>>  209:           while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
     210:             vTaskDelay(pdMS_TO_TICKS(1));
     211:           }
     212:           src.m_timer = nullptr;
```

#### MISRA C 2012 Rule 17.8 (14 violation(s))

**A function parameter should not be modified**

Function parameters should not be modified within the function body. If modification is needed, use a local copy of the parameter.

*Rationale: Modifying parameters can make function behavior less predictable and harder to understand.*

**Violation 1**: freertos_semaphore.hpp:234:50
*Reason: Coding Style*

```cpp
     231:    * otherwise pdFALSE.
     232:    *
     233:    */
>>>  234:   BaseType_t take(const TickType_t ticks_to_wait = portMAX_DELAY) {
     235:     return xSemaphoreTake(m_semaphore, ticks_to_wait);
     236:   }
     237:   /**
```

**Violation 2**: freertos_semaphore.hpp:356:50
*Reason: Coding Style*

```cpp
     353:    * otherwise pdFALSE.
     354:    *
     355:    */
>>>  356:   BaseType_t take(const TickType_t ticks_to_wait = portMAX_DELAY) {
     357:     return xSemaphoreTake(m_semaphore, ticks_to_wait);
     358:   }
     359:   /**
```

**Violation 3**: freertos_semaphore.hpp:453:17
*Reason: Coding Style*

```cpp
     450:    * @return counting_semaphore& reference to the counting semaphore.
     451:    */
     452:   counting_semaphore &operator+=(UBaseType_t count) {
>>>  453:     while (count--) {
     454:       give();
     455:     }
     456:     return *this;
```

**Violation 4**: freertos_semaphore.hpp:544:50
*Reason: Coding Style*

```cpp
     541:    * @param ticks_to_wait timeout in ticks to wait for the mutex.
     542:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     543:    */
>>>  544:   BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
     545:     auto rc = xSemaphoreTake(m_semaphore, ticks_to_wait);
     546:     if (rc) {
     547:       m_locked = true;
```

**Violation 5**: freertos_semaphore.hpp:697:50
*Reason: Coding Style*

```cpp
     694:    * @param ticks_to_wait timeout in ticks to wait for the recursive mutex.
     695:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     696:    */
>>>  697:   BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
     698:     auto rc = xSemaphoreTakeRecursive(m_semaphore, ticks_to_wait);
     699:     if (rc) {
     700:       m_recursions_count++;
```

**Violation 6**: freertos_sw_timer.hpp:237:51
*Reason: Coding Style*

```cpp
     234:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     235:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
     236:    */
>>>  237:   BaseType_t start(const TickType_t ticks_to_wait = portMAX_DELAY) {
     238:     if (!m_timer) {
     239:       return pdFAIL;
     240:     }
```

**Violation 7**: freertos_sw_timer.hpp:296:50
*Reason: Coding Style*

```cpp
     293:    * @param ticks_to_wait timeout in ticks to wait for the timer to stop
     294:    * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
     295:    */
>>>  296:   BaseType_t stop(const TickType_t ticks_to_wait = portMAX_DELAY) {
     297:     if (!m_timer) {
     298:       return pdFAIL;
     299:     }
```

**Violation 8**: freertos_sw_timer.hpp:355:51
*Reason: Coding Style*

```cpp
     352:    * @param ticks_to_wait timeout in ticks to wait for the timer to reset
     353:    * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
     354:    */
>>>  355:   BaseType_t reset(const TickType_t ticks_to_wait = portMAX_DELAY) {
     356:     if (!m_timer) {
     357:       return pdFAIL;
     358:     }
```

**Violation 9**: freertos_sw_timer.hpp:410:52
*Reason: Coding Style*

```cpp
     407:    * pdFAIL
     408:    */
     409:   BaseType_t period(const TickType_t new_period_ticks,
>>>  410:                     const TickType_t ticks_to_wait = portMAX_DELAY) {
     411:     if (!m_timer) {
     412:       return pdFAIL;
     413:     }
```

**Violation 10**: freertos_stream_buffer.hpp:141:38
*Reason: Coding Style*

```cpp
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
     140:   BaseType_t send(const void *data, size_t data_size,
>>>  141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
     144:   /**
```

**Violation 11**: freertos_stream_buffer.hpp:178:38
*Reason: Coding Style*

```cpp
     175:    */
     176:   template <typename Iterator>
     177:   BaseType_t send(Iterator begin, Iterator end,
>>>  178:                   TickType_t timeout = portMAX_DELAY) {
     179:     return send(&*begin, std::distance(begin, end), timeout);
     180:   }
     181:   /**
```

**Violation 12**: freertos_stream_buffer.hpp:281:37
*Reason: Coding Style*

```cpp
     278:    * @return size_t Number of bytes received.
     279:    */
     280:   size_t receive(void *data, size_t data_size,
>>>  281:                  TickType_t timeout = portMAX_DELAY) {
     282:     return xStreamBufferReceive(m_stream_buffer, data, data_size, timeout);
     283:   }
     284:   /**
```

**Violation 13**: freertos_stream_buffer.hpp:84:58
*Reason: Coding Style*

```cpp
      81:  */
      82: template <size_t StreamBufferSize> class dynamic_stream_buffer_allocator {
      83: public:
>>>   84:   StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
      85:     return xStreamBufferCreate(StreamBufferSize, trigger_level_bytes);
      86:   }
      87: };
```

**Violation 14**: freertos_stream_buffer.hpp:69:58
*Reason: Coding Style*

```cpp
      66:   static_stream_buffer_allocator &
      67:   operator=(static_stream_buffer_allocator &&) = delete;
      68: 
>>>   69:   StreamBufferHandle_t create(size_t trigger_level_bytes = 1) {
      70:     return xStreamBufferCreateStatic(StreamBufferSize, trigger_level_bytes,
      71:                                      m_storage.data(),
      72:                                      &m_stream_buffer_placeholder);
```

#### MISRA C 2012 Rule 2.3 (2 violation(s))

**A project should not contain unused type declarations**

Unused type declarations may indicate incomplete code or can be removed to simplify the codebase. This rule helps identify dead code and improves maintainability.

*Rationale: Unused declarations can confuse developers and increase compilation time unnecessarily.*

**Violation 1**: freertos_queue.hpp:47:0
*Reason: Coding Style*

```cpp
      44: 
      45: namespace freertos {
      46: 
>>>   47: using std::optional;
      48: 
      49: #if configSUPPORT_STATIC_ALLOCATION
      50: /**
```

**Violation 2**: freertos_queue.hpp:47:0
*Reason: Coding Style*

```cpp
      44: 
      45: namespace freertos {
      46: 
>>>   47: using std::optional;
      48: 
      49: #if configSUPPORT_STATIC_ALLOCATION
      50: /**
```

#### MISRA C 2012 Rule 2.7 (15 violation(s))

**There should be no unused parameters in functions**

Function parameters that are not used may indicate an error in the implementation or can be removed to clarify the interface. If a parameter must remain for interface compatibility, it should be explicitly marked as unused.

*Rationale: Unused parameters can indicate incomplete implementations or interface issues and should be explicitly handled.*

**Violation 1**: freertos_semaphore.hpp:295:43
*Reason: Coding Style*

```cpp
     292:    * @param max_count maximum count of the counting semaphore.
     293:    *
     294:    */
>>>  295:   explicit counting_semaphore(UBaseType_t max_count = 1)
     296:       : m_semaphore{m_allocator.create_counting(max_count)} {
     297:     configASSERT(m_semaphore);
     298:   }
```

**Violation 2**: freertos_semaphore.hpp:424:23
*Reason: Coding Style*

```cpp
     421:    */
     422:   // NOLINTNEXTLINE(cert-dcl21-cpp): RAII class, copy is deleted -
     423:   // post-increment returns reference instead of copy
>>>  424:   counting_semaphore &operator++(int) {
     425:     give();
     426:     return *this;
     427:   }
```

**Violation 3**: freertos_semaphore.hpp:436:23
*Reason: Coding Style*

```cpp
     433:    */
     434:   // NOLINTNEXTLINE(cert-dcl21-cpp): RAII class, copy is deleted -
     435:   // post-decrement returns reference instead of copy
>>>  436:   counting_semaphore &operator--(int) {
     437:     take();
     438:     return *this;
     439:   }
```

**Violation 4**: freertos_semaphore.hpp:787:30
*Reason: Coding Style*

```cpp
     784:    *
     785:    * @param mutex mutex to guard
     786:    */
>>>  787:   explicit lock_guard(Mutex &mutex) : m_mutex{mutex} { m_mutex.lock(); }
     788: 
     789:   /**
     790:    * @brief Destruct the lock guard object and unlock the mutex.
```

**Violation 5**: freertos_semaphore.hpp:826:34
*Reason: Coding Style*

```cpp
     823:    *
     824:    * @param mutex mutex to guard
     825:    */
>>>  826:   explicit try_lock_guard(Mutex &mutex)
     827:       : m_mutex{mutex}, m_lock_acquired{static_cast<bool>(m_mutex.try_lock())} {
     828:   }
     829:   /**
```

**Violation 6**: freertos_semaphore.hpp:870:34
*Reason: Coding Style*

```cpp
     867:    *
     868:    * @param mutex mutex to guard
     869:    */
>>>  870:   explicit lock_guard_isr(Mutex &mutex) : m_mutex{mutex} {
     871:     m_mutex.lock_isr(m_high_priority_task_woken);
     872:   }
     873:   /**
```

**Violation 7**: freertos_semaphore.hpp:920:29
*Reason: Coding Style*

```cpp
     917:    * @param mutex mutex to guard
     918:    * @param ticks_to_wait timeout in ticks to wait for the mutex.
     919:    */
>>>  920:   timeout_lock_guard(Mutex &mutex, TickType_t ticks_to_wait)
     921:       : m_mutex{mutex},
     922:         m_lock_acquired{static_cast<bool>(m_mutex.lock(ticks_to_wait))} {}
     923:   /**
```

**Violation 8**: freertos_semaphore.hpp:930:29
*Reason: Coding Style*

```cpp
     927:    * @param timeout timeout to wait for the mutex.
     928:    */
     929:   template <typename Rep, typename Period>
>>>  930:   timeout_lock_guard(Mutex &mutex,
     931:                      const std::chrono::duration<Rep, Period> &timeout)
     932:       : m_mutex{mutex},
     933:         m_lock_acquired{static_cast<bool>(m_mutex.lock(
```

**Violation 9**: freertos_semaphore.hpp:931:64
*Reason: Coding Style*

```cpp
     928:    */
     929:   template <typename Rep, typename Period>
     930:   timeout_lock_guard(Mutex &mutex,
>>>  931:                      const std::chrono::duration<Rep, Period> &timeout)
     932:       : m_mutex{mutex},
     933:         m_lock_acquired{static_cast<bool>(m_mutex.lock(
     934:             std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
```

**Violation 10**: freertos_sw_timer.hpp:130:30
*Reason: Coding Style*

```cpp
     127:    * @param auto_reload auto-reload flag
     128:    * @param callback callback routine
     129:    */
>>>  130:   explicit timer(const char *name, const TickType_t period_ticks,
     131:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     132:       : m_callback{std::move(callback)}, m_started{false},
     133:         m_timer{m_allocator.create(name, period_ticks, auto_reload, this,
```

**Violation 11**: freertos_sw_timer.hpp:131:30
*Reason: Coding Style*

```cpp
     128:    * @param callback callback routine
     129:    */
     130:   explicit timer(const char *name, const TickType_t period_ticks,
>>>  131:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     132:       : m_callback{std::move(callback)}, m_started{false},
     133:         m_timer{m_allocator.create(name, period_ticks, auto_reload, this,
     134:                                    callback_wrapper)} {
```

**Violation 12**: freertos_sw_timer.hpp:148:30
*Reason: Coding Style*

```cpp
     145:    * @param callback callback routine
     146:    */
     147:   template <typename Rep, typename Period>
>>>  148:   explicit timer(const char *name,
     149:                  const std::chrono::duration<Rep, Period> &period,
     150:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     151:       : timer{name,
```

**Violation 13**: freertos_sw_timer.hpp:149:60
*Reason: Coding Style*

```cpp
     146:    */
     147:   template <typename Rep, typename Period>
     148:   explicit timer(const char *name,
>>>  149:                  const std::chrono::duration<Rep, Period> &period,
     150:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     151:       : timer{name,
     152:               static_cast<TickType_t>(
```

**Violation 14**: freertos_sw_timer.hpp:150:30
*Reason: Coding Style*

```cpp
     147:   template <typename Rep, typename Period>
     148:   explicit timer(const char *name,
     149:                  const std::chrono::duration<Rep, Period> &period,
>>>  150:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     151:       : timer{name,
     152:               static_cast<TickType_t>(
     153:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
```

**Violation 15**: freertos_stream_buffer.hpp:108:33
*Reason: Coding Style*

```cpp
     105:    * @param trigger_level_bytes the number of bytes that must be in the stream
     106:    * buffer before a task that is blocked on a read operation will be unblocked
     107:    */
>>>  108:   explicit stream_buffer(size_t trigger_level_bytes = 1)
     109:       : m_stream_buffer{m_allocator.create(trigger_level_bytes)} {
     110:     configASSERT(m_stream_buffer);
     111:   }
```

#### MISRA C 2012 Rule 20.9 (25 violation(s))

**All identifiers used in the controlling expression of #if or #elif preprocessing directives shall be #define'd before evaluation**

Preprocessor identifiers used in conditional compilation should be explicitly defined to avoid undefined behavior in preprocessor evaluation.

*Rationale: Undefined preprocessor identifiers evaluate to 0, which may not be the intended behavior.*

**Violation 1**: freertos_queue.hpp:49:2
*Reason: Coding Style*

```cpp
      46: 
      47: using std::optional;
      48: 
>>>   49: #if configSUPPORT_STATIC_ALLOCATION
      50: /**
      51:  * @brief An allocator for the queue that uses a static memory allocation.
      52:  *
```

**Violation 2**: freertos_queue.hpp:73:2
*Reason: Coding Style*

```cpp
      70:   }
      71: };
      72: #endif
>>>   73: #if configSUPPORT_DYNAMIC_ALLOCATION
      74: /**
      75:  * @brief An allocator for the queue that uses a dynamic memory allocation.
      76:  *
```

**Violation 3**: freertos_queue.hpp:687:2
*Reason: Coding Style*

```cpp
     684:   }
     685: };
     686: 
>>>  687: #if configSUPPORT_STATIC_ALLOCATION
     688: /**
     689:  * @brief A namespace for the FreeRTOS queue with a static memory allocation.
     690:  *
```

**Violation 4**: freertos_queue.hpp:702:2
*Reason: Coding Style*

```cpp
     699:     freertos::queue<QueueLength, T, static_queue_allocator<QueueLength, T>>;
     700: } // namespace sa
     701: #endif
>>>  702: #if configSUPPORT_DYNAMIC_ALLOCATION
     703: /**
     704:  * @brief A namespace for the FreeRTOS queue with a dynamic memory allocation.
     705:  *
```

**Violation 5**: freertos_semaphore.hpp:47:2
*Reason: Coding Style*

```cpp
      44: 
      45: namespace freertos {
      46: 
>>>   47: #if configSUPPORT_STATIC_ALLOCATION
      48: /**
      49:  * @brief An allocator for the semaphore that uses a static memory allocation.
      50:  *
```

**Violation 6**: freertos_semaphore.hpp:80:2
*Reason: Coding Style*

```cpp
      77:   }
      78: };
      79: #endif
>>>   80: #if configSUPPORT_DYNAMIC_ALLOCATION
      81: /**
      82:  * @brief An allocator for the semaphore that uses a dynamic memory allocation.
      83:  *
```

**Violation 7**: freertos_semaphore.hpp:960:2
*Reason: Coding Style*

```cpp
     957:   bool locked(void) const { return m_lock_acquired && m_mutex.locked(); }
     958: };
     959: 
>>>  960: #if configSUPPORT_STATIC_ALLOCATION
     961: /**
     962:  * @brief A wrapper for the FreeRTOS binary semaphore with static memory
     963:  * allocation.
```

**Violation 8**: freertos_semaphore.hpp:992:2
*Reason: Coding Style*

```cpp
     989:     freertos::recursive_mutex<freertos::static_semaphore_allocator>;
     990: } // namespace sa
     991: #endif
>>>  992: #if configSUPPORT_DYNAMIC_ALLOCATION
     993: /**
     994:  * @brief A wrapper for the FreeRTOS binary semaphore with dynamic memory
     995:  * allocation.
```

**Violation 9**: freertos_sw_timer.hpp:49:2
*Reason: Coding Style*

```cpp
      46: 
      47: namespace freertos {
      48: 
>>>   49: #if configUSE_TIMERS
      50: 
      51: using std::function;
      52: 
```

**Violation 10**: freertos_sw_timer.hpp:53:2
*Reason: Coding Style*

```cpp
      50: 
      51: using std::function;
      52: 
>>>   53: #if configSUPPORT_STATIC_ALLOCATION
      54: /**
      55:  * @brief An allocator for the software timer that uses a static memory
      56:  * allocation.
```

**Violation 11**: freertos_sw_timer.hpp:597:2
*Reason: Coding Style*

```cpp
     594:   }
     595: };
     596: 
>>>  597: #if configSUPPORT_STATIC_ALLOCATION
     598: /**
     599:  * @brief Namespace for the kernel objects static memory allocation.
     600:  *
```

**Violation 12**: freertos_sw_timer.hpp:80:2
*Reason: Coding Style*

```cpp
      77:   }
      78: };
      79: #endif
>>>   80: #if configSUPPORT_DYNAMIC_ALLOCATION
      81: /**
      82:  * @brief An allocator for the software timer that uses a dynamic memory
      83:  * allocation.
```

**Violation 13**: freertos_sw_timer.hpp:610:2
*Reason: Coding Style*

```cpp
     607: using timer = freertos::timer<freertos::static_sw_timer_allocator>;
     608: } // namespace sa
     609: #endif
>>>  610: #if configSUPPORT_DYNAMIC_ALLOCATION
     611: /**
     612:  * @brief Namespace for the kernel objects dynamic memory allocation.
     613:  *
```

**Violation 14**: freertos_stream_buffer.hpp:47:2
*Reason: Coding Style*

```cpp
      44: 
      45: namespace freertos {
      46: 
>>>   47: #if configSUPPORT_STATIC_ALLOCATION
      48: /**
      49:  * @brief An allocator for the stream buffer that uses a static memory
      50:  * allocation.
```

**Violation 15**: freertos_stream_buffer.hpp:76:2
*Reason: Coding Style*

```cpp
      73:   }
      74: };
      75: #endif
>>>   76: #if configSUPPORT_DYNAMIC_ALLOCATION
      77: /**
      78:  * @brief An allocator for the stream buffer that uses a dynamic memory
      79:  * allocation.
```

**Violation 16**: freertos_stream_buffer.hpp:387:2
*Reason: Coding Style*

```cpp
     384:   BaseType_t full(void) { return xStreamBufferIsFull(m_stream_buffer); }
     385: };
     386: 
>>>  387: #if configSUPPORT_STATIC_ALLOCATION
     388: /**
     389:  * @brief Namespace for the FreeRTOS stream buffer with static memory allocation
     390:  *
```

**Violation 17**: freertos_stream_buffer.hpp:404:2
*Reason: Coding Style*

```cpp
     401:     freertos::static_stream_buffer_allocator<StreamBufferSize>>;
     402: } // namespace sa
     403: #endif
>>>  404: #if configSUPPORT_DYNAMIC_ALLOCATION
     405: /**
     406:  * @brief Namespace for the FreeRTOS stream buffer with dynamic memory
     407:  * allocation
```

**Violation 18**: freertos_event_group.hpp:45:2
*Reason: Coding Style*

```cpp
      42: 
      43: namespace freertos {
      44: 
>>>   45: #if configSUPPORT_STATIC_ALLOCATION
      46: /**
      47:  * @brief An allocator for the event group that uses a static memory allocation.
      48:  *
```

**Violation 19**: freertos_event_group.hpp:68:2
*Reason: Coding Style*

```cpp
      65:   }
      66: };
      67: #endif
>>>   68: #if configSUPPORT_DYNAMIC_ALLOCATION
      69: /**
      70:  * @brief An allocator for the event group that uses a dynamic memory
      71:  * allocation.
```

**Violation 20**: freertos_event_group.hpp:235:2
*Reason: Coding Style*

```cpp
     232:   }
     233: };
     234: 
>>>  235: #if configSUPPORT_STATIC_ALLOCATION
     236: /**
     237:  * @brief Namespace for aliases of RTOS kernel objects that use a static memory
     238:  *
```

**Violation 21**: freertos_event_group.hpp:249:2
*Reason: Coding Style*

```cpp
     246:     freertos::event_group<freertos::static_event_group_allocator>;
     247: } // namespace sa
     248: #endif
>>>  249: #if configSUPPORT_DYNAMIC_ALLOCATION
     250: /**
     251:  * @brief Namespace for aliases of RTOS kernel objects that use a dynamic memory
     252:  *
```

**Violation 22**: freertos_message_buffer.hpp:46:2
*Reason: Coding Style*

```cpp
      43: 
      44: namespace freertos {
      45: 
>>>   46: #if configSUPPORT_STATIC_ALLOCATION
      47: /**
      48:  * @brief An allocator for the message buffer that uses a static memory
      49:  * allocation.
```

**Violation 23**: freertos_message_buffer.hpp:74:2
*Reason: Coding Style*

```cpp
      71:   }
      72: };
      73: #endif
>>>   74: #if configSUPPORT_DYNAMIC_ALLOCATION
      75: /**
      76:  * @brief An allocator for the message buffer that uses a dynamic memory
      77:  * allocation.
```

**Violation 24**: freertos_message_buffer.hpp:219:2
*Reason: Coding Style*

```cpp
     216:   BaseType_t full(void) { return xMessageBufferIsFull(m_message_buffer); }
     217: };
     218: 
>>>  219: #if configSUPPORT_STATIC_ALLOCATION
     220: /**
     221:  * @brief Namespace for the FreeRTOS message buffer with static memory
     222:  * allocation
```

**Violation 25**: freertos_message_buffer.hpp:237:2
*Reason: Coding Style*

```cpp
     234:     freertos::static_message_buffer_allocator<MessageBufferSize>>;
     235: } // namespace sa
     236: #endif
>>>  237: #if configSUPPORT_DYNAMIC_ALLOCATION
     238: /**
     239:  * @brief Namespace for the FreeRTOS message buffer with dynamic memory
     240:  * allocation
```

#### MISRA C 2012 Rule 21.2 (1 violation(s))

**A reserved identifier or reserved macro name shall not be declared**

Identifiers reserved by the C standard (such as those beginning with underscore) should not be used in user code to avoid conflicts with implementation-defined behavior.

*Rationale: Using reserved identifiers can conflict with compiler and standard library implementations.*

**Violation 1**: freertos_stream_buffer.hpp:346:10
*Reason: Coding Style*

```cpp
     343:    *
     344:    * @return size_t Number of bytes free in the stream buffer.
     345:    */
>>>  346:   size_t free(void) { return xStreamBufferSpacesAvailable(m_stream_buffer); }
     347:   /**
     348:    * @brief Reset the stream buffer to the cleared state.
     349:    * @ref https://www.freertos.org/xStreamBufferReset.html
```

#### MISRA C 2012 Rule 3.1 (2 violation(s))

**The character sequences /* and // shall not appear in /* comment */**

Comment delimiters should not appear within comments to avoid confusion about comment boundaries and prevent accidental code commenting.

*Rationale: Nested comment sequences can lead to unexpected comment boundaries and code being accidentally commented out.*

**Violation 1**: freertos_queue.hpp:84:1
*Reason: Coding Style*

```cpp
      81: };
      82: #endif
      83: 
>>>   84: /**
      85:  * @brief A modern C++ wrapper for FreeRTOS queues with type safety.
      86:  *
      87:  * This class provides a type-safe, RAII wrapper around FreeRTOS queues for
```

**Violation 2**: freertos_semaphore.hpp:98:1
*Reason: Coding Style*

```cpp
      95: };
      96: #endif
      97: 
>>>   98: /**
      99:  * @brief A modern C++ wrapper for FreeRTOS binary semaphores.
     100:  *
     101:  * Binary semaphores are used for both mutual exclusion and task
```

#### MISRA C 2012 Rule 5.8 (16 violation(s))

**Identifiers that define objects or functions with external linkage shall be unique**

External identifiers must be unique to avoid linking errors and undefined behavior when linking multiple translation units.

*Rationale: Non-unique external identifiers can cause linking errors and unpredictable behavior in multi-file projects.*

**Violation 1**: freertos_task.hpp:221:20
*Reason: Coding Style*

```cpp
     218:    * @param priority  priority of the task
     219:    * @param task_routine  task routine function
     220:    */
>>>  221:   task(const char *name, UBaseType_t priority, task_routine_t &&task_routine)
     222:       : m_allocator{}, m_taskRoutine{task_routine},
     223:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     224:   /**
```

**Violation 2**: freertos_task.hpp:853:15
*Reason: Coding Style*

```cpp
     850:    *
     851:    * @return const char*  task name
     852:    */
>>>  853:   const char *name(void) const { return m_task.name(); }
     854: // Task notification API
     855: #if configUSE_TASK_NOTIFICATIONS
     856:   /**
```

**Violation 3**: freertos_semaphore.hpp:452:46
*Reason: Coding Style*

```cpp
     449:    * @param count the number of times to give the counting semaphore.
     450:    * @return counting_semaphore& reference to the counting semaphore.
     451:    */
>>>  452:   counting_semaphore &operator+=(UBaseType_t count) {
     453:     while (count--) {
     454:       give();
     455:     }
```

**Violation 4**: freertos_semaphore.hpp:486:4
*Reason: Coding Style*

```cpp
     483:    * created.
     484:    *
     485:    */
>>>  486:   ~mutex(void) {
     487:     if (m_semaphore) {
     488:       vSemaphoreDelete(m_semaphore);
     489:     }
```

**Violation 5**: freertos_semaphore.hpp:530:16
*Reason: Coding Style*

```cpp
     527:    * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
     528:    */
     529:   BaseType_t unlock_isr(void) {
>>>  530:     BaseType_t high_priority_task_woken = pdFALSE;
     531:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     532:     if (rc) {
     533:       m_locked = false;
```

**Violation 6**: freertos_semaphore.hpp:891:14
*Reason: Coding Style*

```cpp
     888:    * @return BaseType_t pdTRUE if the high priority task was woken, otherwise
     889:    * pdFALSE.
     890:    */
>>>  891:   BaseType_t high_priority_task_woken(void) const {
     892:     return m_high_priority_task_woken;
     893:   }
     894:   /**
```

**Violation 7**: freertos_task.hpp:221:20
*Reason: Coding Style*

```cpp
     218:    * @param priority  priority of the task
     219:    * @param task_routine  task routine function
     220:    */
>>>  221:   task(const char *name, UBaseType_t priority, task_routine_t &&task_routine)
     222:       : m_allocator{}, m_taskRoutine{task_routine},
     223:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     224:   /**
```

**Violation 8**: freertos_task.hpp:853:15
*Reason: Coding Style*

```cpp
     850:    *
     851:    * @return const char*  task name
     852:    */
>>>  853:   const char *name(void) const { return m_task.name(); }
     854: // Task notification API
     855: #if configUSE_TASK_NOTIFICATIONS
     856:   /**
```

**Violation 9**: freertos_semaphore.hpp:452:46
*Reason: Coding Style*

```cpp
     449:    * @param count the number of times to give the counting semaphore.
     450:    * @return counting_semaphore& reference to the counting semaphore.
     451:    */
>>>  452:   counting_semaphore &operator+=(UBaseType_t count) {
     453:     while (count--) {
     454:       give();
     455:     }
```

**Violation 10**: freertos_semaphore.hpp:445:15
*Reason: Coding Style*

```cpp
     442:    *
     443:    * @return UBaseType_t count of the counting semaphore.
     444:    */
>>>  445:   UBaseType_t count() { return uxSemaphoreGetCount(m_semaphore); }
     446:   /**
     447:    * @brief Give the counting semaphore.
     448:    *
```

**Violation 11**: freertos_semaphore.hpp:787:30
*Reason: Coding Style*

```cpp
     784:    *
     785:    * @param mutex mutex to guard
     786:    */
>>>  787:   explicit lock_guard(Mutex &mutex) : m_mutex{mutex} { m_mutex.lock(); }
     788: 
     789:   /**
     790:    * @brief Destruct the lock guard object and unlock the mutex.
```

**Violation 12**: freertos_semaphore.hpp:486:4
*Reason: Coding Style*

```cpp
     483:    * created.
     484:    *
     485:    */
>>>  486:   ~mutex(void) {
     487:     if (m_semaphore) {
     488:       vSemaphoreDelete(m_semaphore);
     489:     }
```

**Violation 13**: freertos_semaphore.hpp:530:16
*Reason: Coding Style*

```cpp
     527:    * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
     528:    */
     529:   BaseType_t unlock_isr(void) {
>>>  530:     BaseType_t high_priority_task_woken = pdFALSE;
     531:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     532:     if (rc) {
     533:       m_locked = false;
```

**Violation 14**: freertos_semaphore.hpp:891:14
*Reason: Coding Style*

```cpp
     888:    * @return BaseType_t pdTRUE if the high priority task was woken, otherwise
     889:    * pdFALSE.
     890:    */
>>>  891:   BaseType_t high_priority_task_woken(void) const {
     892:     return m_high_priority_task_woken;
     893:   }
     894:   /**
```

**Violation 15**: freertos_task.hpp:221:20
*Reason: Coding Style*

```cpp
     218:    * @param priority  priority of the task
     219:    * @param task_routine  task routine function
     220:    */
>>>  221:   task(const char *name, UBaseType_t priority, task_routine_t &&task_routine)
     222:       : m_allocator{}, m_taskRoutine{task_routine},
     223:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     224:   /**
```

**Violation 16**: freertos_task.hpp:853:15
*Reason: Coding Style*

```cpp
     850:    *
     851:    * @return const char*  task name
     852:    */
>>>  853:   const char *name(void) const { return m_task.name(); }
     854: // Task notification API
     855: #if configUSE_TASK_NOTIFICATIONS
     856:   /**
```

#### MISRA C 2012 Rule 8.2 (45 violation(s))

**Function types shall be in prototype form with named parameters**

Function declarations should include parameter names in addition to types. This improves code readability and helps with documentation.

*Rationale: Named parameters in function prototypes make the interface clearer and improve code documentation.*

**Violation 1**: freertos_queue.hpp:206:15
*Reason: Coding Style*

```cpp
     203:       vQueueAddToRegistry(m_queue, registred_name);
     204:     }
     205:   }
>>>  206:   queue(const queue &) = delete;
     207:   queue(queue &&src) = delete;
     208:   ~queue(void) {
     209:     if (m_queue) {
```

**Violation 2**: freertos_queue.hpp:218:26
*Reason: Coding Style*

```cpp
     215:     }
     216:   }
     217: 
>>>  218:   queue &operator=(const queue &) = delete;
     219:   queue &operator=(queue &&src) = delete;
     220: 
     221:   /**
```

**Violation 3**: freertos_semaphore.hpp:174:19
*Reason: Coding Style*

```cpp
     171:    * @brief Construct a new binary semaphore object
     172:    *
     173:    */
>>>  174:   binary_semaphore() : m_semaphore{m_allocator.create_binary()} {
     175:     configASSERT(m_semaphore);
     176:   }
     177:   binary_semaphore(const binary_semaphore &) = delete;
```

**Violation 4**: freertos_semaphore.hpp:177:26
*Reason: Coding Style*

```cpp
     174:   binary_semaphore() : m_semaphore{m_allocator.create_binary()} {
     175:     configASSERT(m_semaphore);
     176:   }
>>>  177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
     180:    * @brief Destruct the binary semaphore object and
```

**Violation 5**: freertos_semaphore.hpp:190:37
*Reason: Coding Style*

```cpp
     187:     }
     188:   }
     189: 
>>>  190:   binary_semaphore &operator=(const binary_semaphore &) = delete;
     191:   binary_semaphore &operator=(binary_semaphore &&src) = delete;
     192: 
     193:   /**
```

**Violation 6**: freertos_semaphore.hpp:201:18
*Reason: Coding Style*

```cpp
     198:    * otherwise pdFALSE.
     199:    *
     200:    */
>>>  201:   BaseType_t give() { return xSemaphoreGive(m_semaphore); }
     202:   /**
     203:    * @brief Give the binary semaphore from an ISR.
     204:    * @ref https://www.freertos.org/a00124.html
```

**Violation 7**: freertos_semaphore.hpp:299:28
*Reason: Coding Style*

```cpp
     296:       : m_semaphore{m_allocator.create_counting(max_count)} {
     297:     configASSERT(m_semaphore);
     298:   }
>>>  299:   counting_semaphore(const counting_semaphore &) = delete;
     300:   counting_semaphore(counting_semaphore &&src) = delete;
     301:   /**
     302:    * @brief Destruct the counting semaphore object and
```

**Violation 8**: freertos_semaphore.hpp:312:39
*Reason: Coding Style*

```cpp
     309:     }
     310:   }
     311: 
>>>  312:   counting_semaphore &operator=(const counting_semaphore &) = delete;
     313:   counting_semaphore &operator=(counting_semaphore &&src) = delete;
     314: 
     315:   /**
```

**Violation 9**: freertos_semaphore.hpp:323:18
*Reason: Coding Style*

```cpp
     320:    * otherwise pdFALSE.
     321:    *
     322:    */
>>>  323:   BaseType_t give() { return xSemaphoreGive(m_semaphore); }
     324:   /**
     325:    * @brief Give the counting semaphore from an ISR.
     326:    * @ref https://www.freertos.org/a00124.html
```

**Violation 10**: freertos_semaphore.hpp:403:33
*Reason: Coding Style*

```cpp
     400:    *
     401:    * @return counting_semaphore& reference to the counting semaphore.
     402:    */
>>>  403:   counting_semaphore &operator++() {
     404:     give();
     405:     return *this;
     406:   }
```

**Violation 11**: freertos_semaphore.hpp:412:33
*Reason: Coding Style*

```cpp
     409:    *
     410:    * @return counting_semaphore& reference to the counting semaphore.
     411:    */
>>>  412:   counting_semaphore &operator--() {
     413:     take();
     414:     return *this;
     415:   }
```

**Violation 12**: freertos_semaphore.hpp:445:20
*Reason: Coding Style*

```cpp
     442:    *
     443:    * @return UBaseType_t count of the counting semaphore.
     444:    */
>>>  445:   UBaseType_t count() { return uxSemaphoreGetCount(m_semaphore); }
     446:   /**
     447:    * @brief Give the counting semaphore.
     448:    *
```

**Violation 13**: freertos_semaphore.hpp:476:8
*Reason: Coding Style*

```cpp
     473:    * @brief Construct a new mutex object
     474:    *
     475:    */
>>>  476:   mutex() : m_semaphore{m_allocator.create_mutex()}, m_locked{false} {
     477:     configASSERT(m_semaphore);
     478:   }
     479:   mutex(const mutex &) = delete;
```

**Violation 14**: freertos_semaphore.hpp:479:15
*Reason: Coding Style*

```cpp
     476:   mutex() : m_semaphore{m_allocator.create_mutex()}, m_locked{false} {
     477:     configASSERT(m_semaphore);
     478:   }
>>>  479:   mutex(const mutex &) = delete;
     480:   mutex(mutex &&src) = delete;
     481:   /**
     482:    * @brief Destruct the mutex object and delete the mutex instance if it was
```

**Violation 15**: freertos_semaphore.hpp:492:26
*Reason: Coding Style*

```cpp
     489:     }
     490:   }
     491: 
>>>  492:   mutex &operator=(const mutex &) = delete;
     493:   mutex &operator=(mutex &&src) = delete;
     494: 
     495:   /**
```

**Violation 16**: freertos_semaphore.hpp:501:20
*Reason: Coding Style*

```cpp
     498:    *
     499:    * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
     500:    */
>>>  501:   BaseType_t unlock() {
     502:     auto rc = xSemaphoreGive(m_semaphore);
     503:     if (rc) {
     504:       m_locked = false;
```

**Violation 17**: freertos_semaphore.hpp:597:22
*Reason: Coding Style*

```cpp
     594:    *
     595:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     596:    */
>>>  597:   BaseType_t try_lock() {
     598:     auto rc = xSemaphoreTake(m_semaphore, 0);
     599:     if (rc) {
     600:       m_locked = true;
```

**Violation 18**: freertos_semaphore.hpp:628:18
*Reason: Coding Style*

```cpp
     625:    * @brief Construct a new recursive mutex object
     626:    *
     627:    */
>>>  628:   recursive_mutex() : m_semaphore{m_allocator.create_recursive_mutex()} {
     629:     configASSERT(m_semaphore);
     630:   }
     631:   recursive_mutex(const recursive_mutex &) = delete;
```

**Violation 19**: freertos_semaphore.hpp:631:25
*Reason: Coding Style*

```cpp
     628:   recursive_mutex() : m_semaphore{m_allocator.create_recursive_mutex()} {
     629:     configASSERT(m_semaphore);
     630:   }
>>>  631:   recursive_mutex(const recursive_mutex &) = delete;
     632:   recursive_mutex(recursive_mutex &&src) = delete;
     633:   /**
     634:    * @brief Destruct the recursive mutex object and delete the recursive mutex
```

**Violation 20**: freertos_semaphore.hpp:644:36
*Reason: Coding Style*

```cpp
     641:     }
     642:   }
     643: 
>>>  644:   recursive_mutex &operator=(const recursive_mutex &) = delete;
     645:   recursive_mutex &operator=(recursive_mutex &&src) = delete;
     646: 
     647:   /**
```

**Violation 21**: freertos_semaphore.hpp:654:20
*Reason: Coding Style*

```cpp
     651:    * @return BaseType_t pdTRUE if the recursive mutex was successfully unlocked,
     652:    * otherwise pdFALSE.
     653:    */
>>>  654:   BaseType_t unlock() {
     655:     auto rc = xSemaphoreGiveRecursive(m_semaphore);
     656:     if (rc && m_recursions_count > 0) {
     657:       m_recursions_count--;
```

**Violation 22**: freertos_semaphore.hpp:750:22
*Reason: Coding Style*

```cpp
     747:    *
     748:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     749:    */
>>>  750:   BaseType_t try_lock() {
     751:     auto rc = xSemaphoreTakeRecursive(m_semaphore, 0);
     752:     if (rc) {
     753:       m_recursions_count++;
```

**Violation 23**: freertos_semaphore.hpp:796:20
*Reason: Coding Style*

```cpp
     793:   ~lock_guard(void) { m_mutex.unlock(); }
     794: 
     795:   // Delete copy and move operations for RAII safety
>>>  796:   lock_guard(const lock_guard &) = delete;
     797:   lock_guard(lock_guard &&) = delete;
     798:   lock_guard &operator=(const lock_guard &) = delete;
     799:   lock_guard &operator=(lock_guard &&) = delete;
```

**Violation 24**: freertos_semaphore.hpp:797:14
*Reason: Coding Style*

```cpp
     794: 
     795:   // Delete copy and move operations for RAII safety
     796:   lock_guard(const lock_guard &) = delete;
>>>  797:   lock_guard(lock_guard &&) = delete;
     798:   lock_guard &operator=(const lock_guard &) = delete;
     799:   lock_guard &operator=(lock_guard &&) = delete;
     800: 
```

**Violation 25**: freertos_semaphore.hpp:798:31
*Reason: Coding Style*

```cpp
     795:   // Delete copy and move operations for RAII safety
     796:   lock_guard(const lock_guard &) = delete;
     797:   lock_guard(lock_guard &&) = delete;
>>>  798:   lock_guard &operator=(const lock_guard &) = delete;
     799:   lock_guard &operator=(lock_guard &&) = delete;
     800: 
     801:   /**
```

**Violation 26**: freertos_semaphore.hpp:799:25
*Reason: Coding Style*

```cpp
     796:   lock_guard(const lock_guard &) = delete;
     797:   lock_guard(lock_guard &&) = delete;
     798:   lock_guard &operator=(const lock_guard &) = delete;
>>>  799:   lock_guard &operator=(lock_guard &&) = delete;
     800: 
     801:   /**
     802:    * @brief Checks if the mutex is locked.
```

**Violation 27**: freertos_semaphore.hpp:840:24
*Reason: Coding Style*

```cpp
     837:   }
     838: 
     839:   // Delete copy and move operations for RAII safety
>>>  840:   try_lock_guard(const try_lock_guard &) = delete;
     841:   try_lock_guard(try_lock_guard &&) = delete;
     842:   try_lock_guard &operator=(const try_lock_guard &) = delete;
     843:   try_lock_guard &operator=(try_lock_guard &&) = delete;
```

**Violation 28**: freertos_semaphore.hpp:841:18
*Reason: Coding Style*

```cpp
     838: 
     839:   // Delete copy and move operations for RAII safety
     840:   try_lock_guard(const try_lock_guard &) = delete;
>>>  841:   try_lock_guard(try_lock_guard &&) = delete;
     842:   try_lock_guard &operator=(const try_lock_guard &) = delete;
     843:   try_lock_guard &operator=(try_lock_guard &&) = delete;
     844: 
```

**Violation 29**: freertos_semaphore.hpp:842:35
*Reason: Coding Style*

```cpp
     839:   // Delete copy and move operations for RAII safety
     840:   try_lock_guard(const try_lock_guard &) = delete;
     841:   try_lock_guard(try_lock_guard &&) = delete;
>>>  842:   try_lock_guard &operator=(const try_lock_guard &) = delete;
     843:   try_lock_guard &operator=(try_lock_guard &&) = delete;
     844: 
     845:   /**
```

**Violation 30**: freertos_semaphore.hpp:843:29
*Reason: Coding Style*

```cpp
     840:   try_lock_guard(const try_lock_guard &) = delete;
     841:   try_lock_guard(try_lock_guard &&) = delete;
     842:   try_lock_guard &operator=(const try_lock_guard &) = delete;
>>>  843:   try_lock_guard &operator=(try_lock_guard &&) = delete;
     844: 
     845:   /**
     846:    * @brief Checks if the mutex is locked.
```

**Violation 31**: freertos_semaphore.hpp:880:24
*Reason: Coding Style*

```cpp
     877:   ~lock_guard_isr(void) { m_mutex.unlock_isr(m_high_priority_task_woken); }
     878: 
     879:   // Delete copy and move operations for RAII safety
>>>  880:   lock_guard_isr(const lock_guard_isr &) = delete;
     881:   lock_guard_isr(lock_guard_isr &&) = delete;
     882:   lock_guard_isr &operator=(const lock_guard_isr &) = delete;
     883:   lock_guard_isr &operator=(lock_guard_isr &&) = delete;
```

**Violation 32**: freertos_semaphore.hpp:881:18
*Reason: Coding Style*

```cpp
     878: 
     879:   // Delete copy and move operations for RAII safety
     880:   lock_guard_isr(const lock_guard_isr &) = delete;
>>>  881:   lock_guard_isr(lock_guard_isr &&) = delete;
     882:   lock_guard_isr &operator=(const lock_guard_isr &) = delete;
     883:   lock_guard_isr &operator=(lock_guard_isr &&) = delete;
     884: 
```

**Violation 33**: freertos_semaphore.hpp:882:35
*Reason: Coding Style*

```cpp
     879:   // Delete copy and move operations for RAII safety
     880:   lock_guard_isr(const lock_guard_isr &) = delete;
     881:   lock_guard_isr(lock_guard_isr &&) = delete;
>>>  882:   lock_guard_isr &operator=(const lock_guard_isr &) = delete;
     883:   lock_guard_isr &operator=(lock_guard_isr &&) = delete;
     884: 
     885:   /**
```

**Violation 34**: freertos_semaphore.hpp:883:29
*Reason: Coding Style*

```cpp
     880:   lock_guard_isr(const lock_guard_isr &) = delete;
     881:   lock_guard_isr(lock_guard_isr &&) = delete;
     882:   lock_guard_isr &operator=(const lock_guard_isr &) = delete;
>>>  883:   lock_guard_isr &operator=(lock_guard_isr &&) = delete;
     884: 
     885:   /**
     886:    * @brief Checks if high priority task was woken.
```

**Violation 35**: freertos_semaphore.hpp:947:28
*Reason: Coding Style*

```cpp
     944:   }
     945: 
     946:   // Delete copy and move operations for RAII safety
>>>  947:   timeout_lock_guard(const timeout_lock_guard &) = delete;
     948:   timeout_lock_guard(timeout_lock_guard &&) = delete;
     949:   timeout_lock_guard &operator=(const timeout_lock_guard &) = delete;
     950:   timeout_lock_guard &operator=(timeout_lock_guard &&) = delete;
```

**Violation 36**: freertos_semaphore.hpp:948:22
*Reason: Coding Style*

```cpp
     945: 
     946:   // Delete copy and move operations for RAII safety
     947:   timeout_lock_guard(const timeout_lock_guard &) = delete;
>>>  948:   timeout_lock_guard(timeout_lock_guard &&) = delete;
     949:   timeout_lock_guard &operator=(const timeout_lock_guard &) = delete;
     950:   timeout_lock_guard &operator=(timeout_lock_guard &&) = delete;
     951: 
```

**Violation 37**: freertos_semaphore.hpp:949:39
*Reason: Coding Style*

```cpp
     946:   // Delete copy and move operations for RAII safety
     947:   timeout_lock_guard(const timeout_lock_guard &) = delete;
     948:   timeout_lock_guard(timeout_lock_guard &&) = delete;
>>>  949:   timeout_lock_guard &operator=(const timeout_lock_guard &) = delete;
     950:   timeout_lock_guard &operator=(timeout_lock_guard &&) = delete;
     951: 
     952:   /**
```

**Violation 38**: freertos_semaphore.hpp:950:33
*Reason: Coding Style*

```cpp
     947:   timeout_lock_guard(const timeout_lock_guard &) = delete;
     948:   timeout_lock_guard(timeout_lock_guard &&) = delete;
     949:   timeout_lock_guard &operator=(const timeout_lock_guard &) = delete;
>>>  950:   timeout_lock_guard &operator=(timeout_lock_guard &&) = delete;
     951: 
     952:   /**
     953:    * @brief Checks if the mutex is locked.
```

**Violation 39**: freertos_stream_buffer.hpp:112:23
*Reason: Coding Style*

```cpp
     109:       : m_stream_buffer{m_allocator.create(trigger_level_bytes)} {
     110:     configASSERT(m_stream_buffer);
     111:   }
>>>  112:   stream_buffer(const stream_buffer &) = delete;
     113:   stream_buffer(stream_buffer &&src) = delete;
     114:   /**
     115:    * @brief Destruct the stream buffer object and delete the stream buffer
```

**Violation 40**: freertos_stream_buffer.hpp:125:34
*Reason: Coding Style*

```cpp
     122:     }
     123:   }
     124: 
>>>  125:   stream_buffer &operator=(const stream_buffer &) = delete;
     126:   stream_buffer &operator=(stream_buffer &&src) = delete;
     127: 
     128:   /**
```

**Violation 41**: freertos_event_group.hpp:95:21
*Reason: Coding Style*

```cpp
      92:    *
      93:    */
      94:   event_group(void) : m_event_group(m_allocator.create()) {}
>>>   95:   event_group(const event_group &) = delete;
      96:   event_group(event_group &&other) = delete;
      97:   /**
      98:    * @brief Destruct the event group object and delete the event group instance
```

**Violation 42**: freertos_event_group.hpp:108:32
*Reason: Coding Style*

```cpp
     105:     }
     106:   }
     107: 
>>>  108:   event_group &operator=(const event_group &) = delete;
     109:   event_group &operator=(event_group &&other) = delete;
     110: 
     111:   /**
```

**Violation 43**: freertos_message_buffer.hpp:104:26
*Reason: Coding Style*

```cpp
     101:    * @brief Construct a new message buffer object
     102:    *
     103:    */
>>>  104:   explicit message_buffer() : m_message_buffer{m_allocator.create()} {
     105:     configASSERT(m_message_buffer);
     106:   }
     107:   message_buffer(const message_buffer &) = delete;
```

**Violation 44**: freertos_message_buffer.hpp:107:24
*Reason: Coding Style*

```cpp
     104:   explicit message_buffer() : m_message_buffer{m_allocator.create()} {
     105:     configASSERT(m_message_buffer);
     106:   }
>>>  107:   message_buffer(const message_buffer &) = delete;
     108:   message_buffer(message_buffer &&src) = delete;
     109:   /**
     110:    * @brief Destruct the message buffer object and delete the message buffer
```

**Violation 45**: freertos_message_buffer.hpp:120:35
*Reason: Coding Style*

```cpp
     117:     }
     118:   }
     119: 
>>>  120:   message_buffer &operator=(const message_buffer &) = delete;
     121:   message_buffer &operator=(message_buffer &&src) = delete;
     122: 
     123:   /**
```

#### MISRA C 2012 Rule 8.5 (6 violation(s))

**An external object or function shall be declared once in one and only one file**

External objects and functions should have exactly one declaration to avoid multiple definition errors and ensure consistent interfaces.

*Rationale: Multiple declarations can lead to inconsistencies and linking errors.*

**Violation 1**: freertos_task.hpp:1074:6
*Reason: Coding Style*

```cpp
    1071:  * @tparam Duration duration type
    1072:  * @param timePoint time point to sleep until
    1073:  */
>>> 1074: void delay_until(const std::chrono::system_clock::time_point &wakeTime);
    1075: /**
    1076:  * @brief Sleep the task until the specified time point.
    1077:  *
```

**Violation 2**: freertos_task.hpp:1057:6
*Reason: Coding Style*

```cpp
    1054:  * @tparam Duration duration type
    1055:  * @param timePoint time point to sleep until
    1056:  */
>>> 1057: void delay_until(TickType_t &previousWakeTime, TickType_t period);
    1058: 
    1059: template <typename Rep, typename Period>
    1060: void delay_until(TickType_t &previousWakeTime,
```

**Violation 3**: freertos_task.hpp:1082:6
*Reason: Coding Style*

```cpp
    1079:  * @tparam Duration duration type
    1080:  * @param timePoint time point to sleep until
    1081:  */
>>> 1082: void delay_until(const std::chrono::steady_clock::time_point &wakeTime);
    1083: 
    1084: // task utilities:
    1085: 
```

**Violation 4**: freertos_task.hpp:1074:6
*Reason: Coding Style*

```cpp
    1071:  * @tparam Duration duration type
    1072:  * @param timePoint time point to sleep until
    1073:  */
>>> 1074: void delay_until(const std::chrono::system_clock::time_point &wakeTime);
    1075: /**
    1076:  * @brief Sleep the task until the specified time point.
    1077:  *
```

**Violation 5**: freertos_task.hpp:1057:6
*Reason: Coding Style*

```cpp
    1054:  * @tparam Duration duration type
    1055:  * @param timePoint time point to sleep until
    1056:  */
>>> 1057: void delay_until(TickType_t &previousWakeTime, TickType_t period);
    1058: 
    1059: template <typename Rep, typename Period>
    1060: void delay_until(TickType_t &previousWakeTime,
```

**Violation 6**: freertos_task.hpp:1082:6
*Reason: Coding Style*

```cpp
    1079:  * @tparam Duration duration type
    1080:  * @param timePoint time point to sleep until
    1081:  */
>>> 1082: void delay_until(const std::chrono::steady_clock::time_point &wakeTime);
    1083: 
    1084: // task utilities:
    1085: 
```

#### MISRA C 2012 Rule 8.6 (40 violation(s))

**An identifier with external linkage shall have exactly one external definition**

Each external identifier should be defined exactly once across the entire program to avoid multiple definition errors.

*Rationale: Multiple definitions of the same external identifier violate the one definition rule and can cause linking errors.*

**Violation 1**: freertos_task.cc:37:6
*Reason: Coding Style*

```cpp
      34: 
      35: namespace freertos {
      36: 
>>>   37: void delay(TickType_t ticks) { vTaskDelay(ticks); }
      38: void delay_until(TickType_t &previousWakeTime, TickType_t period) {
      39:   vTaskDelayUntil(&previousWakeTime, period);
      40: }
```

**Violation 2**: freertos_task.hpp:1033:6
*Reason: Coding Style*

```cpp
    1030:  * @param duration duration to delay
    1031:  */
    1032: template <typename Rep, typename Period>
>>> 1033: void delay(std::chrono::duration<Rep, Period> duration) {
    1034:   delay(
    1035:       std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    1036: }
```

**Violation 3**: freertos_task.cc:38:6
*Reason: Coding Style*

```cpp
      35: namespace freertos {
      36: 
      37: void delay(TickType_t ticks) { vTaskDelay(ticks); }
>>>   38: void delay_until(TickType_t &previousWakeTime, TickType_t period) {
      39:   vTaskDelayUntil(&previousWakeTime, period);
      40: }
      41: void delay_until(const std::chrono::system_clock::time_point &wakeTime) {
```

**Violation 4**: freertos_task.hpp:1060:6
*Reason: Coding Style*

```cpp
    1057: void delay_until(TickType_t &previousWakeTime, TickType_t period);
    1058: 
    1059: template <typename Rep, typename Period>
>>> 1060: void delay_until(TickType_t &previousWakeTime,
    1061:                  std::chrono::duration<Rep, Period> period) {
    1062:   delay_until(
    1063:       previousWakeTime,
```

**Violation 5**: freertos_message_buffer.hpp:120:19
*Reason: Coding Style*

```cpp
     117:     }
     118:   }
     119: 
>>>  120:   message_buffer &operator=(const message_buffer &) = delete;
     121:   message_buffer &operator=(message_buffer &&src) = delete;
     122: 
     123:   /**
```

**Violation 6**: freertos_event_group.hpp:109:16
*Reason: Coding Style*

```cpp
     106:   }
     107: 
     108:   event_group &operator=(const event_group &) = delete;
>>>  109:   event_group &operator=(event_group &&other) = delete;
     110: 
     111:   /**
     112:    * @brief Method to get the handle of the event group.
```

**Violation 7**: freertos_queue.hpp:218:10
*Reason: Coding Style*

```cpp
     215:     }
     216:   }
     217: 
>>>  218:   queue &operator=(const queue &) = delete;
     219:   queue &operator=(queue &&src) = delete;
     220: 
     221:   /**
```

**Violation 8**: freertos_message_buffer.hpp:121:19
*Reason: Coding Style*

```cpp
     118:   }
     119: 
     120:   message_buffer &operator=(const message_buffer &) = delete;
>>>  121:   message_buffer &operator=(message_buffer &&src) = delete;
     122: 
     123:   /**
     124:    * @brief Method sends a discret message to the message buffer.
```

**Violation 9**: freertos_queue.hpp:231:14
*Reason: Coding Style*

```cpp
     228:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     229:    * errQUEUE_FULL.
     230:    */
>>>  231:   BaseType_t send(const T &item, TickType_t ticks_to_wait) {
     232:     return xQueueSend(m_queue, &item, ticks_to_wait);
     233:   }
     234:   /**
```

**Violation 10**: freertos_message_buffer.hpp:148:10
*Reason: Coding Style*

```cpp
     145:    * @return size_t number of bytes sent
     146:    */
     147:   template <typename Rep, typename Period>
>>>  148:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
     149:               const std::chrono::duration<Rep, Period> &xTicksToWait) {
     150:     return send(
     151:         pvTxData, xDataLengthBytes,
```

**Violation 11**: freertos_queue.hpp:400:14
*Reason: Coding Style*

```cpp
     397:    * @return BaseType_t pdPASS if the item was successfully received, otherwise
     398:    * pdFALSE.
     399:    */
>>>  400:   BaseType_t receive(T &item, TickType_t ticks_to_wait) {
     401:     return xQueueReceive(m_queue, &item, ticks_to_wait);
     402:   }
     403:   /**
```

**Violation 12**: freertos_message_buffer.hpp:181:10
*Reason: Coding Style*

```cpp
     178:    * @return size_t number of bytes received
     179:    */
     180:   template <typename Rep, typename Period>
>>>  181:   size_t receive(void *pvRxData, size_t xBufferLengthBytes,
     182:                  const std::chrono::duration<Rep, Period> &timeout) {
     183:     return receive(
     184:         pvRxData, xBufferLengthBytes,
```

**Violation 13**: freertos_queue.hpp:520:14
*Reason: Coding Style*

```cpp
     517:    *
     518:    * @return BaseType_t pdPASS if the queue was reset, pdFAIL otherwise.
     519:    */
>>>  520:   BaseType_t reset(void) { return xQueueReset(m_queue); }
     521:   /**
     522:    * @brief A version of send_back method that overwrites the items in the queue
     523:    * if it is full.
```

**Violation 14**: freertos_message_buffer.hpp:202:14
*Reason: Coding Style*

```cpp
     199:    *
     200:    * @return BaseType_t pdPass if the message buffer was reset, pdFAIL otherwise
     201:    */
>>>  202:   BaseType_t reset(void) { return xMessageBufferReset(m_message_buffer); }
     203:   /**
     204:    * @brief Method checking if the message buffer is empty.
     205:    * @ref https://www.freertos.org/xMessageBufferIsEmpty.html
```

**Violation 15**: freertos_semaphore.hpp:190:21
*Reason: Coding Style*

```cpp
     187:     }
     188:   }
     189: 
>>>  190:   binary_semaphore &operator=(const binary_semaphore &) = delete;
     191:   binary_semaphore &operator=(binary_semaphore &&src) = delete;
     192: 
     193:   /**
```

**Violation 16**: freertos_queue.hpp:219:10
*Reason: Coding Style*

```cpp
     216:   }
     217: 
     218:   queue &operator=(const queue &) = delete;
>>>  219:   queue &operator=(queue &&src) = delete;
     220: 
     221:   /**
     222:    * @brief Posts an item to the back of a queue.
```

**Violation 17**: freertos_stream_buffer.hpp:125:18
*Reason: Coding Style*

```cpp
     122:     }
     123:   }
     124: 
>>>  125:   stream_buffer &operator=(const stream_buffer &) = delete;
     126:   stream_buffer &operator=(stream_buffer &&src) = delete;
     127: 
     128:   /**
```

**Violation 18**: freertos_semaphore.hpp:950:23
*Reason: Coding Style*

```cpp
     947:   timeout_lock_guard(const timeout_lock_guard &) = delete;
     948:   timeout_lock_guard(timeout_lock_guard &&) = delete;
     949:   timeout_lock_guard &operator=(const timeout_lock_guard &) = delete;
>>>  950:   timeout_lock_guard &operator=(timeout_lock_guard &&) = delete;
     951: 
     952:   /**
     953:    * @brief Checks if the mutex is locked.
```

**Violation 19**: freertos_stream_buffer.hpp:140:14
*Reason: Coding Style*

```cpp
     137:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     138:    * insufficient memory available to copy the data into the stream buffer.
     139:    */
>>>  140:   BaseType_t send(const void *data, size_t data_size,
     141:                   TickType_t timeout = portMAX_DELAY) {
     142:     return xStreamBufferSend(m_stream_buffer, data, data_size, timeout);
     143:   }
```

**Violation 20**: freertos_queue.hpp:244:14
*Reason: Coding Style*

```cpp
     241:    * errQUEUE_FULL.
     242:    */
     243:   template <typename Rep, typename Period>
>>>  244:   BaseType_t send(const T &item,
     245:                   const std::chrono::duration<Rep, Period> &timeout) {
     246:     return send(
     247:         item, pdMS_TO_TICKS(
```

**Violation 21**: freertos_stream_buffer.hpp:212:14
*Reason: Coding Style*

```cpp
     209:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     210:    * insufficient memory available to copy the data into the stream buffer.
     211:    */
>>>  212:   BaseType_t send_isr(const void *data, size_t data_size,
     213:                       BaseType_t &higher_priority_task_woken) {
     214:     return xStreamBufferSendFromISR(m_stream_buffer, data, data_size,
     215:                                     &higher_priority_task_woken);
```

**Violation 22**: freertos_queue.hpp:272:14
*Reason: Coding Style*

```cpp
     269:    * @return BaseType_t  pdPASS if the item was successfully posted, otherwise
     270:    * errQUEUE_FULL.
     271:    */
>>>  272:   BaseType_t send_isr(const T &item) {
     273:     BaseType_t higher_priority_task_woken = pdFALSE;
     274:     return xQueueSendFromISR(m_queue, &item, &higher_priority_task_woken);
     275:   }
```

**Violation 23**: freertos_stream_buffer.hpp:280:10
*Reason: Coding Style*

```cpp
     277:    * buffer.
     278:    * @return size_t Number of bytes received.
     279:    */
>>>  280:   size_t receive(void *data, size_t data_size,
     281:                  TickType_t timeout = portMAX_DELAY) {
     282:     return xStreamBufferReceive(m_stream_buffer, data, data_size, timeout);
     283:   }
```

**Violation 24**: freertos_queue.hpp:444:15
*Reason: Coding Style*

```cpp
     441:    * queue is empty.
     442:    */
     443:   template <typename Rep, typename Period>
>>>  444:   optional<T> receive(const std::chrono::duration<Rep, Period> &timeout) {
     445:     return receive(pdMS_TO_TICKS(
     446:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
     447:             .count()));
```

**Violation 25**: freertos_stream_buffer.hpp:313:10
*Reason: Coding Style*

```cpp
     310:    * data from the stream buffer, otherwise it will be set to pdFALSE.
     311:    * @return size_t Number of bytes received.
     312:    */
>>>  313:   size_t receive_isr(void *data, size_t data_size,
     314:                      BaseType_t &higher_priority_task_woken) {
     315:     return xStreamBufferReceiveFromISR(m_stream_buffer, data, data_size,
     316:                                        &higher_priority_task_woken);
```

**Violation 26**: freertos_queue.hpp:482:15
*Reason: Coding Style*

```cpp
     479:    * @return optional<T> The item received from the queue or std::nullopt if the
     480:    * queue is empty.
     481:    */
>>>  482:   optional<T> receive_isr(void) {
     483:     T item;
     484:     BaseType_t higher_priority_task_woken = pdFALSE;
     485:     if (xQueueReceiveFromISR(m_queue, &item, &higher_priority_task_woken) ==
```

**Violation 27**: freertos_stream_buffer.hpp:337:10
*Reason: Coding Style*

```cpp
     334:    *
     335:    * @return size_t Number of bytes available in the stream buffer.
     336:    */
>>>  337:   size_t available(void) {
     338:     return xStreamBufferBytesAvailable(m_stream_buffer);
     339:   }
     340:   /**
```

**Violation 28**: freertos_message_buffer.hpp:193:10
*Reason: Coding Style*

```cpp
     190:    *
     191:    * @return size_t the number of bytes available in the buffer
     192:    */
>>>  193:   size_t available(void) const {
     194:     return xMessageBufferSpaceAvailable(m_message_buffer);
     195:   }
     196:   /**
```

**Violation 29**: freertos_stream_buffer.hpp:353:14
*Reason: Coding Style*

```cpp
     350:    *
     351:    * @return BaseType_t pdPass if the stream buffer was reset, pdFAIL otherwise.
     352:    */
>>>  353:   BaseType_t reset(void) { return xStreamBufferReset(m_stream_buffer); }
     354:   /**
     355:    * @brief Set the trigger level of the stream buffer.
     356:    * @ref https://www.freertos.org/xStreamBufferSetTriggerLevel.html
```

**Violation 30**: freertos_stream_buffer.hpp:370:24
*Reason: Coding Style*

```cpp
     367:    *
     368:    * @return StreamBufferHandle_t Handle of the stream buffer.
     369:    */
>>>  370:   StreamBufferHandle_t handle(void) const { return m_stream_buffer; }
     371:   /**
     372:    * @brief Check if the stream buffer is empty.
     373:    * @ref https://www.freertos.org/xStreamBufferIsEmpty.html
```

**Violation 31**: freertos_event_group.hpp:117:22
*Reason: Coding Style*

```cpp
     114:    *
     115:    * @return EventGroupHandle_t event group handle
     116:    */
>>>  117:   EventGroupHandle_t handle(void) const { return m_event_group; }
     118: 
     119:   /**
     120:    * @brief Method to set bits in the event group.
```

**Violation 32**: freertos_stream_buffer.hpp:377:14
*Reason: Coding Style*

```cpp
     374:    *
     375:    * @return BaseType_t pdTRUE if the stream buffer is empty, pdFALSE otherwise.
     376:    */
>>>  377:   BaseType_t empty(void) { return xStreamBufferIsEmpty(m_stream_buffer); }
     378:   /**
     379:    * @brief Check if the stream buffer is full.
     380:    * @ref https://www.freertos.org/xStreamBufferIsFull.html
```

**Violation 33**: freertos_message_buffer.hpp:209:14
*Reason: Coding Style*

```cpp
     206:    *
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
>>>  209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
     210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
```

**Violation 34**: freertos_stream_buffer.hpp:384:14
*Reason: Coding Style*

```cpp
     381:    *
     382:    * @return BaseType_t pdTRUE if the stream buffer is full, pdFALSE otherwise.
     383:    */
>>>  384:   BaseType_t full(void) { return xStreamBufferIsFull(m_stream_buffer); }
     385: };
     386: 
     387: #if configSUPPORT_STATIC_ALLOCATION
```

**Violation 35**: freertos_message_buffer.hpp:216:14
*Reason: Coding Style*

```cpp
     213:    *
     214:    * @return BaseType_t pdTRUE if the message buffer is full, pdFALSE otherwise
     215:    */
>>>  216:   BaseType_t full(void) { return xMessageBufferIsFull(m_message_buffer); }
     217: };
     218: 
     219: #if configSUPPORT_STATIC_ALLOCATION
```

**Violation 36**: freertos_task.hpp:1219:21
*Reason: Coding Style*

```cpp
    1216:   // Delete copy and move operations for RAII safety
    1217:   critical_section(const critical_section &) = delete;
    1218:   critical_section(critical_section &&) = delete;
>>> 1219:   critical_section &operator=(const critical_section &) = delete;
    1220:   critical_section &operator=(critical_section &&) = delete;
    1221: };
    1222: 
```

**Violation 37**: freertos_stream_buffer.hpp:126:18
*Reason: Coding Style*

```cpp
     123:   }
     124: 
     125:   stream_buffer &operator=(const stream_buffer &) = delete;
>>>  126:   stream_buffer &operator=(stream_buffer &&src) = delete;
     127: 
     128:   /**
     129:    * @brief Send data to the stream buffer.
```

**Violation 38**: freertos_task.hpp:269:16
*Reason: Coding Style*

```cpp
     266:    *
     267:    * @return TaskHandle_t task handle
     268:    */
>>>  269:   TaskHandle_t handle(void) const { return m_hTask; }
     270: #if INCLUDE_vTaskSuspend
     271:   /**
     272:    * @brief Suspend the task.
```

**Violation 39**: freertos_task.hpp:423:15
*Reason: Coding Style*

```cpp
     420:    *
     421:    * @return const char* task name
     422:    */
>>>  423:   const char *name(void) const { return pcTaskGetName(m_hTask); }
     424: // Task notification API
     425: #if configUSE_TASK_NOTIFICATIONS
     426:   /**
```

**Violation 40**: freertos_queue.hpp:668:15
*Reason: Coding Style*

```cpp
     665:    *
     666:    * @return const char* The name of the queue.
     667:    */
>>>  668:   const char *name(void) const { return pcQueueGetName(m_queue); }
     669:   /**
     670:    * @brief Method checking if the queue is full from an ISR.
     671:    * @ref https://www.freertos.org/a00018.html#xQueueIsQueueFullFromISR
```

#### MISRA C 2012 Rule 8.7 (15 violation(s))

**Functions and objects should not be defined with external linkage if they are referenced from only one translation unit**

Functions and objects used only within a single file should be declared static to limit their scope and avoid namespace pollution.

*Rationale: Internal linkage prevents namespace pollution and allows for better optimization by the compiler.*

**Violation 1**: freertos_task.hpp:733:8
*Reason: Coding Style*

```cpp
     730:    *
     731:    * @return bool true if the task is running, false otherwise
     732:    */
>>>  733:   bool is_running(void) const {
     734:     switch (m_task.state()) {
     735:     case eRunning:
     736:     case eReady:
```

**Violation 2**: freertos_task.hpp:709:16
*Reason: Coding Style*

```cpp
     706:    *
     707:    * @return TaskHandle_t task handle
     708:    */
>>>  709:   TaskHandle_t handle(void) const { return m_task.handle(); }
     710: #if INCLUDE_vTaskSuspend
     711:   /**
     712:    * @brief Suspend the task.
```

**Violation 3**: freertos_task.hpp:748:8
*Reason: Coding Style*

```cpp
     745:    * @brief Terminate the task.
     746:    *
     747:    */
>>>  748:   void terminate(void) { m_task.terminate(); }
     749: #if INCLUDE_xTaskAbortDelay
     750:   /**
     751:    * @brief Abort the delay of the task.
```

**Violation 4**: freertos_task.cc:59:12
*Reason: Coding Style*

```cpp
      56: #if INCLUDE_xTaskGetIdleTaskHandle
      57: TaskHandle_t idle_task_handle(void) { return xTaskGetIdleTaskHandle(); }
      58: #endif
>>>   59: TickType_t tick_count(void) { return xTaskGetTickCount(); }
      60: TickType_t tick_count_isr(void) { return xTaskGetTickCountFromISR(); }
      61: std::chrono::milliseconds time_since_scheduler_started(void) {
      62:   return std::chrono::milliseconds{tick_count() * portTICK_PERIOD_MS};
```

**Violation 5**: freertos_task.cc:60:12
*Reason: Coding Style*

```cpp
      57: TaskHandle_t idle_task_handle(void) { return xTaskGetIdleTaskHandle(); }
      58: #endif
      59: TickType_t tick_count(void) { return xTaskGetTickCount(); }
>>>   60: TickType_t tick_count_isr(void) { return xTaskGetTickCountFromISR(); }
      61: std::chrono::milliseconds time_since_scheduler_started(void) {
      62:   return std::chrono::milliseconds{tick_count() * portTICK_PERIOD_MS};
      63: }
```

**Violation 6**: freertos_stream_buffer.hpp:264:14
*Reason: Coding Style*

```cpp
     261:    * insufficient memory available to copy the data into the stream buffer.
     262:    */
     263:   template <typename Iterator>
>>>  264:   BaseType_t send_isr(Iterator begin, Iterator end) {
     265:     BaseType_t higher_priority_task_woken = pdFALSE;
     266:     return send_isr(&*begin, std::distance(begin, end),
     267:                     higher_priority_task_woken);
```

**Violation 7**: freertos_stream_buffer.hpp:327:10
*Reason: Coding Style*

```cpp
     324:    * @param data_size Maximum number of bytes to copy into the buffer.
     325:    * @return size_t Number of bytes received.
     326:    */
>>>  327:   size_t receive_isr(void *data, size_t data_size) {
     328:     BaseType_t higher_priority_task_woken = pdFALSE;
     329:     return receive_isr(data, data_size, higher_priority_task_woken);
     330:   }
```

**Violation 8**: freertos_stream_buffer.hpp:264:14
*Reason: Coding Style*

```cpp
     261:    * insufficient memory available to copy the data into the stream buffer.
     262:    */
     263:   template <typename Iterator>
>>>  264:   BaseType_t send_isr(Iterator begin, Iterator end) {
     265:     BaseType_t higher_priority_task_woken = pdFALSE;
     266:     return send_isr(&*begin, std::distance(begin, end),
     267:                     higher_priority_task_woken);
```

**Violation 9**: freertos_stream_buffer.hpp:327:10
*Reason: Coding Style*

```cpp
     324:    * @param data_size Maximum number of bytes to copy into the buffer.
     325:    * @return size_t Number of bytes received.
     326:    */
>>>  327:   size_t receive_isr(void *data, size_t data_size) {
     328:     BaseType_t higher_priority_task_woken = pdFALSE;
     329:     return receive_isr(data, data_size, higher_priority_task_woken);
     330:   }
```

**Violation 10**: freertos_task.hpp:733:8
*Reason: Coding Style*

```cpp
     730:    *
     731:    * @return bool true if the task is running, false otherwise
     732:    */
>>>  733:   bool is_running(void) const {
     734:     switch (m_task.state()) {
     735:     case eRunning:
     736:     case eReady:
```

**Violation 11**: freertos_task.hpp:709:16
*Reason: Coding Style*

```cpp
     706:    *
     707:    * @return TaskHandle_t task handle
     708:    */
>>>  709:   TaskHandle_t handle(void) const { return m_task.handle(); }
     710: #if INCLUDE_vTaskSuspend
     711:   /**
     712:    * @brief Suspend the task.
```

**Violation 12**: freertos_task.hpp:748:8
*Reason: Coding Style*

```cpp
     745:    * @brief Terminate the task.
     746:    *
     747:    */
>>>  748:   void terminate(void) { m_task.terminate(); }
     749: #if INCLUDE_xTaskAbortDelay
     750:   /**
     751:    * @brief Abort the delay of the task.
```

**Violation 13**: freertos_task.hpp:733:8
*Reason: Coding Style*

```cpp
     730:    *
     731:    * @return bool true if the task is running, false otherwise
     732:    */
>>>  733:   bool is_running(void) const {
     734:     switch (m_task.state()) {
     735:     case eRunning:
     736:     case eReady:
```

**Violation 14**: freertos_task.hpp:709:16
*Reason: Coding Style*

```cpp
     706:    *
     707:    * @return TaskHandle_t task handle
     708:    */
>>>  709:   TaskHandle_t handle(void) const { return m_task.handle(); }
     710: #if INCLUDE_vTaskSuspend
     711:   /**
     712:    * @brief Suspend the task.
```

**Violation 15**: freertos_task.hpp:748:8
*Reason: Coding Style*

```cpp
     745:    * @brief Terminate the task.
     746:    *
     747:    */
>>>  748:   void terminate(void) { m_task.terminate(); }
     749: #if INCLUDE_xTaskAbortDelay
     750:   /**
     751:    * @brief Abort the delay of the task.
```

### Analysis Notes

- **Standard**: MISRA C 2012 (applicable rules for C++)
- **Tool**: cppcheck with MISRA addon
- **Scope**: Library modules only (src/, include/)
- **Rule texts**: Cannot be displayed due to MISRA licensing restrictions
- **Compatibility**: Many MISRA C 2012 rules overlap with MISRA C++ 2008 requirements
- **Integration**: This analysis complements the existing clang-tidy static analysis


## Enhanced cppcheck Analysis (All Rules)

### Summary

- **Total Violations**: 100
- **Unique Rules Violated**: 4
- **Files Analyzed**: 9
- **Analysis Errors**: 3

### Violations by Severity

- **Style**: 94
- **Warning**: 6

### Violations by Category

- **Unused Code**: 84 violation(s)
- **Other**: 10 violation(s)
- **Const Correctness**: 6 violation(s)

### Violations by Rule

- **constParameterReference** (Const Correctness): 6 violation(s)
- **shadowFunction** (Other): 4 violation(s)
- **uninitMemberVar** (Other): 6 violation(s)
- **unusedFunction** (Unused Code): 84 violation(s)

### Violations by File

- **freertos_event_group.hpp**: 11 violation(s)
- **freertos_message_buffer.hpp**: 9 violation(s)
- **freertos_queue.hpp**: 26 violation(s)
- **freertos_semaphore.hpp**: 8 violation(s)
- **freertos_stream_buffer.hpp**: 11 violation(s)
- **freertos_sw_timer.hpp**: 11 violation(s)
- **freertos_task.cc**: 6 violation(s)
- **freertos_task.hpp**: 18 violation(s)

### Detailed Violations with Code Context

#### Const Correctness (6 violation(s))

##### constParameterReference

**Violation 1**: freertos_task.hpp:919:37
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     916:    * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
     917:    */
     918:   BaseType_t notify_isr(const uint32_t val, eNotifyAction action,
>>>  919:                         BaseType_t &higherPriorityTaskWoken) {
     920:     return m_task.notify_isr(val, action, higherPriorityTaskWoken);
     921:   }
     922:   /**
```

**Violation 2**: freertos_task.hpp:944:47
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     941:    */
     942:   BaseType_t notify_and_query_isr(const uint32_t val, eNotifyAction action,
     943:                                   uint32_t &prev_value,
>>>  944:                                   BaseType_t &higherPriorityTaskWoken) {
     945:     return m_task.notify_and_query_isr(val, action, prev_value,
     946:                                        higherPriorityTaskWoken);
     947:   }
```

**Violation 3**: freertos_task.hpp:919:37
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     916:    * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
     917:    */
     918:   BaseType_t notify_isr(const uint32_t val, eNotifyAction action,
>>>  919:                         BaseType_t &higherPriorityTaskWoken) {
     920:     return m_task.notify_isr(val, action, higherPriorityTaskWoken);
     921:   }
     922:   /**
```

**Violation 4**: freertos_task.hpp:944:47
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     941:    */
     942:   BaseType_t notify_and_query_isr(const uint32_t val, eNotifyAction action,
     943:                                   uint32_t &prev_value,
>>>  944:                                   BaseType_t &higherPriorityTaskWoken) {
     945:     return m_task.notify_and_query_isr(val, action, prev_value,
     946:                                        higherPriorityTaskWoken);
     947:   }
```

**Violation 5**: freertos_task.hpp:919:37
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     916:    * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
     917:    */
     918:   BaseType_t notify_isr(const uint32_t val, eNotifyAction action,
>>>  919:                         BaseType_t &higherPriorityTaskWoken) {
     920:     return m_task.notify_isr(val, action, higherPriorityTaskWoken);
     921:   }
     922:   /**
```

**Violation 6**: freertos_task.hpp:944:47
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     941:    */
     942:   BaseType_t notify_and_query_isr(const uint32_t val, eNotifyAction action,
     943:                                   uint32_t &prev_value,
>>>  944:                                   BaseType_t &higherPriorityTaskWoken) {
     945:     return m_task.notify_and_query_isr(val, action, prev_value,
     946:                                        higherPriorityTaskWoken);
     947:   }
```

#### Other (10 violation(s))

##### shadowFunction

**Violation 1**: freertos_sw_timer.hpp:204:14
*Style*: Local variable 'name' shadows outer function

```cpp
     201:         while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
     202:           vTaskDelay(pdMS_TO_TICKS(1));
     203:         }
>>>  204:         auto name = pcTimerGetName(src.m_timer);
     205:         auto period = xTimerGetPeriod(src.m_timer);
     206:         auto auto_reload = uxTimerGetReloadMode(src.m_timer);
     207:         rc = xTimerDelete(src.m_timer, portMAX_DELAY);
```

**Violation 2**: freertos_sw_timer.hpp:205:14
*Style*: Local variable 'period' shadows outer function

```cpp
     202:           vTaskDelay(pdMS_TO_TICKS(1));
     203:         }
     204:         auto name = pcTimerGetName(src.m_timer);
>>>  205:         auto period = xTimerGetPeriod(src.m_timer);
     206:         auto auto_reload = uxTimerGetReloadMode(src.m_timer);
     207:         rc = xTimerDelete(src.m_timer, portMAX_DELAY);
     208:         if (rc == pdPASS) {
```

**Violation 3**: freertos_sw_timer.hpp:204:14
*Style*: Local variable 'name' shadows outer function

```cpp
     201:         while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
     202:           vTaskDelay(pdMS_TO_TICKS(1));
     203:         }
>>>  204:         auto name = pcTimerGetName(src.m_timer);
     205:         auto period = xTimerGetPeriod(src.m_timer);
     206:         auto auto_reload = uxTimerGetReloadMode(src.m_timer);
     207:         rc = xTimerDelete(src.m_timer, portMAX_DELAY);
```

**Violation 4**: freertos_sw_timer.hpp:205:14
*Style*: Local variable 'period' shadows outer function

```cpp
     202:           vTaskDelay(pdMS_TO_TICKS(1));
     203:         }
     204:         auto name = pcTimerGetName(src.m_timer);
>>>  205:         auto period = xTimerGetPeriod(src.m_timer);
     206:         auto auto_reload = uxTimerGetReloadMode(src.m_timer);
     207:         rc = xTimerDelete(src.m_timer, portMAX_DELAY);
     208:         if (rc == pdPASS) {
```

##### uninitMemberVar

**Violation 1**: freertos_queue.hpp:59:3
*Warning*: Member variable 'static_queue_allocator::m_storage' is not initialized in the constructor.

```cpp
      56:   std::array<uint8_t, QueueLength * sizeof(T)> m_storage;
      57: 
      58: public:
>>>   59:   static_queue_allocator() = default;
      60:   ~static_queue_allocator() = default;
      61:   static_queue_allocator(const static_queue_allocator &) = delete;
      62:   static_queue_allocator(static_queue_allocator &&) = delete;
```

**Violation 2**: freertos_stream_buffer.hpp:58:3
*Warning*: Member variable 'static_stream_buffer_allocator::m_storage' is not initialized in the constructor.

```cpp
      55:   std::array<uint8_t, StreamBufferSize> m_storage;
      56: 
      57: public:
>>>   58:   static_stream_buffer_allocator() = default;
      59:   ~static_stream_buffer_allocator() = default;
      60:   static_stream_buffer_allocator(const static_stream_buffer_allocator &) =
      61:       delete;
```

**Violation 3**: freertos_message_buffer.hpp:57:3
*Warning*: Member variable 'static_message_buffer_allocator::m_storage' is not initialized in the constructor.

```cpp
      54:   std::array<uint8_t, MessageBufferSize> m_storage;
      55: 
      56: public:
>>>   57:   static_message_buffer_allocator() = default;
      58:   ~static_message_buffer_allocator() = default;
      59:   static_message_buffer_allocator(const static_message_buffer_allocator &) =
      60:       delete;
```

**Violation 4**: freertos_queue.hpp:59:3
*Warning*: Member variable 'static_queue_allocator::m_storage' is not initialized in the constructor.

```cpp
      56:   std::array<uint8_t, QueueLength * sizeof(T)> m_storage;
      57: 
      58: public:
>>>   59:   static_queue_allocator() = default;
      60:   ~static_queue_allocator() = default;
      61:   static_queue_allocator(const static_queue_allocator &) = delete;
      62:   static_queue_allocator(static_queue_allocator &&) = delete;
```

**Violation 5**: freertos_stream_buffer.hpp:58:3
*Warning*: Member variable 'static_stream_buffer_allocator::m_storage' is not initialized in the constructor.

```cpp
      55:   std::array<uint8_t, StreamBufferSize> m_storage;
      56: 
      57: public:
>>>   58:   static_stream_buffer_allocator() = default;
      59:   ~static_stream_buffer_allocator() = default;
      60:   static_stream_buffer_allocator(const static_stream_buffer_allocator &) =
      61:       delete;
```

**Violation 6**: freertos_message_buffer.hpp:57:3
*Warning*: Member variable 'static_message_buffer_allocator::m_storage' is not initialized in the constructor.

```cpp
      54:   std::array<uint8_t, MessageBufferSize> m_storage;
      55: 
      56: public:
>>>   57:   static_message_buffer_allocator() = default;
      58:   ~static_message_buffer_allocator() = default;
      59:   static_message_buffer_allocator(const static_message_buffer_allocator &) =
      60:       delete;
```

#### Unused Code (84 violation(s))

##### unusedFunction

**Violation 1**: freertos_task.hpp:1046:0
*Style*: The function 'sleep_for' is never used.

```cpp
    1043:  * @param duration duration to sleep
    1044:  */
    1045: template <typename Rep, typename Period>
>>> 1046: void sleep_for(std::chrono::duration<Rep, Period> duration) {
    1047:   delay(duration);
    1048: }
    1049: 
```

**Violation 2**: freertos_task.hpp:1113:0
*Style*: The function 'total_run_time' is never used.

```cpp
    1110:    * @return UBaseType_t number of tasks
    1111:    */
    1112:   UBaseType_t count(void) const { return m_task_count; }
>>> 1113:   std::chrono::milliseconds total_run_time(void) const {
    1114:     return std::chrono::milliseconds{m_total_run_time};
    1115:   }
    1116:   /**
```

**Violation 3**: freertos_task.hpp:1121:0
*Style*: The function 'begin' is never used.

```cpp
    1118:    *
    1119:    * @return const TaskStatus_t* begin iterator
    1120:    */
>>> 1121:   const TaskStatus_t *begin(void) const { return m_status_array.data(); }
    1122:   /**
    1123:    * @brief Return the end iterator of the task status array.
    1124:    *
```

**Violation 4**: freertos_task.hpp:1127:0
*Style*: The function 'end' is never used.

```cpp
    1124:    *
    1125:    * @return const TaskStatus_t* end iterator
    1126:    */
>>> 1127:   const TaskStatus_t *end(void) const {
    1128:     return m_status_array.data() + m_task_count;
    1129:   }
    1130: };
```

**Violation 5**: freertos_task.cc:54:0
*Style*: The function 'current_task_handle' is never used.

```cpp
      51:   }
      52: }
      53: #if INCLUDE_xTaskGetCurrentTaskHandle
>>>   54: TaskHandle_t current_task_handle(void) { return xTaskGetCurrentTaskHandle(); }
      55: #endif
      56: #if INCLUDE_xTaskGetIdleTaskHandle
      57: TaskHandle_t idle_task_handle(void) { return xTaskGetIdleTaskHandle(); }
```

**Violation 6**: freertos_task.cc:57:0
*Style*: The function 'idle_task_handle' is never used.

```cpp
      54: TaskHandle_t current_task_handle(void) { return xTaskGetCurrentTaskHandle(); }
      55: #endif
      56: #if INCLUDE_xTaskGetIdleTaskHandle
>>>   57: TaskHandle_t idle_task_handle(void) { return xTaskGetIdleTaskHandle(); }
      58: #endif
      59: TickType_t tick_count(void) { return xTaskGetTickCount(); }
      60: TickType_t tick_count_isr(void) { return xTaskGetTickCountFromISR(); }
```

**Violation 7**: freertos_task.cc:61:0
*Style*: The function 'time_since_scheduler_started' is never used.

```cpp
      58: #endif
      59: TickType_t tick_count(void) { return xTaskGetTickCount(); }
      60: TickType_t tick_count_isr(void) { return xTaskGetTickCountFromISR(); }
>>>   61: std::chrono::milliseconds time_since_scheduler_started(void) {
      62:   return std::chrono::milliseconds{tick_count() * portTICK_PERIOD_MS};
      63: }
      64: std::chrono::milliseconds time_since_scheduler_started_isr(void) {
```

**Violation 8**: freertos_task.cc:64:0
*Style*: The function 'time_since_scheduler_started_isr' is never used.

```cpp
      61: std::chrono::milliseconds time_since_scheduler_started(void) {
      62:   return std::chrono::milliseconds{tick_count() * portTICK_PERIOD_MS};
      63: }
>>>   64: std::chrono::milliseconds time_since_scheduler_started_isr(void) {
      65:   return std::chrono::milliseconds{tick_count_isr() * portTICK_PERIOD_MS};
      66: }
      67: #if INCLUDE_xTaskGetSchedulerState || configUSE_TIMERS
```

**Violation 9**: freertos_task.cc:70:0
*Style*: The function 'task_count' is never used.

```cpp
      67: #if INCLUDE_xTaskGetSchedulerState || configUSE_TIMERS
      68: BaseType_t get_scheduler_state(void) { return xTaskGetSchedulerState(); }
      69: #endif
>>>   70: UBaseType_t task_count(void) { return uxTaskGetNumberOfTasks(); }
      71: void yield(void) { taskYIELD(); }
      72: 
      73: } // namespace freertos
```

**Violation 10**: freertos_task.cc:71:0
*Style*: The function 'yield' is never used.

```cpp
      68: BaseType_t get_scheduler_state(void) { return xTaskGetSchedulerState(); }
      69: #endif
      70: UBaseType_t task_count(void) { return uxTaskGetNumberOfTasks(); }
>>>   71: void yield(void) { taskYIELD(); }
      72: 
      73: } // namespace freertos
```

**Violation 11**: freertos_queue.hpp:261:0
*Style*: The function 'send_isr' is never used.

```cpp
     258:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     259:    * errQUEUE_FULL.
     260:    */
>>>  261:   BaseType_t send_isr(const T &item, BaseType_t &higher_priority_task_woken) {
     262:     return xQueueSendFromISR(m_queue, &item, &higher_priority_task_woken);
     263:   }
     264:   /**
```

**Violation 12**: freertos_queue.hpp:316:0
*Style*: The function 'send_back_isr' is never used.

```cpp
     313:    * @return BaseType_t pdPASS if the item
     314:    * was successfully posted, otherwise errQUEUE_FULL.
     315:    */
>>>  316:   BaseType_t send_back_isr(const T &item,
     317:                            BaseType_t &higher_priority_task_woken) {
     318:     return xQueueSendToBackFromISR(m_queue, &item, &higher_priority_task_woken);
     319:   }
```

**Violation 13**: freertos_queue.hpp:372:0
*Style*: The function 'send_front_isr' is never used.

```cpp
     369:    * @return BaseType_t pdPASS if the item
     370:    * was successfully posted, otherwise errQUEUE_FULL.
     371:    */
>>>  372:   BaseType_t send_front_isr(const T &item,
     373:                             BaseType_t &higher_priority_task_woken) {
     374:     return xQueueSendToFrontFromISR(m_queue, &item,
     375:                                     &higher_priority_task_woken);
```

**Violation 14**: freertos_queue.hpp:459:0
*Style*: The function 'receive_isr' is never used.

```cpp
     456:    * @return BaseType_t pdPASS if the item was successfully received, otherwise
     457:    * pdFALSE.
     458:    */
>>>  459:   BaseType_t receive_isr(T &item, BaseType_t &higher_priority_task_woken) {
     460:     return xQueueReceiveFromISR(m_queue, &item, &higher_priority_task_woken);
     461:   }
     462:   /**
```

**Violation 15**: freertos_queue.hpp:497:0
*Style*: The function 'messages_waiting' is never used.

```cpp
     494:    *
     495:    * @return UBaseType_t The number of items stored in the queue.
     496:    */
>>>  497:   UBaseType_t messages_waiting(void) { return uxQueueMessagesWaiting(m_queue); }
     498:   /**
     499:    * @brief Return the number of items stored in the queue from an ISR.
     500:    * @ref https://www.freertos.org/a00018.html#uxQueueMessagesWaitingFromISR
```

**Violation 16**: freertos_queue.hpp:504:0
*Style*: The function 'messages_waiting_isr' is never used.

```cpp
     501:    *
     502:    * @return UBaseType_t The number of items stored in the queue.
     503:    */
>>>  504:   UBaseType_t messages_waiting_isr(void) {
     505:     return uxQueueMessagesWaitingFromISR(m_queue);
     506:   }
     507:   /**
```

**Violation 17**: freertos_queue.hpp:513:0
*Style*: The function 'spaces_available' is never used.

```cpp
     510:    *
     511:    * @return UBaseType_t The number of spaces available in the queue.
     512:    */
>>>  513:   UBaseType_t spaces_available(void) { return uxQueueSpacesAvailable(m_queue); }
     514:   /**
     515:    * @brief Resets the queue to the empty state.
     516:    * @ref https://www.freertos.org/a00018.html#xQueueReset
```

**Violation 18**: freertos_queue.hpp:520:0
*Style*: The function 'reset' is never used.

```cpp
     517:    *
     518:    * @return BaseType_t pdPASS if the queue was reset, pdFAIL otherwise.
     519:    */
>>>  520:   BaseType_t reset(void) { return xQueueReset(m_queue); }
     521:   /**
     522:    * @brief A version of send_back method that overwrites the items in the queue
     523:    * if it is full.
```

**Violation 19**: freertos_queue.hpp:529:0
*Style*: The function 'overwrite' is never used.

```cpp
     526:    * @param item An item to be posted on the queue.
     527:    * @return BaseType_t pdPASS returned always.
     528:    */
>>>  529:   BaseType_t overwrite(const T &item) {
     530:     return xQueueOverwrite(m_queue, &item);
     531:   }
     532:   /**
```

**Violation 20**: freertos_queue.hpp:542:0
*Style*: The function 'overwrite_isr' is never used.

```cpp
     539:    * unblocked a task of higher priority.
     540:    * @return BaseType_t pdPASS returned always.
     541:    */
>>>  542:   BaseType_t overwrite_isr(const T &item,
     543:                            BaseType_t &higher_priority_task_woken) {
     544:     return xQueueOverwriteFromISR(m_queue, &item, &higher_priority_task_woken);
     545:   }
```

**Violation 21**: freertos_queue.hpp:598:0
*Style*: The function 'peek_isr' is never used.

```cpp
     595:    * @return BaseType_t pdPASS if the item
     596:    * was successfully peeked, otherwise pdFALSE.
     597:    */
>>>  598:   BaseType_t peek_isr(T &item, BaseType_t &higher_priority_task_woken) {
     599:     return xQueuePeekFromISR(m_queue, &item, &higher_priority_task_woken);
     600:   }
     601:   /**
```

**Violation 22**: freertos_queue.hpp:668:0
*Style*: The function 'name' is never used.

```cpp
     665:    *
     666:    * @return const char* The name of the queue.
     667:    */
>>>  668:   const char *name(void) const { return pcQueueGetName(m_queue); }
     669:   /**
     670:    * @brief Method checking if the queue is full from an ISR.
     671:    * @ref https://www.freertos.org/a00018.html#xQueueIsQueueFullFromISR
```

**Violation 23**: freertos_queue.hpp:675:0
*Style*: The function 'full_isr' is never used.

```cpp
     672:    *
     673:    * @return BaseType_t pdTRUE if the queue is full, pdFALSE otherwise.
     674:    */
>>>  675:   BaseType_t full_isr(void) const { return xQueueIsQueueFullFromISR(m_queue); }
     676:   /**
     677:    * @brief Method checking if the queue is empty from an ISR.
     678:    * @ref https://www.freertos.org/a00018.html#xQueueIsQueueEmptyFromISR
```

**Violation 24**: freertos_queue.hpp:682:0
*Style*: The function 'empty_isr' is never used.

```cpp
     679:    *
     680:    * @return BaseType_t pdTRUE if the queue is empty, pdFALSE otherwise.
     681:    */
>>>  682:   BaseType_t empty_isr(void) const {
     683:     return xQueueIsQueueEmptyFromISR(m_queue);
     684:   }
     685: };
```

**Violation 25**: freertos_semaphore.hpp:211:0
*Style*: The function 'give_isr' is never used.

```cpp
     208:    * @return BaseType_t pdTRUE if the semaphore was successfully given,
     209:    *
     210:    */
>>>  211:   BaseType_t give_isr(BaseType_t &high_priority_task_woken) {
     212:     return xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     213:   }
     214:   /**
```

**Violation 26**: freertos_semaphore.hpp:248:0
*Style*: The function 'take_isr' is never used.

```cpp
     245:    * otherwise pdFALSE.
     246:    *
     247:    */
>>>  248:   BaseType_t take_isr(BaseType_t &high_priority_task_woken) {
     249:     return xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
     250:   }
     251:   /**
```

**Violation 27**: freertos_semaphore.hpp:768:0
*Style*: The function 'recursions_count' is never used.

```cpp
     765:    *
     766:    * @return uint8_t number of recursions of the recursive mutex.
     767:    */
>>>  768:   uint8_t recursions_count(void) const { return m_recursions_count; }
     769: };
     770: 
     771: /**
```

**Violation 28**: freertos_semaphore.hpp:891:0
*Style*: The function 'high_priority_task_woken' is never used.

```cpp
     888:    * @return BaseType_t pdTRUE if the high priority task was woken, otherwise
     889:    * pdFALSE.
     890:    */
>>>  891:   BaseType_t high_priority_task_woken(void) const {
     892:     return m_high_priority_task_woken;
     893:   }
     894:   /**
```

**Violation 29**: freertos_sw_timer.hpp:537:0
*Style*: The function 'reload_mode' is never used.

```cpp
     534:    * @param auto_reload pdTRUE to enable auto-reload mode, pdFALSE to disable
     535:    * @return timer& reference to the timer object
     536:    */
>>>  537:   timer &reload_mode(UBaseType_t auto_reload) {
     538:     if (m_timer) {
     539:       vTimerSetReloadMode(m_timer, auto_reload);
     540:     }
```

**Violation 30**: freertos_sw_timer.hpp:569:0
*Style*: The function 'remaining_time' is never used.

```cpp
     566:    * @return std::chrono::milliseconds remaining time before the timer expires
     567:    * in milliseconds.
     568:    */
>>>  569:   std::chrono::milliseconds remaining_time(void) const {
     570:     return std::chrono::milliseconds{remaining_ticks()};
     571:   }
     572:   /**
```

**Violation 31**: freertos_sw_timer.hpp:578:0
*Style*: The function 'running' is never used.

```cpp
     575:    *
     576:    * @return BaseType_t pdTRUE if the timer is running, pdFALSE otherwise
     577:    */
>>>  578:   BaseType_t running(void) const {
     579:     if (!m_timer) {
     580:       return pdFALSE;
     581:     }
```

**Violation 32**: freertos_sw_timer.hpp:589:0
*Style*: The function 'name' is never used.

```cpp
     586:    *
     587:    * @return const char* name of the timer
     588:    */
>>>  589:   const char *name(void) const {
     590:     if (!m_timer) {
     591:       return nullptr;
     592:     }
```

**Violation 33**: freertos_stream_buffer.hpp:337:0
*Style*: The function 'available' is never used.

```cpp
     334:    *
     335:    * @return size_t Number of bytes available in the stream buffer.
     336:    */
>>>  337:   size_t available(void) {
     338:     return xStreamBufferBytesAvailable(m_stream_buffer);
     339:   }
     340:   /**
```

**Violation 34**: freertos_stream_buffer.hpp:346:0
*Style*: The function 'free' is never used.

```cpp
     343:    *
     344:    * @return size_t Number of bytes free in the stream buffer.
     345:    */
>>>  346:   size_t free(void) { return xStreamBufferSpacesAvailable(m_stream_buffer); }
     347:   /**
     348:    * @brief Reset the stream buffer to the cleared state.
     349:    * @ref https://www.freertos.org/xStreamBufferReset.html
```

**Violation 35**: freertos_stream_buffer.hpp:353:0
*Style*: The function 'reset' is never used.

```cpp
     350:    *
     351:    * @return BaseType_t pdPass if the stream buffer was reset, pdFAIL otherwise.
     352:    */
>>>  353:   BaseType_t reset(void) { return xStreamBufferReset(m_stream_buffer); }
     354:   /**
     355:    * @brief Set the trigger level of the stream buffer.
     356:    * @ref https://www.freertos.org/xStreamBufferSetTriggerLevel.html
```

**Violation 36**: freertos_stream_buffer.hpp:362:0
*Style*: The function 'set_trigger_level' is never used.

```cpp
     359:    * buffer before a task that is blocked on a read operation will be unblocked.
     360:    * @return BaseType_t pdPass if the trigger level was set, pdFAIL otherwise.
     361:    */
>>>  362:   BaseType_t set_trigger_level(size_t trigger_level_bytes) {
     363:     return xStreamBufferSetTriggerLevel(m_stream_buffer, trigger_level_bytes);
     364:   }
     365:   /**
```

**Violation 37**: freertos_stream_buffer.hpp:370:0
*Style*: The function 'handle' is never used.

```cpp
     367:    *
     368:    * @return StreamBufferHandle_t Handle of the stream buffer.
     369:    */
>>>  370:   StreamBufferHandle_t handle(void) const { return m_stream_buffer; }
     371:   /**
     372:    * @brief Check if the stream buffer is empty.
     373:    * @ref https://www.freertos.org/xStreamBufferIsEmpty.html
```

**Violation 38**: freertos_stream_buffer.hpp:377:0
*Style*: The function 'empty' is never used.

```cpp
     374:    *
     375:    * @return BaseType_t pdTRUE if the stream buffer is empty, pdFALSE otherwise.
     376:    */
>>>  377:   BaseType_t empty(void) { return xStreamBufferIsEmpty(m_stream_buffer); }
     378:   /**
     379:    * @brief Check if the stream buffer is full.
     380:    * @ref https://www.freertos.org/xStreamBufferIsFull.html
```

**Violation 39**: freertos_stream_buffer.hpp:384:0
*Style*: The function 'full' is never used.

```cpp
     381:    *
     382:    * @return BaseType_t pdTRUE if the stream buffer is full, pdFALSE otherwise.
     383:    */
>>>  384:   BaseType_t full(void) { return xStreamBufferIsFull(m_stream_buffer); }
     385: };
     386: 
     387: #if configSUPPORT_STATIC_ALLOCATION
```

**Violation 40**: freertos_event_group.hpp:126:0
*Style*: The function 'set_bits' is never used.

```cpp
     123:    * @param bits_to_set bits to set
     124:    * @return EventBits_t bits set
     125:    */
>>>  126:   EventBits_t set_bits(const EventBits_t bits_to_set) {
     127:     return xEventGroupSetBits(m_event_group, bits_to_set);
     128:   }
     129:   /**
```

**Violation 41**: freertos_event_group.hpp:136:0
*Style*: The function 'set_bits_isr' is never used.

```cpp
     133:    * @param bits_to_set bits to set
     134:    * @return EventBits_t bits set
     135:    */
>>>  136:   EventBits_t set_bits_isr(const EventBits_t bits_to_set) {
     137:     BaseType_t higher_priority_task_woken = pdFALSE;
     138:     const EventBits_t bits_set = xEventGroupSetBitsFromISR(
     139:         m_event_group, bits_to_set, &higher_priority_task_woken);
```

**Violation 42**: freertos_event_group.hpp:150:0
*Style*: The function 'clear_bits' is never used.

```cpp
     147:    * @param bits_to_clear bits to clear
     148:    * @return EventBits_t bits cleared
     149:    */
>>>  150:   EventBits_t clear_bits(const EventBits_t bits_to_clear) {
     151:     return xEventGroupClearBits(m_event_group, bits_to_clear);
     152:   }
     153:   /**
```

**Violation 43**: freertos_event_group.hpp:198:0
*Style*: The function 'get_bits' is never used.

```cpp
     195:    *
     196:    * @return EventBits_t Current value of the event group bits
     197:    */
>>>  198:   EventBits_t get_bits(void) const { return xEventGroupGetBits(m_event_group); }
     199:   /**
     200:    * @brief Method to get the bits of the event group from an ISR.
     201:    * @ref https://www.freertos.org/xEventGroupGetBitsFromISR.html
```

**Violation 44**: freertos_event_group.hpp:205:0
*Style*: The function 'get_bits_isr' is never used.

```cpp
     202:    *
     203:    * @return EventBits_t Current value of the event group bits
     204:    */
>>>  205:   EventBits_t get_bits_isr(void) const {
     206:     return xEventGroupGetBitsFromISR(m_event_group);
     207:   }
     208:   /**
```

**Violation 45**: freertos_message_buffer.hpp:193:0
*Style*: The function 'available' is never used.

```cpp
     190:    *
     191:    * @return size_t the number of bytes available in the buffer
     192:    */
>>>  193:   size_t available(void) const {
     194:     return xMessageBufferSpaceAvailable(m_message_buffer);
     195:   }
     196:   /**
```

**Violation 46**: freertos_message_buffer.hpp:209:0
*Style*: The function 'empty' is never used.

```cpp
     206:    *
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
>>>  209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
     210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
```

**Violation 47**: freertos_message_buffer.hpp:216:0
*Style*: The function 'full' is never used.

```cpp
     213:    *
     214:    * @return BaseType_t pdTRUE if the message buffer is full, pdFALSE otherwise
     215:    */
>>>  216:   BaseType_t full(void) { return xMessageBufferIsFull(m_message_buffer); }
     217: };
     218: 
     219: #if configSUPPORT_STATIC_ALLOCATION
```

**Violation 48**: freertos_queue.hpp:316:0
*Style*: The function 'send_back_isr' is never used.

```cpp
     313:    * @return BaseType_t pdPASS if the item
     314:    * was successfully posted, otherwise errQUEUE_FULL.
     315:    */
>>>  316:   BaseType_t send_back_isr(const T &item,
     317:                            BaseType_t &higher_priority_task_woken) {
     318:     return xQueueSendToBackFromISR(m_queue, &item, &higher_priority_task_woken);
     319:   }
```

**Violation 49**: freertos_queue.hpp:372:0
*Style*: The function 'send_front_isr' is never used.

```cpp
     369:    * @return BaseType_t pdPASS if the item
     370:    * was successfully posted, otherwise errQUEUE_FULL.
     371:    */
>>>  372:   BaseType_t send_front_isr(const T &item,
     373:                             BaseType_t &higher_priority_task_woken) {
     374:     return xQueueSendToFrontFromISR(m_queue, &item,
     375:                                     &higher_priority_task_woken);
```

**Violation 50**: freertos_queue.hpp:497:0
*Style*: The function 'messages_waiting' is never used.

```cpp
     494:    *
     495:    * @return UBaseType_t The number of items stored in the queue.
     496:    */
>>>  497:   UBaseType_t messages_waiting(void) { return uxQueueMessagesWaiting(m_queue); }
     498:   /**
     499:    * @brief Return the number of items stored in the queue from an ISR.
     500:    * @ref https://www.freertos.org/a00018.html#uxQueueMessagesWaitingFromISR
```

**Violation 51**: freertos_queue.hpp:504:0
*Style*: The function 'messages_waiting_isr' is never used.

```cpp
     501:    *
     502:    * @return UBaseType_t The number of items stored in the queue.
     503:    */
>>>  504:   UBaseType_t messages_waiting_isr(void) {
     505:     return uxQueueMessagesWaitingFromISR(m_queue);
     506:   }
     507:   /**
```

**Violation 52**: freertos_queue.hpp:513:0
*Style*: The function 'spaces_available' is never used.

```cpp
     510:    *
     511:    * @return UBaseType_t The number of spaces available in the queue.
     512:    */
>>>  513:   UBaseType_t spaces_available(void) { return uxQueueSpacesAvailable(m_queue); }
     514:   /**
     515:    * @brief Resets the queue to the empty state.
     516:    * @ref https://www.freertos.org/a00018.html#xQueueReset
```

**Violation 53**: freertos_queue.hpp:529:0
*Style*: The function 'overwrite' is never used.

```cpp
     526:    * @param item An item to be posted on the queue.
     527:    * @return BaseType_t pdPASS returned always.
     528:    */
>>>  529:   BaseType_t overwrite(const T &item) {
     530:     return xQueueOverwrite(m_queue, &item);
     531:   }
     532:   /**
```

**Violation 54**: freertos_queue.hpp:542:0
*Style*: The function 'overwrite_isr' is never used.

```cpp
     539:    * unblocked a task of higher priority.
     540:    * @return BaseType_t pdPASS returned always.
     541:    */
>>>  542:   BaseType_t overwrite_isr(const T &item,
     543:                            BaseType_t &higher_priority_task_woken) {
     544:     return xQueueOverwriteFromISR(m_queue, &item, &higher_priority_task_woken);
     545:   }
```

**Violation 55**: freertos_queue.hpp:598:0
*Style*: The function 'peek_isr' is never used.

```cpp
     595:    * @return BaseType_t pdPASS if the item
     596:    * was successfully peeked, otherwise pdFALSE.
     597:    */
>>>  598:   BaseType_t peek_isr(T &item, BaseType_t &higher_priority_task_woken) {
     599:     return xQueuePeekFromISR(m_queue, &item, &higher_priority_task_woken);
     600:   }
     601:   /**
```

**Violation 56**: freertos_queue.hpp:675:0
*Style*: The function 'full_isr' is never used.

```cpp
     672:    *
     673:    * @return BaseType_t pdTRUE if the queue is full, pdFALSE otherwise.
     674:    */
>>>  675:   BaseType_t full_isr(void) const { return xQueueIsQueueFullFromISR(m_queue); }
     676:   /**
     677:    * @brief Method checking if the queue is empty from an ISR.
     678:    * @ref https://www.freertos.org/a00018.html#xQueueIsQueueEmptyFromISR
```

**Violation 57**: freertos_queue.hpp:682:0
*Style*: The function 'empty_isr' is never used.

```cpp
     679:    *
     680:    * @return BaseType_t pdTRUE if the queue is empty, pdFALSE otherwise.
     681:    */
>>>  682:   BaseType_t empty_isr(void) const {
     683:     return xQueueIsQueueEmptyFromISR(m_queue);
     684:   }
     685: };
```

**Violation 58**: freertos_semaphore.hpp:211:0
*Style*: The function 'give_isr' is never used.

```cpp
     208:    * @return BaseType_t pdTRUE if the semaphore was successfully given,
     209:    *
     210:    */
>>>  211:   BaseType_t give_isr(BaseType_t &high_priority_task_woken) {
     212:     return xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     213:   }
     214:   /**
```

**Violation 59**: freertos_semaphore.hpp:248:0
*Style*: The function 'take_isr' is never used.

```cpp
     245:    * otherwise pdFALSE.
     246:    *
     247:    */
>>>  248:   BaseType_t take_isr(BaseType_t &high_priority_task_woken) {
     249:     return xSemaphoreTakeFromISR(m_semaphore, &high_priority_task_woken);
     250:   }
     251:   /**
```

**Violation 60**: freertos_semaphore.hpp:768:0
*Style*: The function 'recursions_count' is never used.

```cpp
     765:    *
     766:    * @return uint8_t number of recursions of the recursive mutex.
     767:    */
>>>  768:   uint8_t recursions_count(void) const { return m_recursions_count; }
     769: };
     770: 
     771: /**
```

**Violation 61**: freertos_semaphore.hpp:891:0
*Style*: The function 'high_priority_task_woken' is never used.

```cpp
     888:    * @return BaseType_t pdTRUE if the high priority task was woken, otherwise
     889:    * pdFALSE.
     890:    */
>>>  891:   BaseType_t high_priority_task_woken(void) const {
     892:     return m_high_priority_task_woken;
     893:   }
     894:   /**
```

**Violation 62**: freertos_stream_buffer.hpp:346:0
*Style*: The function 'free' is never used.

```cpp
     343:    *
     344:    * @return size_t Number of bytes free in the stream buffer.
     345:    */
>>>  346:   size_t free(void) { return xStreamBufferSpacesAvailable(m_stream_buffer); }
     347:   /**
     348:    * @brief Reset the stream buffer to the cleared state.
     349:    * @ref https://www.freertos.org/xStreamBufferReset.html
```

**Violation 63**: freertos_stream_buffer.hpp:362:0
*Style*: The function 'set_trigger_level' is never used.

```cpp
     359:    * buffer before a task that is blocked on a read operation will be unblocked.
     360:    * @return BaseType_t pdPass if the trigger level was set, pdFAIL otherwise.
     361:    */
>>>  362:   BaseType_t set_trigger_level(size_t trigger_level_bytes) {
     363:     return xStreamBufferSetTriggerLevel(m_stream_buffer, trigger_level_bytes);
     364:   }
     365:   /**
```

**Violation 64**: freertos_sw_timer.hpp:537:0
*Style*: The function 'reload_mode' is never used.

```cpp
     534:    * @param auto_reload pdTRUE to enable auto-reload mode, pdFALSE to disable
     535:    * @return timer& reference to the timer object
     536:    */
>>>  537:   timer &reload_mode(UBaseType_t auto_reload) {
     538:     if (m_timer) {
     539:       vTimerSetReloadMode(m_timer, auto_reload);
     540:     }
```

**Violation 65**: freertos_sw_timer.hpp:569:0
*Style*: The function 'remaining_time' is never used.

```cpp
     566:    * @return std::chrono::milliseconds remaining time before the timer expires
     567:    * in milliseconds.
     568:    */
>>>  569:   std::chrono::milliseconds remaining_time(void) const {
     570:     return std::chrono::milliseconds{remaining_ticks()};
     571:   }
     572:   /**
```

**Violation 66**: freertos_sw_timer.hpp:578:0
*Style*: The function 'running' is never used.

```cpp
     575:    *
     576:    * @return BaseType_t pdTRUE if the timer is running, pdFALSE otherwise
     577:    */
>>>  578:   BaseType_t running(void) const {
     579:     if (!m_timer) {
     580:       return pdFALSE;
     581:     }
```

**Violation 67**: freertos_task.hpp:1046:0
*Style*: The function 'sleep_for' is never used.

```cpp
    1043:  * @param duration duration to sleep
    1044:  */
    1045: template <typename Rep, typename Period>
>>> 1046: void sleep_for(std::chrono::duration<Rep, Period> duration) {
    1047:   delay(duration);
    1048: }
    1049: 
```

**Violation 68**: freertos_task.hpp:1113:0
*Style*: The function 'total_run_time' is never used.

```cpp
    1110:    * @return UBaseType_t number of tasks
    1111:    */
    1112:   UBaseType_t count(void) const { return m_task_count; }
>>> 1113:   std::chrono::milliseconds total_run_time(void) const {
    1114:     return std::chrono::milliseconds{m_total_run_time};
    1115:   }
    1116:   /**
```

**Violation 69**: freertos_task.hpp:1121:0
*Style*: The function 'begin' is never used.

```cpp
    1118:    *
    1119:    * @return const TaskStatus_t* begin iterator
    1120:    */
>>> 1121:   const TaskStatus_t *begin(void) const { return m_status_array.data(); }
    1122:   /**
    1123:    * @brief Return the end iterator of the task status array.
    1124:    *
```

**Violation 70**: freertos_task.hpp:1127:0
*Style*: The function 'end' is never used.

```cpp
    1124:    *
    1125:    * @return const TaskStatus_t* end iterator
    1126:    */
>>> 1127:   const TaskStatus_t *end(void) const {
    1128:     return m_status_array.data() + m_task_count;
    1129:   }
    1130: };
```

**Violation 71**: freertos_event_group.hpp:117:0
*Style*: The function 'handle' is never used.

```cpp
     114:    *
     115:    * @return EventGroupHandle_t event group handle
     116:    */
>>>  117:   EventGroupHandle_t handle(void) const { return m_event_group; }
     118: 
     119:   /**
     120:    * @brief Method to set bits in the event group.
```

**Violation 72**: freertos_event_group.hpp:126:0
*Style*: The function 'set_bits' is never used.

```cpp
     123:    * @param bits_to_set bits to set
     124:    * @return EventBits_t bits set
     125:    */
>>>  126:   EventBits_t set_bits(const EventBits_t bits_to_set) {
     127:     return xEventGroupSetBits(m_event_group, bits_to_set);
     128:   }
     129:   /**
```

**Violation 73**: freertos_event_group.hpp:136:0
*Style*: The function 'set_bits_isr' is never used.

```cpp
     133:    * @param bits_to_set bits to set
     134:    * @return EventBits_t bits set
     135:    */
>>>  136:   EventBits_t set_bits_isr(const EventBits_t bits_to_set) {
     137:     BaseType_t higher_priority_task_woken = pdFALSE;
     138:     const EventBits_t bits_set = xEventGroupSetBitsFromISR(
     139:         m_event_group, bits_to_set, &higher_priority_task_woken);
```

**Violation 74**: freertos_event_group.hpp:150:0
*Style*: The function 'clear_bits' is never used.

```cpp
     147:    * @param bits_to_clear bits to clear
     148:    * @return EventBits_t bits cleared
     149:    */
>>>  150:   EventBits_t clear_bits(const EventBits_t bits_to_clear) {
     151:     return xEventGroupClearBits(m_event_group, bits_to_clear);
     152:   }
     153:   /**
```

**Violation 75**: freertos_event_group.hpp:198:0
*Style*: The function 'get_bits' is never used.

```cpp
     195:    *
     196:    * @return EventBits_t Current value of the event group bits
     197:    */
>>>  198:   EventBits_t get_bits(void) const { return xEventGroupGetBits(m_event_group); }
     199:   /**
     200:    * @brief Method to get the bits of the event group from an ISR.
     201:    * @ref https://www.freertos.org/xEventGroupGetBitsFromISR.html
```

**Violation 76**: freertos_event_group.hpp:205:0
*Style*: The function 'get_bits_isr' is never used.

```cpp
     202:    *
     203:    * @return EventBits_t Current value of the event group bits
     204:    */
>>>  205:   EventBits_t get_bits_isr(void) const {
     206:     return xEventGroupGetBitsFromISR(m_event_group);
     207:   }
     208:   /**
```

**Violation 77**: freertos_message_buffer.hpp:193:0
*Style*: The function 'available' is never used.

```cpp
     190:    *
     191:    * @return size_t the number of bytes available in the buffer
     192:    */
>>>  193:   size_t available(void) const {
     194:     return xMessageBufferSpaceAvailable(m_message_buffer);
     195:   }
     196:   /**
```

**Violation 78**: freertos_message_buffer.hpp:202:0
*Style*: The function 'reset' is never used.

```cpp
     199:    *
     200:    * @return BaseType_t pdPass if the message buffer was reset, pdFAIL otherwise
     201:    */
>>>  202:   BaseType_t reset(void) { return xMessageBufferReset(m_message_buffer); }
     203:   /**
     204:    * @brief Method checking if the message buffer is empty.
     205:    * @ref https://www.freertos.org/xMessageBufferIsEmpty.html
```

**Violation 79**: freertos_message_buffer.hpp:209:0
*Style*: The function 'empty' is never used.

```cpp
     206:    *
     207:    * @return BaseType_t pdTRUE if the message buffer is empty, pdFALSE otherwise
     208:    */
>>>  209:   BaseType_t empty(void) { return xMessageBufferIsEmpty(m_message_buffer); }
     210:   /**
     211:    * @brief Method checking if the message buffer is full.
     212:    * @ref https://www.freertos.org/xMessageBufferIsFull.html
```

**Violation 80**: freertos_message_buffer.hpp:216:0
*Style*: The function 'full' is never used.

```cpp
     213:    *
     214:    * @return BaseType_t pdTRUE if the message buffer is full, pdFALSE otherwise
     215:    */
>>>  216:   BaseType_t full(void) { return xMessageBufferIsFull(m_message_buffer); }
     217: };
     218: 
     219: #if configSUPPORT_STATIC_ALLOCATION
```

**Violation 81**: freertos_task.hpp:1046:0
*Style*: The function 'sleep_for' is never used.

```cpp
    1043:  * @param duration duration to sleep
    1044:  */
    1045: template <typename Rep, typename Period>
>>> 1046: void sleep_for(std::chrono::duration<Rep, Period> duration) {
    1047:   delay(duration);
    1048: }
    1049: 
```

**Violation 82**: freertos_task.hpp:1113:0
*Style*: The function 'total_run_time' is never used.

```cpp
    1110:    * @return UBaseType_t number of tasks
    1111:    */
    1112:   UBaseType_t count(void) const { return m_task_count; }
>>> 1113:   std::chrono::milliseconds total_run_time(void) const {
    1114:     return std::chrono::milliseconds{m_total_run_time};
    1115:   }
    1116:   /**
```

**Violation 83**: freertos_task.hpp:1121:0
*Style*: The function 'begin' is never used.

```cpp
    1118:    *
    1119:    * @return const TaskStatus_t* begin iterator
    1120:    */
>>> 1121:   const TaskStatus_t *begin(void) const { return m_status_array.data(); }
    1122:   /**
    1123:    * @brief Return the end iterator of the task status array.
    1124:    *
```

**Violation 84**: freertos_task.hpp:1127:0
*Style*: The function 'end' is never used.

```cpp
    1124:    *
    1125:    * @return const TaskStatus_t* end iterator
    1126:    */
>>> 1127:   const TaskStatus_t *end(void) const {
    1128:     return m_status_array.data() + m_task_count;
    1129:   }
    1130: };
```

### Analysis Errors

Some files could not be fully analyzed:

```
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc:0:0: error: Bailing out from analysis: Checking file failed: Failed to execute addon 'misra' - exitcode is 1 [internalError]
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos.hpp:0:0: error: Bailing out from analysis: Checking file failed: Failed to execute addon 'misra' - exitcode is 1 [internalError]
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:0:0: error: Bailing out from analysis: Checking file failed: Failed to execute addon 'misra' - exitcode is 1 [internalError]
```

### Analysis Notes

- **Tool**: cppcheck with all rules enabled (--enable=all)
- **Checks**: All available cppcheck checks including MISRA C 2012, style, performance, portability, security
- **Scope**: Library modules only (src/, include/)
- **Integration**: This analysis complements the existing clang-tidy static analysis
- **MISRA Compliance**: MISRA rule texts cannot be displayed due to licensing restrictions


## Detailed clang-tidy Analysis

### Actionable Issues with Code Context (2 issues)

#### CERT Secure Coding (2 issue(s))

##### cert-dcl21-cpp (2 issue(s))

**Issue 1**: freertos_semaphore.hpp:424:3
*Reason: Const Correctness*

**Message**: overloaded 'operator++' returns a reference instead of a constant object type

```cpp
     421:    */
     422:   // NOLINTNEXTLINE(cert-dcl21-cpp): RAII class, copy is deleted -
     423:   // post-increment returns reference instead of copy
>>>  424:   counting_semaphore &operator++(int) {
     425:     give();
     426:     return *this;
     427:   }
```

**Issue 2**: freertos_semaphore.hpp:436:3
*Reason: Const Correctness*

**Message**: overloaded 'operator--' returns a reference instead of a constant object type

```cpp
     433:    */
     434:   // NOLINTNEXTLINE(cert-dcl21-cpp): RAII class, copy is deleted -
     435:   // post-decrement returns reference instead of copy
>>>  436:   counting_semaphore &operator--(int) {
     437:     take();
     438:     return *this;
     439:   }
```



---

## Complete Test Results Summary

The following table shows all individual test cases executed during this validation run:

| Test ID | Test Name | Module | Status | Time (s) |
|---------|-----------|---------|---------|----------|
| 1 | FreeRTOSTaskTest.StaticTaskAllocatorConstruction | Freertos Task | ✅ PASS | 0.000 |
| 2 | FreeRTOSTaskTest.StaticTaskAllocatorCreate | Freertos Task | ✅ PASS | 0.000 |
| 3 | FreeRTOSTaskTest.StaticTaskAllocatorCreateNullReturn | Freertos Task | ✅ PASS | 0.000 |
| 4 | FreeRTOSTaskTest.DynamicTaskAllocatorConstruction | Freertos Task | ✅ PASS | 0.000 |
| 5 | FreeRTOSTaskTest.DynamicTaskAllocatorCreateSuccess | Freertos Task | ✅ PASS | 0.000 |
| 6 | FreeRTOSTaskTest.DynamicTaskAllocatorCreateFailure | Freertos Task | ✅ PASS | 0.000 |
| 7 | FreeRTOSTaskTest.StaticTaskConstruction | Freertos Task | ✅ PASS | 0.000 |
| 8 | FreeRTOSTaskTest.StaticTaskConstructionWithString | Freertos Task | ✅ PASS | 0.000 |
| 9 | FreeRTOSTaskTest.StaticTaskDestruction | Freertos Task | ✅ PASS | 0.000 |
| 10 | FreeRTOSTaskTest.StaticTaskDestructionNullHandle | Freertos Task | ✅ PASS | 0.000 |
| 11 | FreeRTOSTaskTest.StaticTaskSuspendResume | Freertos Task | ✅ PASS | 0.000 |
| 12 | FreeRTOSTaskTest.StaticTaskTerminate | Freertos Task | ✅ PASS | 0.000 |
| 13 | FreeRTOSTaskTest.StaticTaskPriority | Freertos Task | ✅ PASS | 0.000 |
| 14 | FreeRTOSTaskTest.StaticTaskName | Freertos Task | ✅ PASS | 0.000 |
| 15 | FreeRTOSTaskTest.StaticTaskState | Freertos Task | ✅ PASS | 0.000 |
| 16 | FreeRTOSTaskTest.TaskApplicationTag | Freertos Task | ✅ PASS | 0.000 |
| 17 | FreeRTOSTaskTest.TaskStackWatermark | Freertos Task | ✅ PASS | 0.000 |
| 18 | FreeRTOSTaskTest.TaskTraceStatus | Freertos Task | ✅ PASS | 0.000 |
| 19 | FreeRTOSTaskTest.StaticTaskAbortDelay | Freertos Task | ✅ PASS | 0.000 |
| 20 | FreeRTOSTaskTest.StaticTaskAbortDelayNullHandle | Freertos Task | ✅ PASS | 0.000 |
| 21 | FreeRTOSTaskTest.TaskSuspendedOnStart | Freertos Task | ✅ PASS | 0.000 |
| 22 | FreeRTOSTaskTest.TaskNotSuspendedOnStart | Freertos Task | ✅ PASS | 0.000 |
| 23 | FreeRTOSTaskTest.DynamicTaskConstruction | Freertos Task | ✅ PASS | 0.000 |
| 24 | FreeRTOSTaskTest.DynamicTaskConstructionFailure | Freertos Task | ✅ PASS | 0.000 |
| 25 | FreeRTOSTaskTest.TaskNotifications | Freertos Task | ✅ PASS | 0.000 |
| 26 | FreeRTOSTaskTest.TaskNotificationsExtended | Freertos Task | ✅ PASS | 0.000 |
| 27 | FreeRTOSTaskTest.PeriodicTaskConstruction | Freertos Task | ✅ PASS | 0.000 |
| 28 | FreeRTOSTaskTest.PeriodicTaskWithString | Freertos Task | ✅ PASS | 0.000 |
| 29 | FreeRTOSTaskTest.PeriodicTaskZeroPeriod | Freertos Task | ✅ PASS | 0.000 |
| 30 | FreeRTOSTaskTest.PeriodicTaskNoPeriod | Freertos Task | ✅ PASS | 0.000 |
| 31 | FreeRTOSTaskTest.PeriodicTaskIsRunning | Freertos Task | ✅ PASS | 0.000 |
| 32 | FreeRTOSTaskTest.PeriodicTaskTerminate | Freertos Task | ✅ PASS | 0.000 |
| 33 | FreeRTOSTaskTest.PeriodicTaskDestructorAbortDelay | Freertos Task | ✅ PASS | 0.000 |
| 34 | FreeRTOSTaskTest.PeriodicTaskNotificationExtensions | Freertos Task | ✅ PASS | 0.000 |
| 35 | FreeRTOSTaskTest.DelayFunction | Freertos Task | ✅ PASS | 0.000 |
| 36 | FreeRTOSTaskTest.DelayUntilFunction | Freertos Task | ✅ PASS | 0.000 |
| 37 | FreeRTOSTaskTest.SleepForFunction | Freertos Task | ✅ PASS | 0.000 |
| 38 | FreeRTOSTaskTest.CurrentTaskHandle | Freertos Task | ✅ PASS | 0.000 |
| 39 | FreeRTOSTaskTest.IdleTaskHandle | Freertos Task | ✅ PASS | 0.000 |
| 40 | FreeRTOSTaskTest.TickCount | Freertos Task | ✅ PASS | 0.000 |
| 41 | FreeRTOSTaskTest.TaskCount | Freertos Task | ✅ PASS | 0.000 |
| 42 | FreeRTOSTaskTest.SchedulerState | Freertos Task | ✅ PASS | 0.000 |
| 43 | FreeRTOSTaskTest.TimeSinceSchedulerStarted | Freertos Task | ✅ PASS | 0.000 |
| 44 | FreeRTOSTaskTest.CriticalSection | Freertos Task | ✅ PASS | 0.000 |
| 45 | FreeRTOSTaskTest.CriticalSectionISR | Freertos Task | ✅ PASS | 0.000 |
| 46 | FreeRTOSTaskTest.InterruptBarrier | Freertos Task | ✅ PASS | 0.000 |
| 47 | FreeRTOSTaskTest.SchedulerBarrier | Freertos Task | ✅ PASS | 0.000 |
| 48 | FreeRTOSTaskTest.StackAllocationLimitation | Freertos Task | ✅ PASS | 0.000 |
| 49 | FreeRTOSTaskTest.InvalidParameters | Freertos Task | ✅ PASS | 0.000 |
| 50 | FreeRTOSTaskTest.ZeroStackSize | Freertos Task | ✅ PASS | 0.000 |
| 51 | FreeRTOSTaskTest.VeryHighPriority | Freertos Task | ✅ PASS | 0.000 |
| 52 | FreeRTOSTaskTest.TaskSystemStatus | Freertos Task | ✅ PASS | 0.000 |
| 53 | FreeRTOSTaskTest.TaskChronoCompatibility | Freertos Task | ✅ PASS | 0.000 |
| 54 | FreeRTOSTaskTest.DelayUntilTimePoint | Freertos Task | ✅ PASS | 0.000 |
| 55 | FreeRTOSTaskTest.TaskMoveConstruction | Freertos Task | ✅ PASS | 0.000 |
| 56 | FreeRTOSTaskTest.PeriodicTaskMoveConstruction | Freertos Task | ✅ PASS | 0.000 |
| 57 | FreeRTOSTaskTest.TaskExecutionDirectCall | Freertos Task | ✅ PASS | 0.000 |
| 58 | FreeRTOSTaskTest.PeriodicTaskTypo | Freertos Task | ✅ PASS | 0.000 |
| 59 | FreeRTOSTaskTest.TaskTemplateInstantiation | Freertos Task | ✅ PASS | 0.000 |
| 60 | FreeRTOSTaskTest.DynamicTaskDifferentSizes | Freertos Task | ✅ PASS | 0.000 |
| 61 | FreeRTOSTaskTest.TaskNotificationEdgeCases | Freertos Task | ✅ PASS | 0.000 |
| 62 | FreeRTOSTaskTest.TaskSystemStatusTemplate | Freertos Task | ✅ PASS | 0.000 |
| 63 | FreeRTOSTaskTest.DelayFunctionsWithDifferentDurations | Freertos Task | ✅ PASS | 0.000 |
| 64 | FreeRTOSTaskTest.DelayUntilWithPeriodReference | Freertos Task | ✅ PASS | 0.000 |
| 65 | FreeRTOSTaskTest.RacingConditionTaskConstructorInitialization | Freertos Task | ✅ PASS | 0.000 |
| 66 | FreeRTOSTaskTest.ConcurrentTaskCreationAndDestruction | Freertos Task | ✅ PASS | 0.000 |
| 67 | FreeRTOSTaskTest.MoveSemanticsRacingConditions | Freertos Task | ✅ PASS | 0.000 |
| 68 | FreeRTOSTaskTest.PeriodicTaskLifecycleRacingConditions | Freertos Task | ✅ PASS | 0.000 |
| 69 | FreeRTOSTaskTest.NotificationRacingConditions | Freertos Task | ✅ PASS | 0.000 |
| 70 | FreeRTOSTaskTest.ComplexMultitaskingScenario | Freertos Task | ✅ PASS | 0.000 |
| 71 | FreeRTOSTaskTest.TaskSystemStatusUnderLoad | Freertos Task | ✅ PASS | 0.000 |
| 72 | FreeRTOSTaskTest.ConstructorInitializationOrderRaceCondition | Freertos Task | ✅ PASS | 0.000 |
| 73 | FreeRTOSTaskTest.TaskExecutionInternalFunction | Freertos Task | ✅ PASS | 0.000 |
| 74 | FreeRTOSTaskTest.PeriodicTaskRunMethodExecution | Freertos Task | ✅ PASS | 0.000 |
| 75 | FreeRTOSTaskTest.YieldFunctionExecution | Freertos Task | ✅ PASS | 0.000 |
| 76 | FreeRTOSTaskTest.CriticalSectionAndBarrierUtilities | Freertos Task | ✅ PASS | 0.000 |
| 77 | FreeRTOSTaskTest.TaskSystemStatusCoverage | Freertos Task | ✅ PASS | 0.000 |
| 78 | FreeRTOSTaskTest.AdvancedRacingConditionScenarios | Freertos Task | ✅ PASS | 0.000 |
| 79 | FreeRTOSTaskTest.EdgeCaseErrorHandling | Freertos Task | ✅ PASS | 0.000 |
| 80 | FreeRTOSTaskTest.AdvancedChronoCompatibility | Freertos Task | ✅ PASS | 0.000 |
| 81 | FreeRTOSTaskTest.PriorityInheritanceScenario | Freertos Task | ✅ PASS | 0.000 |
| 82 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorConstruction | Freertos Semaphore | ✅ PASS | 0.000 |
| 83 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateBinary | Freertos Semaphore | ✅ PASS | 0.000 |
| 84 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateCounting | Freertos Semaphore | ✅ PASS | 0.000 |
| 85 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateMutex | Freertos Semaphore | ✅ PASS | 0.000 |
| 86 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateRecursiveMutex | Freertos Semaphore | ✅ PASS | 0.000 |
| 87 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorConstruction | Freertos Semaphore | ✅ PASS | 0.000 |
| 88 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateBinary | Freertos Semaphore | ✅ PASS | 0.000 |
| 89 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateCounting | Freertos Semaphore | ✅ PASS | 0.000 |
| 90 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateMutex | Freertos Semaphore | ✅ PASS | 0.000 |
| 91 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateRecursiveMutex | Freertos Semaphore | ✅ PASS | 0.000 |
| 92 | FreeRTOSSemaphoreTest.BinarySemaphoreConstruction | Freertos Semaphore | ✅ PASS | 0.000 |
| 93 | FreeRTOSSemaphoreTest.BinarySemaphoreDestruction | Freertos Semaphore | ✅ PASS | 0.000 |
| 94 | FreeRTOSSemaphoreTest.BinarySemaphoreDestructionNullHandle | Freertos Semaphore | ✅ PASS | 0.000 |
| 95 | FreeRTOSSemaphoreTest.BinarySemaphoreGive | Freertos Semaphore | ✅ PASS | 0.000 |
| 96 | FreeRTOSSemaphoreTest.BinarySemaphoreTake | Freertos Semaphore | ✅ PASS | 0.000 |
| 97 | FreeRTOSSemaphoreTest.BinarySemaphoreTakeWithTimeout | Freertos Semaphore | ✅ PASS | 0.000 |
| 98 | FreeRTOSSemaphoreTest.BinarySemaphoreISROperations | Freertos Semaphore | ✅ PASS | 0.000 |
| 99 | FreeRTOSSemaphoreTest.BinarySemaphoreChronoTimeout | Freertos Semaphore | ✅ PASS | 0.000 |
| 100 | FreeRTOSSemaphoreTest.CountingSemaphoreConstruction | Freertos Semaphore | ✅ PASS | 0.000 |
| 101 | FreeRTOSSemaphoreTest.CountingSemaphoreDefaultConstruction | Freertos Semaphore | ✅ PASS | 0.000 |
| 102 | FreeRTOSSemaphoreTest.CountingSemaphoreOperators | Freertos Semaphore | ✅ PASS | 0.000 |
| 103 | FreeRTOSSemaphoreTest.CountingSemaphoreGetCount | Freertos Semaphore | ✅ PASS | 0.000 |
| 104 | FreeRTOSSemaphoreTest.MutexConstruction | Freertos Semaphore | ✅ PASS | 0.000 |
| 105 | FreeRTOSSemaphoreTest.MutexLockUnlock | Freertos Semaphore | ✅ PASS | 0.000 |
| 106 | FreeRTOSSemaphoreTest.MutexTryLock | Freertos Semaphore | ✅ PASS | 0.000 |
| 107 | FreeRTOSSemaphoreTest.MutexLockWithTimeout | Freertos Semaphore | ✅ PASS | 0.000 |
| 108 | FreeRTOSSemaphoreTest.MutexISROperations | Freertos Semaphore | ✅ PASS | 0.000 |
| 109 | FreeRTOSSemaphoreTest.RecursiveMutexConstruction | Freertos Semaphore | ✅ PASS | 0.000 |
| 110 | FreeRTOSSemaphoreTest.RecursiveMutexLockUnlock | Freertos Semaphore | ✅ PASS | 0.000 |
| 111 | FreeRTOSSemaphoreTest.RecursiveMutexTryLock | Freertos Semaphore | ✅ PASS | 0.000 |
| 112 | FreeRTOSSemaphoreTest.RecursiveMutexNestedLocks | Freertos Semaphore | ✅ PASS | 0.000 |
| 113 | FreeRTOSSemaphoreTest.RecursiveMutexNestedFunction | Freertos Semaphore | ✅ PASS | 0.000 |
| 114 | FreeRTOSSemaphoreTest.RecursiveMutexLockGuardRAII | Freertos Semaphore | ✅ PASS | 0.000 |
| 115 | FreeRTOSSemaphoreTest.RecursiveMutexNestedLockGuards | Freertos Semaphore | ✅ PASS | 0.000 |
| 116 | FreeRTOSSemaphoreTest.RecursiveMutexTryLockGuardRAII | Freertos Semaphore | ✅ PASS | 0.000 |
| 117 | FreeRTOSSemaphoreTest.RecursiveMutexTryLockGuardFailure | Freertos Semaphore | ✅ PASS | 0.000 |
| 118 | FreeRTOSSemaphoreTest.RecursiveMutexNestedTryLockGuards | Freertos Semaphore | ✅ PASS | 0.000 |
| 119 | FreeRTOSSemaphoreTest.RecursiveMutexLockGuardISRRAII | Freertos Semaphore | ✅ PASS | 0.000 |
| 120 | FreeRTOSSemaphoreTest.RecursiveMutexNestedLockGuardISR | Freertos Semaphore | ✅ PASS | 0.000 |
| 121 | FreeRTOSSemaphoreTest.RecursiveMutexTimeoutLockGuardRAII | Freertos Semaphore | ✅ PASS | 0.000 |
| 122 | FreeRTOSSemaphoreTest.RecursiveMutexTimeoutLockGuardChrono | Freertos Semaphore | ✅ PASS | 0.000 |
| 123 | FreeRTOSSemaphoreTest.RecursiveMutexNestedTimeoutLockGuards | Freertos Semaphore | ✅ PASS | 0.000 |
| 124 | FreeRTOSSemaphoreTest.RecursiveMutexMixedLockGuardTypes | Freertos Semaphore | ✅ PASS | 0.000 |
| 125 | FreeRTOSSemaphoreTest.RecursiveMutexTimeoutLockGuardFailure | Freertos Semaphore | ✅ PASS | 0.000 |
| 126 | FreeRTOSSemaphoreTest.RecursiveMutexTryLockGuardAcquiredStateTrackingFailure | Freertos Semaphore | ✅ PASS | 0.000 |
| 127 | FreeRTOSSemaphoreTest.LockGuardRAII | Freertos Semaphore | ✅ PASS | 0.000 |
| 128 | FreeRTOSSemaphoreTest.TryLockGuardRAII | Freertos Semaphore | ✅ PASS | 0.000 |
| 129 | FreeRTOSSemaphoreTest.TryLockGuardFailure | Freertos Semaphore | ✅ PASS | 0.000 |
| 130 | FreeRTOSSemaphoreTest.LockGuardISRRAII | Freertos Semaphore | ✅ PASS | 0.000 |
| 131 | FreeRTOSSemaphoreTest.TimeoutLockGuardRAII | Freertos Semaphore | ✅ PASS | 0.000 |
| 132 | FreeRTOSSemaphoreTest.TimeoutLockGuardChronoRAII | Freertos Semaphore | ✅ PASS | 0.000 |
| 133 | FreeRTOSSemaphoreTest.TimeoutLockGuardFailure | Freertos Semaphore | ✅ PASS | 0.000 |
| 134 | FreeRTOSSemaphoreTest.TryLockGuardAcquiredStateTrackingSuccess | Freertos Semaphore | ✅ PASS | 0.000 |
| 135 | FreeRTOSSemaphoreTest.TryLockGuardAcquiredStateTrackingFailure | Freertos Semaphore | ✅ PASS | 0.000 |
| 136 | FreeRTOSSemaphoreTest.TimeoutLockGuardAcquiredStateTrackingSuccess | Freertos Semaphore | ✅ PASS | 0.000 |
| 137 | FreeRTOSSemaphoreTest.TimeoutLockGuardAcquiredStateTrackingFailure | Freertos Semaphore | ✅ PASS | 0.000 |
| 138 | FreeRTOSSemaphoreTest.LockGuardStateTrackingRaceConditionProtection | Freertos Semaphore | ✅ PASS | 0.000 |
| 139 | FreeRTOSSemaphoreTest.RecursiveMutexLockGuardAcquiredStateTracking | Freertos Semaphore | ✅ PASS | 0.000 |
| 140 | FreeRTOSSemaphoreTest.SemaphoreCreationFailure | Freertos Semaphore | ✅ PASS | 0.000 |
| 141 | FreeRTOSSemaphoreTest.StaticAllocationLimitation | Freertos Semaphore | ✅ PASS | 0.000 |
| 142 | FreeRTOSSemaphoreTest.MutexLockFailure | Freertos Semaphore | ✅ PASS | 0.000 |
| 143 | FreeRTOSSemaphoreTest.LargeCountingSemaphore | Freertos Semaphore | ✅ PASS | 0.000 |
| 144 | FreeRTOSSemaphoreTest.ZeroCountingSemaphore | Freertos Semaphore | ✅ PASS | 0.000 |
| 145 | FreeRTOSSemaphoreTest.StaticVsDynamicBehavior | Freertos Semaphore | ✅ PASS | 0.000 |
| 146 | FreeRTOSSemaphoreTest.StaticAliasNamespace | Freertos Semaphore | ✅ PASS | 0.000 |
| 147 | FreeRTOSSemaphoreTest.DynamicAliasNamespace | Freertos Semaphore | ✅ PASS | 0.000 |
| 148 | FreeRTOSSemaphoreTest.NamespaceAliasCountingSemaphore | Freertos Semaphore | ✅ PASS | 0.000 |
| 149 | FreeRTOSSemaphoreTest.NamespaceAliasMutex | Freertos Semaphore | ✅ PASS | 0.000 |
| 150 | FreeRTOSSemaphoreTest.NamespaceAliasRecursiveMutex | Freertos Semaphore | ✅ PASS | 0.000 |
| 151 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountBasic | Freertos Semaphore | ✅ PASS | 0.000 |
| 152 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountTryLock | Freertos Semaphore | ✅ PASS | 0.000 |
| 153 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountISR | Freertos Semaphore | ✅ PASS | 0.000 |
| 154 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountFailureScenarios | Freertos Semaphore | ✅ PASS | 0.000 |
| 155 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountEdgeCases | Freertos Semaphore | ✅ PASS | 0.000 |
| 156 | FreeRTOSSemaphoreTest.CountingSemaphoreMultipleOperators | Freertos Semaphore | ✅ PASS | 0.000 |
| 157 | FreeRTOSSemaphoreTest.MutexUnlockWithoutLock | Freertos Semaphore | ✅ PASS | 0.000 |
| 158 | FreeRTOSSemaphoreTest.BinarySemaphoreMultipleGive | Freertos Semaphore | ✅ PASS | 0.000 |
| 159 | FreeRTOSSemaphoreTest.BinarySemaphoreTakeUntaken | Freertos Semaphore | ✅ PASS | 0.000 |
| 160 | FreeRTOSSemaphoreTest.CountingSemaphoreEdgeCounts | Freertos Semaphore | ✅ PASS | 0.000 |
| 161 | FreeRTOSSemaphoreTest.ChronoTimeoutCompatibility | Freertos Semaphore | ✅ PASS | 0.000 |
| 162 | FreeRTOSSemaphoreTest.SemaphoreAPICompleteness | Freertos Semaphore | ✅ PASS | 0.000 |
| 163 | FreeRTOSQueueTest.StaticQueueAllocatorConstruction | Freertos Queue | ✅ PASS | 0.000 |
| 164 | FreeRTOSQueueTest.StaticQueueAllocatorCreate | Freertos Queue | ✅ PASS | 0.000 |
| 165 | FreeRTOSQueueTest.StaticQueueAllocatorDifferentTypes | Freertos Queue | ✅ PASS | 0.000 |
| 166 | FreeRTOSQueueTest.DynamicQueueAllocatorConstruction | Freertos Queue | ✅ PASS | 0.000 |
| 167 | FreeRTOSQueueTest.DynamicQueueAllocatorCreate | Freertos Queue | ✅ PASS | 0.000 |
| 168 | FreeRTOSQueueTest.QueueConstructionWithoutRegistry | Freertos Queue | ✅ PASS | 0.000 |
| 169 | FreeRTOSQueueTest.QueueConstructionWithRegistry | Freertos Queue | ✅ PASS | 0.000 |
| 170 | FreeRTOSQueueTest.QueueDestructionNullHandle | Freertos Queue | ✅ PASS | 0.000 |
| 171 | FreeRTOSQueueTest.QueueSendBasic | Freertos Queue | ✅ PASS | 0.000 |
| 172 | FreeRTOSQueueTest.QueueSendBack | Freertos Queue | ✅ PASS | 0.000 |
| 173 | FreeRTOSQueueTest.QueueSendFront | Freertos Queue | ✅ PASS | 0.000 |
| 174 | FreeRTOSQueueTest.QueueSendTimeout | Freertos Queue | ✅ PASS | 0.000 |
| 175 | FreeRTOSQueueTest.QueueSendISR | Freertos Queue | ✅ PASS | 0.000 |
| 176 | FreeRTOSQueueTest.QueueSendChronoTimeout | Freertos Queue | ✅ PASS | 0.000 |
| 177 | FreeRTOSQueueTest.QueueReceiveReference | Freertos Queue | ✅ PASS | 0.000 |
| 178 | FreeRTOSQueueTest.QueueReceiveOptional | Freertos Queue | ✅ PASS | 0.000 |
| 179 | FreeRTOSQueueTest.QueueReceiveOptionalEmpty | Freertos Queue | ✅ PASS | 0.000 |
| 180 | FreeRTOSQueueTest.QueueReceiveISR | Freertos Queue | ✅ PASS | 0.000 |
| 181 | FreeRTOSQueueTest.QueueReceiveChronoTimeout | Freertos Queue | ✅ PASS | 0.000 |
| 182 | FreeRTOSQueueTest.QueuePeekReference | Freertos Queue | ✅ PASS | 0.000 |
| 183 | FreeRTOSQueueTest.QueuePeekOptional | Freertos Queue | ✅ PASS | 0.000 |
| 184 | FreeRTOSQueueTest.QueuePeekISR | Freertos Queue | ✅ PASS | 0.000 |
| 185 | FreeRTOSQueueTest.QueueMessagesWaiting | Freertos Queue | ✅ PASS | 0.000 |
| 186 | FreeRTOSQueueTest.QueueSpacesAvailable | Freertos Queue | ✅ PASS | 0.000 |
| 187 | FreeRTOSQueueTest.QueueIsEmpty | Freertos Queue | ✅ PASS | 0.000 |
| 188 | FreeRTOSQueueTest.QueueIsFull | Freertos Queue | ✅ PASS | 0.000 |
| 189 | FreeRTOSQueueTest.QueueGetName | Freertos Queue | ✅ PASS | 0.000 |
| 190 | FreeRTOSQueueTest.QueueReset | Freertos Queue | ✅ PASS | 0.000 |
| 191 | FreeRTOSQueueTest.QueueOverwrite | Freertos Queue | ✅ PASS | 0.000 |
| 192 | FreeRTOSQueueTest.QueueOverwriteISR | Freertos Queue | ✅ PASS | 0.000 |
| 193 | FreeRTOSQueueTest.StaticQueueAliasUsage | Freertos Queue | ✅ PASS | 0.000 |
| 194 | FreeRTOSQueueTest.DynamicQueueAliasUsage | Freertos Queue | ✅ PASS | 0.000 |
| 195 | FreeRTOSQueueTest.QueueCreationFailure | Freertos Queue | ✅ PASS | 0.000 |
| 196 | FreeRTOSQueueTest.LargeQueueSize | Freertos Queue | ✅ PASS | 0.000 |
| 197 | FreeRTOSQueueTest.ZeroSizeQueue | Freertos Queue | ✅ PASS | 0.000 |
| 198 | FreeRTOSQueueTest.ComplexDataTypeQueue | Freertos Queue | ✅ PASS | 0.000 |
| 199 | FreeRTOSQueueTest.QueueAPICompleteness | Freertos Queue | ✅ PASS | 0.000 |
| 200 | FreeRTOSQueueTest.QueueWithVariousDataTypes | Freertos Queue | ✅ PASS | 0.000 |
| 201 | FreeRTOSQueueTest.QueueWithEnumTypes | Freertos Queue | ✅ PASS | 0.000 |
| 202 | FreeRTOSQueueTest.QueueWithPointerTypes | Freertos Queue | ✅ PASS | 0.000 |
| 203 | FreeRTOSQueueTest.QueueChronoMicrosecondsTimeout | Freertos Queue | ✅ PASS | 0.000 |
| 204 | FreeRTOSQueueTest.QueueChronoMinutesTimeout | Freertos Queue | ✅ PASS | 0.000 |
| 205 | FreeRTOSQueueTest.QueueChronoNanosecondsTimeout | Freertos Queue | ✅ PASS | 0.000 |
| 206 | FreeRTOSQueueTest.QueueChronoOptionalReceiveVariousDurations | Freertos Queue | ✅ PASS | 0.000 |
| 207 | FreeRTOSQueueTest.StaticQueueAllocatorWithVariousTypes | Freertos Queue | ✅ PASS | 0.000 |
| 208 | FreeRTOSQueueTest.QueueISROperationsWithDifferentTypes | Freertos Queue | ✅ PASS | 0.000 |
| 209 | FreeRTOSQueueTest.QueueSendBackISROperations | Freertos Queue | ✅ PASS | 0.000 |
| 210 | FreeRTOSQueueTest.QueueSendFrontISROperations | Freertos Queue | ✅ PASS | 0.000 |
| 211 | FreeRTOSQueueTest.QueueMessagesWaitingISR | Freertos Queue | ✅ PASS | 0.000 |
| 212 | FreeRTOSEventGroupTest.StaticEventGroupAllocatorConstruction | Freertos Event Group | ✅ PASS | 0.000 |
| 213 | FreeRTOSEventGroupTest.StaticEventGroupAllocatorCreate | Freertos Event Group | ✅ PASS | 0.000 |
| 214 | FreeRTOSEventGroupTest.StaticEventGroupAllocatorCreateFailure | Freertos Event Group | ✅ PASS | 0.000 |
| 215 | FreeRTOSEventGroupTest.DynamicEventGroupAllocatorConstruction | Freertos Event Group | ✅ PASS | 0.000 |
| 216 | FreeRTOSEventGroupTest.DynamicEventGroupAllocatorCreate | Freertos Event Group | ✅ PASS | 0.000 |
| 217 | FreeRTOSEventGroupTest.DynamicEventGroupAllocatorCreateFailure | Freertos Event Group | ✅ PASS | 0.000 |
| 218 | FreeRTOSEventGroupTest.EventGroupConstruction | Freertos Event Group | ✅ PASS | 0.000 |
| 219 | FreeRTOSEventGroupTest.EventGroupDestructionNullHandle | Freertos Event Group | ✅ PASS | 0.000 |
| 220 | FreeRTOSEventGroupTest.StaticEventGroupConstruction | Freertos Event Group | ✅ PASS | 0.000 |
| 221 | FreeRTOSEventGroupTest.EventGroupSetBits | Freertos Event Group | ✅ PASS | 0.000 |
| 222 | FreeRTOSEventGroupTest.EventGroupSetBitsISR | Freertos Event Group | ✅ PASS | 0.000 |
| 223 | FreeRTOSEventGroupTest.EventGroupClearBits | Freertos Event Group | ✅ PASS | 0.000 |
| 224 | FreeRTOSEventGroupTest.EventGroupGetBits | Freertos Event Group | ✅ PASS | 0.000 |
| 225 | FreeRTOSEventGroupTest.EventGroupGetBitsISR | Freertos Event Group | ✅ PASS | 0.000 |
| 226 | FreeRTOSEventGroupTest.EventGroupWaitBitsAnyBits | Freertos Event Group | ✅ PASS | 0.000 |
| 227 | FreeRTOSEventGroupTest.EventGroupWaitBitsAllBits | Freertos Event Group | ✅ PASS | 0.000 |
| 228 | FreeRTOSEventGroupTest.EventGroupWaitBitsTimeout | Freertos Event Group | ✅ PASS | 0.000 |
| 229 | FreeRTOSEventGroupTest.EventGroupWaitBitsChronoTimeout | Freertos Event Group | ✅ PASS | 0.000 |
| 230 | FreeRTOSEventGroupTest.EventGroupSync | Freertos Event Group | ✅ PASS | 0.000 |
| 231 | FreeRTOSEventGroupTest.EventGroupSyncChronoTimeout | Freertos Event Group | ✅ PASS | 0.000 |
| 232 | FreeRTOSEventGroupTest.EventGroupSyncTimeout | Freertos Event Group | ✅ PASS | 0.000 |
| 233 | FreeRTOSEventGroupTest.EventGroupChronoMicrosecondsTimeout | Freertos Event Group | ✅ PASS | 0.000 |
| 234 | FreeRTOSEventGroupTest.EventGroupChronoNanosecondsTimeout | Freertos Event Group | ✅ PASS | 0.000 |
| 235 | FreeRTOSEventGroupTest.EventGroupChronoMinutesTimeout | Freertos Event Group | ✅ PASS | 0.000 |
| 236 | FreeRTOSEventGroupTest.EventGroupComplexScenario | Freertos Event Group | ✅ PASS | 0.000 |
| 237 | FreeRTOSEventGroupTest.EventGroupCreationFailure | Freertos Event Group | ✅ PASS | 0.000 |
| 238 | FreeRTOSEventGroupTest.EventGroupAllBitsPattern | Freertos Event Group | ✅ PASS | 0.000 |
| 239 | FreeRTOSEventGroupTest.EventGroupZeroBitsPattern | Freertos Event Group | ✅ PASS | 0.000 |
| 240 | FreeRTOSEventGroupTest.StaticVsDynamicBehavior | Freertos Event Group | ✅ PASS | 0.000 |
| 241 | FreeRTOSEventGroupTest.EventGroupAPICompleteness | Freertos Event Group | ✅ PASS | 0.000 |
| 242 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorConstruction | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 243 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorCreate | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 244 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorCreateWithTriggerLevel | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 245 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorCreateFailure | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 246 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorConstruction | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 247 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorCreate | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 248 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorCreateWithTriggerLevel | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 249 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorCreateFailure | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 250 | FreeRTOSStreamBufferTest.StaticStreamBufferConstruction | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 251 | FreeRTOSStreamBufferTest.DynamicStreamBufferConstruction | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 252 | FreeRTOSStreamBufferTest.StreamBufferConstructionWithTriggerLevel | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 253 | FreeRTOSStreamBufferTest.StreamBufferDestructionNullHandle | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 254 | FreeRTOSStreamBufferTest.StreamBufferSendBasic | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 255 | FreeRTOSStreamBufferTest.StreamBufferSendWithTimeout | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 256 | FreeRTOSStreamBufferTest.StreamBufferSendChronoTimeout | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 257 | FreeRTOSStreamBufferTest.StreamBufferSendIterators | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 258 | FreeRTOSStreamBufferTest.StreamBufferSendIteratorsWithTimeout | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 259 | FreeRTOSStreamBufferTest.StreamBufferSendPartial | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 260 | FreeRTOSStreamBufferTest.StreamBufferSendISR | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 261 | FreeRTOSStreamBufferTest.StreamBufferSendISRWithoutWoken | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 262 | FreeRTOSStreamBufferTest.StreamBufferSendISRIterators | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 263 | FreeRTOSStreamBufferTest.StreamBufferReceiveBasic | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 264 | FreeRTOSStreamBufferTest.StreamBufferReceiveWithTimeout | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 265 | FreeRTOSStreamBufferTest.StreamBufferReceiveChronoTimeout | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 266 | FreeRTOSStreamBufferTest.StreamBufferReceiveTimeout | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 267 | FreeRTOSStreamBufferTest.StreamBufferReceiveISR | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 268 | FreeRTOSStreamBufferTest.StreamBufferReceiveISRWithoutWoken | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 269 | FreeRTOSStreamBufferTest.StreamBufferBytesAvailable | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 270 | FreeRTOSStreamBufferTest.StreamBufferSpacesAvailable | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 271 | FreeRTOSStreamBufferTest.StreamBufferIsEmpty | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 272 | FreeRTOSStreamBufferTest.StreamBufferIsNotEmpty | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 273 | FreeRTOSStreamBufferTest.StreamBufferIsFull | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 274 | FreeRTOSStreamBufferTest.StreamBufferIsNotFull | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 275 | FreeRTOSStreamBufferTest.StreamBufferReset | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 276 | FreeRTOSStreamBufferTest.StreamBufferResetFailure | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 277 | FreeRTOSStreamBufferTest.StreamBufferSetTriggerLevel | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 278 | FreeRTOSStreamBufferTest.StreamBufferSetTriggerLevelFailure | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 279 | FreeRTOSStreamBufferTest.StreamBufferGetHandle | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 280 | FreeRTOSStreamBufferTest.StreamBufferCreationFailure | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 281 | FreeRTOSStreamBufferTest.StreamBufferSendZeroBytes | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 282 | FreeRTOSStreamBufferTest.StreamBufferReceiveZeroBytes | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 283 | FreeRTOSStreamBufferTest.StreamBufferComplexScenario | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 284 | FreeRTOSStreamBufferTest.StaticVsDynamicBehavior | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 285 | FreeRTOSStreamBufferTest.StreamBufferChronoMicrosecondsTimeout | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 286 | FreeRTOSStreamBufferTest.StreamBufferChronoNanosecondsTimeout | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 287 | FreeRTOSStreamBufferTest.StreamBufferChronoSecondsTimeout | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 288 | FreeRTOSStreamBufferTest.StreamBufferAPICompleteness | Freertos Stream Buffer | ✅ PASS | 0.000 |
| 289 | FreeRTOSMessageBufferTest.StaticMessageBufferAllocatorConstruction | Freertos Message Buffer | ✅ PASS | 0.000 |
| 290 | FreeRTOSMessageBufferTest.StaticMessageBufferAllocatorCreate | Freertos Message Buffer | ✅ PASS | 0.000 |
| 291 | FreeRTOSMessageBufferTest.StaticMessageBufferAllocatorCreateFailure | Freertos Message Buffer | ✅ PASS | 0.000 |
| 292 | FreeRTOSMessageBufferTest.DynamicMessageBufferAllocatorConstruction | Freertos Message Buffer | ✅ PASS | 0.000 |
| 293 | FreeRTOSMessageBufferTest.DynamicMessageBufferAllocatorCreateSuccess | Freertos Message Buffer | ✅ PASS | 0.000 |
| 294 | FreeRTOSMessageBufferTest.DynamicMessageBufferAllocatorCreateFailure | Freertos Message Buffer | ✅ PASS | 0.000 |
| 295 | FreeRTOSMessageBufferTest.StaticMessageBufferConstruction | Freertos Message Buffer | ✅ PASS | 0.000 |
| 296 | FreeRTOSMessageBufferTest.StaticMessageBufferDestruction | Freertos Message Buffer | ✅ PASS | 0.000 |
| 297 | FreeRTOSMessageBufferTest.StaticMessageBufferDestructionNullHandle | Freertos Message Buffer | ✅ PASS | 0.000 |
| 298 | FreeRTOSMessageBufferTest.DynamicMessageBufferConstruction | Freertos Message Buffer | ✅ PASS | 0.000 |
| 299 | FreeRTOSMessageBufferTest.DynamicMessageBufferConstructionFailure | Freertos Message Buffer | ✅ PASS | 0.000 |
| 300 | FreeRTOSMessageBufferTest.MessageBufferSendSuccess | Freertos Message Buffer | ✅ PASS | 0.000 |
| 301 | FreeRTOSMessageBufferTest.MessageBufferSendTimeout | Freertos Message Buffer | ✅ PASS | 0.000 |
| 302 | FreeRTOSMessageBufferTest.MessageBufferSendWithChrono | Freertos Message Buffer | ✅ PASS | 0.000 |
| 303 | FreeRTOSMessageBufferTest.MessageBufferSendZeroLengthMessage | Freertos Message Buffer | ✅ PASS | 0.000 |
| 304 | FreeRTOSMessageBufferTest.MessageBufferSendMaxSizeMessage | Freertos Message Buffer | ✅ PASS | 0.000 |
| 305 | FreeRTOSMessageBufferTest.MessageBufferReceiveSuccess | Freertos Message Buffer | ✅ PASS | 0.000 |
| 306 | FreeRTOSMessageBufferTest.MessageBufferReceiveTimeout | Freertos Message Buffer | ✅ PASS | 0.000 |
| 307 | FreeRTOSMessageBufferTest.MessageBufferReceiveWithChrono | Freertos Message Buffer | ✅ PASS | 0.000 |
| 308 | FreeRTOSMessageBufferTest.MessageBufferReceiveBufferTooSmall | Freertos Message Buffer | ✅ PASS | 0.000 |
| 309 | FreeRTOSMessageBufferTest.MessageBufferAvailableSpace | Freertos Message Buffer | ✅ PASS | 0.000 |
| 310 | FreeRTOSMessageBufferTest.MessageBufferIsEmpty | Freertos Message Buffer | ✅ PASS | 0.000 |
| 311 | FreeRTOSMessageBufferTest.MessageBufferIsFull | Freertos Message Buffer | ✅ PASS | 0.000 |
| 312 | FreeRTOSMessageBufferTest.MessageBufferReset | Freertos Message Buffer | ✅ PASS | 0.000 |
| 313 | FreeRTOSMessageBufferTest.MessageBufferCreationFailure | Freertos Message Buffer | ✅ PASS | 0.000 |
| 314 | FreeRTOSMessageBufferTest.MessageBufferZeroSizeBuffer | Freertos Message Buffer | ✅ PASS | 0.000 |
| 315 | FreeRTOSMessageBufferTest.MessageBufferVeryLargeBuffer | Freertos Message Buffer | ✅ PASS | 0.000 |
| 316 | FreeRTOSMessageBufferTest.MessageBufferChronoMicrosecondsTimeout | Freertos Message Buffer | ✅ PASS | 0.000 |
| 317 | FreeRTOSMessageBufferTest.MessageBufferChronoSecondsTimeout | Freertos Message Buffer | ✅ PASS | 0.000 |
| 318 | FreeRTOSMessageBufferTest.StaticVsDynamicBehaviorComparison | Freertos Message Buffer | ✅ PASS | 0.000 |
| 319 | FreeRTOSMessageBufferTest.MessageBufferComplexSendReceiveScenario | Freertos Message Buffer | ✅ PASS | 0.000 |
| 320 | FreeRTOSMessageBufferTest.MessageBufferAPICompleteness | Freertos Message Buffer | ✅ PASS | 0.000 |
| 321 | FreeRTOSSwTimerTest.StaticTimerAllocatorConstruction | Freertos Sw Timer | ✅ PASS | 0.000 |
| 322 | FreeRTOSSwTimerTest.StaticTimerAllocatorCreate | Freertos Sw Timer | ✅ PASS | 0.000 |
| 323 | FreeRTOSSwTimerTest.StaticTimerAllocatorCreateNullReturn | Freertos Sw Timer | ✅ PASS | 0.000 |
| 324 | FreeRTOSSwTimerTest.DynamicTimerAllocatorConstruction | Freertos Sw Timer | ✅ PASS | 0.000 |
| 325 | FreeRTOSSwTimerTest.DynamicTimerAllocatorCreate | Freertos Sw Timer | ✅ PASS | 0.000 |
| 326 | FreeRTOSSwTimerTest.DynamicTimerAllocatorCreateNullReturn | Freertos Sw Timer | ✅ PASS | 0.000 |
| 327 | FreeRTOSSwTimerTest.StaticTimerConstruction | Freertos Sw Timer | ✅ PASS | 0.000 |
| 328 | FreeRTOSSwTimerTest.StaticTimerConstructionWithChrono | Freertos Sw Timer | ✅ PASS | 0.000 |
| 329 | FreeRTOSSwTimerTest.StaticTimerDestruction | Freertos Sw Timer | ✅ PASS | 0.000 |
| 330 | FreeRTOSSwTimerTest.StaticTimerDestructionNullHandle | Freertos Sw Timer | ✅ PASS | 0.000 |
| 331 | FreeRTOSSwTimerTest.StaticTimerStart | Freertos Sw Timer | ✅ PASS | 0.000 |
| 332 | FreeRTOSSwTimerTest.StaticTimerStartWithTimeout | Freertos Sw Timer | ✅ PASS | 0.000 |
| 333 | FreeRTOSSwTimerTest.StaticTimerStartWithChrono | Freertos Sw Timer | ✅ PASS | 0.000 |
| 334 | FreeRTOSSwTimerTest.StaticTimerStop | Freertos Sw Timer | ✅ PASS | 0.000 |
| 335 | FreeRTOSSwTimerTest.StaticTimerReset | Freertos Sw Timer | ✅ PASS | 0.000 |
| 336 | FreeRTOSSwTimerTest.StaticTimerPeriodChange | Freertos Sw Timer | ✅ PASS | 0.000 |
| 337 | FreeRTOSSwTimerTest.StaticTimerIsRunning | Freertos Sw Timer | ✅ PASS | 0.000 |
| 338 | FreeRTOSSwTimerTest.StaticTimerGetPeriod | Freertos Sw Timer | ✅ PASS | 0.000 |
| 339 | FreeRTOSSwTimerTest.StaticTimerGetName | Freertos Sw Timer | ✅ PASS | 0.000 |
| 340 | FreeRTOSSwTimerTest.StaticTimerReloadMode | Freertos Sw Timer | ✅ PASS | 0.000 |
| 341 | FreeRTOSSwTimerTest.DynamicTimerConstruction | Freertos Sw Timer | ✅ PASS | 0.000 |
| 342 | FreeRTOSSwTimerTest.DynamicTimerConstructionFailure | Freertos Sw Timer | ✅ PASS | 0.000 |
| 343 | FreeRTOSSwTimerTest.StaticTimerStartFromISR | Freertos Sw Timer | ✅ PASS | 0.000 |
| 344 | FreeRTOSSwTimerTest.StaticTimerStartFromISRNoParam | Freertos Sw Timer | ✅ PASS | 0.000 |
| 345 | FreeRTOSSwTimerTest.StaticTimerStopFromISR | Freertos Sw Timer | ✅ PASS | 0.000 |
| 346 | FreeRTOSSwTimerTest.StaticTimerResetFromISR | Freertos Sw Timer | ✅ PASS | 0.000 |
| 347 | FreeRTOSSwTimerTest.StaticTimerPeriodFromISR | Freertos Sw Timer | ✅ PASS | 0.000 |
| 348 | FreeRTOSSwTimerTest.StaticTimerRemainingTime | Freertos Sw Timer | ✅ PASS | 0.000 |
| 349 | FreeRTOSSwTimerTest.StaticTimerRemainingTimeNullHandle | Freertos Sw Timer | ✅ PASS | 0.000 |
| 350 | FreeRTOSSwTimerTest.TimerOperationsWithNullHandle | Freertos Sw Timer | ✅ PASS | 0.000 |
| 351 | FreeRTOSSwTimerTest.TimerFailureConditions | Freertos Sw Timer | ✅ PASS | 0.000 |
| 352 | FreeRTOSSwTimerTest.TimerMoveConstructionIssueScenario | Freertos Sw Timer | ✅ PASS | 0.000 |
| 353 | FreeRTOSSwTimerTest.TimerMoveConstruction | Freertos Sw Timer | ✅ PASS | 0.000 |
| 354 | FreeRTOSSwTimerTest.MovedFromTimerIsInvalidated | Freertos Sw Timer | ✅ PASS | 0.000 |
| 355 | FreeRTOSSwTimerTest.TimerMoveAssignment | Freertos Sw Timer | ✅ PASS | 0.000 |
| 356 | FreeRTOSSwTimerTest.ChronoCompatibility | Freertos Sw Timer | ✅ PASS | 0.000 |
| 357 | FreeRTOSSwTimerTest.PeriodChangeWithMixedDurationTypes | Freertos Sw Timer | ✅ PASS | 0.000 |
| 358 | FreeRTOSSwTimerTest.PeriodChangeWithVariousDurationCombinations | Freertos Sw Timer | ✅ PASS | 0.000 |
| 359 | FreeRTOSSwTimerTest.PeriodChangeWithMicrosecondPrecision | Freertos Sw Timer | ✅ PASS | 0.000 |
| 360 | FreeRTOSSwTimerTest.PeriodChangeWithMinutePrecision | Freertos Sw Timer | ✅ PASS | 0.000 |
| 361 | FreeRTOSSwTimerTest.PeriodChangeWithFloatingPointDurations | Freertos Sw Timer | ✅ PASS | 0.000 |
| 362 | FreeRTOSSwTimerTest.PeriodChangeEdgeCaseDurations | Freertos Sw Timer | ✅ PASS | 0.000 |
| 363 | FreeRTOSSwTimerTest.ConstructorWithVariousDurationTypes | Freertos Sw Timer | ✅ PASS | 0.000 |
| 364 | FreeRTOSSwTimerTest.StartStopResetWithMixedDurationTypes | Freertos Sw Timer | ✅ PASS | 0.000 |
| 365 | FreeRTOSSwTimerTest.ISRFunctionsWithMixedDurationTypes | Freertos Sw Timer | ✅ PASS | 0.000 |
| 366 | FreeRTOSSwTimerTest.SingleShotTimerBehavior | Freertos Sw Timer | ✅ PASS | 0.000 |
| 367 | FreeRTOSSwTimerTest.AutoReloadTimerBehavior | Freertos Sw Timer | ✅ PASS | 0.000 |
| 368 | FreeRTOSSwTimerTest.CallbackFunctionality | Freertos Sw Timer | ✅ PASS | 0.000 |
| 369 | FreeRTOSSwTimerTest.DestructorComplexScenarios | Freertos Sw Timer | ✅ PASS | 0.000 |
| 370 | FreeRTOSSwTimerTest.DestructorWithActiveTimer | Freertos Sw Timer | ✅ PASS | 0.000 |
| 371 | EnhancedMultitaskingTest.TaskActuallyExecutes | Enhanced Multitasking | ✅ PASS | 0.050 |
| 372 | EnhancedMultitaskingTest.TaskExecutionWithSuspendResume | Enhanced Multitasking | ✅ PASS | 0.060 |
| 373 | EnhancedMultitaskingTest.MultipleTasksConcurrentExecution | Enhanced Multitasking | ✅ PASS | 0.100 |
| 374 | EnhancedMultitaskingTest.TaskSynchronizationWithNotifications | Enhanced Multitasking | ✅ PASS | 0.050 |
| 375 | EnhancedMultitaskingTest.TaskLifecycleRacingConditions | Enhanced Multitasking | ✅ PASS | 0.032 |
| 376 | EnhancedMultitaskingTest.TaskMoveSemanticsConcurrency | Enhanced Multitasking | ✅ PASS | 0.035 |
| 377 | EnhancedMultitaskingTest.PeriodicTaskExecution | Enhanced Multitasking | ✅ PASS | 0.040 |
| 378 | EnhancedMultitaskingTest.MultiplePeriodicTasksCoordination | Enhanced Multitasking | ✅ PASS | 0.097 |
| 379 | EnhancedMultitaskingTest.TaskExceptionHandling | Enhanced Multitasking | ✅ PASS | 0.020 |
| 380 | EnhancedMultitaskingTest.TaskDeleteDuringExecution | Enhanced Multitasking | ✅ PASS | 0.106 |
| 381 | EnhancedMultitaskingTest.TaskExecFunctionCoverage | Enhanced Multitasking | ✅ PASS | 0.010 |
| 382 | EnhancedMultitaskingTest.SuspendedTaskStartupCoverage | Enhanced Multitasking | ✅ PASS | 0.015 |
| 383 | STLSemaphoreTest.STLBinarySemaphoreBasicOperations | Freertos Semaphore Stl | ✅ PASS | 0.000 |
| 384 | STLSemaphoreTest.STLBinarySemaphoreTimeout | Freertos Semaphore Stl | ✅ PASS | 0.100 |
| 385 | STLSemaphoreTest.STLBinarySemaphoreProducerConsumer | Freertos Semaphore Stl | ✅ PASS | 0.050 |
| 386 | STLSemaphoreTest.STLBinarySemaphoreMultipleProducers | Freertos Semaphore Stl | ✅ PASS | 0.040 |
| 387 | STLSemaphoreTest.STLCountingSemaphoreBasicOperations | Freertos Semaphore Stl | ✅ PASS | 0.000 |
| 388 | STLSemaphoreTest.STLCountingSemaphoreMaxLimit | Freertos Semaphore Stl | ✅ PASS | 0.000 |
| 389 | STLSemaphoreTest.STLCountingSemaphoreResourcePool | Freertos Semaphore Stl | ✅ PASS | 0.100 |
| 390 | STLSemaphoreTest.STLMutexBasicOperations | Freertos Semaphore Stl | ✅ PASS | 0.000 |
| 391 | STLSemaphoreTest.STLMutexCriticalSection | Freertos Semaphore Stl | ✅ PASS | 0.031 |
| 392 | STLSemaphoreTest.STLRecursiveMutexBasicOperations | Freertos Semaphore Stl | ✅ PASS | 0.000 |
| 393 | STLSemaphoreTest.STLRecursiveMutexNestedLocking | Freertos Semaphore Stl | ✅ PASS | 0.000 |
| 394 | STLSemaphoreTest.STLProducerConsumerWithBuffering | Freertos Semaphore Stl | ✅ PASS | 0.101 |
| 395 | STLSemaphoreTest.STLReaderWriterPattern | Freertos Semaphore Stl | ✅ PASS | 0.434 |
| 396 | STLSemaphoreTest.STLSemaphoreStressTest | Freertos Semaphore Stl | ✅ PASS | 0.077 |
| 397 | STLSemaphoreTest.STLTimeoutAccuracy | Freertos Semaphore Stl | ✅ PASS | 0.350 |
| 398 | TimeoutPrecisionTest.MicrosecondDelayConversion | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 399 | TimeoutPrecisionTest.NanosecondDelayConversion | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 400 | TimeoutPrecisionTest.MixedDurationTypes | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 401 | TimeoutPrecisionTest.ChronoBinarySemaphoreTimeout | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 402 | TimeoutPrecisionTest.ChronoCountingSemaphoreTimeout | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 403 | TimeoutPrecisionTest.ZeroDurationHandling | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 404 | TimeoutPrecisionTest.MaximumDurationHandling | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 405 | TimeoutPrecisionTest.DurationCastingAccuracy | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 406 | TimeoutPrecisionTest.ConcurrentTimeoutOperations | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 407 | TimeoutPrecisionTest.EdgeCaseDurations | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 408 | TimeoutPrecisionTest.TickConversionConsistency | Enhanced Timeout Precision | ✅ PASS | 0.000 |
| 409 | Cpp17FeaturesTest.TaskMoveSemanticsPerfectForwarding | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 410 | Cpp17FeaturesTest.TaskMoveAssignmentChaining | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 411 | Cpp17FeaturesTest.PeriodicTaskMoveWithChronoTypes | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 412 | Cpp17FeaturesTest.LambdaCaptureVarieties | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 413 | Cpp17FeaturesTest.FunctionObjectVarieties | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 414 | Cpp17FeaturesTest.RAIIExceptionSafety | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 415 | Cpp17FeaturesTest.CriticalSectionRAII | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 416 | Cpp17FeaturesTest.TypeTraitsValidation | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 417 | Cpp17FeaturesTest.TemplateDeductionGuides | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 418 | Cpp17FeaturesTest.CompileTimeConstants | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 419 | Cpp17FeaturesTest.ChronoTypesIntegration | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 420 | Cpp17FeaturesTest.STLContainerIntegration | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 421 | Cpp17FeaturesTest.UniquePtrIntegration | Enhanced Cpp17 Features | ✅ PASS | 0.000 |
| 422 | EnhancedFreeRTOSSwTimerTest.TimerCreationWithEnhancedMocks | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 423 | EnhancedFreeRTOSSwTimerTest.SingleShotTimerRealBehavior | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 424 | EnhancedFreeRTOSSwTimerTest.AutoReloadTimerRealBehavior | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 425 | EnhancedFreeRTOSSwTimerTest.TimerResetRealBehavior | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 426 | EnhancedFreeRTOSSwTimerTest.PeriodChangeWithRealBehavior | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 427 | EnhancedFreeRTOSSwTimerTest.PeriodChangeWithChronoDurations | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 428 | EnhancedFreeRTOSSwTimerTest.MultipleTimersCoordination | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 429 | EnhancedFreeRTOSSwTimerTest.TimerInteraction | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 430 | EnhancedFreeRTOSSwTimerTest.InvalidTimerOperations | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 431 | EnhancedFreeRTOSSwTimerTest.ZeroPeriodTimer | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 432 | EnhancedFreeRTOSSwTimerTest.NullCallbackTimer | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 433 | EnhancedFreeRTOSSwTimerTest.TimingAccuracyVerification | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 434 | EnhancedFreeRTOSSwTimerTest.ExpiryTimeCalculation | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 435 | EnhancedFreeRTOSSwTimerTest.DynamicTimerEnhancedBehavior | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 436 | EnhancedFreeRTOSSwTimerTest.ISRFunctionsEnhancedBehavior | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 437 | EnhancedFreeRTOSSwTimerTest.CommandQueueProcessing | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 438 | EnhancedFreeRTOSSwTimerTest.ComprehensiveIntegrationTest | Enhanced Freertos Sw Timer | ✅ PASS | 0.000 |
| 439 | EnhancedFreeRTOSSwTimerTest.ManyTimersStressTest | Enhanced Freertos Sw Timer | ✅ PASS | 0.002 |

**Table Summary:**
- **Total Individual Tests**: 439
- **Passed**: 439 (✅)
- **Failed**: 0 (❌)
- **Success Rate**: 100.0%

## Detailed Test Results by Module

### Task Module Tests

**Module Statistics:**
- Tests: 93
- Passed: 93
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.615s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 1 | FreeRTOSTaskTest.StaticTaskAllocatorConstruction | ✅ PASS | 0.000s |
| 2 | FreeRTOSTaskTest.StaticTaskAllocatorCreate | ✅ PASS | 0.000s |
| 3 | FreeRTOSTaskTest.StaticTaskAllocatorCreateNullReturn | ✅ PASS | 0.000s |
| 4 | FreeRTOSTaskTest.DynamicTaskAllocatorConstruction | ✅ PASS | 0.000s |
| 5 | FreeRTOSTaskTest.DynamicTaskAllocatorCreateSuccess | ✅ PASS | 0.000s |
| 6 | FreeRTOSTaskTest.DynamicTaskAllocatorCreateFailure | ✅ PASS | 0.000s |
| 7 | FreeRTOSTaskTest.StaticTaskConstruction | ✅ PASS | 0.000s |
| 8 | FreeRTOSTaskTest.StaticTaskConstructionWithString | ✅ PASS | 0.000s |
| 9 | FreeRTOSTaskTest.StaticTaskDestruction | ✅ PASS | 0.000s |
| 10 | FreeRTOSTaskTest.StaticTaskDestructionNullHandle | ✅ PASS | 0.000s |
| 11 | FreeRTOSTaskTest.StaticTaskSuspendResume | ✅ PASS | 0.000s |
| 12 | FreeRTOSTaskTest.StaticTaskTerminate | ✅ PASS | 0.000s |
| 13 | FreeRTOSTaskTest.StaticTaskPriority | ✅ PASS | 0.000s |
| 14 | FreeRTOSTaskTest.StaticTaskName | ✅ PASS | 0.000s |
| 15 | FreeRTOSTaskTest.StaticTaskState | ✅ PASS | 0.000s |
| 16 | FreeRTOSTaskTest.TaskApplicationTag | ✅ PASS | 0.000s |
| 17 | FreeRTOSTaskTest.TaskStackWatermark | ✅ PASS | 0.000s |
| 18 | FreeRTOSTaskTest.TaskTraceStatus | ✅ PASS | 0.000s |
| 19 | FreeRTOSTaskTest.StaticTaskAbortDelay | ✅ PASS | 0.000s |
| 20 | FreeRTOSTaskTest.StaticTaskAbortDelayNullHandle | ✅ PASS | 0.000s |
| 21 | FreeRTOSTaskTest.TaskSuspendedOnStart | ✅ PASS | 0.000s |
| 22 | FreeRTOSTaskTest.TaskNotSuspendedOnStart | ✅ PASS | 0.000s |
| 23 | FreeRTOSTaskTest.DynamicTaskConstruction | ✅ PASS | 0.000s |
| 24 | FreeRTOSTaskTest.DynamicTaskConstructionFailure | ✅ PASS | 0.000s |
| 25 | FreeRTOSTaskTest.TaskNotifications | ✅ PASS | 0.000s |
| 26 | FreeRTOSTaskTest.TaskNotificationsExtended | ✅ PASS | 0.000s |
| 27 | FreeRTOSTaskTest.PeriodicTaskConstruction | ✅ PASS | 0.000s |
| 28 | FreeRTOSTaskTest.PeriodicTaskWithString | ✅ PASS | 0.000s |
| 29 | FreeRTOSTaskTest.PeriodicTaskZeroPeriod | ✅ PASS | 0.000s |
| 30 | FreeRTOSTaskTest.PeriodicTaskNoPeriod | ✅ PASS | 0.000s |
| 31 | FreeRTOSTaskTest.PeriodicTaskIsRunning | ✅ PASS | 0.000s |
| 32 | FreeRTOSTaskTest.PeriodicTaskTerminate | ✅ PASS | 0.000s |
| 33 | FreeRTOSTaskTest.PeriodicTaskDestructorAbortDelay | ✅ PASS | 0.000s |
| 34 | FreeRTOSTaskTest.PeriodicTaskNotificationExtensions | ✅ PASS | 0.000s |
| 35 | FreeRTOSTaskTest.DelayFunction | ✅ PASS | 0.000s |
| 36 | FreeRTOSTaskTest.DelayUntilFunction | ✅ PASS | 0.000s |
| 37 | FreeRTOSTaskTest.SleepForFunction | ✅ PASS | 0.000s |
| 38 | FreeRTOSTaskTest.CurrentTaskHandle | ✅ PASS | 0.000s |
| 39 | FreeRTOSTaskTest.IdleTaskHandle | ✅ PASS | 0.000s |
| 40 | FreeRTOSTaskTest.TickCount | ✅ PASS | 0.000s |
| 41 | FreeRTOSTaskTest.TaskCount | ✅ PASS | 0.000s |
| 42 | FreeRTOSTaskTest.SchedulerState | ✅ PASS | 0.000s |
| 43 | FreeRTOSTaskTest.TimeSinceSchedulerStarted | ✅ PASS | 0.000s |
| 44 | FreeRTOSTaskTest.CriticalSection | ✅ PASS | 0.000s |
| 45 | FreeRTOSTaskTest.CriticalSectionISR | ✅ PASS | 0.000s |
| 46 | FreeRTOSTaskTest.InterruptBarrier | ✅ PASS | 0.000s |
| 47 | FreeRTOSTaskTest.SchedulerBarrier | ✅ PASS | 0.000s |
| 48 | FreeRTOSTaskTest.StackAllocationLimitation | ✅ PASS | 0.000s |
| 49 | FreeRTOSTaskTest.InvalidParameters | ✅ PASS | 0.000s |
| 50 | FreeRTOSTaskTest.ZeroStackSize | ✅ PASS | 0.000s |
| 51 | FreeRTOSTaskTest.VeryHighPriority | ✅ PASS | 0.000s |
| 52 | FreeRTOSTaskTest.TaskSystemStatus | ✅ PASS | 0.000s |
| 53 | FreeRTOSTaskTest.TaskChronoCompatibility | ✅ PASS | 0.000s |
| 54 | FreeRTOSTaskTest.DelayUntilTimePoint | ✅ PASS | 0.000s |
| 55 | FreeRTOSTaskTest.TaskMoveConstruction | ✅ PASS | 0.000s |
| 56 | FreeRTOSTaskTest.PeriodicTaskMoveConstruction | ✅ PASS | 0.000s |
| 57 | FreeRTOSTaskTest.TaskExecutionDirectCall | ✅ PASS | 0.000s |
| 58 | FreeRTOSTaskTest.PeriodicTaskTypo | ✅ PASS | 0.000s |
| 59 | FreeRTOSTaskTest.TaskTemplateInstantiation | ✅ PASS | 0.000s |
| 60 | FreeRTOSTaskTest.DynamicTaskDifferentSizes | ✅ PASS | 0.000s |
| 61 | FreeRTOSTaskTest.TaskNotificationEdgeCases | ✅ PASS | 0.000s |
| 62 | FreeRTOSTaskTest.TaskSystemStatusTemplate | ✅ PASS | 0.000s |
| 63 | FreeRTOSTaskTest.DelayFunctionsWithDifferentDurations | ✅ PASS | 0.000s |
| 64 | FreeRTOSTaskTest.DelayUntilWithPeriodReference | ✅ PASS | 0.000s |
| 65 | FreeRTOSTaskTest.RacingConditionTaskConstructorInitialization | ✅ PASS | 0.000s |
| 66 | FreeRTOSTaskTest.ConcurrentTaskCreationAndDestruction | ✅ PASS | 0.000s |
| 67 | FreeRTOSTaskTest.MoveSemanticsRacingConditions | ✅ PASS | 0.000s |
| 68 | FreeRTOSTaskTest.PeriodicTaskLifecycleRacingConditions | ✅ PASS | 0.000s |
| 69 | FreeRTOSTaskTest.NotificationRacingConditions | ✅ PASS | 0.000s |
| 70 | FreeRTOSTaskTest.ComplexMultitaskingScenario | ✅ PASS | 0.000s |
| 71 | FreeRTOSTaskTest.TaskSystemStatusUnderLoad | ✅ PASS | 0.000s |
| 72 | FreeRTOSTaskTest.ConstructorInitializationOrderRaceCondition | ✅ PASS | 0.000s |
| 73 | FreeRTOSTaskTest.TaskExecutionInternalFunction | ✅ PASS | 0.000s |
| 74 | FreeRTOSTaskTest.PeriodicTaskRunMethodExecution | ✅ PASS | 0.000s |
| 75 | FreeRTOSTaskTest.YieldFunctionExecution | ✅ PASS | 0.000s |
| 76 | FreeRTOSTaskTest.CriticalSectionAndBarrierUtilities | ✅ PASS | 0.000s |
| 77 | FreeRTOSTaskTest.TaskSystemStatusCoverage | ✅ PASS | 0.000s |
| 78 | FreeRTOSTaskTest.AdvancedRacingConditionScenarios | ✅ PASS | 0.000s |
| 79 | FreeRTOSTaskTest.EdgeCaseErrorHandling | ✅ PASS | 0.000s |
| 80 | FreeRTOSTaskTest.AdvancedChronoCompatibility | ✅ PASS | 0.000s |
| 81 | FreeRTOSTaskTest.PriorityInheritanceScenario | ✅ PASS | 0.000s |
| 371 | EnhancedMultitaskingTest.TaskActuallyExecutes | ✅ PASS | 0.050s |
| 372 | EnhancedMultitaskingTest.TaskExecutionWithSuspendResume | ✅ PASS | 0.060s |
| 373 | EnhancedMultitaskingTest.MultipleTasksConcurrentExecution | ✅ PASS | 0.100s |
| 374 | EnhancedMultitaskingTest.TaskSynchronizationWithNotifications | ✅ PASS | 0.050s |
| 375 | EnhancedMultitaskingTest.TaskLifecycleRacingConditions | ✅ PASS | 0.032s |
| 376 | EnhancedMultitaskingTest.TaskMoveSemanticsConcurrency | ✅ PASS | 0.035s |
| 377 | EnhancedMultitaskingTest.PeriodicTaskExecution | ✅ PASS | 0.040s |
| 378 | EnhancedMultitaskingTest.MultiplePeriodicTasksCoordination | ✅ PASS | 0.097s |
| 379 | EnhancedMultitaskingTest.TaskExceptionHandling | ✅ PASS | 0.020s |
| 380 | EnhancedMultitaskingTest.TaskDeleteDuringExecution | ✅ PASS | 0.106s |
| 381 | EnhancedMultitaskingTest.TaskExecFunctionCoverage | ✅ PASS | 0.010s |
| 382 | EnhancedMultitaskingTest.SuspendedTaskStartupCoverage | ✅ PASS | 0.015s |

### Semaphore Module Tests

**Module Statistics:**
- Tests: 96
- Passed: 96
- Failed: 0
- Success Rate: 100.0%
- Total Time: 1.283s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 82 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorConstruction | ✅ PASS | 0.000s |
| 83 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateBinary | ✅ PASS | 0.000s |
| 84 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateCounting | ✅ PASS | 0.000s |
| 85 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateMutex | ✅ PASS | 0.000s |
| 86 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateRecursiveMutex | ✅ PASS | 0.000s |
| 87 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorConstruction | ✅ PASS | 0.000s |
| 88 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateBinary | ✅ PASS | 0.000s |
| 89 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateCounting | ✅ PASS | 0.000s |
| 90 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateMutex | ✅ PASS | 0.000s |
| 91 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateRecursiveMutex | ✅ PASS | 0.000s |
| 92 | FreeRTOSSemaphoreTest.BinarySemaphoreConstruction | ✅ PASS | 0.000s |
| 93 | FreeRTOSSemaphoreTest.BinarySemaphoreDestruction | ✅ PASS | 0.000s |
| 94 | FreeRTOSSemaphoreTest.BinarySemaphoreDestructionNullHandle | ✅ PASS | 0.000s |
| 95 | FreeRTOSSemaphoreTest.BinarySemaphoreGive | ✅ PASS | 0.000s |
| 96 | FreeRTOSSemaphoreTest.BinarySemaphoreTake | ✅ PASS | 0.000s |
| 97 | FreeRTOSSemaphoreTest.BinarySemaphoreTakeWithTimeout | ✅ PASS | 0.000s |
| 98 | FreeRTOSSemaphoreTest.BinarySemaphoreISROperations | ✅ PASS | 0.000s |
| 99 | FreeRTOSSemaphoreTest.BinarySemaphoreChronoTimeout | ✅ PASS | 0.000s |
| 100 | FreeRTOSSemaphoreTest.CountingSemaphoreConstruction | ✅ PASS | 0.000s |
| 101 | FreeRTOSSemaphoreTest.CountingSemaphoreDefaultConstruction | ✅ PASS | 0.000s |
| 102 | FreeRTOSSemaphoreTest.CountingSemaphoreOperators | ✅ PASS | 0.000s |
| 103 | FreeRTOSSemaphoreTest.CountingSemaphoreGetCount | ✅ PASS | 0.000s |
| 104 | FreeRTOSSemaphoreTest.MutexConstruction | ✅ PASS | 0.000s |
| 105 | FreeRTOSSemaphoreTest.MutexLockUnlock | ✅ PASS | 0.000s |
| 106 | FreeRTOSSemaphoreTest.MutexTryLock | ✅ PASS | 0.000s |
| 107 | FreeRTOSSemaphoreTest.MutexLockWithTimeout | ✅ PASS | 0.000s |
| 108 | FreeRTOSSemaphoreTest.MutexISROperations | ✅ PASS | 0.000s |
| 109 | FreeRTOSSemaphoreTest.RecursiveMutexConstruction | ✅ PASS | 0.000s |
| 110 | FreeRTOSSemaphoreTest.RecursiveMutexLockUnlock | ✅ PASS | 0.000s |
| 111 | FreeRTOSSemaphoreTest.RecursiveMutexTryLock | ✅ PASS | 0.000s |
| 112 | FreeRTOSSemaphoreTest.RecursiveMutexNestedLocks | ✅ PASS | 0.000s |
| 113 | FreeRTOSSemaphoreTest.RecursiveMutexNestedFunction | ✅ PASS | 0.000s |
| 114 | FreeRTOSSemaphoreTest.RecursiveMutexLockGuardRAII | ✅ PASS | 0.000s |
| 115 | FreeRTOSSemaphoreTest.RecursiveMutexNestedLockGuards | ✅ PASS | 0.000s |
| 116 | FreeRTOSSemaphoreTest.RecursiveMutexTryLockGuardRAII | ✅ PASS | 0.000s |
| 117 | FreeRTOSSemaphoreTest.RecursiveMutexTryLockGuardFailure | ✅ PASS | 0.000s |
| 118 | FreeRTOSSemaphoreTest.RecursiveMutexNestedTryLockGuards | ✅ PASS | 0.000s |
| 119 | FreeRTOSSemaphoreTest.RecursiveMutexLockGuardISRRAII | ✅ PASS | 0.000s |
| 120 | FreeRTOSSemaphoreTest.RecursiveMutexNestedLockGuardISR | ✅ PASS | 0.000s |
| 121 | FreeRTOSSemaphoreTest.RecursiveMutexTimeoutLockGuardRAII | ✅ PASS | 0.000s |
| 122 | FreeRTOSSemaphoreTest.RecursiveMutexTimeoutLockGuardChrono | ✅ PASS | 0.000s |
| 123 | FreeRTOSSemaphoreTest.RecursiveMutexNestedTimeoutLockGuards | ✅ PASS | 0.000s |
| 124 | FreeRTOSSemaphoreTest.RecursiveMutexMixedLockGuardTypes | ✅ PASS | 0.000s |
| 125 | FreeRTOSSemaphoreTest.RecursiveMutexTimeoutLockGuardFailure | ✅ PASS | 0.000s |
| 126 | FreeRTOSSemaphoreTest.RecursiveMutexTryLockGuardAcquiredStateTrackingFailure | ✅ PASS | 0.000s |
| 127 | FreeRTOSSemaphoreTest.LockGuardRAII | ✅ PASS | 0.000s |
| 128 | FreeRTOSSemaphoreTest.TryLockGuardRAII | ✅ PASS | 0.000s |
| 129 | FreeRTOSSemaphoreTest.TryLockGuardFailure | ✅ PASS | 0.000s |
| 130 | FreeRTOSSemaphoreTest.LockGuardISRRAII | ✅ PASS | 0.000s |
| 131 | FreeRTOSSemaphoreTest.TimeoutLockGuardRAII | ✅ PASS | 0.000s |
| 132 | FreeRTOSSemaphoreTest.TimeoutLockGuardChronoRAII | ✅ PASS | 0.000s |
| 133 | FreeRTOSSemaphoreTest.TimeoutLockGuardFailure | ✅ PASS | 0.000s |
| 134 | FreeRTOSSemaphoreTest.TryLockGuardAcquiredStateTrackingSuccess | ✅ PASS | 0.000s |
| 135 | FreeRTOSSemaphoreTest.TryLockGuardAcquiredStateTrackingFailure | ✅ PASS | 0.000s |
| 136 | FreeRTOSSemaphoreTest.TimeoutLockGuardAcquiredStateTrackingSuccess | ✅ PASS | 0.000s |
| 137 | FreeRTOSSemaphoreTest.TimeoutLockGuardAcquiredStateTrackingFailure | ✅ PASS | 0.000s |
| 138 | FreeRTOSSemaphoreTest.LockGuardStateTrackingRaceConditionProtection | ✅ PASS | 0.000s |
| 139 | FreeRTOSSemaphoreTest.RecursiveMutexLockGuardAcquiredStateTracking | ✅ PASS | 0.000s |
| 140 | FreeRTOSSemaphoreTest.SemaphoreCreationFailure | ✅ PASS | 0.000s |
| 141 | FreeRTOSSemaphoreTest.StaticAllocationLimitation | ✅ PASS | 0.000s |
| 142 | FreeRTOSSemaphoreTest.MutexLockFailure | ✅ PASS | 0.000s |
| 143 | FreeRTOSSemaphoreTest.LargeCountingSemaphore | ✅ PASS | 0.000s |
| 144 | FreeRTOSSemaphoreTest.ZeroCountingSemaphore | ✅ PASS | 0.000s |
| 145 | FreeRTOSSemaphoreTest.StaticVsDynamicBehavior | ✅ PASS | 0.000s |
| 146 | FreeRTOSSemaphoreTest.StaticAliasNamespace | ✅ PASS | 0.000s |
| 147 | FreeRTOSSemaphoreTest.DynamicAliasNamespace | ✅ PASS | 0.000s |
| 148 | FreeRTOSSemaphoreTest.NamespaceAliasCountingSemaphore | ✅ PASS | 0.000s |
| 149 | FreeRTOSSemaphoreTest.NamespaceAliasMutex | ✅ PASS | 0.000s |
| 150 | FreeRTOSSemaphoreTest.NamespaceAliasRecursiveMutex | ✅ PASS | 0.000s |
| 151 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountBasic | ✅ PASS | 0.000s |
| 152 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountTryLock | ✅ PASS | 0.000s |
| 153 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountISR | ✅ PASS | 0.000s |
| 154 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountFailureScenarios | ✅ PASS | 0.000s |
| 155 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountEdgeCases | ✅ PASS | 0.000s |
| 156 | FreeRTOSSemaphoreTest.CountingSemaphoreMultipleOperators | ✅ PASS | 0.000s |
| 157 | FreeRTOSSemaphoreTest.MutexUnlockWithoutLock | ✅ PASS | 0.000s |
| 158 | FreeRTOSSemaphoreTest.BinarySemaphoreMultipleGive | ✅ PASS | 0.000s |
| 159 | FreeRTOSSemaphoreTest.BinarySemaphoreTakeUntaken | ✅ PASS | 0.000s |
| 160 | FreeRTOSSemaphoreTest.CountingSemaphoreEdgeCounts | ✅ PASS | 0.000s |
| 161 | FreeRTOSSemaphoreTest.ChronoTimeoutCompatibility | ✅ PASS | 0.000s |
| 162 | FreeRTOSSemaphoreTest.SemaphoreAPICompleteness | ✅ PASS | 0.000s |
| 383 | STLSemaphoreTest.STLBinarySemaphoreBasicOperations | ✅ PASS | 0.000s |
| 384 | STLSemaphoreTest.STLBinarySemaphoreTimeout | ✅ PASS | 0.100s |
| 385 | STLSemaphoreTest.STLBinarySemaphoreProducerConsumer | ✅ PASS | 0.050s |
| 386 | STLSemaphoreTest.STLBinarySemaphoreMultipleProducers | ✅ PASS | 0.040s |
| 387 | STLSemaphoreTest.STLCountingSemaphoreBasicOperations | ✅ PASS | 0.000s |
| 388 | STLSemaphoreTest.STLCountingSemaphoreMaxLimit | ✅ PASS | 0.000s |
| 389 | STLSemaphoreTest.STLCountingSemaphoreResourcePool | ✅ PASS | 0.100s |
| 390 | STLSemaphoreTest.STLMutexBasicOperations | ✅ PASS | 0.000s |
| 391 | STLSemaphoreTest.STLMutexCriticalSection | ✅ PASS | 0.031s |
| 392 | STLSemaphoreTest.STLRecursiveMutexBasicOperations | ✅ PASS | 0.000s |
| 393 | STLSemaphoreTest.STLRecursiveMutexNestedLocking | ✅ PASS | 0.000s |
| 394 | STLSemaphoreTest.STLProducerConsumerWithBuffering | ✅ PASS | 0.101s |
| 395 | STLSemaphoreTest.STLReaderWriterPattern | ✅ PASS | 0.434s |
| 396 | STLSemaphoreTest.STLSemaphoreStressTest | ✅ PASS | 0.077s |
| 397 | STLSemaphoreTest.STLTimeoutAccuracy | ✅ PASS | 0.350s |

### Queue Module Tests

**Module Statistics:**
- Tests: 49
- Passed: 49
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.000s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 163 | FreeRTOSQueueTest.StaticQueueAllocatorConstruction | ✅ PASS | 0.000s |
| 164 | FreeRTOSQueueTest.StaticQueueAllocatorCreate | ✅ PASS | 0.000s |
| 165 | FreeRTOSQueueTest.StaticQueueAllocatorDifferentTypes | ✅ PASS | 0.000s |
| 166 | FreeRTOSQueueTest.DynamicQueueAllocatorConstruction | ✅ PASS | 0.000s |
| 167 | FreeRTOSQueueTest.DynamicQueueAllocatorCreate | ✅ PASS | 0.000s |
| 168 | FreeRTOSQueueTest.QueueConstructionWithoutRegistry | ✅ PASS | 0.000s |
| 169 | FreeRTOSQueueTest.QueueConstructionWithRegistry | ✅ PASS | 0.000s |
| 170 | FreeRTOSQueueTest.QueueDestructionNullHandle | ✅ PASS | 0.000s |
| 171 | FreeRTOSQueueTest.QueueSendBasic | ✅ PASS | 0.000s |
| 172 | FreeRTOSQueueTest.QueueSendBack | ✅ PASS | 0.000s |
| 173 | FreeRTOSQueueTest.QueueSendFront | ✅ PASS | 0.000s |
| 174 | FreeRTOSQueueTest.QueueSendTimeout | ✅ PASS | 0.000s |
| 175 | FreeRTOSQueueTest.QueueSendISR | ✅ PASS | 0.000s |
| 176 | FreeRTOSQueueTest.QueueSendChronoTimeout | ✅ PASS | 0.000s |
| 177 | FreeRTOSQueueTest.QueueReceiveReference | ✅ PASS | 0.000s |
| 178 | FreeRTOSQueueTest.QueueReceiveOptional | ✅ PASS | 0.000s |
| 179 | FreeRTOSQueueTest.QueueReceiveOptionalEmpty | ✅ PASS | 0.000s |
| 180 | FreeRTOSQueueTest.QueueReceiveISR | ✅ PASS | 0.000s |
| 181 | FreeRTOSQueueTest.QueueReceiveChronoTimeout | ✅ PASS | 0.000s |
| 182 | FreeRTOSQueueTest.QueuePeekReference | ✅ PASS | 0.000s |
| 183 | FreeRTOSQueueTest.QueuePeekOptional | ✅ PASS | 0.000s |
| 184 | FreeRTOSQueueTest.QueuePeekISR | ✅ PASS | 0.000s |
| 185 | FreeRTOSQueueTest.QueueMessagesWaiting | ✅ PASS | 0.000s |
| 186 | FreeRTOSQueueTest.QueueSpacesAvailable | ✅ PASS | 0.000s |
| 187 | FreeRTOSQueueTest.QueueIsEmpty | ✅ PASS | 0.000s |
| 188 | FreeRTOSQueueTest.QueueIsFull | ✅ PASS | 0.000s |
| 189 | FreeRTOSQueueTest.QueueGetName | ✅ PASS | 0.000s |
| 190 | FreeRTOSQueueTest.QueueReset | ✅ PASS | 0.000s |
| 191 | FreeRTOSQueueTest.QueueOverwrite | ✅ PASS | 0.000s |
| 192 | FreeRTOSQueueTest.QueueOverwriteISR | ✅ PASS | 0.000s |
| 193 | FreeRTOSQueueTest.StaticQueueAliasUsage | ✅ PASS | 0.000s |
| 194 | FreeRTOSQueueTest.DynamicQueueAliasUsage | ✅ PASS | 0.000s |
| 195 | FreeRTOSQueueTest.QueueCreationFailure | ✅ PASS | 0.000s |
| 196 | FreeRTOSQueueTest.LargeQueueSize | ✅ PASS | 0.000s |
| 197 | FreeRTOSQueueTest.ZeroSizeQueue | ✅ PASS | 0.000s |
| 198 | FreeRTOSQueueTest.ComplexDataTypeQueue | ✅ PASS | 0.000s |
| 199 | FreeRTOSQueueTest.QueueAPICompleteness | ✅ PASS | 0.000s |
| 200 | FreeRTOSQueueTest.QueueWithVariousDataTypes | ✅ PASS | 0.000s |
| 201 | FreeRTOSQueueTest.QueueWithEnumTypes | ✅ PASS | 0.000s |
| 202 | FreeRTOSQueueTest.QueueWithPointerTypes | ✅ PASS | 0.000s |
| 203 | FreeRTOSQueueTest.QueueChronoMicrosecondsTimeout | ✅ PASS | 0.000s |
| 204 | FreeRTOSQueueTest.QueueChronoMinutesTimeout | ✅ PASS | 0.000s |
| 205 | FreeRTOSQueueTest.QueueChronoNanosecondsTimeout | ✅ PASS | 0.000s |
| 206 | FreeRTOSQueueTest.QueueChronoOptionalReceiveVariousDurations | ✅ PASS | 0.000s |
| 207 | FreeRTOSQueueTest.StaticQueueAllocatorWithVariousTypes | ✅ PASS | 0.000s |
| 208 | FreeRTOSQueueTest.QueueISROperationsWithDifferentTypes | ✅ PASS | 0.000s |
| 209 | FreeRTOSQueueTest.QueueSendBackISROperations | ✅ PASS | 0.000s |
| 210 | FreeRTOSQueueTest.QueueSendFrontISROperations | ✅ PASS | 0.000s |
| 211 | FreeRTOSQueueTest.QueueMessagesWaitingISR | ✅ PASS | 0.000s |

### EventGroup Module Tests

**Module Statistics:**
- Tests: 30
- Passed: 30
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.000s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 212 | FreeRTOSEventGroupTest.StaticEventGroupAllocatorConstruction | ✅ PASS | 0.000s |
| 213 | FreeRTOSEventGroupTest.StaticEventGroupAllocatorCreate | ✅ PASS | 0.000s |
| 214 | FreeRTOSEventGroupTest.StaticEventGroupAllocatorCreateFailure | ✅ PASS | 0.000s |
| 215 | FreeRTOSEventGroupTest.DynamicEventGroupAllocatorConstruction | ✅ PASS | 0.000s |
| 216 | FreeRTOSEventGroupTest.DynamicEventGroupAllocatorCreate | ✅ PASS | 0.000s |
| 217 | FreeRTOSEventGroupTest.DynamicEventGroupAllocatorCreateFailure | ✅ PASS | 0.000s |
| 218 | FreeRTOSEventGroupTest.EventGroupConstruction | ✅ PASS | 0.000s |
| 219 | FreeRTOSEventGroupTest.EventGroupDestructionNullHandle | ✅ PASS | 0.000s |
| 220 | FreeRTOSEventGroupTest.StaticEventGroupConstruction | ✅ PASS | 0.000s |
| 221 | FreeRTOSEventGroupTest.EventGroupSetBits | ✅ PASS | 0.000s |
| 222 | FreeRTOSEventGroupTest.EventGroupSetBitsISR | ✅ PASS | 0.000s |
| 223 | FreeRTOSEventGroupTest.EventGroupClearBits | ✅ PASS | 0.000s |
| 224 | FreeRTOSEventGroupTest.EventGroupGetBits | ✅ PASS | 0.000s |
| 225 | FreeRTOSEventGroupTest.EventGroupGetBitsISR | ✅ PASS | 0.000s |
| 226 | FreeRTOSEventGroupTest.EventGroupWaitBitsAnyBits | ✅ PASS | 0.000s |
| 227 | FreeRTOSEventGroupTest.EventGroupWaitBitsAllBits | ✅ PASS | 0.000s |
| 228 | FreeRTOSEventGroupTest.EventGroupWaitBitsTimeout | ✅ PASS | 0.000s |
| 229 | FreeRTOSEventGroupTest.EventGroupWaitBitsChronoTimeout | ✅ PASS | 0.000s |
| 230 | FreeRTOSEventGroupTest.EventGroupSync | ✅ PASS | 0.000s |
| 231 | FreeRTOSEventGroupTest.EventGroupSyncChronoTimeout | ✅ PASS | 0.000s |
| 232 | FreeRTOSEventGroupTest.EventGroupSyncTimeout | ✅ PASS | 0.000s |
| 233 | FreeRTOSEventGroupTest.EventGroupChronoMicrosecondsTimeout | ✅ PASS | 0.000s |
| 234 | FreeRTOSEventGroupTest.EventGroupChronoNanosecondsTimeout | ✅ PASS | 0.000s |
| 235 | FreeRTOSEventGroupTest.EventGroupChronoMinutesTimeout | ✅ PASS | 0.000s |
| 236 | FreeRTOSEventGroupTest.EventGroupComplexScenario | ✅ PASS | 0.000s |
| 237 | FreeRTOSEventGroupTest.EventGroupCreationFailure | ✅ PASS | 0.000s |
| 238 | FreeRTOSEventGroupTest.EventGroupAllBitsPattern | ✅ PASS | 0.000s |
| 239 | FreeRTOSEventGroupTest.EventGroupZeroBitsPattern | ✅ PASS | 0.000s |
| 240 | FreeRTOSEventGroupTest.StaticVsDynamicBehavior | ✅ PASS | 0.000s |
| 241 | FreeRTOSEventGroupTest.EventGroupAPICompleteness | ✅ PASS | 0.000s |

### StreamBuffer Module Tests

**Module Statistics:**
- Tests: 47
- Passed: 47
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.000s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 242 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorConstruction | ✅ PASS | 0.000s |
| 243 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorCreate | ✅ PASS | 0.000s |
| 244 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorCreateWithTriggerLevel | ✅ PASS | 0.000s |
| 245 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorCreateFailure | ✅ PASS | 0.000s |
| 246 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorConstruction | ✅ PASS | 0.000s |
| 247 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorCreate | ✅ PASS | 0.000s |
| 248 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorCreateWithTriggerLevel | ✅ PASS | 0.000s |
| 249 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorCreateFailure | ✅ PASS | 0.000s |
| 250 | FreeRTOSStreamBufferTest.StaticStreamBufferConstruction | ✅ PASS | 0.000s |
| 251 | FreeRTOSStreamBufferTest.DynamicStreamBufferConstruction | ✅ PASS | 0.000s |
| 252 | FreeRTOSStreamBufferTest.StreamBufferConstructionWithTriggerLevel | ✅ PASS | 0.000s |
| 253 | FreeRTOSStreamBufferTest.StreamBufferDestructionNullHandle | ✅ PASS | 0.000s |
| 254 | FreeRTOSStreamBufferTest.StreamBufferSendBasic | ✅ PASS | 0.000s |
| 255 | FreeRTOSStreamBufferTest.StreamBufferSendWithTimeout | ✅ PASS | 0.000s |
| 256 | FreeRTOSStreamBufferTest.StreamBufferSendChronoTimeout | ✅ PASS | 0.000s |
| 257 | FreeRTOSStreamBufferTest.StreamBufferSendIterators | ✅ PASS | 0.000s |
| 258 | FreeRTOSStreamBufferTest.StreamBufferSendIteratorsWithTimeout | ✅ PASS | 0.000s |
| 259 | FreeRTOSStreamBufferTest.StreamBufferSendPartial | ✅ PASS | 0.000s |
| 260 | FreeRTOSStreamBufferTest.StreamBufferSendISR | ✅ PASS | 0.000s |
| 261 | FreeRTOSStreamBufferTest.StreamBufferSendISRWithoutWoken | ✅ PASS | 0.000s |
| 262 | FreeRTOSStreamBufferTest.StreamBufferSendISRIterators | ✅ PASS | 0.000s |
| 263 | FreeRTOSStreamBufferTest.StreamBufferReceiveBasic | ✅ PASS | 0.000s |
| 264 | FreeRTOSStreamBufferTest.StreamBufferReceiveWithTimeout | ✅ PASS | 0.000s |
| 265 | FreeRTOSStreamBufferTest.StreamBufferReceiveChronoTimeout | ✅ PASS | 0.000s |
| 266 | FreeRTOSStreamBufferTest.StreamBufferReceiveTimeout | ✅ PASS | 0.000s |
| 267 | FreeRTOSStreamBufferTest.StreamBufferReceiveISR | ✅ PASS | 0.000s |
| 268 | FreeRTOSStreamBufferTest.StreamBufferReceiveISRWithoutWoken | ✅ PASS | 0.000s |
| 269 | FreeRTOSStreamBufferTest.StreamBufferBytesAvailable | ✅ PASS | 0.000s |
| 270 | FreeRTOSStreamBufferTest.StreamBufferSpacesAvailable | ✅ PASS | 0.000s |
| 271 | FreeRTOSStreamBufferTest.StreamBufferIsEmpty | ✅ PASS | 0.000s |
| 272 | FreeRTOSStreamBufferTest.StreamBufferIsNotEmpty | ✅ PASS | 0.000s |
| 273 | FreeRTOSStreamBufferTest.StreamBufferIsFull | ✅ PASS | 0.000s |
| 274 | FreeRTOSStreamBufferTest.StreamBufferIsNotFull | ✅ PASS | 0.000s |
| 275 | FreeRTOSStreamBufferTest.StreamBufferReset | ✅ PASS | 0.000s |
| 276 | FreeRTOSStreamBufferTest.StreamBufferResetFailure | ✅ PASS | 0.000s |
| 277 | FreeRTOSStreamBufferTest.StreamBufferSetTriggerLevel | ✅ PASS | 0.000s |
| 278 | FreeRTOSStreamBufferTest.StreamBufferSetTriggerLevelFailure | ✅ PASS | 0.000s |
| 279 | FreeRTOSStreamBufferTest.StreamBufferGetHandle | ✅ PASS | 0.000s |
| 280 | FreeRTOSStreamBufferTest.StreamBufferCreationFailure | ✅ PASS | 0.000s |
| 281 | FreeRTOSStreamBufferTest.StreamBufferSendZeroBytes | ✅ PASS | 0.000s |
| 282 | FreeRTOSStreamBufferTest.StreamBufferReceiveZeroBytes | ✅ PASS | 0.000s |
| 283 | FreeRTOSStreamBufferTest.StreamBufferComplexScenario | ✅ PASS | 0.000s |
| 284 | FreeRTOSStreamBufferTest.StaticVsDynamicBehavior | ✅ PASS | 0.000s |
| 285 | FreeRTOSStreamBufferTest.StreamBufferChronoMicrosecondsTimeout | ✅ PASS | 0.000s |
| 286 | FreeRTOSStreamBufferTest.StreamBufferChronoNanosecondsTimeout | ✅ PASS | 0.000s |
| 287 | FreeRTOSStreamBufferTest.StreamBufferChronoSecondsTimeout | ✅ PASS | 0.000s |
| 288 | FreeRTOSStreamBufferTest.StreamBufferAPICompleteness | ✅ PASS | 0.000s |

### MessageBuffer Module Tests

**Module Statistics:**
- Tests: 32
- Passed: 32
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.000s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 289 | FreeRTOSMessageBufferTest.StaticMessageBufferAllocatorConstruction | ✅ PASS | 0.000s |
| 290 | FreeRTOSMessageBufferTest.StaticMessageBufferAllocatorCreate | ✅ PASS | 0.000s |
| 291 | FreeRTOSMessageBufferTest.StaticMessageBufferAllocatorCreateFailure | ✅ PASS | 0.000s |
| 292 | FreeRTOSMessageBufferTest.DynamicMessageBufferAllocatorConstruction | ✅ PASS | 0.000s |
| 293 | FreeRTOSMessageBufferTest.DynamicMessageBufferAllocatorCreateSuccess | ✅ PASS | 0.000s |
| 294 | FreeRTOSMessageBufferTest.DynamicMessageBufferAllocatorCreateFailure | ✅ PASS | 0.000s |
| 295 | FreeRTOSMessageBufferTest.StaticMessageBufferConstruction | ✅ PASS | 0.000s |
| 296 | FreeRTOSMessageBufferTest.StaticMessageBufferDestruction | ✅ PASS | 0.000s |
| 297 | FreeRTOSMessageBufferTest.StaticMessageBufferDestructionNullHandle | ✅ PASS | 0.000s |
| 298 | FreeRTOSMessageBufferTest.DynamicMessageBufferConstruction | ✅ PASS | 0.000s |
| 299 | FreeRTOSMessageBufferTest.DynamicMessageBufferConstructionFailure | ✅ PASS | 0.000s |
| 300 | FreeRTOSMessageBufferTest.MessageBufferSendSuccess | ✅ PASS | 0.000s |
| 301 | FreeRTOSMessageBufferTest.MessageBufferSendTimeout | ✅ PASS | 0.000s |
| 302 | FreeRTOSMessageBufferTest.MessageBufferSendWithChrono | ✅ PASS | 0.000s |
| 303 | FreeRTOSMessageBufferTest.MessageBufferSendZeroLengthMessage | ✅ PASS | 0.000s |
| 304 | FreeRTOSMessageBufferTest.MessageBufferSendMaxSizeMessage | ✅ PASS | 0.000s |
| 305 | FreeRTOSMessageBufferTest.MessageBufferReceiveSuccess | ✅ PASS | 0.000s |
| 306 | FreeRTOSMessageBufferTest.MessageBufferReceiveTimeout | ✅ PASS | 0.000s |
| 307 | FreeRTOSMessageBufferTest.MessageBufferReceiveWithChrono | ✅ PASS | 0.000s |
| 308 | FreeRTOSMessageBufferTest.MessageBufferReceiveBufferTooSmall | ✅ PASS | 0.000s |
| 309 | FreeRTOSMessageBufferTest.MessageBufferAvailableSpace | ✅ PASS | 0.000s |
| 310 | FreeRTOSMessageBufferTest.MessageBufferIsEmpty | ✅ PASS | 0.000s |
| 311 | FreeRTOSMessageBufferTest.MessageBufferIsFull | ✅ PASS | 0.000s |
| 312 | FreeRTOSMessageBufferTest.MessageBufferReset | ✅ PASS | 0.000s |
| 313 | FreeRTOSMessageBufferTest.MessageBufferCreationFailure | ✅ PASS | 0.000s |
| 314 | FreeRTOSMessageBufferTest.MessageBufferZeroSizeBuffer | ✅ PASS | 0.000s |
| 315 | FreeRTOSMessageBufferTest.MessageBufferVeryLargeBuffer | ✅ PASS | 0.000s |
| 316 | FreeRTOSMessageBufferTest.MessageBufferChronoMicrosecondsTimeout | ✅ PASS | 0.000s |
| 317 | FreeRTOSMessageBufferTest.MessageBufferChronoSecondsTimeout | ✅ PASS | 0.000s |
| 318 | FreeRTOSMessageBufferTest.StaticVsDynamicBehaviorComparison | ✅ PASS | 0.000s |
| 319 | FreeRTOSMessageBufferTest.MessageBufferComplexSendReceiveScenario | ✅ PASS | 0.000s |
| 320 | FreeRTOSMessageBufferTest.MessageBufferAPICompleteness | ✅ PASS | 0.000s |

### Timer Module Tests

**Module Statistics:**
- Tests: 50
- Passed: 50
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.000s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 321 | FreeRTOSSwTimerTest.StaticTimerAllocatorConstruction | ✅ PASS | 0.000s |
| 322 | FreeRTOSSwTimerTest.StaticTimerAllocatorCreate | ✅ PASS | 0.000s |
| 323 | FreeRTOSSwTimerTest.StaticTimerAllocatorCreateNullReturn | ✅ PASS | 0.000s |
| 324 | FreeRTOSSwTimerTest.DynamicTimerAllocatorConstruction | ✅ PASS | 0.000s |
| 325 | FreeRTOSSwTimerTest.DynamicTimerAllocatorCreate | ✅ PASS | 0.000s |
| 326 | FreeRTOSSwTimerTest.DynamicTimerAllocatorCreateNullReturn | ✅ PASS | 0.000s |
| 327 | FreeRTOSSwTimerTest.StaticTimerConstruction | ✅ PASS | 0.000s |
| 328 | FreeRTOSSwTimerTest.StaticTimerConstructionWithChrono | ✅ PASS | 0.000s |
| 329 | FreeRTOSSwTimerTest.StaticTimerDestruction | ✅ PASS | 0.000s |
| 330 | FreeRTOSSwTimerTest.StaticTimerDestructionNullHandle | ✅ PASS | 0.000s |
| 331 | FreeRTOSSwTimerTest.StaticTimerStart | ✅ PASS | 0.000s |
| 332 | FreeRTOSSwTimerTest.StaticTimerStartWithTimeout | ✅ PASS | 0.000s |
| 333 | FreeRTOSSwTimerTest.StaticTimerStartWithChrono | ✅ PASS | 0.000s |
| 334 | FreeRTOSSwTimerTest.StaticTimerStop | ✅ PASS | 0.000s |
| 335 | FreeRTOSSwTimerTest.StaticTimerReset | ✅ PASS | 0.000s |
| 336 | FreeRTOSSwTimerTest.StaticTimerPeriodChange | ✅ PASS | 0.000s |
| 337 | FreeRTOSSwTimerTest.StaticTimerIsRunning | ✅ PASS | 0.000s |
| 338 | FreeRTOSSwTimerTest.StaticTimerGetPeriod | ✅ PASS | 0.000s |
| 339 | FreeRTOSSwTimerTest.StaticTimerGetName | ✅ PASS | 0.000s |
| 340 | FreeRTOSSwTimerTest.StaticTimerReloadMode | ✅ PASS | 0.000s |
| 341 | FreeRTOSSwTimerTest.DynamicTimerConstruction | ✅ PASS | 0.000s |
| 342 | FreeRTOSSwTimerTest.DynamicTimerConstructionFailure | ✅ PASS | 0.000s |
| 343 | FreeRTOSSwTimerTest.StaticTimerStartFromISR | ✅ PASS | 0.000s |
| 344 | FreeRTOSSwTimerTest.StaticTimerStartFromISRNoParam | ✅ PASS | 0.000s |
| 345 | FreeRTOSSwTimerTest.StaticTimerStopFromISR | ✅ PASS | 0.000s |
| 346 | FreeRTOSSwTimerTest.StaticTimerResetFromISR | ✅ PASS | 0.000s |
| 347 | FreeRTOSSwTimerTest.StaticTimerPeriodFromISR | ✅ PASS | 0.000s |
| 348 | FreeRTOSSwTimerTest.StaticTimerRemainingTime | ✅ PASS | 0.000s |
| 349 | FreeRTOSSwTimerTest.StaticTimerRemainingTimeNullHandle | ✅ PASS | 0.000s |
| 350 | FreeRTOSSwTimerTest.TimerOperationsWithNullHandle | ✅ PASS | 0.000s |
| 351 | FreeRTOSSwTimerTest.TimerFailureConditions | ✅ PASS | 0.000s |
| 352 | FreeRTOSSwTimerTest.TimerMoveConstructionIssueScenario | ✅ PASS | 0.000s |
| 353 | FreeRTOSSwTimerTest.TimerMoveConstruction | ✅ PASS | 0.000s |
| 354 | FreeRTOSSwTimerTest.MovedFromTimerIsInvalidated | ✅ PASS | 0.000s |
| 355 | FreeRTOSSwTimerTest.TimerMoveAssignment | ✅ PASS | 0.000s |
| 356 | FreeRTOSSwTimerTest.ChronoCompatibility | ✅ PASS | 0.000s |
| 357 | FreeRTOSSwTimerTest.PeriodChangeWithMixedDurationTypes | ✅ PASS | 0.000s |
| 358 | FreeRTOSSwTimerTest.PeriodChangeWithVariousDurationCombinations | ✅ PASS | 0.000s |
| 359 | FreeRTOSSwTimerTest.PeriodChangeWithMicrosecondPrecision | ✅ PASS | 0.000s |
| 360 | FreeRTOSSwTimerTest.PeriodChangeWithMinutePrecision | ✅ PASS | 0.000s |
| 361 | FreeRTOSSwTimerTest.PeriodChangeWithFloatingPointDurations | ✅ PASS | 0.000s |
| 362 | FreeRTOSSwTimerTest.PeriodChangeEdgeCaseDurations | ✅ PASS | 0.000s |
| 363 | FreeRTOSSwTimerTest.ConstructorWithVariousDurationTypes | ✅ PASS | 0.000s |
| 364 | FreeRTOSSwTimerTest.StartStopResetWithMixedDurationTypes | ✅ PASS | 0.000s |
| 365 | FreeRTOSSwTimerTest.ISRFunctionsWithMixedDurationTypes | ✅ PASS | 0.000s |
| 366 | FreeRTOSSwTimerTest.SingleShotTimerBehavior | ✅ PASS | 0.000s |
| 367 | FreeRTOSSwTimerTest.AutoReloadTimerBehavior | ✅ PASS | 0.000s |
| 368 | FreeRTOSSwTimerTest.CallbackFunctionality | ✅ PASS | 0.000s |
| 369 | FreeRTOSSwTimerTest.DestructorComplexScenarios | ✅ PASS | 0.000s |
| 370 | FreeRTOSSwTimerTest.DestructorWithActiveTimer | ✅ PASS | 0.000s |

### EnhancedTimer Module Tests

**Module Statistics:**
- Tests: 18
- Passed: 18
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.002s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 422 | EnhancedFreeRTOSSwTimerTest.TimerCreationWithEnhancedMocks | ✅ PASS | 0.000s |
| 423 | EnhancedFreeRTOSSwTimerTest.SingleShotTimerRealBehavior | ✅ PASS | 0.000s |
| 424 | EnhancedFreeRTOSSwTimerTest.AutoReloadTimerRealBehavior | ✅ PASS | 0.000s |
| 425 | EnhancedFreeRTOSSwTimerTest.TimerResetRealBehavior | ✅ PASS | 0.000s |
| 426 | EnhancedFreeRTOSSwTimerTest.PeriodChangeWithRealBehavior | ✅ PASS | 0.000s |
| 427 | EnhancedFreeRTOSSwTimerTest.PeriodChangeWithChronoDurations | ✅ PASS | 0.000s |
| 428 | EnhancedFreeRTOSSwTimerTest.MultipleTimersCoordination | ✅ PASS | 0.000s |
| 429 | EnhancedFreeRTOSSwTimerTest.TimerInteraction | ✅ PASS | 0.000s |
| 430 | EnhancedFreeRTOSSwTimerTest.InvalidTimerOperations | ✅ PASS | 0.000s |
| 431 | EnhancedFreeRTOSSwTimerTest.ZeroPeriodTimer | ✅ PASS | 0.000s |
| 432 | EnhancedFreeRTOSSwTimerTest.NullCallbackTimer | ✅ PASS | 0.000s |
| 433 | EnhancedFreeRTOSSwTimerTest.TimingAccuracyVerification | ✅ PASS | 0.000s |
| 434 | EnhancedFreeRTOSSwTimerTest.ExpiryTimeCalculation | ✅ PASS | 0.000s |
| 435 | EnhancedFreeRTOSSwTimerTest.DynamicTimerEnhancedBehavior | ✅ PASS | 0.000s |
| 436 | EnhancedFreeRTOSSwTimerTest.ISRFunctionsEnhancedBehavior | ✅ PASS | 0.000s |
| 437 | EnhancedFreeRTOSSwTimerTest.CommandQueueProcessing | ✅ PASS | 0.000s |
| 438 | EnhancedFreeRTOSSwTimerTest.ComprehensiveIntegrationTest | ✅ PASS | 0.000s |
| 439 | EnhancedFreeRTOSSwTimerTest.ManyTimersStressTest | ✅ PASS | 0.002s |

### EnhancedFeatures Module Tests

**Module Statistics:**
- Tests: 13
- Passed: 13
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.000s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 409 | Cpp17FeaturesTest.TaskMoveSemanticsPerfectForwarding | ✅ PASS | 0.000s |
| 410 | Cpp17FeaturesTest.TaskMoveAssignmentChaining | ✅ PASS | 0.000s |
| 411 | Cpp17FeaturesTest.PeriodicTaskMoveWithChronoTypes | ✅ PASS | 0.000s |
| 412 | Cpp17FeaturesTest.LambdaCaptureVarieties | ✅ PASS | 0.000s |
| 413 | Cpp17FeaturesTest.FunctionObjectVarieties | ✅ PASS | 0.000s |
| 414 | Cpp17FeaturesTest.RAIIExceptionSafety | ✅ PASS | 0.000s |
| 415 | Cpp17FeaturesTest.CriticalSectionRAII | ✅ PASS | 0.000s |
| 416 | Cpp17FeaturesTest.TypeTraitsValidation | ✅ PASS | 0.000s |
| 417 | Cpp17FeaturesTest.TemplateDeductionGuides | ✅ PASS | 0.000s |
| 418 | Cpp17FeaturesTest.CompileTimeConstants | ✅ PASS | 0.000s |
| 419 | Cpp17FeaturesTest.ChronoTypesIntegration | ✅ PASS | 0.000s |
| 420 | Cpp17FeaturesTest.STLContainerIntegration | ✅ PASS | 0.000s |
| 421 | Cpp17FeaturesTest.UniquePtrIntegration | ✅ PASS | 0.000s |

### EnhancedTimeout Module Tests

**Module Statistics:**
- Tests: 11
- Passed: 11
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.000s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 398 | TimeoutPrecisionTest.MicrosecondDelayConversion | ✅ PASS | 0.000s |
| 399 | TimeoutPrecisionTest.NanosecondDelayConversion | ✅ PASS | 0.000s |
| 400 | TimeoutPrecisionTest.MixedDurationTypes | ✅ PASS | 0.000s |
| 401 | TimeoutPrecisionTest.ChronoBinarySemaphoreTimeout | ✅ PASS | 0.000s |
| 402 | TimeoutPrecisionTest.ChronoCountingSemaphoreTimeout | ✅ PASS | 0.000s |
| 403 | TimeoutPrecisionTest.ZeroDurationHandling | ✅ PASS | 0.000s |
| 404 | TimeoutPrecisionTest.MaximumDurationHandling | ✅ PASS | 0.000s |
| 405 | TimeoutPrecisionTest.DurationCastingAccuracy | ✅ PASS | 0.000s |
| 406 | TimeoutPrecisionTest.ConcurrentTimeoutOperations | ✅ PASS | 0.000s |
| 407 | TimeoutPrecisionTest.EdgeCaseDurations | ✅ PASS | 0.000s |
| 408 | TimeoutPrecisionTest.TickConversionConsistency | ✅ PASS | 0.000s |

## Code Coverage Analysis

### Coverage Overview
The project achieves excellent code coverage with **97.0% line coverage** and **98.2% function coverage**.

### Coverage Breakdown
- **Lines Covered**: 7395 out of 7620 total lines
- **Functions Covered**: 2902 out of 2955 total functions
- **Coverage Target**: Main library modules only (excludes test infrastructure and system headers)

**Detailed Uncovered Areas Analysis:**

The following sections provide specific references to uncovered code areas and explanations for why they cannot be covered by unit tests.

### Internal task execution functions called by FreeRTOS kernel

**Reason for exclusion:** These functions are called internally by the FreeRTOS kernel during task execution and cannot be directly invoked in unit tests

**Uncovered Area 1**: freertos_sw_timer.hpp:202
*Line*: 202

```cpp
    if (rc == pdPASS) {
    while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
>>> vTaskDelay(pdMS_TO_TICKS(1));
    }
    auto name = pcTimerGetName(src.m_timer);
```

**Uncovered Area 2**: freertos_sw_timer.hpp:210
*Line*: 210

```cpp
    if (rc == pdPASS) {
    while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
>>> vTaskDelay(pdMS_TO_TICKS(1));
    }
    src.m_timer = nullptr;
```


### Platform-specific or hardware-dependent code

**Reason for exclusion:** These code sections depend on specific hardware configurations or FreeRTOS kernel internals not available in the test environment

**Uncovered Area 3**: freertos_sw_timer.hpp:218
*Line*: 218

```cpp
    if (m_timer) {
    if (src.m_started) {
>>> rc = xTimerStart(m_timer, portMAX_DELAY);
    if (rc == pdPASS) {
    m_started = true;
```

**Uncovered Area 4**: freertos_sw_timer.hpp:219
*Line*: 219

```cpp
    if (src.m_started) {
    rc = xTimerStart(m_timer, portMAX_DELAY);
>>> if (rc == pdPASS) {
    m_started = true;
    }
```

**Uncovered Area 5**: freertos_sw_timer.hpp:220
*Line*: 220

```cpp
    rc = xTimerStart(m_timer, portMAX_DELAY);
    if (rc == pdPASS) {
>>> m_started = true;
    }
    }
```


### Defensive programming and robustness checks

**Reason for exclusion:** These are safety checks and defensive programming patterns that are difficult to trigger in controlled test conditions

**Uncovered Area 6**: freertos_task.hpp:275
*Function*: `freertos::task<freertos::static_task_allocator<0ul> >::suspend()`

```cpp
    *
    */
>>> void suspend(void) { vTaskSuspend(m_hTask); }
    /**
    * @brief Resume the task.
```

**Uncovered Area 7**: freertos_task.hpp:275
*Function*: `freertos::task<freertos::dynamic_task_allocator<2048ul> >::suspend()`

```cpp
    *
    */
>>> void suspend(void) { vTaskSuspend(m_hTask); }
    /**
    * @brief Resume the task.
```

**Uncovered Area 8**: freertos_task.hpp:616
*Function*: `freertos::periodic_task<freertos::static_task_allocator<1024ul> >::periodic_task<double, std::ratio<1l, 1000l> >(char const*, unsigned int, std::function<void ()>&&, std::function<void ()>&&, std::function<void ()>&&, std::chrono::duration<double, std::ratio<1l, 1000l> > const&, bool)::{lambda()#1}::operator()() const`

```cpp
    m_on_start{std::move(on_start)}, m_on_stop{std::move(on_stop)},
    m_periodic_routine{std::move(periodic_routine)},
>>> m_task{name, priority, [this]() { run(); }, start_suspended} {}
    /**
    * @brief Construct a new periodic task object
```

**Uncovered Area 9**: freertos_task.hpp:616
*Function*: `freertos::periodic_task<freertos::static_task_allocator<1024ul> >::periodic_task<long, std::ratio<1l, 1000l> >(char const*, unsigned int, std::function<void ()>&&, std::function<void ()>&&, std::function<void ()>&&, std::chrono::duration<long, std::ratio<1l, 1000l> > const&, bool)::{lambda()#1}::operator()() const`

```cpp
    m_on_start{std::move(on_start)}, m_on_stop{std::move(on_stop)},
    m_periodic_routine{std::move(periodic_routine)},
>>> m_task{name, priority, [this]() { run(); }, start_suspended} {}
    /**
    * @brief Construct a new periodic task object
```

**Uncovered Area 10**: freertos_queue.hpp:489
*Line*: 489

```cpp
    return item;
    }
>>> return {};
    }
    /**
```

**Uncovered Area 11**: freertos_queue.hpp:628
*Line*: 628

```cpp
    return item;
    }
>>> return {};
    }
    /**
```


**Summary:** 11 uncovered code areas identified across 3 categories.

**Note:** These uncovered areas represent code that by design cannot be easily tested in a unit test environment. They require either integration testing with the actual FreeRTOS kernel, specific hardware configurations, or are defensive programming measures for edge cases that are difficult to reproduce.



### Coverage Quality Assessment
The high coverage percentage indicates:
- **Comprehensive test coverage** across all FreeRTOS wrapper modules
- **Good API validation** with both positive and negative test cases
- **Robust error handling verification** where applicable in unit test context
- **Thorough edge case testing** for boundary conditions

## Test Quality Metrics

### Test Distribution by Category
- **Task Module**: 93 tests (21.2%)
- **Semaphore Module**: 96 tests (21.9%)
- **Queue Module**: 49 tests (11.2%)
- **EventGroup Module**: 30 tests (6.8%)
- **StreamBuffer Module**: 47 tests (10.7%)
- **MessageBuffer Module**: 32 tests (7.3%)
- **Timer Module**: 50 tests (11.4%)
- **EnhancedTimer Module**: 18 tests (4.1%)
- **EnhancedFeatures Module**: 13 tests (3.0%)
- **EnhancedTimeout Module**: 11 tests (2.5%)


### Performance Characteristics
- **Fastest Test**: 0.000 seconds
- **Slowest Test**: 0.434 seconds
- **Performance Distribution**:
  - Very Fast (< 0.01s): 418 tests
  - Fast (0.01-0.05s): 8 tests
  - Normal (0.05-0.1s): 6 tests
  - Slow (> 0.1s): 7 tests

## Validation Conclusions

### ✅ Test Suite Maturity
- **Comprehensive Coverage**: 439 test cases across all FreeRTOS wrapper modules
- **Perfect Success Rate**: All tests passing indicates stable and robust implementation
- **Good Module Distribution**: Balanced testing across tasks, synchronization primitives, and communication mechanisms

### ✅ Code Quality Validation
- **High Coverage**: 97.0% line coverage demonstrates thorough validation
- **API Completeness**: 98.2% function coverage shows comprehensive API testing
- **Production Ready**: Test results indicate code is ready for production use

### 🔄 Continuous Validation
This report is automatically generated with each test execution to ensure:
- **Current State Reflection**: Results always match the latest code changes
- **Regression Detection**: Any new failures are immediately identified
- **Quality Maintenance**: Coverage and test metrics are continuously monitored

## Recommendations

1. **Maintain Test Coverage**: Continue to add tests for new features to maintain high coverage
2. **Integration Testing**: Consider adding integration tests with actual FreeRTOS kernel for uncovered areas
3. **Performance Monitoring**: Monitor test execution times to detect performance regressions
4. **Failure Analysis**: When failures occur, this report will provide detailed failure information


---

*Report Generated*: March 11, 2026 at 10:24:03  
*Test Framework*: GoogleTest/GoogleMock  
*Coverage Tool*: LCOV/GCOV  
*Total Test Execution Time*: 2.04 seconds  
*Validation Status*: ✅ **All tests passing - System validated for production use**

---

*Report generated: March 11, 2026 at 10:24 AM*
*Tools: Google Test + lcov + clang-tidy + cppcheck (MISRA + all rules)*
*Scope: Library modules only (src/, include/)*
