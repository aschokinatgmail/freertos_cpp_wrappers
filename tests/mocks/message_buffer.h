#pragma once

// FreeRTOS message buffer mock header for host-based testing
// This header provides the basic message buffer types and constants needed
// by the FreeRTOS C++ wrappers for unit testing

#include "FreeRTOS.h"

// Message buffer types are already defined in FreeRTOS.h:
// - MessageBufferHandle_t 
// - StaticMessageBuffer_t

// Message buffer creation functions are already declared in FreeRTOS.h as well
// This header serves as the include point for message buffer functionality

#ifdef __cplusplus
extern "C" {
#endif

// All message buffer function declarations are already in FreeRTOS.h
// This include structure matches the real FreeRTOS API where message_buffer.h
// would include the necessary declarations

#ifdef __cplusplus
}
#endif