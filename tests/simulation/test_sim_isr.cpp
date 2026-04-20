// FreeRTOS+POSIX cannot simulate hardware interrupts. These tests exercise
// the ISR wrapper API surface (isr_result<T>, isr_result<void>, give_isr) from
// task context only. Real ISR semantics are verified on target hardware via
// mock-based unit tests.

#include <freertos.hpp>
#include <gtest/gtest.h>

#include <atomic>

struct ISRResults {
  std::atomic<bool> isr_result_struct;
  std::atomic<bool> isr_result_bool;
  std::atomic<bool> isr_result_void;
  std::atomic<bool> isr_methods_exist;
  std::atomic<bool> expected_isr;
  std::atomic<bool> all_done;

  void reset() {
    isr_result_struct.store(false);
    isr_result_bool.store(false);
    isr_result_void.store(false);
    isr_methods_exist.store(false);
    expected_isr.store(false);
    all_done.store(false);
  }
};

static ISRResults s_isr;

void isr_orchestrator(void *param) {
  auto *r = static_cast<ISRResults *>(param);

  r->isr_result_struct.store(false);
  r->isr_result_bool.store(false);
  r->isr_result_void.store(false);
  r->isr_methods_exist.store(false);
  r->expected_isr.store(false);

  {
    freertos::isr_result<int> ir{42, pdTRUE};
    r->isr_result_struct.store((ir.result == 42) && (ir.higher_priority_task_woken == pdTRUE));
  }

  {
    freertos::isr_result<BaseType_t> ir{pdTRUE, pdFALSE};
    r->isr_result_bool.store((ir.result == pdTRUE) && (ir.higher_priority_task_woken == pdFALSE));
  }

  {
    auto ir = freertos::isr_result<void>{};
    r->isr_result_void.store(true);
    (void)ir;
  }

  {
    freertos::da::binary_semaphore bs;
    auto result = bs.give_isr();
    r->isr_methods_exist.store(result.result == pdTRUE || result.result == pdFALSE);
  }

  {
    freertos::da::binary_semaphore bs;
    bs.give();
    auto ex_result = bs.take_ex(pdMS_TO_TICKS(10));
    r->expected_isr.store(ex_result.has_value());
  }

  r->all_done.store(true);
  vTaskEndScheduler();
}

TEST(SimISR, AllISRTests) {
  s_isr.reset();

  xTaskCreate(isr_orchestrator, "isr", 2048, &s_isr, 1, nullptr);
  vTaskStartScheduler();

  EXPECT_TRUE(s_isr.isr_result_struct) << "ISR result struct";
  EXPECT_TRUE(s_isr.isr_result_bool) << "ISR result bool fields";
  EXPECT_TRUE(s_isr.isr_result_void) << "ISR result void";
  EXPECT_TRUE(s_isr.isr_methods_exist) << "ISR methods exist";
  EXPECT_TRUE(s_isr.expected_isr) << "Expected with ISR result";
  EXPECT_TRUE(s_isr.all_done) << "All tests completed";
}