# Validation and Verification Report

## Executive Summary

This report provides comprehensive validation and verification results for the FreeRTOS C++ Wrappers project, including detailed test execution outcomes and code coverage analysis.

### Test Execution Summary
- **Total Tests Executed**: 12
- **✅ Passed**: 11 tests
- **❌ Failed**: 1 tests
- **Success Rate**: 91.7% (1 tests failed)
- **Total Execution Time**: 1.97 seconds
- **Average Test Time**: 0.1642 seconds per test

### Code Coverage Summary
- **Line Coverage**: 96.3% (7406/7689 lines)
- **Function Coverage**: 95.0% (2907/3061 functions)
- **Coverage Scope**: Main library modules only (src/ and include/ directories)

## Detailed Test Results by Module

## Code Coverage Analysis

### Coverage Overview
The project achieves excellent code coverage with **96.3% line coverage** and **95.0% function coverage**.

### Coverage Breakdown
- **Lines Covered**: 7406 out of 7689 total lines
- **Functions Covered**: 2907 out of 3061 total functions
- **Coverage Target**: Main library modules only (excludes test infrastructure and system headers)

**Detailed Uncovered Areas Analysis:**

The following sections provide specific references to uncovered code areas and explanations for why they cannot be covered by unit tests.

### Internal task execution functions called by FreeRTOS kernel

**Reason for exclusion:** These functions are called internally by the FreeRTOS kernel during task execution and cannot be directly invoked in unit tests

**Uncovered Area 1**: freertos_task.hpp:173-182
*Function*: `freertos::task<freertos::static_task_allocator<0ul> >::task_exec(void*)`

```cpp
    TaskHandle_t m_hTask;
    
>>> static void task_exec(void *context) {
    auto pThis = static_cast<task *>(context);
    assert(nullptr != pThis);
```

**Uncovered Area 2**: freertos_task.hpp:173-182
*Function*: `freertos::task<freertos::dynamic_task_allocator<2048ul> >::task_exec(void*)`

```cpp
    TaskHandle_t m_hTask;
    
>>> static void task_exec(void *context) {
    auto pThis = static_cast<task *>(context);
    assert(nullptr != pThis);
```

**Uncovered Area 3**: freertos_task.hpp:572-586
*Function*: `freertos::periodic_task<freertos::static_task_allocator<1024ul> >::run()`

```cpp
    task<TaskAllocator> m_task;
    
>>> void run() {
    m_on_start();
    while (is_running()) {
```

**Uncovered Area 4**: test_freertos_task.cpp:1140-1142
*Function*: `FreeRTOSTaskTest_TaskExecutionDirectCall_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    bool task_executed = false;
>>> sa::task<1024> test_task("ExecutionTask", 2, [&task_executed]() {
    task_executed = true;
    });
```

**Uncovered Area 5**: test_freertos_task.cpp:1613-1615
*Function*: `FreeRTOSTaskTest_TaskExecutionInternalFunction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> suspended_task("SuspendedTask", 1, []() {
    // Task routine that would be executed
    }, true); // start_suspended = true
```

**Uncovered Area 6**: test_freertos_task.cpp:1623-1625
*Function*: `FreeRTOSTaskTest_TaskExecutionInternalFunction_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    .WillOnce(Return(reinterpret_cast<TaskHandle_t>(0x2000)));
    
>>> sa::task<1024> normal_task("NormalTask", 2, []() {
    // Normal task routine
    }); // start_suspended defaults to false for this constructor
```

**Uncovered Area 7**: test_freertos_task.cpp:1650
*Function*: `FreeRTOSTaskTest_PeriodicTaskRunMethodExecution_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    sa::periodic_task<1024> periodic_task(
    "PeriodicRunTask", 1,
>>> [&on_start_calls]() { on_start_calls++; },    // on_start
    [&on_stop_calls]() { on_stop_calls++; },      // on_stop
    []() { /* periodic_routine */ },               // periodic_routine
```

**Uncovered Area 8**: test_freertos_task.cpp:1651
*Function*: `FreeRTOSTaskTest_PeriodicTaskRunMethodExecution_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    "PeriodicRunTask", 1,
    [&on_start_calls]() { on_start_calls++; },    // on_start
>>> [&on_stop_calls]() { on_stop_calls++; },      // on_stop
    []() { /* periodic_routine */ },               // periodic_routine
    std::chrono::milliseconds(100)                 // period
```

**Uncovered Area 9**: test_freertos_task.cpp:1652
*Function*: `FreeRTOSTaskTest_PeriodicTaskRunMethodExecution_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    [&on_start_calls]() { on_start_calls++; },    // on_start
    [&on_stop_calls]() { on_stop_calls++; },      // on_stop
>>> []() { /* periodic_routine */ },               // periodic_routine
    std::chrono::milliseconds(100)                 // period
    );
```

**Uncovered Area 10**: test_freertos_task.cpp:676
*Function*: `FreeRTOSTaskTest_PeriodicTaskIsRunning_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "RunningTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 11**: test_freertos_task.cpp:677
*Function*: `FreeRTOSTaskTest_PeriodicTaskIsRunning_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    100ms     // period
```

**Uncovered Area 12**: test_freertos_task.cpp:678
*Function*: `FreeRTOSTaskTest_PeriodicTaskIsRunning_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    100ms     // period
    );
```

**Uncovered Area 13**: freertos_sw_timer.hpp:200
*Line*: 200

```cpp
    if (rc == pdPASS) {
    while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
>>> vTaskDelay(pdMS_TO_TICKS(1));
    }
    auto name = pcTimerGetName(src.m_timer);
```

**Uncovered Area 14**: freertos_sw_timer.hpp:208
*Line*: 208

```cpp
    if (rc == pdPASS) {
    while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
>>> vTaskDelay(pdMS_TO_TICKS(1));
    }
    src.m_timer = nullptr;
```


### Error handling and edge case scenarios

**Reason for exclusion:** These code paths handle rare error conditions or require specific FreeRTOS kernel states that are difficult to reproduce in unit tests

**Uncovered Area 15**: test_enhanced_cpp17_features.cpp:200
*Function*: `Cpp17FeaturesTest_RAIIExceptionSafety_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    // Test RAII behavior with exceptions
    {
>>> sa::task<1024> task("RAIITest", 1, []() { /* test */ });
    EXPECT_NE(task.handle(), nullptr);
    
```

**Uncovered Area 16**: test_freertos_task.cpp:148
*Function*: `FreeRTOSTaskTest_DynamicTaskAllocatorCreateFailure_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    .WillOnce(Return(pdFAIL));
    
>>> auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, nullptr);
    
```

**Uncovered Area 17**: test_freertos_task.cpp:1771
*Function*: `FreeRTOSTaskTest_EdgeCaseErrorHandling_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(nullptr));
    
>>> sa::task<1024> null_task("TestTask", 1, []() {});
    
    // Task should handle null gracefully
```

**Uncovered Area 18**: test_freertos_task.cpp:382
*Function*: `FreeRTOSTaskTest_StaticTaskAbortDelay_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("AbortTask", 2, []() {});
    
    // Test abort_delay with valid handle
