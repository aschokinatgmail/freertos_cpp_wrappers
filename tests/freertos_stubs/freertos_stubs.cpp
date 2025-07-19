#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "stream_buffer.h"
#include "timers.h"
#include <cstddef>
#include <cstdint>

// Stub implementations for FreeRTOS functions
// These are minimal implementations just to allow compilation

// Forward declarations for freertos namespace functions
namespace freertos {
    void delay(TickType_t ticks);
    void delay_until(TickType_t &previousWakeTime, TickType_t period);
}

extern "C" {

// Task functions
BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, 
                      const char * const pcName,
                      const uint16_t usStackDepth,
                      void * const pvParameters,
                      UBaseType_t uxPriority,
                      TaskHandle_t * const pxCreatedTask) {
    (void)pxTaskCode; (void)pcName; (void)usStackDepth; (void)pvParameters; (void)uxPriority; (void)pxCreatedTask;
    return pdPASS;
}

void vTaskDelay(const TickType_t xTicksToDelay) { (void)xTicksToDelay; }
void vTaskDelayUntil(TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement) {
    (void)pxPreviousWakeTime; (void)xTimeIncrement;
}
TickType_t xTaskGetTickCount(void) { return 0; }
TaskHandle_t xTaskGetCurrentTaskHandle(void) { return nullptr; }
void vTaskSuspendAll(void) {}
BaseType_t xTaskResumeAll(void) { return pdFALSE; }
eTaskState eTaskGetState(TaskHandle_t xTask) { (void)xTask; return eReady; }
UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask) { (void)xTask; return 0; }
void vTaskDelete(TaskHandle_t xTaskToDelete) { (void)xTaskToDelete; }
const char *pcTaskGetName(TaskHandle_t xTaskToQuery) { (void)xTaskToQuery; return "stub"; }
TickType_t xTaskGetTickCountFromISR(void) { return 0; }
TaskHandle_t xTaskGetIdleTaskHandle(void) { return nullptr; }

TaskHandle_t xTaskCreateStatic(void (*pxTaskCode)(void*),
                              const char * const pcName,
                              const uint32_t ulStackDepth,
                              void * const pvParameters,
                              UBaseType_t uxPriority,
                              StackType_t * const puxStackBuffer,
                              void * const pxTaskBuffer) {
    (void)pxTaskCode; (void)pcName; (void)ulStackDepth; (void)pvParameters; 
    (void)uxPriority; (void)puxStackBuffer; (void)pxTaskBuffer;
    return nullptr;
}

// Queue functions
QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize) {
    (void)uxQueueLength; (void)uxItemSize; return nullptr;
}
BaseType_t xQueueSend(QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait) {
    (void)xQueue; (void)pvItemToQueue; (void)xTicksToWait; return pdPASS;
}
BaseType_t xQueueReceive(QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait) {
    (void)xQueue; (void)pvBuffer; (void)xTicksToWait; return pdPASS;
}
UBaseType_t uxQueueMessagesWaiting(const QueueHandle_t xQueue) { (void)xQueue; return 0; }
void vQueueDelete(QueueHandle_t xQueue) { (void)xQueue; }
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken) {
    (void)xQueue; (void)pvItemToQueue; (void)pxHigherPriorityTaskWoken; return pdPASS;
}
BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void * const pvBuffer, BaseType_t * const pxHigherPriorityTaskWoken) {
    (void)xQueue; (void)pvBuffer; (void)pxHigherPriorityTaskWoken; return pdPASS;
}
BaseType_t xQueueReset(QueueHandle_t xQueue) { (void)xQueue; return pdPASS; }
UBaseType_t uxQueueSpacesAvailable(const QueueHandle_t xQueue) { (void)xQueue; return 0; }
UBaseType_t uxQueueMessagesWaitingFromISR(const QueueHandle_t xQueue) { (void)xQueue; return 0; }
const char *pcQueueGetName(QueueHandle_t xQueue) { (void)xQueue; return "stub"; }
BaseType_t xQueueIsQueueFullFromISR(const QueueHandle_t xQueue) { (void)xQueue; return pdFALSE; }
BaseType_t xQueueIsQueueEmptyFromISR(const QueueHandle_t xQueue) { (void)xQueue; return pdTRUE; }
void vQueueUnregisterQueue(QueueHandle_t xQueue) { (void)xQueue; }
void vQueueAddToRegistry(QueueHandle_t xQueue, const char *pcQueueName) { (void)xQueue; (void)pcQueueName; }

