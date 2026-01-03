// StopTokenExample.cpp
// Demonstrates std::stop_token (C++20) for graceful thread cancellation
// Shows the difference between old-style atomic flags and modern stop_token
//
// KEY CONCEPTS:
// 1. std::jthread - Automatically joins, supports stop_token
// 2. std::stop_token - Cooperative cancellation signal
// 3. std::stop_source - Controls stop requests
// 4. std::stop_callback - Actions on stop request
//
// USE CASES:
// ✓ REST/Monitor threads (non-critical)
// ✓ Worker threads in thread pools
// ✓ Background tasks that need graceful shutdown
// ✓ Any interruptible operation
//
// ADVANTAGES OVER atomic<bool>:
// ✓ Standard, type-safe
// ✓ Callback support (cleanup on stop)
// ✓ Works with condition_variable_any
// ✓ No manual join() needed with jthread
// ✓ Cannot be accidentally set to true

#include <iostream>
#include <thread>
#include <stop_token>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <atomic>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <functional>

using namespace std::chrono_literals;

// ============================================================================
// SECTION 1: Old Style vs New Style - Comparison
// ============================================================================

namespace old_vs_new {

// OLD STYLE: Using atomic<bool> for cancellation
class OldStyleThread {
private:
    std::atomic<bool> stop_flag_{false};
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    
public:
    void start() {
        thread_ = std::thread([this] {
            std::cout << "  [Old Style] Thread started\n";
            
            while (!stop_flag_.load()) {
                std::unique_lock<std::mutex> lock(mutex_);
                
                // Problem: Can't easily interrupt cv.wait()
                // Must use wait_for() with timeout
                if (cv_.wait_for(lock, 100ms, [this] { return stop_flag_.load(); })) {
                    break;
                }
                
                std::cout << "  [Old Style] Working...\n";
            }
            
            std::cout << "  [Old Style] Thread stopped\n";
        });
    }
    
    void stop() {
        stop_flag_ = true;
        cv_.notify_all();
        if (thread_.joinable()) {
            thread_.join();  // Manual join required
        }
    }
};

// NEW STYLE: Using std::jthread with stop_token
class NewStyleThread {
private:
    std::jthread thread_;  // Automatically joins on destruction
    std::mutex mutex_;
    std::condition_variable_any cv_;  // Note: _any variant for stop_token
    
public:
    void start() {
        thread_ = std::jthread([this](std::stop_token stoken) {
            std::cout << "  [New Style] Thread started\n";
            
            while (!stoken.stop_requested()) {
                std::unique_lock<std::mutex> lock(mutex_);
                
                // Can be interrupted immediately!
                if (cv_.wait(lock, stoken, [] { return false; })) {
                    break;  // Stop requested
                }
                
                std::cout << "  [New Style] Working...\n";
            }
            
            std::cout << "  [New Style] Thread stopped\n";
        });
    }
    
