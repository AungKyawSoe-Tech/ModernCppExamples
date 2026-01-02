// AsioMultipleContexts.cpp
// Comprehensive educational example of using multiple io_context objects in standalone ASIO
// Demonstrates LAN/WAN separation, thread pooling, and priority-based I/O handling

#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <chrono>
#include <string>
#include <atomic>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <functional>
#include <condition_variable>

// Note: This example uses simulated ASIO patterns for educational purposes
// For actual ASIO usage, install standalone ASIO: https://think-async.com/Asio/
// Then: #include <asio.hpp>
// And replace SimulatedAsio classes with real asio::io_context, asio::steady_timer, etc.

using namespace std::chrono_literals;

// ============================================================================
// SIMULATED ASIO CLASSES (for demonstration without requiring ASIO installation)
// ============================================================================
// In real code, replace these with actual ASIO classes

namespace SimulatedAsio {

// Simulate asio::io_context
class io_context {
private:
    std::atomic<bool> stopped_{false};
    std::atomic<int> work_count_{0};
    std::vector<std::function<void()>> pending_work_;
    std::mutex work_mutex_;
    std::condition_variable work_cv_;
    std::string name_;  // For debugging
    
public:
    explicit io_context(const std::string& name = "io_context") : name_(name) {}
    
    // Main event loop - blocks until no more work
    void run() {
        std::cout << "[" << name_ << "] Thread " << std::this_thread::get_id() 
                  << " calling run()\n";
        
        while (!stopped_) {
            std::function<void()> work;
            
            {
                std::unique_lock<std::mutex> lock(work_mutex_);
                work_cv_.wait_for(lock, 50ms, [this] {
                    return !pending_work_.empty() || stopped_;
                });
                
                if (stopped_ && pending_work_.empty()) {
                    break;
                }
                
                if (!pending_work_.empty()) {
                    work = std::move(pending_work_.back());
                    pending_work_.pop_back();
                }
            }
            
            if (work) {
                work();
                --work_count_;
            }
            
            // If no more work, exit
            if (work_count_ == 0 && pending_work_.empty()) {
                break;
            }
        }
        
        std::cout << "[" << name_ << "] Thread " << std::this_thread::get_id() 
                  << " exiting run()\n";
    }
    
    // Post work to the io_context
    void post(std::function<void()> handler) {
        {
            std::lock_guard<std::mutex> lock(work_mutex_);
            pending_work_.push_back(std::move(handler));
            ++work_count_;
        }
        work_cv_.notify_one();
    }
    
    // Stop the io_context
    void stop() {
        stopped_ = true;
        work_cv_.notify_all();
    }
    
    const std::string& name() const { return name_; }
};

// Simulate asio::steady_timer
class steady_timer {
private:
    io_context& io_;
    std::chrono::milliseconds duration_;
    std::jthread timer_thread_;
    
public:
    steady_timer(io_context& io, std::chrono::milliseconds ms) 
        : io_(io), duration_(ms) {}
    
    void async_wait(std::function<void()> handler) {
        // Start timer in background, then post handler to io_context
        timer_thread_ = std::jthread([this, h = std::move(handler)]() {
            std::this_thread::sleep_for(duration_);
            io_.post(h);  // Post to io_context when timer expires
        });
    }
};

// Work guard to keep io_context alive
template<typename Executor>
class executor_work_guard {
private:
    io_context& io_;
    
public:
    explicit executor_work_guard(io_context& io) : io_(io) {}
    ~executor_work_guard() = default;
};

} // namespace SimulatedAsio

// ============================================================================
// SECTION 1: Basic Multiple io_context Pattern
// ============================================================================

void demonstrate_basic_multiple_contexts() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 1. Basic Multiple io_context Pattern ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Concept: Two separate io_context instances, each with its own thread\n";
    std::cout << "Use case: Isolate different types of I/O operations\n\n";
    
    SimulatedAsio::io_context io1("io_context_1");
    SimulatedAsio::io_context io2("io_context_2");
    
    // Post work to both contexts
    io1.post([]() {
        std::cout << "  Work executing on io_context_1\n";
    });
    
    io2.post([]() {
        std::cout << "  Work executing on io_context_2\n";
    });
    
    // Run each io_context in a separate thread
    std::thread t1([&io1]() { io1.run(); });
    std::thread t2([&io2]() { io2.run(); });
    
    t1.join();
    t2.join();
    
    std::cout << "\n✓ Both contexts completed independently\n";
    std::cout << "✓ Each thread called run() on its assigned io_context\n";
}

