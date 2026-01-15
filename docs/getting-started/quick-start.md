# Quick Start Guide

Get up and running with FreeRTOS C++ Wrappers in just a few minutes!

## Prerequisites

Before you begin, make sure you have:

- **C++17 compatible compiler** (GCC 7+, Clang 6+, or MSVC 2017+)
- **CMake 3.12 or later**
- **FreeRTOS source code**
- **Git** for cloning the repository

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/aschokinatgmail/freertos_cpp_wrappers.git
cd freertos_cpp_wrappers
```

### 2. Build the Library

=== "Debug Build"

    ```bash
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
    make -j$(nproc)
    ```

=== "Release Build"

    ```bash
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j$(nproc)
    ```

=== "With Static Analysis"

    ```bash
    mkdir build && cd build
    cmake -DENABLE_CLANG_TIDY=ON -DCLANG_TIDY_WARNINGS_AS_ERRORS=OFF ..
    make -j$(nproc)
    ```

### 3. Run Tests (Optional)

```bash
# From the build directory
ctest --verbose --output-on-failure
```

## Your First Program

Create a simple program to test the installation:

```cpp title="main.cpp"
#include "freertos.hpp"
#include <iostream>

// Simple worker task
void worker_task_function() {
    int counter = 0;
    while (true) {
        std::cout << "Worker task iteration: " << ++counter << std::endl;
        
        // Sleep for 1 second using std::chrono
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    std::cout << "FreeRTOS C++ Wrappers Demo" << std::endl;
    
    // Create a task with 1024 bytes of stack
    freertos::task<1024> worker_task(
        "WorkerTask",           // Task name
        3,                      // Priority
        worker_task_function    // Task function
    );
    
    // Create a periodic task that runs every 500ms
    freertos::periodic_task<512> timer_task(
        "TimerTask",            // Task name
        2,                      // Priority  
        std::chrono::milliseconds(500),  // Period
        []() {                  // Lambda task function
            static int timer_count = 0;
            std::cout << "Timer task execution #" << ++timer_count << std::endl;
        }
    );
    
    std::cout << "Starting FreeRTOS scheduler..." << std::endl;
    
    // Start the FreeRTOS scheduler
    vTaskStartScheduler();
    
    // This line should never be reached
    return 0;
}
```

## Compile and Run

```bash
g++ -std=c++17 -I./include -o demo main.cpp -lfreertos
./demo
```

You should see output similar to:

```
FreeRTOS C++ Wrappers Demo
Starting FreeRTOS scheduler...
Worker task iteration: 1
Timer task execution #1
Worker task iteration: 2
Timer task execution #2
Worker task iteration: 3
Timer task execution #3
...
```

## Common Patterns

### Creating Tasks

=== "Lambda Function"

    ```cpp
    freertos::task<1024> my_task("MyTask", 3, []() {
        while (true) {
            // Your code here
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    });
    ```

=== "Function Pointer"

    ```cpp
    void task_function() {
        while (true) {
            // Your code here
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    
    freertos::task<1024> my_task("MyTask", 3, task_function);
    ```

=== "Static Allocation"

    ```cpp
    freertos::task<1024, freertos::static_task_allocator<1024>> static_task(
        "StaticTask", 3, task_function
    );
    ```

### Synchronization

```cpp
// Binary semaphore for signaling
freertos::binary_semaphore signal;

// Producer
signal.give();  // Signal

// Consumer  
if (signal.take(std::chrono::seconds(1))) {
    // Signal received within timeout
}
```

### Message Passing

```cpp
// Type-safe queue
freertos::queue<int, 10> int_queue;

// Send
int_queue.send(42, std::chrono::milliseconds(100));

// Receive
int value;
if (int_queue.receive(value, std::chrono::seconds(1))) {
    // Value received successfully
}
```

## Next Steps

Now that you have the basics working:

1. **[Explore Examples](../examples/basic-usage.md)** - See real-world usage patterns
2. **[Browse API Reference](../api/overview.md)** - Detailed API documentation

## Getting Help

If you encounter issues:

- **[Check the Examples](../examples/basic-usage.md)** for similar use cases
- **[Read the API Documentation](../api/overview.md)** for detailed reference
- **[Search Issues](https://github.com/aschokinatgmail/freertos_cpp_wrappers/issues)** on GitHub
- **[Ask Questions](https://github.com/aschokinatgmail/freertos_cpp_wrappers/discussions)** in GitHub Discussions

## Troubleshooting

### Common Issues

!!! warning "Compiler Errors"
    Make sure you're using a C++17 compatible compiler. The library uses modern C++ features that are not available in older standards.

!!! warning "FreeRTOS Not Found"
    Ensure FreeRTOS headers are in your include path and the FreeRTOS library is linked properly.

!!! tip "Stack Size Issues"
    If tasks crash or behave unexpectedly, try increasing the stack size template parameter.

!!! tip "Priority Conflicts"
    FreeRTOS uses priority-based scheduling. Higher numbers mean higher priority. Make sure your task priorities are appropriate for your application.