    void stop() {
        thread_.request_stop();  // Signal stop
        // No manual join() needed - jthread does it automatically!
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 1: Old Style vs New Style ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. Old style with atomic<bool>:\n";
    {
        OldStyleThread old_thread;
        old_thread.start();
        std::this_thread::sleep_for(250ms);
        std::cout << "  [Main] Requesting stop...\n";
        old_thread.stop();
    }
    
    std::cout << "\n2. New style with stop_token:\n";
    {
        NewStyleThread new_thread;
        new_thread.start();
        std::this_thread::sleep_for(250ms);
        std::cout << "  [Main] Requesting stop...\n";
        new_thread.stop();
    }
    
    std::cout << "\n✓ ADVANTAGES of stop_token:\n";
    std::cout << "  • Type-safe (can't accidentally set to wrong value)\n";
    std::cout << "  • Works with condition_variable_any for immediate interruption\n";
    std::cout << "  • jthread automatically joins\n";
    std::cout << "  • Standard interface for cancellation\n";
}

} // namespace old_vs_new

// ============================================================================
// SECTION 2: REST Service with stop_token (Non-Critical Thread)
// ============================================================================

namespace rest_service_example {

class RESTService {
private:
    std::jthread thread_;
    std::mutex mutex_;
    std::condition_variable_any cv_;
    std::queue<std::string> request_queue_;
    int request_count_ = 0;
    
public:
    void start() {
        thread_ = std::jthread([this](std::stop_token stoken) {
            std::cout << "  [REST] Service started (non-critical thread)\n";
            
            // Register callback for cleanup
            std::stop_callback callback(stoken, [this] {
                std::cout << "  [REST] Stop requested, cleaning up...\n";
                std::lock_guard<std::mutex> lock(mutex_);
                std::cout << "  [REST] Processed " << request_count_ 
                          << " requests before stop\n";
            });
            
            try {
                while (!stoken.stop_requested()) {
                    std::unique_lock<std::mutex> lock(mutex_);
                    
                    // Wait for requests or stop signal
                    bool stop = cv_.wait(lock, stoken, [this] { 
                        return !request_queue_.empty(); 
                    });
                    
                    if (stop) {
                        std::cout << "  [REST] Stop signal received during wait\n";
                        break;
                    }
                    
                    // Process request
                    if (!request_queue_.empty()) {
                        std::string request = request_queue_.front();
                        request_queue_.pop();
                        lock.unlock();
                        
                        handle_request(request, stoken);
                        
                        lock.lock();
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "  [REST] Exception: " << e.what() << "\n";
                std::cerr << "  [REST] Non-critical thread exiting (core services OK)\n";
            }
            
            std::cout << "  [REST] Service stopped gracefully\n";
        });
    }
    
    void handle_request(const std::string& request, std::stop_token stoken) {
        std::cout << "  [REST] Handling request: " << request << "\n";
        
        // Simulate processing with interruptible sleep
        for (int i = 0; i < 5; ++i) {
            if (stoken.stop_requested()) {
                std::cout << "  [REST] Request handling interrupted\n";
                return;
            }
            
            std::this_thread::sleep_for(100ms);
        }
        
        ++request_count_;
        std::cout << "  [REST] Request completed (total: " << request_count_ << ")\n";
        
        // Simulate occasional error in REST service
        if (request.find("CAUSE_ERROR") != std::string::npos) {
            throw std::runtime_error("REST service error (non-critical)");
        }
    }
    
    void submit_request(const std::string& request) {
        std::lock_guard<std::mutex> lock(mutex_);
        request_queue_.push(request);
        cv_.notify_one();
    }
    
    void stop() {
        std::cout << "  [Main] Requesting REST service stop...\n";
        thread_.request_stop();
        // jthread automatically joins
    }
    
    bool is_running() const {
        return thread_.joinable() && !thread_.get_stop_token().stop_requested();
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 2: REST Service with stop_token ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    RESTService rest;
    rest.start();
    
    std::cout << "Submitting requests...\n";
    rest.submit_request("GET /api/users");
    rest.submit_request("POST /api/data");
    
    std::this_thread::sleep_for(300ms);
    
    std::cout << "\nSimulating graceful shutdown...\n";
    rest.stop();
    
    std::cout << "\n✓ REST service stopped gracefully\n";
    std::cout << "✓ Core services (database, cache) would continue running\n";
}

} // namespace rest_service_example

// ============================================================================
// SECTION 3: Monitoring Service with stop_token
// ============================================================================

namespace monitor_service_example {

class MonitorService {
private:
    std::jthread thread_;
    std::atomic<int> health_checks_{0};
    
public:
    void start() {
        thread_ = std::jthread([this](std::stop_token stoken) {
            std::cout << "  [Monitor] Service started (non-critical)\n";
            
            // Cleanup callback
            std::stop_callback cleanup(stoken, [this] {
                std::cout << "  [Monitor] Performed " << health_checks_.load() 
                          << " health checks\n";
            });
            
            while (!stoken.stop_requested()) {
                // Perform health check
                ++health_checks_;
                std::cout << "  [Monitor] Health check #" << health_checks_.load() 
                          << " - System OK\n";
                
                // Interruptible sleep
                auto wake_time = std::chrono::steady_clock::now() + 1s;
                while (std::chrono::steady_clock::now() < wake_time) {
                    if (stoken.stop_requested()) {
                        std::cout << "  [Monitor] Stop requested during sleep\n";
                        return;
                    }
                    std::this_thread::sleep_for(100ms);
                }
            }
            
            std::cout << "  [Monitor] Service stopped\n";
        });
    }
    
    void stop() {
        thread_.request_stop();
    }
    
    int get_health_checks() const {
        return health_checks_.load();
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 3: Monitoring Service with stop_token ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    MonitorService monitor;
    monitor.start();
    
    std::cout << "Monitoring running...\n";
    std::this_thread::sleep_for(2500ms);
    
    std::cout << "\nStopping monitor service...\n";
    monitor.stop();
    
    std::cout << "\n✓ Total health checks: " << monitor.get_health_checks() << "\n";
}

} // namespace monitor_service_example

// ============================================================================
// SECTION 4: Thread Pool with stop_token
// ============================================================================

namespace thread_pool_example {

class ThreadPool {
private:
    std::vector<std::jthread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable_any cv_;
    
public:
    explicit ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this, i](std::stop_token stoken) {
                std::cout << "  [Worker " << i << "] Started\n";
                
                while (!stoken.stop_requested()) {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        
                        // Wait for task or stop signal
                        bool stop = cv_.wait(lock, stoken, [this] { 
                            return !tasks_.empty(); 
                        });
                        
                        if (stop) {
                            std::cout << "  [Worker " << i << "] Stop requested\n";
                            break;
                        }
                        
                        if (!tasks_.empty()) {
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }
                    }
                    
                    if (task) {
                        std::cout << "  [Worker " << i << "] Executing task\n";
                        task();
                    }
                }
                
                std::cout << "  [Worker " << i << "] Stopped\n";
            });
        }
    }
    
