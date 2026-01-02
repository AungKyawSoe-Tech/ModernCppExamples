// FuturePromiseAsync.cpp
// Comprehensive guide to C++ concurrency primitives:
// - std::future and std::promise
// - std::packaged_task
// - std::async
// And how they relate to ASIO
//
// KEY CONCEPTS:
// 1. These are NOT alternatives to ASIO - they serve different purposes
// 2. future/promise/async are for TASK-BASED concurrency
// 3. ASIO is for ASYNCHRONOUS I/O and event-driven programming
// 4. They can be used together!
//
// WHEN TO USE WHAT:
// - std::async: Simplest, for fire-and-forget async tasks
// - std::promise/future: Manual control, producer-consumer pattern
// - std::packaged_task: Wrap callable, control execution timing
// - ASIO: Network I/O, timers, serial ports, event-driven architecture

#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <functional>
#include <vector>
#include <queue>
#include <random>
#include <iomanip>

using namespace std::chrono_literals;

// ============================================================================
// SECTION 1: std::async - The Simplest Way
// ============================================================================

namespace async_examples {

// Simple async task
int calculate_sum(int a, int b) {
    std::cout << "  [async] Calculating " << a << " + " << b 
              << " on thread " << std::this_thread::get_id() << "\n";
    std::this_thread::sleep_for(500ms);
    return a + b;
}

// Async task with exception
int divide(int a, int b) {
    std::cout << "  [async] Dividing " << a << " / " << b << "\n";
    std::this_thread::sleep_for(300ms);
    if (b == 0) {
        throw std::invalid_argument("Division by zero!");
    }
    return a / b;
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 1: std::async - The Simplest Way ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Main thread ID: " << std::this_thread::get_id() << "\n\n";
    
    // 1. Launch async task (may run in new thread or deferred)
    std::cout << "1. Basic async (default policy):\n";
    auto future1 = std::async(calculate_sum, 10, 20);
    std::cout << "  Task launched, doing other work...\n";
    std::this_thread::sleep_for(200ms);
    std::cout << "  Getting result: " << future1.get() << "\n\n";
    
    // 2. Force async execution in new thread
    std::cout << "2. Force async (std::launch::async):\n";
    auto future2 = std::async(std::launch::async, calculate_sum, 15, 25);
    std::cout << "  Task running in parallel...\n";
    std::cout << "  Result: " << future2.get() << "\n\n";
    
    // 3. Deferred execution (lazy evaluation)
    std::cout << "3. Deferred execution (std::launch::deferred):\n";
    std::cout << "  Launching deferred task...\n";
    auto future3 = std::async(std::launch::deferred, calculate_sum, 5, 10);
    std::cout << "  Task not started yet!\n";
    std::cout << "  Calling get()... (task runs NOW on this thread)\n";
    std::cout << "  Result: " << future3.get() << "\n\n";
    
    // 4. Exception handling
    std::cout << "4. Exception handling with async:\n";
    auto future4 = std::async(std::launch::async, divide, 100, 0);
    try {
        std::cout << "  Getting result...\n";
        int result = future4.get();
        std::cout << "  Result: " << result << "\n";
    } catch (const std::exception& e) {
        std::cout << "  ✓ Caught exception: " << e.what() << "\n";
    }
    
    std::cout << "\n✓ WHEN TO USE std::async:\n";
    std::cout << "  • Quick fire-and-forget async operations\n";
    std::cout << "  • Simple parallel computations\n";
    std::cout << "  • Don't need fine control over thread management\n";
    std::cout << "  • Want automatic exception propagation\n";
}

} // namespace async_examples

// ============================================================================
// SECTION 2: std::promise and std::future - Producer-Consumer Pattern
// ============================================================================

