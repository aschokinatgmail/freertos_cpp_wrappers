#pragma once

#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

// Timer callback function type
typedef void (*TimerCallbackFunction_t)(TimerHandle_t xTimer);

// Timer functions
TimerHandle_t xTimerCreate(const char * const pcTimerName,
                          const TickType_t xTimerPeriodInTicks,
                          const UBaseType_t uxAutoReload,
                          void * const pvTimerID,
                          TimerCallbackFunction_t pxCallbackFunction);
BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait);
BaseType_t xTimerStop(TimerHandle_t xTimer, TickType_t xTicksToWait);
BaseType_t xTimerReset(TimerHandle_t xTimer, TickType_t xTicksToWait);
BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xTicksToWait);
void *pvTimerGetTimerID(const TimerHandle_t xTimer);
const char *pcTimerGetName(TimerHandle_t xTimer);
TickType_t xTimerGetPeriod(TimerHandle_t xTimer);
BaseType_t xTimerIsTimerActive(TimerHandle_t xTimer);

// Static allocation
#if configSUPPORT_STATIC_ALLOCATION
typedef struct {
    uint8_t dummy[128];
} StaticTimer_t;

TimerHandle_t xTimerCreateStatic(const char * const pcTimerName,
                                const TickType_t xTimerPeriodInTicks,
                                const UBaseType_t uxAutoReload,
                                void * const pvTimerID,
                                TimerCallbackFunction_t pxCallbackFunction,
                                StaticTimer_t *pxTimerBuffer);
#endif

#ifdef __cplusplus
}
#endif