# FreeRTOS+POSIX Simulation Tests

## Two test styles

### 1. Category tests (`test_sim_*.cpp`)

Each file contains exactly one `TEST()` that batch-runs multiple
sub-scenarios inside a single FreeRTOS task (the *orchestrator*).

**Why one TEST per binary?** The FreeRTOS+POSIX port only supports
one `vTaskStartScheduler` / `vTaskEndScheduler` cycle per process.
You cannot restart the scheduler, so each category is a separate
executable with its own scheduler invocation.

Assertions use `EXPECT_TRUE` (not `ASSERT_TRUE`) so that independent
scenarios are still reported on failure. Scenarios that depend on
earlier ones may be skipped if an upstream check fails.

### 2. Smoke tests (`test_sim_smoke.cpp`, `test_sim_system.cpp`)

These use `sim_test_fixture.hpp` which provides subprocess isolation
(`run_in_subprocess`). Each test case forks a child process that runs
its own scheduler, allowing multiple tests per binary.

## How to add a new category test

Use the `add_sim_test()` helper defined in `CMakeLists.txt`:

```cmake
add_sim_test(simulation_my_category test_sim_my_category.cpp)
```

The helper automatically links `sim_app_hooks.cpp` (FreeRTOS static
allocation hooks), `freertos_task.cc`, and the kernel simulation
library. An optional `TIMEOUT` parameter overrides the default 120s:

```cmake
add_sim_test(simulation_stress test_sim_stress.cpp TIMEOUT 300)
```

## Application memory hooks

`sim_app_hooks.cpp` provides `vApplicationGetIdleTaskMemory` and
`vApplicationGetTimerTaskMemory`. Only one set of buffers is needed
per binary because each binary runs exactly one scheduler invocation.