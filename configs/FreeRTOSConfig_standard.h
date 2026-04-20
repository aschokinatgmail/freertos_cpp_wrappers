/*
 * FreeRTOSConfig_standard.h
 *
 * Standard FreeRTOS configuration preset for general-purpose embedded projects.
 * Target: Cortex-M3/M4, moderate RAM.
 *
 * Enables all common primitives:
 *   - Tasks (static + dynamic allocation)
 *   - Binary and counting semaphores
 *   - Mutexes and recursive mutexes
 *   - Queues
 *   - Software timers
 *   - Task notifications
 *   - Event groups
 *   - Stream buffers
 *
 * RAM estimate: ~8KB for kernel objects.
 *
 * Copy this file to your project as FreeRTOSConfig.h and customize
 * heap size, stack sizes, and tick rate for your hardware.
 */

#ifndef FREERTOSCONFIG_STANDARD_H
#define FREERTOSCONFIG_STANDARD_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *----------------------------------------------------------*/

/* Scheduling */
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      ( 72000000UL )
#define configTICK_RATE_HZ                      ( 1000UL )
#define configMAX_PRIORITIES                    8
#define configMINIMAL_STACK_SIZE                256
#define configMAX_TASK_NAME_LEN                 16
#define configIDLE_SHOULD_YIELD                 1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   1

/* Memory allocation */
#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   8192
#define configAPPLICATION_ALLOCATED_HEAP        0
#define configSTACK_DEPTH_TYPE                  uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE        size_t

/* Sync primitives */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_STREAM_BUFFERS                1
#define configUSE_EVENT_GROUPS                  1
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               3
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            256

/* Hook functions */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_MALLOC_FAILED_HOOK            1

/* Diagnostics */
#define configCHECK_FOR_STACK_OVERFLOW          1
#define configQUEUE_REGISTRY_SIZE               8
#define configUSE_TRACE_FACILITY                0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0
#define configGENERATE_RUN_TIME_STATS           0

/* Assert */
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
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xEventGroupSetBitFromISR        1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 0
#define INCLUDE_xTaskResumeFromISR              1
#define INCLUDE_vTaskGetInfo                    0

/* Co-routines (deprecated, disabled) */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         0

/* C++ wrappers specific */
/* Thread safety annotations: optional. To enable, compile with
 * -DFREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS=1 (requires Clang). */

#endif /* FREERTOSCONFIG_STANDARD_H */