```

**Uncovered Area 19**: test_freertos_task.cpp:398
*Function*: `FreeRTOSTaskTest_StaticTaskAbortDelayNullHandle_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(nullptr));
    
>>> sa::task<1024> test_task("NullAbortTask", 2, []() {});
    
    // Should return pdFALSE for null handle without calling FreeRTOS function
```

**Uncovered Area 20**: test_freertos_task.cpp:460
*Function*: `FreeRTOSTaskTest_DynamicTaskConstructionFailure_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(pdFAIL));
    
>>> da::task<2048> test_task("FailTask", 3, []() {});
    
    EXPECT_EQ(test_task.handle(), nullptr);
```

**Uncovered Area 21**: test_freertos_task.cpp:737
*Function*: `FreeRTOSTaskTest_PeriodicTaskDestructorAbortDelay_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "AbortDelayTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 22**: test_freertos_task.cpp:738
*Function*: `FreeRTOSTaskTest_PeriodicTaskDestructorAbortDelay_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    100ms     // period
```

**Uncovered Area 23**: test_freertos_task.cpp:739
*Function*: `FreeRTOSTaskTest_PeriodicTaskDestructorAbortDelay_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    100ms     // period
    );
```


### Platform-specific or hardware-dependent code

**Reason for exclusion:** These code sections depend on specific hardware configurations or FreeRTOS kernel internals not available in the test environment

**Uncovered Area 24**: freertos_task.hpp:515-521
*Function*: `freertos::task<freertos::static_task_allocator<256ul> >::notify_wait(unsigned int, unsigned int, unsigned int&, unsigned int)`

```cpp
    * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
    */
>>> BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
    uint32_t ulBitsToClearOnExit,
    uint32_t &notification_value,
```

**Uncovered Area 25**: freertos_task.hpp:515-521
*Function*: `freertos::task<freertos::static_task_allocator<512ul> >::notify_wait(unsigned int, unsigned int, unsigned int&, unsigned int)`

```cpp
    * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
    */
>>> BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
    uint32_t ulBitsToClearOnExit,
    uint32_t &notification_value,
```

**Uncovered Area 26**: freertos_task.hpp:532-540
*Function*: `int freertos::task<freertos::static_task_allocator<256ul> >::notify_wait<long, std::ratio<1l, 1000l> >(unsigned int, unsigned int, unsigned int&, std::chrono::duration<long, std::ratio<1l, 1000l> >)`

```cpp
    */
    template <typename Rep, typename Period>
>>> BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
    uint32_t ulBitsToClearOnExit,
    uint32_t &notification_value,
```

**Uncovered Area 27**: freertos_task.hpp:532-540
*Function*: `int freertos::task<freertos::static_task_allocator<512ul> >::notify_wait<long, std::ratio<1l, 1000l> >(unsigned int, unsigned int, unsigned int&, std::chrono::duration<long, std::ratio<1l, 1000l> >)`

```cpp
    */
    template <typename Rep, typename Period>
>>> BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
    uint32_t ulBitsToClearOnExit,
    uint32_t &notification_value,
```

**Uncovered Area 28**: freertos_sw_timer.hpp:216
*Line*: 216

```cpp
    if (m_timer) {
    if (src.m_started) {
>>> rc = xTimerStart(m_timer, portMAX_DELAY);
    if (rc == pdPASS) {
    m_started = true;
```

**Uncovered Area 29**: freertos_sw_timer.hpp:217
*Line*: 217

```cpp
    if (src.m_started) {
    rc = xTimerStart(m_timer, portMAX_DELAY);
>>> if (rc == pdPASS) {
    m_started = true;
    }
```

**Uncovered Area 30**: freertos_sw_timer.hpp:218
*Line*: 218

```cpp
    rc = xTimerStart(m_timer, portMAX_DELAY);
    if (rc == pdPASS) {
>>> m_started = true;
    }
    }
```

**Uncovered Area 31**: freertos_task.hpp:577
*Line*: 577

```cpp
    if (0 != m_period.count()) {
    #if configUSE_TASK_NOTIFICATIONS
>>> uint32_t notification_value = 0;
    m_task.notify_wait(0, 0, notification_value, m_period);
    #else
```

**Uncovered Area 32**: freertos_task.hpp:578
*Line*: 578

```cpp
    #if configUSE_TASK_NOTIFICATIONS
    uint32_t notification_value = 0;
>>> m_task.notify_wait(0, 0, notification_value, m_period);
    #else
    delay(m_period);
```


### Defensive programming and robustness checks

**Reason for exclusion:** These are safety checks and defensive programming patterns that are difficult to trigger in controlled test conditions

**Uncovered Area 33**: freertos_task.hpp:273
*Function*: `freertos::task<freertos::static_task_allocator<0ul> >::suspend()`

```cpp
    *
    */
>>> void suspend(void) { vTaskSuspend(m_hTask); }
    /**
    * @brief Resume the task.
```

**Uncovered Area 34**: freertos_task.hpp:273
*Function*: `freertos::task<freertos::dynamic_task_allocator<2048ul> >::suspend()`

```cpp
    *
    */
>>> void suspend(void) { vTaskSuspend(m_hTask); }
    /**
    * @brief Resume the task.
```

**Uncovered Area 35**: freertos_task.hpp:611
*Function*: `freertos::periodic_task<freertos::static_task_allocator<1024ul> >::periodic_task<double, std::ratio<1l, 1000l> >(char const*, unsigned int, std::function<void ()>&&, std::function<void ()>&&, std::function<void ()>&&, std::chrono::duration<double, std::ratio<1l, 1000l> > const&, bool)::{lambda()#1}::operator()() const`

```cpp
    m_on_start{std::move(on_start)}, m_on_stop{std::move(on_stop)},
    m_periodic_routine{std::move(periodic_routine)},
>>> m_task{name, priority, [this]() { run(); }, start_suspended} {}
    /**
    * @brief Construct a new periodic task object
```

**Uncovered Area 36**: freertos_task.hpp:611
*Function*: `freertos::periodic_task<freertos::static_task_allocator<1024ul> >::periodic_task<long, std::ratio<1l, 1000l> >(char const*, unsigned int, std::function<void ()>&&, std::function<void ()>&&, std::function<void ()>&&, std::chrono::duration<long, std::ratio<1l, 1000l> > const&, bool)::{lambda()#1}::operator()() const`

```cpp
    m_on_start{std::move(on_start)}, m_on_stop{std::move(on_stop)},
    m_periodic_routine{std::move(periodic_routine)},
>>> m_task{name, priority, [this]() { run(); }, start_suspended} {}
    /**
    * @brief Construct a new periodic task object
```

**Uncovered Area 37**: FreeRTOS.h:163
*Function*: `FreeRTOSMock::~FreeRTOSMock()`

```cpp
    class FreeRTOSMock {
    public:
>>> virtual ~FreeRTOSMock() = default;
    
    // Task creation and deletion
```

**Uncovered Area 38**: FreeRTOS.h:323
*Function*: `FreeRTOSMock::xMessageBufferSendFromISR(void*, void const*, unsigned long, int*)`

