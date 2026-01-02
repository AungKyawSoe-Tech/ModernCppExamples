// MultiThreadedMicroservices.cpp
// Demonstrates multi-threaded microservices architecture with different
// exception handling strategies: REST threads use pthread_exit(), core threads use abort()

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <csignal>
#include <functional>

#ifdef __linux__
    #include <pthread.h>
    #include <execinfo.h>
    #include <cxxabi.h>
#elif _WIN32
    #include <windows.h>
    #include <dbghelp.h>
#endif

using namespace std::chrono;
using namespace std::chrono_literals;

// ============================================================================
// SECTION 1: Stack Trace and Logging Infrastructure
// ============================================================================

class Logger {
private:
    static std::mutex log_mutex_;
    
    static std::string get_timestamp() {
        auto now = system_clock::now();
        auto time = system_clock::to_time_t(now);
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%H:%M:%S")
            << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }
    
    static std::string get_thread_id() {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        return oss.str();
    }
    
public:
    enum Level { INFO, WARNING, ERROR, CRITICAL };
    
    static void log(Level level, const std::string& message, 
                   const std::string& thread_name = "") {
        std::lock_guard<std::mutex> lock(log_mutex_);
        
        const char* level_str[] = {"INFO", "WARN", "ERROR", "CRIT"};
        const char* color[] = {"\033[32m", "\033[33m", "\033[31m", "\033[35m"};
        const char* reset = "\033[0m";
        
        std::cout << "[" << get_timestamp() << "] "
                  << color[level] << "[" << level_str[level] << "]" << reset
                  << " [TID:" << get_thread_id().substr(0, 6) << "]";
        
        if (!thread_name.empty()) {
            std::cout << " [" << thread_name << "]";
        }
        
        std::cout << " " << message << "\n";
    }
    
    static std::vector<std::string> capture_stack_trace() {
        std::vector<std::string> frames;
        
#ifdef __linux__
        void* buffer[32];
        int frame_count = backtrace(buffer, 32);
        char** symbols = backtrace_symbols(buffer, frame_count);
        
        if (symbols) {
            for (int i = 0; i < frame_count && i < 10; ++i) {
                std::string frame = symbols[i];
                
                // Try to demangle
                const char* begin = strchr(symbols[i], '(');
                const char* end = strchr(symbols[i], '+');
                
                if (begin && end && begin < end) {
                    begin++;
                    std::string mangled(begin, end - begin);
                    int status;
                    char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
                    if (status == 0 && demangled) {
                        frame = demangled;
                        free(demangled);
                    }
                }
                
                frames.push_back(frame);
            }
            free(symbols);
        }
#elif _WIN32
        void* buffer[32];
        HANDLE process = GetCurrentProcess();
        SymInitialize(process, NULL, TRUE);
        
        WORD frame_count = CaptureStackBackTrace(0, 32, buffer, NULL);
        SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256, 1);
        
        if (symbol) {
            symbol->MaxNameLen = 255;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            
            for (WORD i = 0; i < frame_count && i < 10; ++i) {
                if (SymFromAddr(process, (DWORD64)buffer[i], 0, symbol)) {
                    frames.push_back(symbol->Name);
                } else {
                    std::ostringstream oss;
                    oss << "0x" << std::hex << (uint64_t)buffer[i];
                    frames.push_back(oss.str());
                }
            }
            free(symbol);
        }
        SymCleanup(process);
#else
        frames.push_back("[Stack trace not available]");
#endif
        
        return frames;
    }
    
    static void log_exception_with_stack(const std::exception& e, 
                                         const std::string& thread_name) {
        log(CRITICAL, std::string("EXCEPTION: ") + e.what(), thread_name);
        
        auto stack = capture_stack_trace();
        if (!stack.empty()) {
            log(CRITICAL, "Stack trace:", thread_name);
            for (size_t i = 0; i < stack.size(); ++i) {
                std::ostringstream oss;
                oss << "  #" << i << ": " << stack[i];
                log(CRITICAL, oss.str(), thread_name);
            }
        }
    }
};

std::mutex Logger::log_mutex_;

// ============================================================================
// SECTION 2: Thread Type Identification and Exception Policies
// ============================================================================

