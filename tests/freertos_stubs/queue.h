#pragma once

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

// Queue functions
QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize);
BaseType_t xQueueSend(QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait);
BaseType_t xQueueReceive(QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait);
UBaseType_t uxQueueMessagesWaiting(const QueueHandle_t xQueue);
void vQueueDelete(QueueHandle_t xQueue);

// ISR functions
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken);
BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void * const pvBuffer, BaseType_t * const pxHigherPriorityTaskWoken);

// Additional functions
BaseType_t xQueueReset(QueueHandle_t xQueue);
UBaseType_t uxQueueSpacesAvailable(const QueueHandle_t xQueue);

// More ISR functions
UBaseType_t uxQueueMessagesWaitingFromISR(const QueueHandle_t xQueue);
const char *pcQueueGetName(QueueHandle_t xQueue);
BaseType_t xQueueIsQueueFullFromISR(const QueueHandle_t xQueue);
BaseType_t xQueueIsQueueEmptyFromISR(const QueueHandle_t xQueue);
void vQueueUnregisterQueue(QueueHandle_t xQueue);
void vQueueAddToRegistry(QueueHandle_t xQueue, const char *pcQueueName);

// Static allocation
#if configSUPPORT_STATIC_ALLOCATION
typedef struct {
    uint8_t dummy[128];
} StaticQueue_t;

QueueHandle_t xQueueCreateStatic(const UBaseType_t uxQueueLength,
                                const UBaseType_t uxItemSize,
                                uint8_t *pucQueueStorage,
                                StaticQueue_t *pxStaticQueue);
#endif

#ifdef __cplusplus
}
#endif