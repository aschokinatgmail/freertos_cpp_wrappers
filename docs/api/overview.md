# API Reference Overview

The FreeRTOS C++ Wrappers library provides modern C++17 RAII wrappers for all major FreeRTOS APIs. This page provides an overview of the available classes and their key features.

## Core Principles

All wrapper classes follow these design principles:

- **RAII Resource Management** - Automatic resource cleanup
- **Type Safety** - Compile-time error prevention
- **std::chrono Support** - Modern timeout handling
- **Zero Overhead** - No performance penalty over raw FreeRTOS
- **Move Semantics** - Efficient resource transfer

## Main Components

### Tasks

| Class | Description | Header |
|-------|-------------|---------|
| `freertos::task<StackSize, Allocator>` | General-purpose task wrapper | `freertos_task.hpp` |
| `freertos::periodic_task<StackSize, Allocator>` | Periodic task with automatic timing | `freertos_task.hpp` |

**Key Features:**
- Template-based stack size specification
- Support for both static and dynamic allocation
- Automatic task cleanup on destruction
- Lambda and function pointer support

**Example:**
```cpp
freertos::task<1024> my_task("TaskName", 3, []() {
    while (true) {
        // Task code here
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
});
```

### Semaphores

| Class | Description | Header |
|-------|-------------|---------|
| `freertos::binary_semaphore<Allocator>` | Binary semaphore for signaling | `freertos_semaphore.hpp` |
| `freertos::counting_semaphore<MaxCount, Allocator>` | Counting semaphore for resources | `freertos_semaphore.hpp` |
| `freertos::mutex<Allocator>` | Mutual exclusion primitive | `freertos_semaphore.hpp` |
| `freertos::recursive_mutex<Allocator>` | Recursive mutex | `freertos_semaphore.hpp` |

**Key Features:**
- std::chrono timeout support
- RAII lock guards
- Automatic cleanup on destruction

**Example:**
```cpp
freertos::binary_semaphore signal;
if (signal.take(std::chrono::seconds(1))) {
    // Signal received within timeout
}
```

### Lock Guards

| Class | Description | Header |
|-------|-------------|---------|
| `freertos::lock_guard<Mutex>` | Basic RAII lock guard | `freertos_semaphore.hpp` |
| `freertos::try_lock_guard<Mutex>` | Non-blocking lock guard | `freertos_semaphore.hpp` |
| `freertos::timeout_lock_guard<Mutex>` | Timeout-based lock guard | `freertos_semaphore.hpp` |

**Key Features:**
- Automatic lock/unlock management
- Exception safety
- Various locking strategies

**Example:**
```cpp
freertos::mutex mtx;
{
    freertos::lock_guard<freertos::mutex> lock(mtx);
    // Critical section - mutex automatically unlocked at end of scope
}
```

### Queues

| Class | Description | Header |
|-------|-------------|---------|
| `freertos::queue<Type, Size, Allocator>` | Type-safe message queue | `freertos_queue.hpp` |

**Key Features:**
- Type-safe message passing
- Template-based size specification
- std::chrono timeout support
- Support for complex C++ types

**Example:**
```cpp
freertos::queue<int, 10> int_queue;
int value = 42;
if (int_queue.send(value, std::chrono::milliseconds(100))) {
    // Message sent successfully
}
```

### Event Groups

| Class | Description | Header |
|-------|-------------|---------|
| `freertos::event_group<Allocator>` | Multi-event synchronization | `freertos_event_group.hpp` |

**Key Features:**
- Multi-bit event signaling
- Wait for any or all events
- std::chrono timeout support
- Automatic cleanup

**Example:**
```cpp
freertos::event_group events;
constexpr EventBits_t READY_BIT = BIT0;

events.set_bits(READY_BIT);
if (events.wait_bits(READY_BIT, true, true, std::chrono::seconds(5))) {
    // Event received within timeout
}
```

### Stream Buffers

| Class | Description | Header |
|-------|-------------|---------|
| `freertos::stream_buffer<Size, Allocator>` | Byte stream communication | `freertos_stream_buffer.hpp` |

**Key Features:**
- Efficient byte stream passing
- Variable-length data support
- std::chrono timeout support
- Automatic buffer management

**Example:**
```cpp
freertos::stream_buffer<1024> stream;
const char* data = "Hello, World!";
stream.send(data, strlen(data), std::chrono::milliseconds(100));
```

