# Validation and Verification Report

## Executive Summary

This report provides comprehensive validation and verification results for the FreeRTOS C++ Wrappers project, including detailed test execution outcomes and code coverage analysis.

### Test Execution Summary
- **Total Tests Executed**: 421
- **✅ Passed**: 421 tests
- **❌ Failed**: 0 tests
- **Success Rate**: 100.0% (All tests passed!)
- **Total Execution Time**: 5.00 seconds
- **Average Test Time**: 0.0119 seconds per test

### Code Coverage Summary
- **Line Coverage**: 96.6% (6836/7076 lines)
- **Function Coverage**: 94.9% (2777/2925 functions)
- **Coverage Scope**: Main library modules only (src/ and include/ directories)

## Detailed Test Results by Module

### Task Module Tests

**Module Statistics:**
- Tests: 81
- Passed: 81
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.810s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 1 | FreeRTOSTaskTest.StaticTaskAllocatorConstruction | ✅ PASS | 0.010s |
| 2 | FreeRTOSTaskTest.StaticTaskAllocatorCreate | ✅ PASS | 0.010s |
| 3 | FreeRTOSTaskTest.StaticTaskAllocatorCreateNullReturn | ✅ PASS | 0.010s |
| 4 | FreeRTOSTaskTest.DynamicTaskAllocatorConstruction | ✅ PASS | 0.010s |
| 5 | FreeRTOSTaskTest.DynamicTaskAllocatorCreateSuccess | ✅ PASS | 0.010s |
| 6 | FreeRTOSTaskTest.DynamicTaskAllocatorCreateFailure | ✅ PASS | 0.010s |
| 7 | FreeRTOSTaskTest.StaticTaskConstruction | ✅ PASS | 0.010s |
| 8 | FreeRTOSTaskTest.StaticTaskConstructionWithString | ✅ PASS | 0.010s |
| 9 | FreeRTOSTaskTest.StaticTaskDestruction | ✅ PASS | 0.010s |
| 10 | FreeRTOSTaskTest.StaticTaskDestructionNullHandle | ✅ PASS | 0.010s |
| 11 | FreeRTOSTaskTest.StaticTaskSuspendResume | ✅ PASS | 0.010s |
| 12 | FreeRTOSTaskTest.StaticTaskTerminate | ✅ PASS | 0.010s |
| 13 | FreeRTOSTaskTest.StaticTaskPriority | ✅ PASS | 0.010s |
| 14 | FreeRTOSTaskTest.StaticTaskName | ✅ PASS | 0.010s |
| 15 | FreeRTOSTaskTest.StaticTaskState | ✅ PASS | 0.010s |
| 16 | FreeRTOSTaskTest.TaskApplicationTag | ✅ PASS | 0.010s |
| 17 | FreeRTOSTaskTest.TaskStackWatermark | ✅ PASS | 0.010s |
| 18 | FreeRTOSTaskTest.TaskTraceStatus | ✅ PASS | 0.010s |
| 19 | FreeRTOSTaskTest.StaticTaskAbortDelay | ✅ PASS | 0.010s |
| 20 | FreeRTOSTaskTest.StaticTaskAbortDelayNullHandle | ✅ PASS | 0.010s |
| 21 | FreeRTOSTaskTest.TaskSuspendedOnStart | ✅ PASS | 0.010s |
| 22 | FreeRTOSTaskTest.TaskNotSuspendedOnStart | ✅ PASS | 0.010s |
| 23 | FreeRTOSTaskTest.DynamicTaskConstruction | ✅ PASS | 0.010s |
| 24 | FreeRTOSTaskTest.DynamicTaskConstructionFailure | ✅ PASS | 0.010s |
| 25 | FreeRTOSTaskTest.TaskNotifications | ✅ PASS | 0.010s |
| 26 | FreeRTOSTaskTest.TaskNotificationsExtended | ✅ PASS | 0.010s |
| 27 | FreeRTOSTaskTest.PeriodicTaskConstruction | ✅ PASS | 0.010s |
| 28 | FreeRTOSTaskTest.PeriodicTaskWithString | ✅ PASS | 0.010s |
| 29 | FreeRTOSTaskTest.PeriodicTaskZeroPeriod | ✅ PASS | 0.010s |
| 30 | FreeRTOSTaskTest.PeriodicTaskNoPeriod | ✅ PASS | 0.010s |
| 31 | FreeRTOSTaskTest.PeriodicTaskIsRunning | ✅ PASS | 0.010s |
| 32 | FreeRTOSTaskTest.PeriodicTaskTerminate | ✅ PASS | 0.010s |
| 33 | FreeRTOSTaskTest.PeriodicTaskDestructorAbortDelay | ✅ PASS | 0.010s |
| 34 | FreeRTOSTaskTest.PeriodicTaskNotificationExtensions | ✅ PASS | 0.010s |
| 35 | FreeRTOSTaskTest.DelayFunction | ✅ PASS | 0.010s |
| 36 | FreeRTOSTaskTest.DelayUntilFunction | ✅ PASS | 0.010s |
| 37 | FreeRTOSTaskTest.SleepForFunction | ✅ PASS | 0.010s |
| 38 | FreeRTOSTaskTest.CurrentTaskHandle | ✅ PASS | 0.010s |
| 39 | FreeRTOSTaskTest.IdleTaskHandle | ✅ PASS | 0.010s |
| 40 | FreeRTOSTaskTest.TickCount | ✅ PASS | 0.010s |
| 41 | FreeRTOSTaskTest.TaskCount | ✅ PASS | 0.010s |
| 42 | FreeRTOSTaskTest.SchedulerState | ✅ PASS | 0.010s |
| 43 | FreeRTOSTaskTest.TimeSinceSchedulerStarted | ✅ PASS | 0.010s |
| 44 | FreeRTOSTaskTest.CriticalSection | ✅ PASS | 0.010s |
| 45 | FreeRTOSTaskTest.CriticalSectionISR | ✅ PASS | 0.010s |
| 46 | FreeRTOSTaskTest.InterruptBarrier | ✅ PASS | 0.010s |
| 47 | FreeRTOSTaskTest.SchedulerBarrier | ✅ PASS | 0.010s |
| 48 | FreeRTOSTaskTest.StackAllocationLimitation | ✅ PASS | 0.010s |
| 49 | FreeRTOSTaskTest.InvalidParameters | ✅ PASS | 0.010s |
| 50 | FreeRTOSTaskTest.ZeroStackSize | ✅ PASS | 0.010s |
| 51 | FreeRTOSTaskTest.VeryHighPriority | ✅ PASS | 0.010s |
| 52 | FreeRTOSTaskTest.TaskSystemStatus | ✅ PASS | 0.010s |
| 53 | FreeRTOSTaskTest.TaskChronoCompatibility | ✅ PASS | 0.010s |
| 54 | FreeRTOSTaskTest.DelayUntilTimePoint | ✅ PASS | 0.010s |
| 55 | FreeRTOSTaskTest.TaskMoveConstruction | ✅ PASS | 0.010s |
| 56 | FreeRTOSTaskTest.PeriodicTaskMoveConstruction | ✅ PASS | 0.010s |
| 57 | FreeRTOSTaskTest.TaskExecutionDirectCall | ✅ PASS | 0.010s |
| 58 | FreeRTOSTaskTest.PeriodicTaskTypo | ✅ PASS | 0.010s |
| 59 | FreeRTOSTaskTest.TaskTemplateInstantiation | ✅ PASS | 0.010s |
| 60 | FreeRTOSTaskTest.DynamicTaskDifferentSizes | ✅ PASS | 0.010s |
| 61 | FreeRTOSTaskTest.TaskNotificationEdgeCases | ✅ PASS | 0.010s |
| 62 | FreeRTOSTaskTest.TaskSystemStatusTemplate | ✅ PASS | 0.010s |
| 63 | FreeRTOSTaskTest.DelayFunctionsWithDifferentDurations | ✅ PASS | 0.010s |
| 64 | FreeRTOSTaskTest.DelayUntilWithPeriodReference | ✅ PASS | 0.010s |
| 65 | FreeRTOSTaskTest.RacingConditionTaskConstructorInitialization | ✅ PASS | 0.010s |
| 66 | FreeRTOSTaskTest.ConcurrentTaskCreationAndDestruction | ✅ PASS | 0.010s |
| 67 | FreeRTOSTaskTest.MoveSemanticsRacingConditions | ✅ PASS | 0.010s |
| 68 | FreeRTOSTaskTest.PeriodicTaskLifecycleRacingConditions | ✅ PASS | 0.010s |
| 69 | FreeRTOSTaskTest.NotificationRacingConditions | ✅ PASS | 0.010s |
| 70 | FreeRTOSTaskTest.ComplexMultitaskingScenario | ✅ PASS | 0.010s |
| 71 | FreeRTOSTaskTest.TaskSystemStatusUnderLoad | ✅ PASS | 0.010s |
| 72 | FreeRTOSTaskTest.ConstructorInitializationOrderRaceCondition | ✅ PASS | 0.010s |
| 73 | FreeRTOSTaskTest.TaskExecutionInternalFunction | ✅ PASS | 0.010s |
| 74 | FreeRTOSTaskTest.PeriodicTaskRunMethodExecution | ✅ PASS | 0.010s |
| 75 | FreeRTOSTaskTest.YieldFunctionExecution | ✅ PASS | 0.010s |
| 76 | FreeRTOSTaskTest.CriticalSectionAndBarrierUtilities | ✅ PASS | 0.010s |
| 77 | FreeRTOSTaskTest.TaskSystemStatusCoverage | ✅ PASS | 0.010s |
| 78 | FreeRTOSTaskTest.AdvancedRacingConditionScenarios | ✅ PASS | 0.010s |
| 79 | FreeRTOSTaskTest.EdgeCaseErrorHandling | ✅ PASS | 0.010s |
| 80 | FreeRTOSTaskTest.AdvancedChronoCompatibility | ✅ PASS | 0.010s |
| 81 | FreeRTOSTaskTest.PriorityInheritanceScenario | ✅ PASS | 0.010s |

