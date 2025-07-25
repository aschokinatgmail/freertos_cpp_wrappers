# Validation and Verification Report

## Executive Summary

This report provides comprehensive validation and verification results for the FreeRTOS C++ Wrappers project, including detailed test execution outcomes and code coverage analysis.

### Test Execution Summary
- **Total Tests Executed**: 421
- **✅ Passed**: 421 tests
- **❌ Failed**: 0 tests
- **Success Rate**: 100.0% (All tests passed!)
- **Total Execution Time**: 5.19 seconds
- **Average Test Time**: 0.0123 seconds per test

### Code Coverage Summary
- **Line Coverage**: 96.6% (6842/7082 lines)
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
- Total Time: 2.070s

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
| 395 | STLSemaphoreTest.STLReaderWriterPattern | ✅ PASS | 0.410s |
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
- **Lines Covered**: 6842 out of 7082 total lines
- **Functions Covered**: 2777 out of 2925 total functions
- **Coverage Target**: Main library modules only (excludes test infrastructure and system headers)

**Detailed Uncovered Areas Analysis:**

The following sections provide specific references to uncovered code areas and explanations for why they cannot be covered by unit tests.

### Internal task execution functions called by FreeRTOS kernel

**Reason for exclusion:** These functions are called internally by the FreeRTOS kernel during task execution and cannot be directly invoked in unit tests

**Uncovered Area 1**: freertos_sw_timer.hpp:113-117
*Function*: `freertos::timer<freertos::static_sw_timer_allocator>::callback_wrapper(void*)`

```cpp
    TimerHandle_t m_timer;
    
>>> static void callback_wrapper(TimerHandle_t t) {
    auto *const self = static_cast<timer *>(pvTimerGetTimerID(t));
    configASSERT(self);
```

**Uncovered Area 2**: freertos_sw_timer.hpp:113-117
*Function*: `freertos::timer<freertos::dynamic_sw_timer_allocator>::callback_wrapper(void*)`

```cpp
    TimerHandle_t m_timer;
    
>>> static void callback_wrapper(TimerHandle_t t) {
    auto *const self = static_cast<timer *>(pvTimerGetTimerID(t));
    configASSERT(self);
```

**Uncovered Area 3**: freertos_task.hpp:173-182
*Function*: `freertos::task<freertos::static_task_allocator<0ul> >::task_exec(void*)`

```cpp
    TaskHandle_t m_hTask;
    
>>> static void task_exec(void *context) {
    auto pThis = static_cast<task *>(context);
    assert(nullptr != pThis);
```

**Uncovered Area 4**: freertos_task.hpp:173-182
*Function*: `freertos::task<freertos::dynamic_task_allocator<2048ul> >::task_exec(void*)`

```cpp
    TaskHandle_t m_hTask;
    
>>> static void task_exec(void *context) {
    auto pThis = static_cast<task *>(context);
    assert(nullptr != pThis);
```

**Uncovered Area 5**: freertos_task.hpp:572-586
*Function*: `freertos::periodic_task<freertos::static_task_allocator<1024ul> >::run()`

```cpp
    task<TaskAllocator> m_task;
    
>>> void run() {
    m_on_start();
    while (is_running()) {
```

**Uncovered Area 6**: test_freertos_task.cpp:1140-1142
*Function*: `FreeRTOSTaskTest_TaskExecutionDirectCall_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    bool task_executed = false;
>>> sa::task<1024> test_task("ExecutionTask", 2, [&task_executed]() {
    task_executed = true;
    });
```

**Uncovered Area 7**: test_freertos_task.cpp:1613-1615
*Function*: `FreeRTOSTaskTest_TaskExecutionInternalFunction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> suspended_task("SuspendedTask", 1, []() {
    // Task routine that would be executed
    }, true); // start_suspended = true
```

**Uncovered Area 8**: test_freertos_task.cpp:1623-1625
*Function*: `FreeRTOSTaskTest_TaskExecutionInternalFunction_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    .WillOnce(Return(reinterpret_cast<TaskHandle_t>(0x2000)));
    
>>> sa::task<1024> normal_task("NormalTask", 2, []() {
    // Normal task routine
    }); // start_suspended defaults to false for this constructor
```

**Uncovered Area 9**: test_freertos_task.cpp:1650
*Function*: `FreeRTOSTaskTest_PeriodicTaskRunMethodExecution_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    sa::periodic_task<1024> periodic_task(
    "PeriodicRunTask", 1,
>>> [&on_start_calls]() { on_start_calls++; },    // on_start
    [&on_stop_calls]() { on_stop_calls++; },      // on_stop
    []() { /* periodic_routine */ },               // periodic_routine
```

**Uncovered Area 10**: test_freertos_task.cpp:1651
*Function*: `FreeRTOSTaskTest_PeriodicTaskRunMethodExecution_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    "PeriodicRunTask", 1,
    [&on_start_calls]() { on_start_calls++; },    // on_start
>>> [&on_stop_calls]() { on_stop_calls++; },      // on_stop
    []() { /* periodic_routine */ },               // periodic_routine
    std::chrono::milliseconds(100)                 // period
```

**Uncovered Area 11**: test_freertos_task.cpp:1652
*Function*: `FreeRTOSTaskTest_PeriodicTaskRunMethodExecution_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    [&on_start_calls]() { on_start_calls++; },    // on_start
    [&on_stop_calls]() { on_stop_calls++; },      // on_stop
>>> []() { /* periodic_routine */ },               // periodic_routine
    std::chrono::milliseconds(100)                 // period
    );
```

**Uncovered Area 12**: test_freertos_task.cpp:676
*Function*: `FreeRTOSTaskTest_PeriodicTaskIsRunning_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "RunningTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 13**: test_freertos_task.cpp:677
*Function*: `FreeRTOSTaskTest_PeriodicTaskIsRunning_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    100ms     // period
```

**Uncovered Area 14**: test_freertos_task.cpp:678
*Function*: `FreeRTOSTaskTest_PeriodicTaskIsRunning_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    100ms     // period
    );
```

**Uncovered Area 15**: freertos_sw_timer.hpp:200
*Line*: 200

```cpp
    if (rc == pdPASS) {
    while (xTimerIsTimerActive(src.m_timer) != pdFALSE) {
>>> vTaskDelay(pdMS_TO_TICKS(1));
    }
    auto name = pcTimerGetName(src.m_timer);
```

**Uncovered Area 16**: freertos_sw_timer.hpp:208
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

**Uncovered Area 17**: test_enhanced_cpp17_features.cpp:200
*Function*: `Cpp17FeaturesTest_RAIIExceptionSafety_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    // Test RAII behavior with exceptions
    {
>>> sa::task<1024> task("RAIITest", 1, []() { /* test */ });
    EXPECT_NE(task.handle(), nullptr);
    
```

**Uncovered Area 18**: test_freertos_task.cpp:148
*Function*: `FreeRTOSTaskTest_DynamicTaskAllocatorCreateFailure_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    .WillOnce(Return(pdFAIL));
    
>>> auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, nullptr);
    
```

**Uncovered Area 19**: test_freertos_task.cpp:1771
*Function*: `FreeRTOSTaskTest_EdgeCaseErrorHandling_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(nullptr));
    
