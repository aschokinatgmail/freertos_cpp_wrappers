# Migration Guide: v1.0.2 to v3.0.0

This guide covers migrating from FreeRTOS C++ Wrappers v1.0.2 to v3.0.0. The library
skipped the v2.x release version; the changes between v1.0.2 and v3.0.0 are substantial
and include breaking API changes alongside new features.

---

## Quick Start Migration Checklist

- [ ] Upgrade your compiler and build system to support C++17
- [ ] Set `CMAKE_CXX_STANDARD=17` (REQUIRED) in your CMakeLists.txt
- [ ] Update all calls that pass raw `UBaseType_t`/`TickType_t`/`EventBits_t` to APIs
      now expecting `freertos::priority`, `freertos::tick_count`, or
      `freertos::event_bits` — use explicit construction (e.g. `freertos::priority(2)`)
- [ ] Update ISR call sites that previously consumed raw `BaseType_t` to use
      `isr_result<T>` fields (`.result` and `.higher_priority_task_woken`)
- [ ] Remove any discarded return values from `try_*` methods (they are now
      `[[nodiscard]]`)
- [ ] Resolve any ambiguity between `freertos::tick_count` (the class) and
      `freertos::tick_count()` (the free function) via namespace qualification
- [ ] Delete copy construction/assignment usage on wrapper objects (now deleted;
      use move instead)
- [ ] Add `#include "freertos_config.hpp"` where you need feature-detection macros
- [ ] Decide whether to adopt the new `*_ex()` error-handling API in new code

---

## Breaking Changes

### 1. C++17 Is Required

The library now requires a C++17-capable compiler. `freertos_config.hpp` emits a
`static_assert` failure if `__cplusplus < 201703L`.

**Before (v1.0.2):**

```cmake
set(CMAKE_CXX_STANDARD 14)
```

**After (v3.0.0):**

```cmake
set(CMAKE_CXX_STANDARD 17 REQUIRED)
```

If your project targets C++14 or earlier, you must upgrade your toolchain before
using this version of the library.

### 2. Strong Type Aliases Replace Raw Integers

Four new wrapper types are introduced in `freertos_strong_types.hpp`:

| Type              | Wraps           | Where Used                            |
|-------------------|-----------------|---------------------------------------|
| `freertos::priority`           | `UBaseType_t`   | task constructors, `set_priority()`  |
| `freertos::event_bits`         | `EventBits_t`   | event group operations                |
| `freertos::tick_count`         | `TickType_t`    | timeout parameters, tick queries     |
| `freertos::core_affinity_mask` | `UBaseType_t`   | SMP affinity methods                  |

All constructors are `explicit`. Code that previously passed a raw integer will no
longer compile.

**Before (v1.0.2):**

```cpp
freertos::task<512> my_task("worker", 3, func);
my_task.set_priority(5);
```

**After (v3.0.0):**

```cpp
freertos::task<512> my_task("worker", freertos::priority(3), func);
my_task.set_priority(freertos::priority(5));
```

The same pattern applies to `freertos::event_bits` and `freertos::tick_count`:

```cpp
// Before
auto bits = eg.wait_bits(0x01, pdTRUE, pdTRUE, 100);

// After
auto bits = eg.wait_bits(freertos::event_bits(0x01), pdTRUE, pdTRUE,
                         freertos::tick_count(100));
```

### 3. `freertos::tick_count` Class Shadows the Free Function

The free function `freertos::tick_count()` (which returns the current tick count) now
shares its name with the `freertos::tick_count` strong-type class. If you call the free
function unqualified in a context where the class type is visible, the compiler may
interpret the call as a type name.

**Resolution:** Use explicit qualification:

```cpp
auto now = freertos::tick_count();  // May be ambiguous
auto now = ::freertos::tick_count(); // Calls the free function explicitly
```

Or store the result through the task API instead.

### 4. ISR Methods Now Return `isr_result<T>`

All ISR-safe methods (`give_isr()`, `take_isr()`, `send_isr()`, `receive_isr()`,
`lock_isr()`, `unlock_isr()`, etc.) previously returned `BaseType_t`. They now return
`freertos::isr_result<T>`, which contains both the operation result and the
`higher_priority_task_woken` flag.

