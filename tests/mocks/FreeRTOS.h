#pragma once

// FreeRTOS mock header - provides essential FreeRTOS types and constants for host-based testing

#include <cstdint>
#include <gmock/gmock.h>

// Basic FreeRTOS types
typedef void* TaskHandle_t;
typedef void (*TaskFunction_t)(void*);
typedef uint32_t TickType_t;
typedef uint32_t UBaseType_t;
typedef int32_t BaseType_t;
typedef uint32_t StackType_t;

// FreeRTOS constants
#define pdTRUE    1
#define pdFALSE   0
#define pdPASS    1
#define pdFAIL    0

#define portMAX_DELAY 0xFFFFFFFF
#define portTICK_PERIOD_MS 1

// Task states
typedef enum {
    eRunning = 0,
    eReady,
    eBlocked,
    eSuspended,
    eDeleted,
    eInvalid
} eTaskState;

// Task notification actions
typedef enum {
    eNoAction = 0,
    eSetBits,
    eIncrement,
    eSetValueWithOverwrite,
    eSetValueWithoutOverwrite
} eNotifyAction;

// Static allocation support flag (enabled for testing)
#define configSUPPORT_STATIC_ALLOCATION 1
#define configSUPPORT_DYNAMIC_ALLOCATION 1

// Feature flags (enabled for comprehensive testing)
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_vTaskDelete 1

// Semaphore types
typedef void* SemaphoreHandle_t;

// Semaphore static allocation structures (empty for mock)
typedef struct {
    uint8_t dummy[80]; // Placeholder size
} StaticSemaphore_t;
#define INCLUDE_xTaskAbortDelay 1
#define INCLUDE_uxTaskPriorityGet 1
#define INCLUDE_vTaskPrioritySet 1
#define INCLUDE_eTaskGetState 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1
#define INCLUDE_xTaskGetIdleTaskHandle 1
#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_uxTaskGetStackHighWaterMark2 1
#define INCLUDE_xTaskGetSchedulerState 1

#define configUSE_MUTEXES 1
#define configUSE_TRACE_FACILITY 1
#define configUSE_APPLICATION_TASK_TAG 1
#define configUSE_TASK_NOTIFICATIONS 1
#define configUSE_TIMERS 1

// Task status structure for trace facility
typedef struct {
    TaskHandle_t xHandle;
    const char* pcTaskName;
    UBaseType_t xTaskNumber;
    eTaskState eCurrentState;
    UBaseType_t uxCurrentPriority;
    UBaseType_t uxBasePriority;
    uint32_t ulRunTimeCounter;
    StackType_t* pxStackBase;
    uint16_t usStackHighWaterMark;
} TaskStatus_t;

// Static task structure (opaque for testing)
typedef struct {
    uint8_t ucDummy[256]; // Dummy data for static task buffer
} StaticTask_t;

// Hook function type
typedef void (*TaskHookFunction_t)(void);

// Critical section macros (no-op for host testing with comments)
#define taskENTER_CRITICAL()       // Host testing: Critical section entry not applicable
#define taskEXIT_CRITICAL()        // Host testing: Critical section exit not applicable
#define taskENTER_CRITICAL_FROM_ISR() 0  // Host testing: ISR critical section not applicable
#define taskEXIT_CRITICAL_FROM_ISR(x)    // Host testing: ISR critical section not applicable
#define taskDISABLE_INTERRUPTS()         // Host testing: Interrupt disable not applicable
#define taskENABLE_INTERRUPTS()          // Host testing: Interrupt enable not applicable
#define taskYIELD()                      // Host testing: Task yield stubbed

// Assert macro (no-op for host testing with comment)
#define configASSERT(x)                  // Host testing: Assert not applicable, condition: x

// Scheduler control macros
#define vTaskSuspendAll()                // Host testing: Scheduler suspend stubbed
#define xTaskResumeAll() pdTRUE          // Host testing: Scheduler resume stubbed

// Mock class for FreeRTOS API calls
class FreeRTOSMock {
public:
    virtual ~FreeRTOSMock() = default;
    