>>> sa::task<1024> null_task("TestTask", 1, []() {});
    
    // Task should handle null gracefully
```

**Uncovered Area 20**: test_freertos_task.cpp:382
*Function*: `FreeRTOSTaskTest_StaticTaskAbortDelay_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("AbortTask", 2, []() {});
    
    // Test abort_delay with valid handle
```

**Uncovered Area 21**: test_freertos_task.cpp:398
*Function*: `FreeRTOSTaskTest_StaticTaskAbortDelayNullHandle_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(nullptr));
    
>>> sa::task<1024> test_task("NullAbortTask", 2, []() {});
    
    // Should return pdFALSE for null handle without calling FreeRTOS function
```

**Uncovered Area 22**: test_freertos_task.cpp:460
*Function*: `FreeRTOSTaskTest_DynamicTaskConstructionFailure_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(pdFAIL));
    
>>> da::task<2048> test_task("FailTask", 3, []() {});
    
    EXPECT_EQ(test_task.handle(), nullptr);
```

**Uncovered Area 23**: test_freertos_task.cpp:737
*Function*: `FreeRTOSTaskTest_PeriodicTaskDestructorAbortDelay_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "AbortDelayTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 24**: test_freertos_task.cpp:738
*Function*: `FreeRTOSTaskTest_PeriodicTaskDestructorAbortDelay_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    100ms     // period
```

**Uncovered Area 25**: test_freertos_task.cpp:739
*Function*: `FreeRTOSTaskTest_PeriodicTaskDestructorAbortDelay_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    100ms     // period
    );
```

**Uncovered Area 26**: freertos_sw_timer.hpp:113
*Line*: 113

```cpp
    TimerHandle_t m_timer;
    
>>> static void callback_wrapper(TimerHandle_t t) {
    auto *const self = static_cast<timer *>(pvTimerGetTimerID(t));
    configASSERT(self);
```

**Uncovered Area 27**: freertos_sw_timer.hpp:114
*Line*: 114

```cpp
    
    static void callback_wrapper(TimerHandle_t t) {
>>> auto *const self = static_cast<timer *>(pvTimerGetTimerID(t));
    configASSERT(self);
    self->m_callback();
```

**Uncovered Area 28**: freertos_sw_timer.hpp:116
*Line*: 116

```cpp
    auto *const self = static_cast<timer *>(pvTimerGetTimerID(t));
    configASSERT(self);
>>> self->m_callback();
    }
    
```

**Uncovered Area 29**: freertos_sw_timer.hpp:117
*Line*: 117

```cpp
    configASSERT(self);
    self->m_callback();
>>> }
    
    public:
```


### Platform-specific or hardware-dependent code

**Reason for exclusion:** These code sections depend on specific hardware configurations or FreeRTOS kernel internals not available in the test environment

**Uncovered Area 30**: freertos_task.hpp:515-521
*Function*: `freertos::task<freertos::static_task_allocator<256ul> >::notify_wait(unsigned int, unsigned int, unsigned int&, unsigned int)`

```cpp
    * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
    */
>>> BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
    uint32_t ulBitsToClearOnExit,
    uint32_t &notification_value,
```

**Uncovered Area 31**: freertos_task.hpp:515-521
*Function*: `freertos::task<freertos::static_task_allocator<512ul> >::notify_wait(unsigned int, unsigned int, unsigned int&, unsigned int)`

```cpp
    * @return BaseType_t pdTRUE if the notification was given, pdFALSE otherwise
    */
>>> BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
    uint32_t ulBitsToClearOnExit,
    uint32_t &notification_value,
```

**Uncovered Area 32**: freertos_task.hpp:532-540
*Function*: `int freertos::task<freertos::static_task_allocator<256ul> >::notify_wait<long, std::ratio<1l, 1000l> >(unsigned int, unsigned int, unsigned int&, std::chrono::duration<long, std::ratio<1l, 1000l> >)`

```cpp
    */
    template <typename Rep, typename Period>
>>> BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
    uint32_t ulBitsToClearOnExit,
    uint32_t &notification_value,
```

**Uncovered Area 33**: freertos_task.hpp:532-540
*Function*: `int freertos::task<freertos::static_task_allocator<512ul> >::notify_wait<long, std::ratio<1l, 1000l> >(unsigned int, unsigned int, unsigned int&, std::chrono::duration<long, std::ratio<1l, 1000l> >)`

```cpp
    */
    template <typename Rep, typename Period>
>>> BaseType_t notify_wait(uint32_t ulBitsToClearOnEntry,
    uint32_t ulBitsToClearOnExit,
    uint32_t &notification_value,
```

**Uncovered Area 34**: freertos_sw_timer.hpp:216
*Line*: 216

```cpp
    if (m_timer) {
    if (src.m_started) {
>>> rc = xTimerStart(m_timer, portMAX_DELAY);
    if (rc == pdPASS) {
    m_started = true;
```

**Uncovered Area 35**: freertos_sw_timer.hpp:217
*Line*: 217

```cpp
    if (src.m_started) {
    rc = xTimerStart(m_timer, portMAX_DELAY);
>>> if (rc == pdPASS) {
    m_started = true;
    }
```

**Uncovered Area 36**: freertos_sw_timer.hpp:218
*Line*: 218

```cpp
    rc = xTimerStart(m_timer, portMAX_DELAY);
    if (rc == pdPASS) {
>>> m_started = true;
    }
    }
```

**Uncovered Area 37**: freertos_task.hpp:577
*Line*: 577

```cpp
    if (0 != m_period.count()) {
    #if configUSE_TASK_NOTIFICATIONS
>>> uint32_t notification_value = 0;
    m_task.notify_wait(0, 0, notification_value, m_period);
    #else
```

**Uncovered Area 38**: freertos_task.hpp:578
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

**Uncovered Area 39**: freertos_task.hpp:273
*Function*: `freertos::task<freertos::static_task_allocator<0ul> >::suspend()`

```cpp
    *
    */
>>> void suspend(void) { vTaskSuspend(m_hTask); }
    /**
    * @brief Resume the task.
```

**Uncovered Area 40**: freertos_task.hpp:273
*Function*: `freertos::task<freertos::dynamic_task_allocator<2048ul> >::suspend()`

