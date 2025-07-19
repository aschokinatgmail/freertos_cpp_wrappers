#pragma once

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

// Message buffer functions
MessageBufferHandle_t xMessageBufferCreate(size_t xBufferSizeBytes);
size_t xMessageBufferSend(MessageBufferHandle_t xMessageBuffer,
                         const void *pvTxData,
                         size_t xDataLengthBytes,
                         TickType_t xTicksToWait);
size_t xMessageBufferReceive(MessageBufferHandle_t xMessageBuffer,
                            void *pvRxData,
                            size_t xBufferLengthBytes,
                            TickType_t xTicksToWait);
size_t xMessageBufferSpacesAvailable(MessageBufferHandle_t xMessageBuffer);
size_t xMessageBufferSpaceAvailable(MessageBufferHandle_t xMessageBuffer);
BaseType_t xMessageBufferIsEmpty(MessageBufferHandle_t xMessageBuffer);
BaseType_t xMessageBufferIsFull(MessageBufferHandle_t xMessageBuffer);
void vMessageBufferDelete(MessageBufferHandle_t xMessageBuffer);

// ISR functions
size_t xMessageBufferSendFromISR(MessageBufferHandle_t xMessageBuffer,
                                const void *pvTxData,
                                size_t xDataLengthBytes,
                                BaseType_t *pxHigherPriorityTaskWoken);
size_t xMessageBufferReceiveFromISR(MessageBufferHandle_t xMessageBuffer,
                                   void *pvRxData,
                                   size_t xBufferLengthBytes,
                                   BaseType_t *pxHigherPriorityTaskWoken);

// Additional functions
BaseType_t xMessageBufferReset(MessageBufferHandle_t xMessageBuffer);

// Static allocation
#if configSUPPORT_STATIC_ALLOCATION
typedef struct {
    uint8_t dummy[128];
} StaticMessageBuffer_t;

MessageBufferHandle_t xMessageBufferCreateStatic(size_t xBufferSizeBytes,
                                                 uint8_t *pucMessageBufferStorageArea,
                                                 StaticMessageBuffer_t *pxStaticMessageBuffer);
#endif

#ifdef __cplusplus
}
#endif