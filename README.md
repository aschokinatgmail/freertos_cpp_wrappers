# C++ Wrappers for FreeRTOS

Lightweight collection of FreeRTOS API wrappers compliant to modern C++ standards

## Features

- Modern C++17 RAII wrappers for FreeRTOS APIs
- Comprehensive test coverage (322+ tests)
- Static analysis integration with clang-tidy
- Support for both static and dynamic allocation strategies
- chrono compatibility for timeout handling

## Testing

The project includes comprehensive test coverage with 322 automated tests covering all major functionality:

- **Task Module**: 46 tests (lifecycle, periodic tasks, move semantics)
- **Semaphore Module**: 68 tests (binary, counting, mutexes, lock guards)  
- **Queue Module**: 49 tests (send/receive operations, state management)
- **Event Group Module**: 30 tests (bit operations, wait conditions)
- **Stream Buffer Module**: 47 tests (buffer operations, ISR variants)
- **Message Buffer Module**: 32 tests (variable-length messages, allocators)
- **Timer Module**: 50 tests (duration types, callback functionality, move semantics)

### Running Tests

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
ctest --verbose
```

## Code Quality Reports

The project maintains several quality reports:

### Test Report
- **File**: `TEST_REPORT.md`
- **Content**: Comprehensive test execution results and coverage details
- **Update**: Regenerated after test runs

### Static Analysis Report  
- **File**: `STATIC_ANALYSIS_REPORT.md`
- **Content**: clang-tidy static analysis results for library modules
- **Scope**: Library code only (src/, include/) - tests excluded
- **Checks**: cppcoreguidelines-*, cert-*, google-*, hicpp-*

### Coverage Report
- **File**: `COVERAGE_REPORT.md` 
- **Content**: Code coverage analysis with LCOV/GCOV
- **Metrics**: Line coverage, function coverage, branch analysis

### Generating Reports

```bash
# Generate static analysis report
make static-analysis-report

# Generate coverage report (requires ENABLE_COVERAGE=ON)
cmake -DENABLE_COVERAGE=ON ..
make -j$(nproc)
ctest
# (manual lcov commands - see COVERAGE_REPORT.md)

# Test reports are generated automatically during ctest execution
```

## Build Options

- `ENABLE_CLANG_TIDY` (default: ON) - Enable static analysis
- `CLANG_TIDY_WARNINGS_AS_ERRORS` (default: OFF) - Treat warnings as errors
- `GENERATE_CLANG_TIDY_REPORT` (default: ON) - Generate static analysis report
- `ENABLE_COVERAGE` (default: OFF) - Enable code coverage collection