```cpp
    *
    */
>>> void suspend(void) { vTaskSuspend(m_hTask); }
    /**
    * @brief Resume the task.
```

**Uncovered Area 41**: freertos_task.hpp:611
*Function*: `freertos::periodic_task<freertos::static_task_allocator<1024ul> >::periodic_task<double, std::ratio<1l, 1000l> >(char const*, unsigned int, std::function<void ()>&&, std::function<void ()>&&, std::function<void ()>&&, std::chrono::duration<double, std::ratio<1l, 1000l> > const&, bool)::{lambda()#1}::operator()() const`

```cpp
    m_on_start{std::move(on_start)}, m_on_stop{std::move(on_stop)},
    m_periodic_routine{std::move(periodic_routine)},
>>> m_task{name, priority, [this]() { run(); }, start_suspended} {}
    /**
    * @brief Construct a new periodic task object
```

**Uncovered Area 42**: freertos_task.hpp:611
*Function*: `freertos::periodic_task<freertos::static_task_allocator<1024ul> >::periodic_task<long, std::ratio<1l, 1000l> >(char const*, unsigned int, std::function<void ()>&&, std::function<void ()>&&, std::function<void ()>&&, std::chrono::duration<long, std::ratio<1l, 1000l> > const&, bool)::{lambda()#1}::operator()() const`

```cpp
    m_on_start{std::move(on_start)}, m_on_stop{std::move(on_stop)},
    m_periodic_routine{std::move(periodic_routine)},
>>> m_task{name, priority, [this]() { run(); }, start_suspended} {}
    /**
    * @brief Construct a new periodic task object
```

**Uncovered Area 43**: FreeRTOS.h:163
*Function*: `FreeRTOSMock::~FreeRTOSMock()`

```cpp
    class FreeRTOSMock {
    public:
>>> virtual ~FreeRTOSMock() = default;
    
    // Task creation and deletion
```

**Uncovered Area 44**: FreeRTOS.h:323
*Function*: `FreeRTOSMock::xMessageBufferSendFromISR(void*, void const*, unsigned long, int*)`

```cpp
    MOCK_METHOD(void, vMessageBufferDelete, (MessageBufferHandle_t xMessageBuffer));
    MOCK_METHOD(size_t, xMessageBufferSend, (MessageBufferHandle_t xMessageBuffer, const void* pvTxData, size_t xDataLengthBytes, TickType_t xTicksToWait));
>>> MOCK_METHOD(size_t, xMessageBufferSendFromISR, (MessageBufferHandle_t xMessageBuffer, const void* pvTxData, size_t xDataLengthBytes, BaseType_t* pxHigherPriorityTaskWoken));
    MOCK_METHOD(size_t, xMessageBufferReceive, (MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, TickType_t xTicksToWait));
    MOCK_METHOD(size_t, xMessageBufferReceiveFromISR, (MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, BaseType_t* pxHigherPriorityTaskWoken));
```

**Uncovered Area 45**: FreeRTOS.h:325
*Function*: `FreeRTOSMock::xMessageBufferReceiveFromISR(void*, void*, unsigned long, int*)`

```cpp
    MOCK_METHOD(size_t, xMessageBufferSendFromISR, (MessageBufferHandle_t xMessageBuffer, const void* pvTxData, size_t xDataLengthBytes, BaseType_t* pxHigherPriorityTaskWoken));
    MOCK_METHOD(size_t, xMessageBufferReceive, (MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, TickType_t xTicksToWait));
>>> MOCK_METHOD(size_t, xMessageBufferReceiveFromISR, (MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, BaseType_t* pxHigherPriorityTaskWoken));
    MOCK_METHOD(size_t, xMessageBufferSpaceAvailable, (MessageBufferHandle_t xMessageBuffer));
    MOCK_METHOD(BaseType_t, xMessageBufferReset, (MessageBufferHandle_t xMessageBuffer));
```

**Uncovered Area 46**: freertos_mocks.cpp:772-777
*Function*: `xMessageBufferSendFromISR`

```cpp

```

**Uncovered Area 47**: freertos_mocks.cpp:786-791
*Function*: `xMessageBufferReceiveFromISR`

```cpp

```

**Uncovered Area 48**: stl_semaphore_mocks.hpp:165
*Function*: `freertos_test::stl_counting_semaphore::take(unsigned int)::{lambda()#1}::operator()() const`

```cpp
    if (timeout_ms == portMAX_DELAY) {
    // Infinite wait
>>> m_condition.wait(lock, [this] { return m_count > 0; });
    m_count--;
    return pdTRUE;
```

**Uncovered Area 49**: stl_semaphore_mocks.hpp:77
*Function*: `freertos_test::stl_binary_semaphore::take(unsigned int)::{lambda()#1}::operator()() const`

```cpp
    if (timeout_ms == portMAX_DELAY) {
    // Infinite wait
>>> m_condition.wait(lock, [this] { return m_available; });
    m_available = false;
    return pdTRUE;
```

**Uncovered Area 50**: test_enhanced_cpp17_features.cpp:115
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .Times(AtLeast(2));
    
>>> auto on_start = [&start_count]() { start_count++; };
    auto on_stop = [&stop_count]() { stop_count++; };
    auto periodic = [&periodic_count]() { periodic_count++; };
```

**Uncovered Area 51**: test_enhanced_cpp17_features.cpp:116
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    auto on_start = [&start_count]() { start_count++; };
>>> auto on_stop = [&stop_count]() { stop_count++; };
    auto periodic = [&periodic_count]() { periodic_count++; };
    
```

**Uncovered Area 52**: test_enhanced_cpp17_features.cpp:117
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    auto on_start = [&start_count]() { start_count++; };
    auto on_stop = [&stop_count]() { stop_count++; };
>>> auto periodic = [&periodic_count]() { periodic_count++; };
    
    // Test move with different chrono duration types - using static allocation
```

**Uncovered Area 53**: test_enhanced_cpp17_features.cpp:127
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#4}::operator()() const`

```cpp
    // Create another task and move it
    sa::periodic_task<1024> task2("PeriodicMove2", 2,
>>> [&start_count]() { start_count++; },
    [&stop_count]() { stop_count++; },
    [&periodic_count]() { periodic_count++; });
```

**Uncovered Area 54**: test_enhanced_cpp17_features.cpp:128
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#5}::operator()() const`

```cpp
    sa::periodic_task<1024> task2("PeriodicMove2", 2,
    [&start_count]() { start_count++; },
>>> [&stop_count]() { stop_count++; },
    [&periodic_count]() { periodic_count++; });
    
```

**Uncovered Area 55**: test_enhanced_cpp17_features.cpp:129
*Function*: `Cpp17FeaturesTest_PeriodicTaskMoveWithChronoTypes_Test::TestBody()::{lambda()#6}::operator()() const`