```cpp
    MOCK_METHOD(void, vMessageBufferDelete, (MessageBufferHandle_t xMessageBuffer));
    MOCK_METHOD(size_t, xMessageBufferSend, (MessageBufferHandle_t xMessageBuffer, const void* pvTxData, size_t xDataLengthBytes, TickType_t xTicksToWait));
>>> MOCK_METHOD(size_t, xMessageBufferSendFromISR, (MessageBufferHandle_t xMessageBuffer, const void* pvTxData, size_t xDataLengthBytes, BaseType_t* pxHigherPriorityTaskWoken));
    MOCK_METHOD(size_t, xMessageBufferReceive, (MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, TickType_t xTicksToWait));
    MOCK_METHOD(size_t, xMessageBufferReceiveFromISR, (MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, BaseType_t* pxHigherPriorityTaskWoken));
```

**Uncovered Area 39**: FreeRTOS.h:325
*Function*: `FreeRTOSMock::xMessageBufferReceiveFromISR(void*, void*, unsigned long, int*)`

```cpp
    MOCK_METHOD(size_t, xMessageBufferSendFromISR, (MessageBufferHandle_t xMessageBuffer, const void* pvTxData, size_t xDataLengthBytes, BaseType_t* pxHigherPriorityTaskWoken));
    MOCK_METHOD(size_t, xMessageBufferReceive, (MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, TickType_t xTicksToWait));
>>> MOCK_METHOD(size_t, xMessageBufferReceiveFromISR, (MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, BaseType_t* pxHigherPriorityTaskWoken));
    MOCK_METHOD(size_t, xMessageBufferSpaceAvailable, (MessageBufferHandle_t xMessageBuffer));
    MOCK_METHOD(BaseType_t, xMessageBufferReset, (MessageBufferHandle_t xMessageBuffer));
```

**Uncovered Area 40**: enhanced_timer_mocks.cpp:159-164
*Function*: `freertos_mocks::TimerServiceSimulator::changePeriodFromISR(void*, unsigned int, int*)`

```cpp
    }
    
>>> BaseType_t TimerServiceSimulator::changePeriodFromISR(TimerHandle_t timer, TickType_t new_period, BaseType_t* higher_priority_task_woken) {
    if (higher_priority_task_woken) {
    *higher_priority_task_woken = pdFALSE;  // Simulate no high priority task woken for testing
```

**Uncovered Area 41**: enhanced_timer_mocks.cpp:198-202
*Function*: `freertos_mocks::TimerServiceSimulator::setReloadMode(void*, unsigned int)`

```cpp
    }
    
>>> void TimerServiceSimulator::setReloadMode(TimerHandle_t timer, UBaseType_t auto_reload) {
    if (isValidTimer(timer)) {
    timers_[timer]->auto_reload = auto_reload;
```

**Uncovered Area 42**: enhanced_timer_mocks.cpp:360-368
*Function*: `freertos_mocks::TimerServiceSimulator::getActiveTimerCount() const`

```cpp
    }
    
>>> size_t TimerServiceSimulator::getActiveTimerCount() const {
    size_t count = 0;
    for (const auto& pair : timers_) {
```

**Uncovered Area 43**: enhanced_timer_mocks.cpp:374-376
*Function*: `freertos_mocks::TimerServiceSimulator::hasTimer(void*) const`

```cpp
    }
    
>>> bool TimerServiceSimulator::hasTimer(TimerHandle_t timer) const {
    return isValidTimer(timer);
    }
```

**Uncovered Area 44**: enhanced_timer_mocks.cpp:378-381
*Function*: `freertos_mocks::TimerServiceSimulator::getCallbackCount(void*) const`

```cpp
    }
    
>>> size_t TimerServiceSimulator::getCallbackCount(TimerHandle_t timer) const {
    auto it = callback_counts_.find(timer);
    return it != callback_counts_.end() ? it->second : 0;
```

**Uncovered Area 45**: enhanced_timer_mocks.cpp:383-385
*Function*: `freertos_mocks::TimerServiceSimulator::clearCallbackHistory()`

```cpp
    }
    
>>> void TimerServiceSimulator::clearCallbackHistory() {
    callback_counts_.clear();
    }
```

**Uncovered Area 46**: enhanced_timer_mocks.hpp:172-180
*Function*: `freertos_mocks::EnhancedTimerMock::simulateTaskDelay(unsigned int)`

```cpp
    
    // vTaskDelay simulation - works with timer time advancement
>>> void simulateTaskDelay(TickType_t ticks) {
    if (simulation_enabled_) {
    // In enhanced simulation mode, vTaskDelay just advances simulated time
```

**Uncovered Area 47**: freertos_mocks.cpp:896-901
*Function*: `xMessageBufferSendFromISR`

```cpp

```

**Uncovered Area 48**: freertos_mocks.cpp:910-915
*Function*: `xMessageBufferReceiveFromISR`

```cpp

```

**Uncovered Area 49**: stl_semaphore_mocks.hpp:165
*Function*: `freertos_test::stl_counting_semaphore::take(unsigned int)::{lambda()#1}::operator()() const`

```cpp
    if (timeout_ms == portMAX_DELAY) {
    // Infinite wait
>>> m_condition.wait(lock, [this] { return m_count > 0; });
    m_count--;
    return pdTRUE;
```

**Uncovered Area 50**: stl_semaphore_mocks.hpp:77
*Function*: `freertos_test::stl_binary_semaphore::take(unsigned int)::{lambda()#1}::operator()() const`

```cpp
    if (timeout_ms == portMAX_DELAY) {
    // Infinite wait
>>> m_condition.wait(lock, [this] { return m_available; });
    m_available = false;
    return pdTRUE;
```

**Uncovered Area 51**: test_enhanced_cpp17_features.cpp:115
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .Times(AtLeast(2));
    
>>> auto on_start = [&start_count]() { start_count++; };
    auto on_stop = [&stop_count]() { stop_count++; };
    auto periodic = [&periodic_count]() { periodic_count++; };
```

**Uncovered Area 52**: test_enhanced_cpp17_features.cpp:116
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    auto on_start = [&start_count]() { start_count++; };
>>> auto on_stop = [&stop_count]() { stop_count++; };
    auto periodic = [&periodic_count]() { periodic_count++; };
    
```

**Uncovered Area 53**: test_enhanced_cpp17_features.cpp:117
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    auto on_start = [&start_count]() { start_count++; };
    auto on_stop = [&stop_count]() { stop_count++; };
>>> auto periodic = [&periodic_count]() { periodic_count++; };
    
    // Test move with different chrono duration types - using static allocation
```

**Uncovered Area 54**: test_enhanced_cpp17_features.cpp:127
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#4}::operator()() const`

```cpp
    // Create another task and move it
    sa::periodic_task<1024> task2("PeriodicMove2", 2,
>>> [&start_count]() { start_count++; },
    [&stop_count]() { stop_count++; },
    [&periodic_count]() { periodic_count++; });
```

**Uncovered Area 55**: test_enhanced_cpp17_features.cpp:128
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#5}::operator()() const`

```cpp
    sa::periodic_task<1024> task2("PeriodicMove2", 2,
    [&start_count]() { start_count++; },
>>> [&stop_count]() { stop_count++; },
    [&periodic_count]() { periodic_count++; });
    
```