**Before (v1.0.2):**

```cpp
BaseType_t task_woken = pdFALSE;
BaseType_t result = sem.give_isr(task_woken);
portYIELD_FROM_ISR(task_woken);
```

**After (v3.0.0):**

```cpp
auto result = sem.give_isr();
portYIELD_FROM_ISR(result.higher_priority_task_woken);
if (result.result == pdTRUE) { /* success */ }
```

### 5. All `try_*` Methods Are `[[nodiscard]]`

Methods like `try_acquire()`, `try_lock()`, `try_lock_for()` are now marked
`[[nodiscard]]`. Discarding their return value will produce a compiler warning
(or error with `-Werror`). You must check the return value.

**Before (v1.0.2):**

```cpp
mtx.try_lock();  // result ignored — compiled without warning
```

**After (v3.0.0):**

```cpp
if (mtx.try_lock()) {
    // acquired
} else {
    // not acquired
}
```

### 6. Copy Operations Are Deleted

All wrapper classes (semaphores, mutexes, queues, tasks, timers, event groups, stream
and message buffers) now delete copy construction and copy assignment. Move construction,
move assignment, and `swap()` are supported instead.

If you were copying a wrapper object, replace with move semantics or a reference:

```cpp
// Before (v1.0.2) — may have compiled depending on version
auto sem_copy = sem;

// After (v3.0.0)
auto sem_moved = std::move(sem);
// Or use a reference
freertos::binary_semaphore<>& sem_ref = sem;
```

---

## New Features

### Dual API: FreeRTOS Names and C++ Standard Names

Semaphore and mutex classes now provide C++-idiomatic method names alongside the
existing FreeRTOS-style names:

| FreeRTOS Name     | C++ Standard Name |
|-------------------|-------------------|
| `binary_semaphore::take()`    | `binary_semaphore::acquire()` |
| `binary_semaphore::try_acquire()` / `take(0)` | `binary_semaphore::try_acquire()` |
| `binary_semaphore::give()`    | `binary_semaphore::release()`  |
| `mutex::lock()`    | (already existed)  |
| `mutex::try_lock()` | (already existed)  |
| `mutex::try_lock_for()`  | (already existed)  |
| `mutex::try_lock_until()` | (already existed)  |
| `mutex::unlock()`  | (already existed)  |

No existing methods were removed. Both naming styles are available.

### Expected API (`*_ex()` Methods)

New methods returning `freertos::expected<T, freertos::error>` instead of raw
return codes:

- `binary_semaphore`: `give_ex()`, `take_ex()`, `give_ex_isr()`, `take_ex_isr()`
- `counting_semaphore`: `give_ex()`, `take_ex()`, `give_ex_isr()`, `take_ex_isr()`
- `mutex`: `lock_ex()`, `try_lock_ex()`, `unlock_ex()`, `lock_ex_isr()`,
  `unlock_ex_isr()`
- `queue`: `send_ex()`, `receive_ex()`, `send_ex_isr()`, `receive_ex_isr()`
- `task`: `start_ex()`, `stop_ex()`
- And corresponding methods on other wrapper classes

The `freertos::error` enum provides structured error codes: `ok`, `timeout`,
`would_block`, `queue_full`, `queue_empty`, `semaphore_not_owned`,
`invalid_handle`, `out_of_memory`, `invalid_parameter`.

ISR variants return `isr_result<expected<T, error>>`.

Example:

```cpp
auto result = sem.take_ex(freertos::tick_count(100));
if (result.has_value()) {
    // success
} else {
    switch (result.error()) {
    case freertos::error::timeout:   break;
    case freertos::error::would_block: break;
    default: break;
    }
}
```

### Thread Safety Annotations

Optional Clang thread safety analysis is available. Enable by defining
`FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS` to `1` before including the
library headers. On non-Clang compilers, all annotation macros expand to nothing
and impose zero runtime cost.

```cpp
#define FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS 1
#include "freertos.hpp"
```

Annotations include: `FREERTOS_CAPABILITY`, `FREERTOS_GUARDED_BY`,
`FREERTOS_ACQUIRE`, `FREERTOS_RELEASE`, `FREERTOS_SCOPED_CAPABILITY`,
`FREERTOS_REQUIRES`, `FREERTOS_EXCLUDES`, and others. Full list is in
`freertos_thread_safety.hpp`.

