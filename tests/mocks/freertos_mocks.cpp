#include "FreeRTOS.h"
#include "freertos_mocks.hpp"

// Static member definitions for mock instance management
std::shared_ptr<::testing::StrictMock<::FreeRTOSMock>> FreeRTOSMockInstance::instance_;

void FreeRTOSMockInstance::setGlobalInstance(::FreeRTOSMock* mock) {
    extern ::FreeRTOSMock* g_freertos_mock;
    g_freertos_mock = mock;
}

// Global mock instance
::FreeRTOSMock* g_freertos_mock = nullptr;

// Function implementations that delegate to the mock
extern "C" {

TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode, const char* pcName, uint32_t ulStackDepth,
                               void* pvParameters, UBaseType_t uxPriority, StackType_t* puxStackBuffer,
                               StaticTask_t* pxTaskBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskCreateStatic(pxTaskCode, pcName, ulStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer);
    }
    return nullptr;
}

BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char* pcName, uint16_t usStackDepth,
                      void* pvParameters, UBaseType_t uxPriority, TaskHandle_t* pxCreatedTask) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
    }
    return pdFAIL;
}

void vTaskDelete(TaskHandle_t xTaskToDelete) {
    if (g_freertos_mock) {
        g_freertos_mock->vTaskDelete(xTaskToDelete);
    }
}

void vTaskSuspend(TaskHandle_t xTaskToSuspend) {
    if (g_freertos_mock) {
        g_freertos_mock->vTaskSuspend(xTaskToSuspend);
    }
}

void vTaskResume(TaskHandle_t xTaskToResume) {
    if (g_freertos_mock) {
        g_freertos_mock->vTaskResume(xTaskToResume);
    }
}

BaseType_t xTaskResumeFromISR(TaskHandle_t xTaskToResume) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskResumeFromISR(xTaskToResume);
    }
    return pdFALSE;
}

BaseType_t xTaskAbortDelay(TaskHandle_t xTask) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskAbortDelay(xTask);
    }
    return pdFALSE;
}

eTaskState eTaskGetState(TaskHandle_t xTask) {
    if (g_freertos_mock) {
        return g_freertos_mock->eTaskGetState(xTask);
    }
    return eInvalid;
}

const char* pcTaskGetName(TaskHandle_t xTaskToQuery) {
    if (g_freertos_mock) {
        return g_freertos_mock->pcTaskGetName(xTaskToQuery);
    }
    return "UnknownTask";
}

UBaseType_t uxTaskPriorityGet(TaskHandle_t xTask) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxTaskPriorityGet(xTask);
    }
    return 0;
}

UBaseType_t uxTaskPriorityGetFromISR(TaskHandle_t xTask) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxTaskPriorityGetFromISR(xTask);
    }
    return 0;
}

void vTaskPrioritySet(TaskHandle_t xTask, UBaseType_t uxNewPriority) {
    if (g_freertos_mock) {
        g_freertos_mock->vTaskPrioritySet(xTask, uxNewPriority);
    }
}

TaskHandle_t xTaskGetCurrentTaskHandle(void) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskGetCurrentTaskHandle();
    }
    return nullptr;
}

TaskHandle_t xTaskGetIdleTaskHandle(void) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskGetIdleTaskHandle();
    }
    return nullptr;
}

TickType_t xTaskGetTickCount(void) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskGetTickCount();
    }
    return 0;
}

TickType_t xTaskGetTickCountFromISR(void) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskGetTickCountFromISR();
    }
    return 0;
}

BaseType_t xTaskGetSchedulerState(void) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskGetSchedulerState();
    }
    return 0;
}

UBaseType_t uxTaskGetNumberOfTasks(void) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxTaskGetNumberOfTasks();
    }
    return 0;
}

UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxTaskGetStackHighWaterMark(xTask);
    }
    return 0;
}

UBaseType_t uxTaskGetStackHighWaterMark2(TaskHandle_t xTask) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxTaskGetStackHighWaterMark2(xTask);
    }
    return 0;
}

void vTaskDelay(TickType_t xTicksToDelay) {
    if (g_freertos_mock) {
        g_freertos_mock->vTaskDelay(xTicksToDelay);
    }
}