QueueHandle_t xQueueCreateStatic(const UBaseType_t uxQueueLength,
                                const UBaseType_t uxItemSize,
                                uint8_t *pucQueueStorage,
                                StaticQueue_t *pxStaticQueue) {
    (void)uxQueueLength; (void)uxItemSize; (void)pucQueueStorage; (void)pxStaticQueue;
    return nullptr;
}

// Semaphore functions
SemaphoreHandle_t xSemaphoreCreateBinary(void) { return nullptr; }
SemaphoreHandle_t xSemaphoreCreateCounting(const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount) {
    (void)uxMaxCount; (void)uxInitialCount; return nullptr;
}
SemaphoreHandle_t xSemaphoreCreateMutex(void) { return nullptr; }
SemaphoreHandle_t xSemaphoreCreateRecursiveMutex(void) { return nullptr; }
BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore) { (void)xSemaphore; return pdPASS; }
BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait) {
    (void)xSemaphore; (void)xTicksToWait; return pdPASS;
}
UBaseType_t uxSemaphoreGetCount(SemaphoreHandle_t xSemaphore) { (void)xSemaphore; return 0; }
void vSemaphoreDelete(SemaphoreHandle_t xSemaphore) { (void)xSemaphore; }
BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xSemaphore; (void)pxHigherPriorityTaskWoken; return pdPASS;
}
BaseType_t xSemaphoreTakeFromISR(SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xSemaphore; (void)pxHigherPriorityTaskWoken; return pdPASS;
}

SemaphoreHandle_t xSemaphoreCreateBinaryStatic(StaticSemaphore_t *pxSemaphoreBuffer) {
    (void)pxSemaphoreBuffer; return nullptr;
}
SemaphoreHandle_t xSemaphoreCreateCountingStatic(const UBaseType_t uxMaxCount,
                                                 const UBaseType_t uxInitialCount,
                                                 StaticSemaphore_t *pxSemaphoreBuffer) {
    (void)uxMaxCount; (void)uxInitialCount; (void)pxSemaphoreBuffer; return nullptr;
}
SemaphoreHandle_t xSemaphoreCreateMutexStatic(StaticSemaphore_t *pxMutexBuffer) {
    (void)pxMutexBuffer; return nullptr;
}
SemaphoreHandle_t xSemaphoreCreateRecursiveMutexStatic(StaticSemaphore_t *pxMutexBuffer) {
    (void)pxMutexBuffer; return nullptr;
}

// Event group functions  
EventGroupHandle_t xEventGroupCreate(void) { return nullptr; }
EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet) {
    (void)xEventGroup; (void)uxBitsToSet; return 0;
}
EventBits_t xEventGroupWaitBits(EventGroupHandle_t xEventGroup,
                               const EventBits_t uxBitsToWaitFor,
                               const BaseType_t xClearOnExit,
                               const BaseType_t xWaitForAllBits,
                               TickType_t xTicksToWait) {
    (void)xEventGroup; (void)uxBitsToWaitFor; (void)xClearOnExit; (void)xWaitForAllBits; (void)xTicksToWait;
    return 0;
}
EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear) {
    (void)xEventGroup; (void)uxBitsToClear; return 0;
}
EventBits_t xEventGroupGetBits(EventGroupHandle_t xEventGroup) { (void)xEventGroup; return 0; }
void vEventGroupDelete(EventGroupHandle_t xEventGroup) { (void)xEventGroup; }
EventBits_t xEventGroupSetBitsFromISR(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xEventGroup; (void)uxBitsToSet; (void)pxHigherPriorityTaskWoken; return 0;
}
EventBits_t xEventGroupClearBitsFromISR(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear) {
    (void)xEventGroup; (void)uxBitsToClear; return 0;
}
EventBits_t xEventGroupGetBitsFromISR(EventGroupHandle_t xEventGroup) { (void)xEventGroup; return 0; }
EventBits_t xEventGroupSync(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait) {
    (void)xEventGroup; (void)uxBitsToSet; (void)uxBitsToWaitFor; (void)xTicksToWait; return 0;
}