### Allocator Pattern

All wrapper classes now accept a template allocator parameter:

```cpp
template <typename Allocator = dynamic_semaphore_allocator>
class binary_semaphore;

template <typename Allocator = dynamic_task_allocator<StackSize>>
class task;
```

Backwards-compatible: the default allocator is the dynamic allocator, so existing
code using `freertos::binary_semaphore<>` or `freertos::binary_semaphore` continues
to work.

Convenience namespaces are provided:

| Namespace | Allocation Strategy      | Header Alias Example                         |
|-----------|--------------------------|----------------------------------------------|
| `freertos::sa::` | Static (stack/bss) | `sa::binary_semaphore` |
| `freertos::da::` | Dynamic (heap)     | `da::binary_semaphore`  |
| `freertos::ea::` | External (user-supplied memory) | `ea::binary_semaphore<YourRegion>` |

### External Allocator (`ea::` Namespace)

A new allocation strategy allows you to supply your own memory region through the
`freertos::external_memory_region` struct:

```cpp
struct external_memory_region {
    void *(*allocate)(size_t size);
    void (*deallocate)(void *ptr);
};
```

Use it with the `ea::` namespace aliases:

```cpp
freertos::external_memory_region my_region{my_alloc, my_free};
freertos::ea::binary_semaphore<freertos::external_memory_region> sem(my_region);
```

This requires `configSUPPORT_STATIC_ALLOCATION=1` in your FreeRTOS configuration.

### Feature Detection Macros

`freertos_config.hpp` defines `FREERTOS_CPP_WRAPPERS_HAS_*` macros that automatically
detect FreeRTOS configuration. Wrapper classes for disabled features are conditionally
compiled out.

Key macros:

| Macro                                      | Enabled When                                |
|--------------------------------------------|---------------------------------------------|
| `FREERTOS_CPP_WRAPPERS_HAS_MUTEX`          | `configUSE_MUTEXES == 1`                    |
| `FREERTOS_CPP_WRAPPERS_HAS_COUNTING_SEMAPHORE` | `configUSE_COUNTING_SEMAPHORES == 1`   |
| `FREERTOS_CPP_WRAPPERS_HAS_RECURSIVE_MUTEX` | `configUSE_RECURSIVE_MUTEXES == 1`        |
| `FREERTOS_CPP_WRAPPERS_HAS_SW_TIMER`       | `configUSE_TIMERS == 1`                     |
| `FREERTOS_CPP_WRAPPERS_HAS_TASK_NOTIFICATIONS` | `configUSE_TASK_NOTIFICATIONS == 1`      |
| `FREERTOS_CPP_WRAPPERS_HAS_INDEXED_NOTIFICATIONS` | `configTASK_NOTIFICATION_ARRAY_ENTRIES > 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_STREAM_BUFFER`  | `configUSE_STREAM_BUFFERS == 1`             |
| `FREERTOS_CPP_WRAPPERS_HAS_EVENT_GROUP`    | `configUSE_EVENT_GROUPS == 1`               |
| `FREERTOS_CPP_WRAPPERS_HAS_QUEUE_SET`      | `configUSE_QUEUE_SETS == 1`                 |
| `FREERTOS_CPP_WRAPPERS_HAS_STATIC_ALLOCATION` | `configSUPPORT_STATIC_ALLOCATION == 1`  |
| `FREERTOS_CPP_WRAPPERS_HAS_DYNAMIC_ALLOCATION` | `configSUPPORT_DYNAMIC_ALLOCATION == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_SMP_AFFINITY`  | `configNUMBER_OF_CORES > 1 && configUSE_CORE_AFFINITY == 1` |
| `FREERTOS_CPP_WRAPPERS_HAS_TICKLESS_IDLE`  | `configUSE_TICKLESS_IDLE != 0`             |

### Queue Sets

A new `freertos::queue_set` class is available when `configUSE_QUEUE_SETS=1`:

```cpp
freertos::queue_set set(3);
set.add(queue1.get_handle());
set.add(sem.get_handle());

auto activated = set.select(portMAX_DELAY);
```

### Indexed Task Notifications