void vTaskDelayUntil(TickType_t* pxPreviousWakeTime, TickType_t xTimeIncrement) {
    if (g_freertos_mock) {
        g_freertos_mock->vTaskDelayUntil(pxPreviousWakeTime, xTimeIncrement);
    }
}

BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskNotifyGive(xTaskToNotify);
    }
    return pdFALSE;
}

uint32_t ulTaskNotifyTake(BaseType_t xClearCountOnExit, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->ulTaskNotifyTake(xClearCountOnExit, xTicksToWait);
    }
    return 0;
}

BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskNotify(xTaskToNotify, ulValue, eAction);
    }
    return pdFALSE;
}

BaseType_t xTaskNotifyAndQuery(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t* pulPreviousNotifyValue) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskNotifyAndQuery(xTaskToNotify, ulValue, eAction, pulPreviousNotifyValue);
    }
    return pdFALSE;
}

BaseType_t xTaskNotifyFromISR(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskNotifyFromISR(xTaskToNotify, ulValue, eAction, pxHigherPriorityTaskWoken);
    }
    return pdFALSE;
}

BaseType_t xTaskNotifyAndQueryFromISR(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t* pulPreviousNotifyValue, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskNotifyAndQueryFromISR(xTaskToNotify, ulValue, eAction, pulPreviousNotifyValue, pxHigherPriorityTaskWoken);
    }
    return pdFALSE;
}

BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t* pulNotificationValue, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskNotifyWait(ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait);
    }
    return pdFALSE;
}

BaseType_t xTaskNotifyStateClear(TaskHandle_t xTask) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTaskNotifyStateClear(xTask);
    }
    return pdFALSE;
}

uint32_t ulTaskNotifyValueClear(TaskHandle_t xTask, uint32_t ulBitsToClear) {
    if (g_freertos_mock) {
        return g_freertos_mock->ulTaskNotifyValueClear(xTask, ulBitsToClear);
    }
    return 0;
}

void vTaskGetInfo(TaskHandle_t xTask, TaskStatus_t* pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState) {
    if (g_freertos_mock) {
        g_freertos_mock->vTaskGetInfo(xTask, pxTaskStatus, xGetFreeStackSpace, eState);
    }
}

UBaseType_t uxTaskGetSystemState(TaskStatus_t* pxTaskStatusArray, UBaseType_t uxArraySize, uint32_t* pulTotalRunTime) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, pulTotalRunTime);
    }
    return 0;
}

void vTaskSetApplicationTaskTag(TaskHandle_t xTask, TaskHookFunction_t pxHookFunction) {
    if (g_freertos_mock) {
        g_freertos_mock->vTaskSetApplicationTaskTag(xTask, pxHookFunction);
    }
}

TaskHookFunction_t ulTaskGetApplicationTaskTag(TaskHandle_t xTask) {
    if (g_freertos_mock) {
        return g_freertos_mock->ulTaskGetApplicationTaskTag(xTask);
    }
    return nullptr;
}

TaskHookFunction_t ulTaskGetApplicationTaskTagFromISR(TaskHandle_t xTask) {
    if (g_freertos_mock) {
        return g_freertos_mock->ulTaskGetApplicationTaskTagFromISR(xTask);
    }
    return nullptr;
}

// Semaphore API implementations
SemaphoreHandle_t xSemaphoreCreateBinary(void) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreCreateBinary();
    }
    return nullptr;
}

SemaphoreHandle_t xSemaphoreCreateBinaryStatic(StaticSemaphore_t* pxSemaphoreBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreCreateBinaryStatic(pxSemaphoreBuffer);
    }
    return nullptr;
}

SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount, UBaseType_t uxInitialCount) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreCreateCounting(uxMaxCount, uxInitialCount);
    }
    return nullptr;
}

SemaphoreHandle_t xSemaphoreCreateCountingStatic(UBaseType_t uxMaxCount, UBaseType_t uxInitialCount, StaticSemaphore_t* pxSemaphoreBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreCreateCountingStatic(uxMaxCount, uxInitialCount, pxSemaphoreBuffer);
    }
    return nullptr;
}

SemaphoreHandle_t xSemaphoreCreateMutex(void) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreCreateMutex();
    }
    return nullptr;
}