    template<typename F>
    void enqueue(F&& task) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.emplace(std::forward<F>(task));
        }
        cv_.notify_one();
    }
    
    void stop() {
        std::cout << "  [ThreadPool] Requesting stop for all workers...\n";
        for (auto& worker : workers_) {
            worker.request_stop();
        }
        cv_.notify_all();
        // jthread automatically joins all threads
    }
    
    ~ThreadPool() {
        stop();
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 4: Thread Pool with stop_token ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    ThreadPool pool(3);
    
    std::cout << "Enqueueing tasks...\n";
    for (int i = 1; i <= 5; ++i) {
        pool.enqueue([i] {
            std::cout << "    Task " << i << " processing...\n";
            std::this_thread::sleep_for(200ms);
            std::cout << "    Task " << i << " completed\n";
        });
    }
    
    std::this_thread::sleep_for(1s);
    
    std::cout << "\nStopping thread pool...\n";
    pool.stop();
    
    std::cout << "\n✓ All workers stopped gracefully\n";
}

} // namespace thread_pool_example

// ============================================================================
// SECTION 5: stop_callback - Cleanup on Stop
// ============================================================================

namespace stop_callback_example {

class ServiceWithCleanup {
private:
    std::jthread thread_;
    
public:
    void start() {
        thread_ = std::jthread([](std::stop_token stoken) {
            std::cout << "  [Service] Started\n";
            
            // Register multiple callbacks
            std::stop_callback callback1(stoken, [] {
                std::cout << "  [Cleanup 1] Closing network connections...\n";
            });
            
            std::stop_callback callback2(stoken, [] {
                std::cout << "  [Cleanup 2] Flushing buffers...\n";
            });
            
            std::stop_callback callback3(stoken, [] {
                std::cout << "  [Cleanup 3] Saving state...\n";
            });
            
            // Simulate work
            int work_count = 0;
            while (!stoken.stop_requested()) {
                ++work_count;
                std::cout << "  [Service] Working... (" << work_count << ")\n";
                std::this_thread::sleep_for(500ms);
            }
            
            std::cout << "  [Service] Stopped after " << work_count << " iterations\n";
            // Callbacks execute here (in reverse order of registration)
        });
    }
    
