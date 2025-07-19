#pragma once

// Stub FreeRTOS.h for testing
// This provides minimal definitions needed for compilation testing

#include <cstddef>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

// Basic FreeRTOS types
typedef unsigned long TickType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef void * TaskHandle_t;
typedef void * QueueHandle_t;
typedef void * SemaphoreHandle_t;
typedef void * EventGroupHandle_t;
typedef void * MessageBufferHandle_t;
typedef void * StreamBufferHandle_t;
typedef void * TimerHandle_t;
typedef unsigned long StackType_t;

// Configuration constants - commonly used values
#define configSUPPORT_STATIC_ALLOCATION 1
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configTICK_RATE_HZ 1000
#define configMAX_PRIORITIES 25
#define configASSERT(x)

// Common constants
#define pdPASS 1
#define pdFAIL 0
#define pdTRUE 1
#define pdFALSE 0

// Task states
typedef enum {
    eRunning = 0,
    eReady,
    eBlocked,
    eSuspended,
    eDeleted,
    eInvalid
} eTaskState;

// Wait constants
#define portMAX_DELAY 0xFFFFFFFFUL
#define portTICK_PERIOD_MS 1

// Task macros
#define taskENTER_CRITICAL() 
#define taskEXIT_CRITICAL()
#define taskENTER_CRITICAL_FROM_ISR() 0
#define taskEXIT_CRITICAL_FROM_ISR(x)
#define taskDISABLE_INTERRUPTS()
#define taskENABLE_INTERRUPTS()
#define portYIELD_FROM_ISR(x)

// Task functions
void vTaskSuspendAll(void);
BaseType_t xTaskResumeAll(void);
eTaskState eTaskGetState(TaskHandle_t xTask);
UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask);
void vTaskDelete(TaskHandle_t xTaskToDelete);
const char *pcTaskGetName(TaskHandle_t xTaskToQuery);
TaskHandle_t xTaskCreateStatic(void (*pxTaskCode)(void*),
                              const char * const pcName,
                              const uint32_t ulStackDepth,
                              void * const pvParameters,
                              UBaseType_t uxPriority,
                              StackType_t * const puxStackBuffer,
                              void * const pxTaskBuffer);

#ifdef __cplusplus
}
#endif