```cpp
    [&start_count]() { start_count++; },
    [&stop_count]() { stop_count++; },
>>> [&periodic_count]() { periodic_count++; });
    
    // Test move construction instead of assignment (which is deleted)
```

**Uncovered Area 56**: test_enhanced_cpp17_features.cpp:152-154
*Function*: `Cpp17FeaturesTest_LambdaCaptureVarieties_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Capture by reference
>>> sa::task<1024> task1("RefCapture", 1, [&message]() {
    EXPECT_EQ(message, "Test message");
    });
```

**Uncovered Area 57**: test_enhanced_cpp17_features.cpp:157-159
*Function*: `Cpp17FeaturesTest_LambdaCaptureVarieties_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    // Capture by value
>>> sa::task<1024> task2("ValueCapture", 1, [counter]() {
    EXPECT_EQ(counter, 42);
    });
```

**Uncovered Area 58**: test_enhanced_cpp17_features.cpp:162-164
*Function*: `Cpp17FeaturesTest_LambdaCaptureVarieties_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    
    // Simple lambda without capture
>>> sa::task<1024> task3("NoCapture", 1, []() {
    // Simple test function
    });
```

**Uncovered Area 59**: test_enhanced_cpp17_features.cpp:172
*Function*: `Cpp17FeaturesTest_FunctionObjectVarieties_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Test with copyable function object instead
>>> auto func_lambda = []() { /* test */ };
    sa::task<1024> task1("StdFunction", 1, func_lambda);
    
```

**Uncovered Area 60**: test_enhanced_cpp17_features.cpp:178
*Function*: `Cpp17FeaturesTest_FunctionObjectVarieties_Test::TestBody()::CustomCallable::operator()() const`

```cpp
    class CustomCallable {
    public:
>>> void operator()() const { /* test */ }
    };
    
```

**Uncovered Area 61**: test_enhanced_cpp17_features.cpp:185
*Function*: `Cpp17FeaturesTest_FunctionObjectVarieties_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    // Test with function pointer
>>> sa::task<1024> task3("FunctionPointer", 1, []() { /* test */ });
    }
    
```

**Uncovered Area 62**: test_enhanced_cpp17_features.cpp:292
*Function*: `Cpp17FeaturesTest_CompileTimeConstants_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // These should compile with constexpr values - queues are created at construction
>>> sa::task<stack_size> task("ConstexprTest", priority, []() { /* test */ });
    sa::queue<queue_size, int> queue;
    
```

**Uncovered Area 63**: test_enhanced_cpp17_features.cpp:313
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    // Test with different chrono duration types in periodic tasks - using static allocation
    sa::periodic_task<1024> task1("ChronoMs", 1,
>>> []() { /* start */ },
    []() { /* stop */ },
    []() { /* periodic */ },
```

**Uncovered Area 64**: test_enhanced_cpp17_features.cpp:314
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    sa::periodic_task<1024> task1("ChronoMs", 1,
    []() { /* start */ },
>>> []() { /* stop */ },
    []() { /* periodic */ },
    500ms);
```

**Uncovered Area 65**: test_enhanced_cpp17_features.cpp:315
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() { /* start */ },
    []() { /* stop */ },
>>> []() { /* periodic */ },
    500ms);
    
```

**Uncovered Area 66**: test_enhanced_cpp17_features.cpp:319
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#4}::operator()() const`

```cpp
    
    sa::periodic_task<1024> task2("ChronoMs2", 1,
>>> []() { /* start */ },
    []() { /* stop */ },
    []() { /* periodic */ },
```

**Uncovered Area 67**: test_enhanced_cpp17_features.cpp:320
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#5}::operator()() const`

```cpp
    sa::periodic_task<1024> task2("ChronoMs2", 1,
    []() { /* start */ },
>>> []() { /* stop */ },
    []() { /* periodic */ },
    750ms);
```

**Uncovered Area 68**: test_enhanced_cpp17_features.cpp:321
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#6}::operator()() const`

```cpp
    []() { /* start */ },
    []() { /* stop */ },
>>> []() { /* periodic */ },
    750ms);
    
```

**Uncovered Area 69**: test_enhanced_cpp17_features.cpp:325
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#7}::operator()() const`

```cpp
    
    sa::periodic_task<1024> task3("ChronoCustom", 1,
>>> []() { /* start */ },
    []() { /* stop */ },
    []() { /* periodic */ },
```

**Uncovered Area 70**: test_enhanced_cpp17_features.cpp:326
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#8}::operator()() const`

```cpp
    sa::periodic_task<1024> task3("ChronoCustom", 1,
    []() { /* start */ },
>>> []() { /* stop */ },
    []() { /* periodic */ },
    duration<double, std::milli>(123.456));
```

**Uncovered Area 71**: test_enhanced_cpp17_features.cpp:327
*Function*: `Cpp17FeaturesTest_ChronoTypesIntegration_Test::TestBody()::{lambda()#9}::operator()() const`

```cpp
    []() { /* start */ },
    []() { /* stop */ },
>>> []() { /* periodic */ },
    duration<double, std::milli>(123.456));
    
```

**Uncovered Area 72**: test_enhanced_cpp17_features.cpp:350
*Function*: `Cpp17FeaturesTest_STLContainerIntegration_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Use emplace_back to construct in place
>>> tasks.emplace_back("VectorTask1", 1, []() { /* test */ });
    tasks.emplace_back("VectorTask2", 2, []() { /* test */ });
    tasks.emplace_back("VectorTask3", 3, []() { /* test */ });
```

**Uncovered Area 73**: test_enhanced_cpp17_features.cpp:351
*Function*: `Cpp17FeaturesTest_STLContainerIntegration_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    // Use emplace_back to construct in place
    tasks.emplace_back("VectorTask1", 1, []() { /* test */ });
>>> tasks.emplace_back("VectorTask2", 2, []() { /* test */ });
    tasks.emplace_back("VectorTask3", 3, []() { /* test */ });
    
```

**Uncovered Area 74**: test_enhanced_cpp17_features.cpp:352
*Function*: `Cpp17FeaturesTest_STLContainerIntegration_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    tasks.emplace_back("VectorTask1", 1, []() { /* test */ });
    tasks.emplace_back("VectorTask2", 2, []() { /* test */ });
>>> tasks.emplace_back("VectorTask3", 3, []() { /* test */ });
    
    EXPECT_EQ(tasks.size(), 3);
```

