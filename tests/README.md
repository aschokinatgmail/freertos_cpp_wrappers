# FreeRTOS Task Unit Tests

This directory contains comprehensive unit tests for the FreeRTOS C++ task wrapper library.

## Overview

The test suite provides comprehensive behavioral unit tests for:
- Static and dynamic task allocators
- Task construction, allocation, and deallocation 
- Typical usage patterns: creation, starting, and deletion
- Periodic task operation and timing
- Stack allocation limitation (freertos::sa::task fails on stack)
- Error and edge cases with invalid parameters

## Features

- **Host-based testing**: Uses GoogleTest/GoogleMock with FreeRTOS API mocks
- **Comprehensive coverage**: 46 tests covering all major functionality
- **Clear documentation**: Comments explain untestable or RTOS-only features
- **Template for future tests**: Can be used as a template for other modules

## Building and Running Tests

### Prerequisites

- CMake 3.14 or higher
- C++17 compatible compiler
- GoogleTest and GoogleMock libraries

### Install Dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y libgtest-dev libgmock-dev cmake build-essential
```

### Build Tests

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build the project and tests
make -j$(nproc)
```

### Run Tests

```bash
# Run all tests
./tests/test_freertos_task

# Run tests with XML output
./tests/test_freertos_task --gtest_output=xml:test_results.xml

# Run specific test
./tests/test_freertos_task --gtest_filter=FreeRTOSTaskTest.StaticTaskAllocatorConstruction

# Run tests through CTest
ctest --verbose
```

## Test Structure

### Test Categories

1. **Static Task Allocator Tests**
   - Construction and destruction
   - Task creation and allocation
   - Error handling

2. **Dynamic Task Allocator Tests**
   - Construction and destruction  
   - Task creation and allocation
   - Error handling

3. **Basic Task Tests**
   - Task construction with both allocators
   - Task control operations (suspend, resume, terminate)
   - Task information retrieval (priority, name, state)
   - Task notifications

4. **Periodic Task Tests**
   - Construction with various periods
   - Task lifecycle management
   - Running state checking

5. **Utility Function Tests**
   - Delay and timing functions
   - System information functions
   - Critical sections and barriers

6. **Edge Cases and Error Handling**
   - Invalid parameters
   - Zero stack sizes
   - Move semantics limitations

### Key Testing Patterns

- **Mock Expectations**: All FreeRTOS API calls are mocked with strict expectations
- **RAII Testing**: Proper constructor/destructor behavior verification
- **Error Path Testing**: Invalid inputs and edge cases are thoroughly tested
- **Move Semantics**: Documents current limitations in move constructor behavior

## Mock Architecture

### FreeRTOS Mocks

The test framework includes comprehensive mocks for FreeRTOS APIs:

- `tests/mocks/FreeRTOS.h` - Main FreeRTOS types and mock class
- `tests/mocks/freertos_mocks.cpp` - Mock implementation
- `tests/mocks/task.h` - Task-specific header stub
- `tests/mocks/semphr.h` - Semaphore header stub

### Mock Features

- **Complete API Coverage**: All FreeRTOS functions used by the library are mocked
- **Type Compatibility**: Proper FreeRTOS types and constants
- **Host-based**: No actual FreeRTOS kernel required
- **Configurable**: Easy to extend for additional FreeRTOS features

## Limitations and Comments

### Stack Allocation Limitation

The tests document a current limitation where static tasks cannot be safely allocated on the stack due to the way FreeRTOS static allocation works. The `StackAllocationLimitation` test documents this behavior.

### Move Constructor Behavior

The tests reveal that the current move constructor implementation uses default memberwise move, which can lead to double deletion of task handles. This is documented in the move construction tests.

### RTOS-Only Features

Some features require a real FreeRTOS environment:
- Actual task execution and scheduling
- Real-time behavior verification
- Inter-task communication timing
- Interrupt service routine behavior

These limitations are clearly documented in test comments.

## Extending the Tests

This test suite serves as a template for testing other FreeRTOS wrapper modules:

1. **Copy the mock architecture** for the target module
2. **Add module-specific mock functions** to the FreeRTOS mock class
3. **Create comprehensive test cases** following the patterns established here
4. **Document limitations and RTOS-only features** in test comments

## Test Results

When all tests pass, you should see:

```
[==========] Running 46 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 46 tests from FreeRTOSTaskTest
[       OK ] All tests completed successfully
[----------] 46 tests from FreeRTOSTaskTest (X ms total)
[==========] 46 tests from 1 test suite ran. (X ms total)
[  PASSED  ] 46 tests.
```

## Integration with CI/CD

The tests can be integrated into continuous integration pipelines:

```bash
# Example CI script
mkdir build && cd build
cmake ..
make -j$(nproc)
ctest --output-on-failure
```

The tests generate XML output compatible with most CI systems when run with `--gtest_output=xml:results.xml`.