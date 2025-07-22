# Static Analysis Report

## Overview

**Static Analysis Tool**: clang-tidy
**Analysis Scope**: Library modules only - src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-*

## Statistics

### Summary

- **Total Issues Found**: 0
- **Warnings**: 0
- **Errors**: 0
- **Files Analyzed**: 9
- **Unique Check Types**: 2

### Suppressed Issues

- **Total Suppressed**: 706534
- **Non-user Code**: 705517
- **NOLINT Directives**: 1017

### Issues by Check Category

- **cert**: 2 issues
- **cppcoreguidelines**: 1 issues

### Files with Issues

- **freertos_semaphore.hpp**: 2 issues
- **freertos_sw_timer.hpp**: 1 issues

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

## Detailed Analysis

```
[1/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc.
78381 warnings generated.
[2/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos.hpp.
156779 warnings generated.
[3/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_event_group.hpp.
235173 warnings generated.
[4/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp.
313565 warnings generated.
[5/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp.
391962 warnings generated.
[6/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp.
470356 warnings generated.
[7/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp.
548748 warnings generated.
[8/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp.
627143 warnings generated.
[9/9] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp.
705523 warnings generated.
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:421:3: warning: overloaded 'operator++' returns a reference instead of a constant object type [cert-dcl21-cpp]
  421 |   counting_semaphore &
      |   ^~~~~~~~~~~~~~~~~~~~
      |   const counting_semaphore<SemaphoreAllocator> 
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:432:3: warning: overloaded 'operator--' returns a reference instead of a constant object type [cert-dcl21-cpp]
  432 |   counting_semaphore &
      |   ^~~~~~~~~~~~~~~~~~~~
      |   const counting_semaphore<SemaphoreAllocator> 
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp:131:5: warning: 'm_timer' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  130 |       : m_timer{nullptr}, m_callback{std::move(callback)}, m_started{false} {
      |                ~~~~~~~~~
  131 |     m_timer = m_allocator.create(name, period_ticks, auto_reload, this,
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  132 |                                  callback_wrapper);
      |                                  ~~~~~~~~~~~~~~~~~~
Suppressed 706534 warnings (705517 in non-user code, 1017 NOLINT).
Use -header-filter=.* to display errors from all non-system headers. Use -system-headers to display errors from system headers as well.
```

---
*Generated: July 22, 2025*
*Tool: clang-tidy*
*Scope: Library modules only*
