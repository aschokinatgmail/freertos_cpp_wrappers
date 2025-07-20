#pragma once

// Semaphore-specific FreeRTOS definitions - includes the main FreeRTOS.h mock
#include "FreeRTOS.h"

// This header is included by freertos_task.hpp but not used by the task functionality
// All required types and constants are already defined in FreeRTOS.h mock