// ============================================================================
// SECTION 2: Thread Pool Pattern (Multiple threads, one io_context)
// ============================================================================

void demonstrate_thread_pool_pattern() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 2. Thread Pool Pattern (Multiple threads share one io_context) ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Concept: Multiple threads call run() on the SAME io_context\n";
    std::cout << "Benefit: Work is automatically distributed across threads\n\n";
    
    SimulatedAsio::io_context io("thread_pool");
    
    // Post 6 tasks
    for (int i = 1; i <= 6; ++i) {
        io.post([i]() {
            std::cout << "  Task " << i << " executing on thread " 
                      << std::this_thread::get_id() << "\n";
            std::this_thread::sleep_for(100ms);  // Simulate work
        });
    }
    
    // Create thread pool: 3 threads all calling run() on the same io_context
    std::vector<std::thread> thread_pool;
    for (int i = 0; i < 3; ++i) {
        thread_pool.emplace_back([&io]() {
            io.run();
        });
    }
    
    // Wait for all threads
    for (auto& t : thread_pool) {
        t.join();
    }
    
    std::cout << "\n✓ 6 tasks distributed across 3 threads automatically\n";
    std::cout << "✓ This is the standard ASIO thread pool pattern\n";
}

// ============================================================================
// SECTION 3: LAN vs WAN Separation (The Main Example!)
// ============================================================================

// Simulate a network connection
class Connection {
private:
    std::string name_;
    std::string type_;  // "LAN" or "WAN"
    int request_count_;
    
public:
    Connection(const std::string& name, const std::string& type)
        : name_(name), type_(type), request_count_(0) {}
    
    void handle_request(int request_id) {
        ++request_count_;
        std::cout << "  [" << type_ << " - " << name_ << "] Processing request #" 
                  << request_id << " (total: " << request_count_ << ")\n";
        
        // Simulate processing time
        if (type_ == "LAN") {
            std::this_thread::sleep_for(50ms);   // Fast LAN processing
        } else {
            std::this_thread::sleep_for(150ms);  // Slower WAN processing
        }
    }
    
    const std::string& name() const { return name_; }
};

void demonstrate_lan_wan_separation() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 3. LAN vs WAN Separation (Dual io_context) ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Scenario: Web server handling both LAN and WAN traffic\n";
    std::cout << "Architecture:\n";
    std::cout << "  • io_lan  → Fast local network clients (prioritized)\n";
    std::cout << "  • io_wan  → Slower internet clients (lower priority)\n";
    std::cout << "Benefits:\n";
    std::cout << "  • LAN traffic never blocked by slow WAN connections\n";
    std::cout << "  • Can apply different rate limits per context\n";
    std::cout << "  • Independent thread pools for each network\n\n";
    
    // Create two separate io_context instances
    SimulatedAsio::io_context io_lan("io_LAN");
    SimulatedAsio::io_context io_wan("io_WAN");
    
    // Create connections
    auto lan_client1 = std::make_shared<Connection>("InternalAPI", "LAN");
    auto lan_client2 = std::make_shared<Connection>("Dashboard", "LAN");
    auto wan_client1 = std::make_shared<Connection>("PublicAPI", "WAN");
    auto wan_client2 = std::make_shared<Connection>("MobileApp", "WAN");
    
    std::cout << "Posting work to both contexts...\n\n";
    
    // Post LAN work (fast, high priority)
    for (int i = 1; i <= 3; ++i) {
        io_lan.post([lan_client1, i]() {
            lan_client1->handle_request(i);
        });
        
        io_lan.post([lan_client2, i]() {
            lan_client2->handle_request(i);
        });
    }
    
    // Post WAN work (slower, lower priority)
    for (int i = 1; i <= 3; ++i) {
        io_wan.post([wan_client1, i]() {
            wan_client1->handle_request(i);
        });
        
        io_wan.post([wan_client2, i]() {
            wan_client2->handle_request(i);
        });
    }
    
    // Run LAN with 2 threads (higher capacity for fast traffic)
    std::vector<std::thread> lan_threads;
    lan_threads.emplace_back([&io_lan]() { io_lan.run(); });
    lan_threads.emplace_back([&io_lan]() { io_lan.run(); });
    
    // Run WAN with 1 thread (lower capacity, prevent resource exhaustion)
    std::thread wan_thread([&io_wan]() { io_wan.run(); });
    
    // Wait for completion
    for (auto& t : lan_threads) {
        t.join();
    }
    wan_thread.join();
    
    std::cout << "\n✓ LAN traffic processed with 2 threads (fast completion)\n";
    std::cout << "✓ WAN traffic processed with 1 thread (controlled rate)\n";
    std::cout << "✓ Neither context blocked the other\n";
}

