// FreeRTOS+POSIX simulation application-memory hooks.
//
// Each simulation test binary runs exactly one vTaskStartScheduler /
// vTaskEndScheduler cycle per process (the POSIX port does not support
// restarting the scheduler). These static-allocation hooks are linked
// into every category test via add_sim_test() so that only one copy
// of the buffers is needed per binary.
//
// For the two test styles used in this directory:
// - Category tests (test_sim_*.cpp) run a single TEST() per binary
//   with many sub-scenarios batched inside, because the POSIX port
//   only allows one scheduler invocation per process. Assertions use
//   EXPECT_TRUE (not ASSERT_TRUE) so independent scenarios are still
//   reported even if earlier ones fail; dependent scenarios may be
//   skipped on upstream failure.
// - Smoke tests (test_sim_smoke.cpp) use sim_test_fixture.hpp which
//   provides subprocess isolation for the raw C API smoke check.

#include <FreeRTOS.h>
#include <task.h>

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