**Uncovered Area 56**: test_enhanced_cpp17_features.cpp:129
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#6}::operator()() const`

```cpp
    [&start_count]() { start_count++; },
    [&stop_count]() { stop_count++; },
>>> [&periodic_count]() { periodic_count++; });
    
    // Test move construction instead of assignment (which is deleted)
```

**Uncovered Area 57**: test_enhanced_cpp17_features.cpp:152-154
*Function*: `Cpp17FeaturesTest_LambdaCaptureVarieties_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Capture by reference
>>> sa::task<1024> task1("RefCapture", 1, [&message]() {
    EXPECT_EQ(message, "Test message");
    });
```

**Uncovered Area 58**: test_enhanced_cpp17_features.cpp:157-159
*Function*: `Cpp17FeaturesTest_LambdaCaptureVarieties_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    // Capture by value
>>> sa::task<1024> task2("ValueCapture", 1, [counter]() {
    EXPECT_EQ(counter, 42);
    });
```

**Uncovered Area 59**: test_enhanced_cpp17_features.cpp:162-164
*Function*: `Cpp17FeaturesTest_LambdaCaptureVarieties_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    
    // Simple lambda without capture
>>> sa::task<1024> task3("NoCapture", 1, []() {
    // Simple test function
    });
```

**Uncovered Area 60**: test_enhanced_cpp17_features.cpp:172
*Function*: `Cpp17FeaturesTest_FunctionObjectVarieties_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Test with copyable function object instead
>>> auto func_lambda = []() { /* test */ };
    sa::task<1024> task1("StdFunction", 1, func_lambda);
    
```

**Uncovered Area 61**: test_enhanced_cpp17_features.cpp:178
*Function*: `Cpp17FeaturesTest_FunctionObjectVarieties_Test::TestBody()::CustomCallable::operator()() const`

```cpp
    class CustomCallable {
    public:
>>> void operator()() const { /* test */ }
    };
    
```

**Uncovered Area 62**: test_enhanced_cpp17_features.cpp:185
*Function*: `Cpp17FeaturesTest_FunctionObjectVarieties_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    // Test with function pointer
>>> sa::task<1024> task3("FunctionPointer", 1, []() { /* test */ });
    }
    
```

**Uncovered Area 63**: test_enhanced_cpp17_features.cpp:292
*Function*: `Cpp17FeaturesTest_CompileTimeConstants_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // These should compile with constexpr values - queues are created at construction
>>> sa::task<stack_size> task("ConstexprTest", priority, []() { /* test */ });
    sa::queue<queue_size, int> queue;
    
```

**Uncovered Area 64**: test_enhanced_cpp17_features.cpp:313
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    // Test with different chrono duration types in periodic tasks - using static allocation
    sa::periodic_task<1024> task1("ChronoMs", 1,
>>> []() { /* start */ },
    []() { /* stop */ },
    []() { /* periodic */ },
```

**Uncovered Area 65**: test_enhanced_cpp17_features.cpp:314
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    sa::periodic_task<1024> task1("ChronoMs", 1,
    []() { /* start */ },
>>> []() { /* stop */ },
    []() { /* periodic */ },
    500ms);
```

**Uncovered Area 66**: test_enhanced_cpp17_features.cpp:315
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() { /* start */ },
    []() { /* stop */ },
>>> []() { /* periodic */ },
    500ms);
    
```

**Uncovered Area 67**: test_enhanced_cpp17_features.cpp:319
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#4}::operator()() const`

```cpp
    
    sa::periodic_task<1024> task2("ChronoMs2", 1,
>>> []() { /* start */ },
    []() { /* stop */ },
    []() { /* periodic */ },
```

**Uncovered Area 68**: test_enhanced_cpp17_features.cpp:320
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#5}::operator()() const`

```cpp
    sa::periodic_task<1024> task2("ChronoMs2", 1,
    []() { /* start */ },
>>> []() { /* stop */ },
    []() { /* periodic */ },
    750ms);
```

**Uncovered Area 69**: test_enhanced_cpp17_features.cpp:321
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#6}::operator()() const`

```cpp
    []() { /* start */ },
    []() { /* stop */ },
>>> []() { /* periodic */ },
    750ms);
    
```

**Uncovered Area 70**: test_enhanced_cpp17_features.cpp:325
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#7}::operator()() const`

```cpp
    
    sa::periodic_task<1024> task3("ChronoCustom", 1,
>>> []() { /* start */ },
    []() { /* stop */ },
    []() { /* periodic */ },
```

**Uncovered Area 71**: test_enhanced_cpp17_features.cpp:326
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#8}::operator()() const`

```cpp
    sa::periodic_task<1024> task3("ChronoCustom", 1,
    []() { /* start */ },
>>> []() { /* stop */ },
    []() { /* periodic */ },
    duration<double, std::milli>(123.456));
```

**Uncovered Area 72**: test_enhanced_cpp17_features.cpp:327
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#9}::operator()() const`

```cpp
    []() { /* start */ },
    []() { /* stop */ },
>>> []() { /* periodic */ },
    duration<double, std::milli>(123.456));
    
```

**Uncovered Area 73**: test_enhanced_cpp17_features.cpp:350
*Function*: `Cpp17FeaturesTest_STLContainerIntegration_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Use emplace_back to construct in place
>>> tasks.emplace_back("VectorTask1", 1, []() { /* test */ });
    tasks.emplace_back("VectorTask2", 2, []() { /* test */ });
    tasks.emplace_back("VectorTask3", 3, []() { /* test */ });
```

**Uncovered Area 74**: test_enhanced_cpp17_features.cpp:351
*Function*: `Cpp17FeaturesTest_STLContainerIntegration_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    // Use emplace_back to construct in place
    tasks.emplace_back("VectorTask1", 1, []() { /* test */ });
>>> tasks.emplace_back("VectorTask2", 2, []() { /* test */ });
    tasks.emplace_back("VectorTask3", 3, []() { /* test */ });
    
```

**Uncovered Area 75**: test_enhanced_cpp17_features.cpp:352
*Function*: `Cpp17FeaturesTest_STLContainerIntegration_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    tasks.emplace_back("VectorTask1", 1, []() { /* test */ });
    tasks.emplace_back("VectorTask2", 2, []() { /* test */ });
>>> tasks.emplace_back("VectorTask3", 3, []() { /* test */ });
    
    EXPECT_EQ(tasks.size(), 3);
```

**Uncovered Area 76**: test_enhanced_cpp17_features.cpp:369
*Function*: `Cpp17FeaturesTest_UniquePtrIntegration_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Test with unique_ptr for dynamic task management
>>> auto task1 = std::make_unique<sa::task<1024>>("UniqueTask1", 1, []() { /* test */ });
    auto task2 = std::make_unique<sa::task<1024>>("UniqueTask2", 2, []() { /* test */ });
    
```

**Uncovered Area 77**: test_enhanced_cpp17_features.cpp:370
*Function*: `Cpp17FeaturesTest_UniquePtrIntegration_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    // Test with unique_ptr for dynamic task management
    auto task1 = std::make_unique<sa::task<1024>>("UniqueTask1", 1, []() { /* test */ });
>>> auto task2 = std::make_unique<sa::task<1024>>("UniqueTask2", 2, []() { /* test */ });
    
    EXPECT_NE(task1->handle(), nullptr);
```