enum class ThreadType {
    CORE_SERVICE,    // Critical - abort() on exception
    REST_SERVICE,    // Non-critical - pthread_exit() on exception
    MONITORING       // Non-critical - pthread_exit() on exception
};

class ThreadContext {
private:
    ThreadType type_;
    std::string name_;
    
public:
    ThreadContext(ThreadType type, const std::string& name)
        : type_(type), name_(name) {}
    
    ThreadType get_type() const { return type_; }
    const std::string& get_name() const { return name_; }
    
    bool is_core_service() const { return type_ == ThreadType::CORE_SERVICE; }
    bool is_rest_service() const { return type_ == ThreadType::REST_SERVICE; }
    
    void handle_exception(const std::exception& e) {
        Logger::log_exception_with_stack(e, name_);
        
        if (is_core_service()) {
            Logger::log(Logger::CRITICAL, 
                       "CORE SERVICE FAILURE - Calling abort() to terminate application!", 
                       name_);
            std::cerr << "\n╔════════════════════════════════════════╗\n";
            std::cerr << "║   CRITICAL: CORE SERVICE CRASHED       ║\n";
            std::cerr << "║   Terminating entire application       ║\n";
            std::cerr << "╚════════════════════════════════════════╝\n\n";
            
            std::abort();  // Terminate entire process
        } else {
            Logger::log(Logger::ERROR, 
                       "REST/MONITORING SERVICE FAILURE - Exiting thread only (core services continue)", 
                       name_);
            std::cerr << "\n╔════════════════════════════════════════╗\n";
            std::cerr << "║   REST/Monitor thread exiting          ║\n";
            std::cerr << "║   Core services still running OK       ║\n";
            std::cerr << "╚════════════════════════════════════════╝\n\n";
            
#ifdef __linux__
            pthread_exit(nullptr);  // Exit only this thread
#elif _WIN32
            ExitThread(1);  // Windows equivalent
#else
            std::this_thread::sleep_for(1s);
            throw;  // Fallback
#endif
        }
    }
};

// Thread-local storage for thread context
thread_local std::unique_ptr<ThreadContext> g_thread_context;

// ============================================================================
// SECTION 3: Simple JSON Parser (for demonstration)
// ============================================================================

class JsonParseException : public std::runtime_error {
public:
    explicit JsonParseException(const std::string& msg) 
        : std::runtime_error("JSON Parse Error: " + msg) {}
};

class SimpleJson {
private:
    std::string data_;
    
public:
    explicit SimpleJson(const std::string& json_str) : data_(json_str) {
        validate();
    }
    
    void validate() const {
        // Simple validation
        if (data_.empty()) {
            throw JsonParseException("Empty JSON string");
        }
        
        // Must start with { and end with }
        size_t start = data_.find_first_not_of(" \t\n\r");
        size_t end = data_.find_last_not_of(" \t\n\r");
        
        if (start == std::string::npos || end == std::string::npos) {
            throw JsonParseException("Invalid JSON: whitespace only");
        }
        
        if (data_[start] != '{' || data_[end] != '}') {
            throw JsonParseException("Invalid JSON: must start with { and end with }");
        }
        
        // Count braces
        int brace_count = 0;
        for (char c : data_) {
            if (c == '{') brace_count++;
            if (c == '}') brace_count--;
            if (brace_count < 0) {
                throw JsonParseException("Invalid JSON: unmatched closing brace");
            }
        }
        
        if (brace_count != 0) {
            throw JsonParseException("Invalid JSON: unmatched opening brace");
        }
    }
    
    std::string get_field(const std::string& field_name) const {
        std::string search = "\"" + field_name + "\"";
        size_t pos = data_.find(search);
        
        if (pos == std::string::npos) {
            throw JsonParseException("Field not found: " + field_name);
        }
        
        // Find value after colon
        size_t colon = data_.find(':', pos);
        if (colon == std::string::npos) {
            throw JsonParseException("Malformed field: " + field_name);
        }
        
        size_t value_start = data_.find_first_not_of(" \t\n\r", colon + 1);
        size_t value_end = data_.find_first_of(",}", value_start);
        
        if (value_start == std::string::npos || value_end == std::string::npos) {
            throw JsonParseException("Cannot extract value for: " + field_name);
        }
        
        std::string value = data_.substr(value_start, value_end - value_start);
        
        // Remove quotes if present
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }
        
