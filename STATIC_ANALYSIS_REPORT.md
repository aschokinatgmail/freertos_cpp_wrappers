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


*Generated: July 24, 2025*
*Tools: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)*
*Scope: Library modules only*
