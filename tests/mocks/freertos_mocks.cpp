#include "FreeRTOS.h"

// Global mock instance
FreeRTOSMock* g_freertos_mock = nullptr;

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

}