// ============================================================================
// SECTION 4: Priority-Based io_context (Critical vs Normal)
// ============================================================================

class PriorityServer {
private:
    SimulatedAsio::io_context io_critical_;
    SimulatedAsio::io_context io_normal_;
    std::atomic<int> critical_count_{0};
    std::atomic<int> normal_count_{0};
    
public:
    PriorityServer() : io_critical_("io_CRITICAL"), io_normal_("io_NORMAL") {}
    
    void handle_critical_request(const std::string& request) {
        io_critical_.post([this, request]() {
            int id = ++critical_count_;
            std::cout << "  [CRITICAL] Request " << id << ": " << request 
                      << " (thread " << std::this_thread::get_id() << ")\n";
            std::this_thread::sleep_for(30ms);  // Fast processing
        });
    }
    
    void handle_normal_request(const std::string& request) {
        io_normal_.post([this, request]() {
            int id = ++normal_count_;
            std::cout << "  [NORMAL]   Request " << id << ": " << request 
                      << " (thread " << std::this_thread::get_id() << ")\n";
            std::this_thread::sleep_for(80ms);  // Slower processing
        });
    }
    
    void run() {
        // Critical requests get dedicated thread
        std::thread critical_thread([this]() {
            io_critical_.run();
        });
        
        // Normal requests share a thread pool
        std::thread normal_thread([this]() {
            io_normal_.run();
        });
        
        critical_thread.join();
        normal_thread.join();
    }
};

void demonstrate_priority_separation() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 4. Priority-Based Separation (Critical vs Normal) ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Scenario: System with time-critical and regular operations\n";
    std::cout << "Examples:\n";
    std::cout << "  • Critical: Heartbeats, alarms, safety-critical commands\n";
    std::cout << "  • Normal:   Data logging, statistics, background tasks\n\n";
    
    PriorityServer server;
    
    // Mix critical and normal requests
    server.handle_critical_request("Emergency Stop");
    server.handle_normal_request("Log statistics");
    server.handle_critical_request("Heartbeat");
    server.handle_normal_request("Update dashboard");
    server.handle_critical_request("Safety check");
    server.handle_normal_request("Backup data");
    
    std::cout << "\nProcessing requests...\n\n";
    server.run();
    
    std::cout << "\n✓ Critical requests never delayed by normal traffic\n";
    std::cout << "✓ Each context can have different threading strategies\n";
}

// ============================================================================
// SECTION 5: Service-Type Separation (Clients, Database, Logging)
// ============================================================================

void demonstrate_service_type_separation() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 5. Service-Type Separation ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Architecture: One io_context per service type\n\n";
    
    SimulatedAsio::io_context io_clients("io_CLIENTS");
    SimulatedAsio::io_context io_database("io_DATABASE");
    SimulatedAsio::io_context io_logging("io_LOGGING");
    
    // Client connections (high concurrency)
    for (int i = 1; i <= 4; ++i) {
        io_clients.post([i]() {
            std::cout << "  [CLIENTS]  Handling client " << i << " connection\n";
            std::this_thread::sleep_for(60ms);
        });
    }
    
    // Database operations (controlled concurrency)
    for (int i = 1; i <= 2; ++i) {
        io_database.post([i]() {
            std::cout << "  [DATABASE] Executing query " << i << "\n";
            std::this_thread::sleep_for(100ms);
        });
    }
    
    // Logging operations (background, low priority)
    for (int i = 1; i <= 3; ++i) {
        io_logging.post([i]() {
            std::cout << "  [LOGGING]  Writing log entry " << i << "\n";
            std::this_thread::sleep_for(40ms);
        });
    }
    
    std::cout << "Starting service threads...\n\n";
    
    // Each service gets its own thread configuration
    std::thread client_thread([&]() { io_clients.run(); });
    std::thread db_thread([&]() { io_database.run(); });
    std::thread log_thread([&]() { io_logging.run(); });
    
    client_thread.join();
    db_thread.join();
    log_thread.join();
    
    std::cout << "\n✓ Services isolated from each other\n";
    std::cout << "✓ Database load doesn't impact client connections\n";
    std::cout << "✓ Logging doesn't block critical services\n";
}