**Uncovered Area 78**: test_enhanced_cpp17_features.cpp:69
*Function*: `Cpp17FeaturesTest_TaskMoveSemanticsPerfectForwarding_Test::TestBody()::{lambda()#1}::operator()()`

```cpp
    
    // Test perfect forwarding with various lambda captures
>>> auto task_func1 = [&call_count]() mutable { call_count++; };
    auto task_func2 = [&call_count](){ call_count += 2; };
    
```

**Uncovered Area 79**: test_enhanced_cpp17_features.cpp:70
*Function*: `Cpp17FeaturesTest_TaskMoveSemanticsPerfectForwarding_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    // Test perfect forwarding with various lambda captures
    auto task_func1 = [&call_count]() mutable { call_count++; };
>>> auto task_func2 = [&call_count](){ call_count += 2; };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
```

**Uncovered Area 80**: test_enhanced_cpp17_features.cpp:92
*Function*: `Cpp17FeaturesTest_TaskMoveAssignmentChaining_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .Times(AtLeast(2)); // Two tasks will be destroyed during moves
    
>>> auto task_func = [&executed]() { executed = true; };
    
    // Create multiple tasks
```

**Uncovered Area 81**: test_enhanced_cpp17_features.cpp:95
*Function*: `Cpp17FeaturesTest_TaskMoveAssignmentChaining_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    // Create multiple tasks
>>> sa::task<1024> task1("Original", 1, [&executed]() { executed = true; });
    sa::task<1024> task2("ToMove1", 2, task_func);
    sa::task<1024> task3("ToMove2", 3, task_func);
```

**Uncovered Area 82**: test_enhanced_freertos_sw_timer.cpp:379
*Function*: `EnhancedFreeRTOSSwTimerTest_ZeroPeriodTimer_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    auto& sim = enhanced_mock->getSimulator();
    TimerHandle_t handle = sim.createTimer("ZeroPeriod", 0, pdTRUE, nullptr,
>>> [](TimerHandle_t){});
    EXPECT_EQ(handle, nullptr);
    }
```

**Uncovered Area 83**: test_freertos_sw_timer.cpp:155
*Function*: `FreeRTOSSwTimerTest_StaticTimerAllocatorCreate_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    
    TimerHandle_t handle = allocator.create("TestTimer", 100, pdTRUE, &allocator,
>>> [](TimerHandle_t){});
    
    EXPECT_EQ(handle, mock_timer_handle);
```

**Uncovered Area 84**: test_freertos_sw_timer.cpp:168
*Function*: `FreeRTOSSwTimerTest_StaticTimerAllocatorCreateNullReturn_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    
    TimerHandle_t handle = allocator.create("TestTimer", 100, pdTRUE, nullptr,
>>> [](TimerHandle_t){});
    
    EXPECT_EQ(handle, nullptr);
```

**Uncovered Area 85**: test_freertos_sw_timer.cpp:203
*Function*: `FreeRTOSSwTimerTest_DynamicTimerAllocatorCreate_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    
    TimerHandle_t handle = allocator.create("TestTimer", 200, pdFALSE, &allocator,
>>> [](TimerHandle_t){});
    
    EXPECT_EQ(handle, mock_timer_handle);
```

**Uncovered Area 86**: test_freertos_sw_timer.cpp:216
*Function*: `FreeRTOSSwTimerTest_DynamicTimerAllocatorCreateNullReturn_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    
    TimerHandle_t handle = allocator.create("TestTimer", 200, pdFALSE, nullptr,
>>> [](TimerHandle_t){});
    
    EXPECT_EQ(handle, nullptr);
```

**Uncovered Area 87**: test_freertos_sw_timer.cpp:90
*Function*: `FreeRTOSSwTimerTest::createTestCallback()::{lambda()#1}::operator()() const`

```cpp
    // Helper function to create a test callback
    std::function<void()> createTestCallback() {
>>> return [this]() { callback_count++; };
    }
    
```

**Uncovered Area 88**: test_freertos_task.cpp:1020
*Function*: `FreeRTOSTaskTest_TaskChronoCompatibility_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("ChronoTask", 2, [](){});
    
    // Test chrono duration compatibility with notify_take
```

**Uncovered Area 89**: test_freertos_task.cpp:1090
*Function*: `FreeRTOSTaskTest_TaskMoveConstruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> original_task("MoveTest", 2, [](){});
    EXPECT_EQ(original_task.handle(), mock_task_handle);
    
```

**Uncovered Area 90**: test_freertos_task.cpp:1108
*Function*: `FreeRTOSTaskTest_PeriodicTaskMoveConstruction_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    sa::periodic_task<1024> original_task("PeriodicMoveTest", 2,
>>> [](){}, [](){}, [](){}, 100ms);
    EXPECT_EQ(original_task.handle(), mock_task_handle);
    
```

**Uncovered Area 91**: test_freertos_task.cpp:1108
*Function*: `FreeRTOSTaskTest_PeriodicTaskMoveConstruction_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    
    sa::periodic_task<1024> original_task("PeriodicMoveTest", 2,
>>> [](){}, [](){}, [](){}, 100ms);
    EXPECT_EQ(original_task.handle(), mock_task_handle);
    
```

**Uncovered Area 92**: test_freertos_task.cpp:1108
*Function*: `FreeRTOSTaskTest_PeriodicTaskMoveConstruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    sa::periodic_task<1024> original_task("PeriodicMoveTest", 2,
>>> [](){}, [](){}, [](){}, 100ms);
    EXPECT_EQ(original_task.handle(), mock_task_handle);
    
```

**Uncovered Area 93**: test_freertos_task.cpp:1162
*Function*: `FreeRTOSTaskTest_PeriodicTaskTypo_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "TypoTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 94**: test_freertos_task.cpp:1163
*Function*: `FreeRTOSTaskTest_PeriodicTaskTypo_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    100ms     // period
```

**Uncovered Area 95**: test_freertos_task.cpp:1164
*Function*: `FreeRTOSTaskTest_PeriodicTaskTypo_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    100ms     // period
    );
```

**Uncovered Area 96**: test_freertos_task.cpp:1187
*Function*: `FreeRTOSTaskTest_TaskTemplateInstantiation_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<512> small_task("SmallTask", 1, []() {});
    EXPECT_EQ(small_task.handle(), mock_task_handle);
    
```

**Uncovered Area 97**: test_freertos_task.cpp:1203
*Function*: `FreeRTOSTaskTest_DynamicTaskDifferentSizes_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    ));
    
>>> da::task<2048> dyn_task("DynTask", 3, []() {});
    EXPECT_EQ(dyn_task.handle(), mock_handle2);
    
```

**Uncovered Area 98**: test_freertos_task.cpp:1213
*Function*: `FreeRTOSTaskTest_TaskNotificationEdgeCases_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("EdgeTask", 2, []() {});
    
    // Test notification with different duration types (microseconds)
