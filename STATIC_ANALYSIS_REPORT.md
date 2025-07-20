# Static Analysis Report

## Overview

**Static Analysis Tool**: clang-tidy
**Analysis Scope**: Library modules only - src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-*

## Statistics

### Summary

- **Total Issues Found**: 34358
- **Warnings**: 34357
- **Errors**: 1
- **Files Analyzed**: 3
- **Unique Check Types**: 9

### Issues by Check Category

- **cppcoreguidelines**: 27 issues
- **google**: 1 issues
- **clang**: 1 issues

### Files with Issues

- **freertos_task.hpp**: 28 issues
- **FreeRTOS.h**: 1 issues

### Analyzed Files

- FreeRTOS.h
- freertos_task.cc.
- freertos_task.hpp

## Detailed Analysis

```
34357 warnings and 1 error generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc.
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:56:35: warning: class 'static_task_allocator' defines a copy constructor, a copy assignment operator, a move constructor and a move assignment operator but does not define a destructor [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
   56 | template <size_t StackSize> class static_task_allocator {
      |                                   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:57:3: warning: do not declare C-style arrays, use std::array<> instead [cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays]
   57 |   StackType_t m_stackBuffer[StackSize / sizeof(StackType_t)];
      |   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:133:65: warning: rvalue reference parameter 'task_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  133 |   task(const char *name, UBaseType_t priority, task_routine_t &&task_routine,
      |                                                                 ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:137:5: warning: 'm_hTask' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  135 |       : m_allocator{}, m_hTask{nullptr}, m_taskRoutine{std::move(task_routine)},
      |                               ~~~~~~~~~
      |                               m_allocator.create(task_exec, name, priority, this)
  136 |         m_start_suspended{start_suspended} {
  137 |     m_hTask = m_allocator.create(task_exec, name, priority, this);
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:148:25: warning: rvalue reference parameter 'task_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  148 |        task_routine_t &&task_routine, bool start_suspended = true)
      |                         ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:501:18: warning: variable 'notification_value' is not initialized [cppcoreguidelines-init-variables]
  501 |         uint32_t notification_value;
      |                  ^                 
      |                                     = 0
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:528:34: warning: rvalue reference parameter 'on_start' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  528 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:528:61: warning: rvalue reference parameter 'on_stop' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  528 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                                             ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:529:34: warning: rvalue reference parameter 'periodic_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  529 |                 task_routine_t &&periodic_routine,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:551:34: warning: rvalue reference parameter 'on_start' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  551 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:551:61: warning: rvalue reference parameter 'on_stop' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  551 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                                             ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:552:34: warning: rvalue reference parameter 'periodic_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  552 |                 task_routine_t &&periodic_routine,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:573:34: warning: rvalue reference parameter 'on_start' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  573 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:573:61: warning: rvalue reference parameter 'on_stop' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  573 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                                             ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:574:34: warning: rvalue reference parameter 'periodic_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  574 |                 task_routine_t &&periodic_routine, bool start_suspended = true)
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:593:34: warning: rvalue reference parameter 'on_start' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  593 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:593:61: warning: rvalue reference parameter 'on_stop' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  593 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                                             ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:594:34: warning: rvalue reference parameter 'periodic_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  594 |                 task_routine_t &&periodic_routine, bool start_suspended = true)
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:937:1: warning: missing username/bug in TODO [google-readability-todo]
  937 | // TODO: add less than ms delays
      | ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      | // TODO(runner): add less than ms delays
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:1014:3: warning: do not declare C-style arrays, use std::array<> instead [cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays]
 1014 |   TaskStatus_t m_status_array[status_array_capacity];
      |   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:1015:15: warning: use default member initializer for 'm_task_count' [cppcoreguidelines-use-default-member-init]
 1015 |   UBaseType_t m_task_count;
      |               ^           
      |                           {0}
 1016 |   uint32_t m_total_run_time;
 1017 | 
 1018 | public:
 1019 |   /**
 1020 |    * @brief Construct a new task system status object
 1021 |    *
 1022 |    */
 1023 |   task_system_status(void) : m_task_count{0}, m_total_run_time{0} {
      |                              ~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:1016:12: warning: use default member initializer for 'm_total_run_time' [cppcoreguidelines-use-default-member-init]
 1016 |   uint32_t m_total_run_time;
      |            ^               
      |                            {0}
 1017 | 
 1018 | public:
 1019 |   /**
 1020 |    * @brief Construct a new task system status object
 1021 |    *
 1022 |    */
 1023 |   task_system_status(void) : m_task_count{0}, m_total_run_time{0} {
      |                                               ~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:1023:3: warning: constructor does not initialize these fields: m_status_array [cppcoreguidelines-pro-type-member-init,hicpp-member-init]
 1014 |   TaskStatus_t m_status_array[status_array_capacity];
      |                                                     
      |                                                     {}
 1015 |   UBaseType_t m_task_count;
 1016 |   uint32_t m_total_run_time;
 1017 | 
 1018 | public:
 1019 |   /**
 1020 |    * @brief Construct a new task system status object
 1021 |    *
 1022 |    */
 1023 |   task_system_status(void) : m_task_count{0}, m_total_run_time{0} {
      |   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:1122:7: warning: class 'critical_section' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
 1122 | class critical_section {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:1141:7: warning: class 'critical_section_isr' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
 1141 | class critical_section_isr {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:1142:15: warning: use default member initializer for 'm_saved_interrupt_status' [cppcoreguidelines-use-default-member-init]
 1142 |   UBaseType_t m_saved_interrupt_status;
      |               ^                       
      |                                       {taskENTER_CRITICAL_FROM_ISR()}
 1143 | 
 1144 | public:
 1145 |   /**
 1146 |    * @brief Construct a new critical section isr object
 1147 |    *
 1148 |    */
 1149 |   critical_section_isr(void)
 1150 |       : m_saved_interrupt_status{taskENTER_CRITICAL_FROM_ISR()} {}
      |         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:1165:7: warning: class 'interrupt_barrier' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
 1165 | class interrupt_barrier {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:1184:7: warning: class 'scheduler_barrier' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
 1184 | class scheduler_barrier {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/tests/mocks/FreeRTOS.h:6:10: error: 'gmock/gmock.h' file not found [clang-diagnostic-error]
    6 | #include <gmock/gmock.h>
      |          ^~~~~~~~~~~~~~~
Suppressed 34321 warnings (34321 in non-user code).
Use -header-filter=.* to display errors from all non-system headers. Use -system-headers to display errors from system headers as well.
Found compiler error(s).
```

---
*Generated: July 20, 2025*
*Tool: clang-tidy*
*Scope: Library modules only*
