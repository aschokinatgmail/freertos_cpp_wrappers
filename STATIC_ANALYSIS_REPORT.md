# Static Analysis Report

## Overview

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

- **Total Suppressed**: 706534
- **Non-user Code**: 705517
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
- **Files Analyzed**: 10
- **Analysis Errors**: 3
- **Other Issues**: 109

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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

**Violation 1**: freertos_message_buffer.hpp:147:25
*Style*: misra violation

```cpp
     144:    * @param xTicksToWait timeout in ticks to wait for the message buffer to
     145:    * @return size_t number of bytes sent
     146:    */
>>>  147:   template <typename Rep, typename Period>
     148:   size_t send(const void *pvTxData, size_t xDataLengthBytes,
     149:               const std::chrono::duration<Rep, Period> &xTicksToWait) {
     150:     return send(
```

**Violation 2**: freertos_message_buffer.hpp:180:25
*Style*: misra violation

```cpp
     177:    * become available
     178:    * @return size_t number of bytes received
     179:    */
>>>  180:   template <typename Rep, typename Period>
     181:   size_t receive(void *pvRxData, size_t xBufferLengthBytes,
     182:                  const std::chrono::duration<Rep, Period> &timeout) {
     183:     return receive(
```

**Violation 3**: freertos_message_buffer.hpp:94:35
*Style*: misra violation

```cpp
      91:  * @tparam MessageBufferSize size of the message buffer
      92:  * @tparam MessageBufferAllocator type of the message buffer allocator
      93:  */
>>>   94: template <size_t MessageBufferSize, typename MessageBufferAllocator>
      95: class message_buffer {
      96:   MessageBufferAllocator m_allocator{};
      97:   MessageBufferHandle_t m_message_buffer{nullptr};
```

**Violation 4**: freertos_message_buffer.hpp:54:21
*Style*: misra violation

```cpp
      51:  */
      52: template <size_t MessageBufferSize> class static_message_buffer_allocator {
      53:   StaticMessageBuffer_t m_message_buffer_placeholder{};
>>>   54:   std::array<uint8_t, MessageBufferSize> m_storage;
      55: 
      56: public:
      57:   static_message_buffer_allocator() = default;
```

**Violation 5**: freertos_queue.hpp:243:25
*Style*: misra violation

```cpp
     240:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     241:    * errQUEUE_FULL.
     242:    */
>>>  243:   template <typename Rep, typename Period>
     244:   BaseType_t send(const T &item,
     245:                   const std::chrono::duration<Rep, Period> &timeout) {
     246:     return send(
```

**Violation 6**: freertos_queue.hpp:298:25
*Style*: misra violation

```cpp
     295:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     296:    * errQUEUE_FULL.
     297:    */
>>>  298:   template <typename Rep, typename Period>
     299:   BaseType_t send_back(const T &item,
     300:                        const std::chrono::duration<Rep, Period> &timeout) {
     301:     return send_back(
```

**Violation 7**: freertos_queue.hpp:354:25
*Style*: misra violation

```cpp
     351:    * @return BaseType_t pdPASS if the item was successfully posted, otherwise
     352:    * errQUEUE_FULL.
     353:    */
>>>  354:   template <typename Rep, typename Period>
     355:   BaseType_t send_front(const T &item,
     356:                         const std::chrono::duration<Rep, Period> &timeout) {
     357:     return send_front(
```

**Violation 8**: freertos_queue.hpp:427:25
*Style*: misra violation

```cpp
     424:    * @return BaseType_t pdPASS if the item
     425:    * was successfully received, otherwise pdFALSE.
     426:    */
>>>  427:   template <typename Rep, typename Period>
     428:   BaseType_t receive(T &item,
     429:                      const std::chrono::duration<Rep, Period> &timeout) {
     430:     return receive(
```

**Violation 9**: freertos_queue.hpp:443:25
*Style*: misra violation

```cpp
     440:    * @return optional<T> The item received from the queue or std::nullopt if the
     441:    * queue is empty.
     442:    */
>>>  443:   template <typename Rep, typename Period>
     444:   optional<T> receive(const std::chrono::duration<Rep, Period> &timeout) {
     445:     return receive(pdMS_TO_TICKS(
     446:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
```

**Violation 10**: freertos_queue.hpp:580:25
*Style*: misra violation

```cpp
     577:    * @return BaseType_t pdPASS if the item was successfully peeked, otherwise
     578:    * pdFALSE.
     579:    */
>>>  580:   template <typename Rep, typename Period>
     581:   BaseType_t peek(T &item, const std::chrono::duration<Rep, Period> &timeout) {
     582:     return peek(
     583:         item, pdMS_TO_TICKS(
```

**Violation 11**: freertos_queue.hpp:638:25
*Style*: misra violation

```cpp
     635:    * @return optional<T> The item peeked from the queue or std::nullopt if the
     636:    * queue is empty.
     637:    */
>>>  638:   template <typename Rep, typename Period>
     639:   optional<T> peek(const std::chrono::duration<Rep, Period> &timeout) {
     640:     return peek(pdMS_TO_TICKS(
     641:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
```

**Violation 12**: freertos_queue.hpp:188:29
*Style*: misra violation

```cpp
     185:  * static_queue;
     186:  * ```
     187:  */