EventGroupHandle_t xEventGroupCreateStatic(StaticEventGroup_t *pxEventGroupBuffer) {
    (void)pxEventGroupBuffer; return nullptr;
}

// Message buffer functions
MessageBufferHandle_t xMessageBufferCreate(size_t xBufferSizeBytes) { (void)xBufferSizeBytes; return nullptr; }
size_t xMessageBufferSend(MessageBufferHandle_t xMessageBuffer,
                         const void *pvTxData,
                         size_t xDataLengthBytes,
                         TickType_t xTicksToWait) {
    (void)xMessageBuffer; (void)pvTxData; (void)xDataLengthBytes; (void)xTicksToWait; return 0;
}
size_t xMessageBufferReceive(MessageBufferHandle_t xMessageBuffer,
                            void *pvRxData,
                            size_t xBufferLengthBytes,
                            TickType_t xTicksToWait) {
    (void)xMessageBuffer; (void)pvRxData; (void)xBufferLengthBytes; (void)xTicksToWait; return 0;
}
size_t xMessageBufferSpacesAvailable(MessageBufferHandle_t xMessageBuffer) { (void)xMessageBuffer; return 0; }
size_t xMessageBufferSpaceAvailable(MessageBufferHandle_t xMessageBuffer) { (void)xMessageBuffer; return 0; }
BaseType_t xMessageBufferIsEmpty(MessageBufferHandle_t xMessageBuffer) { (void)xMessageBuffer; return pdTRUE; }
BaseType_t xMessageBufferIsFull(MessageBufferHandle_t xMessageBuffer) { (void)xMessageBuffer; return pdFALSE; }
void vMessageBufferDelete(MessageBufferHandle_t xMessageBuffer) { (void)xMessageBuffer; }
size_t xMessageBufferSendFromISR(MessageBufferHandle_t xMessageBuffer,
                                const void *pvTxData,
                                size_t xDataLengthBytes,
                                BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xMessageBuffer; (void)pvTxData; (void)xDataLengthBytes; (void)pxHigherPriorityTaskWoken; return 0;
}
size_t xMessageBufferReceiveFromISR(MessageBufferHandle_t xMessageBuffer,
                                   void *pvRxData,
                                   size_t xBufferLengthBytes,
                                   BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xMessageBuffer; (void)pvRxData; (void)xBufferLengthBytes; (void)pxHigherPriorityTaskWoken; return 0;
}
BaseType_t xMessageBufferReset(MessageBufferHandle_t xMessageBuffer) { (void)xMessageBuffer; return pdPASS; }

MessageBufferHandle_t xMessageBufferCreateStatic(size_t xBufferSizeBytes,
                                                 uint8_t *pucMessageBufferStorageArea,
                                                 StaticMessageBuffer_t *pxStaticMessageBuffer) {
    (void)xBufferSizeBytes; (void)pucMessageBufferStorageArea; (void)pxStaticMessageBuffer; return nullptr;
}