When `configTASK_NOTIFICATION_ARRAY_ENTRIES > 1`, overloaded notification methods
accept an `UBaseType_t index` parameter:

```cpp
task.notify(index, value, eAction);
task.notify_take(index, clear_on_exit, timeout);
task.notify_wait(index, entry_bits, exit_bits, clear_on_exit, timeout);
```

### SMP Core Affinity

When `configNUMBER_OF_CORES > 1 && configUSE_CORE_AFFINITY == 1`, task classes gain:

```cpp
void set_affinity(freertos::core_affinity_mask mask);
freertos::core_affinity_mask affinity() const;
void clear_affinity();
// ISR variants
isr_result<freertos::core_affinity_mask> affinity_isr() const;
```

### New Free Functions

```cpp
bool freertos::is_isr();                        // Replaces manual portCHECK_IF_IN_ISR()
void freertos::catch_up_ticks(TickType_t);     // Available when tickless idle enabled
TaskHandle_t freertos::timer_daemon_task_handle(); // Available when timers enabled
UBaseType_t freertos::base_priority();          // Returns the current base priority
```

### New RAII Guards

Four new scoped guard classes for critical sections and barriers:

| Guard Class                    | Enters                          | Exits                      |
|--------------------------------|---------------------------------|----------------------------|
| `freertos::critical_section`   | `taskENTER_CRITICAL()`          | `taskEXIT_CRITICAL()`      |
| `freertos::critical_section_isr` | Enters/leaves critical from ISR | `taskEXIT_CRITICAL_FROM_ISR()` |
| `freertos::interrupt_barrier`  | `taskDISABLE_INTERRUPTS()`      | `taskENABLE_INTERRUPTS()`  |
| `freertos::scheduler_barrier`  | `vTaskSuspendAll()`             | `xTaskResumeAll()`          |

Usage:

```cpp
{
    freertos::critical_section cs;
    // critical section code
} // automatically exits critical section
```

### Polyfills: `freertos::expected` and `freertos::span`

- `freertos::expected<T, E>`: A C++17 polyfill for `std::expected<T, E>`. When
  compiled with C++20 (`__cpp_lib_expected >= 202211L`), it aliases to
  `std::expected<T, E>`.
- `freertos::span<T>`: A C++17 polyfill for `std::span<T>`. When compiled with
  C++20 (`__cpp_lib_span >= 202002L`), it aliases to `std::span<T>`.

### Move Semantics

All wrapper classes now support move construction, move assignment, and `swap()`.
Copy construction and copy assignment are deleted. This enables transferring
ownership of FreeRTOS handles without resource leaks or double-deletion:

```cpp
freertos::binary_semaphore<> sem1;
freertos::binary_semaphore<> sem2 = std::move(sem1);
// sem1 is now in a valid-but-empty state (handle is nullptr)
```

---

## API Compatibility

### Unchanged (Backwards Compatible)

The following APIs remain identical in signature and behavior:

- `binary_semaphore::give()`, `take()` (and chrono overloads)
- `counting_semaphore::give()`, `take()` (and chrono overloads)
- `mutex::lock()`, `unlock()`, `try_lock()`, `try_lock_for()`, `try_lock_until()`
- `recursive_mutex::lock()`, `unlock()`, `try_lock()`
- `queue::send()`, `receive()`, `send_isr()`, `receive_isr()`
- `event_group::set_bits()`, `wait_bits()`, `clear_bits()`
- `sw_timer::start()`, `stop()`, `reset()`
- `stream_buffer::send()`, `receive()`
- `message_buffer::send()`, `receive()`
- All `*_isr()` method names (return type changed to `isr_result<T>`)
- All constructor signatures (allocator template parameter has a default)

The `da::` and `sa::` convenience type aliases remain available.

### Changed