### Semaphore Module Tests

**Module Statistics:**
- Tests: 96
- Passed: 96
- Failed: 0
- Success Rate: 100.0%
- Total Time: 2.040s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 82 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorConstruction | ✅ PASS | 0.010s |
| 83 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateBinary | ✅ PASS | 0.010s |
| 84 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateCounting | ✅ PASS | 0.010s |
| 85 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateMutex | ✅ PASS | 0.010s |
| 86 | FreeRTOSSemaphoreTest.StaticSemaphoreAllocatorCreateRecursiveMutex | ✅ PASS | 0.010s |
| 87 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorConstruction | ✅ PASS | 0.010s |
| 88 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateBinary | ✅ PASS | 0.010s |
| 89 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateCounting | ✅ PASS | 0.010s |
| 90 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateMutex | ✅ PASS | 0.010s |
| 91 | FreeRTOSSemaphoreTest.DynamicSemaphoreAllocatorCreateRecursiveMutex | ✅ PASS | 0.010s |
| 92 | FreeRTOSSemaphoreTest.BinarySemaphoreConstruction | ✅ PASS | 0.010s |
| 93 | FreeRTOSSemaphoreTest.BinarySemaphoreDestruction | ✅ PASS | 0.010s |
| 94 | FreeRTOSSemaphoreTest.BinarySemaphoreDestructionNullHandle | ✅ PASS | 0.010s |
| 95 | FreeRTOSSemaphoreTest.BinarySemaphoreGive | ✅ PASS | 0.010s |
| 96 | FreeRTOSSemaphoreTest.BinarySemaphoreTake | ✅ PASS | 0.010s |
| 97 | FreeRTOSSemaphoreTest.BinarySemaphoreTakeWithTimeout | ✅ PASS | 0.010s |
| 98 | FreeRTOSSemaphoreTest.BinarySemaphoreISROperations | ✅ PASS | 0.010s |
| 99 | FreeRTOSSemaphoreTest.BinarySemaphoreChronoTimeout | ✅ PASS | 0.010s |
| 100 | FreeRTOSSemaphoreTest.CountingSemaphoreConstruction | ✅ PASS | 0.010s |
| 101 | FreeRTOSSemaphoreTest.CountingSemaphoreDefaultConstruction | ✅ PASS | 0.010s |
| 102 | FreeRTOSSemaphoreTest.CountingSemaphoreOperators | ✅ PASS | 0.010s |
| 103 | FreeRTOSSemaphoreTest.CountingSemaphoreGetCount | ✅ PASS | 0.010s |
| 104 | FreeRTOSSemaphoreTest.MutexConstruction | ✅ PASS | 0.010s |
| 105 | FreeRTOSSemaphoreTest.MutexLockUnlock | ✅ PASS | 0.010s |
| 106 | FreeRTOSSemaphoreTest.MutexTryLock | ✅ PASS | 0.010s |
| 107 | FreeRTOSSemaphoreTest.MutexLockWithTimeout | ✅ PASS | 0.010s |
| 108 | FreeRTOSSemaphoreTest.MutexISROperations | ✅ PASS | 0.010s |
| 109 | FreeRTOSSemaphoreTest.RecursiveMutexConstruction | ✅ PASS | 0.010s |
| 110 | FreeRTOSSemaphoreTest.RecursiveMutexLockUnlock | ✅ PASS | 0.010s |
| 111 | FreeRTOSSemaphoreTest.RecursiveMutexTryLock | ✅ PASS | 0.010s |
| 112 | FreeRTOSSemaphoreTest.RecursiveMutexNestedLocks | ✅ PASS | 0.010s |
| 113 | FreeRTOSSemaphoreTest.RecursiveMutexNestedFunction | ✅ PASS | 0.010s |
| 114 | FreeRTOSSemaphoreTest.RecursiveMutexLockGuardRAII | ✅ PASS | 0.010s |
| 115 | FreeRTOSSemaphoreTest.RecursiveMutexNestedLockGuards | ✅ PASS | 0.010s |
| 116 | FreeRTOSSemaphoreTest.RecursiveMutexTryLockGuardRAII | ✅ PASS | 0.010s |
| 117 | FreeRTOSSemaphoreTest.RecursiveMutexTryLockGuardFailure | ✅ PASS | 0.010s |
| 118 | FreeRTOSSemaphoreTest.RecursiveMutexNestedTryLockGuards | ✅ PASS | 0.010s |
| 119 | FreeRTOSSemaphoreTest.RecursiveMutexLockGuardISRRAII | ✅ PASS | 0.010s |
| 120 | FreeRTOSSemaphoreTest.RecursiveMutexNestedLockGuardISR | ✅ PASS | 0.010s |
| 121 | FreeRTOSSemaphoreTest.RecursiveMutexTimeoutLockGuardRAII | ✅ PASS | 0.010s |
| 122 | FreeRTOSSemaphoreTest.RecursiveMutexTimeoutLockGuardChrono | ✅ PASS | 0.010s |
| 123 | FreeRTOSSemaphoreTest.RecursiveMutexNestedTimeoutLockGuards | ✅ PASS | 0.010s |
| 124 | FreeRTOSSemaphoreTest.RecursiveMutexMixedLockGuardTypes | ✅ PASS | 0.010s |
| 125 | FreeRTOSSemaphoreTest.RecursiveMutexTimeoutLockGuardFailure | ✅ PASS | 0.010s |
| 126 | FreeRTOSSemaphoreTest.RecursiveMutexTryLockGuardAcquiredStateTrackingFailure | ✅ PASS | 0.010s |
| 127 | FreeRTOSSemaphoreTest.LockGuardRAII | ✅ PASS | 0.010s |
| 128 | FreeRTOSSemaphoreTest.TryLockGuardRAII | ✅ PASS | 0.010s |
| 129 | FreeRTOSSemaphoreTest.TryLockGuardFailure | ✅ PASS | 0.010s |
| 130 | FreeRTOSSemaphoreTest.LockGuardISRRAII | ✅ PASS | 0.010s |
| 131 | FreeRTOSSemaphoreTest.TimeoutLockGuardRAII | ✅ PASS | 0.010s |
| 132 | FreeRTOSSemaphoreTest.TimeoutLockGuardChronoRAII | ✅ PASS | 0.010s |
| 133 | FreeRTOSSemaphoreTest.TimeoutLockGuardFailure | ✅ PASS | 0.010s |
| 134 | FreeRTOSSemaphoreTest.TryLockGuardAcquiredStateTrackingSuccess | ✅ PASS | 0.010s |
| 135 | FreeRTOSSemaphoreTest.TryLockGuardAcquiredStateTrackingFailure | ✅ PASS | 0.010s |
| 136 | FreeRTOSSemaphoreTest.TimeoutLockGuardAcquiredStateTrackingSuccess | ✅ PASS | 0.010s |
| 137 | FreeRTOSSemaphoreTest.TimeoutLockGuardAcquiredStateTrackingFailure | ✅ PASS | 0.010s |
| 138 | FreeRTOSSemaphoreTest.LockGuardStateTrackingRaceConditionProtection | ✅ PASS | 0.010s |
| 139 | FreeRTOSSemaphoreTest.RecursiveMutexLockGuardAcquiredStateTracking | ✅ PASS | 0.010s |
| 140 | FreeRTOSSemaphoreTest.SemaphoreCreationFailure | ✅ PASS | 0.010s |
| 141 | FreeRTOSSemaphoreTest.StaticAllocationLimitation | ✅ PASS | 0.010s |
| 142 | FreeRTOSSemaphoreTest.MutexLockFailure | ✅ PASS | 0.010s |
| 143 | FreeRTOSSemaphoreTest.LargeCountingSemaphore | ✅ PASS | 0.010s |
| 144 | FreeRTOSSemaphoreTest.ZeroCountingSemaphore | ✅ PASS | 0.010s |
| 145 | FreeRTOSSemaphoreTest.StaticVsDynamicBehavior | ✅ PASS | 0.010s |
| 146 | FreeRTOSSemaphoreTest.StaticAliasNamespace | ✅ PASS | 0.010s |
| 147 | FreeRTOSSemaphoreTest.DynamicAliasNamespace | ✅ PASS | 0.010s |
| 148 | FreeRTOSSemaphoreTest.NamespaceAliasCountingSemaphore | ✅ PASS | 0.010s |
| 149 | FreeRTOSSemaphoreTest.NamespaceAliasMutex | ✅ PASS | 0.010s |
| 150 | FreeRTOSSemaphoreTest.NamespaceAliasRecursiveMutex | ✅ PASS | 0.010s |
| 151 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountBasic | ✅ PASS | 0.010s |
| 152 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountTryLock | ✅ PASS | 0.010s |
| 153 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountISR | ✅ PASS | 0.010s |
| 154 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountFailureScenarios | ✅ PASS | 0.010s |
| 155 | FreeRTOSSemaphoreTest.RecursiveMutexRecursionsCountEdgeCases | ✅ PASS | 0.010s |
| 156 | FreeRTOSSemaphoreTest.CountingSemaphoreMultipleOperators | ✅ PASS | 0.010s |
| 157 | FreeRTOSSemaphoreTest.MutexUnlockWithoutLock | ✅ PASS | 0.010s |
| 158 | FreeRTOSSemaphoreTest.BinarySemaphoreMultipleGive | ✅ PASS | 0.010s |
| 159 | FreeRTOSSemaphoreTest.BinarySemaphoreTakeUntaken | ✅ PASS | 0.010s |
| 160 | FreeRTOSSemaphoreTest.CountingSemaphoreEdgeCounts | ✅ PASS | 0.010s |
| 161 | FreeRTOSSemaphoreTest.ChronoTimeoutCompatibility | ✅ PASS | 0.010s |
| 162 | FreeRTOSSemaphoreTest.SemaphoreAPICompleteness | ✅ PASS | 0.010s |
| 383 | STLSemaphoreTest.STLBinarySemaphoreBasicOperations | ✅ PASS | 0.000s |
| 384 | STLSemaphoreTest.STLBinarySemaphoreTimeout | ✅ PASS | 0.100s |
| 385 | STLSemaphoreTest.STLBinarySemaphoreProducerConsumer | ✅ PASS | 0.050s |
| 386 | STLSemaphoreTest.STLBinarySemaphoreMultipleProducers | ✅ PASS | 0.040s |
| 387 | STLSemaphoreTest.STLCountingSemaphoreBasicOperations | ✅ PASS | 0.000s |
| 388 | STLSemaphoreTest.STLCountingSemaphoreMaxLimit | ✅ PASS | 0.000s |
| 389 | STLSemaphoreTest.STLCountingSemaphoreResourcePool | ✅ PASS | 0.100s |
| 390 | STLSemaphoreTest.STLMutexBasicOperations | ✅ PASS | 0.000s |
| 391 | STLSemaphoreTest.STLMutexCriticalSection | ✅ PASS | 0.030s |
| 392 | STLSemaphoreTest.STLRecursiveMutexBasicOperations | ✅ PASS | 0.000s |
| 393 | STLSemaphoreTest.STLRecursiveMutexNestedLocking | ✅ PASS | 0.000s |
| 394 | STLSemaphoreTest.STLProducerConsumerWithBuffering | ✅ PASS | 0.100s |
| 395 | STLSemaphoreTest.STLReaderWriterPattern | ✅ PASS | 0.380s |
| 396 | STLSemaphoreTest.STLSemaphoreStressTest | ✅ PASS | 0.080s |
| 397 | STLSemaphoreTest.STLTimeoutAccuracy | ✅ PASS | 0.350s |