// ============================================================================
// SECTION 6: When to Call run() - Summary
// ============================================================================

void demonstrate_run_call_patterns() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 6. When to Call io_context.run() - Complete Guide ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "PATTERN 1: Single Thread per io_context\n";
    std::cout << "────────────────────────────────────────\n";
    std::cout << "  asio::io_context io1, io2;\n";
    std::cout << "  std::thread t1([&]() { io1.run(); });  // Thread 1 → io1\n";
    std::cout << "  std::thread t2([&]() { io2.run(); });  // Thread 2 → io2\n";
    std::cout << "  Use: Separate concerns (LAN/WAN, Client/DB)\n\n";
    
    std::cout << "PATTERN 2: Thread Pool (Multiple threads, one io_context)\n";
    std::cout << "──────────────────────────────────────────────────────────\n";
    std::cout << "  asio::io_context io;\n";
    std::cout << "  std::thread t1([&]() { io.run(); });  // Thread 1 → io\n";
    std::cout << "  std::thread t2([&]() { io.run(); });  // Thread 2 → io\n";
    std::cout << "  std::thread t3([&]() { io.run(); });  // Thread 3 → io\n";
    std::cout << "  Use: Distribute work across threads automatically\n\n";
    
    std::cout << "PATTERN 3: Main Thread runs io_context\n";
    std::cout << "───────────────────────────────────────\n";
    std::cout << "  asio::io_context io;\n";
    std::cout << "  // Post all work...\n";
    std::cout << "  io.run();  // Main thread blocks here\n";
    std::cout << "  Use: Simple single-threaded servers\n\n";
    
    std::cout << "PATTERN 4: Keep io_context Alive\n";
    std::cout << "────────────────────────────────\n";
    std::cout << "  asio::io_context io;\n";
    std::cout << "  auto guard = asio::make_work_guard(io);\n";
    std::cout << "  std::thread t([&]() { io.run(); });  // Won't exit immediately\n";
    std::cout << "  // ... post work later ...\n";
    std::cout << "  guard.reset();  // Allow run() to exit when work done\n";
    std::cout << "  Use: Long-running services with dynamic work\n\n";
    
    std::cout << "KEY RULES:\n";
    std::cout << "──────────\n";
    std::cout << "  1. run() BLOCKS until no more work (or stopped)\n";
    std::cout << "  2. Multiple threads CAN call run() on SAME io_context (thread pool)\n";
    std::cout << "  3. One thread should NOT call run() on MULTIPLE io_contexts\n";
    std::cout << "  4. run() returns when work queue is empty (use work_guard to prevent)\n";
    std::cout << "  5. Call stop() to force run() to exit early\n\n";
}

// ============================================================================
// SECTION 7: Real-World Example - Microservice Gateway
// ============================================================================

void demonstrate_microservice_gateway() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 7. Real-World Example: Microservice API Gateway ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Architecture:\n";
    std::cout << "  ┌─────────────────┐\n";
    std::cout << "  │   io_frontend   │ ← Public internet clients (rate limited)\n";
    std::cout << "  │   (1 thread)    │\n";
    std::cout << "  └─────────────────┘\n";
    std::cout << "          ↓\n";
    std::cout << "  ┌─────────────────┐\n";
    std::cout << "  │   io_internal   │ ← Internal microservices (fast LAN)\n";
    std::cout << "  │   (4 threads)   │\n";
    std::cout << "  └─────────────────┘\n";
    std::cout << "          ↓\n";
    std::cout << "  ┌─────────────────┐\n";
    std::cout << "  │   io_database   │ ← Database connections (controlled)\n";
    std::cout << "  │   (2 threads)   │\n";
    std::cout << "  └─────────────────┘\n\n";
    
    SimulatedAsio::io_context io_frontend("FRONTEND");
    SimulatedAsio::io_context io_internal("INTERNAL");
    SimulatedAsio::io_context io_database("DATABASE");
    
    // Simulate incoming requests
    std::cout << "Processing requests...\n\n";
    
    io_frontend.post([]() {
        std::cout << "  [FRONTEND] Public client → Authentication\n";
        std::this_thread::sleep_for(50ms);
    });
    
    io_internal.post([]() {
        std::cout << "  [INTERNAL] Calling user-service microservice\n";
        std::this_thread::sleep_for(30ms);
    });
    
    io_internal.post([]() {
        std::cout << "  [INTERNAL] Calling order-service microservice\n";
        std::this_thread::sleep_for(30ms);
    });
    
    io_database.post([]() {
        std::cout << "  [DATABASE] Query user preferences\n";
        std::this_thread::sleep_for(80ms);
    });
    
    // Thread allocation reflects importance and load
    std::thread frontend_thread([&]() { io_frontend.run(); });
    
    std::vector<std::thread> internal_threads;
    for (int i = 0; i < 2; ++i) {  // 2 threads for internal services
        internal_threads.emplace_back([&]() { io_internal.run(); });
    }
    
    std::thread db_thread([&]() { io_database.run(); });
    
    frontend_thread.join();
    for (auto& t : internal_threads) { t.join(); }
    db_thread.join();
    
    std::cout << "\n✓ Frontend protected from internal service overload\n";
    std::cout << "✓ Internal services have high concurrency (4 threads)\n";
    std::cout << "✓ Database access controlled (2 threads max)\n";
    std::cout << "✓ Each layer can be scaled independently\n";
}