namespace promise_future_examples {

// Producer thread sets value via promise
void produce_value(std::promise<int> promise, int value) {
    std::cout << "  [Producer] Starting work...\n";
    std::this_thread::sleep_for(1s);
    std::cout << "  [Producer] Producing value: " << value << "\n";
    promise.set_value(value);  // Set the value
    std::cout << "  [Producer] Value set, exiting\n";
}

// Producer that fails
void produce_with_error(std::promise<int> promise) {
    std::cout << "  [Producer] Starting work...\n";
    std::this_thread::sleep_for(500ms);
    try {
        throw std::runtime_error("Production failed!");
    } catch (...) {
        std::cout << "  [Producer] Error occurred, setting exception\n";
        promise.set_exception(std::current_exception());
    }
}

// Multiple consumers waiting for same value
void consume_value(std::shared_future<int> future, int consumer_id) {
    std::cout << "  [Consumer " << consumer_id << "] Waiting for value...\n";
    int value = future.get();  // Blocks until value is ready
    std::cout << "  [Consumer " << consumer_id << "] Got value: " << value << "\n";
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 2: std::promise and std::future ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    // 1. Basic promise-future pair
    std::cout << "1. Basic producer-consumer with promise/future:\n";
    std::promise<int> promise1;
    std::future<int> future1 = promise1.get_future();
    
    std::thread producer1(produce_value, std::move(promise1), 42);
    
    std::cout << "  [Consumer] Waiting for result...\n";
    int result1 = future1.get();
    std::cout << "  [Consumer] Received: " << result1 << "\n\n";
    producer1.join();
    
    // 2. Exception propagation
    std::cout << "2. Exception handling with promise/future:\n";
    std::promise<int> promise2;
    std::future<int> future2 = promise2.get_future();
    
    std::thread producer2(produce_with_error, std::move(promise2));
    
    try {
        std::cout << "  [Consumer] Waiting for result...\n";
        int result2 = future2.get();
        std::cout << "  [Consumer] Received: " << result2 << "\n";
    } catch (const std::exception& e) {
        std::cout << "  [Consumer] ✓ Caught exception: " << e.what() << "\n";
    }
    producer2.join();
    std::cout << "\n";
    
    // 3. Multiple consumers with shared_future
    std::cout << "3. Multiple consumers with shared_future:\n";
    std::promise<int> promise3;
    std::shared_future<int> shared_future = promise3.get_future().share();
    
    // Launch multiple consumers
    std::vector<std::thread> consumers;
    for (int i = 1; i <= 3; ++i) {
        consumers.emplace_back(consume_value, shared_future, i);
    }
    
    std::this_thread::sleep_for(500ms);
    std::cout << "  [Main] Setting value 100\n";
    promise3.set_value(100);
    
    for (auto& t : consumers) {
        t.join();
    }
    
    std::cout << "\n✓ WHEN TO USE std::promise/future:\n";
    std::cout << "  • Need manual control over when value is set\n";
    std::cout << "  • Producer-consumer pattern across threads\n";
    std::cout << "  • Complex synchronization scenarios\n";
    std::cout << "  • Want to set value from a different location than task creation\n";
    std::cout << "  • Need shared_future for multiple consumers\n";
}

} // namespace promise_future_examples

// ============================================================================
// SECTION 3: std::packaged_task - Wrap Callable, Control Execution
// ============================================================================

