#pragma once

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

// Task function prototype
typedef void (*TaskFunction_t)(void *);

// Task creation stub functions
BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, 
                      const char * const pcName,
                      const uint16_t usStackDepth,
                      void * const pvParameters,
                      UBaseType_t uxPriority,
                      TaskHandle_t * const pxCreatedTask);

// Task delay functions
void vTaskDelay(const TickType_t xTicksToDelay);
void vTaskDelayUntil(TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement);

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

// Additional ISR and utility functions
TickType_t xTaskGetTickCountFromISR(void);
TaskHandle_t xTaskGetIdleTaskHandle(void);

// Configuration defines
#define INCLUDE_xTaskGetCurrentTaskHandle 1
#define INCLUDE_xTaskGetIdleTaskHandle 1

// Static allocation structures
#if configSUPPORT_STATIC_ALLOCATION
typedef struct {
    // Minimal placeholder
    uint8_t dummy[128];
} StaticTask_t;
#endif

#ifdef __cplusplus
}
#endif