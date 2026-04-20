#include <freertos.hpp>
#include <gtest/gtest.h>

extern "C" {
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxIdleTaskStackBuffer,
                                     configSTACK_DEPTH_TYPE *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxIdleTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
}

struct ISRResults {
  volatile bool isr_result_struct;
  volatile bool isr_result_bool;
  volatile bool isr_result_void;
  volatile bool isr_methods_exist;
  volatile bool expected_isr;
  volatile bool all_done;
};

static ISRResults s_isr{};

void isr_orchestrator(void *param) {
  auto *r = static_cast<ISRResults *>(param);

  r->isr_result_struct = false;
  r->isr_result_bool = false;
  r->isr_result_void = false;
  r->isr_methods_exist = false;
  r->expected_isr = false;

  {
    freertos::isr_result<int> ir{42, pdTRUE};
    r->isr_result_struct = (ir.result == 42) && (ir.higher_priority_task_woken == pdTRUE);
  }

  {
    freertos::isr_result<BaseType_t> ir{pdTRUE, pdFALSE};
    r->isr_result_bool = (ir.result == pdTRUE) && (ir.higher_priority_task_woken == pdFALSE);
  }

  {
    auto ir = freertos::isr_result<void>{};
    r->isr_result_void = true;
    (void)ir;
  }

  {
    freertos::da::binary_semaphore bs;
    auto result = bs.give_isr();
    r->isr_methods_exist = (result.result == pdTRUE || result.result == pdFALSE);
  }

  {
    freertos::da::binary_semaphore bs;
    bs.give();
    auto ex_result = bs.take_ex(pdMS_TO_TICKS(10));
    r->expected_isr = ex_result.has_value();
  }

  r->all_done = true;
  vTaskEndScheduler();
}

TEST(SimISR, AllISRTests) {
  s_isr = {};

  xTaskCreate(isr_orchestrator, "isr", 2048, &s_isr, 1, nullptr);
  vTaskStartScheduler();

  EXPECT_TRUE(s_isr.isr_result_struct) << "ISR result struct";
  EXPECT_TRUE(s_isr.isr_result_bool) << "ISR result bool fields";
  EXPECT_TRUE(s_isr.isr_result_void) << "ISR result void";
  EXPECT_TRUE(s_isr.isr_methods_exist) << "ISR methods exist";
  EXPECT_TRUE(s_isr.expected_isr) << "Expected with ISR result";
  EXPECT_TRUE(s_isr.all_done) << "All tests completed";
}