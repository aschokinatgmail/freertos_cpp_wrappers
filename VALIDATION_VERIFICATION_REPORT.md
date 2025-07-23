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

- **MISRA C++ 2012 Violations**: 259
- **Style Issues**: 353
- **Warnings**: 6
- **Errors**: 3

#### Detailed Violations

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

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
*Severity*: Style

```cpp
     324:    * @param data_size Maximum number of bytes to copy into the buffer.
     325:    * @return size_t Number of bytes received.
     326:    */
>>>  327:   size_t receive_isr(void *data, size_t data_size) {
     328:     BaseType_t higher_priority_task_woken = pdFALSE;
     329:     return receive_isr(data, data_size, higher_priority_task_woken);
     330:   }
```

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

#### clang-diagnostic-error (9 violation(s))

**Violation 1**: freertos_event_group.hpp:39:10
*Severity*: Error
*Message*: 'FreeRTOS.h' file not found

```cpp
      36: #error "This header is for C++ only"
      37: #endif
      38: 
>>>   39: #include <FreeRTOS.h>
      40: #include <chrono>
      41: #include <event_groups.h>
      42: 
```

**Violation 2**: freertos_event_group.hpp:39:10
*Severity*: Error
*Message*: 'FreeRTOS.h' file not found

```cpp
      36: #error "This header is for C++ only"
      37: #endif
      38: 
>>>   39: #include <FreeRTOS.h>
      40: #include <chrono>
      41: #include <event_groups.h>
      42: 
```

**Violation 3**: freertos_message_buffer.hpp:39:10
*Severity*: Error
*Message*: 'FreeRTOS.h' file not found

```cpp
      36: #error "This header is for C++ only"
      37: #endif
      38: 
>>>   39: #include <FreeRTOS.h>
      40: #include <array>
      41: #include <chrono>
      42: #include <message_buffer.h>
```

**Violation 4**: freertos_queue.hpp:39:10
*Severity*: Error
*Message*: 'FreeRTOS.h' file not found

```cpp
      36: #error "This header is for C++ only"
      37: #endif
      38: 
>>>   39: #include <FreeRTOS.h>
      40: #include <array>
      41: #include <chrono>
      42: #include <optional>
```

**Violation 5**: freertos_semaphore.hpp:39:10
*Severity*: Error
*Message*: 'FreeRTOS.h' file not found

```cpp
      36: #error "This header is for C++ only"
      37: #endif
      38: 
>>>   39: #include <FreeRTOS.h>
      40: #include <chrono>
      41: #include <ctime>
      42: #include <semphr.h>
```

**Violation 6**: freertos_stream_buffer.hpp:39:10
*Severity*: Error
*Message*: 'FreeRTOS.h' file not found

```cpp
      36: #error "This header is for C++ only"
      37: #endif
      38: 
>>>   39: #include <FreeRTOS.h>
      40: #include <array>
      41: #include <chrono>
      42: #include <optional>
```

**Violation 7**: freertos_sw_timer.hpp:39:10
*Severity*: Error
*Message*: 'FreeRTOS.h' file not found

```cpp
      36: #error "This header is for C++ only"
      37: #endif
      38: 
>>>   39: #include <FreeRTOS.h>
      40: #include <chrono>
      41: #include <cstdbool>
      42: #include <ctime>
```

**Violation 8**: freertos_task.hpp:39:10
*Severity*: Error
*Message*: 'FreeRTOS.h' file not found

```cpp
      36: #error "This header is for C++ only"
      37: #endif
      38: 
>>>   39: #include <FreeRTOS.h>
      40: #include <array>
      41: #include <cassert>
      42: #include <chrono>
```

**Violation 9**: freertos_task.hpp:39:10
*Severity*: Error
*Message*: 'FreeRTOS.h' file not found

```cpp
      36: #error "This header is for C++ only"
      37: #endif
      38: 
>>>   39: #include <FreeRTOS.h>
      40: #include <array>
      41: #include <cassert>
      42: #include <chrono>
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