| v1.0.2                                    | v3.0.0                                                       | Notes                     |
|-------------------------------------------|--------------------------------------------------------------|---------------------------|
| Raw `UBaseType_t` for priorities           | `freertos::priority` (explicit constructor)                 | Compile error if not wrapped |
| Raw `TickType_t` for tick counts          | `freertos::tick_count` (explicit constructor)               | Compile error if not wrapped |
| Raw `EventBits_t` for event bits          | `freertos::event_bits` (explicit constructor)               | Compile error if not wrapped |
| `BaseType_t` return from ISR methods      | `isr_result<T>` return type                                  | Must access `.result` and `.higher_priority_task_woken` |
| Discardable `try_*` return values         | `[[nodiscard]]` on all `try_*` methods                      | Compile warning/error if discarded |
| Copyable wrapper objects                  | Move-only; copy deleted                                      | Use `std::move()` or references |
| `freertos::tick_count()` free function    | Name collision with `freertos::tick_count` class            | Use `::freertos::tick_count()` to call the free function |
| ISR methods take `BaseType_t&` param      | ISR methods return `isr_result<T>` with no out-parameters   | See Breaking Change #4    |

### Added (No Removals)

- `acquire()` / `try_acquire()` / `release()` on `binary_semaphore`
- `*_ex()` and `*_ex_isr()` methods returning `expected<T, error>`
- `freertos::queue_set` class
- `freertos::critical_section`, `critical_section_isr`, `interrupt_barrier`,
  `scheduler_barrier` RAII guards
- `freertos::is_isr()`, `catch_up_ticks()`, `timer_daemon_task_handle()`,
  `base_priority()` free functions
- SMP affinity methods when applicable
- Indexed notification overloads when applicable
- Thread safety annotation macros
- `freertos::expected<T,E>` polyfill
- `freertos::span<T>` polyfill
- `ea::` namespace for external allocators

---

## Step-by-Step Migration

### Step 1: Upgrade Build System to C++17

Update your `CMakeLists.txt` (or equivalent build configuration):

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

Verify your compiler supports C++17. GCC >= 7, Clang >= 5, and MSVC >= 19.14
(all with appropriate standard library support) are known to work.

### Step 2: Update Package Reference

Update your dependency reference from v1.0.2 to v3.0.0 (whether via
`FetchContent`, `find_package`, git submodule, or manual copy).

### Step 3: Fix Strong Type Errors

Compile the project. The compiler will flag every location where a raw integer is
passed to an API expecting a strong type. Wrap each value:

```cpp
// Priority
freertos::priority(2)

// Tick count / timeout
freertos::tick_count(100)
freertos::tick_count(portMAX_DELAY)

// Event bits
freertos::event_bits(0x01 | 0x02)

// Core affinity mask
freertos::core_affinity_mask(0x03)
// Or use the static factory:
freertos::core_affinity_mask::core(0) | freertos::core_affinity_mask::core(1)
```

### Step 4: Fix ISR Return Type Handling

Update ISR call sites. Replace `BaseType_t` output parameters with `isr_result<T>`
field access:

```cpp
// Before
BaseType_t woken = pdFALSE;
BaseType_t result = sem.give_isr(woken);
portYIELD_FROM_ISR(woken);

// After
auto result = sem.give_isr();
portYIELD_FROM_ISR(result.higher_priority_task_woken);
```

Do the same for `take_isr()`, `lock_isr()`, `unlock_isr()`, `send_isr()`,
`receive_isr()`, and all other `*_isr()` methods.

### Step 5: Fix `[[nodiscard]]` Warnings

Address every `[[nodiscard]]` warning. The simplest approach is to check the
return value:

```cpp
// Before
mtx.try_lock();  // discarded

// After - option A: check the result
if (mtx.try_lock()) { /* acquired */ }

// After - option B: use the _ex() variant for richer error info
auto result = mtx.try_lock_ex();
if (result.has_value()) { /* acquired */ }
```

### Step 6: Update `freertos::tick_count` Call Sites

If you call the `freertos::tick_count()` free function, disambiguate from the
`freertos::tick_count` class:

```cpp
// If ambiguous:
auto t = freertos::tick_count();  // may resolve to the type
// Use:
auto t = ::freertos::tick_count();  // calls the free function
```

### Step 7: Replace Copy Usage with Move or References

If you were copying wrapper objects, replace with move semantics or references:

```cpp
// Before — copy (now deleted)
freertos::binary_semaphore<> copy = original;

// After — move
freertos::binary_semaphore<> moved = std::move(original);

// After — reference
freertos::binary_semaphore<>& ref = original;
```

### Step 8: (Optional) Adopt New Features

In new code or when refactoring, consider adopting:

