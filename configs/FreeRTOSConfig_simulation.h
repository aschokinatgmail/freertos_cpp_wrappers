/*
 * FreeRTOSConfig_simulation.h
 *
 * FreeRTOS configuration preset for host-based unit testing and simulation.
 * Target: FreeRTOS+POSIX on Linux/macOS for CI pipelines and development.
 *
 * Enables all C++ wrapper features for comprehensive test coverage:
 *   - Both static and dynamic allocation
 *   - All synchronization and communication primitives
 *   - Indexed task notifications
 *   - Thread safety annotations enabled
 *   - Large heap (4 MB) for test scenarios
 *   - configASSERT as lightweight check (no infinite loop on failure)
 *   - No hardware-specific settings
 *
 * Use this preset for unit tests, integration tests, and CI pipelines
 * running on host machines. It is the same configuration used by the
 * project's own test suite.
 *
 * Copy this file to your project as FreeRTOSConfig.h.
 */

#ifndef FREERTOSCONFIG_SIMULATION_H
#define FREERTOSCONFIG_SIMULATION_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *----------------------------------------------------------*/

/* Scheduling */
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      ( 100000000UL )
#define configTICK_RATE_HZ                      ( 1000UL )
#define configMAX_PRIORITIES                    16
#define configMINIMAL_STACK_SIZE                1024
#define configMAX_TASK_NAME_LEN                 32
#define configIDLE_SHOULD_YIELD                 1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   4

/* Memory allocation */
#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   ( 4 * 1024 * 1024 )
#define configAPPLICATION_ALLOCATED_HEAP        0
#define configSTACK_DEPTH_TYPE                  uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE        size_t

/* Sync primitives — all enabled */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_STREAM_BUFFERS                1
#define configUSE_EVENT_GROUPS                  1
#define configUSE_QUEUE_SETS                    1
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               3
#define configTIMER_QUEUE_LENGTH                20
#define configTIMER_TASK_STACK_DEPTH            1024

/* Hook functions */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_MALLOC_FAILED_HOOK            0

/* Diagnostics */
#define configCHECK_FOR_STACK_OVERFLOW          1
#define configQUEUE_REGISTRY_SIZE               16
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
#define configGENERATE_RUN_TIME_STATS           1

/* Assert — lightweight for simulation: abort on failure */
#include <cassert>
#define configASSERT( x )                       assert( x )

/* Optional API functions — all included */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xEventGroupSetBitFromISR        1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskResumeFromISR              1
#define INCLUDE_vTaskGetInfo                    1

/* Co-routines (deprecated, disabled) */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         0

/* C++ wrappers specific */
#define FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS 1

#endif /* FREERTOSCONFIG_SIMULATION_H */