SemaphoreHandle_t xSemaphoreCreateMutexStatic(StaticSemaphore_t* pxMutexBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreCreateMutexStatic(pxMutexBuffer);
    }
    return nullptr;
}

SemaphoreHandle_t xSemaphoreCreateRecursiveMutex(void) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreCreateRecursiveMutex();
    }
    return nullptr;
}

SemaphoreHandle_t xSemaphoreCreateRecursiveMutexStatic(StaticSemaphore_t* pxMutexBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreCreateRecursiveMutexStatic(pxMutexBuffer);
    }
    return nullptr;
}

BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreGive(xSemaphore);
    }
    return pdFALSE;
}

BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreGiveFromISR(xSemaphore, pxHigherPriorityTaskWoken);
    }
    return pdFALSE;
}

BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreTake(xSemaphore, xTicksToWait);
    }
    return pdFALSE;
}

BaseType_t xSemaphoreTakeFromISR(SemaphoreHandle_t xSemaphore, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xSemaphoreTakeFromISR(xSemaphore, pxHigherPriorityTaskWoken);
    }
    return pdFALSE;
}

UBaseType_t uxSemaphoreGetCount(SemaphoreHandle_t xSemaphore) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxSemaphoreGetCount(xSemaphore);
    }
    return 0;
}

void vSemaphoreDelete(SemaphoreHandle_t xSemaphore) {
    if (g_freertos_mock) {
        g_freertos_mock->vSemaphoreDelete(xSemaphore);
    }
}

// Queue function implementations
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueCreate(uxQueueLength, uxItemSize);
    }
    return nullptr;
}

QueueHandle_t xQueueCreateStatic(UBaseType_t uxQueueLength, UBaseType_t uxItemSize, uint8_t* pucQueueStorage, StaticQueue_t* pxStaticQueue) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueCreateStatic(uxQueueLength, uxItemSize, pucQueueStorage, pxStaticQueue);
    }
    return nullptr;
}

void vQueueDelete(QueueHandle_t xQueue) {
    if (g_freertos_mock) {
        g_freertos_mock->vQueueDelete(xQueue);
    }
}

BaseType_t xQueueSend(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueSend(xQueue, pvItemToQueue, xTicksToWait);
    }
    return pdFAIL;
}

BaseType_t xQueueSendToBack(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueSendToBack(xQueue, pvItemToQueue, xTicksToWait);
    }
    return pdFAIL;
}

BaseType_t xQueueSendToFront(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueSendToFront(xQueue, pvItemToQueue, xTicksToWait);
    }
    return pdFAIL;
}

BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void* pvItemToQueue, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueSendFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken);
    }
    return pdFAIL;
}

BaseType_t xQueueSendToBackFromISR(QueueHandle_t xQueue, const void* pvItemToQueue, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueSendToBackFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken);
    }
    return pdFAIL;
}

BaseType_t xQueueSendToFrontFromISR(QueueHandle_t xQueue, const void* pvItemToQueue, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueSendToFrontFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken);
    }
    return pdFAIL;
}

BaseType_t xQueueReceive(QueueHandle_t xQueue, void* pvBuffer, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueReceive(xQueue, pvBuffer, xTicksToWait);
    }
    return pdFAIL;
}

BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void* pvBuffer, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueReceiveFromISR(xQueue, pvBuffer, pxHigherPriorityTaskWoken);
    }
    return pdFAIL;
}

BaseType_t xQueuePeek(QueueHandle_t xQueue, void* pvBuffer, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueuePeek(xQueue, pvBuffer, xTicksToWait);
    }
    return pdFAIL;
}

BaseType_t xQueuePeekFromISR(QueueHandle_t xQueue, void* pvBuffer, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueuePeekFromISR(xQueue, pvBuffer, pxHigherPriorityTaskWoken);
    }
    return pdFAIL;
}

UBaseType_t uxQueueMessagesWaiting(QueueHandle_t xQueue) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxQueueMessagesWaiting(xQueue);
    }
    return 0;
}

UBaseType_t uxQueueMessagesWaitingFromISR(QueueHandle_t xQueue) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxQueueMessagesWaitingFromISR(xQueue);
    }
    return 0;
}

