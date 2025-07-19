#pragma once

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

// Event bits type
typedef TickType_t EventBits_t;

// Event group functions
EventGroupHandle_t xEventGroupCreate(void);
EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet);
EventBits_t xEventGroupWaitBits(EventGroupHandle_t xEventGroup,
                               const EventBits_t uxBitsToWaitFor,
                               const BaseType_t xClearOnExit,
                               const BaseType_t xWaitForAllBits,
                               TickType_t xTicksToWait);
EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear);
EventBits_t xEventGroupGetBits(EventGroupHandle_t xEventGroup);
void vEventGroupDelete(EventGroupHandle_t xEventGroup);

// ISR functions
EventBits_t xEventGroupSetBitsFromISR(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t *pxHigherPriorityTaskWoken);
EventBits_t xEventGroupClearBitsFromISR(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear);
EventBits_t xEventGroupGetBitsFromISR(EventGroupHandle_t xEventGroup);

// Additional functions
EventBits_t xEventGroupSync(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait);

// Static allocation
#if configSUPPORT_STATIC_ALLOCATION
typedef struct {
    uint8_t dummy[128];
} StaticEventGroup_t;

EventGroupHandle_t xEventGroupCreateStatic(StaticEventGroup_t *pxEventGroupBuffer);
#endif

#ifdef __cplusplus
}
#endif