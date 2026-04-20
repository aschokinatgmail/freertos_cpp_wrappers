/*
 * FreeRTOSConfig_full.h
 *
 * Full-featured FreeRTOS configuration preset for feature-rich applications.
 * Target: Cortex-M4/M7, generous RAM.
 *
 * Enables all C++ wrapper features:
 *   - All Standard preset features, plus:
 *   - Queue sets
 *   - Message buffers
 *   - Indexed task notifications (configTASK_NOTIFICATION_ARRAY_ENTRIES > 1)
 *   - SMP core affinity (configNUMBER_OF_CORES > 1, configUSE_CORE_AFFINITY)
 *   - Thread safety annotations enabled
 *   - Trace facility and runtime stats
 *
 * RAM estimate: ~16KB+ for kernel objects.
 *
 * Copy this file to your project as FreeRTOSConfig.h and customize
 * heap size, stack sizes, and tick rate for your hardware.
 */

#ifndef FREERTOSCONFIG_FULL_H
#define FREERTOSCONFIG_FULL_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *----------------------------------------------------------*/

/* Scheduling */
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      ( 168000000UL )
#define configTICK_RATE_HZ                      ( 1000UL )
#define configMAX_PRIORITIES                    16
#define configMINIMAL_STACK_SIZE                256
#define configMAX_TASK_NAME_LEN                 16
#define configIDLE_SHOULD_YIELD                 1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   4

/* SMP */
#define configNUMBER_OF_CORES                   2
#define configUSE_CORE_AFFINITY                 1

/* Memory allocation */
#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   16384
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
#define configUSE_QUEUE_SETS                    1
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               3
#define configTIMER_QUEUE_LENGTH                20
#define configTIMER_TASK_STACK_DEPTH            512

/* Hook functions */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_MALLOC_FAILED_HOOK            1

/* Diagnostics */
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configQUEUE_REGISTRY_SIZE               16
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
#define configGENERATE_RUN_TIME_STATS           1

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

#endif /* FREERTOSCONFIG_FULL_H */