    void stop() {
        thread_.request_stop();
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 5: stop_callback for Cleanup ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    ServiceWithCleanup service;
    service.start();
    
    std::this_thread::sleep_for(1500ms);
    
    std::cout << "\nRequesting stop...\n";
    service.stop();
    
    std::cout << "\n✓ Cleanup callbacks executed automatically\n";
}

} // namespace stop_callback_example

// ============================================================================
// SECTION 6: Complete Microservices Example
// ============================================================================

namespace microservices_example {

// Core service (critical - no stop_token, uses exception for abort)
class DatabaseService {
private:
    std::thread thread_;  // Regular thread, not jthread
    std::atomic<bool> running_{false};
    
public:
    void start() {
        running_ = true;
        thread_ = std::thread([this] {
            std::cout << "  [Database] CORE service started (critical)\n";
            
            try {
                while (running_) {
                    std::cout << "  [Database] Processing transactions...\n";
                    std::this_thread::sleep_for(500ms);
                    
                    // Simulate critical error (would call abort())
                    // For demo, we just log it
                }
            }
            catch (const std::exception& e) {
                std::cerr << "  [Database] CRITICAL ERROR: " << e.what() << "\n";
                std::cerr << "  [Database] Would call abort() in real system\n";
            }
            
            std::cout << "  [Database] Core service stopped\n";
        });
    }
    
    void stop() {
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    }
};

// Non-core service (non-critical - uses stop_token)
class MetricsService {
private:
    std::jthread thread_;
    std::atomic<int> metrics_collected_{0};
    
public:
    void start() {
        thread_ = std::jthread([this](std::stop_token stoken) {
            std::cout << "  [Metrics] NON-CORE service started (stop_token)\n";
            
            std::stop_callback cleanup(stoken, [this] {
                std::cout << "  [Metrics] Collected " << metrics_collected_.load() 
                          << " metrics before stop\n";
            });
            
            while (!stoken.stop_requested()) {
                ++metrics_collected_;
                std::cout << "  [Metrics] Collecting metric #" 
                          << metrics_collected_.load() << "\n";
                
                // Interruptible sleep
                auto wake_time = std::chrono::steady_clock::now() + 800ms;
                while (std::chrono::steady_clock::now() < wake_time) {
                    if (stoken.stop_requested()) {
                        return;
                    }
                    std::this_thread::sleep_for(100ms);
                }
            }
        });
    }
    
    void stop() {
        std::cout << "  [Metrics] Requesting graceful stop...\n";
        thread_.request_stop();
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 6: Complete Microservices Example ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    DatabaseService database;
    MetricsService metrics;
    
    std::cout << "Starting services...\n";
    database.start();
    metrics.start();
    
    std::this_thread::sleep_for(2s);
    
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║  Stopping NON-CORE service             ║\n";
    std::cout << "║  (Metrics - uses stop_token)           ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
    
    metrics.stop();
    
    std::cout << "\n✓ Metrics service stopped gracefully\n";
    std::cout << "✓ Database service (core) still running\n";
    
    std::this_thread::sleep_for(1s);
    
    std::cout << "\nStopping all services...\n";
    database.stop();
    
    std::cout << "\n✓ All services stopped\n";
}

} // namespace microservices_example

// ============================================================================
// SECTION 7: Summary and Best Practices
// ============================================================================

void show_summary() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Summary: stop_token for Non-Core Threads ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "┌────────────────────────────────────────────────────────────┐\n";
    std::cout << "│  THREAD TYPE        │  STOP MECHANISM                      │\n";
    std::cout << "├────────────────────────────────────────────────────────────┤\n";
    std::cout << "│  Core (Critical)    │  abort() on exception                │\n";
    std::cout << "│                     │  std::thread + atomic<bool>          │\n";
    std::cout << "│                     │  Ensures entire process stops        │\n";
    std::cout << "├────────────────────────────────────────────────────────────┤\n";
    std::cout << "│  Non-Core           │  std::jthread + stop_token ✓         │\n";
    std::cout << "│  (REST, Monitor)    │  Graceful shutdown                   │\n";
    std::cout << "│                     │  Core services continue running      │\n";
    std::cout << "└────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "WHY stop_token FOR NON-CORE THREADS:\n";
    std::cout << "  ✓ Type-safe cancellation signal\n";
    std::cout << "  ✓ Standard C++20 interface\n";
    std::cout << "  ✓ Works with condition_variable_any for immediate interruption\n";
    std::cout << "  ✓ std::jthread automatically joins (RAII)\n";
    std::cout << "  ✓ stop_callback for automatic cleanup\n";
    std::cout << "  ✓ Cannot accidentally set wrong value (unlike bool)\n\n";
    
