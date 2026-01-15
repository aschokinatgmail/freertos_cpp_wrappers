# Basic Usage Examples

This page demonstrates fundamental usage patterns of the FreeRTOS C++ Wrappers library.

## Task Creation and Management

### Simple Worker Task

The most basic usage pattern is creating a task that performs work in a loop:

```cpp title="basic_task_example.cpp"
#include "freertos.hpp"
#include <iostream>

int main() {
    // Create a simple worker task
    freertos::task<1024> worker_task("Worker", 3, []() {
        int counter = 0;
        while (true) {
            std::cout << "Worker task iteration: " << ++counter << std::endl;
            
            // Sleep for 1 second using std::chrono
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    // Create a periodic task that runs every 500ms
    freertos::periodic_task<512> timer_task("Timer", 2, 
        std::chrono::milliseconds(500), []() {
        static int timer_count = 0;
        std::cout << "Timer task execution #" << ++timer_count << std::endl;
    });

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();
    return 0;
}
```

### Task with Function Pointer

You can also use regular functions instead of lambdas:

```cpp
void sensor_task_function() {
    while (true) {
        // Read sensor data
        float temperature = read_temperature_sensor();
        float humidity = read_humidity_sensor();
        
        std::cout << "Temperature: " << temperature << "°C, "
                  << "Humidity: " << humidity << "%" << std::endl;
        
        // Wait 2 seconds before next reading
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

int main() {
    // Create task using function pointer
    freertos::task<1024> sensor_task("SensorTask", 4, sensor_task_function);
    
    vTaskStartScheduler();
    return 0;
}
```

## Synchronization Primitives

### Binary Semaphores for Signaling

Binary semaphores are perfect for simple signaling between tasks:

```cpp
#include "freertos.hpp"
#include <iostream>

freertos::binary_semaphore data_ready_signal;

void producer_task() {
    int data_id = 0;
    while (true) {
        // Simulate data production
        std::cout << "Producing data packet " << ++data_id << std::endl;
        
        // Simulate work time
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        
        // Signal that data is ready
        data_ready_signal.give();
    }
}

void consumer_task() {
    while (true) {
        // Wait for data ready signal with 2 second timeout
        if (data_ready_signal.take(std::chrono::seconds(2))) {
            std::cout << "Processing received data..." << std::endl;
            
            // Simulate processing time
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            
            std::cout << "Data processed successfully" << std::endl;
        } else {
            std::cout << "Timeout waiting for data" << std::endl;
        }
    }
}

int main() {
    freertos::task<1024> producer("Producer", 3, producer_task);
    freertos::task<1024> consumer("Consumer", 2, consumer_task);
    
    vTaskStartScheduler();
    return 0;
}
```

### Counting Semaphores for Resource Management

Counting semaphores can manage access to multiple instances of a resource:

```cpp
#include "freertos.hpp"
#include <iostream>

// Allow up to 3 concurrent database connections
freertos::counting_semaphore<3> db_connections(3);

void database_task(int task_id) {
    while (true) {
        // Try to acquire a database connection
        if (db_connections.take(std::chrono::seconds(1))) {
            std::cout << "Task " << task_id << " acquired DB connection" << std::endl;
            
            // Simulate database work
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            std::cout << "Task " << task_id << " releasing DB connection" << std::endl;
            
            // Release the connection
            db_connections.give();
        } else {
            std::cout << "Task " << task_id << " - no DB connections available" << std::endl;
        }
        
        // Wait before trying again
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main() {
    // Create 5 tasks that compete for 3 database connections
    freertos::task<512> task1("DBTask1", 2, [](){ database_task(1); });
    freertos::task<512> task2("DBTask2", 2, [](){ database_task(2); });
    freertos::task<512> task3("DBTask3", 2, [](){ database_task(3); });
    freertos::task<512> task4("DBTask4", 2, [](){ database_task(4); });
    freertos::task<512> task5("DBTask5", 2, [](){ database_task(5); });
    
    vTaskStartScheduler();
    return 0;
}
```

## Thread-Safe Data Sharing

### Mutex with RAII Lock Guards

Mutexes protect shared resources from concurrent access:

