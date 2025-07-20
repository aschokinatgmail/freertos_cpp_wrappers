#pragma once

// FreeRTOS queue mock header - provides queue types and function declarations for host-based testing

#include "FreeRTOS.h"

// Queue types
typedef void* QueueHandle_t;

// Queue error codes
#define errQUEUE_EMPTY    0
#define errQUEUE_FULL     0

// Queue registry function macros for testing
#define configQUEUE_REGISTRY_SIZE 10
#define configUSE_QUEUE_SETS 1

// Function declarations (to be mocked in freertos_mocks.cpp)
extern "C" {
    
// Queue creation functions
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize);
QueueHandle_t xQueueCreateStatic(UBaseType_t uxQueueLength, UBaseType_t uxItemSize, uint8_t* pucQueueStorage, StaticQueue_t* pxStaticQueue);

// Queue deletion
void vQueueDelete(QueueHandle_t xQueue);

// Send functions
BaseType_t xQueueSend(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait);
BaseType_t xQueueSendToBack(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait);
BaseType_t xQueueSendToFront(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait);

// Send from ISR functions
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void* pvItemToQueue, BaseType_t* pxHigherPriorityTaskWoken);
BaseType_t xQueueSendToBackFromISR(QueueHandle_t xQueue, const void* pvItemToQueue, BaseType_t* pxHigherPriorityTaskWoken);
BaseType_t xQueueSendToFrontFromISR(QueueHandle_t xQueue, const void* pvItemToQueue, BaseType_t* pxHigherPriorityTaskWoken);

// Receive functions
BaseType_t xQueueReceive(QueueHandle_t xQueue, void* pvBuffer, TickType_t xTicksToWait);
BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void* pvBuffer, BaseType_t* pxHigherPriorityTaskWoken);

// Peek functions
BaseType_t xQueuePeek(QueueHandle_t xQueue, void* pvBuffer, TickType_t xTicksToWait);
BaseType_t xQueuePeekFromISR(QueueHandle_t xQueue, void* pvBuffer, BaseType_t* pxHigherPriorityTaskWoken);

// Queue info functions
UBaseType_t uxQueueMessagesWaiting(QueueHandle_t xQueue);
UBaseType_t uxQueueMessagesWaitingFromISR(QueueHandle_t xQueue);
UBaseType_t uxQueueSpacesAvailable(QueueHandle_t xQueue);

// Queue state functions
BaseType_t xQueueIsQueueEmptyFromISR(QueueHandle_t xQueue);
BaseType_t xQueueIsQueueFullFromISR(QueueHandle_t xQueue);

// Queue reset and overwrite functions
BaseType_t xQueueReset(QueueHandle_t xQueue);
BaseType_t xQueueOverwrite(QueueHandle_t xQueue, const void* pvItemToQueue);
BaseType_t xQueueOverwriteFromISR(QueueHandle_t xQueue, const void* pvItemToQueue, BaseType_t* pxHigherPriorityTaskWoken);

// Queue registry functions
void vQueueAddToRegistry(QueueHandle_t xQueue, const char* pcQueueName);
void vQueueUnregisterQueue(QueueHandle_t xQueue);
const char* pcQueueGetName(QueueHandle_t xQueue);

} // extern "C"

// Tick conversion macro
#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t)((xTimeInMs) / portTICK_PERIOD_MS))