// Stream buffer functions
StreamBufferHandle_t xStreamBufferCreate(size_t xBufferSizeBytes, size_t xTriggerLevelBytes) {
    (void)xBufferSizeBytes; (void)xTriggerLevelBytes; return nullptr;
}
size_t xStreamBufferSend(StreamBufferHandle_t xStreamBuffer,
                        const void *pvTxData,
                        size_t xDataLengthBytes,
                        TickType_t xTicksToWait) {
    (void)xStreamBuffer; (void)pvTxData; (void)xDataLengthBytes; (void)xTicksToWait; return 0;
}
size_t xStreamBufferReceive(StreamBufferHandle_t xStreamBuffer,
                           void *pvRxData,
                           size_t xBufferLengthBytes,
                           TickType_t xTicksToWait) {
    (void)xStreamBuffer; (void)pvRxData; (void)xBufferLengthBytes; (void)xTicksToWait; return 0;
}
size_t xStreamBufferBytesAvailable(StreamBufferHandle_t xStreamBuffer) { (void)xStreamBuffer; return 0; }
size_t xStreamBufferSpacesAvailable(StreamBufferHandle_t xStreamBuffer) { (void)xStreamBuffer; return 0; }
BaseType_t xStreamBufferIsEmpty(StreamBufferHandle_t xStreamBuffer) { (void)xStreamBuffer; return pdTRUE; }
BaseType_t xStreamBufferIsFull(StreamBufferHandle_t xStreamBuffer) { (void)xStreamBuffer; return pdFALSE; }
void vStreamBufferDelete(StreamBufferHandle_t xStreamBuffer) { (void)xStreamBuffer; }
size_t xStreamBufferSendFromISR(StreamBufferHandle_t xStreamBuffer,
                               const void *pvTxData,
                               size_t xDataLengthBytes,
                               BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xStreamBuffer; (void)pvTxData; (void)xDataLengthBytes; (void)pxHigherPriorityTaskWoken; return 0;
}
size_t xStreamBufferReceiveFromISR(StreamBufferHandle_t xStreamBuffer,
                                  void *pvRxData,
                                  size_t xBufferLengthBytes,
                                  BaseType_t *pxHigherPriorityTaskWoken) {
    (void)xStreamBuffer; (void)pvRxData; (void)xBufferLengthBytes; (void)pxHigherPriorityTaskWoken; return 0;
}
BaseType_t xStreamBufferReset(StreamBufferHandle_t xStreamBuffer) { (void)xStreamBuffer; return pdPASS; }
BaseType_t xStreamBufferSetTriggerLevel(StreamBufferHandle_t xStreamBuffer, size_t xTriggerLevel) {
    (void)xStreamBuffer; (void)xTriggerLevel; return pdPASS;
}

StreamBufferHandle_t xStreamBufferCreateStatic(size_t xBufferSizeBytes,
                                               size_t xTriggerLevelBytes,
                                               uint8_t *pucStreamBufferStorageArea,
                                               StaticStreamBuffer_t *pxStaticStreamBuffer) {
    (void)xBufferSizeBytes; (void)xTriggerLevelBytes; (void)pucStreamBufferStorageArea; (void)pxStaticStreamBuffer;
    return nullptr;
}

// Timer functions
TimerHandle_t xTimerCreate(const char * const pcTimerName,
                          const TickType_t xTimerPeriodInTicks,
                          const UBaseType_t uxAutoReload,
                          void * const pvTimerID,
                          TimerCallbackFunction_t pxCallbackFunction) {
    (void)pcTimerName; (void)xTimerPeriodInTicks; (void)uxAutoReload; (void)pvTimerID; (void)pxCallbackFunction;
    return nullptr;
}
BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    (void)xTimer; (void)xTicksToWait; return pdPASS;
}
BaseType_t xTimerStop(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    (void)xTimer; (void)xTicksToWait; return pdPASS;
}
BaseType_t xTimerReset(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    (void)xTimer; (void)xTicksToWait; return pdPASS;
}
BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    (void)xTimer; (void)xTicksToWait; return pdPASS;
}
void *pvTimerGetTimerID(const TimerHandle_t xTimer) { (void)xTimer; return nullptr; }
const char *pcTimerGetName(TimerHandle_t xTimer) { (void)xTimer; return "stub"; }
TickType_t xTimerGetPeriod(TimerHandle_t xTimer) { (void)xTimer; return 0; }
BaseType_t xTimerIsTimerActive(TimerHandle_t xTimer) { (void)xTimer; return pdFALSE; }

TimerHandle_t xTimerCreateStatic(const char * const pcTimerName,
                                const TickType_t xTimerPeriodInTicks,
                                const UBaseType_t uxAutoReload,
                                void * const pvTimerID,
                                TimerCallbackFunction_t pxCallbackFunction,
                                StaticTimer_t *pxTimerBuffer) {
    (void)pcTimerName; (void)xTimerPeriodInTicks; (void)uxAutoReload; (void)pvTimerID; 
    (void)pxCallbackFunction; (void)pxTimerBuffer;
    return nullptr;
}

} // extern "C"

// FreeRTOS C++ wrapper namespace function implementations
namespace freertos {
void delay(TickType_t ticks) { vTaskDelay(ticks); }
void delay_until(TickType_t &previousWakeTime, TickType_t period) {
    vTaskDelayUntil(&previousWakeTime, period);
}
}