    // Task creation and deletion
    MOCK_METHOD(TaskHandle_t, xTaskCreateStatic, 
                (TaskFunction_t pxTaskCode, const char* pcName, uint32_t ulStackDepth,
                 void* pvParameters, UBaseType_t uxPriority, StackType_t* puxStackBuffer,
                 StaticTask_t* pxTaskBuffer));
    
    MOCK_METHOD(BaseType_t, xTaskCreate,
                (TaskFunction_t pxTaskCode, const char* pcName, uint16_t usStackDepth,
                 void* pvParameters, UBaseType_t uxPriority, TaskHandle_t* pxCreatedTask));
    
    MOCK_METHOD(void, vTaskDelete, (TaskHandle_t xTaskToDelete));
    
    // Task control
    MOCK_METHOD(void, vTaskSuspend, (TaskHandle_t xTaskToSuspend));
    MOCK_METHOD(void, vTaskResume, (TaskHandle_t xTaskToResume));
    MOCK_METHOD(BaseType_t, xTaskResumeFromISR, (TaskHandle_t xTaskToResume));
    MOCK_METHOD(BaseType_t, xTaskAbortDelay, (TaskHandle_t xTask));
    
    // Task information
    MOCK_METHOD(eTaskState, eTaskGetState, (TaskHandle_t xTask));
    MOCK_METHOD(const char*, pcTaskGetName, (TaskHandle_t xTaskToQuery));
    MOCK_METHOD(UBaseType_t, uxTaskPriorityGet, (TaskHandle_t xTask));
    MOCK_METHOD(UBaseType_t, uxTaskPriorityGetFromISR, (TaskHandle_t xTask));
    MOCK_METHOD(void, vTaskPrioritySet, (TaskHandle_t xTask, UBaseType_t uxNewPriority));
    
    // Task utilities
    MOCK_METHOD(TaskHandle_t, xTaskGetCurrentTaskHandle, ());
    MOCK_METHOD(TaskHandle_t, xTaskGetIdleTaskHandle, ());
    MOCK_METHOD(TickType_t, xTaskGetTickCount, ());
    MOCK_METHOD(TickType_t, xTaskGetTickCountFromISR, ());
    MOCK_METHOD(BaseType_t, xTaskGetSchedulerState, ());
    MOCK_METHOD(UBaseType_t, uxTaskGetNumberOfTasks, ());
    
    // Stack monitoring
    MOCK_METHOD(UBaseType_t, uxTaskGetStackHighWaterMark, (TaskHandle_t xTask));
    MOCK_METHOD(UBaseType_t, uxTaskGetStackHighWaterMark2, (TaskHandle_t xTask));
    
    // Task delays
    MOCK_METHOD(void, vTaskDelay, (TickType_t xTicksToDelay));
    MOCK_METHOD(void, vTaskDelayUntil, (TickType_t* pxPreviousWakeTime, TickType_t xTimeIncrement));
    
    // Task notifications
    MOCK_METHOD(BaseType_t, xTaskNotifyGive, (TaskHandle_t xTaskToNotify));
    MOCK_METHOD(uint32_t, ulTaskNotifyTake, (BaseType_t xClearCountOnExit, TickType_t xTicksToWait));
    MOCK_METHOD(BaseType_t, xTaskNotify, (TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction));
    MOCK_METHOD(BaseType_t, xTaskNotifyAndQuery, (TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t* pulPreviousNotifyValue));
    MOCK_METHOD(BaseType_t, xTaskNotifyFromISR, (TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, BaseType_t* pxHigherPriorityTaskWoken));
    MOCK_METHOD(BaseType_t, xTaskNotifyAndQueryFromISR, (TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t* pulPreviousNotifyValue, BaseType_t* pxHigherPriorityTaskWoken));
    MOCK_METHOD(BaseType_t, xTaskNotifyWait, (uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t* pulNotificationValue, TickType_t xTicksToWait));
    MOCK_METHOD(BaseType_t, xTaskNotifyStateClear, (TaskHandle_t xTask));
    MOCK_METHOD(uint32_t, ulTaskNotifyValueClear, (TaskHandle_t xTask, uint32_t ulBitsToClear));
    