### Queue Module Tests

**Module Statistics:**
- Tests: 49
- Passed: 49
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.490s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 163 | FreeRTOSQueueTest.StaticQueueAllocatorConstruction | ✅ PASS | 0.010s |
| 164 | FreeRTOSQueueTest.StaticQueueAllocatorCreate | ✅ PASS | 0.010s |
| 165 | FreeRTOSQueueTest.StaticQueueAllocatorDifferentTypes | ✅ PASS | 0.010s |
| 166 | FreeRTOSQueueTest.DynamicQueueAllocatorConstruction | ✅ PASS | 0.010s |
| 167 | FreeRTOSQueueTest.DynamicQueueAllocatorCreate | ✅ PASS | 0.010s |
| 168 | FreeRTOSQueueTest.QueueConstructionWithoutRegistry | ✅ PASS | 0.010s |
| 169 | FreeRTOSQueueTest.QueueConstructionWithRegistry | ✅ PASS | 0.010s |
| 170 | FreeRTOSQueueTest.QueueDestructionNullHandle | ✅ PASS | 0.010s |
| 171 | FreeRTOSQueueTest.QueueSendBasic | ✅ PASS | 0.010s |
| 172 | FreeRTOSQueueTest.QueueSendBack | ✅ PASS | 0.010s |
| 173 | FreeRTOSQueueTest.QueueSendFront | ✅ PASS | 0.010s |
| 174 | FreeRTOSQueueTest.QueueSendTimeout | ✅ PASS | 0.010s |
| 175 | FreeRTOSQueueTest.QueueSendISR | ✅ PASS | 0.010s |
| 176 | FreeRTOSQueueTest.QueueSendChronoTimeout | ✅ PASS | 0.010s |
| 177 | FreeRTOSQueueTest.QueueReceiveReference | ✅ PASS | 0.010s |
| 178 | FreeRTOSQueueTest.QueueReceiveOptional | ✅ PASS | 0.010s |
| 179 | FreeRTOSQueueTest.QueueReceiveOptionalEmpty | ✅ PASS | 0.010s |
| 180 | FreeRTOSQueueTest.QueueReceiveISR | ✅ PASS | 0.010s |
| 181 | FreeRTOSQueueTest.QueueReceiveChronoTimeout | ✅ PASS | 0.010s |
| 182 | FreeRTOSQueueTest.QueuePeekReference | ✅ PASS | 0.010s |
| 183 | FreeRTOSQueueTest.QueuePeekOptional | ✅ PASS | 0.010s |
| 184 | FreeRTOSQueueTest.QueuePeekISR | ✅ PASS | 0.010s |
| 185 | FreeRTOSQueueTest.QueueMessagesWaiting | ✅ PASS | 0.010s |
| 186 | FreeRTOSQueueTest.QueueSpacesAvailable | ✅ PASS | 0.010s |
| 187 | FreeRTOSQueueTest.QueueIsEmpty | ✅ PASS | 0.010s |
| 188 | FreeRTOSQueueTest.QueueIsFull | ✅ PASS | 0.010s |
| 189 | FreeRTOSQueueTest.QueueGetName | ✅ PASS | 0.010s |
| 190 | FreeRTOSQueueTest.QueueReset | ✅ PASS | 0.010s |
| 191 | FreeRTOSQueueTest.QueueOverwrite | ✅ PASS | 0.010s |
| 192 | FreeRTOSQueueTest.QueueOverwriteISR | ✅ PASS | 0.010s |
| 193 | FreeRTOSQueueTest.StaticQueueAliasUsage | ✅ PASS | 0.010s |
| 194 | FreeRTOSQueueTest.DynamicQueueAliasUsage | ✅ PASS | 0.010s |
| 195 | FreeRTOSQueueTest.QueueCreationFailure | ✅ PASS | 0.010s |
| 196 | FreeRTOSQueueTest.LargeQueueSize | ✅ PASS | 0.010s |
| 197 | FreeRTOSQueueTest.ZeroSizeQueue | ✅ PASS | 0.010s |
| 198 | FreeRTOSQueueTest.ComplexDataTypeQueue | ✅ PASS | 0.010s |
| 199 | FreeRTOSQueueTest.QueueAPICompleteness | ✅ PASS | 0.010s |
| 200 | FreeRTOSQueueTest.QueueWithVariousDataTypes | ✅ PASS | 0.010s |
| 201 | FreeRTOSQueueTest.QueueWithEnumTypes | ✅ PASS | 0.010s |
| 202 | FreeRTOSQueueTest.QueueWithPointerTypes | ✅ PASS | 0.010s |
| 203 | FreeRTOSQueueTest.QueueChronoMicrosecondsTimeout | ✅ PASS | 0.010s |
| 204 | FreeRTOSQueueTest.QueueChronoMinutesTimeout | ✅ PASS | 0.010s |
| 205 | FreeRTOSQueueTest.QueueChronoNanosecondsTimeout | ✅ PASS | 0.010s |
| 206 | FreeRTOSQueueTest.QueueChronoOptionalReceiveVariousDurations | ✅ PASS | 0.010s |
| 207 | FreeRTOSQueueTest.StaticQueueAllocatorWithVariousTypes | ✅ PASS | 0.010s |
| 208 | FreeRTOSQueueTest.QueueISROperationsWithDifferentTypes | ✅ PASS | 0.010s |
| 209 | FreeRTOSQueueTest.QueueSendBackISROperations | ✅ PASS | 0.010s |
| 210 | FreeRTOSQueueTest.QueueSendFrontISROperations | ✅ PASS | 0.010s |
| 211 | FreeRTOSQueueTest.QueueMessagesWaitingISR | ✅ PASS | 0.010s |

