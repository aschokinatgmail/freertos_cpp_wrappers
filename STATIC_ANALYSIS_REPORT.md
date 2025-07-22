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
- **Files Analyzed**: 1
- **Unique Check Types**: 1

### Suppressed Issues

- **Total Suppressed**: 628028
- **Non-user Code**: 627122
- **NOLINT Directives**: 906

### Issues by Check Category

- **cppcoreguidelines**: 1 issues

### Files with Issues

- **freertos_sw_timer.hpp**: 1 issues

### Analyzed Files

- freertos_sw_timer.hpp

## Detailed Analysis

```
[1/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc.
78381 warnings generated.
[2/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_event_group.hpp.
156775 warnings generated.
[3/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp.
235167 warnings generated.
[4/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp.
313564 warnings generated.
[5/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp.
391956 warnings generated.
[6/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp.
470348 warnings generated.
[7/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp.
548743 warnings generated.
[8/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp.
627123 warnings generated.
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp:131:5: warning: 'm_timer' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  130 |       : m_timer{nullptr}, m_callback{std::move(callback)}, m_started{false} {
      |                ~~~~~~~~~
  131 |     m_timer = m_allocator.create(name, period_ticks, auto_reload, this,
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  132 |                                  callback_wrapper);
      |                                  ~~~~~~~~~~~~~~~~~~
Suppressed 628028 warnings (627122 in non-user code, 906 NOLINT).
Use -header-filter=.* to display errors from all non-system headers. Use -system-headers to display errors from system headers as well.
```

---
*Generated: July 22, 2025*
*Tool: clang-tidy*
*Scope: Library modules only*
