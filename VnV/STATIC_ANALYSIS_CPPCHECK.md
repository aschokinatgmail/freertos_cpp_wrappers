Checking src/freertos_atomic_wait.cc ...
src/freertos_atomic_wait.cc:74:29: warning: Identical inner 'if' condition is always true. [identicalInnerCondition]
        if (entry.semaphore == nullptr) {
                            ^
src/freertos_atomic_wait.cc:72:25: note: outer condition: entry.semaphore==nullptr
    if (entry.semaphore == nullptr) {
                        ^
src/freertos_atomic_wait.cc:74:29: note: identical inner condition: entry.semaphore==nullptr
        if (entry.semaphore == nullptr) {
                            ^
Checking src/freertos_atomic_wait.cc: FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL=1;FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY=FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY...
Checking src/freertos_atomic_wait.cc: FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL=2;FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY=FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY...
src/freertos_atomic_wait.cc:0:0: information: Limiting analysis of branches. Use --check-level=exhaustive to analyze all branches. [normalCheckLevelMaxBranches]

^
src/freertos_atomic_wait.cc:188:30: warning: Identical inner 'if' condition is always true. [identicalInnerCondition]
            if (bucket.mutex == nullptr) {
                             ^
src/freertos_atomic_wait.cc:186:26: note: outer condition: bucket.mutex==nullptr
        if (bucket.mutex == nullptr) {
                         ^
src/freertos_atomic_wait.cc:188:30: note: identical inner condition: bucket.mutex==nullptr
            if (bucket.mutex == nullptr) {
                             ^
include/freertos_atomic_wait.hpp:136:11: style: Variable 'bucket' can be declared as reference to const [constVariableReference]
    auto &bucket = freertos_wait_buckets[idx];
          ^
include/freertos_atomic_wait.hpp:151:11: style: Variable 'bucket' can be declared as reference to const [constVariableReference]
    auto &bucket = freertos_wait_buckets[idx];
          ^
Checking src/freertos_atomic_wait.cc: FREERTOS_CPP_WRAPPERS_ATOMIC_WAIT_IMPL=3;FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY=FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY...
Checking src/freertos_atomic_wait.cc: FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY=FREERTOS_CPP_WRAPPERS_ENABLE_ATOMIC_WAIT_NOTIFY...
Checking src/freertos_atomic_wait.cc: FREERTOS_CPP_WRAPPERS_HAS_MUTEX;FREERTOS_CPP_WRAPPERS_HAS_DYNAMIC_ALLOCATION=0;FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION;FREERTOS_CPP_WRAPPERS_CPP17...
Checking src/freertos_atomic_wait.cc: FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS=FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS...
Checking src/freertos_atomic_wait.cc: configNUMBER_OF_CORES=2;configNUMBER_OF_CORES=configNUMBER_OF_CORES;configUSE_CORE_AFFINITY=1;configUSE_CORE_AFFINITY=configUSE_CORE_AFFINITY...
Checking src/freertos_atomic_wait.cc: configSUPPORT_DYNAMIC_ALLOCATION=1;configSUPPORT_DYNAMIC_ALLOCATION=configSUPPORT_DYNAMIC_ALLOCATION...
Checking src/freertos_atomic_wait.cc: configSUPPORT_STATIC_ALLOCATION=1;configSUPPORT_STATIC_ALLOCATION=configSUPPORT_STATIC_ALLOCATION...
Checking src/freertos_atomic_wait.cc: configTASK_NOTIFICATION_ARRAY_ENTRIES=2;configTASK_NOTIFICATION_ARRAY_ENTRIES=configTASK_NOTIFICATION_ARRAY_ENTRIES...
Checking src/freertos_atomic_wait.cc: configUSE_COUNTING_SEMAPHORES=1;configUSE_COUNTING_SEMAPHORES=configUSE_COUNTING_SEMAPHORES...
Checking src/freertos_atomic_wait.cc: configUSE_EVENT_GROUPS=1;configUSE_EVENT_GROUPS=configUSE_EVENT_GROUPS...
Checking src/freertos_atomic_wait.cc: configUSE_MUTEXES=1;configUSE_MUTEXES=configUSE_MUTEXES...
Checking src/freertos_atomic_wait.cc: configUSE_QUEUE_SETS=1;configUSE_QUEUE_SETS=configUSE_QUEUE_SETS...
Checking src/freertos_atomic_wait.cc: configUSE_RECURSIVE_MUTEXES=1;configUSE_RECURSIVE_MUTEXES=configUSE_RECURSIVE_MUTEXES...
Checking src/freertos_atomic_wait.cc: configUSE_STREAM_BUFFERS=1;configUSE_STREAM_BUFFERS=configUSE_STREAM_BUFFERS...
Checking src/freertos_atomic_wait.cc: configUSE_TASK_NOTIFICATIONS=1;configUSE_TASK_NOTIFICATIONS=configUSE_TASK_NOTIFICATIONS...
Checking src/freertos_atomic_wait.cc: configUSE_TICKLESS_IDLE=configUSE_TICKLESS_IDLE...
Checking src/freertos_atomic_wait.cc: configUSE_TIMERS=1;configUSE_TIMERS=configUSE_TIMERS...
1/2 files checked 73% done
Checking src/freertos_task.cc ...
src/freertos_task.cc:0:0: information: Limiting analysis of branches. Use --check-level=exhaustive to analyze all branches. [normalCheckLevelMaxBranches]

^
include/freertos_expected.hpp:130:13: warning: Member variable 'expected::m_error_storage' is not initialized in the constructor. [uninitMemberVar]
  constexpr expected(const T &val) : m_has_value(true) {
            ^
include/freertos_expected.hpp:134:13: warning: Member variable 'expected::m_error_storage' is not initialized in the constructor. [uninitMemberVar]
  constexpr expected(T &&val) : m_has_value(true) {
            ^
include/freertos_expected.hpp:138:13: warning: Member variable 'expected::m_storage' is not initialized in the constructor. [uninitMemberVar]
  constexpr expected(const unexpected<E> &err) : m_has_value(false) {
            ^
include/freertos_expected.hpp:142:13: warning: Member variable 'expected::m_storage' is not initialized in the constructor. [uninitMemberVar]
  constexpr expected(unexpected<E> &&err) : m_has_value(false) {
            ^
include/freertos_expected.hpp:146:13: warning: Member variable 'expected::m_storage' is not initialized in the constructor. [uninitMemberVar]
  constexpr expected(unexpect_t, const E &err) : m_has_value(false) {
            ^
include/freertos_expected.hpp:150:13: warning: Member variable 'expected::m_storage' is not initialized in the constructor. [uninitMemberVar]
  constexpr expected(unexpect_t, E &&err) : m_has_value(false) {
            ^
include/freertos_expected.hpp:412:13: warning: Member variable 'expected::m_error_storage' is not initialized in the constructor. [uninitMemberVar]
  constexpr expected() noexcept : m_has_value(true) {}
            ^
include/freertos_task.hpp:71:3: warning: Member variable 'static_task_allocator::m_stackBuffer' is not initialized in the constructor. [uninitMemberVar]
  static_task_allocator() = default;
  ^
include/freertos_task.hpp:102:8: style: The member function 'freertos::dynamic_task_allocator::swap' can be static. [functionStatic]
  void swap(dynamic_task_allocator &other) noexcept { (void)other; }
       ^
include/freertos_task.hpp:104:16: style: The member function 'freertos::dynamic_task_allocator::create' can be static. [functionStatic]
  TaskHandle_t create(TaskFunction_t taskFunction, const char *name,
               ^
include/freertos_task.hpp:530:26: style: The member function 'freertos::task::notify_take' can be static. [functionStatic]
  [[nodiscard]] uint32_t notify_take(BaseType_t clear_count_on_exit = pdTRUE,
                         ^
include/freertos_task.hpp:595:28: style: The member function 'freertos::task::notify_wait' can be static. [functionStatic]
  [[nodiscard]] BaseType_t notify_wait(uint32_t bits_to_clear_on_entry,
                           ^
include/freertos_task.hpp:642:26: style: The member function 'freertos::task::notify_take' can be static. [functionStatic]
  [[nodiscard]] uint32_t notify_take(UBaseType_t index, BaseType_t clear_count_on_exit,
                         ^
include/freertos_task.hpp:680:28: style: The member function 'freertos::task::notify_wait' can be static. [functionStatic]
  [[nodiscard]] BaseType_t notify_wait(UBaseType_t index, uint32_t bits_to_clear_on_entry,
                           ^
include/freertos_expected.hpp:86:13: style: Class 'unexpected' has a constructor with 1 argument that is not explicit. [noExplicitConstructor]
  constexpr unexpected(const E &err) : m_error(err) {}
            ^
include/freertos_expected.hpp:87:13: style: Class 'unexpected' has a constructor with 1 argument that is not explicit. [noExplicitConstructor]
  constexpr unexpected(E &&err) : m_error(std::move(err)) {}
            ^
include/freertos_expected.hpp:130:13: style: Class 'expected' has a constructor with 1 argument that is not explicit. [noExplicitConstructor]
  constexpr expected(const T &val) : m_has_value(true) {
            ^
include/freertos_expected.hpp:134:13: style: Class 'expected' has a constructor with 1 argument that is not explicit. [noExplicitConstructor]
  constexpr expected(T &&val) : m_has_value(true) {
            ^
include/freertos_expected.hpp:138:13: style: Class 'expected' has a constructor with 1 argument that is not explicit. [noExplicitConstructor]
  constexpr expected(const unexpected<E> &err) : m_has_value(false) {
            ^
include/freertos_expected.hpp:142:13: style: Class 'expected' has a constructor with 1 argument that is not explicit. [noExplicitConstructor]
  constexpr expected(unexpected<E> &&err) : m_has_value(false) {
            ^
include/freertos_expected.hpp:414:13: style: Class 'expected' has a constructor with 1 argument that is not explicit. [noExplicitConstructor]
  constexpr expected(const unexpected<E> &err) : m_has_value(false) {
            ^
include/freertos_expected.hpp:418:13: style: Class 'expected' has a constructor with 1 argument that is not explicit. [noExplicitConstructor]
  constexpr expected(unexpected<E> &&err) : m_has_value(false) {
            ^
Checking src/freertos_task.cc: FREERTOS_CPP_WRAPPERS_SIMULATION=FREERTOS_CPP_WRAPPERS_SIMULATION...
Checking src/freertos_task.cc: FREERTOS_CPP_WRAPPERS_SIMULATION=FREERTOS_CPP_WRAPPERS_SIMULATION;configUSE_TIMERS...
Checking src/freertos_task.cc: INCLUDE_eTaskGetState...
Checking src/freertos_task.cc: INCLUDE_uxTaskGetStackHighWaterMark...
Checking src/freertos_task.cc: INCLUDE_uxTaskGetStackHighWaterMark2...
Checking src/freertos_task.cc: INCLUDE_vTaskDelete...
Checking src/freertos_task.cc: INCLUDE_vTaskPrioritySet...
Checking src/freertos_task.cc: INCLUDE_vTaskSuspend...
Checking src/freertos_task.cc: INCLUDE_xTaskAbortDelay...
Checking src/freertos_task.cc: INCLUDE_xTaskGetCurrentTaskHandle...
Checking src/freertos_task.cc: INCLUDE_xTaskGetIdleTaskHandle...
Checking src/freertos_task.cc: __cpp_lib_expected=202211L;__cpp_lib_expected=__cpp_lib_expected...
Checking src/freertos_task.cc: configNUMBER_OF_CORES=2;configUSE_CORE_AFFINITY=1...
Checking src/freertos_task.cc: configSUPPORT_DYNAMIC_ALLOCATION...
Checking src/freertos_task.cc: configSUPPORT_STATIC_ALLOCATION...
Checking src/freertos_task.cc: configTASK_NOTIFICATION_ARRAY_ENTRIES=2;configUSE_TASK_NOTIFICATIONS...
Checking src/freertos_task.cc: configUSE_APPLICATION_TASK_TAG...
Checking src/freertos_task.cc: configUSE_TASK_NOTIFICATIONS...
Checking src/freertos_task.cc: configUSE_TICKLESS_IDLE...
Checking src/freertos_task.cc: configUSE_TIMERS...
Checking src/freertos_task.cc: configUSE_TRACE_FACILITY...
2/2 files checked 100% done
nofile:0:0: information: Active checkers: 183/186 (use --checkers-report=<filename> to see details) [checkersReport]

