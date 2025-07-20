

**Static Analysis Tool**: clang-tidy


**Static Analysis Tool**: clang-tidy


**Static Analysis Tool**: clang-tidy


**Static Analysis Tool**: clang-tidy


**Static Analysis Tool**: clang-tidy


**Static Analysis Tool**: clang-tidy
**Analysis Scope**: Library modules only src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-*


**Analysis completed successfully**




**Static Analysis Tool**: clang-tidy
**Analysis Scope**: Library modules only src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-*


**Analysis completed successfully**


```
78406 warnings and 2 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc.
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:56:35: warning: class 'static_task_allocator' defines a copy constructor, a copy assignment operator, a move constructor and a move assignment operator but does not define a destructor [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
   56 | template <size_t StackSize> class static_task_allocator {
      |                                   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:57:3: warning: do not declare C-style arrays, use std::array<> instead [cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays]
   57 |   StackType_t m_stackBuffer[StackSize / sizeof(StackType_t)];
      |   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:137:5: warning: 'm_hTask' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  135 |       : m_allocator{}, m_hTask{nullptr}, m_taskRoutine{std::move(task_routine)},
      |                               ~~~~~~~~~
      |                               m_allocator.create(task_exec, name, priority, this)
  136 |         m_start_suspended{start_suspended} {
  137 |     m_hTask = m_allocator.create(task_exec, name, priority, this);
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:411:37: error: non-const lvalue reference to type 'BaseType_t' (aka 'int') cannot bind to a temporary of type 'std::nullptr_t' [clang-diagnostic-error]
  411 |                         BaseType_t &higherPriorityTaskWoken = nullptr) {
      |                                     ^                         ~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:411:37: note: passing argument to parameter 'higherPriorityTaskWoken' here
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:426:36: error: non-const lvalue reference to type 'BaseType_t' (aka 'int') cannot bind to a temporary of type 'std::nullptr_t' [clang-diagnostic-error]
  426 |                        BaseType_t &higherPriorityTaskWoken = nullptr) {
      |                                    ^                         ~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/../include/freertos_task.hpp:426:36: note: passing argument to parameter 'higherPriorityTaskWoken' here
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
Suppressed 78494 warnings (78381 in non-user code, 113 NOLINT).
Use -header-filter=.* to display errors from all non-system headers. Use -system-headers to display errors from system headers as well.
Found compiler error(s).
```

---
*Generated:\ July\ 20,\ 2025*
*Tool: clang-tidy*
*Scope: Library modules only*