namespace packaged_task_examples {

// Function to be wrapped
int compute_factorial(int n) {
    std::cout << "  [Task] Computing factorial of " << n << "\n";
    std::this_thread::sleep_for(500ms);
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// Task queue for thread pool simulation
class SimpleThreadPool {
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
    
public:
    SimpleThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            threads_.emplace_back([this, i] {
                std::cout << "  [Worker " << i << "] Started\n";
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        
                        if (stop_ && tasks_.empty()) {
                            std::cout << "  [Worker " << i << "] Stopping\n";
                            return;
                        }
                        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    std::cout << "  [Worker " << i << "] Executing task\n";
                    task();
                }
            });
        }
    }
    
    ~SimpleThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& t : threads_) {
            t.join();
        }
    }
    
    template<typename F>
    void enqueue(F&& task) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.push(std::forward<F>(task));
        }
        cv_.notify_one();
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 3: std::packaged_task ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    // 1. Basic packaged_task
    std::cout << "1. Basic packaged_task:\n";
    std::packaged_task<int(int)> task1(compute_factorial);
    std::future<int> future1 = task1.get_future();
    
    std::cout << "  Task created but not executed yet\n";
    std::cout << "  Launching task on new thread...\n";
    std::thread t1(std::move(task1), 5);
    
    std::cout << "  Waiting for result...\n";
    std::cout << "  Factorial(5) = " << future1.get() << "\n";
    t1.join();
    std::cout << "\n";
    
    // 2. Using packaged_task with thread pool
    std::cout << "2. Thread pool with packaged_task:\n";
    SimpleThreadPool pool(2);
    
    std::vector<std::future<int>> futures;
    
    for (int i = 3; i <= 6; ++i) {
        std::packaged_task<int(int)> task(compute_factorial);
        futures.push_back(task.get_future());
        
        // Enqueue task to thread pool - use shared_ptr for copy-ability
        auto task_ptr = std::make_shared<std::packaged_task<int(int)>>(std::move(task));
        int value = i;
        pool.enqueue([task_ptr, value]() {
            (*task_ptr)(value);
        });
    }
    
    std::cout << "\n  All tasks enqueued, waiting for results...\n\n";
    
    for (size_t i = 0; i < futures.size(); ++i) {
        int result = futures[i].get();
        std::cout << "  Result " << (i + 3) << ": factorial = " << result << "\n";
    }
    
    std::cout << "\n✓ WHEN TO USE std::packaged_task:\n";
    std::cout << "  • Wrapping callable objects for later execution\n";
    std::cout << "  • Implementing thread pools or task queues\n";
    std::cout << "  • Need to separate task creation from execution\n";
    std::cout << "  • Want to store tasks in containers\n";
    std::cout << "  • Building custom scheduling systems\n";
}

} // namespace packaged_task_examples

// ============================================================================
// SECTION 4: Comparison Summary
// ============================================================================

void show_comparison() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 4: When to Use What? ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "┌─────────────────┬─────────────────────────────────────────────┐\n";
    std::cout << "│  MECHANISM      │  USE CASE                                   │\n";
    std::cout << "├─────────────────┼─────────────────────────────────────────────┤\n";
    std::cout << "│  std::async     │  • Simplest way to run async tasks          │\n";
    std::cout << "│                 │  • Fire-and-forget operations               │\n";
    std::cout << "│                 │  • Quick parallel computations              │\n";
    std::cout << "│                 │  • Don't need thread control                │\n";
    std::cout << "│                 │  ✓ Best for: Simple async operations        │\n";
    std::cout << "├─────────────────┼─────────────────────────────────────────────┤\n";
    std::cout << "│  promise/future │  • Producer-consumer pattern                │\n";
    std::cout << "│                 │  • Set value from different location        │\n";
    std::cout << "│                 │  • Complex synchronization                  │\n";
    std::cout << "│                 │  • Multiple consumers (shared_future)       │\n";
    std::cout << "│                 │  ✓ Best for: Manual control over value      │\n";
    std::cout << "├─────────────────┼─────────────────────────────────────────────┤\n";
    std::cout << "│  packaged_task  │  • Thread pools / task queues               │\n";
    std::cout << "│                 │  • Separate creation from execution         │\n";
    std::cout << "│                 │  • Store tasks in containers                │\n";
    std::cout << "│                 │  • Custom scheduling systems                │\n";
    std::cout << "│                 │  ✓ Best for: Building task systems          │\n";
    std::cout << "└─────────────────┴─────────────────────────────────────────────┘\n";
}

// ============================================================================
// SECTION 5: Relationship with ASIO
// ============================================================================