### EventGroup Module Tests

**Module Statistics:**
- Tests: 30
- Passed: 30
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.300s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 212 | FreeRTOSEventGroupTest.StaticEventGroupAllocatorConstruction | ✅ PASS | 0.010s |
| 213 | FreeRTOSEventGroupTest.StaticEventGroupAllocatorCreate | ✅ PASS | 0.010s |
| 214 | FreeRTOSEventGroupTest.StaticEventGroupAllocatorCreateFailure | ✅ PASS | 0.010s |
| 215 | FreeRTOSEventGroupTest.DynamicEventGroupAllocatorConstruction | ✅ PASS | 0.010s |
| 216 | FreeRTOSEventGroupTest.DynamicEventGroupAllocatorCreate | ✅ PASS | 0.010s |
| 217 | FreeRTOSEventGroupTest.DynamicEventGroupAllocatorCreateFailure | ✅ PASS | 0.010s |
| 218 | FreeRTOSEventGroupTest.EventGroupConstruction | ✅ PASS | 0.010s |
| 219 | FreeRTOSEventGroupTest.EventGroupDestructionNullHandle | ✅ PASS | 0.010s |
| 220 | FreeRTOSEventGroupTest.StaticEventGroupConstruction | ✅ PASS | 0.010s |
| 221 | FreeRTOSEventGroupTest.EventGroupSetBits | ✅ PASS | 0.010s |
| 222 | FreeRTOSEventGroupTest.EventGroupSetBitsISR | ✅ PASS | 0.010s |
| 223 | FreeRTOSEventGroupTest.EventGroupClearBits | ✅ PASS | 0.010s |
| 224 | FreeRTOSEventGroupTest.EventGroupGetBits | ✅ PASS | 0.010s |
| 225 | FreeRTOSEventGroupTest.EventGroupGetBitsISR | ✅ PASS | 0.010s |
| 226 | FreeRTOSEventGroupTest.EventGroupWaitBitsAnyBits | ✅ PASS | 0.010s |
| 227 | FreeRTOSEventGroupTest.EventGroupWaitBitsAllBits | ✅ PASS | 0.010s |
| 228 | FreeRTOSEventGroupTest.EventGroupWaitBitsTimeout | ✅ PASS | 0.010s |
| 229 | FreeRTOSEventGroupTest.EventGroupWaitBitsChronoTimeout | ✅ PASS | 0.010s |
| 230 | FreeRTOSEventGroupTest.EventGroupSync | ✅ PASS | 0.010s |
| 231 | FreeRTOSEventGroupTest.EventGroupSyncChronoTimeout | ✅ PASS | 0.010s |
| 232 | FreeRTOSEventGroupTest.EventGroupSyncTimeout | ✅ PASS | 0.010s |
| 233 | FreeRTOSEventGroupTest.EventGroupChronoMicrosecondsTimeout | ✅ PASS | 0.010s |
| 234 | FreeRTOSEventGroupTest.EventGroupChronoNanosecondsTimeout | ✅ PASS | 0.010s |
| 235 | FreeRTOSEventGroupTest.EventGroupChronoMinutesTimeout | ✅ PASS | 0.010s |
| 236 | FreeRTOSEventGroupTest.EventGroupComplexScenario | ✅ PASS | 0.010s |
| 237 | FreeRTOSEventGroupTest.EventGroupCreationFailure | ✅ PASS | 0.010s |
| 238 | FreeRTOSEventGroupTest.EventGroupAllBitsPattern | ✅ PASS | 0.010s |
| 239 | FreeRTOSEventGroupTest.EventGroupZeroBitsPattern | ✅ PASS | 0.010s |
| 240 | FreeRTOSEventGroupTest.StaticVsDynamicBehavior | ✅ PASS | 0.010s |
| 241 | FreeRTOSEventGroupTest.EventGroupAPICompleteness | ✅ PASS | 0.010s |

