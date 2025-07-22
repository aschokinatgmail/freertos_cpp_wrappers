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
- Comprehensive test coverage (377+ tests)
- Static analysis integration with clang-tidy
- Support for both static and dynamic allocation strategies
- chrono compatibility for timeout handling
- Automated validation and verification reporting

## Testing

The project includes comprehensive test coverage with 377 automated tests covering all major functionality:

- **Task Module**: 81 tests (lifecycle, periodic tasks, move semantics)
- **Semaphore Module**: 68 tests (binary, counting, mutexes, lock guards)  
- **Queue Module**: 49 tests (send/receive operations, state management)
- **Event Group Module**: 30 tests (bit operations, wait conditions)
- **Stream Buffer Module**: 47 tests (buffer operations, ISR variants)
- **Message Buffer Module**: 32 tests (variable-length messages, allocators)
- **Timer Module**: 50 tests (duration types, callback functionality, move semantics)
- **Enhanced Multitasking**: 12 tests (advanced scenarios, racing conditions)

### Running Tests

```bash
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
make -j$(nproc)
ctest --verbose
```

## Quality Assurance Reports

The project automatically generates two comprehensive quality reports that are updated with each test execution:

### 1. Static Analysis Report
- **File**: `STATIC_ANALYSIS_REPORT.md`
- **Content**: clang-tidy static analysis results for main library modules only
- **Scope**: Library code only (src/, include/) - test harness excluded
- **Checks**: cppcoreguidelines-*, cert-*, google-*, hicpp-*
- **Purpose**: Identifies code quality issues, style violations, and potential bugs

### 2. Validation and Verification Report
- **File**: `VALIDATION_VERIFICATION_REPORT.md`
- **Content**: 
  - Complete test execution results with pass/fail status
  - Individual test performance metrics
  - Code coverage analysis (96.8% line coverage, 95.7% function coverage)
  - Detailed explanation of uncovered code areas
  - Test categorization and quality metrics
- **Purpose**: Demonstrates system validation and production readiness

### Automated Report Generation

Both reports are automatically regenerated to reflect the current state of development:

```bash
# Generate both reports with single command
make validation-verification-report

# This command will:
# 1. Build the project with coverage enabled
# 2. Run all 377 tests
# 3. Generate coverage data
# 4. Run static analysis
# 5. Generate both reports with current results
```

### Manual Report Generation

```bash
# Generate static analysis report only
make static-analysis-report

# Generate validation report manually
./scripts/run_tests_and_generate_reports.sh
```

## Build Options

- `ENABLE_CLANG_TIDY` (default: ON) - Enable static analysis
- `CLANG_TIDY_WARNINGS_AS_ERRORS` (default: OFF) - Treat warnings as errors
- `GENERATE_CLANG_TIDY_REPORT` (default: ON) - Generate static analysis report
- `ENABLE_COVERAGE` (default: OFF) - Enable code coverage collection

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

- ✅ **All 377 tests passing** (100% success rate)
- ✅ **96.8% line coverage** and **95.7% function coverage**
- ✅ **Clean static analysis** (library modules only)
- ✅ **Production ready** with comprehensive validation
