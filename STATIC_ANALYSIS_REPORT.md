# Static Analysis Report

## Overview

**Static Analysis Tool**: clang-tidy
**Analysis Scope**: Library modules only - src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-*

## Statistics

### Summary

- **Total Issues Found**: 78408
- **Warnings**: 78406
- **Errors**: 2
- **Files Analyzed**: 15
- **Unique Check Types**: 13

### Suppressed Issues

- **Total Suppressed**: 628051
- **Non-user Code**: 627147
- **NOLINT Directives**: 904

### Issues by Check Category

- **cppcoreguidelines**: 61 issues
- **clang**: 4 issues
- **hicpp**: 2 issues
- **cert**: 2 issues
- **google**: 1 issues

### Files with Issues

- **freertos_semaphore.hpp**: 28 issues
- **freertos_task.hpp**: 21 issues
- **freertos_message_buffer.hpp**: 5 issues
- **freertos_queue.hpp**: 5 issues
- **freertos_stream_buffer.hpp**: 5 issues
- **freertos_sw_timer.hpp**: 5 issues
- **freertos_event_group.hpp**: 1 issues

### Analyzed Files

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
- freertos_task.cc.
- freertos_task.hpp
- freertos_task.hpp.

## Detailed Analysis

```
[1/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc.
78406 warnings and 2 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc.
[2/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_event_group.hpp.
156802 warnings and 2 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_event_group.hpp.
[3/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp.
235201 warnings and 2 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp.
[4/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp.
313605 warnings and 2 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp.
[5/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp.
392030 warnings and 2 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp.
[6/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp.
470429 warnings and 2 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp.
[7/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp.
548830 warnings and 2 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp.
[8/8] Processing file /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp.
627235 warnings and 4 errors generated.
Error while processing /home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp.
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_event_group.hpp:50:7: warning: class 'static_event_group_allocator' defines a copy constructor, a copy assignment operator, a move constructor and a move assignment operator but does not define a destructor [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
   50 | class static_event_group_allocator {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp:51:43: warning: class 'static_message_buffer_allocator' defines a copy constructor, a copy assignment operator, a move constructor and a move assignment operator but does not define a destructor [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
   51 | template <size_t MessageBufferSize> class static_message_buffer_allocator {
      |                                           ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp:53:3: warning: do not declare C-style arrays, use std::array<> instead [cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays]
   53 |   uint8_t m_storage[MessageBufferSize];
      |   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp:94:26: warning: use default member initializer for 'm_allocator' [cppcoreguidelines-use-default-member-init]
   94 |   MessageBufferAllocator m_allocator;
      |                          ^          
      |                                     {}
   95 |   MessageBufferHandle_t m_message_buffer;
   96 | 
   97 | public:
   98 |   /**
   99 |    * @brief Construct a new message buffer object
  100 |    *
  101 |    */
  102 |   explicit message_buffer() : m_allocator{}, m_message_buffer{nullptr} {
      |                               ~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp:95:25: warning: use default member initializer for 'm_message_buffer' [cppcoreguidelines-use-default-member-init]
   95 |   MessageBufferHandle_t m_message_buffer;
      |                         ^               
      |                                         {nullptr}
   96 | 
   97 | public:
   98 |   /**
   99 |    * @brief Construct a new message buffer object
  100 |    *
  101 |    */
  102 |   explicit message_buffer() : m_allocator{}, m_message_buffer{nullptr} {
      |                                              ~~~~~~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_message_buffer.hpp:103:5: warning: 'm_message_buffer' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  103 |     m_message_buffer = m_allocator.create();
      |     ^
note: this fix will not be applied because it overlaps with another fix
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp:53:49: warning: class 'static_queue_allocator' defines a copy constructor, a copy assignment operator, a move constructor and a move assignment operator but does not define a destructor [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
   53 | template <size_t QueueLength, typename T> class static_queue_allocator {
      |                                                 ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp:55:3: warning: do not declare C-style arrays, use std::array<> instead [cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays]
   55 |   uint8_t m_storage[QueueLength * sizeof(T)];
      |   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp:90:18: warning: use default member initializer for 'm_allocator' [cppcoreguidelines-use-default-member-init]
   90 |   QueueAllocator m_allocator;
      |                  ^          
      |                             {}
   91 |   QueueHandle_t m_queue;
   92 | 
   93 | public:
   94 |   /**
   95 |    * @brief Construct a new queue object
   96 |    *
   97 |    * @param registred_name Name of the queue, which will be used to register it
   98 |    * in the queue registry. If nullptr, the queue will not be registered.
   99 |    */
  100 |   explicit queue(const char *registred_name = nullptr)
  101 |       : m_allocator{}, m_queue{nullptr} {
      |         ~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp:91:17: warning: use default member initializer for 'm_queue' [cppcoreguidelines-use-default-member-init]
   91 |   QueueHandle_t m_queue;
      |                 ^      
      |                        {nullptr}
   92 | 
   93 | public:
   94 |   /**
   95 |    * @brief Construct a new queue object
   96 |    *
   97 |    * @param registred_name Name of the queue, which will be used to register it
   98 |    * in the queue registry. If nullptr, the queue will not be registered.
   99 |    */
  100 |   explicit queue(const char *registred_name = nullptr)
  101 |       : m_allocator{}, m_queue{nullptr} {
      |                        ~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_queue.hpp:102:5: warning: 'm_queue' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  102 |     m_queue = m_allocator.create();
      |     ^
note: this fix will not be applied because it overlaps with another fix
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:43:10: warning: inclusion of deprecated C++ header 'time.h'; consider using 'ctime' instead [hicpp-deprecated-headers]
   43 | #include <time.h>
      |          ^~~~~~~~
      |          <ctime>
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:52:7: warning: class 'static_semaphore_allocator' defines a copy constructor, a copy assignment operator, a move constructor and a move assignment operator but does not define a destructor [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
   52 | class static_semaphore_allocator {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:104:22: warning: use default member initializer for 'm_allocator' [cppcoreguidelines-use-default-member-init]
  104 |   SemaphoreAllocator m_allocator;
      |                      ^          
      |                                 {}
  105 |   SemaphoreHandle_t m_semaphore;
  106 | 
  107 | public:
  108 |   /**
  109 |    * @brief Construct a new binary semaphore object
  110 |    *
  111 |    */
  112 |   binary_semaphore() : m_allocator{}, m_semaphore{nullptr} {
      |                        ~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:105:21: warning: use default member initializer for 'm_semaphore' [cppcoreguidelines-use-default-member-init]
  105 |   SemaphoreHandle_t m_semaphore;
      |                     ^          
      |                                {nullptr}
  106 | 
  107 | public:
  108 |   /**
  109 |    * @brief Construct a new binary semaphore object
  110 |    *
  111 |    */
  112 |   binary_semaphore() : m_allocator{}, m_semaphore{nullptr} {
      |                                       ~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:113:5: warning: 'm_semaphore' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  113 |     m_semaphore = m_allocator.create_binary();
      |     ^
note: this fix will not be applied because it overlaps with another fix
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:224:22: warning: use default member initializer for 'm_allocator' [cppcoreguidelines-use-default-member-init]
  224 |   SemaphoreAllocator m_allocator;
      |                      ^          
      |                                 {}
  225 |   SemaphoreHandle_t m_semaphore;
  226 | 
  227 | public:
  228 |   /**
  229 |    * @brief Construct a new counting semaphore object
  230 |    *
  231 |    * @param max_count maximum count of the counting semaphore.
  232 |    *
  233 |    */
  234 |   explicit counting_semaphore(UBaseType_t max_count = 1)
  235 |       : m_allocator{}, m_semaphore{nullptr} {
      |         ~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:225:21: warning: use default member initializer for 'm_semaphore' [cppcoreguidelines-use-default-member-init]
  225 |   SemaphoreHandle_t m_semaphore;
      |                     ^          
      |                                {nullptr}
  226 | 
  227 | public:
  228 |   /**
  229 |    * @brief Construct a new counting semaphore object
  230 |    *
  231 |    * @param max_count maximum count of the counting semaphore.
  232 |    *
  233 |    */
  234 |   explicit counting_semaphore(UBaseType_t max_count = 1)
  235 |       : m_allocator{}, m_semaphore{nullptr} {
      |                        ~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:236:5: warning: 'm_semaphore' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  236 |     m_semaphore = m_allocator.create_counting(max_count);
      |     ^
note: this fix will not be applied because it overlaps with another fix
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:361:3: warning: overloaded 'operator++' returns a reference instead of a constant object type [cert-dcl21-cpp]
  361 |   counting_semaphore &operator++(int) {
      |   ^~~~~~~~~~~~~~~~~~~~
      |   const counting_semaphore<SemaphoreAllocator> 
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:370:3: warning: overloaded 'operator--' returns a reference instead of a constant object type [cert-dcl21-cpp]
  370 |   counting_semaphore &operator--(int) {
      |   ^~~~~~~~~~~~~~~~~~~~
      |   const counting_semaphore<SemaphoreAllocator> 
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:401:22: warning: use default member initializer for 'm_allocator' [cppcoreguidelines-use-default-member-init]
  401 |   SemaphoreAllocator m_allocator;
      |                      ^          
      |                                 {}
  402 |   SemaphoreHandle_t m_semaphore;
  403 |   uint8_t m_locked : 1;
  404 | 
  405 | public:
  406 |   /**
  407 |    * @brief Construct a new mutex object
  408 |    *
  409 |    */
  410 |   mutex() : m_allocator{}, m_semaphore{nullptr}, m_locked{false} {
      |             ~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:402:21: warning: use default member initializer for 'm_semaphore' [cppcoreguidelines-use-default-member-init]
  402 |   SemaphoreHandle_t m_semaphore;
      |                     ^          
      |                                {nullptr}
  403 |   uint8_t m_locked : 1;
  404 | 
  405 | public:
  406 |   /**
  407 |    * @brief Construct a new mutex object
  408 |    *
  409 |    */
  410 |   mutex() : m_allocator{}, m_semaphore{nullptr}, m_locked{false} {
      |                            ~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:411:5: warning: 'm_semaphore' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  411 |     m_semaphore = m_allocator.create_mutex();
      |     ^
note: this fix will not be applied because it overlaps with another fix
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:554:22: warning: use default member initializer for 'm_allocator' [cppcoreguidelines-use-default-member-init]
  554 |   SemaphoreAllocator m_allocator;
      |                      ^          
      |                                 {}
  555 |   SemaphoreHandle_t m_semaphore;
  556 |   uint8_t m_locked : 1;
  557 |   uint8_t m_lock_count;
  558 | 
  559 | public:
  560 |   /**
  561 |    * @brief Construct a new recursive mutex object
  562 |    *
  563 |    */
  564 |   recursive_mutex() : m_allocator{}, m_semaphore{nullptr}, m_locked{false}, m_lock_count{0} {
      |                       ~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:555:21: warning: use default member initializer for 'm_semaphore' [cppcoreguidelines-use-default-member-init]
  555 |   SemaphoreHandle_t m_semaphore;
      |                     ^          
      |                                {nullptr}
  556 |   uint8_t m_locked : 1;
  557 |   uint8_t m_lock_count;
  558 | 
  559 | public:
  560 |   /**
  561 |    * @brief Construct a new recursive mutex object
  562 |    *
  563 |    */
  564 |   recursive_mutex() : m_allocator{}, m_semaphore{nullptr}, m_locked{false}, m_lock_count{0} {
      |                                      ~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:557:11: warning: use default member initializer for 'm_lock_count' [cppcoreguidelines-use-default-member-init]
  557 |   uint8_t m_lock_count;
      |           ^           
      |                       {0}
  558 | 
  559 | public:
  560 |   /**
  561 |    * @brief Construct a new recursive mutex object
  562 |    *
  563 |    */
  564 |   recursive_mutex() : m_allocator{}, m_semaphore{nullptr}, m_locked{false}, m_lock_count{0} {
      |                                                                             ~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:565:5: warning: 'm_semaphore' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  565 |     m_semaphore = m_allocator.create_recursive_mutex();
      |     ^
note: this fix will not be applied because it overlaps with another fix
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:720:33: warning: class 'lock_guard' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
  720 | template <typename Mutex> class lock_guard {
      |                                 ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:721:10: warning: member 'm_mutex' of type 'Mutex &' is a reference [cppcoreguidelines-avoid-const-or-ref-data-members]
  721 |   Mutex &m_mutex;
      |          ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:750:33: warning: class 'try_lock_guard' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
  750 | template <typename Mutex> class try_lock_guard {
      |                                 ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:751:10: warning: member 'm_mutex' of type 'Mutex &' is a reference [cppcoreguidelines-avoid-const-or-ref-data-members]
  751 |   Mutex &m_mutex;
      |          ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:752:8: warning: use default member initializer for 'm_lock_acquired' [cppcoreguidelines-use-default-member-init]
  752 |   bool m_lock_acquired;
      |        ^              
      |                       {false}
  753 | 
  754 | public:
  755 |   /**
  756 |    * @brief Construct a new try lock guard object
  757 |    *
  758 |    * @param mutex mutex to guard
  759 |    */
  760 |   explicit try_lock_guard(Mutex &mutex) : m_mutex{mutex}, m_lock_acquired{false} { 
      |                                                           ~~~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:761:5: warning: 'm_lock_acquired' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  761 |     m_lock_acquired = m_mutex.try_lock(); 
      |     ^
note: this fix will not be applied because it overlaps with another fix
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:787:33: warning: class 'lock_guard_isr' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
  787 | template <typename Mutex> class lock_guard_isr {
      |                                 ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:788:10: warning: member 'm_mutex' of type 'Mutex &' is a reference [cppcoreguidelines-avoid-const-or-ref-data-members]
  788 |   Mutex &m_mutex;
      |          ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:789:14: warning: use default member initializer for 'm_high_priority_task_woken' [cppcoreguidelines-use-default-member-init]
  789 |   BaseType_t m_high_priority_task_woken;
      |              ^                         
      |                                        {pdFALSE}
  790 | 
  791 | public:
  792 |   /**
  793 |    * @brief Construct a new lock guard object
  794 |    *
  795 |    * @param mutex mutex to guard
  796 |    */
  797 |   explicit lock_guard_isr(Mutex &mutex)
  798 |       : m_mutex{mutex}, m_high_priority_task_woken{pdFALSE} {
      |                         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:830:33: warning: class 'timeout_lock_guard' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
  830 | template <typename Mutex> class timeout_lock_guard {
      |                                 ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_semaphore.hpp:831:10: warning: member 'm_mutex' of type 'Mutex &' is a reference [cppcoreguidelines-avoid-const-or-ref-data-members]
  831 |   Mutex &m_mutex;
      |          ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp:52:42: warning: class 'static_stream_buffer_allocator' defines a copy constructor, a copy assignment operator, a move constructor and a move assignment operator but does not define a destructor [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
   52 | template <size_t StreamBufferSize> class static_stream_buffer_allocator {
      |                                          ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp:54:3: warning: do not declare C-style arrays, use std::array<> instead [cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays]
   54 |   uint8_t m_storage[StreamBufferSize];
      |   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp:95:25: warning: use default member initializer for 'm_allocator' [cppcoreguidelines-use-default-member-init]
   95 |   StreamBufferAllocator m_allocator;
      |                         ^          
      |                                    {}
   96 |   StreamBufferHandle_t m_stream_buffer;
   97 | 
   98 | public:
   99 |   /**
  100 |    * @brief Construct a new stream buffer object
  101 |    *
  102 |    * @param trigger_level_bytes the number of bytes that must be in the stream
  103 |    * buffer before a task that is blocked on a read operation will be unblocked
  104 |    */
  105 |   explicit stream_buffer(size_t trigger_level_bytes = 1)
  106 |       : m_allocator{}, m_stream_buffer{nullptr} {
      |         ~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp:96:24: warning: use default member initializer for 'm_stream_buffer' [cppcoreguidelines-use-default-member-init]
   96 |   StreamBufferHandle_t m_stream_buffer;
      |                        ^              
      |                                       {nullptr}
   97 | 
   98 | public:
   99 |   /**
  100 |    * @brief Construct a new stream buffer object
  101 |    *
  102 |    * @param trigger_level_bytes the number of bytes that must be in the stream
  103 |    * buffer before a task that is blocked on a read operation will be unblocked
  104 |    */
  105 |   explicit stream_buffer(size_t trigger_level_bytes = 1)
  106 |       : m_allocator{}, m_stream_buffer{nullptr} {
      |                        ~~~~~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_stream_buffer.hpp:107:5: warning: 'm_stream_buffer' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  107 |     m_stream_buffer = m_allocator.create(trigger_level_bytes);
      |     ^
note: this fix will not be applied because it overlaps with another fix
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp:44:10: warning: inclusion of deprecated C++ header 'time.h'; consider using 'ctime' instead [hicpp-deprecated-headers]
   44 | #include <time.h>
      |          ^~~~~~~~
      |          <ctime>
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp:59:7: warning: class 'static_sw_timer_allocator' defines a copy constructor, a copy assignment operator, a move constructor and a move assignment operator but does not define a destructor [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
   59 | class static_sw_timer_allocator {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp:128:62: warning: rvalue reference parameter 'callback' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  128 |                  UBaseType_t auto_reload, timer_callback_t &&callback)
      |                                                              ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp:130:5: warning: 'm_timer' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  129 |       : m_timer{nullptr}, m_callback{callback}, m_started{false} {
      |                ~~~~~~~~~
  130 |     m_timer = m_allocator.create(name, period_ticks, auto_reload, this,
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  131 |                                  callback_wrapper);
      |                                  ~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_sw_timer.hpp:172:10: warning: move assignment operators should be marked noexcept [cppcoreguidelines-noexcept-move-operations,hicpp-noexcept-move]
  172 |   timer &operator=(timer &&src) {
      |          ^
      |                                  noexcept 
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:56:35: warning: class 'static_task_allocator' defines a copy constructor, a copy assignment operator, a move constructor and a move assignment operator but does not define a destructor [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
   56 | template <size_t StackSize> class static_task_allocator {
      |                                   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:57:3: warning: do not declare C-style arrays, use std::array<> instead [cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays]
   57 |   StackType_t m_stackBuffer[StackSize / sizeof(StackType_t)];
      |   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:137:5: warning: 'm_hTask' should be initialized in a member initializer of the constructor [cppcoreguidelines-prefer-member-initializer]
  135 |       : m_allocator{}, m_hTask{nullptr}, m_taskRoutine{std::move(task_routine)},
      |                               ~~~~~~~~~
      |                               m_allocator.create(task_exec, name, priority, this)
  136 |         m_start_suspended{start_suspended} {
  137 |     m_hTask = m_allocator.create(task_exec, name, priority, this);
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:411:37: error: non-const lvalue reference to type 'BaseType_t' (aka 'int') cannot bind to a temporary of type 'std::nullptr_t' [clang-diagnostic-error]
  411 |                         BaseType_t &higherPriorityTaskWoken = nullptr) {
      |                                     ^                         ~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:411:37: note: passing argument to parameter 'higherPriorityTaskWoken' here
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:426:36: error: non-const lvalue reference to type 'BaseType_t' (aka 'int') cannot bind to a temporary of type 'std::nullptr_t' [clang-diagnostic-error]
  426 |                        BaseType_t &higherPriorityTaskWoken = nullptr) {
      |                                    ^                         ~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:426:36: note: passing argument to parameter 'higherPriorityTaskWoken' here
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:501:18: warning: variable 'notification_value' is not initialized [cppcoreguidelines-init-variables]
  501 |         uint32_t notification_value;
      |                  ^                 
      |                                     = 0
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:528:34: warning: rvalue reference parameter 'on_start' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  528 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:528:61: warning: rvalue reference parameter 'on_stop' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  528 |                 task_routine_t &&on_start, task_routine_t &&on_stop,
      |                                                             ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:529:34: warning: rvalue reference parameter 'periodic_routine' is never moved from inside the function body [cppcoreguidelines-rvalue-reference-param-not-moved]
  529 |                 task_routine_t &&periodic_routine,
      |                                  ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:937:1: warning: missing username/bug in TODO [google-readability-todo]
  937 | // TODO: add less than ms delays
      | ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      | // TODO(runner): add less than ms delays
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:1014:3: warning: do not declare C-style arrays, use std::array<> instead [cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays]
 1014 |   TaskStatus_t m_status_array[status_array_capacity];
      |   ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:1015:15: warning: use default member initializer for 'm_task_count' [cppcoreguidelines-use-default-member-init]
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
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:1016:12: warning: use default member initializer for 'm_total_run_time' [cppcoreguidelines-use-default-member-init]
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
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:1023:3: warning: constructor does not initialize these fields: m_status_array [cppcoreguidelines-pro-type-member-init,hicpp-member-init]
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
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:1122:7: warning: class 'critical_section' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
 1122 | class critical_section {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:1141:7: warning: class 'critical_section_isr' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
 1141 | class critical_section_isr {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:1142:15: warning: use default member initializer for 'm_saved_interrupt_status' [cppcoreguidelines-use-default-member-init]
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
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:1165:7: warning: class 'interrupt_barrier' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
 1165 | class interrupt_barrier {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:1184:7: warning: class 'scheduler_barrier' defines a non-default destructor but does not define a copy constructor, a copy assignment operator, a move constructor or a move assignment operator [cppcoreguidelines-special-member-functions,hicpp-special-member-functions]
 1184 | class scheduler_barrier {
      |       ^
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:411:37: error: non-const lvalue reference to type 'BaseType_t' (aka 'int') cannot bind to a temporary of type 'std::nullptr_t' [clang-diagnostic-error]
  411 |                         BaseType_t &higherPriorityTaskWoken = nullptr) {
      |                                     ^                         ~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:411:37: note: passing argument to parameter 'higherPriorityTaskWoken' here
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:426:36: error: non-const lvalue reference to type 'BaseType_t' (aka 'int') cannot bind to a temporary of type 'std::nullptr_t' [clang-diagnostic-error]
  426 |                        BaseType_t &higherPriorityTaskWoken = nullptr) {
      |                                    ^                         ~~~~~~~
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/include/freertos_task.hpp:426:36: note: passing argument to parameter 'higherPriorityTaskWoken' here
Suppressed 628051 warnings (627147 in non-user code, 904 NOLINT).
Use -header-filter=.* to display errors from all non-system headers. Use -system-headers to display errors from system headers as well.
Found compiler error(s).
```

---
*Generated: July 20, 2025*
*Tool: clang-tidy*
*Scope: Library modules only*
