#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include <gtest/gtest.h>

extern "C" {

static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(
    StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
    configSTACK_DEPTH_TYPE *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory(
    StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer,
    configSTACK_DEPTH_TYPE *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

} // extern "C"

namespace {

struct SmokeTestResults {
  volatile bool sem_give_ok;
  volatile bool sem_take_ok;
  volatile bool sem_second_take_ok;
  volatile bool consumer_ran;
  volatile bool all_done;
};

void smoke_test_task(void *pvParams) {
  auto *r = static_cast<SmokeTestResults *>(pvParams);

  SemaphoreHandle_t sem = xSemaphoreCreateBinary();
  if (sem == nullptr) {
    r->all_done = true;
    vTaskEndScheduler();
    return;
  }

  r->sem_give_ok = (xSemaphoreGive(sem) == pdTRUE);
  r->sem_take_ok = (xSemaphoreTake(sem, pdMS_TO_TICKS(100)) == pdTRUE);
  r->sem_second_take_ok = (xSemaphoreTake(sem, pdMS_TO_TICKS(100)) == pdFALSE);
  vSemaphoreDelete(sem);

  SemaphoreHandle_t sync_sem = xSemaphoreCreateBinary();
  if (sync_sem == nullptr) {
    r->all_done = true;
    vTaskEndScheduler();
    return;
  }

  struct ConsumerParams {
    SemaphoreHandle_t sem;
    volatile bool *ran;
  } consumer_params{sync_sem, &r->consumer_ran};

  auto consumer_func = [](void *pvParams) {
    auto *p = static_cast<ConsumerParams *>(pvParams);
    xSemaphoreTake(p->sem, portMAX_DELAY);
    *(p->ran) = true;
    vTaskDelete(nullptr);
  };

  auto producer_func = [](void *pvParams) {
    SemaphoreHandle_t s = static_cast<SemaphoreHandle_t>(pvParams);
    vTaskDelay(pdMS_TO_TICKS(10));
    xSemaphoreGive(s);
    vTaskDelete(nullptr);
  };

  xTaskCreate(consumer_func, "consumer", 256, &consumer_params, 3, nullptr);
  xTaskCreate(producer_func, "producer", 256, sync_sem, 2, nullptr);

  vTaskDelay(pdMS_TO_TICKS(500));

  vSemaphoreDelete(sync_sem);

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimulationSmoke, AllSmokeTests) {
  SmokeTestResults results = {};

  ASSERT_EQ(xTaskCreate(smoke_test_task, "smoke", 256, &results, 1, nullptr),
            pdPASS);

  vTaskStartScheduler();

  EXPECT_TRUE(results.sem_give_ok);
  EXPECT_TRUE(results.sem_take_ok);
  EXPECT_TRUE(results.sem_second_take_ok);
  EXPECT_TRUE(results.consumer_ran);
  EXPECT_TRUE(results.all_done);
}

} // anonymous namespace