### Message Buffers

| Class | Description | Header |
|-------|-------------|---------|
| `freertos::message_buffer<Size, Allocator>` | Variable-length message passing | `freertos_message_buffer.hpp` |

**Key Features:**
- Variable-length message support
- Built-in length handling
- std::chrono timeout support
- Automatic buffer management

**Example:**
```cpp
freertos::message_buffer<512> msg_buffer;
const char* message = "Variable length message";
msg_buffer.send(message, strlen(message), std::chrono::milliseconds(100));
```

### Software Timers

| Class | Description | Header |
|-------|-------------|---------|
| `freertos::sw_timer<Allocator>` | Software timer wrapper | `freertos_sw_timer.hpp` |

**Key Features:**
- One-shot and periodic timers
- std::chrono period specification
- Lambda callback support
- Automatic cleanup

**Example:**
```cpp
freertos::sw_timer timer("MyTimer", std::chrono::seconds(1), true, []() {
    // Timer callback
    printf("Timer fired!\n");
});
timer.start();
```

## Memory Allocation Strategies

All wrapper classes support different allocation strategies through template parameters:

### Dynamic Allocation (Default)

Uses FreeRTOS heap allocation:

```cpp
freertos::task<1024> dynamic_task("Task", 3, task_function);
freertos::binary_semaphore dynamic_sem;
freertos::queue<int, 10> dynamic_queue;
```

### Static Allocation

Uses compile-time allocated memory:

```cpp
freertos::task<1024, freertos::static_task_allocator<1024>> static_task("Task", 3, task_function);
freertos::binary_semaphore<freertos::static_semaphore_allocator> static_sem;
freertos::queue<int, 10, freertos::static_queue_allocator<int, 10>> static_queue;
```

## Common Patterns

### RAII Resource Management

All classes automatically manage their underlying FreeRTOS resources:

```cpp
{
    freertos::binary_semaphore sem;  // Created
    freertos::queue<int, 5> queue;   // Created
    
    // Use sem and queue...
    
}  // sem and queue automatically destroyed here
```

### std::chrono Timeout Handling

All blocking operations support std::chrono durations:

```cpp
using namespace std::chrono_literals;

// Different ways to specify timeouts
sem.take(100ms);                    // 100 milliseconds
sem.take(1s);                       // 1 second
sem.take(std::chrono::seconds(5));  // 5 seconds
```

### Error Handling

Operations that can fail return boolean values:

```cpp
if (queue.send(data, 100ms)) {
    // Success
} else {
    // Timeout or error occurred
}
```

### Move Semantics

Resources can be efficiently transferred:

```cpp
freertos::binary_semaphore create_semaphore() {
    freertos::binary_semaphore sem;
    return sem;  // Moved, not copied
}

auto sem = create_semaphore();  // sem now owns the resource
```

## Integration with Standard Library

The wrappers integrate seamlessly with C++ standard library features:

### Containers

```cpp
// Store wrapper objects in containers
std::vector<freertos::binary_semaphore> semaphores;
semaphores.emplace_back();  // Create semaphore in-place
```

### Algorithms

```cpp
// Use with standard algorithms (where applicable)
std::array<freertos::binary_semaphore, 5> sems;
// Give all semaphores
std::for_each(sems.begin(), sems.end(), [](auto& sem) {
    sem.give();
});
```

### Smart Pointers

```cpp
// Use with smart pointers for dynamic lifetime management
auto task_ptr = std::make_unique<freertos::task<1024>>("Task", 3, task_function);
```

## Thread Safety

All wrapper classes are thread-safe for their intended operations:

- **Creation/Destruction**: Not thread-safe (like standard C++)
- **Operations**: Thread-safe (as per FreeRTOS guarantees)
- **Move Operations**: Not thread-safe (like standard C++)

## Performance Characteristics

The wrappers are designed for zero-overhead:

- **No Virtual Functions**: All operations are direct calls to FreeRTOS
- **Template-Based**: Compile-time optimization opportunities
- **Minimal State**: Only store what's necessary
- **RAII Cleanup**: Automatic, no runtime overhead

## Next Steps

- **[API Reference Overview](../api/overview.md)** - Detailed API documentation
- **[Examples](../examples/basic-usage.md)** - Practical usage examples