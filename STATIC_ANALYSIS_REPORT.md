# Static Analysis Report

## Overview

**Static Analysis Tools**: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)
**Analysis Scope**: Library modules only - src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-* + All cppcheck rules (style, performance, portability, security, etc.) + MISRA C 2012 (applicable to C++)

## clang-tidy Analysis

## Statistics

### Summary

- **Total Issues Found**: 34438
- **Warnings**: 34437
- **Errors**: 1
- **Files Analyzed**: 9
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

- **Total MISRA Violations**: 0
- **Unique Rules Violated**: 0
- **Files Analyzed**: 0
- **Analysis Errors**: 0
- **Other Issues**: 0

### Analysis Notes

- **Standard**: MISRA C 2012 (applicable rules for C++)
- **Tool**: cppcheck with MISRA addon
- **Scope**: Library modules only (src/, include/)
- **Rule texts**: Cannot be displayed due to MISRA licensing restrictions
- **Compatibility**: Many MISRA C 2012 rules overlap with MISRA C++ 2008 requirements
- **Integration**: This analysis complements the existing clang-tidy static analysis

## Enhanced cppcheck Analysis (All Rules)

### Summary

- **Total Violations**: 0
- **Unique Rules Violated**: 0
- **Files Analyzed**: 0
- **Analysis Errors**: 0

### Analysis Notes

- **Tool**: cppcheck with all rules enabled (--enable=all)
- **Checks**: All available cppcheck checks including MISRA C 2012, style, performance, portability, security
- **Scope**: Library modules only (src/, include/)
- **Integration**: This analysis complements the existing clang-tidy static analysis
- **MISRA Compliance**: MISRA rule texts cannot be displayed due to licensing restrictions

## Detailed clang-tidy Analysis

### Actionable Issues with Code Context (18 issues)

#### C++ Core Guidelines (16 issue(s))

##### cppcoreguidelines-rvalue-reference-param-not-moved (16 issue(s))

**Issue 1**: freertos_sw_timer.hpp:129:62
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'callback' is never moved from inside the function body

```cpp
     126:    * @param callback callback routine
     127:    */
     128:   explicit timer(const char *name, const TickType_t period_ticks,
>>>  129:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     130:       : m_callback{std::move(callback)}, m_started{false},
     131:         m_timer{m_allocator.create(name, period_ticks, auto_reload, this,
     132:                                    callback_wrapper)} {
```

**Issue 2**: freertos_sw_timer.hpp:148:62
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'callback' is never moved from inside the function body

```cpp
     145:   template <typename Rep, typename Period>
     146:   explicit timer(const char *name,
     147:                  const std::chrono::duration<Rep, Period> &period,
>>>  148:                  UBaseType_t auto_reload, timer_callback_t &&callback)
     149:       : timer{name,
     150:               static_cast<TickType_t>(
     151:                   std::chrono::duration_cast<std::chrono::milliseconds>(period)
```

**Issue 3**: freertos_task.hpp:194:65
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'task_routine' is never moved from inside the function body

```cpp
     191:    * @param task_routine task routine function
     192:    * @param start_suspended start the task in the suspended state
     193:    */
>>>  194:   task(const char *name, UBaseType_t priority, task_routine_t &&task_routine,
     195:        bool start_suspended = true)
     196:       : m_allocator{}, m_taskRoutine{std::move(task_routine)},
     197:         m_start_suspended{start_suspended},
```

**Issue 4**: freertos_task.hpp:208:25
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'task_routine' is never moved from inside the function body

```cpp
     205:    * @param start_suspended start the task in the suspended state
     206:    */
     207:   task(const std::string &name, UBaseType_t priority,
>>>  208:        task_routine_t &&task_routine, bool start_suspended = true)
     209:       : task{name.c_str(), priority, std::move(task_routine), start_suspended} {
     210:   }
     211: #else
```

**Issue 5**: freertos_task.hpp:604:34
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'on_start' is never moved from inside the function body

```cpp
     601:    */
     602:   template <typename Rep, typename Period>
     603:   periodic_task(const char *name, UBaseType_t priority,
>>>  604:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     605:                 task_routine_t &&periodic_routine,
     606:                 const std::chrono::duration<Rep, Period> &period,
     607:                 bool start_suspended = true)
```

**Issue 6**: freertos_task.hpp:604:61
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'on_stop' is never moved from inside the function body

```cpp
     601:    */
     602:   template <typename Rep, typename Period>
     603:   periodic_task(const char *name, UBaseType_t priority,
>>>  604:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     605:                 task_routine_t &&periodic_routine,
     606:                 const std::chrono::duration<Rep, Period> &period,
     607:                 bool start_suspended = true)
```

**Issue 7**: freertos_task.hpp:605:34
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'periodic_routine' is never moved from inside the function body