>>>  188: template <size_t QueueLength, typename T, typename QueueAllocator> class queue {
     189:   QueueAllocator m_allocator{};
     190:   QueueHandle_t m_queue{nullptr};
     191: 
```

**Violation 13**: freertos_queue.hpp:56:21
*Style*: misra violation

```cpp
      53:  */
      54: template <size_t QueueLength, typename T> class static_queue_allocator {
      55:   StaticQueue_t m_queue_placeholder{};
>>>   56:   std::array<uint8_t, QueueLength * sizeof(T)> m_storage;
      57: 
      58: public:
      59:   static_queue_allocator() = default;
```

**Violation 14**: freertos_semaphore.hpp:271:25
*Style*: misra violation

```cpp
     268:    * otherwise pdFALSE.
     269:    *
     270:    */
>>>  271:   template <typename Rep, typename Period>
     272:   BaseType_t take(const std::chrono::duration<Rep, Period> &timeout) {
     273:     return take(
     274:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 15**: freertos_semaphore.hpp:393:25
*Style*: misra violation

```cpp
     390:    * otherwise pdFALSE.
     391:    *
     392:    */
>>>  393:   template <typename Rep, typename Period>
     394:   BaseType_t take(const std::chrono::duration<Rep, Period> &timeout) {
     395:     return take(
     396:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 16**: freertos_semaphore.hpp:586:25
*Style*: misra violation

```cpp
     583:    * @param timeout timeout to wait for the mutex.
     584:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     585:    */
>>>  586:   template <typename Rep, typename Period>
     587:   BaseType_t lock(const std::chrono::duration<Rep, Period> &timeout) {
     588:     return lock(
     589:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 17**: freertos_semaphore.hpp:739:25
*Style*: misra violation

```cpp
     736:    * @param timeout timeout to wait for the recursive mutex.
     737:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     738:    */
>>>  739:   template <typename Rep, typename Period>
     740:   BaseType_t lock(const std::chrono::duration<Rep, Period> &timeout) {
     741:     return lock(
     742:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 18**: freertos_semaphore.hpp:827:23
*Style*: misra violation

```cpp
     824:    * @param mutex mutex to guard
     825:    */
     826:   explicit try_lock_guard(Mutex &mutex)
>>>  827:       : m_mutex{mutex}, m_lock_acquired{static_cast<bool>(m_mutex.try_lock())} {
     828:   }
     829:   /**
     830:    * @brief Destruct the try lock guard object and unlock the mutex.
```

**Violation 19**: freertos_semaphore.hpp:921:23
*Style*: misra violation

```cpp
     918:    * @param ticks_to_wait timeout in ticks to wait for the mutex.
     919:    */
     920:   timeout_lock_guard(Mutex &mutex, TickType_t ticks_to_wait)
>>>  921:       : m_mutex{mutex},
     922:         m_lock_acquired{static_cast<bool>(m_mutex.lock(ticks_to_wait))} {}
     923:   /**
     924:    * @brief Construct a new timeout lock guard object
```

**Violation 20**: freertos_semaphore.hpp:929:25
*Style*: misra violation

```cpp
     926:    * @param mutex mutex to guard
     927:    * @param timeout timeout to wait for the mutex.
     928:    */
>>>  929:   template <typename Rep, typename Period>
     930:   timeout_lock_guard(Mutex &mutex,
     931:                      const std::chrono::duration<Rep, Period> &timeout)
     932:       : m_mutex{mutex},
```

**Violation 21**: freertos_semaphore.hpp:932:23
*Style*: misra violation

```cpp
     929:   template <typename Rep, typename Period>
     930:   timeout_lock_guard(Mutex &mutex,
     931:                      const std::chrono::duration<Rep, Period> &timeout)
>>>  932:       : m_mutex{mutex},
     933:         m_lock_acquired{static_cast<bool>(m_mutex.lock(
     934:             std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
     935:                 .count()))} {}
```

**Violation 22**: freertos_sw_timer.hpp:130:58
*Style*: misra violation

```cpp
     127:    */
     128:   explicit timer(const char *name, const TickType_t period_ticks,
     129:                  UBaseType_t auto_reload, timer_callback_t &&callback)
>>>  130:       : m_callback{std::move(callback)}, m_started{false},
     131:         m_timer{m_allocator.create(name, period_ticks, auto_reload, this,
     132:                                    callback_wrapper)} {
     133:     configASSERT(m_timer);
```

**Violation 23**: freertos_sw_timer.hpp:145:25
*Style*: misra violation

```cpp
     142:    * @param auto_reload auto-reload flag
     143:    * @param callback callback routine
     144:    */
>>>  145:   template <typename Rep, typename Period>
     146:   explicit timer(const char *name,
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
```

**Violation 24**: freertos_sw_timer.hpp:254:25
*Style*: misra violation

```cpp
     251:    * @param timeout timeout to wait for the timer to start
     252:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
     253:    */
>>>  254:   template <typename Rep, typename Period>
     255:   BaseType_t start(const std::chrono::duration<Rep, Period> &timeout) {
     256:     return start(
     257:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 25**: freertos_sw_timer.hpp:313:25
*Style*: misra violation

```cpp
     310:    * @param timeout timeout to wait for the timer to stop
     311:    * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
     312:    */
>>>  313:   template <typename Rep, typename Period>
     314:   BaseType_t stop(const std::chrono::duration<Rep, Period> &timeout) {
     315:     return stop(
     316:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 26**: freertos_sw_timer.hpp:368:25
*Style*: misra violation

```cpp
     365:    * @param timeout timeout to wait for the timer to reset
     366:    * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
     367:    */
>>>  368:   template <typename Rep, typename Period>
     369:   BaseType_t reset(const std::chrono::duration<Rep, Period> &timeout) {
     370:     return reset(
     371:         std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());
```

**Violation 27**: freertos_sw_timer.hpp:427:31
*Style*: misra violation

```cpp
     424:    * @return BaseType_t pdPASS if the timer period was changed successfully else
     425:    * pdFAIL
     426:    */
>>>  427:   template <typename RepPeriod, typename PeriodPeriod, typename RepTimeout,
     428:             typename PeriodTimeout>
     429:   BaseType_t
     430:   period(const std::chrono::duration<RepPeriod, PeriodPeriod> &new_period,
```

**Violation 28**: freertos_sw_timer.hpp:467:25
*Style*: misra violation

```cpp
     464:    * @return BaseType_t pdPASS if the timer period was changed successfully else
     465:    * pdFAIL
     466:    */
>>>  467:   template <typename Rep, typename Period>
     468:   BaseType_t period_isr(const std::chrono::duration<Rep, Period> &new_period,
     469:                         BaseType_t &high_priority_task_woken) {
     470:     return period_isr(
```

**Violation 29**: freertos_sw_timer.hpp:497:25
*Style*: misra violation

```cpp
     494:    * @return BaseType_t pdPASS if the timer period was changed successfully else
     495:    * pdFAIL
     496:    */
>>>  497:   template <typename Rep, typename Period>
     498:   BaseType_t period_isr(const std::chrono::duration<Rep, Period> &new_period) {
     499:     return period_isr(
     500:         std::chrono::duration_cast<std::chrono::milliseconds>(new_period)
```

**Violation 30**: freertos_event_group.hpp:181:25
*Style*: misra violation

```cpp
     178:    * @param timeout timeout to wait for the bits
     179:    * @return EventBits_t value of the event group after the call returns
     180:    */
>>>  181:   template <typename Rep, typename Period>
     182:   EventBits_t wait_bits(const EventBits_t bits_to_wait_for,
     183:                         const BaseType_t clear_on_exit,
     184:                         const BaseType_t wait_for_all_bits,
```

**Violation 31**: freertos_event_group.hpp:223:25
*Style*: misra violation

```cpp
     220:     return xEventGroupSync(m_event_group, bits_to_set, bits_to_wait_for,
     221:                            ticks_to_wait);
     222:   }
>>>  223:   template <typename Rep, typename Period>
     224:   EventBits_t sync(const EventBits_t bits_to_set,
     225:                    const EventBits_t bits_to_wait_for,
     226:                    const std::chrono::duration<Rep, Period> &timeout) {
```

**Violation 32**: freertos_stream_buffer.hpp:156:25
*Style*: misra violation

```cpp
     153:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     154:    * insufficient memory available to copy the data into the stream buffer.
     155:    */
>>>  156:   template <typename Rep, typename Period>
     157:   BaseType_t send(const void *data, size_t data_size,
     158:                   const std::chrono::duration<Rep, Period> &timeout) {
     159:     return send(
```

**Violation 33**: freertos_stream_buffer.hpp:194:30
*Style*: misra violation

```cpp
     191:    * stream buffer, otherwise errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY if there was
     192:    * insufficient memory available to copy the data into the stream buffer.
     193:    */
>>>  194:   template <typename Iterator, typename Rep, typename Period>
     195:   BaseType_t send(Iterator begin, Iterator end,
     196:                   const std::chrono::duration<Rep, Period> &timeout) {
     197:     return send(&*begin, std::distance(begin, end), timeout);
```

**Violation 34**: freertos_stream_buffer.hpp:294:25
*Style*: misra violation

```cpp
     291:    * @param timeout Duration to wait for the data to be copied into the buffer.
     292:    * @return size_t Number of bytes received.
     293:    */
>>>  294:   template <typename Rep, typename Period>
     295:   size_t receive(void *data, size_t data_size,
     296:                  const std::chrono::duration<Rep, Period> &timeout) {
     297:     return receive(
```

**Violation 35**: freertos_stream_buffer.hpp:96:34
*Style*: misra violation

```cpp
      93:  * @tparam StreamBufferSize size of the stream buffer
      94:  * @tparam StreamBufferAllocator type of the stream buffer allocator
      95:  */
>>>   96: template <size_t StreamBufferSize, typename StreamBufferAllocator>
      97: class stream_buffer {
      98:   StreamBufferAllocator m_allocator{};
      99:   StreamBufferHandle_t m_stream_buffer{nullptr};
```

**Violation 36**: freertos_stream_buffer.hpp:55:21
*Style*: misra violation

```cpp
      52:  */
      53: template <size_t StreamBufferSize> class static_stream_buffer_allocator {
      54:   StaticStreamBuffer_t m_stream_buffer_placeholder{};
>>>   55:   std::array<uint8_t, StreamBufferSize> m_storage;
      56: 
      57: public:
      58:   static_stream_buffer_allocator() = default;
```

#### MISRA C 2012 Rule 13.4 (1 violation(s))

**The result of an assignment operator should not be used**

Assignment expressions should not be used as sub-expressions. Assignments should be separate statements to improve readability and avoid confusion.

*Rationale: Using assignment results as expressions can make code harder to read and may hide logical errors.*

**Violation 1**: freertos_queue.hpp:199:45
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

```cpp
     655:     BaseType_t higher_priority_task_woken = pdFALSE;
     656:     if (xQueuePeekFromISR(m_queue, &item, &higher_priority_task_woken) ==
     657:         pdPASS) {
>>>  658:       return item;
     659:     }
     660:     return {};
     661:   }
```

**Violation 5**: freertos_sw_timer.hpp:237:7
*Style*: misra violation

```cpp
     234:    */
     235:   BaseType_t start(const TickType_t ticks_to_wait = portMAX_DELAY) {
     236:     if (!m_timer) {
>>>  237:       return pdFAIL;
     238:     }
     239:     auto rc = xTimerStart(m_timer, ticks_to_wait);
     240:     if (rc) {
```

**Violation 6**: freertos_sw_timer.hpp:269:7
*Style*: misra violation

```cpp
     266:    */
     267:   BaseType_t start_isr(BaseType_t &high_priority_task_woken) {
     268:     if (!m_timer) {
>>>  269:       return pdFAIL;
     270:     }
     271:     auto rc = xTimerStartFromISR(m_timer, &high_priority_task_woken);
     272:     if (rc) {
```

**Violation 7**: freertos_sw_timer.hpp:296:7
*Style*: misra violation

```cpp
     293:    */
     294:   BaseType_t stop(const TickType_t ticks_to_wait = portMAX_DELAY) {
     295:     if (!m_timer) {
>>>  296:       return pdFAIL;
     297:     }
     298:     auto rc = xTimerStop(m_timer, ticks_to_wait);
     299:     if (rc) {
```

**Violation 8**: freertos_sw_timer.hpp:328:7
*Style*: misra violation

```cpp
     325:    */
     326:   BaseType_t stop_isr(BaseType_t &high_priority_task_woken) {
     327:     if (!m_timer) {
>>>  328:       return pdFAIL;
     329:     }
     330:     auto rc = xTimerStopFromISR(m_timer, &high_priority_task_woken);
     331:     if (rc) {
```

**Violation 9**: freertos_sw_timer.hpp:355:7
*Style*: misra violation

```cpp
     352:    */
     353:   BaseType_t reset(const TickType_t ticks_to_wait = portMAX_DELAY) {
     354:     if (!m_timer) {
>>>  355:       return pdFAIL;
     356:     }
     357:     return xTimerReset(m_timer, ticks_to_wait);
     358:   }
```

**Violation 10**: freertos_sw_timer.hpp:383:7
*Style*: misra violation

```cpp
     380:    */
     381:   BaseType_t reset_isr(BaseType_t &high_priority_task_woken) {
     382:     if (!m_timer) {
>>>  383:       return pdFAIL;
     384:     }
     385:     return xTimerResetFromISR(m_timer, &high_priority_task_woken);
     386:   }
```

**Violation 11**: freertos_sw_timer.hpp:410:7
*Style*: misra violation

```cpp
     407:   BaseType_t period(const TickType_t new_period_ticks,
     408:                     const TickType_t ticks_to_wait = portMAX_DELAY) {
     409:     if (!m_timer) {
>>>  410:       return pdFAIL;
     411:     }
     412:     return xTimerChangePeriod(m_timer, new_period_ticks, ticks_to_wait);
     413:   }
```

**Violation 12**: freertos_sw_timer.hpp:450:7
*Style*: misra violation

```cpp
     447:   BaseType_t period_isr(const TickType_t new_period_ticks,
     448:                         BaseType_t &high_priority_task_woken) {
     449:     if (!m_timer) {
>>>  450:       return pdFAIL;
     451:     }
     452:     return xTimerChangePeriodFromISR(m_timer, new_period_ticks,
     453:                                      &high_priority_task_woken);
```

**Violation 13**: freertos_sw_timer.hpp:516:7
*Style*: misra violation

```cpp
     513:    */
     514:   TickType_t period_ticks(void) const {
     515:     if (!m_timer) {
>>>  516:       return 0;
     517:     }
     518:     return xTimerGetPeriod(m_timer);
     519:   }
```

**Violation 14**: freertos_sw_timer.hpp:556:7
*Style*: misra violation

```cpp
     553:    */
     554:   TickType_t remaining_ticks(void) const {
     555:     if (m_timer) {
>>>  556:       return xTimerGetExpiryTime(m_timer) - xTaskGetTickCount();
     557:     } else {
     558:       return 0;
     559:     }
```

**Violation 15**: freertos_sw_timer.hpp:558:7
*Style*: misra violation

```cpp
     555:     if (m_timer) {
     556:       return xTimerGetExpiryTime(m_timer) - xTaskGetTickCount();
     557:     } else {
>>>  558:       return 0;
     559:     }
     560:   }
     561:   /**
```

**Violation 16**: freertos_sw_timer.hpp:578:7
*Style*: misra violation

```cpp
     575:    */
     576:   BaseType_t running(void) const {
     577:     if (!m_timer) {
>>>  578:       return pdFALSE;
     579:     }
     580:     return xTimerIsTimerActive(m_timer);
     581:   }
```

**Violation 17**: freertos_sw_timer.hpp:589:7
*Style*: misra violation

```cpp
     586:    */
     587:   const char *name(void) const {
     588:     if (!m_timer) {
>>>  589:       return nullptr;
     590:     }
     591:     return pcTimerGetName(m_timer);
     592:   }
```

#### MISRA C 2012 Rule 16.3 (13 violation(s))

**An unconditional break statement shall terminate every switch-clause**

Every case in a switch statement should end with a break statement to prevent fall-through behavior, unless fall-through is explicitly intended and documented.

*Rationale: Missing break statements can cause unintended fall-through behavior that leads to bugs.*

**Violation 1**: freertos_message_buffer.hpp:57:39
*Style*: misra violation

```cpp
      54:   std::array<uint8_t, MessageBufferSize> m_storage;
      55: 
      56: public:
>>>   57:   static_message_buffer_allocator() = default;
      58:   ~static_message_buffer_allocator() = default;
      59:   static_message_buffer_allocator(const static_message_buffer_allocator &) =
      60:       delete;
```

**Violation 2**: freertos_message_buffer.hpp:58:40
*Style*: misra violation

```cpp
      55: 
      56: public:
      57:   static_message_buffer_allocator() = default;
>>>   58:   ~static_message_buffer_allocator() = default;
      59:   static_message_buffer_allocator(const static_message_buffer_allocator &) =
      60:       delete;
      61:   static_message_buffer_allocator(static_message_buffer_allocator &&) = delete;
```

**Violation 3**: freertos_queue.hpp:59:30
*Style*: misra violation

```cpp
      56:   std::array<uint8_t, QueueLength * sizeof(T)> m_storage;
      57: 
      58: public:
>>>   59:   static_queue_allocator() = default;
      60:   ~static_queue_allocator() = default;
      61:   static_queue_allocator(const static_queue_allocator &) = delete;
      62:   static_queue_allocator(static_queue_allocator &&) = delete;
```

**Violation 4**: freertos_queue.hpp:60:31
*Style*: misra violation

```cpp
      57: 
      58: public:
      59:   static_queue_allocator() = default;
>>>   60:   ~static_queue_allocator() = default;
      61:   static_queue_allocator(const static_queue_allocator &) = delete;
      62:   static_queue_allocator(static_queue_allocator &&) = delete;
      63: 
```

**Violation 5**: freertos_semaphore.hpp:56:34
*Style*: misra violation

```cpp
      53:   StaticSemaphore_t m_semaphore_placeholder{};
      54: 
      55: public:
>>>   56:   static_semaphore_allocator() = default;
      57:   ~static_semaphore_allocator() = default;
      58:   static_semaphore_allocator(const static_semaphore_allocator &) = delete;
      59:   static_semaphore_allocator(static_semaphore_allocator &&) = delete;
```

**Violation 6**: freertos_semaphore.hpp:57:35
*Style*: misra violation

```cpp
      54: 
      55: public:
      56:   static_semaphore_allocator() = default;
>>>   57:   ~static_semaphore_allocator() = default;
      58:   static_semaphore_allocator(const static_semaphore_allocator &) = delete;
      59:   static_semaphore_allocator(static_semaphore_allocator &&) = delete;
      60: 
```

**Violation 7**: freertos_sw_timer.hpp:63:33
*Style*: misra violation

```cpp
      60:   StaticTimer_t m_timer_placeholder{};
      61: 
      62: public:
>>>   63:   static_sw_timer_allocator() = default;
      64:   ~static_sw_timer_allocator() = default;
      65:   static_sw_timer_allocator(const static_sw_timer_allocator &) = delete;
      66:   static_sw_timer_allocator(static_sw_timer_allocator &&) = default;
```

**Violation 8**: freertos_sw_timer.hpp:64:34
*Style*: misra violation

```cpp
      61: 
      62: public:
      63:   static_sw_timer_allocator() = default;
>>>   64:   ~static_sw_timer_allocator() = default;
      65:   static_sw_timer_allocator(const static_sw_timer_allocator &) = delete;
      66:   static_sw_timer_allocator(static_sw_timer_allocator &&) = default;
      67: 
```

**Violation 9**: freertos_sw_timer.hpp:66:61
*Style*: misra violation

```cpp
      63:   static_sw_timer_allocator() = default;
      64:   ~static_sw_timer_allocator() = default;
      65:   static_sw_timer_allocator(const static_sw_timer_allocator &) = delete;
>>>   66:   static_sw_timer_allocator(static_sw_timer_allocator &&) = default;
      67: 
      68:   static_sw_timer_allocator &
      69:   operator=(const static_sw_timer_allocator &) = delete;
```

**Violation 10**: freertos_event_group.hpp:54:36
*Style*: misra violation

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
*Style*: misra violation

```cpp
      52: 
      53: public:
      54:   static_event_group_allocator() = default;
>>>   55:   ~static_event_group_allocator() = default;
      56:   static_event_group_allocator(const static_event_group_allocator &) = delete;
      57:   static_event_group_allocator(static_event_group_allocator &&) = delete;
      58: 
```

**Violation 12**: freertos_stream_buffer.hpp:58:38
*Style*: misra violation

```cpp
      55:   std::array<uint8_t, StreamBufferSize> m_storage;
      56: 
      57: public:
>>>   58:   static_stream_buffer_allocator() = default;
      59:   ~static_stream_buffer_allocator() = default;
      60:   static_stream_buffer_allocator(const static_stream_buffer_allocator &) =
      61:       delete;
```

**Violation 13**: freertos_stream_buffer.hpp:59:39
*Style*: misra violation

```cpp
      56: 
      57: public:
      58:   static_stream_buffer_allocator() = default;
>>>   59:   ~static_stream_buffer_allocator() = default;
      60:   static_stream_buffer_allocator(const static_stream_buffer_allocator &) =
      61:       delete;
      62:   static_stream_buffer_allocator(static_stream_buffer_allocator &&) = delete;
```

#### MISRA C 2012 Rule 17.3 (7 violation(s))

**A function shall not be declared implicitly**

All functions should be explicitly declared before use. Implicit function declarations can lead to type mismatches and undefined behavior.

*Rationale: Implicit function declarations can cause type mismatches and make code harder to understand and maintain.*

**Violation 1**: freertos_queue.hpp:413:9
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

```cpp
     653:   optional<T> peek_isr(void) {
     654:     T item;
     655:     BaseType_t higher_priority_task_woken = pdFALSE;
>>>  656:     if (xQueuePeekFromISR(m_queue, &item, &higher_priority_task_woken) ==
     657:         pdPASS) {
     658:       return item;
     659:     }
```

**Violation 5**: freertos_sw_timer.hpp:184:16
*Style*: misra violation

```cpp
     181:     if (m_timer) {
     182:       auto rc = xTimerDelete(m_timer, portMAX_DELAY);
     183:       if (rc == pdPASS) {
>>>  184:         while (xTimerIsTimerActive(m_timer) != pdFALSE) {
     185:           vTaskDelay(pdMS_TO_TICKS(1));
     186:         }
     187:       }
```

**Violation 6**: freertos_sw_timer.hpp:199:16
*Style*: misra violation

```cpp
     196:       }
     197:       auto rc = xTimerStop(src.m_timer, portMAX_DELAY);
     198:       if (rc == pdPASS) {
>>>  199:         while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
     200:           vTaskDelay(pdMS_TO_TICKS(1));
     201:         }
     202:         auto name = pcTimerGetName(src.m_timer);
```

**Violation 7**: freertos_sw_timer.hpp:207:18
*Style*: misra violation

```cpp
     204:         auto auto_reload = uxTimerGetReloadMode(src.m_timer);
     205:         rc = xTimerDelete(src.m_timer, portMAX_DELAY);
     206:         if (rc == pdPASS) {
>>>  207:           while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
     208:             vTaskDelay(pdMS_TO_TICKS(1));
     209:           }
     210:           src.m_timer = nullptr;
```

#### MISRA C 2012 Rule 17.8 (14 violation(s))

**A function parameter should not be modified**

Function parameters should not be modified within the function body. If modification is needed, use a local copy of the parameter.

*Rationale: Modifying parameters can make function behavior less predictable and harder to understand.*

**Violation 1**: freertos_semaphore.hpp:234:50
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

```cpp
     694:    * @param ticks_to_wait timeout in ticks to wait for the recursive mutex.
     695:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     696:    */
>>>  697:   BaseType_t lock(const TickType_t ticks_to_wait = portMAX_DELAY) {
     698:     auto rc = xSemaphoreTakeRecursive(m_semaphore, ticks_to_wait);
     699:     if (rc) {
     700:       m_recursions_count++;
```

**Violation 6**: freertos_sw_timer.hpp:235:51
*Style*: misra violation

```cpp
     232:    * @param ticks_to_wait timeout in ticks to wait for the timer to start
     233:    * @return BaseType_t pdPASS if the timer was started successfully else pdFAIL
     234:    */
>>>  235:   BaseType_t start(const TickType_t ticks_to_wait = portMAX_DELAY) {
     236:     if (!m_timer) {
     237:       return pdFAIL;
     238:     }
```

**Violation 7**: freertos_sw_timer.hpp:294:50
*Style*: misra violation

```cpp
     291:    * @param ticks_to_wait timeout in ticks to wait for the timer to stop
     292:    * @return BaseType_t pdPASS if the timer was stopped successfully else pdFAIL
     293:    */
>>>  294:   BaseType_t stop(const TickType_t ticks_to_wait = portMAX_DELAY) {
     295:     if (!m_timer) {
     296:       return pdFAIL;
     297:     }
```

**Violation 8**: freertos_sw_timer.hpp:353:51
*Style*: misra violation

```cpp
     350:    * @param ticks_to_wait timeout in ticks to wait for the timer to reset
     351:    * @return BaseType_t pdPASS if the timer was reset successfully else pdFAIL
     352:    */
>>>  353:   BaseType_t reset(const TickType_t ticks_to_wait = portMAX_DELAY) {
     354:     if (!m_timer) {
     355:       return pdFAIL;
     356:     }
```

**Violation 9**: freertos_sw_timer.hpp:408:52
*Style*: misra violation

```cpp
     405:    * pdFAIL
     406:    */
     407:   BaseType_t period(const TickType_t new_period_ticks,
>>>  408:                     const TickType_t ticks_to_wait = portMAX_DELAY) {
     409:     if (!m_timer) {
     410:       return pdFAIL;
     411:     }
```

**Violation 10**: freertos_stream_buffer.hpp:141:38
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

```cpp
     928:    */
     929:   template <typename Rep, typename Period>
     930:   timeout_lock_guard(Mutex &mutex,
>>>  931:                      const std::chrono::duration<Rep, Period> &timeout)
     932:       : m_mutex{mutex},
     933:         m_lock_acquired{static_cast<bool>(m_mutex.lock(
     934:             std::chrono::duration_cast<std::chrono::milliseconds>(timeout)
```

**Violation 10**: freertos_sw_timer.hpp:128:30
*Style*: misra violation

```cpp
     125:    * @param auto_reload auto-reload flag
     126:    * @param callback callback routine
     127:    */
>>>  128:   explicit timer(const char *name, const TickType_t period_ticks,
     129:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     130:       : m_callback{std::move(callback)}, m_started{false},
     131:         m_timer{m_allocator.create(name, period_ticks, auto_reload, this,
```

**Violation 11**: freertos_sw_timer.hpp:129:30
*Style*: misra violation

```cpp
     126:    * @param callback callback routine
     127:    */
     128:   explicit timer(const char *name, const TickType_t period_ticks,
>>>  129:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     130:       : m_callback{std::move(callback)}, m_started{false},
     131:         m_timer{m_allocator.create(name, period_ticks, auto_reload, this,
     132:                                    callback_wrapper)} {
```

**Violation 12**: freertos_sw_timer.hpp:146:30
*Style*: misra violation

```cpp
     143:    * @param callback callback routine
     144:    */
     145:   template <typename Rep, typename Period>
>>>  146:   explicit timer(const char *name,
     147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
```

**Violation 13**: freertos_sw_timer.hpp:147:60
*Style*: misra violation

```cpp
     144:    */
     145:   template <typename Rep, typename Period>
     146:   explicit timer(const char *name,
>>>  147:                  const std::chrono::duration<Rep, Period> &period,
     148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
     150:               static_cast<TickType_t>(
```

**Violation 14**: freertos_sw_timer.hpp:148:30
*Style*: misra violation

```cpp
     145:   template <typename Rep, typename Period>
     146:   explicit timer(const char *name,
     147:                  const std::chrono::duration<Rep, Period> &period,
>>>  148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
     150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
```

**Violation 15**: freertos_stream_buffer.hpp:108:33
*Style*: misra violation

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

**Violation 1**: freertos_message_buffer.hpp:46:2
*Style*: misra violation

```cpp
      43: 
      44: namespace freertos {
      45: 
>>>   46: #if configSUPPORT_STATIC_ALLOCATION
      47: /**
      48:  * @brief An allocator for the message buffer that uses a static memory
      49:  * allocation.
```

**Violation 2**: freertos_message_buffer.hpp:74:2
*Style*: misra violation

```cpp
      71:   }
      72: };
      73: #endif
>>>   74: #if configSUPPORT_DYNAMIC_ALLOCATION
      75: /**
      76:  * @brief An allocator for the message buffer that uses a dynamic memory
      77:  * allocation.
```

**Violation 3**: freertos_message_buffer.hpp:219:2
*Style*: misra violation

```cpp
     216:   BaseType_t full(void) { return xMessageBufferIsFull(m_message_buffer); }
     217: };
     218: 
>>>  219: #if configSUPPORT_STATIC_ALLOCATION
     220: /**
     221:  * @brief Namespace for the FreeRTOS message buffer with static memory
     222:  * allocation
```

**Violation 4**: freertos_message_buffer.hpp:237:2
*Style*: misra violation

```cpp
     234:     freertos::static_message_buffer_allocator<MessageBufferSize>>;
     235: } // namespace sa
     236: #endif
>>>  237: #if configSUPPORT_DYNAMIC_ALLOCATION
     238: /**
     239:  * @brief Namespace for the FreeRTOS message buffer with dynamic memory
     240:  * allocation
```

**Violation 5**: freertos_queue.hpp:49:2
*Style*: misra violation

```cpp
      46: 
      47: using std::optional;
      48: 
>>>   49: #if configSUPPORT_STATIC_ALLOCATION
      50: /**
      51:  * @brief An allocator for the queue that uses a static memory allocation.
      52:  *
```

**Violation 6**: freertos_queue.hpp:73:2
*Style*: misra violation

```cpp
      70:   }
      71: };
      72: #endif
>>>   73: #if configSUPPORT_DYNAMIC_ALLOCATION
      74: /**
      75:  * @brief An allocator for the queue that uses a dynamic memory allocation.
      76:  *
```

**Violation 7**: freertos_queue.hpp:687:2
*Style*: misra violation

```cpp
     684:   }
     685: };
     686: 
>>>  687: #if configSUPPORT_STATIC_ALLOCATION
     688: /**
     689:  * @brief A namespace for the FreeRTOS queue with a static memory allocation.
     690:  *
```

**Violation 8**: freertos_queue.hpp:702:2
*Style*: misra violation

```cpp
     699:     freertos::queue<QueueLength, T, static_queue_allocator<QueueLength, T>>;
     700: } // namespace sa
     701: #endif
>>>  702: #if configSUPPORT_DYNAMIC_ALLOCATION
     703: /**
     704:  * @brief A namespace for the FreeRTOS queue with a dynamic memory allocation.
     705:  *
```

**Violation 9**: freertos_semaphore.hpp:47:2
*Style*: misra violation

```cpp
      44: 
      45: namespace freertos {
      46: 
>>>   47: #if configSUPPORT_STATIC_ALLOCATION
      48: /**
      49:  * @brief An allocator for the semaphore that uses a static memory allocation.
      50:  *
```

**Violation 10**: freertos_semaphore.hpp:80:2
*Style*: misra violation

```cpp
      77:   }
      78: };
      79: #endif
>>>   80: #if configSUPPORT_DYNAMIC_ALLOCATION
      81: /**
      82:  * @brief An allocator for the semaphore that uses a dynamic memory allocation.
      83:  *
```

**Violation 11**: freertos_semaphore.hpp:960:2
*Style*: misra violation

```cpp
     957:   bool locked(void) const { return m_lock_acquired && m_mutex.locked(); }
     958: };
     959: 
>>>  960: #if configSUPPORT_STATIC_ALLOCATION
     961: /**
     962:  * @brief A wrapper for the FreeRTOS binary semaphore with static memory
     963:  * allocation.
```

**Violation 12**: freertos_semaphore.hpp:992:2
*Style*: misra violation

```cpp
     989:     freertos::recursive_mutex<freertos::static_semaphore_allocator>;
     990: } // namespace sa
     991: #endif
>>>  992: #if configSUPPORT_DYNAMIC_ALLOCATION
     993: /**
     994:  * @brief A wrapper for the FreeRTOS binary semaphore with dynamic memory
     995:  * allocation.
```

**Violation 13**: freertos_sw_timer.hpp:49:2
*Style*: misra violation

```cpp
      46: 
      47: namespace freertos {
      48: 
>>>   49: #if configUSE_TIMERS
      50: 
      51: using std::function;
      52: 
```

**Violation 14**: freertos_sw_timer.hpp:53:2
*Style*: misra violation

```cpp
      50: 
      51: using std::function;
      52: 
>>>   53: #if configSUPPORT_STATIC_ALLOCATION
      54: /**
      55:  * @brief An allocator for the software timer that uses a static memory
      56:  * allocation.
```

**Violation 15**: freertos_sw_timer.hpp:595:2
*Style*: misra violation

```cpp
     592:   }
     593: };
     594: 
>>>  595: #if configSUPPORT_STATIC_ALLOCATION
     596: /**
     597:  * @brief Namespace for the kernel objects static memory allocation.
     598:  *
```

**Violation 16**: freertos_sw_timer.hpp:80:2
*Style*: misra violation

```cpp
      77:   }
      78: };
      79: #endif
>>>   80: #if configSUPPORT_DYNAMIC_ALLOCATION
      81: /**
      82:  * @brief An allocator for the software timer that uses a dynamic memory
      83:  * allocation.
```

**Violation 17**: freertos_sw_timer.hpp:608:2
*Style*: misra violation

```cpp
     605: using timer = freertos::timer<freertos::static_sw_timer_allocator>;
     606: } // namespace sa
     607: #endif
>>>  608: #if configSUPPORT_DYNAMIC_ALLOCATION
     609: /**
     610:  * @brief Namespace for the kernel objects dynamic memory allocation.
     611:  *
```

**Violation 18**: freertos_event_group.hpp:45:2
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

```cpp
     246:     freertos::event_group<freertos::static_event_group_allocator>;
     247: } // namespace sa
     248: #endif
>>>  249: #if configSUPPORT_DYNAMIC_ALLOCATION
     250: /**
     251:  * @brief Namespace for aliases of RTOS kernel objects that use a dynamic memory
     252:  *
```

**Violation 22**: freertos_stream_buffer.hpp:47:2
*Style*: misra violation

```cpp
      44: 
      45: namespace freertos {
      46: 
>>>   47: #if configSUPPORT_STATIC_ALLOCATION
      48: /**
      49:  * @brief An allocator for the stream buffer that uses a static memory
      50:  * allocation.
```

**Violation 23**: freertos_stream_buffer.hpp:76:2
*Style*: misra violation

```cpp
      73:   }
      74: };
      75: #endif
>>>   76: #if configSUPPORT_DYNAMIC_ALLOCATION
      77: /**
      78:  * @brief An allocator for the stream buffer that uses a dynamic memory
      79:  * allocation.
```

**Violation 24**: freertos_stream_buffer.hpp:387:2
*Style*: misra violation

```cpp
     384:   BaseType_t full(void) { return xStreamBufferIsFull(m_stream_buffer); }
     385: };
     386: 
>>>  387: #if configSUPPORT_STATIC_ALLOCATION
     388: /**
     389:  * @brief Namespace for the FreeRTOS stream buffer with static memory allocation
     390:  *
```

**Violation 25**: freertos_stream_buffer.hpp:404:2
*Style*: misra violation

```cpp
     401:     freertos::static_stream_buffer_allocator<StreamBufferSize>>;
     402: } // namespace sa
     403: #endif
>>>  404: #if configSUPPORT_DYNAMIC_ALLOCATION
     405: /**
     406:  * @brief Namespace for the FreeRTOS stream buffer with dynamic memory
     407:  * allocation
```

#### MISRA C 2012 Rule 21.2 (1 violation(s))

**A reserved identifier or reserved macro name shall not be declared**

Identifiers reserved by the C standard (such as those beginning with underscore) should not be used in user code to avoid conflicts with implementation-defined behavior.

*Rationale: Using reserved identifiers can conflict with compiler and standard library implementations.*

**Violation 1**: freertos_stream_buffer.hpp:346:10
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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

**Violation 1**: freertos_task.hpp:219:20
*Style*: misra violation

```cpp
     216:    * @param priority  priority of the task
     217:    * @param task_routine  task routine function
     218:    */
>>>  219:   task(const char *name, UBaseType_t priority, task_routine_t &&task_routine)
     220:       : m_allocator{}, m_taskRoutine{task_routine},
     221:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     222:   /**
```

**Violation 2**: freertos_task.hpp:848:15
*Style*: misra violation

```cpp
     845:    *
     846:    * @return const char*  task name
     847:    */
>>>  848:   const char *name(void) const { return m_task.name(); }
     849: // Task notification API
     850: #if configUSE_TASK_NOTIFICATIONS
     851:   /**
```

**Violation 3**: freertos_task.hpp:219:20
*Style*: misra violation

```cpp
     216:    * @param priority  priority of the task
     217:    * @param task_routine  task routine function
     218:    */
>>>  219:   task(const char *name, UBaseType_t priority, task_routine_t &&task_routine)
     220:       : m_allocator{}, m_taskRoutine{task_routine},
     221:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     222:   /**
```

**Violation 4**: freertos_task.hpp:848:15
*Style*: misra violation

```cpp
     845:    *
     846:    * @return const char*  task name
     847:    */
>>>  848:   const char *name(void) const { return m_task.name(); }
     849: // Task notification API
     850: #if configUSE_TASK_NOTIFICATIONS
     851:   /**
```

**Violation 5**: freertos_semaphore.hpp:452:46
*Style*: misra violation

```cpp
     449:    * @param count the number of times to give the counting semaphore.
     450:    * @return counting_semaphore& reference to the counting semaphore.
     451:    */
>>>  452:   counting_semaphore &operator+=(UBaseType_t count) {
     453:     while (count--) {
     454:       give();
     455:     }
```

**Violation 6**: freertos_semaphore.hpp:445:15
*Style*: misra violation

```cpp
     442:    *
     443:    * @return UBaseType_t count of the counting semaphore.
     444:    */
>>>  445:   UBaseType_t count() { return uxSemaphoreGetCount(m_semaphore); }
     446:   /**
     447:    * @brief Give the counting semaphore.
     448:    *
```

**Violation 7**: freertos_semaphore.hpp:787:30
*Style*: misra violation

```cpp
     784:    *
     785:    * @param mutex mutex to guard
     786:    */
>>>  787:   explicit lock_guard(Mutex &mutex) : m_mutex{mutex} { m_mutex.lock(); }
     788: 
     789:   /**
     790:    * @brief Destruct the lock guard object and unlock the mutex.
```

**Violation 8**: freertos_semaphore.hpp:486:4
*Style*: misra violation

```cpp
     483:    * created.
     484:    *
     485:    */
>>>  486:   ~mutex(void) {
     487:     if (m_semaphore) {
     488:       vSemaphoreDelete(m_semaphore);
     489:     }
```

**Violation 9**: freertos_semaphore.hpp:530:16
*Style*: misra violation

```cpp
     527:    * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
     528:    */
     529:   BaseType_t unlock_isr(void) {
>>>  530:     BaseType_t high_priority_task_woken = pdFALSE;
     531:     auto rc = xSemaphoreGiveFromISR(m_semaphore, &high_priority_task_woken);
     532:     if (rc) {
     533:       m_locked = false;
```

**Violation 10**: freertos_semaphore.hpp:891:14
*Style*: misra violation

```cpp
     888:    * @return BaseType_t pdTRUE if the high priority task was woken, otherwise
     889:    * pdFALSE.
     890:    */
>>>  891:   BaseType_t high_priority_task_woken(void) const {
     892:     return m_high_priority_task_woken;
     893:   }
     894:   /**
```

**Violation 11**: freertos_semaphore.hpp:452:46
*Style*: misra violation

```cpp
     449:    * @param count the number of times to give the counting semaphore.
     450:    * @return counting_semaphore& reference to the counting semaphore.
     451:    */
>>>  452:   counting_semaphore &operator+=(UBaseType_t count) {
     453:     while (count--) {
     454:       give();
     455:     }
```

**Violation 12**: freertos_semaphore.hpp:486:4
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

```cpp
     888:    * @return BaseType_t pdTRUE if the high priority task was woken, otherwise
     889:    * pdFALSE.
     890:    */
>>>  891:   BaseType_t high_priority_task_woken(void) const {
     892:     return m_high_priority_task_woken;
     893:   }
     894:   /**
```

**Violation 15**: freertos_task.hpp:219:20
*Style*: misra violation

```cpp
     216:    * @param priority  priority of the task
     217:    * @param task_routine  task routine function
     218:    */
>>>  219:   task(const char *name, UBaseType_t priority, task_routine_t &&task_routine)
     220:       : m_allocator{}, m_taskRoutine{task_routine},
     221:         m_hTask{m_allocator.create(task_exec, name, priority, this)} {}
     222:   /**
```

**Violation 16**: freertos_task.hpp:848:15
*Style*: misra violation

```cpp
     845:    *
     846:    * @return const char*  task name
     847:    */
>>>  848:   const char *name(void) const { return m_task.name(); }
     849: // Task notification API
     850: #if configUSE_TASK_NOTIFICATIONS
     851:   /**
```

#### MISRA C 2012 Rule 8.2 (45 violation(s))

**Function types shall be in prototype form with named parameters**

Function declarations should include parameter names in addition to types. This improves code readability and helps with documentation.

*Rationale: Named parameters in function prototypes make the interface clearer and improve code documentation.*

**Violation 1**: freertos_message_buffer.hpp:104:26
*Style*: misra violation

```cpp
     101:    * @brief Construct a new message buffer object
     102:    *
     103:    */
>>>  104:   explicit message_buffer() : m_message_buffer{m_allocator.create()} {
     105:     configASSERT(m_message_buffer);
     106:   }
     107:   message_buffer(const message_buffer &) = delete;
```

**Violation 2**: freertos_message_buffer.hpp:107:24
*Style*: misra violation

```cpp
     104:   explicit message_buffer() : m_message_buffer{m_allocator.create()} {
     105:     configASSERT(m_message_buffer);
     106:   }
>>>  107:   message_buffer(const message_buffer &) = delete;
     108:   message_buffer(message_buffer &&src) = delete;
     109:   /**
     110:    * @brief Destruct the message buffer object and delete the message buffer
```

**Violation 3**: freertos_message_buffer.hpp:120:35
*Style*: misra violation

```cpp
     117:     }
     118:   }
     119: 
>>>  120:   message_buffer &operator=(const message_buffer &) = delete;
     121:   message_buffer &operator=(message_buffer &&src) = delete;
     122: 
     123:   /**
```

**Violation 4**: freertos_queue.hpp:206:15
*Style*: misra violation

```cpp
     203:       vQueueAddToRegistry(m_queue, registred_name);
     204:     }
     205:   }
>>>  206:   queue(const queue &) = delete;
     207:   queue(queue &&src) = delete;
     208:   ~queue(void) {
     209:     if (m_queue) {
```

**Violation 5**: freertos_queue.hpp:218:26
*Style*: misra violation

```cpp
     215:     }
     216:   }
     217: 
>>>  218:   queue &operator=(const queue &) = delete;
     219:   queue &operator=(queue &&src) = delete;
     220: 
     221:   /**
```

**Violation 6**: freertos_semaphore.hpp:174:19
*Style*: misra violation

```cpp
     171:    * @brief Construct a new binary semaphore object
     172:    *
     173:    */
>>>  174:   binary_semaphore() : m_semaphore{m_allocator.create_binary()} {
     175:     configASSERT(m_semaphore);
     176:   }
     177:   binary_semaphore(const binary_semaphore &) = delete;
```

**Violation 7**: freertos_semaphore.hpp:177:26
*Style*: misra violation

```cpp
     174:   binary_semaphore() : m_semaphore{m_allocator.create_binary()} {
     175:     configASSERT(m_semaphore);
     176:   }
>>>  177:   binary_semaphore(const binary_semaphore &) = delete;
     178:   binary_semaphore(binary_semaphore &&src) = delete;
     179:   /**
     180:    * @brief Destruct the binary semaphore object and
```

**Violation 8**: freertos_semaphore.hpp:190:37
*Style*: misra violation

```cpp
     187:     }
     188:   }
     189: 
>>>  190:   binary_semaphore &operator=(const binary_semaphore &) = delete;
     191:   binary_semaphore &operator=(binary_semaphore &&src) = delete;
     192: 
     193:   /**
```

**Violation 9**: freertos_semaphore.hpp:201:18
*Style*: misra violation

```cpp
     198:    * otherwise pdFALSE.
     199:    *
     200:    */
>>>  201:   BaseType_t give() { return xSemaphoreGive(m_semaphore); }
     202:   /**
     203:    * @brief Give the binary semaphore from an ISR.
     204:    * @ref https://www.freertos.org/a00124.html
```

**Violation 10**: freertos_semaphore.hpp:299:28
*Style*: misra violation

```cpp
     296:       : m_semaphore{m_allocator.create_counting(max_count)} {
     297:     configASSERT(m_semaphore);
     298:   }
>>>  299:   counting_semaphore(const counting_semaphore &) = delete;
     300:   counting_semaphore(counting_semaphore &&src) = delete;
     301:   /**
     302:    * @brief Destruct the counting semaphore object and
```

**Violation 11**: freertos_semaphore.hpp:312:39
*Style*: misra violation

```cpp
     309:     }
     310:   }
     311: 
>>>  312:   counting_semaphore &operator=(const counting_semaphore &) = delete;
     313:   counting_semaphore &operator=(counting_semaphore &&src) = delete;
     314: 
     315:   /**
```

**Violation 12**: freertos_semaphore.hpp:323:18
*Style*: misra violation

```cpp
     320:    * otherwise pdFALSE.
     321:    *
     322:    */
>>>  323:   BaseType_t give() { return xSemaphoreGive(m_semaphore); }
     324:   /**
     325:    * @brief Give the counting semaphore from an ISR.
     326:    * @ref https://www.freertos.org/a00124.html
```

**Violation 13**: freertos_semaphore.hpp:403:33
*Style*: misra violation

```cpp
     400:    *
     401:    * @return counting_semaphore& reference to the counting semaphore.
     402:    */
>>>  403:   counting_semaphore &operator++() {
     404:     give();
     405:     return *this;
     406:   }
```

**Violation 14**: freertos_semaphore.hpp:412:33
*Style*: misra violation

```cpp
     409:    *
     410:    * @return counting_semaphore& reference to the counting semaphore.
     411:    */
>>>  412:   counting_semaphore &operator--() {
     413:     take();
     414:     return *this;
     415:   }
```

**Violation 15**: freertos_semaphore.hpp:445:20
*Style*: misra violation

```cpp
     442:    *
     443:    * @return UBaseType_t count of the counting semaphore.
     444:    */
>>>  445:   UBaseType_t count() { return uxSemaphoreGetCount(m_semaphore); }
     446:   /**
     447:    * @brief Give the counting semaphore.
     448:    *
```

**Violation 16**: freertos_semaphore.hpp:476:8
*Style*: misra violation

```cpp
     473:    * @brief Construct a new mutex object
     474:    *
     475:    */
>>>  476:   mutex() : m_semaphore{m_allocator.create_mutex()}, m_locked{false} {
     477:     configASSERT(m_semaphore);
     478:   }
     479:   mutex(const mutex &) = delete;
```

**Violation 17**: freertos_semaphore.hpp:479:15
*Style*: misra violation

```cpp
     476:   mutex() : m_semaphore{m_allocator.create_mutex()}, m_locked{false} {
     477:     configASSERT(m_semaphore);
     478:   }
>>>  479:   mutex(const mutex &) = delete;
     480:   mutex(mutex &&src) = delete;
     481:   /**
     482:    * @brief Destruct the mutex object and delete the mutex instance if it was
```

**Violation 18**: freertos_semaphore.hpp:492:26
*Style*: misra violation

```cpp
     489:     }
     490:   }
     491: 
>>>  492:   mutex &operator=(const mutex &) = delete;
     493:   mutex &operator=(mutex &&src) = delete;
     494: 
     495:   /**
```

**Violation 19**: freertos_semaphore.hpp:501:20
*Style*: misra violation

```cpp
     498:    *
     499:    * @return BaseType_t pdTRUE if the mutex was successfully unlocked,
     500:    */
>>>  501:   BaseType_t unlock() {
     502:     auto rc = xSemaphoreGive(m_semaphore);
     503:     if (rc) {
     504:       m_locked = false;
```

**Violation 20**: freertos_semaphore.hpp:597:22
*Style*: misra violation

```cpp
     594:    *
     595:    * @return BaseType_t pdTRUE if the mutex was successfully locked,
     596:    */
>>>  597:   BaseType_t try_lock() {
     598:     auto rc = xSemaphoreTake(m_semaphore, 0);
     599:     if (rc) {
     600:       m_locked = true;
```

**Violation 21**: freertos_semaphore.hpp:628:18
*Style*: misra violation

```cpp
     625:    * @brief Construct a new recursive mutex object
     626:    *
     627:    */
>>>  628:   recursive_mutex() : m_semaphore{m_allocator.create_recursive_mutex()} {
     629:     configASSERT(m_semaphore);
     630:   }
     631:   recursive_mutex(const recursive_mutex &) = delete;
```

**Violation 22**: freertos_semaphore.hpp:631:25
*Style*: misra violation

```cpp
     628:   recursive_mutex() : m_semaphore{m_allocator.create_recursive_mutex()} {
     629:     configASSERT(m_semaphore);
     630:   }
>>>  631:   recursive_mutex(const recursive_mutex &) = delete;
     632:   recursive_mutex(recursive_mutex &&src) = delete;
     633:   /**
     634:    * @brief Destruct the recursive mutex object and delete the recursive mutex
```

**Violation 23**: freertos_semaphore.hpp:644:36
*Style*: misra violation

```cpp
     641:     }
     642:   }
     643: 
>>>  644:   recursive_mutex &operator=(const recursive_mutex &) = delete;
     645:   recursive_mutex &operator=(recursive_mutex &&src) = delete;
     646: 
     647:   /**
```

**Violation 24**: freertos_semaphore.hpp:654:20
*Style*: misra violation

```cpp
     651:    * @return BaseType_t pdTRUE if the recursive mutex was successfully unlocked,
     652:    * otherwise pdFALSE.
     653:    */
>>>  654:   BaseType_t unlock() {
     655:     auto rc = xSemaphoreGiveRecursive(m_semaphore);
     656:     if (rc && m_recursions_count > 0) {
     657:       m_recursions_count--;
```

**Violation 25**: freertos_semaphore.hpp:750:22
*Style*: misra violation

```cpp
     747:    *
     748:    * @return BaseType_t pdTRUE if the recursive mutex was successfully locked,
     749:    */
>>>  750:   BaseType_t try_lock() {
     751:     auto rc = xSemaphoreTakeRecursive(m_semaphore, 0);
     752:     if (rc) {
     753:       m_recursions_count++;
```

**Violation 26**: freertos_semaphore.hpp:796:20
*Style*: misra violation

```cpp
     793:   ~lock_guard(void) { m_mutex.unlock(); }
     794: 
     795:   // Delete copy and move operations for RAII safety
>>>  796:   lock_guard(const lock_guard &) = delete;
     797:   lock_guard(lock_guard &&) = delete;
     798:   lock_guard &operator=(const lock_guard &) = delete;
     799:   lock_guard &operator=(lock_guard &&) = delete;
```

**Violation 27**: freertos_semaphore.hpp:797:14
*Style*: misra violation

```cpp
     794: 
     795:   // Delete copy and move operations for RAII safety
     796:   lock_guard(const lock_guard &) = delete;
>>>  797:   lock_guard(lock_guard &&) = delete;
     798:   lock_guard &operator=(const lock_guard &) = delete;
     799:   lock_guard &operator=(lock_guard &&) = delete;
     800: 
```

**Violation 28**: freertos_semaphore.hpp:798:31
*Style*: misra violation

```cpp
     795:   // Delete copy and move operations for RAII safety
     796:   lock_guard(const lock_guard &) = delete;
     797:   lock_guard(lock_guard &&) = delete;
>>>  798:   lock_guard &operator=(const lock_guard &) = delete;
     799:   lock_guard &operator=(lock_guard &&) = delete;
     800: 
     801:   /**
```

**Violation 29**: freertos_semaphore.hpp:799:25
*Style*: misra violation

```cpp
     796:   lock_guard(const lock_guard &) = delete;
     797:   lock_guard(lock_guard &&) = delete;
     798:   lock_guard &operator=(const lock_guard &) = delete;
>>>  799:   lock_guard &operator=(lock_guard &&) = delete;
     800: 
     801:   /**
     802:    * @brief Checks if the mutex is locked.
```

**Violation 30**: freertos_semaphore.hpp:840:24
*Style*: misra violation

```cpp
     837:   }
     838: 
     839:   // Delete copy and move operations for RAII safety
>>>  840:   try_lock_guard(const try_lock_guard &) = delete;
     841:   try_lock_guard(try_lock_guard &&) = delete;
     842:   try_lock_guard &operator=(const try_lock_guard &) = delete;
     843:   try_lock_guard &operator=(try_lock_guard &&) = delete;
```

**Violation 31**: freertos_semaphore.hpp:841:18
*Style*: misra violation

```cpp
     838: 
     839:   // Delete copy and move operations for RAII safety
     840:   try_lock_guard(const try_lock_guard &) = delete;
>>>  841:   try_lock_guard(try_lock_guard &&) = delete;
     842:   try_lock_guard &operator=(const try_lock_guard &) = delete;
     843:   try_lock_guard &operator=(try_lock_guard &&) = delete;
     844: 
```

**Violation 32**: freertos_semaphore.hpp:842:35
*Style*: misra violation

```cpp
     839:   // Delete copy and move operations for RAII safety
     840:   try_lock_guard(const try_lock_guard &) = delete;
     841:   try_lock_guard(try_lock_guard &&) = delete;
>>>  842:   try_lock_guard &operator=(const try_lock_guard &) = delete;
     843:   try_lock_guard &operator=(try_lock_guard &&) = delete;
     844: 
     845:   /**
```

**Violation 33**: freertos_semaphore.hpp:843:29
*Style*: misra violation

```cpp
     840:   try_lock_guard(const try_lock_guard &) = delete;
     841:   try_lock_guard(try_lock_guard &&) = delete;
     842:   try_lock_guard &operator=(const try_lock_guard &) = delete;
>>>  843:   try_lock_guard &operator=(try_lock_guard &&) = delete;
     844: 
     845:   /**
     846:    * @brief Checks if the mutex is locked.
```

**Violation 34**: freertos_semaphore.hpp:880:24
*Style*: misra violation

```cpp
     877:   ~lock_guard_isr(void) { m_mutex.unlock_isr(m_high_priority_task_woken); }
     878: 
     879:   // Delete copy and move operations for RAII safety
>>>  880:   lock_guard_isr(const lock_guard_isr &) = delete;
     881:   lock_guard_isr(lock_guard_isr &&) = delete;
     882:   lock_guard_isr &operator=(const lock_guard_isr &) = delete;
     883:   lock_guard_isr &operator=(lock_guard_isr &&) = delete;
```

**Violation 35**: freertos_semaphore.hpp:881:18
*Style*: misra violation

```cpp
     878: 
     879:   // Delete copy and move operations for RAII safety
     880:   lock_guard_isr(const lock_guard_isr &) = delete;
>>>  881:   lock_guard_isr(lock_guard_isr &&) = delete;
     882:   lock_guard_isr &operator=(const lock_guard_isr &) = delete;
     883:   lock_guard_isr &operator=(lock_guard_isr &&) = delete;
     884: 
```

**Violation 36**: freertos_semaphore.hpp:882:35
*Style*: misra violation

```cpp
     879:   // Delete copy and move operations for RAII safety
     880:   lock_guard_isr(const lock_guard_isr &) = delete;
     881:   lock_guard_isr(lock_guard_isr &&) = delete;
>>>  882:   lock_guard_isr &operator=(const lock_guard_isr &) = delete;
     883:   lock_guard_isr &operator=(lock_guard_isr &&) = delete;
     884: 
     885:   /**
```

**Violation 37**: freertos_semaphore.hpp:883:29
*Style*: misra violation

```cpp
     880:   lock_guard_isr(const lock_guard_isr &) = delete;
     881:   lock_guard_isr(lock_guard_isr &&) = delete;
     882:   lock_guard_isr &operator=(const lock_guard_isr &) = delete;
>>>  883:   lock_guard_isr &operator=(lock_guard_isr &&) = delete;
     884: 
     885:   /**
     886:    * @brief Checks if high priority task was woken.
```

**Violation 38**: freertos_semaphore.hpp:947:28
*Style*: misra violation

```cpp
     944:   }
     945: 
     946:   // Delete copy and move operations for RAII safety
>>>  947:   timeout_lock_guard(const timeout_lock_guard &) = delete;
     948:   timeout_lock_guard(timeout_lock_guard &&) = delete;
     949:   timeout_lock_guard &operator=(const timeout_lock_guard &) = delete;
     950:   timeout_lock_guard &operator=(timeout_lock_guard &&) = delete;
```

**Violation 39**: freertos_semaphore.hpp:948:22
*Style*: misra violation

```cpp
     945: 
     946:   // Delete copy and move operations for RAII safety
     947:   timeout_lock_guard(const timeout_lock_guard &) = delete;
>>>  948:   timeout_lock_guard(timeout_lock_guard &&) = delete;
     949:   timeout_lock_guard &operator=(const timeout_lock_guard &) = delete;
     950:   timeout_lock_guard &operator=(timeout_lock_guard &&) = delete;
     951: 
```

**Violation 40**: freertos_semaphore.hpp:949:39
*Style*: misra violation

```cpp
     946:   // Delete copy and move operations for RAII safety
     947:   timeout_lock_guard(const timeout_lock_guard &) = delete;
     948:   timeout_lock_guard(timeout_lock_guard &&) = delete;
>>>  949:   timeout_lock_guard &operator=(const timeout_lock_guard &) = delete;
     950:   timeout_lock_guard &operator=(timeout_lock_guard &&) = delete;
     951: 
     952:   /**
```

**Violation 41**: freertos_semaphore.hpp:950:33
*Style*: misra violation

```cpp
     947:   timeout_lock_guard(const timeout_lock_guard &) = delete;
     948:   timeout_lock_guard(timeout_lock_guard &&) = delete;
     949:   timeout_lock_guard &operator=(const timeout_lock_guard &) = delete;
>>>  950:   timeout_lock_guard &operator=(timeout_lock_guard &&) = delete;
     951: 
     952:   /**
     953:    * @brief Checks if the mutex is locked.
```

**Violation 42**: freertos_event_group.hpp:95:21
*Style*: misra violation

```cpp
      92:    *
      93:    */
      94:   event_group(void) : m_event_group(m_allocator.create()) {}
>>>   95:   event_group(const event_group &) = delete;
      96:   event_group(event_group &&other) = delete;
      97:   /**
      98:    * @brief Destruct the event group object and delete the event group instance
```

**Violation 43**: freertos_event_group.hpp:108:32
*Style*: misra violation

```cpp
     105:     }
     106:   }
     107: 
>>>  108:   event_group &operator=(const event_group &) = delete;
     109:   event_group &operator=(event_group &&other) = delete;
     110: 
     111:   /**
```

**Violation 44**: freertos_stream_buffer.hpp:112:23
*Style*: misra violation

```cpp
     109:       : m_stream_buffer{m_allocator.create(trigger_level_bytes)} {
     110:     configASSERT(m_stream_buffer);
     111:   }
>>>  112:   stream_buffer(const stream_buffer &) = delete;
     113:   stream_buffer(stream_buffer &&src) = delete;
     114:   /**
     115:    * @brief Destruct the stream buffer object and delete the stream buffer
```

**Violation 45**: freertos_stream_buffer.hpp:125:34
*Style*: misra violation

```cpp
     122:     }
     123:   }
     124: 
>>>  125:   stream_buffer &operator=(const stream_buffer &) = delete;
     126:   stream_buffer &operator=(stream_buffer &&src) = delete;
     127: 
     128:   /**
```

#### MISRA C 2012 Rule 8.5 (6 violation(s))

**An external object or function shall be declared once in one and only one file**

External objects and functions should have exactly one declaration to avoid multiple definition errors and ensure consistent interfaces.

*Rationale: Multiple declarations can lead to inconsistencies and linking errors.*

**Violation 1**: freertos_task.hpp:1069:6
*Style*: misra violation

```cpp
    1066:  * @tparam Duration duration type
    1067:  * @param timePoint time point to sleep until
    1068:  */
>>> 1069: void delay_until(const std::chrono::system_clock::time_point &wakeTime);
    1070: /**
    1071:  * @brief Sleep the task until the specified time point.
    1072:  *
```

**Violation 2**: freertos_task.hpp:1052:6
*Style*: misra violation

```cpp
    1049:  * @tparam Duration duration type
    1050:  * @param timePoint time point to sleep until
    1051:  */
>>> 1052: void delay_until(TickType_t &previousWakeTime, TickType_t period);
    1053: 
    1054: template <typename Rep, typename Period>
    1055: void delay_until(TickType_t &previousWakeTime,
```

**Violation 3**: freertos_task.hpp:1077:6
*Style*: misra violation

```cpp
    1074:  * @tparam Duration duration type
    1075:  * @param timePoint time point to sleep until
    1076:  */
>>> 1077: void delay_until(const std::chrono::steady_clock::time_point &wakeTime);
    1078: 
    1079: // task utilities:
    1080: 
```

**Violation 4**: freertos_task.hpp:1069:6
*Style*: misra violation

```cpp
    1066:  * @tparam Duration duration type
    1067:  * @param timePoint time point to sleep until
    1068:  */
>>> 1069: void delay_until(const std::chrono::system_clock::time_point &wakeTime);
    1070: /**
    1071:  * @brief Sleep the task until the specified time point.
    1072:  *
```

**Violation 5**: freertos_task.hpp:1052:6
*Style*: misra violation

```cpp
    1049:  * @tparam Duration duration type
    1050:  * @param timePoint time point to sleep until
    1051:  */
>>> 1052: void delay_until(TickType_t &previousWakeTime, TickType_t period);
    1053: 
    1054: template <typename Rep, typename Period>
    1055: void delay_until(TickType_t &previousWakeTime,
```

**Violation 6**: freertos_task.hpp:1077:6
*Style*: misra violation

```cpp
    1074:  * @tparam Duration duration type
    1075:  * @param timePoint time point to sleep until
    1076:  */
>>> 1077: void delay_until(const std::chrono::steady_clock::time_point &wakeTime);
    1078: 
    1079: // task utilities:
    1080: 
```

#### MISRA C 2012 Rule 8.6 (40 violation(s))

**An identifier with external linkage shall have exactly one external definition**

Each external identifier should be defined exactly once across the entire program to avoid multiple definition errors.

*Rationale: Multiple definitions of the same external identifier violate the one definition rule and can cause linking errors.*

**Violation 1**: freertos_task.cc:37:6
*Style*: misra violation

```cpp
      34: 
      35: namespace freertos {
      36: 
>>>   37: void delay(TickType_t ticks) { vTaskDelay(ticks); }
      38: void delay_until(TickType_t &previousWakeTime, TickType_t period) {
      39:   vTaskDelayUntil(&previousWakeTime, period);
      40: }
```

**Violation 2**: freertos_task.hpp:1028:6
*Style*: misra violation

```cpp
    1025:  * @param duration duration to delay
    1026:  */
    1027: template <typename Rep, typename Period>
>>> 1028: void delay(std::chrono::duration<Rep, Period> duration) {
    1029:   delay(
    1030:       std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    1031: }
```

**Violation 3**: freertos_task.cc:38:6
*Style*: misra violation

```cpp
      35: namespace freertos {
      36: 
      37: void delay(TickType_t ticks) { vTaskDelay(ticks); }
>>>   38: void delay_until(TickType_t &previousWakeTime, TickType_t period) {
      39:   vTaskDelayUntil(&previousWakeTime, period);
      40: }
      41: void delay_until(const std::chrono::system_clock::time_point &wakeTime) {
```

**Violation 4**: freertos_task.hpp:1055:6
*Style*: misra violation

```cpp
    1052: void delay_until(TickType_t &previousWakeTime, TickType_t period);
    1053: 
    1054: template <typename Rep, typename Period>
>>> 1055: void delay_until(TickType_t &previousWakeTime,
    1056:                  std::chrono::duration<Rep, Period> period) {
    1057:   delay_until(
    1058:       previousWakeTime,
```

**Violation 5**: freertos_message_buffer.hpp:120:19
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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

**Violation 20**: freertos_queue.hpp:244:14
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

```cpp
     213:    *
     214:    * @return BaseType_t pdTRUE if the message buffer is full, pdFALSE otherwise
     215:    */
>>>  216:   BaseType_t full(void) { return xMessageBufferIsFull(m_message_buffer); }
     217: };
     218: 
     219: #if configSUPPORT_STATIC_ALLOCATION
```

**Violation 36**: freertos_task.hpp:1214:21
*Style*: misra violation

```cpp
    1211:   // Delete copy and move operations for RAII safety
    1212:   critical_section(const critical_section &) = delete;
    1213:   critical_section(critical_section &&) = delete;
>>> 1214:   critical_section &operator=(const critical_section &) = delete;
    1215:   critical_section &operator=(critical_section &&) = delete;
    1216: };
    1217: 
```

**Violation 37**: freertos_stream_buffer.hpp:126:18
*Style*: misra violation

```cpp
     123:   }
     124: 
     125:   stream_buffer &operator=(const stream_buffer &) = delete;
>>>  126:   stream_buffer &operator=(stream_buffer &&src) = delete;
     127: 
     128:   /**
     129:    * @brief Send data to the stream buffer.
```

**Violation 38**: freertos_task.hpp:267:16
*Style*: misra violation

```cpp
     264:    *
     265:    * @return TaskHandle_t task handle
     266:    */
>>>  267:   TaskHandle_t handle(void) const { return m_hTask; }
     268: #if INCLUDE_vTaskSuspend
     269:   /**
     270:    * @brief Suspend the task.
```

**Violation 39**: freertos_task.hpp:421:15
*Style*: misra violation

```cpp
     418:    *
     419:    * @return const char* task name
     420:    */
>>>  421:   const char *name(void) const { return pcTaskGetName(m_hTask); }
     422: // Task notification API
     423: #if configUSE_TASK_NOTIFICATIONS
     424:   /**
```

**Violation 40**: freertos_queue.hpp:668:15
*Style*: misra violation

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

**Violation 1**: freertos_task.hpp:728:8
*Style*: misra violation

```cpp
     725:    *
     726:    * @return bool true if the task is running, false otherwise
     727:    */
>>>  728:   bool is_running(void) const {
     729:     switch (m_task.state()) {
     730:     case eRunning:
     731:     case eReady:
```

**Violation 2**: freertos_task.hpp:704:16
*Style*: misra violation

```cpp
     701:    *
     702:    * @return TaskHandle_t task handle
     703:    */
>>>  704:   TaskHandle_t handle(void) const { return m_task.handle(); }
     705: #if INCLUDE_vTaskSuspend
     706:   /**
     707:    * @brief Suspend the task.
```

**Violation 3**: freertos_task.hpp:743:8
*Style*: misra violation

```cpp
     740:    * @brief Terminate the task.
     741:    *
     742:    */
>>>  743:   void terminate(void) { m_task.terminate(); }
     744: #if INCLUDE_xTaskAbortDelay
     745:   /**
     746:    * @brief Abort the delay of the task.
```

**Violation 4**: freertos_task.cc:59:12
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

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
*Style*: misra violation

```cpp
     324:    * @param data_size Maximum number of bytes to copy into the buffer.
     325:    * @return size_t Number of bytes received.
     326:    */
>>>  327:   size_t receive_isr(void *data, size_t data_size) {
     328:     BaseType_t higher_priority_task_woken = pdFALSE;
     329:     return receive_isr(data, data_size, higher_priority_task_woken);
     330:   }
```

**Violation 8**: freertos_task.hpp:728:8
*Style*: misra violation

```cpp
     725:    *
     726:    * @return bool true if the task is running, false otherwise
     727:    */
>>>  728:   bool is_running(void) const {
     729:     switch (m_task.state()) {
     730:     case eRunning:
     731:     case eReady:
```

**Violation 9**: freertos_task.hpp:704:16
*Style*: misra violation

```cpp
     701:    *
     702:    * @return TaskHandle_t task handle
     703:    */
>>>  704:   TaskHandle_t handle(void) const { return m_task.handle(); }
     705: #if INCLUDE_vTaskSuspend
     706:   /**
     707:    * @brief Suspend the task.
```

**Violation 10**: freertos_task.hpp:743:8
*Style*: misra violation

```cpp
     740:    * @brief Terminate the task.
     741:    *
     742:    */
>>>  743:   void terminate(void) { m_task.terminate(); }
     744: #if INCLUDE_xTaskAbortDelay
     745:   /**
     746:    * @brief Abort the delay of the task.
```

**Violation 11**: freertos_task.hpp:728:8
*Style*: misra violation

```cpp
     725:    *
     726:    * @return bool true if the task is running, false otherwise
     727:    */
>>>  728:   bool is_running(void) const {
     729:     switch (m_task.state()) {
     730:     case eRunning:
     731:     case eReady:
```

**Violation 12**: freertos_task.hpp:704:16
*Style*: misra violation

```cpp
     701:    *
     702:    * @return TaskHandle_t task handle
     703:    */
>>>  704:   TaskHandle_t handle(void) const { return m_task.handle(); }
     705: #if INCLUDE_vTaskSuspend
     706:   /**
     707:    * @brief Suspend the task.
```

**Violation 13**: freertos_task.hpp:743:8
*Style*: misra violation

```cpp
     740:    * @brief Terminate the task.
     741:    *
     742:    */
>>>  743:   void terminate(void) { m_task.terminate(); }
     744: #if INCLUDE_xTaskAbortDelay
     745:   /**
     746:    * @brief Abort the delay of the task.
```

**Violation 14**: freertos_stream_buffer.hpp:264:14
*Style*: misra violation

```cpp
     261:    * insufficient memory available to copy the data into the stream buffer.
     262:    */
     263:   template <typename Iterator>
>>>  264:   BaseType_t send_isr(Iterator begin, Iterator end) {
     265:     BaseType_t higher_priority_task_woken = pdFALSE;
     266:     return send_isr(&*begin, std::distance(begin, end),
     267:                     higher_priority_task_woken);
```

**Violation 15**: freertos_stream_buffer.hpp:327:10
*Style*: misra violation

```cpp
     324:    * @param data_size Maximum number of bytes to copy into the buffer.
     325:    * @return size_t Number of bytes received.
     326:    */
>>>  327:   size_t receive_isr(void *data, size_t data_size) {
     328:     BaseType_t higher_priority_task_woken = pdFALSE;
     329:     return receive_isr(data, data_size, higher_priority_task_woken);
     330:   }
```

### Analysis Errors

Some files could not be fully analyzed:

```
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc:0:0: error: Bailing out from analysis: Checking file failed: Failed to execute addon 'misra' - exitcode is 1 [internalError]
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos.hpp:0:0: error: Bailing out from analysis: Checking file failed: Failed to execute addon 'misra' - exitcode is 1 [internalError]
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:0:0: error: Bailing out from analysis: Checking file failed: Failed to execute addon 'misra' - exitcode is 1 [internalError]
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

**Violation 1**: freertos_task.hpp:914:37
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     911:    * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
     912:    */
     913:   BaseType_t notify_isr(const uint32_t val, eNotifyAction action,
>>>  914:                         BaseType_t &higherPriorityTaskWoken) {
     915:     return m_task.notify_isr(val, action, higherPriorityTaskWoken);
     916:   }
     917:   /**
```

**Violation 2**: freertos_task.hpp:939:47
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     936:    */
     937:   BaseType_t notify_and_query_isr(const uint32_t val, eNotifyAction action,
     938:                                   uint32_t &prev_value,
>>>  939:                                   BaseType_t &higherPriorityTaskWoken) {
     940:     return m_task.notify_and_query_isr(val, action, prev_value,
     941:                                        higherPriorityTaskWoken);
     942:   }
```

**Violation 3**: freertos_task.hpp:914:37
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     911:    * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
     912:    */
     913:   BaseType_t notify_isr(const uint32_t val, eNotifyAction action,
>>>  914:                         BaseType_t &higherPriorityTaskWoken) {
     915:     return m_task.notify_isr(val, action, higherPriorityTaskWoken);
     916:   }
     917:   /**
```

**Violation 4**: freertos_task.hpp:939:47
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     936:    */
     937:   BaseType_t notify_and_query_isr(const uint32_t val, eNotifyAction action,
     938:                                   uint32_t &prev_value,
>>>  939:                                   BaseType_t &higherPriorityTaskWoken) {
     940:     return m_task.notify_and_query_isr(val, action, prev_value,
     941:                                        higherPriorityTaskWoken);
     942:   }
```

**Violation 5**: freertos_task.hpp:914:37
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     911:    * @return BaseType_t  pdTRUE if the notification was given, pdFALSE otherwise
     912:    */
     913:   BaseType_t notify_isr(const uint32_t val, eNotifyAction action,
>>>  914:                         BaseType_t &higherPriorityTaskWoken) {
     915:     return m_task.notify_isr(val, action, higherPriorityTaskWoken);
     916:   }
     917:   /**
```

**Violation 6**: freertos_task.hpp:939:47
*Style*: Parameter 'higherPriorityTaskWoken' can be declared as reference to const

```cpp
     936:    */
     937:   BaseType_t notify_and_query_isr(const uint32_t val, eNotifyAction action,
     938:                                   uint32_t &prev_value,
>>>  939:                                   BaseType_t &higherPriorityTaskWoken) {
     940:     return m_task.notify_and_query_isr(val, action, prev_value,
     941:                                        higherPriorityTaskWoken);
     942:   }
```

#### Other (10 violation(s))

##### shadowFunction

**Violation 1**: freertos_sw_timer.hpp:202:14
*Style*: Local variable 'name' shadows outer function

```cpp
     199:         while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
     200:           vTaskDelay(pdMS_TO_TICKS(1));
     201:         }
>>>  202:         auto name = pcTimerGetName(src.m_timer);
     203:         auto period = xTimerGetPeriod(src.m_timer);
     204:         auto auto_reload = uxTimerGetReloadMode(src.m_timer);
     205:         rc = xTimerDelete(src.m_timer, portMAX_DELAY);
```

**Violation 2**: freertos_sw_timer.hpp:203:14
*Style*: Local variable 'period' shadows outer function

```cpp
     200:           vTaskDelay(pdMS_TO_TICKS(1));
     201:         }
     202:         auto name = pcTimerGetName(src.m_timer);
>>>  203:         auto period = xTimerGetPeriod(src.m_timer);
     204:         auto auto_reload = uxTimerGetReloadMode(src.m_timer);
     205:         rc = xTimerDelete(src.m_timer, portMAX_DELAY);
     206:         if (rc == pdPASS) {
```

**Violation 3**: freertos_sw_timer.hpp:202:14
*Style*: Local variable 'name' shadows outer function

```cpp
     199:         while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
     200:           vTaskDelay(pdMS_TO_TICKS(1));
     201:         }
>>>  202:         auto name = pcTimerGetName(src.m_timer);
     203:         auto period = xTimerGetPeriod(src.m_timer);
     204:         auto auto_reload = uxTimerGetReloadMode(src.m_timer);
     205:         rc = xTimerDelete(src.m_timer, portMAX_DELAY);
```

**Violation 4**: freertos_sw_timer.hpp:203:14
*Style*: Local variable 'period' shadows outer function

```cpp
     200:           vTaskDelay(pdMS_TO_TICKS(1));
     201:         }
     202:         auto name = pcTimerGetName(src.m_timer);
>>>  203:         auto period = xTimerGetPeriod(src.m_timer);
     204:         auto auto_reload = uxTimerGetReloadMode(src.m_timer);
     205:         rc = xTimerDelete(src.m_timer, portMAX_DELAY);
     206:         if (rc == pdPASS) {
```

##### uninitMemberVar

**Violation 1**: freertos_message_buffer.hpp:57:3
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

**Violation 2**: freertos_queue.hpp:59:3
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

**Violation 3**: freertos_stream_buffer.hpp:58:3
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

**Violation 4**: freertos_message_buffer.hpp:57:3
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

**Violation 5**: freertos_queue.hpp:59:3
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

**Violation 6**: freertos_stream_buffer.hpp:58:3
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

#### Unused Code (84 violation(s))

##### unusedFunction

**Violation 1**: freertos_task.hpp:1041:0
*Style*: The function 'sleep_for' is never used.

```cpp
    1038:  * @param duration duration to sleep
    1039:  */
    1040: template <typename Rep, typename Period>
>>> 1041: void sleep_for(std::chrono::duration<Rep, Period> duration) {
    1042:   delay(duration);
    1043: }
    1044: 
```

**Violation 2**: freertos_task.hpp:1108:0
*Style*: The function 'total_run_time' is never used.

```cpp
    1105:    * @return UBaseType_t number of tasks
    1106:    */
    1107:   UBaseType_t count(void) const { return m_task_count; }
>>> 1108:   std::chrono::milliseconds total_run_time(void) const {
    1109:     return std::chrono::milliseconds{m_total_run_time};
    1110:   }
    1111:   /**
```

**Violation 3**: freertos_task.hpp:1116:0
*Style*: The function 'begin' is never used.

```cpp
    1113:    *
    1114:    * @return const TaskStatus_t* begin iterator
    1115:    */
>>> 1116:   const TaskStatus_t *begin(void) const { return m_status_array.data(); }
    1117:   /**
    1118:    * @brief Return the end iterator of the task status array.
    1119:    *
```

**Violation 4**: freertos_task.hpp:1122:0
*Style*: The function 'end' is never used.

```cpp
    1119:    *
    1120:    * @return const TaskStatus_t* end iterator
    1121:    */
>>> 1122:   const TaskStatus_t *end(void) const {
    1123:     return m_status_array.data() + m_task_count;
    1124:   }
    1125: };
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

**Violation 11**: freertos_event_group.hpp:126:0
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

**Violation 12**: freertos_event_group.hpp:136:0
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

**Violation 13**: freertos_event_group.hpp:150:0
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

**Violation 14**: freertos_event_group.hpp:198:0
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

**Violation 15**: freertos_event_group.hpp:205:0
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

**Violation 16**: freertos_message_buffer.hpp:193:0
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

**Violation 17**: freertos_message_buffer.hpp:209:0
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

**Violation 18**: freertos_message_buffer.hpp:216:0
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

**Violation 19**: freertos_queue.hpp:316:0
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

**Violation 20**: freertos_queue.hpp:372:0
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

**Violation 21**: freertos_queue.hpp:497:0
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

**Violation 22**: freertos_queue.hpp:504:0
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

**Violation 23**: freertos_queue.hpp:513:0
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

**Violation 24**: freertos_queue.hpp:529:0
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

**Violation 25**: freertos_queue.hpp:542:0
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

**Violation 26**: freertos_queue.hpp:598:0
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

**Violation 27**: freertos_queue.hpp:675:0
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

**Violation 28**: freertos_queue.hpp:682:0
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

**Violation 29**: freertos_semaphore.hpp:211:0
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

**Violation 30**: freertos_semaphore.hpp:248:0
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

**Violation 31**: freertos_semaphore.hpp:768:0
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

**Violation 32**: freertos_semaphore.hpp:891:0
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

**Violation 33**: freertos_stream_buffer.hpp:346:0
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

**Violation 34**: freertos_stream_buffer.hpp:362:0
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

**Violation 35**: freertos_sw_timer.hpp:535:0
*Style*: The function 'reload_mode' is never used.

```cpp
     532:    * @param auto_reload pdTRUE to enable auto-reload mode, pdFALSE to disable
     533:    * @return timer& reference to the timer object
     534:    */
>>>  535:   timer &reload_mode(UBaseType_t auto_reload) {
     536:     if (m_timer) {
     537:       vTimerSetReloadMode(m_timer, auto_reload);
     538:     }
```

**Violation 36**: freertos_sw_timer.hpp:567:0
*Style*: The function 'remaining_time' is never used.

```cpp
     564:    * @return std::chrono::milliseconds remaining time before the timer expires
     565:    * in milliseconds.
     566:    */
>>>  567:   std::chrono::milliseconds remaining_time(void) const {
     568:     return std::chrono::milliseconds{remaining_ticks()};
     569:   }
     570:   /**
```

**Violation 37**: freertos_sw_timer.hpp:576:0
*Style*: The function 'running' is never used.

```cpp
     573:    *
     574:    * @return BaseType_t pdTRUE if the timer is running, pdFALSE otherwise
     575:    */
>>>  576:   BaseType_t running(void) const {
     577:     if (!m_timer) {
     578:       return pdFALSE;
     579:     }
```

**Violation 38**: freertos_task.hpp:1041:0
*Style*: The function 'sleep_for' is never used.

```cpp
    1038:  * @param duration duration to sleep
    1039:  */
    1040: template <typename Rep, typename Period>
>>> 1041: void sleep_for(std::chrono::duration<Rep, Period> duration) {
    1042:   delay(duration);
    1043: }
    1044: 
```

**Violation 39**: freertos_task.hpp:1108:0
*Style*: The function 'total_run_time' is never used.

```cpp
    1105:    * @return UBaseType_t number of tasks
    1106:    */
    1107:   UBaseType_t count(void) const { return m_task_count; }
>>> 1108:   std::chrono::milliseconds total_run_time(void) const {
    1109:     return std::chrono::milliseconds{m_total_run_time};
    1110:   }
    1111:   /**
```

**Violation 40**: freertos_task.hpp:1116:0
*Style*: The function 'begin' is never used.

```cpp
    1113:    *
    1114:    * @return const TaskStatus_t* begin iterator
    1115:    */
>>> 1116:   const TaskStatus_t *begin(void) const { return m_status_array.data(); }
    1117:   /**
    1118:    * @brief Return the end iterator of the task status array.
    1119:    *
```

**Violation 41**: freertos_task.hpp:1122:0
*Style*: The function 'end' is never used.

```cpp
    1119:    *
    1120:    * @return const TaskStatus_t* end iterator
    1121:    */
>>> 1122:   const TaskStatus_t *end(void) const {
    1123:     return m_status_array.data() + m_task_count;
    1124:   }
    1125: };
```

**Violation 42**: freertos_message_buffer.hpp:193:0
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

**Violation 43**: freertos_message_buffer.hpp:202:0
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

**Violation 44**: freertos_message_buffer.hpp:209:0
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

**Violation 45**: freertos_message_buffer.hpp:216:0
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

**Violation 46**: freertos_queue.hpp:261:0
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

**Violation 47**: freertos_queue.hpp:316:0
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

**Violation 48**: freertos_queue.hpp:372:0
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

**Violation 49**: freertos_queue.hpp:459:0
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

**Violation 53**: freertos_queue.hpp:520:0
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

**Violation 54**: freertos_queue.hpp:529:0
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

**Violation 55**: freertos_queue.hpp:542:0
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

**Violation 56**: freertos_queue.hpp:598:0
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

**Violation 57**: freertos_queue.hpp:668:0
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

**Violation 58**: freertos_queue.hpp:675:0
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

**Violation 59**: freertos_queue.hpp:682:0
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

**Violation 60**: freertos_semaphore.hpp:211:0
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

**Violation 61**: freertos_semaphore.hpp:248:0
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

**Violation 62**: freertos_semaphore.hpp:768:0
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

**Violation 63**: freertos_semaphore.hpp:891:0
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

**Violation 64**: freertos_task.hpp:1041:0
*Style*: The function 'sleep_for' is never used.

```cpp
    1038:  * @param duration duration to sleep
    1039:  */
    1040: template <typename Rep, typename Period>
>>> 1041: void sleep_for(std::chrono::duration<Rep, Period> duration) {
    1042:   delay(duration);
    1043: }
    1044: 
```

**Violation 65**: freertos_task.hpp:1108:0
*Style*: The function 'total_run_time' is never used.

```cpp
    1105:    * @return UBaseType_t number of tasks
    1106:    */
    1107:   UBaseType_t count(void) const { return m_task_count; }
>>> 1108:   std::chrono::milliseconds total_run_time(void) const {
    1109:     return std::chrono::milliseconds{m_total_run_time};
    1110:   }
    1111:   /**
```

**Violation 66**: freertos_task.hpp:1116:0
*Style*: The function 'begin' is never used.

```cpp
    1113:    *
    1114:    * @return const TaskStatus_t* begin iterator
    1115:    */
>>> 1116:   const TaskStatus_t *begin(void) const { return m_status_array.data(); }
    1117:   /**
    1118:    * @brief Return the end iterator of the task status array.
    1119:    *
```

**Violation 67**: freertos_task.hpp:1122:0
*Style*: The function 'end' is never used.

```cpp
    1119:    *
    1120:    * @return const TaskStatus_t* end iterator
    1121:    */
>>> 1122:   const TaskStatus_t *end(void) const {
    1123:     return m_status_array.data() + m_task_count;
    1124:   }
    1125: };
```

**Violation 68**: freertos_sw_timer.hpp:535:0
*Style*: The function 'reload_mode' is never used.

```cpp
     532:    * @param auto_reload pdTRUE to enable auto-reload mode, pdFALSE to disable
     533:    * @return timer& reference to the timer object
     534:    */
>>>  535:   timer &reload_mode(UBaseType_t auto_reload) {
     536:     if (m_timer) {
     537:       vTimerSetReloadMode(m_timer, auto_reload);
     538:     }
```

**Violation 69**: freertos_sw_timer.hpp:567:0
*Style*: The function 'remaining_time' is never used.

```cpp
     564:    * @return std::chrono::milliseconds remaining time before the timer expires
     565:    * in milliseconds.
     566:    */
>>>  567:   std::chrono::milliseconds remaining_time(void) const {
     568:     return std::chrono::milliseconds{remaining_ticks()};
     569:   }
     570:   /**
```

**Violation 70**: freertos_sw_timer.hpp:576:0
*Style*: The function 'running' is never used.

```cpp
     573:    *
     574:    * @return BaseType_t pdTRUE if the timer is running, pdFALSE otherwise
     575:    */
>>>  576:   BaseType_t running(void) const {
     577:     if (!m_timer) {
     578:       return pdFALSE;
     579:     }
```

**Violation 71**: freertos_sw_timer.hpp:587:0
*Style*: The function 'name' is never used.

```cpp
     584:    *
     585:    * @return const char* name of the timer
     586:    */
>>>  587:   const char *name(void) const {
     588:     if (!m_timer) {
     589:       return nullptr;
     590:     }
```

**Violation 72**: freertos_event_group.hpp:117:0
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

**Violation 73**: freertos_event_group.hpp:126:0
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

**Violation 74**: freertos_event_group.hpp:136:0
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

**Violation 75**: freertos_event_group.hpp:150:0
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

**Violation 76**: freertos_event_group.hpp:198:0
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

**Violation 77**: freertos_event_group.hpp:205:0
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

**Violation 78**: freertos_stream_buffer.hpp:337:0
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

**Violation 79**: freertos_stream_buffer.hpp:346:0
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

**Violation 80**: freertos_stream_buffer.hpp:353:0
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

**Violation 81**: freertos_stream_buffer.hpp:362:0
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

**Violation 82**: freertos_stream_buffer.hpp:370:0
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

**Violation 83**: freertos_stream_buffer.hpp:377:0
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

**Violation 84**: freertos_stream_buffer.hpp:384:0
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

```
[1/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc.
78381 warnings generated.
[2/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos.hpp.
156778 warnings generated.
[3/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_event_group.hpp.
235172 warnings generated.
[4/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp.
313564 warnings generated.
[5/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp.
391961 warnings generated.
[6/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp.
470355 warnings generated.
[7/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp.
548747 warnings generated.
[8/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp.
627141 warnings generated.
[9/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp.
705521 warnings generated.
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:424:3: warning: overloaded 'operator++' returns a reference instead of a constant object type [cert-dcl21-cpp]
  424 |   counting_semaphore &operator++(int) {
      |   ^~~~~~~~~~~~~~~~~~~~
      |   const counting_semaphore<SemaphoreAllocator> 
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:436:3: warning: overloaded 'operator--' returns a reference instead of a constant object type [cert-dcl21-cpp]
  436 |   counting_semaphore &operator--(int) {
      |   ^~~~~~~~~~~~~~~~~~~~
      |   const counting_semaphore<SemaphoreAllocator> 
Suppressed 706534 warnings (705517 in non-user code, 1017 NOLINT).
Use -header-filter=.* to display errors from all non-system headers. Use -system-headers to display errors from system headers as well.
```

---
*Generated: July 25, 2025*
*Tools: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)*
*Scope: Library modules only*
