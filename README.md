# C++ Wrappers for FreeRTOS

Lightweight collection of FreeRTOS API wrappers compliant to modern C++ standards

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

The project automatically generates three comprehensive quality reports that are updated with each test execution:

### 1. Static Analysis Report
- **File**: `STATIC_ANALYSIS_REPORT.md`
- **Content**: Comprehensive static analysis with clang-tidy + enhanced cppcheck (all rules) + MISRA C++
- **Scope**: Library code only (src/, include/) - test harness excluded
- **Tools**: 
  - **clang-tidy**: cppcoreguidelines-*, cert-*, google-*, hicpp-*
  - **Enhanced cppcheck**: All rules (style, performance, portability, security, unused code, const correctness)
  - **MISRA C++**: MISRA C 2012 rules applicable to C++ with detailed descriptions
- **Purpose**: Comprehensive code quality analysis covering standards compliance and best practices

### 2. Validation and Verification Report
- **File**: `VALIDATION_VERIFICATION_REPORT.md`
- **Content**: 
  - Complete test execution results with pass/fail status
  - Individual test performance metrics
  - Code coverage analysis (97.0% line coverage, 98.2% function coverage)
  - Detailed explanation of uncovered code areas
  - Test categorization and quality metrics
- **Purpose**: Demonstrates system validation and production readiness

### Automated Report Generation

All reports are automatically regenerated to reflect the current state of development:

```bash
# Generate all reports with single command
make validation-verification-report

# This command will:
# 1. Build the project with coverage enabled
# 2. Run all 439 tests
# 3. Generate coverage data
# 4. Run comprehensive static analysis (clang-tidy + enhanced cppcheck + MISRA C++)
# 5. Generate all reports with current results
```

### Manual Report Generation

```bash
# Generate comprehensive static analysis report (clang-tidy + enhanced cppcheck + MISRA C++)
make static-analysis-report

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

```bash
# Clone and build with full validation
git clone <repository>
cd freertos_cpp_wrappers
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
make validation-verification-report

# View results
cat ../STATIC_ANALYSIS_REPORT.md
cat ../VALIDATION_VERIFICATION_REPORT.md
```

## Project Status

- ✅ **All 439 tests passing** (100% success rate)
- ✅ **97.0% line coverage** and **98.2% function coverage**
- ✅ **Comprehensive static analysis** with clang-tidy + enhanced cppcheck (all rules) + MISRA C++
- ✅ **Production ready** with comprehensive validation