    std::cout << "COMPARISON: atomic<bool> vs stop_token\n\n";
    
    std::cout << "  atomic<bool> approach:\n";
    std::cout << "    while (!stop_flag.load()) {\n";
    std::cout << "      cv.wait_for(lock, 100ms, ...);  // Needs timeout\n";
    std::cout << "      // Check flag regularly\n";
    std::cout << "    }\n";
    std::cout << "    // Manual join() required\n\n";
    
    std::cout << "  stop_token approach:\n";
    std::cout << "    std::jthread thread([](std::stop_token stoken) {\n";
    std::cout << "      while (!stoken.stop_requested()) {\n";
    std::cout << "        cv.wait(lock, stoken, ...);  // Immediate interrupt!\n";
    std::cout << "      }\n";
    std::cout << "    });\n";
    std::cout << "    thread.request_stop();  // Automatic join on destruction\n\n";
    
    std::cout << "BEST PRACTICES:\n";
    std::cout << "  1. Use stop_token for all non-critical threads\n";
    std::cout << "  2. Use std::jthread (not std::thread) for automatic joining\n";
    std::cout << "  3. Use condition_variable_any (not condition_variable)\n";
    std::cout << "  4. Check stop_requested() in loops\n";
    std::cout << "  5. Use stop_callback for cleanup logic\n";
    std::cout << "  6. Core services: use abort() for critical failures\n";
    std::cout << "  7. Non-core services: graceful shutdown with stop_token\n\n";
    
    std::cout << "MICROSERVICES PATTERN:\n";
    std::cout << "  • Database, Cache, Auth: CORE (abort on failure)\n";
    std::cout << "  • REST, Metrics, Logs: NON-CORE (stop_token for graceful stop)\n";
    std::cout << "  • Core failure → Process terminates (data integrity)\n";
    std::cout << "  • Non-core failure → Thread exits (core continues)\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║         std::stop_token for Non-Core Thread Cancellation       ║\n";
    std::cout << "║                    C++20 Cooperative Cancellation              ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    // Section 1: Old vs New
    old_vs_new::demonstrate();
    
    // Section 2: REST Service
    rest_service_example::demonstrate();
    
    // Section 3: Monitoring Service
    monitor_service_example::demonstrate();
    
    // Section 4: Thread Pool
    thread_pool_example::demonstrate();
    
    // Section 5: stop_callback
    stop_callback_example::demonstrate();
    
    // Section 6: Complete Example
    microservices_example::demonstrate();
    
    // Section 7: Summary
    show_summary();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All demonstrations completed!\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "KEY TAKEAWAY:\n";
    std::cout << "Use std::stop_token with std::jthread for graceful cancellation\n";
    std::cout << "of non-critical threads (REST, monitoring, metrics).\n";
    std::cout << "Core threads should still use abort() for critical failures.\n\n";
    
    return 0;
}