### StreamBuffer Module Tests

**Module Statistics:**
- Tests: 47
- Passed: 47
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.470s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 242 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorConstruction | ✅ PASS | 0.010s |
| 243 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorCreate | ✅ PASS | 0.010s |
| 244 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorCreateWithTriggerLevel | ✅ PASS | 0.010s |
| 245 | FreeRTOSStreamBufferTest.StaticStreamBufferAllocatorCreateFailure | ✅ PASS | 0.010s |
| 246 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorConstruction | ✅ PASS | 0.010s |
| 247 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorCreate | ✅ PASS | 0.010s |
| 248 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorCreateWithTriggerLevel | ✅ PASS | 0.010s |
| 249 | FreeRTOSStreamBufferTest.DynamicStreamBufferAllocatorCreateFailure | ✅ PASS | 0.010s |
| 250 | FreeRTOSStreamBufferTest.StaticStreamBufferConstruction | ✅ PASS | 0.010s |
| 251 | FreeRTOSStreamBufferTest.DynamicStreamBufferConstruction | ✅ PASS | 0.010s |
| 252 | FreeRTOSStreamBufferTest.StreamBufferConstructionWithTriggerLevel | ✅ PASS | 0.010s |
| 253 | FreeRTOSStreamBufferTest.StreamBufferDestructionNullHandle | ✅ PASS | 0.010s |
| 254 | FreeRTOSStreamBufferTest.StreamBufferSendBasic | ✅ PASS | 0.010s |
| 255 | FreeRTOSStreamBufferTest.StreamBufferSendWithTimeout | ✅ PASS | 0.010s |
| 256 | FreeRTOSStreamBufferTest.StreamBufferSendChronoTimeout | ✅ PASS | 0.010s |
| 257 | FreeRTOSStreamBufferTest.StreamBufferSendIterators | ✅ PASS | 0.010s |
| 258 | FreeRTOSStreamBufferTest.StreamBufferSendIteratorsWithTimeout | ✅ PASS | 0.010s |
| 259 | FreeRTOSStreamBufferTest.StreamBufferSendPartial | ✅ PASS | 0.010s |
| 260 | FreeRTOSStreamBufferTest.StreamBufferSendISR | ✅ PASS | 0.010s |
| 261 | FreeRTOSStreamBufferTest.StreamBufferSendISRWithoutWoken | ✅ PASS | 0.010s |
| 262 | FreeRTOSStreamBufferTest.StreamBufferSendISRIterators | ✅ PASS | 0.010s |
| 263 | FreeRTOSStreamBufferTest.StreamBufferReceiveBasic | ✅ PASS | 0.010s |
| 264 | FreeRTOSStreamBufferTest.StreamBufferReceiveWithTimeout | ✅ PASS | 0.010s |
| 265 | FreeRTOSStreamBufferTest.StreamBufferReceiveChronoTimeout | ✅ PASS | 0.010s |
| 266 | FreeRTOSStreamBufferTest.StreamBufferReceiveTimeout | ✅ PASS | 0.010s |
| 267 | FreeRTOSStreamBufferTest.StreamBufferReceiveISR | ✅ PASS | 0.010s |
| 268 | FreeRTOSStreamBufferTest.StreamBufferReceiveISRWithoutWoken | ✅ PASS | 0.010s |
| 269 | FreeRTOSStreamBufferTest.StreamBufferBytesAvailable | ✅ PASS | 0.010s |
| 270 | FreeRTOSStreamBufferTest.StreamBufferSpacesAvailable | ✅ PASS | 0.010s |
| 271 | FreeRTOSStreamBufferTest.StreamBufferIsEmpty | ✅ PASS | 0.010s |
| 272 | FreeRTOSStreamBufferTest.StreamBufferIsNotEmpty | ✅ PASS | 0.010s |
| 273 | FreeRTOSStreamBufferTest.StreamBufferIsFull | ✅ PASS | 0.010s |
| 274 | FreeRTOSStreamBufferTest.StreamBufferIsNotFull | ✅ PASS | 0.010s |
| 275 | FreeRTOSStreamBufferTest.StreamBufferReset | ✅ PASS | 0.010s |
| 276 | FreeRTOSStreamBufferTest.StreamBufferResetFailure | ✅ PASS | 0.010s |
| 277 | FreeRTOSStreamBufferTest.StreamBufferSetTriggerLevel | ✅ PASS | 0.010s |
| 278 | FreeRTOSStreamBufferTest.StreamBufferSetTriggerLevelFailure | ✅ PASS | 0.010s |
| 279 | FreeRTOSStreamBufferTest.StreamBufferGetHandle | ✅ PASS | 0.010s |
| 280 | FreeRTOSStreamBufferTest.StreamBufferCreationFailure | ✅ PASS | 0.010s |
| 281 | FreeRTOSStreamBufferTest.StreamBufferSendZeroBytes | ✅ PASS | 0.010s |
| 282 | FreeRTOSStreamBufferTest.StreamBufferReceiveZeroBytes | ✅ PASS | 0.010s |
| 283 | FreeRTOSStreamBufferTest.StreamBufferComplexScenario | ✅ PASS | 0.010s |
| 284 | FreeRTOSStreamBufferTest.StaticVsDynamicBehavior | ✅ PASS | 0.010s |
| 285 | FreeRTOSStreamBufferTest.StreamBufferChronoMicrosecondsTimeout | ✅ PASS | 0.010s |
| 286 | FreeRTOSStreamBufferTest.StreamBufferChronoNanosecondsTimeout | ✅ PASS | 0.010s |
| 287 | FreeRTOSStreamBufferTest.StreamBufferChronoSecondsTimeout | ✅ PASS | 0.010s |
| 288 | FreeRTOSStreamBufferTest.StreamBufferAPICompleteness | ✅ PASS | 0.010s |