```cpp
     602:   template <typename Rep, typename Period>
     603:   periodic_task(const char *name, UBaseType_t priority,
     604:                 task_routine_t &&on_start, task_routine_t &&on_stop,
>>>  605:                 task_routine_t &&periodic_routine,
     606:                 const std::chrono::duration<Rep, Period> &period,
     607:                 bool start_suspended = true)
     608:       : m_period{std::chrono::duration_cast<std::chrono::milliseconds>(period)},
```

**Issue 8**: freertos_task.hpp:627:34
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'on_start' is never moved from inside the function body

```cpp
     624:    */
     625:   template <typename Rep, typename Period>
     626:   periodic_task(const std::string &name, UBaseType_t priority,
>>>  627:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     628:                 task_routine_t &&periodic_routine,
     629:                 const std::chrono::duration<Rep, Period> &period,
     630:                 bool start_suspended = true)
```

**Issue 9**: freertos_task.hpp:627:61
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'on_stop' is never moved from inside the function body

```cpp
     624:    */
     625:   template <typename Rep, typename Period>
     626:   periodic_task(const std::string &name, UBaseType_t priority,
>>>  627:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     628:                 task_routine_t &&periodic_routine,
     629:                 const std::chrono::duration<Rep, Period> &period,
     630:                 bool start_suspended = true)
```

**Issue 10**: freertos_task.hpp:628:34
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'periodic_routine' is never moved from inside the function body

```cpp
     625:   template <typename Rep, typename Period>
     626:   periodic_task(const std::string &name, UBaseType_t priority,
     627:                 task_routine_t &&on_start, task_routine_t &&on_stop,
>>>  628:                 task_routine_t &&periodic_routine,
     629:                 const std::chrono::duration<Rep, Period> &period,
     630:                 bool start_suspended = true)
     631:       : periodic_task{name.c_str(),
```

**Issue 11**: freertos_task.hpp:649:34
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'on_start' is never moved from inside the function body

```cpp
     646:    * @param start_suspended  start the task in the suspended state
     647:    */
     648:   periodic_task(const char *name, UBaseType_t priority,
>>>  649:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     650:                 task_routine_t &&periodic_routine, bool start_suspended = true)
     651:       : periodic_task{name,
     652:                       priority,
```

**Issue 12**: freertos_task.hpp:649:61
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'on_stop' is never moved from inside the function body

```cpp
     646:    * @param start_suspended  start the task in the suspended state
     647:    */
     648:   periodic_task(const char *name, UBaseType_t priority,
>>>  649:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     650:                 task_routine_t &&periodic_routine, bool start_suspended = true)
     651:       : periodic_task{name,
     652:                       priority,
```

**Issue 13**: freertos_task.hpp:650:34
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'periodic_routine' is never moved from inside the function body

```cpp
     647:    */
     648:   periodic_task(const char *name, UBaseType_t priority,
     649:                 task_routine_t &&on_start, task_routine_t &&on_stop,
>>>  650:                 task_routine_t &&periodic_routine, bool start_suspended = true)
     651:       : periodic_task{name,
     652:                       priority,
     653:                       std::move(on_start),
```

**Issue 14**: freertos_task.hpp:669:34
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'on_start' is never moved from inside the function body

```cpp
     666:    * @param start_suspended  start the task in the suspended state
     667:    */
     668:   periodic_task(const std::string &name, UBaseType_t priority,
>>>  669:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     670:                 task_routine_t &&periodic_routine, bool start_suspended = true)
     671:       : periodic_task{name.c_str(),
     672:                       priority,
```

**Issue 15**: freertos_task.hpp:669:61
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'on_stop' is never moved from inside the function body

```cpp
     666:    * @param start_suspended  start the task in the suspended state
     667:    */
     668:   periodic_task(const std::string &name, UBaseType_t priority,
>>>  669:                 task_routine_t &&on_start, task_routine_t &&on_stop,
     670:                 task_routine_t &&periodic_routine, bool start_suspended = true)
     671:       : periodic_task{name.c_str(),
     672:                       priority,
```

**Issue 16**: freertos_task.hpp:670:34
*Reason: Unused RValue Reference Parameter*

**Message**: rvalue reference parameter 'periodic_routine' is never moved from inside the function body

```cpp
     667:    */
     668:   periodic_task(const std::string &name, UBaseType_t priority,
     669:                 task_routine_t &&on_start, task_routine_t &&on_stop,
>>>  670:                 task_routine_t &&periodic_routine, bool start_suspended = true)
     671:       : periodic_task{name.c_str(),
     672:                       priority,
     673:                       std::move(on_start),
```

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

### Compilation Errors

Some files had compilation errors that prevented full analysis:

```
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/tests/mocks/FreeRTOS.h:6:10: error: 'gmock/gmock.h' file not found [clang-diagnostic-error]
```


*Generated: July 24, 2025*
*Tools: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)*
*Scope: Library modules only*