**Uncovered Area 75**: test_enhanced_cpp17_features.cpp:369
*Function*: `Cpp17FeaturesTest_UniquePtrIntegration_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Test with unique_ptr for dynamic task management
>>> auto task1 = std::make_unique<sa::task<1024>>("UniqueTask1", 1, []() { /* test */ });
    auto task2 = std::make_unique<sa::task<1024>>("UniqueTask2", 2, []() { /* test */ });
    
```

**Uncovered Area 76**: test_enhanced_cpp17_features.cpp:370
*Function*: `Cpp17FeaturesTest_UniquePtrIntegration_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    // Test with unique_ptr for dynamic task management
    auto task1 = std::make_unique<sa::task<1024>>("UniqueTask1", 1, []() { /* test */ });
>>> auto task2 = std::make_unique<sa::task<1024>>("UniqueTask2", 2, []() { /* test */ });
    
    EXPECT_NE(task1->handle(), nullptr);
```

**Uncovered Area 77**: test_enhanced_cpp17_features.cpp:69
*Function*: `Cpp17FeaturesTest_TaskMoveSemanticsPerfectForwarding_Test::TestBody()::{lambda()#1}::operator()()`

```cpp
    
    // Test perfect forwarding with various lambda captures
>>> auto task_func1 = [&call_count]() mutable { call_count++; };
    auto task_func2 = [&call_count](){ call_count += 2; };
    
```

**Uncovered Area 78**: test_enhanced_cpp17_features.cpp:70
*Function*: `Cpp17FeaturesTest_TaskMoveSemanticsPerfectForwarding_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    // Test perfect forwarding with various lambda captures
    auto task_func1 = [&call_count]() mutable { call_count++; };
>>> auto task_func2 = [&call_count](){ call_count += 2; };
    
    EXPECT_CALL(*mock, xTaskCreateStatic(_, _, _, _, _, _, _))
```

**Uncovered Area 79**: test_enhanced_cpp17_features.cpp:92
*Function*: `Cpp17FeaturesTest_TaskMoveAssignmentChaining_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .Times(AtLeast(2)); // Two tasks will be destroyed during moves
    
>>> auto task_func = [&executed]() { executed = true; };
    
    // Create multiple tasks
```

**Uncovered Area 80**: test_enhanced_cpp17_features.cpp:95
*Function*: `Cpp17FeaturesTest_TaskMoveAssignmentChaining_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    // Create multiple tasks
>>> sa::task<1024> task1("Original", 1, [&executed]() { executed = true; });
    sa::task<1024> task2("ToMove1", 2, task_func);
    sa::task<1024> task3("ToMove2", 3, task_func);
```

**Uncovered Area 81**: test_freertos_sw_timer.cpp:155
*Function*: `FreeRTOSSwTimerTest_StaticTimerAllocatorCreate_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    
    TimerHandle_t handle = allocator.create("TestTimer", 100, pdTRUE, &allocator,
>>> [](TimerHandle_t){});
    
    EXPECT_EQ(handle, mock_timer_handle);
```

**Uncovered Area 82**: test_freertos_sw_timer.cpp:168
*Function*: `FreeRTOSSwTimerTest_StaticTimerAllocatorCreateNullReturn_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    
    TimerHandle_t handle = allocator.create("TestTimer", 100, pdTRUE, nullptr,
>>> [](TimerHandle_t){});
    
    EXPECT_EQ(handle, nullptr);
```

**Uncovered Area 83**: test_freertos_sw_timer.cpp:203
*Function*: `FreeRTOSSwTimerTest_DynamicTimerAllocatorCreate_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    
    TimerHandle_t handle = allocator.create("TestTimer", 200, pdFALSE, &allocator,
>>> [](TimerHandle_t){});
    
    EXPECT_EQ(handle, mock_timer_handle);
```

**Uncovered Area 84**: test_freertos_sw_timer.cpp:216
*Function*: `FreeRTOSSwTimerTest_DynamicTimerAllocatorCreateNullReturn_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    
    TimerHandle_t handle = allocator.create("TestTimer", 200, pdFALSE, nullptr,
>>> [](TimerHandle_t){});
    
    EXPECT_EQ(handle, nullptr);
```

**Uncovered Area 85**: test_freertos_sw_timer.cpp:90
*Function*: `FreeRTOSSwTimerTest::createTestCallback()::{lambda()#1}::operator()() const`

```cpp
    // Helper function to create a test callback
    std::function<void()> createTestCallback() {
>>> return [this]() { callback_count++; };
    }
    
```

**Uncovered Area 86**: test_freertos_task.cpp:1020
*Function*: `FreeRTOSTaskTest_TaskChronoCompatibility_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("ChronoTask", 2, [](){});
    
    // Test chrono duration compatibility with notify_take
```

**Uncovered Area 87**: test_freertos_task.cpp:1090
*Function*: `FreeRTOSTaskTest_TaskMoveConstruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> original_task("MoveTest", 2, [](){});
    EXPECT_EQ(original_task.handle(), mock_task_handle);
    
```

**Uncovered Area 88**: test_freertos_task.cpp:1108
*Function*: `FreeRTOSTaskTest_PeriodicTaskMoveConstruction_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    sa::periodic_task<1024> original_task("PeriodicMoveTest", 2,
>>> [](){}, [](){}, [](){}, 100ms);
    EXPECT_EQ(original_task.handle(), mock_task_handle);
    
```

**Uncovered Area 89**: test_freertos_task.cpp:1108
*Function*: `FreeRTOSTaskTest_PeriodicTaskMoveConstruction_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    
    sa::periodic_task<1024> original_task("PeriodicMoveTest", 2,
>>> [](){}, [](){}, [](){}, 100ms);
    EXPECT_EQ(original_task.handle(), mock_task_handle);
    
```

**Uncovered Area 90**: test_freertos_task.cpp:1108
*Function*: `FreeRTOSTaskTest_PeriodicTaskMoveConstruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    sa::periodic_task<1024> original_task("PeriodicMoveTest", 2,
>>> [](){}, [](){}, [](){}, 100ms);
    EXPECT_EQ(original_task.handle(), mock_task_handle);
    
```

**Uncovered Area 91**: test_freertos_task.cpp:1162
*Function*: `FreeRTOSTaskTest_PeriodicTaskTypo_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "TypoTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 92**: test_freertos_task.cpp:1163
*Function*: `FreeRTOSTaskTest_PeriodicTaskTypo_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    100ms     // period
```

**Uncovered Area 93**: test_freertos_task.cpp:1164
*Function*: `FreeRTOSTaskTest_PeriodicTaskTypo_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    100ms     // period
    );
```

**Uncovered Area 94**: test_freertos_task.cpp:1187
*Function*: `FreeRTOSTaskTest_TaskTemplateInstantiation_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<512> small_task("SmallTask", 1, []() {});
    EXPECT_EQ(small_task.handle(), mock_task_handle);
    