    // Trace facility
    MOCK_METHOD(void, vTaskGetInfo, (TaskHandle_t xTask, TaskStatus_t* pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState));
    MOCK_METHOD(UBaseType_t, uxTaskGetSystemState, (TaskStatus_t* pxTaskStatusArray, UBaseType_t uxArraySize, uint32_t* pulTotalRunTime));
    
    // Application task tags
    MOCK_METHOD(void, vTaskSetApplicationTaskTag, (TaskHandle_t xTask, TaskHookFunction_t pxHookFunction));
    MOCK_METHOD(TaskHookFunction_t, ulTaskGetApplicationTaskTag, (TaskHandle_t xTask));
    MOCK_METHOD(TaskHookFunction_t, ulTaskGetApplicationTaskTagFromISR, (TaskHandle_t xTask));
    
    // Semaphore creation
    MOCK_METHOD(SemaphoreHandle_t, xSemaphoreCreateBinary, ());
    MOCK_METHOD(SemaphoreHandle_t, xSemaphoreCreateBinaryStatic, (StaticSemaphore_t* pxSemaphoreBuffer));
    MOCK_METHOD(SemaphoreHandle_t, xSemaphoreCreateCounting, (UBaseType_t uxMaxCount, UBaseType_t uxInitialCount));
    MOCK_METHOD(SemaphoreHandle_t, xSemaphoreCreateCountingStatic, (UBaseType_t uxMaxCount, UBaseType_t uxInitialCount, StaticSemaphore_t* pxSemaphoreBuffer));
    MOCK_METHOD(SemaphoreHandle_t, xSemaphoreCreateMutex, ());
    MOCK_METHOD(SemaphoreHandle_t, xSemaphoreCreateMutexStatic, (StaticSemaphore_t* pxMutexBuffer));
    MOCK_METHOD(SemaphoreHandle_t, xSemaphoreCreateRecursiveMutex, ());
    MOCK_METHOD(SemaphoreHandle_t, xSemaphoreCreateRecursiveMutexStatic, (StaticSemaphore_t* pxMutexBuffer));
    
    // Semaphore operations
    MOCK_METHOD(BaseType_t, xSemaphoreGive, (SemaphoreHandle_t xSemaphore));
    MOCK_METHOD(BaseType_t, xSemaphoreGiveFromISR, (SemaphoreHandle_t xSemaphore, BaseType_t* pxHigherPriorityTaskWoken));
    MOCK_METHOD(BaseType_t, xSemaphoreTake, (SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait));
    MOCK_METHOD(BaseType_t, xSemaphoreTakeFromISR, (SemaphoreHandle_t xSemaphore, BaseType_t* pxHigherPriorityTaskWoken));
    MOCK_METHOD(UBaseType_t, uxSemaphoreGetCount, (SemaphoreHandle_t xSemaphore));
    MOCK_METHOD(void, vSemaphoreDelete, (SemaphoreHandle_t xSemaphore));
};

// Global mock instance
extern FreeRTOSMock* g_freertos_mock;