### MessageBuffer Module Tests

**Module Statistics:**
- Tests: 32
- Passed: 32
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.320s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 289 | FreeRTOSMessageBufferTest.StaticMessageBufferAllocatorConstruction | ✅ PASS | 0.010s |
| 290 | FreeRTOSMessageBufferTest.StaticMessageBufferAllocatorCreate | ✅ PASS | 0.010s |
| 291 | FreeRTOSMessageBufferTest.StaticMessageBufferAllocatorCreateFailure | ✅ PASS | 0.010s |
| 292 | FreeRTOSMessageBufferTest.DynamicMessageBufferAllocatorConstruction | ✅ PASS | 0.010s |
| 293 | FreeRTOSMessageBufferTest.DynamicMessageBufferAllocatorCreateSuccess | ✅ PASS | 0.010s |
| 294 | FreeRTOSMessageBufferTest.DynamicMessageBufferAllocatorCreateFailure | ✅ PASS | 0.010s |
| 295 | FreeRTOSMessageBufferTest.StaticMessageBufferConstruction | ✅ PASS | 0.010s |
| 296 | FreeRTOSMessageBufferTest.StaticMessageBufferDestruction | ✅ PASS | 0.010s |
| 297 | FreeRTOSMessageBufferTest.StaticMessageBufferDestructionNullHandle | ✅ PASS | 0.010s |
| 298 | FreeRTOSMessageBufferTest.DynamicMessageBufferConstruction | ✅ PASS | 0.010s |
| 299 | FreeRTOSMessageBufferTest.DynamicMessageBufferConstructionFailure | ✅ PASS | 0.010s |
| 300 | FreeRTOSMessageBufferTest.MessageBufferSendSuccess | ✅ PASS | 0.010s |
| 301 | FreeRTOSMessageBufferTest.MessageBufferSendTimeout | ✅ PASS | 0.010s |
| 302 | FreeRTOSMessageBufferTest.MessageBufferSendWithChrono | ✅ PASS | 0.010s |
| 303 | FreeRTOSMessageBufferTest.MessageBufferSendZeroLengthMessage | ✅ PASS | 0.010s |
| 304 | FreeRTOSMessageBufferTest.MessageBufferSendMaxSizeMessage | ✅ PASS | 0.010s |
| 305 | FreeRTOSMessageBufferTest.MessageBufferReceiveSuccess | ✅ PASS | 0.010s |
| 306 | FreeRTOSMessageBufferTest.MessageBufferReceiveTimeout | ✅ PASS | 0.010s |
| 307 | FreeRTOSMessageBufferTest.MessageBufferReceiveWithChrono | ✅ PASS | 0.010s |
| 308 | FreeRTOSMessageBufferTest.MessageBufferReceiveBufferTooSmall | ✅ PASS | 0.010s |
| 309 | FreeRTOSMessageBufferTest.MessageBufferAvailableSpace | ✅ PASS | 0.010s |
| 310 | FreeRTOSMessageBufferTest.MessageBufferIsEmpty | ✅ PASS | 0.010s |
| 311 | FreeRTOSMessageBufferTest.MessageBufferIsFull | ✅ PASS | 0.010s |
| 312 | FreeRTOSMessageBufferTest.MessageBufferReset | ✅ PASS | 0.010s |
| 313 | FreeRTOSMessageBufferTest.MessageBufferCreationFailure | ✅ PASS | 0.010s |
| 314 | FreeRTOSMessageBufferTest.MessageBufferZeroSizeBuffer | ✅ PASS | 0.010s |
| 315 | FreeRTOSMessageBufferTest.MessageBufferVeryLargeBuffer | ✅ PASS | 0.010s |
| 316 | FreeRTOSMessageBufferTest.MessageBufferChronoMicrosecondsTimeout | ✅ PASS | 0.010s |
| 317 | FreeRTOSMessageBufferTest.MessageBufferChronoSecondsTimeout | ✅ PASS | 0.010s |
| 318 | FreeRTOSMessageBufferTest.StaticVsDynamicBehaviorComparison | ✅ PASS | 0.010s |
| 319 | FreeRTOSMessageBufferTest.MessageBufferComplexSendReceiveScenario | ✅ PASS | 0.010s |
| 320 | FreeRTOSMessageBufferTest.MessageBufferAPICompleteness | ✅ PASS | 0.010s |