void explain_asio_relationship() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 5: Are They Alternatives to ASIO? ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "SHORT ANSWER: NO - They serve DIFFERENT purposes!\n\n";
    
    std::cout << "┌────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│  future/promise/async (Standard Library Concurrency)          │\n";
    std::cout << "├────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│  PURPOSE:                                                      │\n";
    std::cout << "│    • Task-based concurrency                                    │\n";
    std::cout << "│    • CPU-bound computations                                    │\n";
    std::cout << "│    • Running functions asynchronously                          │\n";
    std::cout << "│    • Getting results from other threads                        │\n";
    std::cout << "│                                                                │\n";
    std::cout << "│  EXAMPLES:                                                     │\n";
    std::cout << "│    • Parallel calculations                                     │\n";
    std::cout << "│    • Image processing in background                            │\n";
    std::cout << "│    • File compression                                          │\n";
    std::cout << "│    • Any CPU-intensive work                                    │\n";
    std::cout << "│                                                                │\n";
    std::cout << "│  LIMITATIONS:                                                  │\n";
    std::cout << "│    ✗ Not designed for I/O operations                           │\n";
    std::cout << "│    ✗ No built-in event loop                                    │\n";
    std::cout << "│    ✗ No socket/network abstractions                            │\n";
    std::cout << "│    ✗ Limited scalability for many I/O operations               │\n";
    std::cout << "└────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│  ASIO (Asynchronous I/O Library)                               │\n";
    std::cout << "├────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│  PURPOSE:                                                      │\n";
    std::cout << "│    • Asynchronous I/O operations                               │\n";
    std::cout << "│    • Network programming (sockets, TCP/UDP)                    │\n";
    std::cout << "│    • Event-driven architecture                                 │\n";
    std::cout << "│    • Timers and signals                                        │\n";
    std::cout << "│    • Serial ports                                              │\n";
    std::cout << "│                                                                │\n";
    std::cout << "│  EXAMPLES:                                                     │\n";
    std::cout << "│    • HTTP/REST servers and clients                             │\n";
    std::cout << "│    • WebSocket servers                                         │\n";
    std::cout << "│    • Chat applications                                         │\n";
    std::cout << "│    • Real-time data streaming                                  │\n";
    std::cout << "│    • Timers and periodic tasks                                 │\n";
    std::cout << "│                                                                │\n";
    std::cout << "│  STRENGTHS:                                                    │\n";
    std::cout << "│    ✓ Optimized for I/O-bound operations                        │\n";
    std::cout << "│    ✓ Event loop / io_context                                   │\n";
    std::cout << "│    ✓ Handles thousands of connections efficiently              │\n";
    std::cout << "│    ✓ Cross-platform I/O abstractions                           │\n";
    std::cout << "│    ✓ Proactor pattern                                          │\n";
    std::cout << "└────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "┌────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│  THEY ARE COMPLEMENTARY - Use Together!                        │\n";
    std::cout << "├────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│                                                                │\n";
    std::cout << "│  EXAMPLE 1: ASIO for I/O + async for CPU work                  │\n";
    std::cout << "│    • Use ASIO to handle HTTP requests                          │\n";
    std::cout << "│    • Use std::async to process images in parallel              │\n";
    std::cout << "│    • Return result via ASIO response                           │\n";
    std::cout << "│                                                                │\n";
    std::cout << "│  EXAMPLE 2: ASIO for networking + packaged_task for jobs       │\n";
    std::cout << "│    • ASIO receives network messages                            │\n";
    std::cout << "│    • Queue CPU-intensive tasks with packaged_task              │\n";
    std::cout << "│    • Thread pool processes tasks                               │\n";
    std::cout << "│    • ASIO sends results back over network                      │\n";
    std::cout << "│                                                                │\n";
    std::cout << "│  EXAMPLE 3: ASIO timers + promise/future for results           │\n";
    std::cout << "│    • ASIO timer for periodic checks                            │\n";
    std::cout << "│    • Use promise/future to coordinate between timers           │\n";
    std::cout << "│    • Combine I/O events with task results                      │\n";
    std::cout << "│                                                                │\n";
    std::cout << "└────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "DECISION TREE:\n";
    std::cout << "  ┌─ Need async I/O (network, files, timers)? ──→ Use ASIO\n";
    std::cout << "  │\n";
    std::cout << "  ├─ Need CPU-bound parallel computation? ──────→ Use std::async\n";
    std::cout << "  │\n";
    std::cout << "  ├─ Need producer-consumer pattern? ───────────→ Use promise/future\n";
    std::cout << "  │\n";
    std::cout << "  ├─ Building thread pool / task queue? ────────→ Use packaged_task\n";
    std::cout << "  │\n";
    std::cout << "  └─ Need both I/O AND computation? ────────────→ Use ASIO + future/async\n";
}