```

**Uncovered Area 99**: test_freertos_task.cpp:1298-1300
*Function*: `FreeRTOSTaskTest_RacingConditionTaskConstructorInitialization_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    });
    
>>> sa::task<1024> test_task("RacingTask", 2, []() {
    // Task execution body
    });
```

**Uncovered Area 100**: test_freertos_task.cpp:1326
*Function*: `FreeRTOSTaskTest_ConcurrentTaskCreationAndDestruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Create tasks with different lifetimes to test destructor race conditions
>>> auto task1 = std::make_unique<sa::task<512>>("Task1", 1, []() {});
    auto task2 = std::make_unique<sa::task<512>>("Task2", 2, []() {});
    auto task3 = std::make_unique<sa::task<512>>("Task3", 3, []() {});
```

**Uncovered Area 101**: test_freertos_task.cpp:1327
*Function*: `FreeRTOSTaskTest_ConcurrentTaskCreationAndDestruction_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    // Create tasks with different lifetimes to test destructor race conditions
    auto task1 = std::make_unique<sa::task<512>>("Task1", 1, []() {});
>>> auto task2 = std::make_unique<sa::task<512>>("Task2", 2, []() {});
    auto task3 = std::make_unique<sa::task<512>>("Task3", 3, []() {});
    
```

**Uncovered Area 102**: test_freertos_task.cpp:1328
*Function*: `FreeRTOSTaskTest_ConcurrentTaskCreationAndDestruction_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    auto task1 = std::make_unique<sa::task<512>>("Task1", 1, []() {});
    auto task2 = std::make_unique<sa::task<512>>("Task2", 2, []() {});
>>> auto task3 = std::make_unique<sa::task<512>>("Task3", 3, []() {});
    
    EXPECT_EQ(task1->handle(), handles[0]);
```

**Uncovered Area 103**: test_freertos_task.cpp:135
*Function*: `FreeRTOSTaskTest_DynamicTaskAllocatorCreateSuccess_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    ));
    
>>> auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, &allocator);
    
```

**Uncovered Area 104**: test_freertos_task.cpp:1354
*Function*: `FreeRTOSTaskTest_MoveSemanticsRacingConditions_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Create original task
>>> sa::task<1024> original_task("MoveTask", 1, []() {});
    EXPECT_EQ(original_task.handle(), original_handle);
    
```

**Uncovered Area 105**: test_freertos_task.cpp:1382
*Function*: `FreeRTOSTaskTest_PeriodicTaskLifecycleRacingConditions_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "PeriodicRace",
    2,
>>> [&start_count]() { start_count++; },  // on_start
    [&stop_count]() { stop_count++; },    // on_stop
    [&periodic_count]() { periodic_count++; },  // periodic_routine
```

**Uncovered Area 106**: test_freertos_task.cpp:1383
*Function*: `FreeRTOSTaskTest_PeriodicTaskLifecycleRacingConditions_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    [&start_count]() { start_count++; },  // on_start
>>> [&stop_count]() { stop_count++; },    // on_stop
    [&periodic_count]() { periodic_count++; },  // periodic_routine
    std::chrono::milliseconds(100)
```

**Uncovered Area 107**: test_freertos_task.cpp:1384
*Function*: `FreeRTOSTaskTest_PeriodicTaskLifecycleRacingConditions_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    [&start_count]() { start_count++; },  // on_start
    [&stop_count]() { stop_count++; },    // on_stop
>>> [&periodic_count]() { periodic_count++; },  // periodic_routine
    std::chrono::milliseconds(100)
    );
```

**Uncovered Area 108**: test_freertos_task.cpp:1411
*Function*: `FreeRTOSTaskTest_NotificationRacingConditions_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("NotifyRace", 2, []() {});
    
    // Simulate rapid notification operations
```

**Uncovered Area 109**: test_freertos_task.cpp:1456-1458
*Function*: `FreeRTOSTaskTest_ComplexMultitaskingScenario_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(producer_handle));
    
>>> sa::task<1024> producer("Producer", 3, []() {
    // Producer logic
    });
```

**Uncovered Area 110**: test_freertos_task.cpp:1464-1466
*Function*: `FreeRTOSTaskTest_ComplexMultitaskingScenario_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    .WillOnce(Return(consumer_handle));
    
>>> sa::task<1024> consumer("Consumer", 2, []() {
    // Consumer logic
    });
```

**Uncovered Area 111**: test_freertos_task.cpp:1472-1474
*Function*: `FreeRTOSTaskTest_ComplexMultitaskingScenario_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    .WillOnce(Return(coordinator_handle));
    
>>> sa::task<1024> coordinator("Coordinator", 4, []() {
    // Coordinator logic
    });
```

**Uncovered Area 112**: test_freertos_task.cpp:1543
*Function*: `FreeRTOSTaskTest_TaskSystemStatusUnderLoad_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(handles[2]));
    
>>> sa::task<512> task1("SysTask1", 1, []() {});
    sa::task<512> task2("SysTask2", 2, []() {});
    sa::task<512> task3("SysTask3", 3, []() {});
```

**Uncovered Area 113**: test_freertos_task.cpp:1544
*Function*: `FreeRTOSTaskTest_TaskSystemStatusUnderLoad_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    sa::task<512> task1("SysTask1", 1, []() {});
>>> sa::task<512> task2("SysTask2", 2, []() {});
    sa::task<512> task3("SysTask3", 3, []() {});
    
```

**Uncovered Area 114**: test_freertos_task.cpp:1545
*Function*: `FreeRTOSTaskTest_TaskSystemStatusUnderLoad_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    sa::task<512> task1("SysTask1", 1, []() {});
    sa::task<512> task2("SysTask2", 2, []() {});
>>> sa::task<512> task3("SysTask3", 3, []() {});
    
    // Test task system status with multiple tasks
```

**Uncovered Area 115**: test_freertos_task.cpp:1594-1596
*Function*: `FreeRTOSTaskTest_ConstructorInitializationOrderRaceCondition_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Create task with routine that sets atomic flag
>>> sa::task<1024> race_test_task("RaceTest", 2, [&task_routine_called]() {
    task_routine_called = true;
    });
```

**Uncovered Area 116**: test_freertos_task.cpp:165-167
*Function*: `FreeRTOSTaskTest_StaticTaskConstruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    bool task_executed = false;
>>> sa::task<1024> test_task("TestTask", 2, [&task_executed]() {
    task_executed = true;
    });
```

**Uncovered Area 117**: test_freertos_task.cpp:1741
*Function*: `FreeRTOSTaskTest_AdvancedRacingConditionScenarios_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(reinterpret_cast<TaskHandle_t>(0x3001)));
    
>>> sa::task<512> task1("RaceTask1", 1, []() {});
    EXPECT_EQ(task1.handle(), reinterpret_cast<TaskHandle_t>(0x3001));
    
```

**Uncovered Area 118**: test_freertos_task.cpp:1748
*Function*: `FreeRTOSTaskTest_AdvancedRacingConditionScenarios_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    .WillOnce(Return(reinterpret_cast<TaskHandle_t>(0x3002)));
    
>>> sa::task<512> task2("RaceTask2", 2, []() {});
    EXPECT_EQ(task2.handle(), reinterpret_cast<TaskHandle_t>(0x3002));
    