UBaseType_t uxQueueSpacesAvailable(QueueHandle_t xQueue) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxQueueSpacesAvailable(xQueue);
    }
    return 0;
}

BaseType_t xQueueIsQueueEmptyFromISR(QueueHandle_t xQueue) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueIsQueueEmptyFromISR(xQueue);
    }
    return pdTRUE;
}

BaseType_t xQueueIsQueueFullFromISR(QueueHandle_t xQueue) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueIsQueueFullFromISR(xQueue);
    }
    return pdFALSE;
}

BaseType_t xQueueReset(QueueHandle_t xQueue) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueReset(xQueue);
    }
    return pdPASS;
}

BaseType_t xQueueOverwrite(QueueHandle_t xQueue, const void* pvItemToQueue) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueOverwrite(xQueue, pvItemToQueue);
    }
    return pdPASS;
}

BaseType_t xQueueOverwriteFromISR(QueueHandle_t xQueue, const void* pvItemToQueue, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xQueueOverwriteFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken);
    }
    return pdPASS;
}

void vQueueAddToRegistry(QueueHandle_t xQueue, const char* pcQueueName) {
    if (g_freertos_mock) {
        g_freertos_mock->vQueueAddToRegistry(xQueue, pcQueueName);
    }
}

void vQueueUnregisterQueue(QueueHandle_t xQueue) {
    if (g_freertos_mock) {
        g_freertos_mock->vQueueUnregisterQueue(xQueue);
    }
}

const char* pcQueueGetName(QueueHandle_t xQueue) {
    if (g_freertos_mock) {
        return g_freertos_mock->pcQueueGetName(xQueue);
    }
    return nullptr;
}

// Event Group function implementations
EventGroupHandle_t xEventGroupCreate(void) {
    if (g_freertos_mock) {
        return g_freertos_mock->xEventGroupCreate();
    }
    return nullptr;
}

EventGroupHandle_t xEventGroupCreateStatic(StaticEventGroup_t* pxEventGroupBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xEventGroupCreateStatic(pxEventGroupBuffer);
    }
    return nullptr;
}

void vEventGroupDelete(EventGroupHandle_t xEventGroup) {
    if (g_freertos_mock) {
        g_freertos_mock->vEventGroupDelete(xEventGroup);
    }
}

EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet) {
    if (g_freertos_mock) {
        return g_freertos_mock->xEventGroupSetBits(xEventGroup, uxBitsToSet);
    }
    return 0;
}

EventBits_t xEventGroupSetBitsFromISR(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xEventGroupSetBitsFromISR(xEventGroup, uxBitsToSet, pxHigherPriorityTaskWoken);
    }
    return 0;
}

EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear) {
    if (g_freertos_mock) {
        return g_freertos_mock->xEventGroupClearBits(xEventGroup, uxBitsToClear);
    }
    return 0;
}

EventBits_t xEventGroupWaitBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xEventGroupWaitBits(xEventGroup, uxBitsToWaitFor, xClearOnExit, xWaitForAllBits, xTicksToWait);
    }
    return 0;
}

EventBits_t xEventGroupGetBits(EventGroupHandle_t xEventGroup) {
    if (g_freertos_mock) {
        return g_freertos_mock->xEventGroupGetBits(xEventGroup);
    }
    return 0;
}

EventBits_t xEventGroupGetBitsFromISR(EventGroupHandle_t xEventGroup) {
    if (g_freertos_mock) {
        return g_freertos_mock->xEventGroupGetBitsFromISR(xEventGroup);
    }
    return 0;
}

EventBits_t xEventGroupSync(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xEventGroupSync(xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTicksToWait);
    }
    return 0;
}

void portYIELD_FROM_ISR(BaseType_t xHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        g_freertos_mock->portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

// Timer function implementations
TimerHandle_t xTimerCreate(const char* pcTimerName, TickType_t xTimerPeriodInTicks, UBaseType_t uxAutoReload, void* pvTimerID, TimerCallbackFunction_t pxCallbackFunction) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerCreate(pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction);
    }
    return nullptr;
}