### Timer Module Tests

**Module Statistics:**
- Tests: 50
- Passed: 50
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.500s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 321 | FreeRTOSSwTimerTest.StaticTimerAllocatorConstruction | ✅ PASS | 0.010s |
| 322 | FreeRTOSSwTimerTest.StaticTimerAllocatorCreate | ✅ PASS | 0.010s |
| 323 | FreeRTOSSwTimerTest.StaticTimerAllocatorCreateNullReturn | ✅ PASS | 0.010s |
| 324 | FreeRTOSSwTimerTest.DynamicTimerAllocatorConstruction | ✅ PASS | 0.010s |
| 325 | FreeRTOSSwTimerTest.DynamicTimerAllocatorCreate | ✅ PASS | 0.010s |
| 326 | FreeRTOSSwTimerTest.DynamicTimerAllocatorCreateNullReturn | ✅ PASS | 0.010s |
| 327 | FreeRTOSSwTimerTest.StaticTimerConstruction | ✅ PASS | 0.010s |
| 328 | FreeRTOSSwTimerTest.StaticTimerConstructionWithChrono | ✅ PASS | 0.010s |
| 329 | FreeRTOSSwTimerTest.StaticTimerDestruction | ✅ PASS | 0.010s |
| 330 | FreeRTOSSwTimerTest.StaticTimerDestructionNullHandle | ✅ PASS | 0.010s |
| 331 | FreeRTOSSwTimerTest.StaticTimerStart | ✅ PASS | 0.010s |
| 332 | FreeRTOSSwTimerTest.StaticTimerStartWithTimeout | ✅ PASS | 0.010s |
| 333 | FreeRTOSSwTimerTest.StaticTimerStartWithChrono | ✅ PASS | 0.010s |
| 334 | FreeRTOSSwTimerTest.StaticTimerStop | ✅ PASS | 0.010s |
| 335 | FreeRTOSSwTimerTest.StaticTimerReset | ✅ PASS | 0.010s |
| 336 | FreeRTOSSwTimerTest.StaticTimerPeriodChange | ✅ PASS | 0.010s |
| 337 | FreeRTOSSwTimerTest.StaticTimerIsRunning | ✅ PASS | 0.010s |
| 338 | FreeRTOSSwTimerTest.StaticTimerGetPeriod | ✅ PASS | 0.010s |
| 339 | FreeRTOSSwTimerTest.StaticTimerGetName | ✅ PASS | 0.010s |
| 340 | FreeRTOSSwTimerTest.StaticTimerReloadMode | ✅ PASS | 0.010s |
| 341 | FreeRTOSSwTimerTest.DynamicTimerConstruction | ✅ PASS | 0.010s |
| 342 | FreeRTOSSwTimerTest.DynamicTimerConstructionFailure | ✅ PASS | 0.010s |
| 343 | FreeRTOSSwTimerTest.StaticTimerStartFromISR | ✅ PASS | 0.010s |
| 344 | FreeRTOSSwTimerTest.StaticTimerStartFromISRNoParam | ✅ PASS | 0.010s |
| 345 | FreeRTOSSwTimerTest.StaticTimerStopFromISR | ✅ PASS | 0.010s |
| 346 | FreeRTOSSwTimerTest.StaticTimerResetFromISR | ✅ PASS | 0.010s |
| 347 | FreeRTOSSwTimerTest.StaticTimerPeriodFromISR | ✅ PASS | 0.010s |
| 348 | FreeRTOSSwTimerTest.StaticTimerRemainingTime | ✅ PASS | 0.010s |
| 349 | FreeRTOSSwTimerTest.StaticTimerRemainingTimeNullHandle | ✅ PASS | 0.010s |
| 350 | FreeRTOSSwTimerTest.TimerOperationsWithNullHandle | ✅ PASS | 0.010s |
| 351 | FreeRTOSSwTimerTest.TimerFailureConditions | ✅ PASS | 0.010s |
| 352 | FreeRTOSSwTimerTest.TimerMoveConstructionIssueScenario | ✅ PASS | 0.010s |
| 353 | FreeRTOSSwTimerTest.TimerMoveConstruction | ✅ PASS | 0.010s |
| 354 | FreeRTOSSwTimerTest.MovedFromTimerIsInvalidated | ✅ PASS | 0.010s |
| 355 | FreeRTOSSwTimerTest.TimerMoveAssignment | ✅ PASS | 0.010s |
| 356 | FreeRTOSSwTimerTest.ChronoCompatibility | ✅ PASS | 0.010s |
| 357 | FreeRTOSSwTimerTest.PeriodChangeWithMixedDurationTypes | ✅ PASS | 0.010s |
| 358 | FreeRTOSSwTimerTest.PeriodChangeWithVariousDurationCombinations | ✅ PASS | 0.010s |
| 359 | FreeRTOSSwTimerTest.PeriodChangeWithMicrosecondPrecision | ✅ PASS | 0.010s |
| 360 | FreeRTOSSwTimerTest.PeriodChangeWithMinutePrecision | ✅ PASS | 0.010s |
| 361 | FreeRTOSSwTimerTest.PeriodChangeWithFloatingPointDurations | ✅ PASS | 0.010s |
| 362 | FreeRTOSSwTimerTest.PeriodChangeEdgeCaseDurations | ✅ PASS | 0.010s |
| 363 | FreeRTOSSwTimerTest.ConstructorWithVariousDurationTypes | ✅ PASS | 0.010s |
| 364 | FreeRTOSSwTimerTest.StartStopResetWithMixedDurationTypes | ✅ PASS | 0.010s |
| 365 | FreeRTOSSwTimerTest.ISRFunctionsWithMixedDurationTypes | ✅ PASS | 0.010s |
| 366 | FreeRTOSSwTimerTest.SingleShotTimerBehavior | ✅ PASS | 0.010s |
| 367 | FreeRTOSSwTimerTest.AutoReloadTimerBehavior | ✅ PASS | 0.010s |
| 368 | FreeRTOSSwTimerTest.CallbackFunctionality | ✅ PASS | 0.010s |
| 369 | FreeRTOSSwTimerTest.DestructorComplexScenarios | ✅ PASS | 0.010s |
| 370 | FreeRTOSSwTimerTest.DestructorWithActiveTimer | ✅ PASS | 0.010s |

