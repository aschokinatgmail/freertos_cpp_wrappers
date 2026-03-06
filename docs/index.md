# FreeRTOS C++ Wrappers

<div class="hero">
  <div class="hero-content">
    <h1>Modern C++17 RAII Wrappers for FreeRTOS</h1>
    <p class="hero-description">
      Lightweight, type-safe, and RAII-compliant wrappers that make FreeRTOS easier 
      and safer to use in modern C++ applications.
    </p>
  </div>
</div>

## 🚀 Key Features

<div class="feature-grid">
  <div class="feature-card">
    <div class="feature-icon">🛡️</div>
    <div class="feature-content">
      <h3>Type Safety</h3>
      <p>Modern C++17 RAII wrappers ensure proper resource management and type safety</p>
    </div>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">⚡</div>
    <div class="feature-content">
      <h3>High Performance</h3>
      <p>Zero-overhead abstractions that don't compromise on performance</p>
    </div>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">🔧</div>
    <div class="feature-content">
      <h3>Easy to Use</h3>
      <p>Intuitive API design that leverages modern C++ features</p>
    </div>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">🧪</div>
    <div class="feature-content">
      <h3>Well Tested</h3>
      <p>439 comprehensive tests covering all functionality</p>
    </div>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">📱</div>
    <div class="feature-content">
      <h3>Flexible Allocation</h3>
      <p>Support for both static and dynamic memory allocation strategies</p>
    </div>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">⏱️</div>
    <div class="feature-content">
      <h3>Chrono Support</h3>
      <p>Full std::chrono compatibility for timeout handling</p>
    </div>
  </div>
</div>

## 🎯 What Makes This Library Special

- **Modern C++17 RAII wrappers** for all major FreeRTOS APIs
- **Comprehensive test coverage** with 439 automated tests (97.0% line coverage, 98.2% function coverage)
- **Static analysis integration** with clang-tidy and MISRA compliance
- **Dual allocation support** for both static and dynamic strategies
- **std::chrono compatibility** for intuitive timeout handling
- **Move semantics support** for efficient resource management
- **Thread-safe operations** with proper synchronization

## 🚦 Quick Start

Get up and running in just 3 simple steps:

### 1. Include the library header

```cpp
#include "freertos.hpp"
```

### 2. Create and use FreeRTOS objects

```cpp
// Create a task with dynamic allocation
freertos::task<1024> my_task("MyTask", 5, []() {
    while (true) {
        // Your task code here
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
});
```

### 3. Start the scheduler

```cpp
vTaskStartScheduler();
```

[Get started →](getting-started/quick-start.md){ .md-button .md-button--primary }

## 📋 Usage Examples

<div class="example-preview">

### Task Management
```cpp
#include "freertos_task.hpp"

// Create a task with dynamic allocation (1024 bytes stack)
freertos::task<1024> worker_task("WorkerTask", 3, []() {
    while (true) {
        // Perform work
        printf("Worker task running\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
});

// Create a periodic task that runs every 100ms
freertos::periodic_task<512> timer_task("TimerTask", 2, 100ms, []() {
    static int counter = 0;
    printf("Periodic task execution #%d\n", ++counter);
});
```

### Synchronization with Semaphores
```cpp
#include "freertos_semaphore.hpp"

// Binary semaphore for signaling
freertos::binary_semaphore signal_sem;

// Producer task
freertos::task<512> producer("Producer", 4, [&]() {
    while (true) {
        produce_data();
        signal_sem.give(); // Signal consumer
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
});

// Consumer task  
freertos::task<512> consumer("Consumer", 3, [&]() {
    while (true) {
        // Wait for signal with timeout
        if (signal_sem.take(std::chrono::seconds(1))) {
            consume_data();
        } else {
            printf("Timeout waiting for data\n");
        }
    }
});
```

### Type-Safe Message Passing
```cpp
#include "freertos_queue.hpp"

struct Message {
    int id;
    std::string data;
};

// Create a queue for 10 messages
freertos::queue<Message, 10> msg_queue;

// Send messages with timeout
Message msg{1, "Hello from sender"};
if (msg_queue.send(msg, std::chrono::milliseconds(100))) {
    printf("Message sent successfully\n");
}

// Receive messages with timeout
Message received_msg;
if (msg_queue.receive(received_msg, std::chrono::seconds(2))) {
    printf("Received: ID=%d, Data=%s\n", received_msg.id, received_msg.data.c_str());
}
```

