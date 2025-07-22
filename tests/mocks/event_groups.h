#pragma once

// Event Groups mock header for FreeRTOS C++ wrapper testing
// This file provides mock FreeRTOS event group types and function declarations

#include "FreeRTOS.h"

// Event group configuration
#define configSUPPORT_STATIC_ALLOCATION 1
#define configSUPPORT_DYNAMIC_ALLOCATION 1

// Event group function declarations
extern "C" {
    EventGroupHandle_t xEventGroupCreate(void);
    EventGroupHandle_t xEventGroupCreateStatic(StaticEventGroup_t* pxEventGroupBuffer);
    void vEventGroupDelete(EventGroupHandle_t xEventGroup);
    EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet);
    EventBits_t xEventGroupSetBitsFromISR(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t* pxHigherPriorityTaskWoken);
    EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear);
    EventBits_t xEventGroupWaitBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait);
    EventBits_t xEventGroupGetBits(EventGroupHandle_t xEventGroup);
    EventBits_t xEventGroupGetBitsFromISR(EventGroupHandle_t xEventGroup);
    EventBits_t xEventGroupSync(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait);
    
    // Port layer function needed by ISR functions
    void portYIELD_FROM_ISR(BaseType_t xHigherPriorityTaskWoken);
}