        return value;
    }
    
    const std::string& raw() const { return data_; }
};

// ============================================================================
// SECTION 4: Core Services (Critical - abort on exception)
// ============================================================================

class DatabaseService {
private:
    std::atomic<bool> running_{false};
    std::atomic<int> query_count_{0};
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::string> query_queue_;
    
public:
    void start() {
        running_ = true;
        
        g_thread_context = std::make_unique<ThreadContext>(
            ThreadType::CORE_SERVICE, "DatabaseService");
        
        Logger::log(Logger::INFO, "Database service started", "DatabaseService");
        
        try {
            while (running_) {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait_for(lock, 500ms, [this] { 
                    return !query_queue_.empty() || !running_; 
                });
                
                if (!running_) break;
                
                while (!query_queue_.empty()) {
                    std::string query = query_queue_.front();
                    query_queue_.pop();
                    lock.unlock();
                    
                    execute_query(query);
                    
                    lock.lock();
                }
            }
        }
        catch (const std::exception& e) {
            g_thread_context->handle_exception(e);
        }
        
        Logger::log(Logger::INFO, "Database service stopped", "DatabaseService");
    }
    
    void execute_query(const std::string& query) {
        query_count_++;
        
        // Simulate query execution
        std::this_thread::sleep_for(50ms);
        
        Logger::log(Logger::INFO, 
                   "Executed query #" + std::to_string(query_count_.load()) + ": " + query,
                   "DatabaseService");
        
        // Simulate critical error in core service (triggered by special query)
        if (query.find("TRIGGER_CORE_FAILURE") != std::string::npos) {
            Logger::log(Logger::ERROR, 
                       "⚠️  SIMULATING CRITICAL DATABASE CORRUPTION!", 
                       "DatabaseService");
            std::this_thread::sleep_for(100ms);
            throw std::runtime_error("CRITICAL: Database corruption detected! Data integrity compromised!");
        }
    }
    
    void submit_query(const std::string& query) {
        std::lock_guard<std::mutex> lock(mutex_);
        query_queue_.push(query);
        cv_.notify_one();
    }
    
    void stop() {
        running_ = false;
        cv_.notify_all();
    }
    
    int get_query_count() const { return query_count_.load(); }
};

class CacheService {
private:
    std::atomic<bool> running_{false};
    std::atomic<int> cache_hits_{0};
    std::atomic<int> cache_misses_{0};
    
public:
    void start() {
        running_ = true;
        
        g_thread_context = std::make_unique<ThreadContext>(
            ThreadType::CORE_SERVICE, "CacheService");
        
        Logger::log(Logger::INFO, "Cache service started", "CacheService");
        
        try {
            while (running_) {
                std::this_thread::sleep_for(1s);
                
                // Simulate cache maintenance
                if (cache_hits_ + cache_misses_ > 0) {
                    int total = cache_hits_ + cache_misses_;
                    float hit_rate = (100.0f * cache_hits_) / total;
                    
                    std::ostringstream oss;
                    oss << "Cache stats: " << cache_hits_.load() << " hits, "
                        << cache_misses_.load() << " misses (hit rate: "
                        << std::fixed << std::setprecision(1) << hit_rate << "%)";
                    
                    Logger::log(Logger::INFO, oss.str(), "CacheService");
                }
            }
        }
        catch (const std::exception& e) {
            g_thread_context->handle_exception(e);
        }
        
        Logger::log(Logger::INFO, "Cache service stopped", "CacheService");
    }
    
    bool get(const std::string& key) {
        // Simulate cache lookup
        bool hit = (std::hash<std::string>{}(key) % 3) == 0;
        
        if (hit) {
            cache_hits_++;
        } else {
            cache_misses_++;
        }
        
        return hit;
    }
    
    void stop() {
        running_ = false;
    }
};

// ============================================================================
// SECTION 5: REST Service (Non-critical - pthread_exit on exception)
// ============================================================================