// Function declarations that delegate to the mock
extern "C" {
    TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode, const char* pcName, uint32_t ulStackDepth,
                                   void* pvParameters, UBaseType_t uxPriority, StackType_t* puxStackBuffer,
                                   StaticTask_t* pxTaskBuffer);
    BaseType_t xTaskCreate(TaskFunction_t pxTaskCode, const char* pcName, uint16_t usStackDepth,
                          void* pvParameters, UBaseType_t uxPriority, TaskHandle_t* pxCreatedTask);
    void vTaskDelete(TaskHandle_t xTaskToDelete);
    void vTaskSuspend(TaskHandle_t xTaskToSuspend);
    void vTaskResume(TaskHandle_t xTaskToResume);
    BaseType_t xTaskResumeFromISR(TaskHandle_t xTaskToResume);
    BaseType_t xTaskAbortDelay(TaskHandle_t xTask);
    eTaskState eTaskGetState(TaskHandle_t xTask);
    const char* pcTaskGetName(TaskHandle_t xTaskToQuery);
    UBaseType_t uxTaskPriorityGet(TaskHandle_t xTask);
    UBaseType_t uxTaskPriorityGetFromISR(TaskHandle_t xTask);
    void vTaskPrioritySet(TaskHandle_t xTask, UBaseType_t uxNewPriority);
    TaskHandle_t xTaskGetCurrentTaskHandle(void);
    TaskHandle_t xTaskGetIdleTaskHandle(void);
    TickType_t xTaskGetTickCount(void);
    TickType_t xTaskGetTickCountFromISR(void);
    BaseType_t xTaskGetSchedulerState(void);
    UBaseType_t uxTaskGetNumberOfTasks(void);
    UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask);
    UBaseType_t uxTaskGetStackHighWaterMark2(TaskHandle_t xTask);
    void vTaskDelay(TickType_t xTicksToDelay);
    void vTaskDelayUntil(TickType_t* pxPreviousWakeTime, TickType_t xTimeIncrement);
    BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify);
    uint32_t ulTaskNotifyTake(BaseType_t xClearCountOnExit, TickType_t xTicksToWait);
    BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction);
    BaseType_t xTaskNotifyAndQuery(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t* pulPreviousNotifyValue);
    BaseType_t xTaskNotifyFromISR(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, BaseType_t* pxHigherPriorityTaskWoken);
    BaseType_t xTaskNotifyAndQueryFromISR(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t* pulPreviousNotifyValue, BaseType_t* pxHigherPriorityTaskWoken);
    BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t* pulNotificationValue, TickType_t xTicksToWait);
    BaseType_t xTaskNotifyStateClear(TaskHandle_t xTask);
    uint32_t ulTaskNotifyValueClear(TaskHandle_t xTask, uint32_t ulBitsToClear);
    void vTaskGetInfo(TaskHandle_t xTask, TaskStatus_t* pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState);
    UBaseType_t uxTaskGetSystemState(TaskStatus_t* pxTaskStatusArray, UBaseType_t uxArraySize, uint32_t* pulTotalRunTime);
    void vTaskSetApplicationTaskTag(TaskHandle_t xTask, TaskHookFunction_t pxHookFunction);
    TaskHookFunction_t ulTaskGetApplicationTaskTag(TaskHandle_t xTask);
    TaskHookFunction_t ulTaskGetApplicationTaskTagFromISR(TaskHandle_t xTask);
    
    // Semaphore functions
    SemaphoreHandle_t xSemaphoreCreateBinary(void);
    SemaphoreHandle_t xSemaphoreCreateBinaryStatic(StaticSemaphore_t* pxSemaphoreBuffer);
    SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount, UBaseType_t uxInitialCount);
    SemaphoreHandle_t xSemaphoreCreateCountingStatic(UBaseType_t uxMaxCount, UBaseType_t uxInitialCount, StaticSemaphore_t* pxSemaphoreBuffer);
    SemaphoreHandle_t xSemaphoreCreateMutex(void);
    SemaphoreHandle_t xSemaphoreCreateMutexStatic(StaticSemaphore_t* pxMutexBuffer);
    SemaphoreHandle_t xSemaphoreCreateRecursiveMutex(void);
    SemaphoreHandle_t xSemaphoreCreateRecursiveMutexStatic(StaticSemaphore_t* pxMutexBuffer);
    BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore);
    BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore, BaseType_t* pxHigherPriorityTaskWoken);
    BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);
    BaseType_t xSemaphoreTakeFromISR(SemaphoreHandle_t xSemaphore, BaseType_t* pxHigherPriorityTaskWoken);
    UBaseType_t uxSemaphoreGetCount(SemaphoreHandle_t xSemaphore);
    void vSemaphoreDelete(SemaphoreHandle_t xSemaphore);
}