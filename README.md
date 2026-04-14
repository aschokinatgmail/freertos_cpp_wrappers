# C++ Wrappers for FreeRTOS

[![Tests](https://img.shields.io/badge/Tests-439%20passing-brightgreen)]()
[![Coverage](https://img.shields.io/badge/Coverage-97%25-brightgreen)]()
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
| `FREERTOS_CPP_WRAPPERS_VERSION` | Full version string (e.g. `"1.0.2"`) |

Include `<freertos.hpp>` to access version macros at compile time.

## Why Use This Library?

| Feature | Benefit |
|---------|---------|
| **Safety-Critical Ready** | MISRA 2012 C compliant, suitable for functonaly safe applications |
| **Modern C++17** | RAII wrappers with move semantics, no raw pointers |
| **Production Tested** | 439 tests, 97% coverage, used in commercial BMS systems |
| **Zero Overhead** | Thin wrappers compile to efficient FreeRTOS calls |
| **Comprehensive** | Tasks, Semaphores, Queues, Event Groups, Stream/Message Buffers, Timers |
| **Well Documented** | Doxygen API reference with examples and tutorials |

### Comparison with Alternatives

| Feature | This Library | Raw FreeRTOS | Other Wrappers |
|---------|--------------|--------------|-----------------|
| RAII Support | ✅ | ❌ | Varies |
| MISRA C 2012 Rulles Compliant | ✅ | N/A | ❌ |
| Test Coverage | 97% | N/A | Unknown |
| C++17 Features | ✅ | ❌ | Limited |
| Documentation | Comprehensive | Reference | Basic |

## Use Cases

This library is ideal for:

- **Industrial IoT Devices** - Reliable real-time data collection and transmission
- **Medical Devices** - Safety certification requirements
- **Any FreeRTOS Project** - Modern C++ interface for existing FreeRTOS codebases

### Industry Applications

- Industrial automation controllers
- Consumer electronics with safety requirements

## Quick Integration

### Option 1: CMake FetchContent (Recommended)

```cmake
Include(FetchContent)
FetchContent_Declare(
  freertos_cpp_wrappers
  GIT_REPOSITORY https://github.com/aschokinatgmail/freertos_cpp_wrappers.git
  GIT_TAG v1.0.2
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

## Code Examples

### Basic Task Creation

```cpp
#include <freertos_task.hpp>

// RAII task with automatic cleanup
freertos::Task my_task(
    "MyTask",           // Name
    1024,               // Stack size (words)
    freertos::Priority::Normal,
    []() {
        while (true) {
            // Task code
            freertos::Task::delay(100);
        }
    }
);

// Task automatically starts and cleans up on destruction
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

freertos::Queue<int> data_queue(10);

// Producer task
data_queue.send(42);

// Consumer task
auto value = data_queue.receive(freertos::Timeout::ms(100));
```

### Timer with Callback

```cpp
#include <freertos_timer.hpp>

freertos::Timer periodic_timer(
    "Periodic",
    freertos::Timeout::ms(500),
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

- **FreeRTOS:** v10.0.0 or later
- **C++ Standard:** C++17 or later
- **Compiler:** GCC 7+, Clang 6+, or MSVC 2019+
- **Platform:** Any FreeRTOS-supported platform

### Tested Platforms

- STM32 (ARM Cortex-M)
- ESP32 (Xtensa)
- Linux (FreeRTOS simulator)

## 📚 Documentation

**[View Online Documentation](docs/html/index.html)** - Comprehensive API reference with examples

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

## Features

- Modern C++17 RAII wrappers for FreeRTOS APIs
- Comprehensive test coverage (439 tests)
- Comprehensive static analysis with clang-tidy + enhanced cppcheck (all rules)
- MISRA C++ compliance analysis with detailed rule descriptions
- Automated code formatting with clang-format (LLVM-based style)
- Support for both static and dynamic allocation strategies
- chrono compatibility for timeout handling
- Enhanced C++17 features testing
- Automated validation and verification reporting

## Testing

The project includes comprehensive test coverage with 439 automated tests covering all major functionality:

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

# This will:
# 1. Build the project with coverage enabled
# 2. Run all tests via Google Test harness
# 3. Generate coverage data
# 4. Run comprehensive static analysis (clang-tidy + enhanced cppcheck + MISRA C++)
# 5. Generate combined timestamped report in VnV/ directory
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

- ✅ **All 439 tests passing** (100% success rate)
- ✅ **97.0% line coverage** and **98.2% function coverage**
- ✅ **Comprehensive static analysis** with clang-tidy + enhanced cppcheck (all rules) + MISRA C++
- ✅ **Production ready** with comprehensive validation

## Contributing

Contributions are welcome! Please ensure:

1. All tests pass: `ctest --verbose`
2. Code coverage remains above 95%
3. Static analysis passes: `make clang-tidy`
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