class RestApiService {
private:
    std::atomic<bool> running_{false};
    std::atomic<int> request_count_{0};
    std::atomic<int> error_count_{0};
    DatabaseService& db_;
    CacheService& cache_;
    
public:
    RestApiService(DatabaseService& db, CacheService& cache)
        : db_(db), cache_(cache) {}
    
    void start() {
        running_ = true;
        
        g_thread_context = std::make_unique<ThreadContext>(
            ThreadType::REST_SERVICE, "RestApiService");
        
        Logger::log(Logger::INFO, "REST API service started on port 8080 (simulated)", 
                   "RestApiService");
        
        try {
            simulate_rest_requests();
        }
        catch (const std::exception& e) {
            g_thread_context->handle_exception(e);
        }
        
        Logger::log(Logger::INFO, "REST API service stopped", "RestApiService");
    }
    
    void simulate_rest_requests() {
        // Simulate various REST requests
        std::vector<std::string> requests = {
            R"({"action": "get_user", "user_id": "123"})",
            R"({"action": "create_order", "product": "laptop", "quantity": 1})",
            R"({"action": "update_profile", "name": "John Doe"})",
            R"({invalid json - missing closing brace)",  // This will cause exception!
            R"({"action": "delete_item", "item_id": "456"})",
            R"()",  // Empty - will cause exception!
        };
        
        int request_num = 0;
        
        while (running_ && request_num < requests.size()) {
            std::this_thread::sleep_for(800ms);
            
            const std::string& request_body = requests[request_num];
            request_count_++;
            
            Logger::log(Logger::INFO, 
                       "Received REST request #" + std::to_string(request_count_.load()),
                       "RestApiService");
            
            try {
                handle_request(request_body);
            }
            catch (const JsonParseException& e) {
                error_count_++;
                
                Logger::log(Logger::ERROR, 
                           "Invalid JSON in request #" + std::to_string(request_count_.load()),
                           "RestApiService");
                
                // This will trigger pthread_exit() through ThreadContext
                throw;
            }
            
            request_num++;
        }
        
        // If we get here without exception, run indefinitely
        while (running_) {
            std::this_thread::sleep_for(1s);
        }
    }
    
    void handle_request(const std::string& json_body) {
        // Parse JSON (may throw JsonParseException)
        SimpleJson json(json_body);
        
        std::string action = json.get_field("action");
        
        Logger::log(Logger::INFO, "Processing action: " + action, "RestApiService");
        
        // Check cache
        if (cache_.get(action)) {
            Logger::log(Logger::INFO, "Cache HIT for action: " + action, "RestApiService");
        } else {
            Logger::log(Logger::INFO, "Cache MISS for action: " + action, "RestApiService");
            // Submit query to database
            db_.submit_query("SELECT * FROM actions WHERE action='" + action + "'");
        }
    }
    
    void stop() {
        running_ = false;
    }
    
    int get_request_count() const { return request_count_.load(); }
    int get_error_count() const { return error_count_.load(); }
};

// ============================================================================
// SECTION 6: Monitoring Service (Non-critical)
// ============================================================================

class MonitoringService {
private:
    std::atomic<bool> running_{false};
    DatabaseService& db_;
    RestApiService& rest_;
    
public:
    MonitoringService(DatabaseService& db, RestApiService& rest)
        : db_(db), rest_(rest) {}
    
    void start() {
        running_ = true;
        
        g_thread_context = std::make_unique<ThreadContext>(
            ThreadType::MONITORING, "MonitoringService");
        
        Logger::log(Logger::INFO, "Monitoring service started", "MonitoringService");
        
        try {
            while (running_) {
                std::this_thread::sleep_for(2s);
                
                std::ostringstream oss;
                oss << "System Health: DB queries=" << db_.get_query_count()
                    << ", REST requests=" << rest_.get_request_count()
                    << ", REST errors=" << rest_.get_error_count();
                
                Logger::log(Logger::INFO, oss.str(), "MonitoringService");
            }
        }
        catch (const std::exception& e) {
            g_thread_context->handle_exception(e);
        }
        
        Logger::log(Logger::INFO, "Monitoring service stopped", "MonitoringService");
    }
    
    void stop() {
        running_ = false;
    }
};

// ============================================================================
// SECTION 7: Microservices Orchestrator
// ============================================================================

