# Multi-Threaded Microservices with Exception Handling

## Overview

This example demonstrates a production-ready multi-threaded microservices architecture with **different exception handling strategies** based on thread criticality:

- **Core Service Threads**: Call `abort()` on exception (entire process terminates)
- **REST Service Threads**: Call `pthread_exit()` on exception (only that thread terminates)
- **Monitoring Threads**: Call `pthread_exit()` on exception (non-critical, graceful degradation)

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│         Multi-Threaded Microservices                    │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌─────────────────────────────────────────┐           │
│  │  Core Services (CRITICAL)               │           │
│  │  Exception → abort() entire process     │           │
│  ├─────────────────────────────────────────┤           │
│  │  • DatabaseService (Thread 1)           │           │
│  │  • CacheService (Thread 2)              │           │
│  └─────────────────────────────────────────┘           │
│                                                         │
│  ┌─────────────────────────────────────────┐           │
│  │  REST Services (NON-CRITICAL)           │           │
│  │  Exception → pthread_exit() thread only │           │
│  ├─────────────────────────────────────────┤           │
│  │  • RestApiService (Thread 3)            │           │
│  │  • MonitoringService (Thread 4)         │           │
│  └─────────────────────────────────────────┘           │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

## Key Components

### 1. Thread Context (Thread-Local Storage)

```cpp
enum class ThreadType {
    CORE_SERVICE,    // Critical - abort() on exception
    REST_SERVICE,    // Non-critical - pthread_exit()
    MONITORING       // Non-critical - pthread_exit()
};

class ThreadContext {
    ThreadType type_;
    std::string name_;
    
public:
    void handle_exception(const std::exception& e) {
        Logger::log_exception_with_stack(e, name_);
        
        if (is_core_service()) {
            abort();  // Terminate entire process
        } else {
            pthread_exit(nullptr);  // Exit only this thread
        }
    }
};

// Thread-local storage
thread_local std::unique_ptr<ThreadContext> g_thread_context;
```

### 2. Logger with Stack Trace

```cpp
class Logger {
    static void log_exception_with_stack(const std::exception& e, 
                                         const std::string& thread_name) {
        log(CRITICAL, "EXCEPTION: " + e.what(), thread_name);
        
        auto stack = capture_stack_trace();
        for (size_t i = 0; i < stack.size(); ++i) {
            log(CRITICAL, "#" + std::to_string(i) + ": " + stack[i], thread_name);
        }
    }
};
```

### 3. Simple JSON Parser with Validation

```cpp
class SimpleJson {
public:
    explicit SimpleJson(const std::string& json_str) {
        validate();  // Throws JsonParseException if invalid
    }
    
    void validate() const {
        if (data_[start] != '{' || data_[end] != '}') {
            throw JsonParseException("Invalid JSON: must start with { and end with }");
        }
        // ... more validation
    }
};
```

### 4. Core Services (Critical)

```cpp
class DatabaseService {
public:
    void start() {
        g_thread_context = std::make_unique<ThreadContext>(
            ThreadType::CORE_SERVICE, "DatabaseService");
        
        try {
            while (running_) {
                execute_query(query);
            }
        }
        catch (const std::exception& e) {
            g_thread_context->handle_exception(e);  // Will call abort()
        }
    }
};

class CacheService {
    // Similar structure - CORE_SERVICE type
};
```

### 5. REST Service (Non-Critical)

```cpp
class RestApiService {
public:
    void start() {
        g_thread_context = std::make_unique<ThreadContext>(
            ThreadType::REST_SERVICE, "RestApiService");
        
        try {
            simulate_rest_requests();
        }
        catch (const std::exception& e) {
            g_thread_context->handle_exception(e);  // Will call pthread_exit()
        }
    }
    
    void handle_request(const std::string& json_body) {
        SimpleJson json(json_body);  // May throw JsonParseException!
        
        std::string action = json.get_field("action");
        // Process action...
    }
};
```

## Demonstration Output

### Scenario: Invalid JSON Request to REST Service

```
[15:35:12.521] [INFO] [RestApiService] Received REST request #4
[15:35:12.521] [ERROR] [RestApiService] Invalid JSON in request #4
[15:35:12.521] [CRIT] [RestApiService] EXCEPTION: JSON Parse Error: 
                Invalid JSON: must start with { and end with }

[15:35:12.527] [CRIT] [RestApiService] Stack trace:
  #0: ./bin/MultiThreadedMicroservices(+0x6b52) [0x5ada203a3b52]
  #1: ./bin/MultiThreadedMicroservices(+0x6f5c) [0x5ada203a3f5c]
  #2: ./bin/MultiThreadedMicroservices(+0x72e5) [0x5ada203a42e5]
  ...

[15:35:12.527] [ERROR] [RestApiService] 
  REST/MONITORING SERVICE FAILURE - Exiting thread only 
  (core services continue)

╔════════════════════════════════════╗
║   REST/Monitor thread exiting      ║
║   Core services still running OK   ║
╚════════════════════════════════════╝

[15:35:13.279] [INFO] [MonitoringService] System Health: 
  DB queries=3, REST requests=4, REST errors=1

[15:35:13.778] [INFO] [CacheService] Cache stats: 
  0 hits, 3 misses (hit rate: 0.0%)

✓ REST service terminated (pthread_exit) but core services survived!
```

