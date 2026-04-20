/*
 * FreeRTOSConfig_safety.h
 *
 * Safety-critical FreeRTOS configuration preset for certified applications.
 * Target: IEC 61508, ISO 26262 safety-certified environments.
 *
 * Restricts features for deterministic behavior and reduced attack surface:
 *   - Static allocation only (no dynamic allocation)
 *   - Mutexes with priority inheritance
 *   - Recursive mutexes
 *   - Counting semaphores
 *   - Queues
 *   - Software timers
 *   - Task notifications
 *   - Event groups
 *   - Stack overflow check mode 2 (most thorough)
 *   - Runtime statistics
 *   - configASSERT always enabled
 *   - Thread safety annotations enabled for static analysis
 *
 * No queue sets, no stream buffers, no message buffers.
 *
 * Copy this file to your project as FreeRTOSConfig.h and customize
 * for your specific safety requirements and hardware.
 */

#ifndef FREERTOSCONFIG_SAFETY_H
#define FREERTOSCONFIG_SAFETY_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *----------------------------------------------------------*/

/* Scheduling */
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      ( 72000000UL )
#define configTICK_RATE_HZ                      ( 1000UL )
#define configMAX_PRIORITIES                    8
#define configMINIMAL_STACK_SIZE                256
#define configMAX_TASK_NAME_LEN                 16
#define configIDLE_SHOULD_YIELD                 1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   1

/* Memory allocation — static only, no dynamic allocation */
#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        0
#define configTOTAL_HEAP_SIZE                   0
#define configAPPLICATION_ALLOCATED_HEAP        0
#define configSTACK_DEPTH_TYPE                  uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE        size_t

/* Sync primitives */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_STREAM_BUFFERS                0
#define configUSE_EVENT_GROUPS                  1
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               3
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            256

/* Hook functions — all enabled for safety monitoring */
#define configUSE_IDLE_HOOK                     1
#define configUSE_TICK_HOOK                     0
#define configUSE_MALLOC_FAILED_HOOK            0

/* Diagnostics — maximum safety checking */
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configQUEUE_REGISTRY_SIZE               8
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
#define configGENERATE_RUN_TIME_STATS           1

/* Assert — always enabled, never compiled out */
#define configASSERT( x )                       if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

/* Optional API functions */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           0
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xEventGroupSetBitFromISR        1
#define INCLUDE_xTimerPendFunctionCall          0
#define INCLUDE_xTaskAbortDelay                 0
#define INCLUDE_xTaskResumeFromISR              1
#define INCLUDE_vTaskGetInfo                    1

/* Co-routines (deprecated, disabled) */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         0

/* C++ wrappers specific */
#define FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS 1

#endif /* FREERTOSCONFIG_SAFETY_H */