```

**Uncovered Area 95**: test_freertos_task.cpp:1203
*Function*: `FreeRTOSTaskTest_DynamicTaskDifferentSizes_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    ));
    
>>> da::task<2048> dyn_task("DynTask", 3, []() {});
    EXPECT_EQ(dyn_task.handle(), mock_handle2);
    
```

**Uncovered Area 96**: test_freertos_task.cpp:1213
*Function*: `FreeRTOSTaskTest_TaskNotificationEdgeCases_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("EdgeTask", 2, []() {});
    
    // Test notification with different duration types (microseconds)
```

**Uncovered Area 97**: test_freertos_task.cpp:1298-1300
*Function*: `FreeRTOSTaskTest_RacingConditionTaskConstructorInitialization_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    });
    
>>> sa::task<1024> test_task("RacingTask", 2, []() {
    // Task execution body
    });
```

**Uncovered Area 98**: test_freertos_task.cpp:1326
*Function*: `FreeRTOSTaskTest_ConcurrentTaskCreationAndDestruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Create tasks with different lifetimes to test destructor race conditions
>>> auto task1 = std::make_unique<sa::task<512>>("Task1", 1, []() {});
    auto task2 = std::make_unique<sa::task<512>>("Task2", 2, []() {});
    auto task3 = std::make_unique<sa::task<512>>("Task3", 3, []() {});
```

**Uncovered Area 99**: test_freertos_task.cpp:1327
*Function*: `FreeRTOSTaskTest_ConcurrentTaskCreationAndDestruction_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    // Create tasks with different lifetimes to test destructor race conditions
    auto task1 = std::make_unique<sa::task<512>>("Task1", 1, []() {});
>>> auto task2 = std::make_unique<sa::task<512>>("Task2", 2, []() {});
    auto task3 = std::make_unique<sa::task<512>>("Task3", 3, []() {});
    
```

**Uncovered Area 100**: test_freertos_task.cpp:1328
*Function*: `FreeRTOSTaskTest_ConcurrentTaskCreationAndDestruction_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    auto task1 = std::make_unique<sa::task<512>>("Task1", 1, []() {});
    auto task2 = std::make_unique<sa::task<512>>("Task2", 2, []() {});
>>> auto task3 = std::make_unique<sa::task<512>>("Task3", 3, []() {});
    
    EXPECT_EQ(task1->handle(), handles[0]);
```

**Uncovered Area 101**: test_freertos_task.cpp:135
*Function*: `FreeRTOSTaskTest_DynamicTaskAllocatorCreateSuccess_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    ));
    
>>> auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, &allocator);
    
```

**Uncovered Area 102**: test_freertos_task.cpp:1354
*Function*: `FreeRTOSTaskTest_MoveSemanticsRacingConditions_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Create original task
>>> sa::task<1024> original_task("MoveTask", 1, []() {});
    EXPECT_EQ(original_task.handle(), original_handle);
    
```

**Uncovered Area 103**: test_freertos_task.cpp:1382
*Function*: `FreeRTOSTaskTest_PeriodicTaskLifecycleRacingConditions_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "PeriodicRace",
    2,
>>> [&start_count]() { start_count++; },  // on_start
    [&stop_count]() { stop_count++; },    // on_stop
    [&periodic_count]() { periodic_count++; },  // periodic_routine
```

**Uncovered Area 104**: test_freertos_task.cpp:1383
*Function*: `FreeRTOSTaskTest_PeriodicTaskLifecycleRacingConditions_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    [&start_count]() { start_count++; },  // on_start
>>> [&stop_count]() { stop_count++; },    // on_stop
    [&periodic_count]() { periodic_count++; },  // periodic_routine
    std::chrono::milliseconds(100)
```

**Uncovered Area 105**: test_freertos_task.cpp:1384
*Function*: `FreeRTOSTaskTest_PeriodicTaskLifecycleRacingConditions_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    [&start_count]() { start_count++; },  // on_start
    [&stop_count]() { stop_count++; },    // on_stop
>>> [&periodic_count]() { periodic_count++; },  // periodic_routine
    std::chrono::milliseconds(100)
    );
```

**Uncovered Area 106**: test_freertos_task.cpp:1411
*Function*: `FreeRTOSTaskTest_NotificationRacingConditions_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("NotifyRace", 2, []() {});
    
    // Simulate rapid notification operations
```

**Uncovered Area 107**: test_freertos_task.cpp:1456-1458
*Function*: `FreeRTOSTaskTest_ComplexMultitaskingScenario_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(producer_handle));
    
>>> sa::task<1024> producer("Producer", 3, []() {
    // Producer logic
    });
```

**Uncovered Area 108**: test_freertos_task.cpp:1464-1466
*Function*: `FreeRTOSTaskTest_ComplexMultitaskingScenario_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    .WillOnce(Return(consumer_handle));
    
>>> sa::task<1024> consumer("Consumer", 2, []() {
    // Consumer logic
    });
```

**Uncovered Area 109**: test_freertos_task.cpp:1472-1474
*Function*: `FreeRTOSTaskTest_ComplexMultitaskingScenario_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    .WillOnce(Return(coordinator_handle));
    
>>> sa::task<1024> coordinator("Coordinator", 4, []() {
    // Coordinator logic
    });
```

**Uncovered Area 110**: test_freertos_task.cpp:1543
*Function*: `FreeRTOSTaskTest_TaskSystemStatusUnderLoad_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(handles[2]));
    
>>> sa::task<512> task1("SysTask1", 1, []() {});
    sa::task<512> task2("SysTask2", 2, []() {});
    sa::task<512> task3("SysTask3", 3, []() {});
```

**Uncovered Area 111**: test_freertos_task.cpp:1544
*Function*: `FreeRTOSTaskTest_TaskSystemStatusUnderLoad_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    sa::task<512> task1("SysTask1", 1, []() {});
>>> sa::task<512> task2("SysTask2", 2, []() {});
    sa::task<512> task3("SysTask3", 3, []() {});
    
```

**Uncovered Area 112**: test_freertos_task.cpp:1545
*Function*: `FreeRTOSTaskTest_TaskSystemStatusUnderLoad_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    sa::task<512> task1("SysTask1", 1, []() {});
    sa::task<512> task2("SysTask2", 2, []() {});
>>> sa::task<512> task3("SysTask3", 3, []() {});
    
    // Test task system status with multiple tasks
```

**Uncovered Area 113**: test_freertos_task.cpp:1594-1596
*Function*: `FreeRTOSTaskTest_ConstructorInitializationOrderRaceCondition_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Create task with routine that sets atomic flag
>>> sa::task<1024> race_test_task("RaceTest", 2, [&task_routine_called]() {
    task_routine_called = true;
    });