- The `*_ex()` expected-based API for richer error handling
- The C++ standard naming style (`acquire`/`release` vs `take`/`give`)
- RAII guards (`critical_section`, `scheduler_barrier`, etc.)
- External allocators (`ea::`) for custom memory management
- Thread safety annotations for Clang-based static analysis
- `freertos::span` for buffer manipulation in stream/message buffers

---

## Common Migration Issues

### Q: My code passes `2` as a task priority and now it fails to compile.

The `priority` constructor is `explicit`. You must wrap the value:

```cpp
// Error: implicit conversion
freertos::task<512> t("worker", 2, func);

// Fix: explicit construction
freertos::task<512> t("worker", freertos::priority(2), func);
```

### Q: My ISR handler no longer compiles — it expects a `BaseType_t&` parameter.

ISR methods no longer accept an output parameter for `higher_priority_task_woken`.
They return `isr_result<T>` instead. See [Step 4](#step-4-fix-isr-return-type-handling).

### Q: I get a `static_assert` about C++17.

The library requires C++17. Update your build system to set the standard to 17 or
higher with the REQUIRED flag. See [Step 1](#step-1-upgrade-build-system-to-c17).

### Q: I get `[[nodiscard]]` warnings or errors on `try_lock()` / `try_acquire()`.

All `try_*` methods now have `[[nodiscard]]`. You must use the return value. Assign it
to a variable or check it in a conditional. See [Step 5](#step-5-fix-nodiscard-warnings).

### Q: I get a compile error about a deleted copy constructor on wrapper objects.

All wrapper types are now move-only. Use `std::move()` to transfer ownership, or
pass by reference. See [Step 7](#step-7-replace-copy-usage-with-move-or-references).

### Q: The `sa::` and `da::` type aliases I was using still compile — should I change them?

No. The `sa::` and `da::` convenience aliases remain unchanged and backwards
compatible. They alias to the same template instantiations as before.

### Q: I call `freertos::tick_count()` to get the current tick count, but the compiler says it is a type.

The `freertos::tick_count` class shadows the free function of the same name. Use
namespace-qualified syntax: `::freertos::tick_count()`. Alternatively, call
`xTaskGetTickCount()` directly if you need the raw value.

### Q: My `configUSE_MUTEXES` or `configUSE_TIMERS` is disabled and a class is missing.

The library now uses feature detection macros (`FREERTOS_CPP_WRAPPERS_HAS_*`) to
conditionally compile wrapper classes. If a FreeRTOS feature is disabled in your
`FreeRTOSConfig.h`, the corresponding C++ wrapper class will not be available. This
is intentional and ensures compile-time consistency with your FreeRTOS configuration.

### Q: I want to use thread safety annotations but I am not using Clang.

Thread safety annotations are a no-op on non-Clang compilers. You can safely define
`FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS` regardless of your compiler. The
macros will expand to nothing on GCC, MSVC, etc. No runtime cost is incurred.

### Q: How do I use the `expected` API in ISR context?

ISR variants of `*_ex()` methods return `isr_result<expected<T, error>>`. Access the
error state through two levels:

```cpp
auto result = sem.take_ex_isr();
if (result.result.has_value()) {
    // success — result.result.value() contains the value
} else {
    // failure — result.result.error() contains the error code
}
portYIELD_FROM_ISR(result.higher_priority_task_woken);
```

### Q: Can I use `freertos::expected` in my own code?

Yes. `freertos::expected<T, E>` is a C++17-compatible polyfill. When compiled under
C++20 with a conforming standard library, it transparently aliases to `std::expected`.
You can use it in application code without depending on C++20.

### Q: I was using `freertos::binary_semaphore` without template arguments. Does this still work?

Yes. The default template argument is `dynamic_semaphore_allocator`. The following
are equivalent:

```cpp
freertos::binary_semaphore<> sem;     // explicit default
freertos::binary_semaphore sem;       // implicit default (C++17)
```

Both create a dynamically-allocated binary semaphore.

### Q: Where is `freertos::span` useful?

`freertos::span<T>` is available as a C++17 polyfill for `std::span<T>`. It is useful
for passing buffer views to stream and message buffer operations without exposing raw
pointer + size pairs. Under C++20, it aliases `std::span<T>` for zero overhead.