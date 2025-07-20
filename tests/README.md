# FreeRTOS C++ Wrapper Tests

This directory contains comprehensive unit tests for the FreeRTOS C++ wrapper library using GoogleTest and GoogleMock frameworks.

## Test Structure

### Completed Modules ✅
- **Task Module** (`test_freertos_task.cpp`) - 46 tests
- **Semaphore Module** (`test_freertos_semaphore.cpp`) - 39 tests

### Planned Modules 📋
- Queue Module (`test_freertos_queue.cpp`)
- Event Group Module (`test_freertos_event_group.cpp`)
- Stream Buffer Module (`test_freertos_stream_buffer.cpp`)
- Message Buffer Module (`test_freertos_message_buffer.cpp`)
- Software Timer Module (`test_freertos_sw_timer.cpp`)

## Test Coverage

**Overall Coverage: 89.2%** (1,211 of 1,358 lines)

See [COVERAGE_REPORT.md](../COVERAGE_REPORT.md) for detailed coverage analysis.

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