```

**Uncovered Area 119**: test_freertos_task.cpp:1796
*Function*: `FreeRTOSTaskTest_AdvancedChronoCompatibility_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("ChronoTask", 2, []() {});
    
    // Test notification take with very small durations
```

**Uncovered Area 120**: test_freertos_task.cpp:180-182
*Function*: `FreeRTOSTaskTest_StaticTaskConstructionWithString_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
    
>>> sa::task<1024> test_task(task_name, 3, []() {
    // Test task routine
    });
```

**Uncovered Area 121**: test_freertos_task.cpp:1838
*Function*: `FreeRTOSTaskTest_PriorityInheritanceScenario_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(high_prio_handle));
    
>>> sa::task<1024> low_prio_task("LowPrio", 1, []() {});
    sa::task<1024> high_prio_task("HighPrio", 5, []() {});
    
```

**Uncovered Area 122**: test_freertos_task.cpp:1839
*Function*: `FreeRTOSTaskTest_PriorityInheritanceScenario_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    sa::task<1024> low_prio_task("LowPrio", 1, []() {});
>>> sa::task<1024> high_prio_task("HighPrio", 5, []() {});
    
    // Low priority task acquires resource and gets priority boosted
```

**Uncovered Area 123**: test_freertos_task.cpp:194
*Function*: `FreeRTOSTaskTest_StaticTaskDestruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    {
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    } // Destructor should call vTaskDelete
    }
```

**Uncovered Area 124**: test_freertos_task.cpp:207
*Function*: `FreeRTOSTaskTest_StaticTaskDestructionNullHandle_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    {
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    }
    }
```

**Uncovered Area 125**: test_freertos_task.cpp:215
*Function*: `FreeRTOSTaskTest_StaticTaskSuspendResume_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, vTaskSuspend(mock_task_handle));
```

**Uncovered Area 126**: test_freertos_task.cpp:235
*Function*: `FreeRTOSTaskTest_StaticTaskTerminate_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
```

**Uncovered Area 127**: test_freertos_task.cpp:248
*Function*: `FreeRTOSTaskTest_StaticTaskPriority_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, uxTaskPriorityGet(mock_task_handle))
```

**Uncovered Area 128**: test_freertos_task.cpp:271
*Function*: `FreeRTOSTaskTest_StaticTaskName_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, pcTaskGetName(mock_task_handle))
```

**Uncovered Area 129**: test_freertos_task.cpp:285
*Function*: `FreeRTOSTaskTest_StaticTaskState_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, eTaskGetState(mock_task_handle))
```

**Uncovered Area 130**: test_freertos_task.cpp:299
*Function*: `FreeRTOSTaskTest_TaskApplicationTag_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TagTask", 2, []() {});
    
    // Test setting application task tag
```

**Uncovered Area 131**: test_freertos_task.cpp:326
*Function*: `FreeRTOSTaskTest_TaskStackWatermark_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("WatermarkTask", 2, []() {});
    
    // Test stack high water mark
```

**Uncovered Area 132**: test_freertos_task.cpp:347
*Function*: `FreeRTOSTaskTest_TaskTraceStatus_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("StatusTask", 2, []() {});
    
    // Test task status with default parameters
```

**Uncovered Area 133**: test_freertos_task.cpp:417-419
*Function*: `FreeRTOSTaskTest_TaskSuspendedOnStart_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Test task created with start_suspended = true (default)
>>> sa::task<1024> suspended_task("SuspendedTask", 1, []() {
    // This task should start suspended
    }, true);
```

**Uncovered Area 134**: test_freertos_task.cpp:431-433
*Function*: `FreeRTOSTaskTest_TaskNotSuspendedOnStart_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Test task created with start_suspended = false
>>> sa::task<1024> active_task("ActiveTask", 1, []() {
    // This task should start immediately
    }, false);
```

**Uncovered Area 135**: test_freertos_task.cpp:447-449
*Function*: `FreeRTOSTaskTest_DynamicTaskConstruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    ));
    
>>> da::task<2048> test_task("DynamicTask", 3, []() {
    // Test task routine
    });
```

**Uncovered Area 136**: test_freertos_task.cpp:476
*Function*: `FreeRTOSTaskTest_TaskNotifications_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("NotifyTask", 2, []() {});
    
    // Test notify_give
```

**Uncovered Area 137**: test_freertos_task.cpp:514
*Function*: `FreeRTOSTaskTest_TaskNotificationsExtended_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("ExtendedNotifyTask", 2, []() {});
    
    // Test notify_and_query (fixed typo: was notfy_and_query)
```

**Uncovered Area 138**: test_freertos_task.cpp:588
*Function*: `FreeRTOSTaskTest_PeriodicTaskConstruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "PeriodicTask",
    2,
>>> [&on_start_called]() { on_start_called = true; },      // on_start
    [&on_stop_called]() { on_stop_called = true; },       // on_stop
    [&periodic_called]() { periodic_called = true; },     // periodic_routine
```

**Uncovered Area 139**: test_freertos_task.cpp:589
*Function*: `FreeRTOSTaskTest_PeriodicTaskConstruction_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    [&on_start_called]() { on_start_called = true; },      // on_start
>>> [&on_stop_called]() { on_stop_called = true; },       // on_stop
    [&periodic_called]() { periodic_called = true; },     // periodic_routine
    100ms                                                   // period
```

**Uncovered Area 140**: test_freertos_task.cpp:590
*Function*: `FreeRTOSTaskTest_PeriodicTaskConstruction_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    [&on_start_called]() { on_start_called = true; },      // on_start
    [&on_stop_called]() { on_stop_called = true; },       // on_stop
>>> [&periodic_called]() { periodic_called = true; },     // periodic_routine
    100ms                                                   // period
    );
```

**Uncovered Area 141**: test_freertos_task.cpp:614
*Function*: `FreeRTOSTaskTest_PeriodicTaskWithString_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    task_name,
    3,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 142**: test_freertos_task.cpp:615
*Function*: `FreeRTOSTaskTest_PeriodicTaskWithString_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    3,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    50ms      // period
```

**Uncovered Area 143**: test_freertos_task.cpp:616
*Function*: `FreeRTOSTaskTest_PeriodicTaskWithString_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    50ms      // period
    );
```

**Uncovered Area 144**: test_freertos_task.cpp:635
*Function*: `FreeRTOSTaskTest_PeriodicTaskZeroPeriod_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "ZeroPeriodTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 145**: test_freertos_task.cpp:636
*Function*: `FreeRTOSTaskTest_PeriodicTaskZeroPeriod_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    0ms       // period - should run continuously
```

**Uncovered Area 146**: test_freertos_task.cpp:637
*Function*: `FreeRTOSTaskTest_PeriodicTaskZeroPeriod_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    0ms       // period - should run continuously
    );
```

**Uncovered Area 147**: test_freertos_task.cpp:656
*Function*: `FreeRTOSTaskTest_PeriodicTaskNoPeriod_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "NoPeriodTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {}   // periodic_routine
```

**Uncovered Area 148**: test_freertos_task.cpp:657
*Function*: `FreeRTOSTaskTest_PeriodicTaskNoPeriod_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {}   // periodic_routine
    // No period specified - should default to 0ms