```cpp
#include "freertos.hpp"
#include <iostream>

class SharedCounter {
private:
    freertos::mutex counter_mutex;
    int counter = 0;

public:
    void increment() {
        // RAII lock guard automatically unlocks when leaving scope
        freertos::lock_guard<freertos::mutex> lock(counter_mutex);
        ++counter;
        std::cout << "Counter incremented to: " << counter << std::endl;
    }
    
    bool try_increment() {
        // Try to acquire lock without blocking
        freertos::try_lock_guard<freertos::mutex> lock(counter_mutex);
        if (lock.owns_lock()) {
            ++counter;
            std::cout << "Counter incremented to: " << counter << std::endl;
            return true;
        }
        return false;
    }
    
    bool timed_increment() {
        // Try to acquire lock with timeout
        freertos::timeout_lock_guard<freertos::mutex> lock(
            counter_mutex, std::chrono::milliseconds(100));
        if (lock.owns_lock()) {
            ++counter;
            std::cout << "Counter incremented to: " << counter << std::endl;
            return true;
        }
        return false;
    }
    
    int get_value() {
        freertos::lock_guard<freertos::mutex> lock(counter_mutex);
        return counter;
    }
};

SharedCounter shared_counter;

void incrementer_task(int task_id) {
    while (true) {
        switch (task_id % 3) {
            case 0:
                shared_counter.increment();
                break;
            case 1:
                if (!shared_counter.try_increment()) {
                    std::cout << "Task " << task_id << " - mutex busy" << std::endl;
                }
                break;
            case 2:
                if (!shared_counter.timed_increment()) {
                    std::cout << "Task " << task_id << " - timeout acquiring mutex" << std::endl;
                }
                break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

int main() {
    freertos::task<512> task1("Inc1", 2, [](){ incrementer_task(1); });
    freertos::task<512> task2("Inc2", 2, [](){ incrementer_task(2); });
    freertos::task<512> task3("Inc3", 2, [](){ incrementer_task(3); });
    
    vTaskStartScheduler();
    return 0;
}
```

## Type-Safe Message Passing

### Simple Queue Example

Queues provide type-safe message passing between tasks:

```cpp
#include "freertos.hpp"
#include <iostream>
#include <string>

struct LogMessage {
    int level;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
};

// Queue for 20 log messages
freertos::queue<LogMessage, 20> log_queue;

void application_task() {
    int message_count = 0;
    while (true) {
        // Create a log message
        LogMessage msg{
            message_count % 4,  // Log level (0-3)
            "Application message #" + std::to_string(message_count),
            std::chrono::system_clock::now()
        };
        
        // Send message with timeout
        if (log_queue.send(msg, std::chrono::milliseconds(100))) {
            std::cout << "Sent log message " << message_count << std::endl;
        } else {
            std::cout << "Failed to send message " << message_count 
                      << " - queue full" << std::endl;
        }
        
        ++message_count;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void logger_task() {
    const char* level_names[] = {"DEBUG", "INFO", "WARN", "ERROR"};
    
    while (true) {
        LogMessage msg;
        
        // Wait for messages with 1 second timeout
        if (log_queue.receive(msg, std::chrono::seconds(1))) {
            auto time = std::chrono::system_clock::to_time_t(msg.timestamp);
            
            std::cout << "[" << level_names[msg.level] << "] " 
                      << msg.message << " (at " << std::ctime(&time) << ")" << std::endl;
        } else {
            std::cout << "Logger: No messages received" << std::endl;
        }
    }
}

int main() {
    freertos::task<1024> app_task("AppTask", 3, application_task);
    freertos::task<1024> log_task("LogTask", 2, logger_task);
    
    vTaskStartScheduler();
    return 0;
}
```

## Using std::chrono for Timeouts

The library fully supports `std::chrono` for intuitive timeout handling:

```cpp
#include "freertos.hpp"
#include <chrono>

using namespace std::chrono_literals;

void timeout_examples() {
    freertos::binary_semaphore sem;
    freertos::queue<int, 10> queue;
    
    // Various timeout specifications
    sem.take(100ms);                    // 100 milliseconds
    sem.take(1s);                       // 1 second  
    sem.take(std::chrono::seconds(5));  // 5 seconds
    
    int value;
    queue.receive(value, 500ms);        // 500 milliseconds
    queue.send(42, 2s);                 // 2 seconds
    
    // No timeout (block indefinitely)
    sem.take();
    queue.receive(value);
}
```

## Best Practices

### Resource Management

!!! tip "Use RAII"
    Always prefer RAII wrappers over raw FreeRTOS handles. The wrappers automatically handle resource cleanup.

```cpp
// Good - RAII wrapper handles cleanup
{
    freertos::binary_semaphore sem;
    // sem is automatically cleaned up when going out of scope
}

// Avoid - manual handle management
SemaphoreHandle_t sem = xSemaphoreCreateBinary();
// Need to manually call vSemaphoreDelete(sem)
```

### Error Handling

!!! tip "Check Return Values"
    Always check return values from operations that can fail, especially those with timeouts.

```cpp
// Good - check if operation succeeded
if (queue.send(data, 100ms)) {
    // Success
} else {
    // Handle timeout or error
}

// Avoid - ignoring potential failures
queue.send(data, 100ms);  // What if it fails?
```

### Task Stack Sizes

!!! warning "Choose Appropriate Stack Sizes"
    Stack sizes are specified in bytes. Be generous but not wasteful.

```cpp
// Small stack for simple tasks
freertos::task<256> simple_task("Simple", 1, simple_function);

// Larger stack for complex operations
freertos::task<2048> complex_task("Complex", 3, complex_function);

// Very large stack for tasks using C++ standard library extensively
freertos::task<4096> std_task("StdTask", 2, std_heavy_function);
```

## Next Steps

Now that you understand the basics:

- **[Basic Usage Examples](../examples/basic-usage.md)** - Learn fundamental patterns
- **[API Reference](../api/overview.md)** - Complete API documentation