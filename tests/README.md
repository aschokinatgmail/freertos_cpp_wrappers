# FreeRTOS C++ Wrapper Tests

This directory contains comprehensive unit tests for the FreeRTOS C++ wrapper library using GoogleTest and GoogleMock frameworks.

## Test Structure

### Completed Modules ✅
- **Task Module** (`test_freertos_task.cpp`) - 46 tests
- **Semaphore Module** (`test_freertos_semaphore.cpp`) - 68 tests
- **Queue Module** (`test_freertos_queue.cpp`) - 49 tests
- **Stream Buffer Module** (`test_freertos_stream_buffer.cpp`) - 47 tests
- **Event Group Module** (`test_freertos_event_group.cpp`) - 30 tests
- **Message Buffer Module** (`test_freertos_message_buffer.cpp`) - 32 tests
- **Software Timer Module** (`test_freertos_sw_timer.cpp`) - 50 tests

**All major FreeRTOS modules are now fully tested!** 🎉

## Test Coverage

**Overall Coverage: 92.4%** (1,425 of 1,542 lines)

**Total Tests: 322** (Task: 46, Semaphore: 68, Queue: 49, Stream Buffer: 47, Event Group: 30, Message Buffer: 32, Timer: 50)

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
./tests/test_freertos_semaphore
./tests/test_freertos_queue
./tests/test_freertos_stream_buffer
./tests/test_freertos_event_group
./tests/test_freertos_message_buffer
./tests/test_freertos_sw_timer

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

5. **Queue Tests**
   - Static and dynamic queue allocators
   - Send operations (send, send_back, send_front) with timeouts and ISR variants  
   - Receive operations with optional<T> and reference variants
   - Peek operations (non-destructive read)
   - Queue state operations (messages_waiting, spaces_available, empty/full checks)
   - Overwrite operations for single-item queues
   - Reset functionality and error handling

6. **Event Group Tests**
   - Static and dynamic event group allocators
   - Bit setting and clearing operations
   - Waiting for bits with AND/OR logic
   - Timeout handling and ISR variants
   - Event group state queries and error handling

7. **Message Buffer Tests**
   - Static and dynamic message buffer allocators
   - Send and receive operations with variable-length messages
   - Buffer state management (space available, empty/full checks)
   - Reset functionality and timeout handling
   - Edge cases with zero-length and maximum-size messages
   - Chrono timeout compatibility

8. **Software Timer Tests**
   - Static and dynamic timer allocators
   - Timer creation with tick-based and chrono-based periods
   - Timer start/stop/reset operations with various timeout parameters
   - Timer period changes and state queries
   - Single-shot vs auto-reload timer behavior
   - Timer name and reload mode management
   - ISR variants of all timer functions (start_isr, stop_isr, reset_isr, period_isr)
   - Mixed duration type support (seconds, milliseconds, microseconds, etc.)
   - Timer callback functionality and ID management
   - Move construction and assignment semantics
   - Complex destructor scenarios and error conditions

9. **Utility Function Tests**
   - Delay and timing functions
   - System information functions
   - Critical sections and barriers

10. **Edge Cases and Error Handling**
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
- `tests/mocks/queue.h` - Queue header stub
- `tests/mocks/stream_buffer.h` - Stream buffer header stub
- `tests/mocks/event_groups.h` - Event group header stub
- `tests/mocks/message_buffer.h` - Message buffer header stub
- `tests/mocks/timers.h` - Software timer header stub

### Mock Features

- **Complete API Coverage**: All FreeRTOS functions used by the library are mocked
- **Type Compatibility**: Proper FreeRTOS types and constants
- **Host-based**: No actual FreeRTOS kernel required
- **Configurable**: Easy to extend for additional FreeRTOS features

## Limitations and Comments

### Stack Allocation Limitation

The tests document a current limitation where static tasks cannot be safely allocated on the stack due to the way FreeRTOS static allocation works. The `StackAllocationLimitation` test documents this behavior.

### Move Constructor Behavior

Previously, the tests documented that the move constructor implementation used default memberwise move, which could lead to double deletion of timer handles. This has been fixed in the current implementation with proper move constructor that transfers ownership and prevents double deletion.

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
# Task Tests
[==========] Running 46 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 46 tests from FreeRTOSTaskTest
[       OK ] All tests completed successfully
[----------] 46 tests from FreeRTOSTaskTest (X ms total)
[==========] 46 tests from 1 test suite ran. (X ms total)
[  PASSED  ] 46 tests.

# Semaphore Tests  
[==========] Running 68 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 68 tests from FreeRTOSSemaphoreTest
[       OK ] All tests completed successfully
[----------] 68 tests from FreeRTOSSemaphoreTest (X ms total)
[==========] 68 tests from 1 test suite ran. (X ms total)
[  PASSED  ] 68 tests.

# Queue Tests
[==========] Running 49 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 49 tests from FreeRTOSQueueTest
[       OK ] All tests completed successfully
[----------] 49 tests from FreeRTOSQueueTest (X ms total)
[==========] 49 tests from 1 test suite ran. (X ms total)
[  PASSED  ] 49 tests.

# Event Group Tests
[==========] Running 30 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 30 tests from FreeRTOSEventGroupTest
[       OK ] All tests completed successfully
[----------] 30 tests from FreeRTOSEventGroupTest (X ms total)
[==========] 30 tests from 1 test suite ran. (X ms total)
[  PASSED  ] 30 tests.

# Message Buffer Tests
[==========] Running 32 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 32 tests from FreeRTOSMessageBufferTest
[       OK ] All tests completed successfully
[----------] 32 tests from FreeRTOSMessageBufferTest (X ms total)
[==========] 32 tests from 1 test suite ran. (X ms total)
[  PASSED  ] 32 tests.

# Software Timer Tests
[==========] Running 50 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 50 tests from FreeRTOSSwTimerTest
[       OK ] All tests completed successfully
[----------] 50 tests from FreeRTOSSwTimerTest (X ms total)
[==========] 50 tests from 1 test suite ran. (X ms total)
[  PASSED  ] 50 tests.
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