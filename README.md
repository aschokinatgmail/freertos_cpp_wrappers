# C++ Wrappers for FreeRTOS

[![Tests](https://img.shields.io/badge/Tests-592%20passing-brightgreen)]()
[![Coverage](https://img.shields.io/badge/Coverage-95%25-brightgreen)]()
[![MISRA C 2012](https://img.shields.io/badge/MISRA%20C++-Compliant-blue)]()
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)]()
[![License](https://img.shields.io/badge/License-MIT-yellow)]()
[![Platform](https://img.shields.io/badge/Platform-FreeRTOS-orange)]()

Lightweight collection of FreeRTOS API wrappers compliant to modern C++ standards

## Versioning

This project follows [Semantic Versioning](https://semver.org/). The version is defined in `CMakeLists.txt` and is the single source of truth.

| Macro | Description |
|---|---|
| `FREERTOS_CPP_WRAPPERS_VERSION_MAJOR` | Major version (breaking changes) |
| `FREERTOS_CPP_WRAPPERS_VERSION_MINOR` | Minor version (new features) |
| `FREERTOS_CPP_WRAPPERS_VERSION_PATCH` | Patch version (bug fixes) |
| `FREERTOS_CPP_WRAPPERS_VERSION` | Full version string (e.g. `"3.0.0"`) |

Include `<freertos.hpp>` to access version macros at compile time.

## Why Use This Library?

| Feature | Benefit |
|---------|---------|
| **Safety-Critical Ready** | MISRA 2012 C compliant, suitable for functionally safe applications |
| **Modern C++17** | RAII wrappers with move semantics, no raw pointers |
| **Production Tested** | 592+ tests, 95% branch coverage, used in commercial BMS systems |
| **Zero Overhead** | Thin wrappers compile to efficient FreeRTOS calls |
| **Dual API** | FreeRTOS-native and std-compatible interfaces |
| **expected\<T,E\> Error Handling** | Type-safe error handling replacing error codes |
| **Strong Typedefs** | Prevent mixing primitive types at compile time |
| **Comprehensive** | Tasks, Semaphores, Queues, Event Groups, Stream/Message Buffers, Timers, Queue Sets |
| **Well Documented** | Doxygen API reference with examples and tutorials |

### Comparison with Alternatives

| Feature | This Library | Raw FreeRTOS | Other Wrappers |
|---------|--------------|--------------|-----------------|
| RAII Support | ✅ | ❌ | Varies |
| MISRA C 2012 Rules Compliant | ✅ | N/A | ❌ |
| expected\<T,E\> Error Handling | ✅ | ❌ | ❌ |
| Strong Typedefs | ✅ | ❌ | ❌ |
| Thread Safety Annotations | ✅ | ❌ | ❌ |
| Test Coverage | 95% branch | N/A | Unknown |
| C++17 Features | ✅ | ❌ | Limited |
| ISR Result Type | ✅ | ❌ | ❌ |
| Queue Sets | ✅ | C API only | ❌ |
| External Allocators | ✅ | ❌ | ❌ |
| Documentation | Comprehensive | Reference | Basic |

## Use Cases

This library is ideal for:

- **Industrial IoT Devices** - Reliable real-time data collection and transmission
- **Medical Devices** - Safety certification requirements
- **Any FreeRTOS Project** - Modern C++ interface for existing FreeRTOS codebases

### Industry Applications

- Industrial automation controllers
- Consumer electronics with safety requirements

## Features (v3.2.0)

| Category | Feature | Description |
|----------|---------|-------------|
| **API** | Dual API | FreeRTOS-native and std-compatible method pairs |
| **Error Handling** | expected\<T,E\> | Type-safe error handling with `freertos::expected` (C++23 polyfill) |
| **Error Handling** | error enum | Unified `freertos::error` enumeration for all wrapper errors |
| **Memory** | External Allocators | Static allocation from user-supplied memory regions |
| **Memory** | Dynamic Allocators | Built-in dynamic allocation with `da::` namespace aliases |
| **Type Safety** | Strong Typedefs | `priority`, `tick_count`, `event_bits`, `core_affinity_mask`, `queue_index`, `semaphore_count` |
| **Concurrency** | Thread Safety Annotations | Clang `__attribute__((capability))` based annotations (opt-in) |
| **Concurrency** | Queue Sets | Monitor multiple queues/semaphores simultaneously |
| **Concurrency** | Indexed Task Notifications | Notification arrays via `configTASK_NOTIFICATION_ARRAY_ENTRIES` |
| **Concurrency** | SMP Core Affinity | Pin tasks to specific CPU cores on SMP FreeRTOS |
| **Semantics** | Move Semantics | All wrappers are move-constructible and move-assignable |
| **Interoperability** | span-based API | `freertos::span` polyfill for buffer operations |
| **Interoperability** | ISR Result Type | `isr_result<T>` bundles return value with `higher_priority_task_woken` flag |
| **Build** | C++17 Feature Detection | Compile-time `FREERTOS_CPP_WRAPPERS_CPP17`, `FREERTOS_CPP_WRAPPERS_CPP20` macros |
| **Build** | C++17 Polyfills | `expected` and `span` polyfills fall back when std library lacks them |

## Quick Integration

### Option 1: CMake FetchContent (Recommended)

```cmake
Include(FetchContent)
FetchContent_Declare(
  freertos_cpp_wrappers
  GIT_REPOSITORY https://github.com/aschokinatgmail/freertos_cpp_wrappers.git
  GIT_TAG v3.2.0
)
FetchContent_MakeAvailable(freertos_cpp_wrappers)

target_link_libraries(your_target freertos_cpp_wrappers)
```

### Option 2: Git Submodule

```bash
git submodule add https://github.com/aschokinatgmail/freertos_cpp_wrappers.git third_party/freertos_cpp_wrappers
```

### Option 3: Single Header (Header-only mode)

```cpp
#define FREERTOS_CPP_WRAPPERS_HEADER_ONLY
#include <freertos_task.hpp>
#include <freertos_semaphore.hpp>
```

## Known Limitations

### Heap Allocation of `std::function` Callbacks

The `task` and `timer` wrappers use `std::function<void()>` for their callback type. In embedded environments with constrained memory, `std::function` may allocate from the heap for large captures (it typically uses a small-object optimization for small lambdas, but this is implementation-defined and may throw `std::bad_alloc`).

If heap-free callbacks are required, use `freertos::fixed_function<N>` — a fixed-capacity delegate that never allocates. It can be constructed from any callable and passed directly to wrapper constructors that accept callable parameters. See `freertos_fixed_function.hpp` for details.

## Code Examples

### Basic Task Creation

```cpp
#include <freertos_task.hpp>

freertos::Task my_task(
    "MyTask",
    1024,
    freertos::priority::Normal,
    []() {
        while (true) {
            freertos::Task::delay(freertos::tick_count{100});
        }
    }
);
```

### Strong Typedefs (v3.0.0)

```cpp
#include <freertos_strong_types.hpp>

// Type-safe FreeRTOS primitives - no more mixing raw integers
freertos::priority task_prio{2};
freertos::tick_count delay_ticks{100};
freertos::event_bits bits{0x01};
freertos::core_affinity_mask mask = freertos::core_affinity_mask::core(0);

// Compile-time prevention of accidental type mixing
// task_prio = delay_ticks;  // Error: different types
auto higher = task_prio.above();   // priority{3}
auto lower  = task_prio.below();   // priority{1}
```

### Expected-based Error Handling (v3.0.0)

```cpp
#include <freertos_queue.hpp>

freertos::queue<10, int> q;

// std-compatible API returns expected<T, error>
auto result = q.send_ex(42);
if (result.has_value()) {
    // Success
} else {
    // Type-safe error: result.error() is freertos::error
    switch (result.error()) {
        case freertos::error::timeout:     break;
        case freertos::error::queue_full:   break;
        case freertos::error::invalid_handle: break;
        default: break;
    }
}

// Chaining with and_then / or_else
q.send_ex(42).and_then([]() { /* success */ })
             .or_else([](freertos::error e) { /* handle error */ });
```

### ISR Result Type (v3.0.0)

```cpp
#include <freertos_isr_result.hpp>

void my_isr_handler() {
    auto result = semaphore.release_isr();
    // result.higher_priority_task_woken tracks context switch need
    // result.result contains the return value
    if (result.higher_priority_task_woken) {
        portYIELD_FROM_ISR();
    }
}
```

### External Allocators (v3.0.0)

```cpp
#include <freertos_external_allocator.hpp>

// Define a memory region for static allocation
struct MyRegion {
    static void* allocate(size_t size) { return region_pool.allocate(size); }
    static void deallocate(void* ptr) { region_pool.deallocate(ptr); }
};

// Create wrappers using external memory
freertos::binary_semaphore<freertos::external_semaphore_allocator<MyRegion>> sem;
freertos::queue<10, int, freertos::external_queue_allocator<MyRegion, 10, int>> q;

// Or use the ea:: namespace aliases
namespace ea = freertos::ea;
ea::binary_semaphore<MyRegion> sem;      // External allocator semaphore
ea::queue<MyRegion, 10, int> q;         // External allocator queue
```

### Thread Safety Annotations (v3.0.0)

```cpp
#define FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS 1
#include <freertos_thread_safety.hpp>

class FREERTOS_CAPABILITY("mutex") SharedData {
    FREERTOS_GUARDED_BY(mutex_) int value_{0};
    freertos::mutex<> mutex_;

public:
    void increment() FREERTOS_ACQUIRE(mutex_) {
        auto lock = mutex_.lock();
        value_++;
    }
    int get() FREERTOS_REQUIRES(mutex_) { return value_; }
};
```

### Semaphore for Resource Protection

```cpp
#include <freertos_semaphore.hpp>

freertos::BinarySemaphore resource_sem;

void protected_operation() {
    auto lock = resource_sem.lock();  // RAII lock
    // Critical section - semaphore automatically released
}
```

### Queue for Inter-Task Communication

```cpp
#include <freertos_queue.hpp>

freertos::queue<10, int> data_queue;

// Producer task
data_queue.send(42);

// Consumer task
auto value = data_queue.receive(freertos::tick_count{100});
```

### Timer with Callback

```cpp
#include <freertos_sw_timer.hpp>

freertos::timer periodic_timer(
    "Periodic",
    freertos::tick_count{500},
    true,  // Auto-reload
    []() {
        // Timer callback
    }
);

periodic_timer.start();
```

## Performance

The library is designed for minimal overhead:

| Operation | Overhead | Notes |
|-----------|----------|-------|
| Task creation | ~10 instructions | Thin wrapper around xTaskCreate |
| Semaphore lock | ~5 instructions | Inline wrapper around xSemaphoreTake |
| Queue send | ~8 instructions | Direct call to xQueueSend |
| Timer start | ~6 instructions | Wrapper around xTimerStart |

All wrappers compile to near-native FreeRTOS calls with no virtual function overhead.

## Requirements

- **C++ Standard:** C++17 or later (required)
- **FreeRTOS:** v10.0.0 or later
- **Compiler:** GCC 7+, Clang 6+, or MSVC 2019+
- **Platform:** Any FreeRTOS-supported platform

### C++17 Requirement

This library requires C++17. Key C++17 features used:
- `std::optional` for optional return values
- `if constexpr` for compile-time branching
- Structured bindings
- `freertos::expected` polyfill (falls back to `std::expected` when C++23 is available)
- `freertos::span` polyfill (falls back to `std::span` when C++20 is available)

### Tested Platforms

- STM32 (ARM Cortex-M)
- ESP32 (Xtensa)
- Linux (FreeRTOS simulator)

## Migration Guide

### Migrating from v1.x to v2.x/v3.x

See the detailed migration guide: [docs/migration-v1-to-v2.md](docs/migration-v1-to-v2.md)

Key changes in v3.0.0:
- **C++17 is now required** (previously C++14)
- **expected\<T,E\> error handling** replaces bare error codes in `_ex`-suffixed methods
- **Strong typedefs** replace raw `UBaseType_t`/`TickType_t` parameters in new API overloads
- **Thread safety annotations** are opt-in via `FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS`
- **Queue sets** are available under `#if configUSE_QUEUE_SETS`
- **External allocators** provide user-controlled static allocation

## 📚 Documentation

**[View Documentation](docs/html/index.html)** (local — generate with `doxygen Doxyfile`)

The library now features modern, user-friendly documentation with:
- **Interactive examples** for all major components
- **Step-by-step tutorials** for getting started
- **Best practices guide** for optimal usage
- **API reference** with detailed explanations
- **Mobile-responsive design** for convenient access

### Generating Documentation

```bash
# Generate HTML documentation
doxygen Doxyfile

# Documentation will be available in docs/html/index.html
```

## Testing

The project includes comprehensive test coverage with 592 automated tests across 8 test executables, covering all major functionality:

- **Task Module**: 81 tests (lifecycle, periodic tasks, move semantics)
- **Semaphore Module**: 81 tests (binary, counting, mutexes, lock guards)
- **Queue Module**: 49 tests (send/receive operations, state management)
- **Event Group Module**: 30 tests (bit operations, wait conditions)
- **Stream Buffer Module**: 47 tests (buffer operations, ISR variants)
- **Message Buffer Module**: 32 tests (variable-length messages, allocators)
- **Timer Module**: 50 tests (duration types, callback functionality, move semantics)
- **Enhanced Multitasking**: 12 tests (advanced scenarios, racing conditions)
- **STL Integration**: 15 tests (STL compatibility, performance analysis)
- **Timeout Precision**: 11 tests (chrono support, sub-millisecond delays)
- **C++17 Features**: 13 tests (move semantics, template metaprogramming, RAII)
- **Enhanced SW Timer**: 18 tests (enhanced timer behavior and coordination)

### Running Tests

```bash
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
make -j$(nproc)
ctest --verbose
```

## Quality Assurance Reports

The project generates a single comprehensive validation and verification report combining all quality assurance activities. Reports are timestamped and stored in the `VnV/` directory.

### Combined Validation and Verification Report
- **Location**: `VnV/VALIDATION_VERIFICATION_REPORT_<YYYYMMDD_HHMMSS>.md/.html`
- **Content**:
  - **Static Code Analysis**: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ results with code context
  - **Test Execution Results**: Complete Google Test results with pass/fail status per test
  - **Code Coverage Analysis**: Line and function coverage with uncovered areas explanation
  - **Validation Conclusions**: Production readiness assessment and recommendations
- **Tools**:
  - **clang-tidy**: cppcoreguidelines-*, cert-*, google-*, hicpp-*
  - **Enhanced cppcheck**: All rules (style, performance, portability, security, unused code, const correctness)
  - **MISRA C++**: MISRA C 2012 rules applicable to C++ with detailed descriptions
  - **Google Test/Mock**: Unit testing with FreeRTOS API mocking
  - **LCOV/GCOV**: Code coverage analysis
- **Scope**: Library code only (src/, include/) - test harness excluded

### Automated Report Generation

Reports are automatically generated with timestamps to track validation history:

```bash
# Generate combined V&V report with single command
make validation-verification-report

# Or run the script directly
./scripts/generate_validation_verification_report.sh
```

### Manual Report Generation

```bash
# Generate validation report manually
./scripts/run_tests_and_generate_reports.sh
```

## Build Options

- `ENABLE_CLANG_TIDY` (default: ON) - Enable comprehensive static analysis
- `CLANG_TIDY_WARNINGS_AS_ERRORS` (default: OFF) - Treat warnings as errors
- `GENERATE_CLANG_TIDY_REPORT` (default: ON) - Generate comprehensive static analysis report
- `ENABLE_MISRA_ANALYSIS` (default: ON) - Enable enhanced cppcheck and MISRA C++ analysis
- `ENABLE_COVERAGE` (default: OFF) - Enable code coverage collection

## Code Formatting

The project follows a consistent coding standard enforced by clang-format based on LLVM style. All source files in `include/` and `src/` directories are automatically formatted according to the `.clang-format` configuration.

### Manual Code Formatting

```bash
# Format all source files using the standalone script
./scripts/format_code.sh

# Format specific files manually
clang-format -i include/freertos_task.hpp src/freertos_task.cc

# Format using CMake target
make format
```

### Automatic Formatting

Code formatting is automatically applied when running tests:

```bash
# This script automatically formats code before building and testing
./scripts/run_tests_and_generate_reports.sh
```

The formatting configuration ensures:
- 2-space indentation
- 80-character line limit
- Consistent bracket placement
- Proper spacing and alignment
- Include sorting and organization

## Quick Start

### Using Docker (Recommended for CI)

```bash
# Build and run tests
docker compose up test

# Run with coverage report
docker compose up coverage

# Generate full validation & verification report
docker compose up validation

# Generate documentation
docker compose up docs
```

### Native Build

```bash
# Clone and build with full validation
git clone <repository>
cd freertos_cpp_wrappers
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
make validation-verification-report

# View results (timestamped reports in VnV/ directory)
ls ../VnV/
```

## Project Status

- ✅ **All 592 tests passing** (100% success rate)
- ✅ **95.0% branch coverage**, **99.9% function coverage**, **100% line coverage**
- ✅ **Comprehensive static analysis** with clang-tidy + enhanced cppcheck (all rules) + MISRA C++
- ✅ **Production ready** with comprehensive validation

## Contributing

Contributions are welcome! Please ensure:

1. All tests pass: `ctest --verbose`
2. Code coverage remains above 95%
3. Static analysis passes: `make static-analysis-report`
4. MISRA C++ compliance maintained
5. Code formatted: `make format`

### Development Setup

```bash
git clone https://github.com/aschokinatgmail/freertos_cpp_wrappers.git
cd freertos_cpp_wrappers
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
make validation-verification-report
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Commercial Use

This library is suitable for commercial applications. The MIT license permits:
- Use in proprietary software
- Modification and distribution
- No warranty requirements

## Acknowledgments

- FreeRTOS by Amazon Web Services
- Google Test framework for unit testing
- Doxygen for documentation generation
- clang-tidy and cppcheck for static analysis

## Author

**Andrey Shchekin**
- GitHub: [@aschokinatgmail](https://github.com/aschokinatgmail)
- LinkedIn: [Andrey Shchekin](https://linkedin.com/in/andrey-shchekin)