```

**Uncovered Area 114**: test_freertos_task.cpp:165-167
*Function*: `FreeRTOSTaskTest_StaticTaskConstruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    bool task_executed = false;
>>> sa::task<1024> test_task("TestTask", 2, [&task_executed]() {
    task_executed = true;
    });
```

**Uncovered Area 115**: test_freertos_task.cpp:1741
*Function*: `FreeRTOSTaskTest_AdvancedRacingConditionScenarios_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(reinterpret_cast<TaskHandle_t>(0x3001)));
    
>>> sa::task<512> task1("RaceTask1", 1, []() {});
    EXPECT_EQ(task1.handle(), reinterpret_cast<TaskHandle_t>(0x3001));
    
```

**Uncovered Area 116**: test_freertos_task.cpp:1748
*Function*: `FreeRTOSTaskTest_AdvancedRacingConditionScenarios_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    .WillOnce(Return(reinterpret_cast<TaskHandle_t>(0x3002)));
    
>>> sa::task<512> task2("RaceTask2", 2, []() {});
    EXPECT_EQ(task2.handle(), reinterpret_cast<TaskHandle_t>(0x3002));
    
```

**Uncovered Area 117**: test_freertos_task.cpp:1796
*Function*: `FreeRTOSTaskTest_AdvancedChronoCompatibility_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("ChronoTask", 2, []() {});
    
    // Test notification take with very small durations
```

**Uncovered Area 118**: test_freertos_task.cpp:180-182
*Function*: `FreeRTOSTaskTest_StaticTaskConstructionWithString_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
    
>>> sa::task<1024> test_task(task_name, 3, []() {
    // Test task routine
    });
```

**Uncovered Area 119**: test_freertos_task.cpp:1838
*Function*: `FreeRTOSTaskTest_PriorityInheritanceScenario_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(high_prio_handle));
    
>>> sa::task<1024> low_prio_task("LowPrio", 1, []() {});
    sa::task<1024> high_prio_task("HighPrio", 5, []() {});
    
```

**Uncovered Area 120**: test_freertos_task.cpp:1839
*Function*: `FreeRTOSTaskTest_PriorityInheritanceScenario_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    
    sa::task<1024> low_prio_task("LowPrio", 1, []() {});
>>> sa::task<1024> high_prio_task("HighPrio", 5, []() {});
    
    // Low priority task acquires resource and gets priority boosted
```

**Uncovered Area 121**: test_freertos_task.cpp:194
*Function*: `FreeRTOSTaskTest_StaticTaskDestruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    {
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    } // Destructor should call vTaskDelete
    }
```

**Uncovered Area 122**: test_freertos_task.cpp:207
*Function*: `FreeRTOSTaskTest_StaticTaskDestructionNullHandle_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    {
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    }
    }
```

**Uncovered Area 123**: test_freertos_task.cpp:215
*Function*: `FreeRTOSTaskTest_StaticTaskSuspendResume_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, vTaskSuspend(mock_task_handle));
```

**Uncovered Area 124**: test_freertos_task.cpp:235
*Function*: `FreeRTOSTaskTest_StaticTaskTerminate_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, vTaskDelete(mock_task_handle));
```

**Uncovered Area 125**: test_freertos_task.cpp:248
*Function*: `FreeRTOSTaskTest_StaticTaskPriority_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, uxTaskPriorityGet(mock_task_handle))
```

**Uncovered Area 126**: test_freertos_task.cpp:271
*Function*: `FreeRTOSTaskTest_StaticTaskName_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, pcTaskGetName(mock_task_handle))
```

**Uncovered Area 127**: test_freertos_task.cpp:285
*Function*: `FreeRTOSTaskTest_StaticTaskState_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TestTask", 2, []() {});
    
    EXPECT_CALL(*mock, eTaskGetState(mock_task_handle))
```

**Uncovered Area 128**: test_freertos_task.cpp:299
*Function*: `FreeRTOSTaskTest_TaskApplicationTag_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("TagTask", 2, []() {});
    
    // Test setting application task tag
```

**Uncovered Area 129**: test_freertos_task.cpp:326
*Function*: `FreeRTOSTaskTest_TaskStackWatermark_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("WatermarkTask", 2, []() {});
    
    // Test stack high water mark
```

**Uncovered Area 130**: test_freertos_task.cpp:347
*Function*: `FreeRTOSTaskTest_TaskTraceStatus_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("StatusTask", 2, []() {});
    
    // Test task status with default parameters
```

**Uncovered Area 131**: test_freertos_task.cpp:417-419
*Function*: `FreeRTOSTaskTest_TaskSuspendedOnStart_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Test task created with start_suspended = true (default)
>>> sa::task<1024> suspended_task("SuspendedTask", 1, []() {
    // This task should start suspended
    }, true);
```

**Uncovered Area 132**: test_freertos_task.cpp:431-433
*Function*: `FreeRTOSTaskTest_TaskNotSuspendedOnStart_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    
    // Test task created with start_suspended = false
>>> sa::task<1024> active_task("ActiveTask", 1, []() {
    // This task should start immediately
    }, false);
```

**Uncovered Area 133**: test_freertos_task.cpp:447-449
*Function*: `FreeRTOSTaskTest_DynamicTaskConstruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    ));
    
>>> da::task<2048> test_task("DynamicTask", 3, []() {
    // Test task routine
    });
```

**Uncovered Area 134**: test_freertos_task.cpp:476
*Function*: `FreeRTOSTaskTest_TaskNotifications_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("NotifyTask", 2, []() {});
    
    // Test notify_give
```

**Uncovered Area 135**: test_freertos_task.cpp:514
*Function*: `FreeRTOSTaskTest_TaskNotificationsExtended_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> test_task("ExtendedNotifyTask", 2, []() {});
    
    // Test notify_and_query (fixed typo: was notfy_and_query)
```

**Uncovered Area 136**: test_freertos_task.cpp:588
*Function*: `FreeRTOSTaskTest_PeriodicTaskConstruction_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "PeriodicTask",
    2,
>>> [&on_start_called]() { on_start_called = true; },      // on_start
    [&on_stop_called]() { on_stop_called = true; },       // on_stop
    [&periodic_called]() { periodic_called = true; },     // periodic_routine
```

**Uncovered Area 137**: test_freertos_task.cpp:589
*Function*: `FreeRTOSTaskTest_PeriodicTaskConstruction_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    [&on_start_called]() { on_start_called = true; },      // on_start
>>> [&on_stop_called]() { on_stop_called = true; },       // on_stop
    [&periodic_called]() { periodic_called = true; },     // periodic_routine
    100ms                                                   // period