class MicroservicesOrchestrator {
private:
    DatabaseService db_service_;
    CacheService cache_service_;
    RestApiService rest_service_;
    MonitoringService monitoring_service_;
    
    std::vector<std::thread> threads_;
    bool simulate_core_failure_;
    
public:
    MicroservicesOrchestrator(bool simulate_core_failure = false)
        : rest_service_(db_service_, cache_service_),
          monitoring_service_(db_service_, rest_service_),
          simulate_core_failure_(simulate_core_failure) {}
    
    void start() {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════════╗\n";
        std::cout << "║   Multi-Threaded Microservices Architecture                ║\n";
        std::cout << "║                                                            ║\n";
        std::cout << "║   Core Services (abort on exception):                     ║\n";
        std::cout << "║     • DatabaseService                                      ║\n";
        std::cout << "║     • CacheService                                         ║\n";
        std::cout << "║                                                            ║\n";
        std::cout << "║   REST Services (pthread_exit on exception):              ║\n";
        std::cout << "║     • RestApiService                                       ║\n";
        std::cout << "║     • MonitoringService                                    ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════╝\n\n";
        
        Logger::log(Logger::INFO, "Starting microservices orchestrator", "Orchestrator");
        
        // Start core services first
        threads_.emplace_back(&DatabaseService::start, &db_service_);
        threads_.emplace_back(&CacheService::start, &cache_service_);
        
        std::this_thread::sleep_for(500ms);
        
        // Start REST and monitoring services
        threads_.emplace_back(&RestApiService::start, &rest_service_);
        threads_.emplace_back(&MonitoringService::start, &monitoring_service_);
        
        Logger::log(Logger::INFO, "All services started", "Orchestrator");
        
        // If simulating core failure, trigger it after a few seconds
        if (simulate_core_failure_) {
            std::this_thread::sleep_for(3s);
            Logger::log(Logger::WARNING, 
                       "⚠️  Triggering core service failure in 1 second...", 
                       "Orchestrator");
            std::this_thread::sleep_for(1s);
            db_service_.submit_query("TRIGGER_CORE_FAILURE - Simulated corruption");
        }
    }
    
    void wait_and_stop(int seconds) {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        
        Logger::log(Logger::INFO, "Initiating graceful shutdown...", "Orchestrator");
        
        // Stop services in reverse order
        monitoring_service_.stop();
        rest_service_.stop();
        cache_service_.stop();
        db_service_.stop();
        
        // Wait for threads to finish
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        Logger::log(Logger::INFO, "All services stopped", "Orchestrator");
    }
};

// ============================================================================
// SECTION 8: Demonstrations
// ============================================================================

void demonstrate_rest_service_exception() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Demonstration 1: REST Service Exception Handling ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Scenario:\n";
    std::cout << "  • REST service will receive invalid JSON requests\n";
    std::cout << "  • JsonParseException will be thrown\n";
    std::cout << "  • REST thread will call pthread_exit() and terminate\n";
    std::cout << "  • Core services (Database, Cache) will continue running\n\n";
    
    std::cout << "Press Enter to start demonstration...\n";
    std::cin.get();
    
    MicroservicesOrchestrator orchestrator(false);  // No core failure
    orchestrator.start();
    
    // Let it run for 10 seconds
    // REST service will encounter invalid JSON around request #4
    orchestrator.wait_and_stop(10);
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "✓ REST service terminated (pthread_exit) but core services survived!\n";
    std::cout << std::string(70, '=') << "\n\n";
}

void demonstrate_core_service_exception() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Demonstration 2: CORE Service Exception Handling ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Scenario:\n";
    std::cout << "  • Core service (Database) will encounter critical error\n";
    std::cout << "  • Exception will be thrown in DatabaseService\n";
    std::cout << "  • DatabaseService thread will call abort()\n";
    std::cout << "  • ⚠️  ENTIRE APPLICATION WILL TERMINATE ⚠️\n\n";
    
    std::cout << "⚠️  WARNING: This will terminate the process with abort()!\n";
    std::cout << "Press Enter to start demonstration...\n";
    std::cin.get();
    
    MicroservicesOrchestrator orchestrator(true);  // Simulate core failure
    orchestrator.start();
    
    // Wait for core failure (will never return - abort() will be called)
    orchestrator.wait_and_stop(30);
    
    // This line will never be reached
    std::cout << "\n❌ This message should NEVER appear (abort() was called)\n";
}

