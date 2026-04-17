# Preset Configurations for FreeRTOS C++ Wrappers v3.0.0

## Overview

The `configs/` directory contains ready-made `FreeRTOSConfig.h` presets for common use cases. Each preset enables a specific subset of FreeRTOS and C++ wrapper features, tuned for a particular target environment.

These presets are starting points. Copy the one that best matches your project, then customize it for your specific requirements (stack sizes, queue lengths, heap size, etc.).

## Available Presets

### Minimal (`configs/FreeRTOSConfig_minimal.h`)

Target: Resource-constrained MCUs (Cortex-M0+, small RAM).

Enables only the essential kernel primitives:

- Tasks with dynamic allocation
- Binary semaphores
- Queues
- Mutexes

RAM estimate: ~2KB for kernel objects.

FreeRTOS macros enabled:

| Macro | Value |
|---|---|
| `configUSE_PREEMPTION` | 1 |
| `configUSE_MUTEXES` | 1 |
| `configUSE_RECURSIVE_MUTEXES` | 0 |
| `configUSE_COUNTING_SEMAPHORES` | 0 |
| `configUSE_TIMERS` | 0 |
| `configUSE_TASK_NOTIFICATIONS` | 0 |
| `configUSE_STREAM_BUFFERS` | 0 |
| `configUSE_EVENT_GROUPS` | 0 |
| `configUSE_QUEUE_SETS` | 0 |
| `configSUPPORT_STATIC_ALLOCATION` | 0 |
| `configSUPPORT_DYNAMIC_ALLOCATION` | 1 |
| `configTOTAL_HEAP_SIZE` | (2048) |
| `configMINIMAL_STACK_SIZE` | 128 |
| `configMAX_PRIORITIES` | 4 |
| `configTICK_RATE_HZ` | 1000 |

FREERTOS_CPP_WRAPPERS features available: `HAS_MUTEX`, `HAS_BINARY_SEMAPHORE` (via `configUSE_MUTEXES`), `HAS_DYNAMIC_ALLOCATION`.

Thread safety annotations: disabled (`FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS` not defined or 0).

### Standard (`configs/FreeRTOSConfig_standard.h`)

Target: General-purpose embedded (Cortex-M3/M4, moderate RAM).

Enables all common primitives except queue sets and SMP:

- Tasks (static + dynamic allocation)
- Binary and counting semaphores
- Mutexes and recursive mutexes
- Queues
- Software timers
- Task notifications
- Event groups
- Stream buffers

RAM estimate: ~8KB for kernel objects.

Thread safety annotations: optional (define `FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS` to enable).

### Full (`configs/FreeRTOSConfig_full.h`)

Target: Feature-rich applications (Cortex-M4/M7, generous RAM).

Enables all C++ wrapper features:

- All Standard preset features, plus:
- Queue sets
- Message buffers
- Indexed task notifications (`configTASK_NOTIFICATION_ARRAY_ENTRIES > 1`)
- SMP core affinity (`configNUMBER_OF_CORES > 1`, `configUSE_CORE_AFFINITY`)
- Thread safety annotations enabled

RAM estimate: ~16KB+ for kernel objects.

Thread safety annotations: enabled (`FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS` = 1).

### Safety-Critical (`configs/FreeRTOSConfig_safety.h`)

Target: Safety-certified applications (IEC 61508, ISO 26262).

Restricts features to reduce attack surface and ensure deterministic behavior:

- Static allocation only (no dynamic allocation)
- No queue sets
- No stream buffers
- No message buffers
- Thread safety annotations enabled for static analysis
- All analysis enabled: MISRA, clang-tidy, coverage

This preset is designed for environments where memory allocation patterns must be fully deterministic at compile time.

### Simulation (`configs/FreeRTOSConfig_simulation.h`)

Target: FreeRTOS+POSIX simulation on Linux/macOS.

Enables all wrappers for comprehensive testing:

- Both static and dynamic allocation
- All primitives enabled
- POSIX port configuration
- Large heap (4 MB)
- Thread safety annotations enabled

Use this preset for unit tests, integration tests, and CI pipelines running on host machines. It is the same configuration used by the project's own test suite.

## How to Use a Preset

1. **Copy** the preset file to your project:
   ```
   cp configs/FreeRTOSConfig_standard.h /your/project/FreeRTOSConfig.h
   ```

2. **Include** it as your project's `FreeRTOSConfig.h`. The FreeRTOS kernel includes `FreeRTOS.h` which expects `FreeRTOSConfig.h` to be on the include path.