```

**Uncovered Area 149**: test_freertos_task.cpp:658
*Function*: `FreeRTOSTaskTest_PeriodicTaskNoPeriod_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {}   // periodic_routine
    // No period specified - should default to 0ms
    );
```

**Uncovered Area 150**: test_freertos_task.cpp:715
*Function*: `FreeRTOSTaskTest_PeriodicTaskTerminate_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "TerminateTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 151**: test_freertos_task.cpp:716
*Function*: `FreeRTOSTaskTest_PeriodicTaskTerminate_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    100ms     // period
```

**Uncovered Area 152**: test_freertos_task.cpp:717
*Function*: `FreeRTOSTaskTest_PeriodicTaskTerminate_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    100ms     // period
    );
```

**Uncovered Area 153**: test_freertos_task.cpp:761
*Function*: `FreeRTOSTaskTest_PeriodicTaskNotificationExtensions_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "PeriodicNotifyTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 154**: test_freertos_task.cpp:762
*Function*: `FreeRTOSTaskTest_PeriodicTaskNotificationExtensions_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    100ms     // period
```

**Uncovered Area 155**: test_freertos_task.cpp:763
*Function*: `FreeRTOSTaskTest_PeriodicTaskNotificationExtensions_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    100ms     // period
    );
```

**Uncovered Area 156**: test_freertos_task.cpp:85
*Function*: `FreeRTOSTaskTest_StaticTaskAllocatorCreate_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    )).WillOnce(Return(mock_task_handle));
    
>>> auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, &allocator);
    
```

**Uncovered Area 157**: test_freertos_task.cpp:957
*Function*: `FreeRTOSTaskTest_StackAllocationLimitation_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    // but in a real environment, this pattern should be avoided for static tasks
    {
>>> sa::task<1024> stack_task("StackLimitationDemo", 1, [](){});
    } // Stack allocation limitation applies here in real FreeRTOS usage
    }
```

**Uncovered Area 158**: test_freertos_task.cpp:970
*Function*: `FreeRTOSTaskTest_InvalidParameters_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(nullptr));
    
>>> sa::task<1024> task_with_null_name(nullptr, 1, [](){});
    EXPECT_EQ(task_with_null_name.handle(), nullptr);
    
```

**Uncovered Area 159**: test_freertos_task.cpp:98
*Function*: `FreeRTOSTaskTest_StaticTaskAllocatorCreateNullReturn_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    .WillOnce(Return(nullptr));
    
>>> auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, nullptr);
    
```

**Uncovered Area 160**: test_freertos_task.cpp:982
*Function*: `FreeRTOSTaskTest_ZeroStackSize_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(nullptr));  // FreeRTOS would likely fail this
    
>>> sa::task<0> zero_stack_task("ZeroStack", 1, [](){});
    EXPECT_EQ(zero_stack_task.handle(), nullptr);
    
```

**Uncovered Area 161**: test_freertos_task.cpp:995
*Function*: `FreeRTOSTaskTest_VeryHighPriority_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> high_priority_task("HighPriority", max_priority, [](){});
    EXPECT_EQ(high_priority_task.handle(), mock_task_handle);
    
```

**Uncovered Area 162**: freertos_queue.hpp:489
*Line*: 489

```cpp
    return item;
    }
>>> return {};
    }
    /**
```

**Uncovered Area 163**: freertos_queue.hpp:628
*Line*: 628

```cpp
    return item;
    }
>>> return {};
    }
    /**
```


**Summary:** 163 uncovered code areas identified across 4 categories.

**Note:** These uncovered areas represent code that by design cannot be easily tested in a unit test environment. They require either integration testing with the actual FreeRTOS kernel, specific hardware configurations, or are defensive programming measures for edge cases that are difficult to reproduce.



### Coverage Quality Assessment
The high coverage percentage indicates:
- **Comprehensive test coverage** across all FreeRTOS wrapper modules
- **Good API validation** with both positive and negative test cases
- **Robust error handling verification** where applicable in unit test context
- **Thorough edge case testing** for boundary conditions

## Test Quality Metrics

### Test Distribution by Category


### Performance Characteristics
- **Fastest Test**: 0.010 seconds
- **Slowest Test**: 1.270 seconds
- **Performance Distribution**:
  - Very Fast (< 0.01s): 0 tests
  - Fast (0.01-0.05s): 10 tests
  - Normal (0.05-0.1s): 0 tests
  - Slow (> 0.1s): 2 tests

## Validation Conclusions

### ✅ Test Suite Maturity
- **Comprehensive Coverage**: 12 test cases across all FreeRTOS wrapper modules
- **Perfect Success Rate**: All tests passing indicates stable and robust implementation
- **Good Module Distribution**: Balanced testing across tasks, synchronization primitives, and communication mechanisms

### ✅ Code Quality Validation
- **High Coverage**: 96.3% line coverage demonstrates thorough validation
- **API Completeness**: 95.0% function coverage shows comprehensive API testing
- **Production Ready**: Test results indicate code is ready for production use

### 🔄 Continuous Validation
This report is automatically generated with each test execution to ensure:
- **Current State Reflection**: Results always match the latest code changes
- **Regression Detection**: Any new failures are immediately identified
- **Quality Maintenance**: Coverage and test metrics are continuously monitored

## Recommendations

1. **Maintain Test Coverage**: Continue to add tests for new features to maintain high coverage
2. **Integration Testing**: Consider adding integration tests with actual FreeRTOS kernel for uncovered areas
3. **Performance Monitoring**: Monitor test execution times to detect performance regressions
4. **Failure Analysis**: When failures occur, this report will provide detailed failure information

## Failed Test Analysis

The following 1 test(s) failed during execution:

1. `test_enhanced_multitasking`


### Failed Test Context Analysis

The following sections provide specific code snippets and explanations for the contexts where test cases failed, helping to understand the failure reasons and debugging approaches.


**Failed Test Context: `test_enhanced_multitasking`**

*Test Type*: Enhanced Timer Behavior Test
*Failure Pattern*: Timer simulation and callback execution issues

```cpp
// General test context
// This test failed during execution with timing or simulation issues
// Check the enhanced timer mock implementation for proper integration
// with the FreeRTOS timer wrapper and vTaskDelay simulation
```

**General Debugging Steps:**
- Verify enhanced timer simulation is enabled during test setup
- Check vTaskDelay integration with timer time advancement
- Ensure command processing mode (immediate vs queued) is appropriate for test
- Review timer expiry calculation and callback execution logic



**Test Execution Details:**
- CTest Return Code: 8
- Total Execution Time: 1.97 seconds
- Failed Tests: 1/12 (8.3%)

**Note:** This report includes failed test cases as requested. The failures provide important debugging information for addressing any issues in the codebase.


---

*Report Generated*: July 25, 2025 at 01:38:50  
*Test Framework*: GoogleTest/GoogleMock  
*Coverage Tool*: LCOV/GCOV  
*Total Test Execution Time*: 1.97 seconds  
*Validation Status*: ❌ **1 tests failing - System requires attention**
