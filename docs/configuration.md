# Configuration Reference

FreeRTOS C++ Wrappers v3.0.0

## Table of Contents

- [Overview](#overview)
- [C++ Standard Detection](#c-standard-detection)
- [FreeRTOS Feature Detection Macros](#freertos-feature-detection-macros)
- [Static Assert Constraints](#static-assert-constraints)
- [Build System Options](#build-system-options)
- [CMake Configuration Examples](#cmake-configuration-examples)
- [Thread Safety Annotations](#thread-safety-annotations)
- [Allocation Strategies](#allocation-strategies)
- [Namespace Aliases](#namespace-aliases)

---

## Overview

The FreeRTOS C++ Wrappers library uses compile-time feature detection macros to adapt its API to the configuration of the underlying FreeRTOS kernel. These macros are defined in `freertos_config.hpp` and are derived from the FreeRTOS `FreeRTOSConfig.h` settings. The library also provides build system options through CMake for static analysis, coverage, documentation, and simulation.

All feature macros are set to `1` when the corresponding FreeRTOS feature is enabled, and `0` otherwise. This allows conditional compilation using `#if FREERTOS_CPP_WRAPPERS_HAS_*` patterns.

---

## C++ Standard Detection

The library requires C++17 or later. Two macros detect the available C++ standard:

| Macro | Condition | Description |
|-------|-----------|-------------|
| `FREERTOS_CPP_WRAPPERS_CPP17` | `__cplusplus >= 201703L` | Set to `1` if C++17 or later is available. A `static_assert` fires if this evaluates to `0`. |
| `FREERTOS_CPP_WRAPPERS_CPP20` | `__cplusplus >= 202002L` | Set to `1` if C++20 or later is available. Used to select `std::expected` and `std::span` polyfill implementations. When C++20 is not available, fallback implementations are provided. |

Note: When compiling in C++20 mode, `FREERTOS_CPP_WRAPPERS_CPP17` is also set to `1`.

---

## FreeRTOS Feature Detection Macros

Each macro below is automatically set to `1` or `0` based on the corresponding FreeRTOS configuration option in `FreeRTOSConfig.h`. These macros control which C++ wrapper classes and methods are available.

| Macro | FreeRTOS Config | What It Enables |
|-------|----------------|-----------------|
| `FREERTOS_CPP_WRAPPERS_HAS_MUTEX` | `configUSE_MUTEXES == 1` | `freertos::mutex`, `lock_guard`, `try_lock_guard`, `timeout_lock_guard`, `lock_guard_isr` |
| `FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE` | `configUSE_COUNTING_SEMAPHORES == 1` | `freertos::counting_semaphore` |
| `FREERTOS_CPP_WRAPPERS_HAS_RECURSIVE_MUTEX` | `configUSE_RECURSIVE_MUTEXES == 1` | `freertos::recursive_mutex` (requires `FREERTOS_CPP_WRAPPERS_HAS_MUTEX`) |
| `FREERTOS_CPP_WRAPPERS_HAS_SW_TIMER` | `configUSE_TIMERS == 1` | `freertos::timer` |
| `FREERTOS_CPP_WRAPPERS_HAS_TASK_NOTIFICATIONS` | `configUSE_TASK_NOTIFICATIONS == 1` | Task notification API on `task` and `periodic_task` |
| `FREERTOS_CPP_WRAPPERS_HAS_INDEXED_NOTIFICATIONS` | `configTASK_NOTIFICATION_ARRAY_ENTRIES > 1` | Indexed notification method overloads (accepting an index parameter) |
| `FREERTOS_CPP_WRAPPERS_HAS_STREAM_BUFFER` | `configUSE_STREAM_BUFFERS == 1` | `freertos::stream_buffer`, `freertos::message_buffer` |
| `FREERTOS_CPP_WRAPPERS_HAS_EVENT_GROUP` | `configUSE_EVENT_GROUPS == 1` | `freertos::event_group` |
| `FREERTOS_CPP_WRAPPERS_HAS_QUEUE_SET` | `configUSE_QUEUE_SETS == 1` | `freertos::queue_set` |
| `FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION` | `configSUPPORT_STATIC_ALLOCATION == 1` | Static allocator classes and `sa::` namespace aliases |
| `FREERTOS_CPP_WRAPPERS_HAS_DYNAMIC_ALLOCATION` | `configSUPPORT_DYNAMIC_ALLOCATION == 1` | Dynamic allocator classes and `da::` namespace aliases |
| `FREERTOS_CPP_WRAPPERS_HAS_SMP_AFFINITY` | `configNUMBER_OF_CORES > 1 && configUSE_CORE_AFFINITY == 1` | `set_affinity()`, `clear_affinity()`, `affinity()`, `affinity_isr()` on task classes |
| `FREERTOS_CPP_WRAPPERS_HAS_TICKLESS_IDLE` | `configUSE_TICKLESS_IDLE != 0` | `catch_up_ticks()` free function |
| `FREERTOS_CPP_WRAPPERS_HAS_THREAD_SAFETY_ANNOTATIONS` | `FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS` defined and truthy | Enables Clang thread safety attribute macros |

---

## Static Assert Constraints

The library enforces the following constraints at compile time via `static_assert`:

| Constraint | Message |
|------------|---------|
| C++17 required | `"FreeRTOS C++ Wrappers require C++17 or later"` |
| Recursive mutex requires mutex | `"configUSE_RECURSIVE_MUTEXES requires configUSE_MUTEXES to be enabled"` |
| At least one allocation strategy | `"At least one of configSUPPORT_STATIC_ALLOCATION or configSUPPORT_DYNAMIC_ALLOCATION must be enabled"` |

If any of these constraints are violated, compilation will fail with a descriptive error message.

---

## Build System Options

The following CMake options control the build configuration:

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_COVERAGE` | OFF | Enable code coverage. Adds `--coverage`, `-fprofile-arcs`, and `-ftest-coverage` flags to the compiler and linker. Also disables exceptions (`-fno-exceptions`) for library code to reduce GCC coverage artifact branches. |
| `ENABLE_CLANG_TIDY` | ON | Enable clang-tidy static analysis for library modules. Runs checks from `cppcoreguidelines-*`, `cert-*`, `google-*`, and `hicpp-*`. |
| `CLANG_TIDY_WARNINGS_AS_ERRORS` | OFF | Treat clang-tidy warnings as errors during the build. Useful for CI pipelines. |
| `GENERATE_CLANG_TIDY_REPORT` | ON | Generate a clang-tidy static analysis report in `VnV/STATIC_ANALYSIS_REPORT.md`. Also includes MISRA and enhanced cppcheck analysis. |
| `ENABLE_MISRA_ANALYSIS` | ON | Enable MISRA C++ analysis via cppcheck. Integrated into the static analysis report when `GENERATE_CLANG_TIDY_REPORT` is also ON. |
| `GENERATE_DOCS` | ON | Enable Doxygen documentation generation. Creates a `docs` build target. |
| `ENABLE_SIMULATION` | OFF | Enable FreeRTOS+POSIX simulation system tests. Builds tests against the FreeRTOS+POSIX kernel port for host-based testing. |
| `CMAKE_CXX_STANDARD` | 17 | C++ standard version. C++17 is required; `CXX_STANDARD_REQUIRED` is ON. |

Additional build targets:

| Target | Description |
|--------|-------------|
| `format` | Formats all C++ source files using clang-format (requires clang-format). |
| `docs` | Generates Doxygen documentation (requires doxygen and `GENERATE_DOCS=ON`). |
| `static-analysis-report` | Generates the comprehensive static analysis report (requires clang-tidy and `GENERATE_CLANG_TIDY_REPORT=ON`). |
| `validation-verification-report` | Generates the combined V&V report in `VnV/` directory. |

---

## CMake Configuration Examples

```cmake
# Minimal configuration (no analysis, no docs)
cmake -DENABLE_CLANG_TIDY=OFF -DENABLE_MISRA_ANALYSIS=OFF ..

# Coverage configuration
cmake -DENABLE_COVERAGE=ON -DENABLE_CLANG_TIDY=OFF -DENABLE_MISRA_ANALYSIS=OFF ..

# Simulation configuration
cmake -DENABLE_SIMULATION=ON ..

# Full analysis with warnings-as-errors (CI)
cmake -DENABLE_CLANG_TIDY=ON -DCLANG_TIDY_WARNINGS_AS_ERRORS=ON ..

# Documentation-only build
cmake -DENABLE_CLANG_TIDY=OFF -DENABLE_MISRA_ANALYSIS=OFF -DGENERATE_DOCS=ON ..
```

---

## Thread Safety Annotations

### Overview

The `FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS` macro is a **user-defined** macro -- it is not auto-detected from `FreeRTOSConfig.h`. To enable thread safety annotations, define this macro to a truthy value (e.g., `1`) before including the library headers, typically via your build system:

```cmake
target_compile_definitions(my_target PRIVATE
    FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS=1
)
```

When enabled and compiling with **Clang**, the annotation macros expand to Clang thread safety attributes, enabling compile-time detection of potential data races and lock ordering violations. On non-Clang compilers, or when the macro is not defined, all annotation macros expand to nothing and have no effect.

### Annotation Macros

The following 16 annotation macros are defined in `freertos_thread_safety.hpp`:

| Macro | Clang Attribute | Purpose |
|-------|----------------|---------|
| `FREERTOS_CAPABILITY(...)` | `__attribute__((capability(...)))` | Declares a type as a capability (lock/mutex) |
| `FREERTOS_SCOPED_CAPABILITY` | `__attribute__((scoped_lockable))` | Declares a type as a scoped lockable object |
| `FREERTOS_GUARDED_BY(...)` | `__attribute__((guarded_by(...)))` | Declares that a variable is guarded by the named capability |
| `FREERTOS_PT_GUARDED_BY(...)` | `__attribute__((pt_guarded_by(...)))` | Declares that the data pointed to by a pointer is guarded |
| `FREERTOS_ACQUIRED_BY(...)` | `__attribute__((acquired_by(...)))` | Declares that a capability has been acquired by the annotated function |
| `FREERTOS_REQUIRES(...)` | `__attribute__((requires_capability(...)))` | Declares that the function requires exclusive access to the named capability |
| `FREERTOS_REQUIRES_SHARED(...)` | `__attribute__((requires_shared_capability(...)))` | Declares that the function requires shared (reader) access to the named capability |
| `FREERTOS_ACQUIRE(...)` | `__attribute__((acquire_capability(...)))` | Declares that the function acquires exclusive access to the named capability |
| `FREERTOS_ACQUIRE_SHARED(...)` | `__attribute__((acquire_shared_capability(...)))` | Declares that the function acquires shared access to the named capability |
| `FREERTOS_RELEASE(...)` | `__attribute__((release_capability(...)))` | Declares that the function releases exclusive access to the named capability |
| `FREERTOS_RELEASE_SHARED(...)` | `__attribute__((release_shared_capability(...)))` | Declares that the function releases shared access to the named capability |
| `FREERTOS_TRY_ACQUIRE(...)` | `__attribute__((try_acquire_capability(...)))` | Declares that the function attempts to acquire exclusive access; returns boolean |
| `FREERTOS_TRY_ACQUIRE_SHARED(...)` | `__attribute__((try_acquire_shared_capability(...)))` | Declares that the function attempts to acquire shared access; returns boolean |
| `FREERTOS_EXCLUDES(...)` | `__attribute__((locks_excluded(...)))` | Declares that the function does not acquire the named capability |
| `FREERTOS_ASSERT_CAPABILITY(...)` | `__attribute__((assert_capability(...)))` | Asserts that the caller already holds exclusive access |
| `FREERTOS_ASSERT_SHARED_CAPABILITY(...)` | `__attribute__((assert_shared_capability(...)))` | Asserts that the caller already holds shared access |
| `FREERTOS_RETURN_CAPABILITY(...)` | `__attribute__((lock_returned(...)))` | Declares that the function returns the named capability |
| `FREERTOS_NO_THREAD_SAFETY_ANALYSIS` | `__attribute__((no_thread_safety_analysis))` | Disables thread safety analysis for the annotated function |

---

## Allocation Strategies

The library supports three allocation strategies for FreeRTOS kernel objects. The choice of strategy determines how memory is obtained for the internal data structures (TCBs, queues, semaphores, etc.) required by the FreeRTOS kernel.

### 1. Static Allocation (`sa::` namespace)

Static allocation embeds the memory for kernel object data structures directly within the C++ wrapper object. This means:

- Memory is allocated at compile time as part of the object layout.
- No dynamic heap allocation occurs at runtime (no calls to `pvPortMalloc`/`vPortFree`).
- Objects can be placed in fixed memory regions suitable for safety-critical and deterministic systems.
- Requires `configSUPPORT_STATIC_ALLOCATION == 1` in `FreeRTOSConfig.h`.

Usage example:

```cpp
freertos::sa::mutex my_mutex;
freertos::sa::queue<10, int> my_queue;
freertos::sa::task<256> my_task;
```

### 2. Dynamic Allocation (`da::` namespace)

Dynamic allocation uses the FreeRTOS heap (`pvPortMalloc`/`vPortFree`) to allocate memory for kernel object data structures at runtime. This means:

- Memory is allocated from the FreeRTOS heap when the object is constructed.
- Memory is returned to the FreeRTOS heap when the object is destructed.
- Requires `configSUPPORT_DYNAMIC_ALLOCATION == 1` in `FreeRTOSConfig.h`.
- Simpler to use when deterministic memory layout is not required.

Usage example:

```cpp
freertos::da::mutex my_mutex;
freertos::da::queue<10, int> my_queue;
freertos::da::task<256> my_task;
```

### 3. External Allocation (`ea::` namespace)

External allocation allows user-provided memory regions to supply memory for kernel object data structures. This means:

- Memory is allocated and freed through function pointers provided by a user-defined `external_memory_region` struct.
- Requires `configSUPPORT_STATIC_ALLOCATION == 1` in `FreeRTOSConfig.h` (external allocators use the static FreeRTOS API internally).
- Provides full control over memory placement, enabling use cases such as external RAM, memory pools, or custom arena allocators.

The `external_memory_region` struct is defined as:

```cpp
struct external_memory_region {
    void *(*allocate)(size_t size);
    void (*deallocate)(void *ptr);
};
```

Usage example:

```cpp
struct my_region {
    static void *allocate(size_t size) { return my_pool_alloc(size); }
    static void deallocate(void *ptr) { my_pool_free(ptr); }
};

freertos::ea::mutex<my_region> my_mutex;
freertos::ea::queue<my_region, 10, int> my_queue;
freertos::ea::task<my_region, 256> my_task;
```

---

## Namespace Aliases

The library provides convenience type aliases in three sub-namespaces of `freertos` to simplify instantiation of kernel objects with each allocation strategy.

### `freertos::sa` -- Static Allocation Aliases

Available when `configSUPPORT_STATIC_ALLOCATION == 1`:

| Alias | Underlying Type | Header |
|-------|----------------|--------|
| `freertos::sa::binary_semaphore` | `freertos::binary_semaphore<freertos::static_semaphore_allocator>` | `freertos_semaphore.hpp` |
| `freertos::sa::counting_semaphore` | `freertos::counting_semaphore<freertos::static_semaphore_allocator>` | `freertos_semaphore.hpp` |
| `freertos::sa::mutex` | `freertos::mutex<freertos::static_semaphore_allocator>` | `freertos_semaphore.hpp` |
| `freertos::sa::recursive_mutex` | `freertos::recursive_mutex<freertos::static_semaphore_allocator>` | `freertos_semaphore.hpp` |
| `freertos::sa::queue<N, T>` | `freertos::queue<N, T, freertos::static_queue_allocator<N, T>>` | `freertos_queue.hpp` |
| `freertos::sa::event_group` | `freertos::event_group<freertos::static_event_group_allocator>` | `freertos_event_group.hpp` |
| `freertos::sa::timer` | `freertos::timer<freertos::static_sw_timer_allocator>` | `freertos_sw_timer.hpp` |
| `freertos::sa::stream_buffer<N>` | `freertos::stream_buffer<N, freertos::static_stream_buffer_allocator<N>>` | `freertos_stream_buffer.hpp` |
| `freertos::sa::message_buffer<N>` | `freertos::message_buffer<N, freertos::static_message_buffer_allocator<N>>` | `freertos_message_buffer.hpp` |
| `freertos::sa::task<S>` | `freertos::task<freertos::static_task_allocator<S>>` | `freertos_task.hpp` |
| `freertos::sa::periodic_task<S>` | `freertos::periodic_task<freertos::static_task_allocator<S>>` | `freertos_task.hpp` |

### `freertos::da` -- Dynamic Allocation Aliases

Available when `configSUPPORT_DYNAMIC_ALLOCATION == 1`:

| Alias | Underlying Type | Header |
|-------|----------------|--------|
| `freertos::da::binary_semaphore` | `freertos::binary_semaphore<freertos::dynamic_semaphore_allocator>` | `freertos_semaphore.hpp` |
| `freertos::da::counting_semaphore` | `freertos::counting_semaphore<freertos::dynamic_semaphore_allocator>` | `freertos_semaphore.hpp` |
| `freertos::da::mutex` | `freertos::mutex<freertos::dynamic_semaphore_allocator>` | `freertos_semaphore.hpp` |
| `freertos::da::recursive_mutex` | `freertos::recursive_mutex<freertos::dynamic_semaphore_allocator>` | `freertos_semaphore.hpp` |
| `freertos::da::queue<N, T>` | `freertos::queue<N, T, freertos::dynamic_queue_allocator<N, T>>` | `freertos_queue.hpp` |
| `freertos::da::event_group` | `freertos::event_group<freertos::dynamic_event_group_allocator>` | `freertos_event_group.hpp` |
| `freertos::da::timer` | `freertos::timer<freertos::dynamic_sw_timer_allocator>` | `freertos_sw_timer.hpp` |
| `freertos::da::stream_buffer<N>` | `freertos::stream_buffer<N, freertos::dynamic_stream_buffer_allocator<N>>` | `freertos_stream_buffer.hpp` |
| `freertos::da::message_buffer<N>` | `freertos::message_buffer<N, freertos::dynamic_message_buffer_allocator<N>>` | `freertos_message_buffer.hpp` |
| `freertos::da::queue_set` | `freertos::queue_set<freertos::dynamic_queue_set_allocator>` | `freertos_queue_set.hpp` |
| `freertos::da::task<S>` | `freertos::task<freertos::dynamic_task_allocator<S>>` | `freertos_task.hpp` |
| `freertos::da::periodic_task<S>` | `freertos::periodic_task<freertos::dynamic_task_allocator<S>>` | `freertos_task.hpp` |

### `freertos::ea` -- External Allocation Aliases

Available when `configSUPPORT_STATIC_ALLOCATION == 1` (external allocators use the static FreeRTOS API):

| Alias | Underlying Type | Header |
|-------|----------------|--------|
| `freertos::ea::binary_semaphore<Region>` | `freertos::binary_semaphore<freertos::external_semaphore_allocator<Region>>` | `freertos.hpp` |
| `freertos::ea::counting_semaphore<Region>` | `freertos::counting_semaphore<freertos::external_semaphore_allocator<Region>>` | `freertos.hpp` |
| `freertos::ea::mutex<Region>` | `freertos::mutex<freertos::external_semaphore_allocator<Region>>` | `freertos.hpp` |
| `freertos::ea::recursive_mutex<Region>` | `freertos::recursive_mutex<freertos::external_semaphore_allocator<Region>>` | `freertos.hpp` |
| `freertos::ea::queue<Region, N, T>` | `freertos::queue<N, T, freertos::external_queue_allocator<Region, N, T>>` | `freertos.hpp` |
| `freertos::ea::event_group<Region>` | `freertos::event_group<freertos::external_event_group_allocator<Region>>` | `freertos.hpp` |
| `freertos::ea::timer<Region>` | `freertos::timer<freertos::external_sw_timer_allocator<Region>>` | `freertos.hpp` |
| `freertos::ea::stream_buffer<Region, N>` | `freertos::stream_buffer<N, freertos::external_stream_buffer_allocator<Region, N>>` | `freertos.hpp` |
| `freertos::ea::message_buffer<Region, N>` | `freertos::message_buffer<N, freertos::external_message_buffer_allocator<Region, N>>` | `freertos.hpp` |
| `freertos::ea::task<Region, S>` | `freertos::task<freertos::external_task_allocator<Region, S>>` | `freertos.hpp` |
| `freertos::ea::periodic_task<Region, S>` | `freertos::periodic_task<freertos::external_task_allocator<Region, S>>` | `freertos.hpp` |

Note: The `queue_set` type only has a dynamic allocation alias (`freertos::da::queue_set`). There is no static or external allocation alias for queue sets, as the FreeRTOS API for queue sets only supports dynamic creation.