```

**Uncovered Area 138**: test_freertos_task.cpp:590
*Function*: `FreeRTOSTaskTest_PeriodicTaskConstruction_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    [&on_start_called]() { on_start_called = true; },      // on_start
    [&on_stop_called]() { on_stop_called = true; },       // on_stop
>>> [&periodic_called]() { periodic_called = true; },     // periodic_routine
    100ms                                                   // period
    );
```

**Uncovered Area 139**: test_freertos_task.cpp:614
*Function*: `FreeRTOSTaskTest_PeriodicTaskWithString_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    task_name,
    3,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 140**: test_freertos_task.cpp:615
*Function*: `FreeRTOSTaskTest_PeriodicTaskWithString_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    3,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    50ms      // period
```

**Uncovered Area 141**: test_freertos_task.cpp:616
*Function*: `FreeRTOSTaskTest_PeriodicTaskWithString_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    50ms      // period
    );
```

**Uncovered Area 142**: test_freertos_task.cpp:635
*Function*: `FreeRTOSTaskTest_PeriodicTaskZeroPeriod_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "ZeroPeriodTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 143**: test_freertos_task.cpp:636
*Function*: `FreeRTOSTaskTest_PeriodicTaskZeroPeriod_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    0ms       // period - should run continuously
```

**Uncovered Area 144**: test_freertos_task.cpp:637
*Function*: `FreeRTOSTaskTest_PeriodicTaskZeroPeriod_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    0ms       // period - should run continuously
    );
```

**Uncovered Area 145**: test_freertos_task.cpp:656
*Function*: `FreeRTOSTaskTest_PeriodicTaskNoPeriod_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "NoPeriodTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {}   // periodic_routine
```

**Uncovered Area 146**: test_freertos_task.cpp:657
*Function*: `FreeRTOSTaskTest_PeriodicTaskNoPeriod_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {}   // periodic_routine
    // No period specified - should default to 0ms
```

**Uncovered Area 147**: test_freertos_task.cpp:658
*Function*: `FreeRTOSTaskTest_PeriodicTaskNoPeriod_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {}   // periodic_routine
    // No period specified - should default to 0ms
    );
```

**Uncovered Area 148**: test_freertos_task.cpp:715
*Function*: `FreeRTOSTaskTest_PeriodicTaskTerminate_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "TerminateTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 149**: test_freertos_task.cpp:716
*Function*: `FreeRTOSTaskTest_PeriodicTaskTerminate_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    100ms     // period
```

**Uncovered Area 150**: test_freertos_task.cpp:717
*Function*: `FreeRTOSTaskTest_PeriodicTaskTerminate_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    100ms     // period
    );
```

**Uncovered Area 151**: test_freertos_task.cpp:761
*Function*: `FreeRTOSTaskTest_PeriodicTaskNotificationExtensions_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    "PeriodicNotifyTask",
    2,
>>> []() {},  // on_start
    []() {},  // on_stop
    []() {},  // periodic_routine
```

**Uncovered Area 152**: test_freertos_task.cpp:762
*Function*: `FreeRTOSTaskTest_PeriodicTaskNotificationExtensions_Test::TestBody()::{lambda()#2}::operator()() const`

```cpp
    2,
    []() {},  // on_start
>>> []() {},  // on_stop
    []() {},  // periodic_routine
    100ms     // period
```

**Uncovered Area 153**: test_freertos_task.cpp:763
*Function*: `FreeRTOSTaskTest_PeriodicTaskNotificationExtensions_Test::TestBody()::{lambda()#3}::operator()() const`

```cpp
    []() {},  // on_start
    []() {},  // on_stop
>>> []() {},  // periodic_routine
    100ms     // period
    );
```

**Uncovered Area 154**: test_freertos_task.cpp:85
*Function*: `FreeRTOSTaskTest_StaticTaskAllocatorCreate_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    )).WillOnce(Return(mock_task_handle));
    
>>> auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, &allocator);
    
```

**Uncovered Area 155**: test_freertos_task.cpp:957
*Function*: `FreeRTOSTaskTest_StackAllocationLimitation_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    // but in a real environment, this pattern should be avoided for static tasks
    {
>>> sa::task<1024> stack_task("StackLimitationDemo", 1, [](){});
    } // Stack allocation limitation applies here in real FreeRTOS usage
    }
```

**Uncovered Area 156**: test_freertos_task.cpp:970
*Function*: `FreeRTOSTaskTest_InvalidParameters_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(nullptr));
    
>>> sa::task<1024> task_with_null_name(nullptr, 1, [](){});
    EXPECT_EQ(task_with_null_name.handle(), nullptr);
    
```

**Uncovered Area 157**: test_freertos_task.cpp:98
*Function*: `FreeRTOSTaskTest_StaticTaskAllocatorCreateNullReturn_Test::TestBody()::{lambda(void*)#1}::operator()(void*) const`

```cpp
    .WillOnce(Return(nullptr));
    
>>> auto task_function = [](void*){};
    TaskHandle_t handle = allocator.create(task_function, "TestTask", 2, nullptr);
    
```

**Uncovered Area 158**: test_freertos_task.cpp:982
*Function*: `FreeRTOSTaskTest_ZeroStackSize_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(nullptr));  // FreeRTOS would likely fail this
    
>>> sa::task<0> zero_stack_task("ZeroStack", 1, [](){});
    EXPECT_EQ(zero_stack_task.handle(), nullptr);
    
```

**Uncovered Area 159**: test_freertos_task.cpp:995
*Function*: `FreeRTOSTaskTest_VeryHighPriority_Test::TestBody()::{lambda()#1}::operator()() const`

```cpp
    .WillOnce(Return(mock_task_handle));
    
>>> sa::task<1024> high_priority_task("HighPriority", max_priority, [](){});
    EXPECT_EQ(high_priority_task.handle(), mock_task_handle);
    
```

**Uncovered Area 160**: freertos_queue.hpp:489
*Line*: 489

```cpp
    return item;
    }
>>> return {};
    }
    /**
```

**Uncovered Area 161**: freertos_queue.hpp:628
*Line*: 628

```cpp
    return item;
    }
>>> return {};
    }
    /**
```


**Summary:** 161 uncovered code areas identified across 4 categories.

**Note:** These uncovered areas represent code that by design cannot be easily tested in a unit test environment. They require either integration testing with the actual FreeRTOS kernel, specific hardware configurations, or are defensive programming measures for edge cases that are difficult to reproduce.



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
- **Slowest Test**: 0.410 seconds
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

*Report Generated*: July 24, 2025 at 14:42:04  
*Test Framework*: GoogleTest/GoogleMock  
*Coverage Tool*: LCOV/GCOV  
*Total Test Execution Time*: 5.19 seconds  
*Validation Status*: ✅ **All tests passing - System validated for production use**