TimerHandle_t xTimerCreateStatic(const char* pcTimerName, TickType_t xTimerPeriodInTicks, UBaseType_t uxAutoReload, void* pvTimerID, TimerCallbackFunction_t pxCallbackFunction, StaticTimer_t* pxTimerBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerCreateStatic(pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction, pxTimerBuffer);
    }
    return nullptr;
}

BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerDelete(xTimer, xTicksToWait);
    }
    return pdFAIL;
}

BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerStart(xTimer, xTicksToWait);
    }
    return pdFAIL;
}

BaseType_t xTimerStartFromISR(TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerStartFromISR(xTimer, pxHigherPriorityTaskWoken);
    }
    return pdFAIL;
}

BaseType_t xTimerStop(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerStop(xTimer, xTicksToWait);
    }
    return pdFAIL;
}

BaseType_t xTimerStopFromISR(TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerStopFromISR(xTimer, pxHigherPriorityTaskWoken);
    }
    return pdFAIL;
}

BaseType_t xTimerReset(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerReset(xTimer, xTicksToWait);
    }
    return pdFAIL;
}

BaseType_t xTimerResetFromISR(TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerResetFromISR(xTimer, pxHigherPriorityTaskWoken);
    }
    return pdFAIL;
}

BaseType_t xTimerChangePeriod(TimerHandle_t xTimer, TickType_t xNewPeriod, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerChangePeriod(xTimer, xNewPeriod, xTicksToWait);
    }
    return pdFAIL;
}

BaseType_t xTimerChangePeriodFromISR(TimerHandle_t xTimer, TickType_t xNewPeriod, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerChangePeriodFromISR(xTimer, xNewPeriod, pxHigherPriorityTaskWoken);
    }
    return pdFAIL;
}

BaseType_t xTimerIsTimerActive(TimerHandle_t xTimer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerIsTimerActive(xTimer);
    }
    return pdFALSE;
}

TickType_t xTimerGetPeriod(TimerHandle_t xTimer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerGetPeriod(xTimer);
    }
    return 0;
}

TickType_t xTimerGetExpiryTime(TimerHandle_t xTimer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xTimerGetExpiryTime(xTimer);
    }
    return 0;
}

UBaseType_t uxTimerGetReloadMode(TimerHandle_t xTimer) {
    if (g_freertos_mock) {
        return g_freertos_mock->uxTimerGetReloadMode(xTimer);
    }
    return pdFALSE;
}

void vTimerSetReloadMode(TimerHandle_t xTimer, UBaseType_t uxAutoReload) {
    if (g_freertos_mock) {
        g_freertos_mock->vTimerSetReloadMode(xTimer, uxAutoReload);
    }
}

const char* pcTimerGetName(TimerHandle_t xTimer) {
    if (g_freertos_mock) {
        return g_freertos_mock->pcTimerGetName(xTimer);
    }
    return "UnknownTimer";
}

void* pvTimerGetTimerID(TimerHandle_t xTimer) {
    if (g_freertos_mock) {
        return g_freertos_mock->pvTimerGetTimerID(xTimer);
    }
    return nullptr;
}

void vMessageBufferDelete(MessageBufferHandle_t xMessageBuffer) {
    if (g_freertos_mock) {
        g_freertos_mock->vMessageBufferDelete(xMessageBuffer);
    }
}

size_t xMessageBufferSend(MessageBufferHandle_t xMessageBuffer, const void* pvTxData, size_t xDataLengthBytes, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xMessageBufferSend(xMessageBuffer, pvTxData, xDataLengthBytes, xTicksToWait);
    }
    return 0;
}

size_t xMessageBufferSendFromISR(MessageBufferHandle_t xMessageBuffer, const void* pvTxData, size_t xDataLengthBytes, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xMessageBufferSendFromISR(xMessageBuffer, pvTxData, xDataLengthBytes, pxHigherPriorityTaskWoken);
    }
    return 0;
}

size_t xMessageBufferReceive(MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xMessageBufferReceive(xMessageBuffer, pvRxData, xBufferLengthBytes, xTicksToWait);
    }
    return 0;
}

size_t xMessageBufferReceiveFromISR(MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xMessageBufferReceiveFromISR(xMessageBuffer, pvRxData, xBufferLengthBytes, pxHigherPriorityTaskWoken);
    }
    return 0;
}

