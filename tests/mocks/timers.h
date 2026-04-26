#pragma once

// FreeRTOS timers.h mock header for host-based testing
// This header provides timer-specific declarations that are included by freertos_sw_timer.hpp

#include "FreeRTOS.h"

// Timer types are already defined in FreeRTOS.h
// This header is included for compatibility with the timer wrapper

// PendedFunction_t is defined in FreeRTOS.h
// Pend function call from ISR context
BaseType_t xTimerPendFunctionCall(PendedFunction_t xFunctionToPend,
                                  void *pvParameter1, uint32_t ulParameter2,
                                  TickType_t xTicksToWait);