## Exception Handling Flow

### REST Service Exception (Non-Critical)

```
Invalid JSON Request
    ↓
SimpleJson::validate() throws JsonParseException
    ↓
RestApiService::handle_request() propagates exception
    ↓
RestApiService::start() catch block
    ↓
ThreadContext::handle_exception()
    ↓
Check thread type: REST_SERVICE
    ↓
Logger::log_exception_with_stack()
    ↓
pthread_exit(nullptr) ← Thread exits, core services continue
```

### Core Service Exception (Critical)

```
Database Corruption Detected
    ↓
DatabaseService::execute_query() throws exception
    ↓
DatabaseService::start() catch block
    ↓
ThreadContext::handle_exception()
    ↓
Check thread type: CORE_SERVICE
    ↓
Logger::log_exception_with_stack()
    ↓
abort() ← Entire process terminates
```

## Best Practices

### ✓ Thread Classification

1. **Core/Critical Services**:
   - Essential for application function
   - Exception → `abort()` entire process
   - Examples: Database, Message Queue, State Manager

2. **REST/External Services**:
   - Handle external/untrusted input
   - Exception → `pthread_exit()` current thread only
   - Examples: HTTP servers, gRPC endpoints, WebSocket handlers

3. **Monitoring/Auxiliary Services**:
   - Non-essential functionality
   - Exception → `pthread_exit()` current thread only
   - Examples: Metrics, logging, health checks

### ✓ Exception Handling Strategy

1. Always log exceptions with stack traces
2. Validate all external input (JSON, XML, protobuf)
3. Use thread-local context to identify thread type
4. Implement graceful degradation for non-critical threads
5. Use RAII for resource cleanup before `pthread_exit()`

### ✓ Isolation and Resilience

1. Run REST handlers in separate threads/thread pool
2. Implement circuit breakers for failing services
3. Use message queues to decouple services
4. Monitor thread health and restart if needed
5. Test exception paths thoroughly

## ⚠️ Critical Warnings

1. **`pthread_exit()` does NOT call destructors for local objects!**
   - Clean up resources manually before calling `pthread_exit()`
   - Use RAII with manual cleanup in catch blocks

2. **Don't use `pthread_exit()` from main thread**
   - Will cause undefined behavior
   - Use `std::exit()` or `return` instead

3. **Windows compatibility**
   - Use `ExitThread()` instead of `pthread_exit()` on Windows
   - Code handles this via `#ifdef _WIN32`

4. **Consider alternatives**
   - `std::terminate()` as alternative to `abort()`
   - `std::exit()` for graceful process termination
   - Exception propagation with `std::exception_ptr`

5. **Thread pool considerations**
   - In thread pool, use exception catching instead of `pthread_exit()`
   - Return error status and let pool manage thread lifecycle

## Platform Support

| Platform | Core Exception | REST Exception | Stack Trace |
|----------|---------------|----------------|-------------|
| Linux    | `abort()`     | `pthread_exit()` | `backtrace()` + `__cxa_demangle()` |
| Windows  | `abort()`     | `ExitThread()`   | `CaptureStackBackTrace()` |
| macOS    | `abort()`     | `pthread_exit()` | `backtrace()` + `__cxa_demangle()` |

## Building and Running

### Build
```bash
cd build
cmake ..
make MultiThreadedMicroservices
```

### Run
```bash
./bin/MultiThreadedMicroservices
```

## Key Takeaways

1. ✅ **REST threads handle untrusted input** → `pthread_exit()` on error
2. ✅ **Core threads are critical** → `abort()` on error
3. ✅ **Always log exceptions with stack traces** for debugging
4. ✅ **Use thread-local context** for thread identification
5. ✅ **Implement graceful degradation** for non-critical services
6. ✅ **Test both failure paths** thoroughly
7. ✅ **Monitor thread health** and implement auto-recovery
8. ✅ **Validate all external input** (JSON, XML, protobuf, etc.)

## Related Examples

- [AdvancedExceptionHandling.cpp](../src/AdvancedExceptionHandling.cpp) - Real stack traces, nested exceptions, RAII
- [ThreadPoolExamples.cpp](../src/ThreadPoolExamples.cpp) - Thread pool implementations
- [AsioMultipleContexts.cpp](../src/AsioMultipleContexts.cpp) - Multiple io_context patterns
- [FunctionalSafetyISO26262.cpp](../src/FunctionalSafetyISO26262.cpp) - Safety-critical patterns

## References

- **POSIX Threads**: `pthread_exit()`, `pthread_create()`, `pthread_join()`
- **C++ Exception Handling**: `std::exception`, `std::runtime_error`, `std::throw_with_nested()`
- **Stack Traces**: `backtrace()`, `backtrace_symbols()`, `abi::__cxa_demangle()`
- **ISO 26262**: Functional safety for automotive systems (see FunctionalSafetyISO26262.cpp)
- **Microservices Patterns**: Circuit breaker, bulkhead, health check
