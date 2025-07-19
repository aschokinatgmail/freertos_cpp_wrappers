#pragma once

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

// Stream buffer functions
StreamBufferHandle_t xStreamBufferCreate(size_t xBufferSizeBytes, size_t xTriggerLevelBytes);
size_t xStreamBufferSend(StreamBufferHandle_t xStreamBuffer,
                        const void *pvTxData,
                        size_t xDataLengthBytes,
                        TickType_t xTicksToWait);
size_t xStreamBufferReceive(StreamBufferHandle_t xStreamBuffer,
                           void *pvRxData,
                           size_t xBufferLengthBytes,
                           TickType_t xTicksToWait);
size_t xStreamBufferBytesAvailable(StreamBufferHandle_t xStreamBuffer);
size_t xStreamBufferSpacesAvailable(StreamBufferHandle_t xStreamBuffer);
BaseType_t xStreamBufferIsEmpty(StreamBufferHandle_t xStreamBuffer);
BaseType_t xStreamBufferIsFull(StreamBufferHandle_t xStreamBuffer);
void vStreamBufferDelete(StreamBufferHandle_t xStreamBuffer);

// ISR functions
size_t xStreamBufferSendFromISR(StreamBufferHandle_t xStreamBuffer,
                               const void *pvTxData,
                               size_t xDataLengthBytes,
                               BaseType_t *pxHigherPriorityTaskWoken);
size_t xStreamBufferReceiveFromISR(StreamBufferHandle_t xStreamBuffer,
                                  void *pvRxData,
                                  size_t xBufferLengthBytes,
                                  BaseType_t *pxHigherPriorityTaskWoken);

// Additional functions
BaseType_t xStreamBufferReset(StreamBufferHandle_t xStreamBuffer);
BaseType_t xStreamBufferSetTriggerLevel(StreamBufferHandle_t xStreamBuffer, size_t xTriggerLevel);

// Static allocation
#if configSUPPORT_STATIC_ALLOCATION
typedef struct {
    uint8_t dummy[128];
} StaticStreamBuffer_t;

StreamBufferHandle_t xStreamBufferCreateStatic(size_t xBufferSizeBytes,
                                               size_t xTriggerLevelBytes,
                                               uint8_t *pucStreamBufferStorageArea,
                                               StaticStreamBuffer_t *pxStaticStreamBuffer);
#endif

#ifdef __cplusplus
}
#endif