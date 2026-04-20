/*
 * FreeRTOSConfig_minimal.h
 *
 * Minimal FreeRTOS configuration preset for resource-constrained targets.
 * Target: Small MCUs (Cortex-M0+, limited RAM).
 *
 * Enables only essential kernel primitives:
 *   - Tasks with dynamic allocation
 *   - Binary semaphores (via mutexes)
 *   - Queues
 *   - Mutexes
 *
 * RAM estimate: ~2KB for kernel objects.
 *
 * Copy this file to your project as FreeRTOSConfig.h and customize
 * heap size, stack sizes, and tick rate for your hardware.
 */

#ifndef FREERTOSCONFIG_MINIMAL_H
#define FREERTOSCONFIG_MINIMAL_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *----------------------------------------------------------*/

/* Scheduling */
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      ( 1000000UL )
#define configTICK_RATE_HZ                      ( 1000UL )
#define configMAX_PRIORITIES                    4
#define configMINIMAL_STACK_SIZE                128
#define configMAX_TASK_NAME_LEN                 8
#define configIDLE_SHOULD_YIELD                 1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   1

/* Memory allocation */
#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   2048
#define configAPPLICATION_ALLOCATED_HEAP        0
#define configSTACK_DEPTH_TYPE                  uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE        size_t

/* Sync primitives */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_COUNTING_SEMAPHORES           0
#define configUSE_TASK_NOTIFICATIONS            0
#define configUSE_STREAM_BUFFERS                0
#define configUSE_EVENT_GROUPS                  0
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIMERS                        0

/* Hook functions */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_MALLOC_FAILED_HOOK            1

/* Diagnostics */
#define configCHECK_FOR_STACK_OVERFLOW          0
#define configQUEUE_REGISTRY_SIZE               0
#define configUSE_TRACE_FACILITY                0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0
#define configGENERATE_RUN_TIME_STATS           0

/* Assert */
#define configASSERT( x )                       if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

/* Optional API functions */
#define INCLUDE_vTaskPrioritySet                0
#define INCLUDE_uxTaskPriorityGet               0
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           0
#define INCLUDE_vTaskSuspend                    0
#define INCLUDE_vTaskDelayUntil                 0
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          0
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     0
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xEventGroupSetBitFromISR        0
#define INCLUDE_xTimerPendFunctionCall          0
#define INCLUDE_xTaskAbortDelay                 0
#define INCLUDE_xTaskResumeFromISR              0
#define INCLUDE_vTaskGetInfo                    0

/* Co-routines (deprecated, disabled) */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         0

/* C++ wrappers specific */
/* Thread safety annotations: disabled (requires Clang) */

#endif /* FREERTOSCONFIG_MINIMAL_H */