3. **Customize** for your specific needs. At minimum, adjust:
   - `configTOTAL_HEAP_SIZE` -- match your available RAM
   - `configMINIMAL_STACK_SIZE` -- match your idle/task requirements
   - `configTICK_RATE_HZ` -- match your hardware timer configuration
   - `configMAX_PRIORITIES` -- match your priority scheme

4. **Build** with appropriate CMake flags:
   ```
   target_include_directories(your_target PRIVATE /path/to/your/config)
   ```

   If you want to enable thread safety annotations, also add:
   ```
   target_compile_definitions(your_target PRIVATE FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS=1)
   ```

   Note: Thread safety annotations require Clang. They expand to `__attribute__` directives that are only supported by Clang's thread safety analysis.

## Customization Checklist

The most commonly customized settings, organized by category:

### Memory

| Setting | Description | Typical Range |
|---|---|---|
| `configTOTAL_HEAP_SIZE` | Total heap size for dynamic allocation | 2048 -- 1M+ |
| `configSUPPORT_STATIC_ALLOCATION` | Enable static (compile-time) allocation | 0 or 1 |
| `configSUPPORT_DYNAMIC_ALLOCATION` | Enable dynamic (heap) allocation | 0 or 1 |

### Scheduling

| Setting | Description | Typical Range |
|---|---|---|
| `configUSE_PREEMPTION` | Preemptive scheduler | 0 (cooperative) or 1 (preemptive) |
| `configMINIMAL_STACK_SIZE` | Minimum stack for idle/task (words) | 64 -- 1024 |
| `configMAX_PRIORITIES` | Number of priority levels | 4 -- 32 |
| `configTICK_RATE_HZ` | Tick frequency in Hz | 100 -- 10000 |
| `configNUMBER_OF_CORES` | Core count for SMP | 1 (single-core) or 2+ |

### Feature Enables

| Setting | Description | Dependency |
|---|---|---|
| `configUSE_MUTEXES` | Enable mutexes | -- |
| `configUSE_RECURSIVE_MUTEXES` | Enable recursive mutexes | `configUSE_MUTEXES` |
| `configUSE_COUNTING_SEMAPHORES` | Enable counting semaphores | -- |
| `configUSE_TIMERS` | Enable software timers | -- |
| `configUSE_TASK_NOTIFICATIONS` | Enable task notifications | -- |
| `configUSE_STREAM_BUFFERS` | Enable stream buffers | -- |
| `configUSE_EVENT_GROUPS` | Enable event groups | -- |
| `configUSE_QUEUE_SETS` | Enable queue sets | -- |

### Diagnostics

| Setting | Description | Typical Value |
|---|---|---|
| `configQUEUE_REGISTRY_SIZE` | Queue registry size (for naming) | 0 -- 32 |
| `configUSE_TRACE_FACILITY` | Enable trace hooks | 0 or 1 |
| `configUSE_STATS_FORMATTING_FUNCTIONS` | Enable `vTaskList()` / `vTaskGetRunTimeStats()` | 0 or 1 |

### C++ Wrappers Specific

| Setting | Description | Requirement |
|---|---|---|
| `configTASK_NOTIFICATION_ARRAY_ENTRIES` | Indexed notification array size | 1 (simple) or 2--4 (indexed) |
| `configUSE_CORE_AFFINITY` | Enable SMP core affinity | `configNUMBER_OF_CORES > 1` |
| `FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS` | Enable Clang thread safety annotations | Clang compiler |

## Compatibility Matrix

The table below shows which FreeRTOS configuration macros are required or relevant for each C++ wrapper class. A check mark indicates the macro must be enabled (set to 1) for the class to be available.

### Task Classes

| Wrapper Class | `configSUPPORT_STATIC_ALLOCATION` | `configSUPPORT_DYNAMIC_ALLOCATION` | `configUSE_TASK_NOTIFICATIONS` | `configTASK_NOTIFICATION_ARRAY_ENTRIES > 1` | `configNUMBER_OF_CORES > 1` + `configUSE_CORE_AFFINITY` | `configUSE_MUTEXES` + `INCLUDE_uxTaskPriorityGet` | `INCLUDE_vTaskSuspend` | `INCLUDE_vTaskDelete` | `INCLUDE_vTaskPrioritySet` | `configUSE_TRACE_FACILITY` | `INCLUDE_eTaskGetState` | `INCLUDE_uxTaskGetStackHighWaterMark` | `INCLUDE_xTaskAbortDelay` | `configUSE_APPLICATION_TASK_TAG` |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| `task` | optional | optional | optional | optional | optional | optional | optional | recommended | optional | optional | optional | optional | optional | optional |
| `periodic_task` | optional | optional | optional | -- | optional | optional | optional | recommended | optional | optional | optional | optional | optional | optional |
| `static_task_allocator` | required | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
| `dynamic_task_allocator` | -- | required | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
| `sa::task` / `sa::periodic_task` | required | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
| `da::task` / `da::periodic_task` | -- | required | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |

### Synchronization Primitives

| Wrapper Class | `configUSE_MUTEXES` | `configUSE_RECURSIVE_MUTEXES` | `configUSE_COUNTING_SEMAPHORES` | `configSUPPORT_STATIC_ALLOCATION` | `configSUPPORT_DYNAMIC_ALLOCATION` | `FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS` |
|---|---|---|---|---|---|---|
| `binary_semaphore` | -- | -- | -- | optional | optional | optional |
| `counting_semaphore` | -- | -- | required | optional | optional | optional |
| `mutex` | required | -- | -- | optional | optional | optional |
| `recursive_mutex` | required | required | -- | optional | optional | optional |
| `lock_guard` | required | -- | -- | -- | -- | optional |
| `try_lock_guard` | required | -- | -- | -- | -- | optional |
| `timeout_lock_guard` | required | -- | -- | -- | -- | optional |
| `sa::binary_semaphore` | -- | -- | -- | required | -- | optional |
| `sa::counting_semaphore` | -- | -- | required | required | -- | optional |
| `sa::mutex` | required | -- | -- | required | -- | optional |
| `sa::recursive_mutex` | required | required | -- | required | -- | optional |
| `da::binary_semaphore` | -- | -- | -- | -- | required | optional |
| `da::counting_semaphore` | -- | -- | required | -- | required | optional |
| `da::mutex` | required | -- | -- | -- | required | optional |
| `da::recursive_mutex` | required | required | -- | -- | required | optional |

### Communication Primitives

| Wrapper Class | `configUSE_QUEUE_SETS` | `configUSE_STREAM_BUFFERS` | `configUSE_TIMERS` | `configSUPPORT_STATIC_ALLOCATION` | `configSUPPORT_DYNAMIC_ALLOCATION` |
|---|---|---|---|---|---|
| `queue` | -- | -- | -- | optional | optional |
| `queue_set` | required | -- | -- | -- | optional |
| `stream_buffer` | -- | required | -- | optional | optional |
| `message_buffer` | -- | required | -- | optional | optional |
| `event_group` | -- | -- | -- | optional | optional |
| `sw_timer` | -- | -- | required | optional | optional |
| `sa::queue` | -- | -- | -- | required | -- |
| `da::queue` | -- | -- | -- | -- | required |

### Feature Detection Macros (derived from FreeRTOS config)

The C++ wrappers use the following feature detection macros, derived automatically from `FreeRTOSConfig.h` by `freertos_config.hpp`:

| Wrapper Feature Macro | Derived From |
|---|---|
| `FREERTOS_CPP_WRAPPERS_HAS_MUTEX` | `configUSE_MUTEXES == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_RECURSIVE_MUTEX` | `configUSE_RECURSIVE_MUTEXES == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE` | `configUSE_COUNTING_SEMAPHORES == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_SW_TIMER` | `configUSE_TIMERS == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_TASK_NOTIFICATIONS` | `configUSE_TASK_NOTIFICATIONS == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_INDEXED_NOTIFICATIONS` | `configTASK_NOTIFICATION_ARRAY_ENTRIES > 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_STREAM_BUFFER` | `configUSE_STREAM_BUFFERS == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_EVENT_GROUP` | `configUSE_EVENT_GROUPS == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_QUEUE_SET` | `configUSE_QUEUE_SETS == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION` | `configSUPPORT_STATIC_ALLOCATION == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_DYNAMIC_ALLOCATION` | `configSUPPORT_DYNAMIC_ALLOCATION == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_SMP_AFFINITY` | `configNUMBER_OF_CORES > 1` and `configUSE_CORE_AFFINITY == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_TICKLESS_IDLE` | `configUSE_TICKLESS_IDLE != 0` |
| `FREERTOS_CPP_WRAPPERS_HAS_THREAD_SAFETY_ANNOTATIONS` | `FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS` defined and nonzero |

### Dependency Constraints

The following constraints are enforced at compile time by `freertos_config.hpp`:

- `configUSE_RECURSIVE_MUTEXES` requires `configUSE_MUTEXES` to be enabled.
- At least one of `configSUPPORT_STATIC_ALLOCATION` or `configSUPPORT_DYNAMIC_ALLOCATION` must be enabled.
- C++17 or later is required (`__cplusplus >= 201703L`).