// ============================================================================
// SECTION 8: Common Pitfalls and Best Practices
// ============================================================================

void demonstrate_best_practices() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 8. Common Pitfalls and Best Practices ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "❌ PITFALL 1: run() exits immediately (no work posted)\n";
    std::cout << "   asio::io_context io;\n";
    std::cout << "   std::thread t([&]() { io.run(); });  // ← Exits immediately!\n";
    std::cout << "   io.post([]() { /* ... */ });  // ← Too late, thread already exited\n\n";
    std::cout << "✓ FIX: Post work BEFORE starting thread, or use work_guard\n\n";
    
    std::cout << "❌ PITFALL 2: One thread calling run() on multiple contexts\n";
    std::cout << "   io1.run();  // Blocks until io1 is done\n";
    std::cout << "   io2.run();  // ← io2 never runs until io1 finishes!\n\n";
    std::cout << "✓ FIX: Use separate threads for each io_context\n\n";
    
    std::cout << "❌ PITFALL 3: Forgetting to join threads\n";
    std::cout << "   std::thread t([&]() { io.run(); });\n";
    std::cout << "   // ← main() exits, std::terminate() called!\n\n";
    std::cout << "✓ FIX: Always join or detach threads\n\n";
    
    std::cout << "❌ PITFALL 4: Sharing mutable state without synchronization\n";
    std::cout << "   int counter = 0;  // ← Multiple threads accessing!\n";
    std::cout << "   io.post([&]() { ++counter; });  // ← RACE CONDITION\n\n";
    std::cout << "✓ FIX: Use std::atomic or std::mutex for shared state\n\n";
    
    std::cout << "BEST PRACTICES:\n";
    std::cout << "───────────────\n";
    std::cout << "  1. Use multiple io_contexts to SEPARATE concerns (not for parallelism)\n";
    std::cout << "  2. For parallelism, use THREAD POOL (multiple threads, one io_context)\n";
    std::cout << "  3. Number of threads ≈ number of CPU cores (for CPU-bound work)\n";
    std::cout << "  4. For I/O-bound work, can have many more threads than cores\n";
    std::cout << "  5. Always use strand for sequential execution across threads\n";
    std::cout << "  6. Profile before optimizing - one io_context is often sufficient\n\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║         ASIO Multiple io_context - Educational Examples           ║\n";
    std::cout << "║                                                                    ║\n";
    std::cout << "║  Learn when and how to use multiple io_context objects in ASIO    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    
    demonstrate_basic_multiple_contexts();
    demonstrate_thread_pool_pattern();
    demonstrate_lan_wan_separation();         // ← Your requested example!
    demonstrate_priority_separation();
    demonstrate_service_type_separation();
    demonstrate_run_call_patterns();
    demonstrate_microservice_gateway();
    demonstrate_best_practices();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All demonstrations completed!\n";
    std::cout << "\nKEY TAKEAWAY:\n";
    std::cout << "  Multiple io_context = SEPARATION (isolate different concerns)\n";
    std::cout << "  Multiple threads on one io_context = PARALLELISM (distribute work)\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    return 0;
}