</div>

[View more examples →](examples/basic-usage.md){ .md-button }

## 📊 Quality Metrics

<div class="metrics-grid">
  <div class="metric-card">
    <div class="metric-number">439</div>
    <div class="metric-label">Automated Tests</div>
    <div class="metric-description">100% Passing</div>
  </div>
  
  <div class="metric-card">
    <div class="metric-number">97.0%</div>
    <div class="metric-label">Line Coverage</div>
    <div class="metric-description">High Quality</div>
  </div>
  
  <div class="metric-card">
    <div class="metric-number">98.2%</div>
    <div class="metric-label">Function Coverage</div>
    <div class="metric-description">Comprehensive</div>
  </div>
  
  <div class="metric-card">
    <div class="metric-number">0</div>
    <div class="metric-label">Critical Issues</div>
    <div class="metric-description">Clean Code</div>
  </div>
</div>

## 🏗️ Memory Allocation Strategies

The library supports both static and dynamic memory allocation strategies:

=== "Dynamic Allocation (Default)"

    ```cpp
    // Uses FreeRTOS heap
    freertos::task<1024> dynamic_task("Task", 3, task_function);
    freertos::binary_semaphore dynamic_sem;
    freertos::queue<int, 10> dynamic_queue;
    ```

=== "Static Allocation"

    ```cpp
    // Uses stack-allocated memory
    freertos::task<1024, freertos::static_task_allocator<1024>> static_task("Task", 3, task_function);
    freertos::binary_semaphore<freertos::static_semaphore_allocator> static_sem;
    freertos::queue<int, 10, freertos::static_queue_allocator<int, 10>> static_queue;
    ```

## 🛠️ Building the Library

### Prerequisites
- C++17 compatible compiler
- CMake 3.12+
- FreeRTOS source code

### Build Configuration
```bash
git clone https://github.com/aschokinatgmail/freertos_cpp_wrappers
cd freertos_cpp_wrappers
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON -DENABLE_CLANG_TIDY=ON ..
make -j$(nproc)
```

[Detailed build instructions →](getting-started/building.md){ .md-button }

## 📚 API Reference

Explore the comprehensive API documentation:

<div class="api-grid">
  <a href="api/tasks/" class="api-card">
    <div class="api-icon">⚙️</div>
    <div class="api-title">Task Management</div>
    <div class="api-description">Create and manage FreeRTOS tasks</div>
  </a>
  
  <a href="api/semaphores/" class="api-card">
    <div class="api-icon">🔒</div>
    <div class="api-title">Semaphores</div>
    <div class="api-description">Binary, counting, and mutex semaphores</div>
  </a>
  
  <a href="api/queues/" class="api-card">
    <div class="api-icon">📬</div>
    <div class="api-title">Queues</div>
    <div class="api-description">Type-safe message passing</div>
  </a>
  
  <a href="api/event-groups/" class="api-card">
    <div class="api-icon">🎭</div>
    <div class="api-title">Event Groups</div>
    <div class="api-description">Multi-event synchronization</div>
  </a>
  
  <a href="api/stream-buffers/" class="api-card">
    <div class="api-icon">🌊</div>
    <div class="api-title">Stream Buffers</div>
    <div class="api-description">Byte stream communication</div>
  </a>
  
  <a href="api/message-buffers/" class="api-card">
    <div class="api-icon">💌</div>
    <div class="api-title">Message Buffers</div>
    <div class="api-description">Variable-length message passing</div>
  </a>
</div>

## 🤝 Contributing

We welcome contributions! Check out our [GitHub repository](https://github.com/aschokinatgmail/freertos_cpp_wrappers) to get started.

## 📄 License

This library is licensed under the MIT License. See the [LICENSE](https://github.com/aschokinatgmail/freertos_cpp_wrappers/blob/main/LICENSE) file for details.

---

<div class="footer-links">
  <a href="https://github.com/aschokinatgmail/freertos_cpp_wrappers">View on GitHub</a> •
  <a href="https://github.com/aschokinatgmail/freertos_cpp_wrappers/issues">Report Issues</a> •  
  <a href="contributing/guidelines/">Contribute</a>
</div>