size_t xMessageBufferSpaceAvailable(MessageBufferHandle_t xMessageBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xMessageBufferSpaceAvailable(xMessageBuffer);
    }
    return 0;
}

BaseType_t xMessageBufferReset(MessageBufferHandle_t xMessageBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xMessageBufferReset(xMessageBuffer);
    }
    return pdPASS;
}

BaseType_t xMessageBufferIsEmpty(MessageBufferHandle_t xMessageBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xMessageBufferIsEmpty(xMessageBuffer);
    }
    return pdTRUE;
}

BaseType_t xMessageBufferIsFull(MessageBufferHandle_t xMessageBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xMessageBufferIsFull(xMessageBuffer);
    }
    return pdFALSE;
}

// Stream buffer function implementations
StreamBufferHandle_t xStreamBufferCreate(size_t xBufferSizeBytes, size_t xTriggerLevelBytes) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferCreate(xBufferSizeBytes, xTriggerLevelBytes);
    }
    return nullptr;
}

StreamBufferHandle_t xStreamBufferCreateStatic(size_t xBufferSizeBytes, size_t xTriggerLevelBytes, 
                                               uint8_t* pucStreamBufferStorageArea, 
                                               StaticStreamBuffer_t* pxStaticStreamBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferCreateStatic(xBufferSizeBytes, xTriggerLevelBytes, 
                                                          pucStreamBufferStorageArea, pxStaticStreamBuffer);
    }
    return nullptr;
}

void vStreamBufferDelete(StreamBufferHandle_t xStreamBuffer) {
    if (g_freertos_mock) {
        g_freertos_mock->vStreamBufferDelete(xStreamBuffer);
    }
}

size_t xStreamBufferSend(StreamBufferHandle_t xStreamBuffer, const void* pvTxData, 
                        size_t xDataLengthBytes, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferSend(xStreamBuffer, pvTxData, xDataLengthBytes, xTicksToWait);
    }
    return 0;
}

size_t xStreamBufferSendFromISR(StreamBufferHandle_t xStreamBuffer, const void* pvTxData, 
                               size_t xDataLengthBytes, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferSendFromISR(xStreamBuffer, pvTxData, xDataLengthBytes, pxHigherPriorityTaskWoken);
    }
    return 0;
}

size_t xStreamBufferReceive(StreamBufferHandle_t xStreamBuffer, void* pvRxData, 
                           size_t xBufferLengthBytes, TickType_t xTicksToWait) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferReceive(xStreamBuffer, pvRxData, xBufferLengthBytes, xTicksToWait);
    }
    return 0;
}

size_t xStreamBufferReceiveFromISR(StreamBufferHandle_t xStreamBuffer, void* pvRxData, 
                                  size_t xBufferLengthBytes, BaseType_t* pxHigherPriorityTaskWoken) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferReceiveFromISR(xStreamBuffer, pvRxData, xBufferLengthBytes, pxHigherPriorityTaskWoken);
    }
    return 0;
}

size_t xStreamBufferBytesAvailable(StreamBufferHandle_t xStreamBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferBytesAvailable(xStreamBuffer);
    }
    return 0;
}

size_t xStreamBufferSpacesAvailable(StreamBufferHandle_t xStreamBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferSpacesAvailable(xStreamBuffer);
    }
    return 0;
}

BaseType_t xStreamBufferIsEmpty(StreamBufferHandle_t xStreamBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferIsEmpty(xStreamBuffer);
    }
    return pdTRUE;
}

BaseType_t xStreamBufferIsFull(StreamBufferHandle_t xStreamBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferIsFull(xStreamBuffer);
    }
    return pdFALSE;
}

BaseType_t xStreamBufferReset(StreamBufferHandle_t xStreamBuffer) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferReset(xStreamBuffer);
    }
    return pdPASS;
}

BaseType_t xStreamBufferSetTriggerLevel(StreamBufferHandle_t xStreamBuffer, size_t xTriggerLevel) {
    if (g_freertos_mock) {
        return g_freertos_mock->xStreamBufferSetTriggerLevel(xStreamBuffer, xTriggerLevel);
    }
    return pdPASS;
}

}