void demonstrate_best_practices() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Best Practices for Multi-Threaded Microservices ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✓ THREAD CLASSIFICATION:\n";
    std::cout << "  1. Core/Critical Services:\n";
    std::cout << "     • Essential for application function\n";
    std::cout << "     • Exception → abort() entire process\n";
    std::cout << "     • Examples: Database, Message Queue, State Manager\n\n";
    
    std::cout << "  2. REST/External Services:\n";
    std::cout << "     • Handle external/untrusted input\n";
    std::cout << "     • Exception → pthread_exit() current thread only\n";
    std::cout << "     • Examples: HTTP servers, gRPC endpoints, WebSocket handlers\n\n";
    
    std::cout << "  3. Monitoring/Auxiliary Services:\n";
    std::cout << "     • Non-essential functionality\n";
    std::cout << "     • Exception → pthread_exit() current thread only\n";
    std::cout << "     • Examples: Metrics, logging, health checks\n\n";
    
    std::cout << "✓ EXCEPTION HANDLING STRATEGY:\n";
    std::cout << "  1. Always log exceptions with stack traces\n";
    std::cout << "  2. Validate all external input (JSON, XML, protobuf)\n";
    std::cout << "  3. Use thread-local context to identify thread type\n";
    std::cout << "  4. Implement graceful degradation for non-critical threads\n";
    std::cout << "  5. Use RAII for resource cleanup before pthread_exit()\n\n";
    
    std::cout << "✓ ISOLATION AND RESILIENCE:\n";
    std::cout << "  1. Run REST handlers in separate threads/thread pool\n";
    std::cout << "  2. Implement circuit breakers for failing services\n";
    std::cout << "  3. Use message queues to decouple services\n";
    std::cout << "  4. Monitor thread health and restart if needed\n";
    std::cout << "  5. Test exception paths thoroughly\n\n";
    
    std::cout << "⚠️  CRITICAL WARNINGS:\n";
    std::cout << "  1. pthread_exit() does NOT call destructors for local objects!\n";
    std::cout << "  2. Clean up resources manually before pthread_exit()\n";
    std::cout << "  3. Don't use pthread_exit() from main thread\n";
    std::cout << "  4. On Windows, use ExitThread() instead of pthread_exit()\n";
    std::cout << "  5. Consider std::terminate() as alternative to abort()\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   Multi-Threaded Microservices with Exception Handling         ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║   Demonstrates different exception handling strategies:        ║\n";
    std::cout << "║     • REST Service: pthread_exit() (thread-local termination) ║\n";
    std::cout << "║     • Core Service: abort() (process-wide termination)        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    std::cout << "\nSelect demonstration:\n";
    std::cout << "  1. REST Service Exception (pthread_exit - graceful)\n";
    std::cout << "  2. CORE Service Exception (abort - terminates process)\n";
    std::cout << "  3. Best Practices Guide (no execution)\n";
    std::cout << "\nEnter choice (1-3): ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();  // Clear newline
    
    switch (choice) {
        case 1:
            demonstrate_rest_service_exception();
            demonstrate_best_practices();
            break;
            
        case 2:
            demonstrate_core_service_exception();
            // Will never reach here - abort() terminates process
            break;
            
        case 3:
            demonstrate_best_practices();
            break;
            
        default:
            std::cout << "\nInvalid choice. Running REST demonstration by default.\n";
            demonstrate_rest_service_exception();
            demonstrate_best_practices();
            break;
    }
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All demonstrations completed!\n";
    std::cout << "\nKEY TAKEAWAYS:\n";
    std::cout << "  1. REST threads handle untrusted input → pthread_exit() on error\n";
    std::cout << "  2. Core threads are critical → abort() on error\n";
    std::cout << "  3. Always log exceptions with stack traces\n";
    std::cout << "  4. Use thread-local context for thread identification\n";
    std::cout << "  5. Implement graceful degradation for non-critical services\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    return 0;
}