### Enhanced Module Tests

**Module Statistics:**
- Tests: 12
- Passed: 12
- Failed: 0
- Success Rate: 100.0%
- Total Time: 0.680s

**Detailed Test Results:**

| Test ID | Test Name | Outcome | Execution Time |
|---------|-----------|---------|----------------|
| 371 | EnhancedMultitaskingTest.TaskActuallyExecutes | ✅ PASS | 0.060s |
| 372 | EnhancedMultitaskingTest.TaskExecutionWithSuspendResume | ✅ PASS | 0.070s |
| 373 | EnhancedMultitaskingTest.MultipleTasksConcurrentExecution | ✅ PASS | 0.110s |
| 374 | EnhancedMultitaskingTest.TaskSynchronizationWithNotifications | ✅ PASS | 0.060s |
| 375 | EnhancedMultitaskingTest.TaskLifecycleRacingConditions | ✅ PASS | 0.040s |
| 376 | EnhancedMultitaskingTest.TaskMoveSemanticsConcurrency | ✅ PASS | 0.040s |
| 377 | EnhancedMultitaskingTest.PeriodicTaskExecution | ✅ PASS | 0.050s |
| 378 | EnhancedMultitaskingTest.MultiplePeriodicTasksCoordination | ✅ PASS | 0.070s |
| 379 | EnhancedMultitaskingTest.TaskExceptionHandling | ✅ PASS | 0.030s |
| 380 | EnhancedMultitaskingTest.TaskDeleteDuringExecution | ✅ PASS | 0.110s |
| 381 | EnhancedMultitaskingTest.TaskExecFunctionCoverage | ✅ PASS | 0.020s |
| 382 | EnhancedMultitaskingTest.SuspendedTaskStartupCoverage | ✅ PASS | 0.020s |

## Code Coverage Analysis

### Coverage Overview
The project achieves excellent code coverage with **96.6% line coverage** and **94.9% function coverage**.

### Coverage Breakdown
- **Lines Covered**: 6836 out of 7076 total lines
- **Functions Covered**: 2777 out of 2925 total functions
- **Coverage Target**: Main library modules only (excludes test infrastructure and system headers)


**Uncovered Areas Analysis:**

The uncovered code primarily consists of:
1. **Internal task execution functions** - Called by FreeRTOS kernel, not directly testable in unit tests
2. **Error handling paths** - Some error conditions that require specific FreeRTOS kernel states
3. **Platform-specific code** - Code paths that depend on specific hardware configurations
4. **Edge case scenarios** - Rare conditions that are difficult to reproduce in unit test environment

These uncovered areas are intentional and represent code that:
- Requires integration testing with actual FreeRTOS kernel
- Involves kernel-level functionality not suitable for unit testing
- Represents defensive programming patterns for robustness


### Coverage Quality Assessment
The high coverage percentage indicates:
- **Comprehensive test coverage** across all FreeRTOS wrapper modules
- **Good API validation** with both positive and negative test cases
- **Robust error handling verification** where applicable in unit test context
- **Thorough edge case testing** for boundary conditions

## Test Quality Metrics

### Test Distribution by Category
- **Task Module**: 81 tests (19.2%)
- **Semaphore Module**: 96 tests (22.8%)
- **Queue Module**: 49 tests (11.6%)
- **EventGroup Module**: 30 tests (7.1%)
- **StreamBuffer Module**: 47 tests (11.2%)
- **MessageBuffer Module**: 32 tests (7.6%)
- **Timer Module**: 50 tests (11.9%)
- **Enhanced Module**: 12 tests (2.9%)


### Performance Characteristics
- **Fastest Test**: 0.000 seconds
- **Slowest Test**: 0.380 seconds
- **Performance Distribution**:
  - Very Fast (< 0.01s): 6 tests
  - Fast (0.01-0.05s): 401 tests
  - Normal (0.05-0.1s): 7 tests
  - Slow (> 0.1s): 7 tests

## Validation Conclusions

### ✅ Test Suite Maturity
- **Comprehensive Coverage**: 421 test cases across all FreeRTOS wrapper modules
- **Perfect Success Rate**: All tests passing indicates stable and robust implementation
- **Good Module Distribution**: Balanced testing across tasks, synchronization primitives, and communication mechanisms

### ✅ Code Quality Validation
- **High Coverage**: 96.6% line coverage demonstrates thorough validation
- **API Completeness**: 94.9% function coverage shows comprehensive API testing
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

---

*Report Generated*: July 23, 2025 at 02:13:19  
*Test Framework*: GoogleTest/GoogleMock  
*Coverage Tool*: LCOV/GCOV  
*Total Test Execution Time*: 5.00 seconds  
*Validation Status*: ✅ **All tests passing - System validated for production use**