// ============================================================================
// SECTION 6: Practical Example - Combining Them
// ============================================================================

namespace combined_example {

// Simulate a web server scenario
class RequestProcessor {
public:
    // Simulate receiving HTTP request (would use ASIO in real app)
    static std::string receive_request() {
        std::cout << "  [ASIO would be here] Received HTTP request\n";
        return "process_image:photo.jpg";
    }
    
    // CPU-intensive image processing (use std::async)
    static std::string process_image(const std::string& filename) {
        std::cout << "  [Worker] Processing image: " << filename << "\n";
        std::this_thread::sleep_for(1s);  // Simulate heavy processing
        return "processed_" + filename;
    }
    
    // Send response (would use ASIO in real app)
    static void send_response(const std::string& result) {
        std::cout << "  [ASIO would be here] Sending response: " << result << "\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 6: Practical Example - ASIO + async ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Scenario: Web server handling image processing requests\n\n";
    
    // 1. ASIO would receive request (simulated)
    std::string request = RequestProcessor::receive_request();
    
    // 2. Parse request
    auto pos = request.find(':');
    std::string filename = request.substr(pos + 1);
    
    // 3. Use std::async for CPU-intensive work (doesn't block I/O thread)
    std::cout << "  Launching async image processing...\n";
    auto future = std::async(std::launch::async, 
                            RequestProcessor::process_image, 
                            filename);
    
    std::cout << "  ASIO thread is free to handle other requests!\n";
    std::this_thread::sleep_for(200ms);
    std::cout << "  (Handling other requests...)\n";
    std::this_thread::sleep_for(300ms);
    
    // 4. Get result and send response
    std::cout << "  Waiting for processing to complete...\n";
    std::string result = future.get();
    
    // 5. ASIO would send response (simulated)
    RequestProcessor::send_response(result);
    
    std::cout << "\n✓ This is how you combine them:\n";
    std::cout << "  • ASIO handles network I/O (non-blocking)\n";
    std::cout << "  • std::async handles CPU work (parallel)\n";
    std::cout << "  • Best of both worlds!\n";
}

} // namespace combined_example

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     C++ Concurrency: future, promise, async, packaged_task     ║\n";
    std::cout << "║            And their relationship with ASIO                    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    // Section 1: std::async
    async_examples::demonstrate();
    
    // Section 2: promise/future
    promise_future_examples::demonstrate();
    
    // Section 3: packaged_task
    packaged_task_examples::demonstrate();
    
    // Section 4: Comparison
    show_comparison();
    
    // Section 5: ASIO relationship
    explain_asio_relationship();
    
    // Section 6: Combined example
    combined_example::demonstrate();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All demonstrations completed!\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "KEY TAKEAWAYS:\n";
    std::cout << "1. future/promise/async are for TASK-BASED concurrency\n";
    std::cout << "2. ASIO is for ASYNCHRONOUS I/O and event-driven programming\n";
    std::cout << "3. They are NOT alternatives - use them TOGETHER!\n";
    std::cout << "4. std::async: simplest for parallel tasks\n";
    std::cout << "5. promise/future: manual control, producer-consumer\n";
    std::cout << "6. packaged_task: thread pools, task queues\n";
    std::cout << "7. ASIO: network I/O, timers, scalable servers\n\n";
    
    return 0;
}
