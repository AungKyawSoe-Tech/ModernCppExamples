// AdvancedExceptionHandling.cpp
// Comprehensive exception handling with real stack traces, nested exceptions,
// and advanced error reporting patterns

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <functional>
#include <exception>
#include <typeinfo>
#include <cstdlib>
#include <cstring>

// Platform-specific includes for stack traces
#ifdef __linux__
    #include <execinfo.h>
    #include <cxxabi.h>
    #include <unistd.h>
#elif _WIN32
    #include <windows.h>
    #include <dbghelp.h>
    #pragma comment(lib, "dbghelp.lib")
#endif

using namespace std::chrono;

// ============================================================================
// SECTION 1: Stack Trace Capture (Platform-Specific)
// ============================================================================

class StackTrace {
private:
    static constexpr size_t MAX_FRAMES = 64;
    std::vector<std::string> frames_;
    
public:
    StackTrace() {
        capture();
    }
    
    void capture() {
        frames_.clear();
        
#ifdef __linux__
        // Linux: Use backtrace() and backtrace_symbols()
        void* buffer[MAX_FRAMES];
        int frame_count = backtrace(buffer, MAX_FRAMES);
        
        char** symbols = backtrace_symbols(buffer, frame_count);
        if (symbols) {
            for (int i = 0; i < frame_count; ++i) {
                frames_.push_back(demangle_symbol(symbols[i]));
            }
            free(symbols);
        }
        
#elif _WIN32
        // Windows: Use CaptureStackBackTrace() and SymFromAddr()
        void* buffer[MAX_FRAMES];
        HANDLE process = GetCurrentProcess();
        SymInitialize(process, NULL, TRUE);
        
        WORD frame_count = CaptureStackBackTrace(0, MAX_FRAMES, buffer, NULL);
        
        SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
        if (symbol) {
            symbol->MaxNameLen = 255;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            
            for (WORD i = 0; i < frame_count; ++i) {
                if (SymFromAddr(process, (DWORD64)(buffer[i]), 0, symbol)) {
                    frames_.push_back(std::string(symbol->Name));
                } else {
                    std::ostringstream oss;
                    oss << "0x" << std::hex << (uint64_t)buffer[i];
                    frames_.push_back(oss.str());
                }
            }
            free(symbol);
        }
        
        SymCleanup(process);
#else
        frames_.push_back("[Stack trace not available on this platform]");
#endif
    }
    
    const std::vector<std::string>& get_frames() const {
        return frames_;
    }
    
    std::string to_string() const {
        std::ostringstream oss;
        oss << "Stack Trace (" << frames_.size() << " frames):\n";
        for (size_t i = 0; i < frames_.size(); ++i) {
            oss << "  #" << std::setw(2) << i << ": " << frames_[i] << "\n";
        }
        return oss.str();
    }
    
private:
#ifdef __linux__
    std::string demangle_symbol(const char* mangled) {
        std::string result = mangled;
        
        // Extract the mangled name between '(' and '+'
        const char* begin = strchr(mangled, '(');
        const char* end = strchr(mangled, '+');
        
        if (begin && end && begin < end) {
            begin++;  // Skip '('
            std::string mangled_name(begin, end - begin);
            
            int status;
            char* demangled = abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status);
            
            if (status == 0 && demangled) {
                result = demangled;
                free(demangled);
            }
        }
        
        return result;
    }
#endif
};

void demonstrate_stack_trace_capture() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 1. Real Stack Trace Capture ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Capturing stack trace from current location...\n\n";
    
    StackTrace trace;
    std::cout << trace.to_string();
    
    std::cout << "\n✓ Stack trace captured using platform-specific APIs\n";
#ifdef __linux__
    std::cout << "✓ Linux: backtrace() + abi::__cxa_demangle()\n";
#elif _WIN32
    std::cout << "✓ Windows: CaptureStackBackTrace() + SymFromAddr()\n";
#else
    std::cout << "ℹ️  Generic fallback (no native stack trace support)\n";
#endif
}

// ============================================================================
// SECTION 2: Exception with Stack Trace
// ============================================================================

class TracedException : public std::runtime_error {
private:
    std::string file_;
    int line_;
    std::string function_;
    StackTrace stack_trace_;
    std::string formatted_message_;
    
    std::string format_message() const {
        std::ostringstream oss;
        
        // Extract filename only
        size_t last_slash = file_.find_last_of("/\\");
        std::string filename = (last_slash != std::string::npos) 
            ? file_.substr(last_slash + 1) : file_;
        
        oss << "\n╔════════════════════════════════════════════════════════════╗\n";
        oss << "║ EXCEPTION THROWN WITH STACK TRACE\n";
        oss << "╠════════════════════════════════════════════════════════════╣\n";
        oss << "║ Location: " << filename << ":" << line_ << "\n";
        oss << "║ Function: " << function_ << "()\n";
        oss << "║ Message:  " << what() << "\n";
        oss << "╠════════════════════════════════════════════════════════════╣\n";
        oss << "║ " << stack_trace_.to_string();
        oss << "╚════════════════════════════════════════════════════════════╝\n";
        
        return oss.str();
    }
    
public:
    TracedException(const std::string& file, int line, const std::string& func,
                   const std::string& message)
        : std::runtime_error(message),
          file_(file),
          line_(line),
          function_(func),
          stack_trace_(),
          formatted_message_(format_message()) {}
    
    const std::string& get_formatted_message() const {
        return formatted_message_;
    }
    
    const StackTrace& get_stack_trace() const {
        return stack_trace_;
    }
    
    const std::string& get_file() const { return file_; }
    int get_line() const { return line_; }
    const std::string& get_function() const { return function_; }
};

#define THROW_TRACED(message) \
    throw TracedException(__FILE__, __LINE__, __FUNCTION__, message)

void nested_function_3() {
    std::cout << "  [3] About to throw exception...\n";
    THROW_TRACED("Critical error in nested function");
}

void nested_function_2() {
    std::cout << "  [2] Calling nested_function_3()...\n";
    nested_function_3();
}

void nested_function_1() {
    std::cout << "  [1] Calling nested_function_2()...\n";
    nested_function_2();
}

void demonstrate_exception_with_stack_trace() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 2. Exception with Stack Trace ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Calling nested functions to build call stack...\n\n";
    
    try {
        nested_function_1();
    }
    catch (const TracedException& e) {
        std::cout << "\n✓ Exception caught!\n";
        std::cout << e.get_formatted_message();
    }
}

// ============================================================================
// SECTION 3: Nested Exceptions (C++11)
// ============================================================================

class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& msg) 
        : std::runtime_error("Database Error: " + msg) {}
};

class ConnectionException : public std::runtime_error {
public:
    explicit ConnectionException(const std::string& msg)
        : std::runtime_error("Connection Error: " + msg) {}
};

void database_operation() {
    std::cout << "    Attempting database query...\n";
    throw DatabaseException("Query timeout after 30 seconds");
}

void connect_to_database() {
    std::cout << "  Attempting to connect to database...\n";
    
    try {
        database_operation();
    }
    catch (const DatabaseException& e) {
        std::cout << "    ⚠️  Database operation failed!\n";
        // Wrap the DatabaseException in a ConnectionException
        std::throw_with_nested(ConnectionException("Failed to establish connection"));
    }
}

void print_nested_exception(const std::exception& e, int level = 0) {
    std::string indent(level * 2, ' ');
    std::cout << indent << "└─ " << e.what() << "\n";
    
    try {
        std::rethrow_if_nested(e);
    }
    catch (const std::exception& nested) {
        print_nested_exception(nested, level + 1);
    }
}

void demonstrate_nested_exceptions() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 3. Nested Exceptions (C++11) ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Concept: Wrap exceptions to preserve context\n\n";
    
    try {
        connect_to_database();
    }
    catch (const std::exception& e) {
        std::cout << "\n✓ Exception caught with nested context:\n\n";
        print_nested_exception(e);
    }
    
    std::cout << "\n✓ Original exception preserved inside wrapper\n";
    std::cout << "✓ Use std::throw_with_nested() and std::rethrow_if_nested()\n";
}

// ============================================================================
// SECTION 4: Exception Guarantee Levels
// ============================================================================

template<typename T>
class Container {
private:
    std::unique_ptr<T[]> data_;
    size_t size_;
    size_t capacity_;
    
public:
    Container() : data_(nullptr), size_(0), capacity_(0) {}
    
    // Basic guarantee: valid state but data may be lost
    void push_back_basic(const T& value) {
        if (size_ == capacity_) {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            auto new_data = std::make_unique<T[]>(new_capacity);
            
            // If copy throws here, we're in an inconsistent state
            for (size_t i = 0; i < size_; ++i) {
                new_data[i] = data_[i];  // May throw!
            }
            
            data_ = std::move(new_data);
            capacity_ = new_capacity;
        }
        
        data_[size_++] = value;
    }
    
    // Strong guarantee: operation succeeds completely or has no effect
    void push_back_strong(const T& value) {
        if (size_ == capacity_) {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            auto new_data = std::make_unique<T[]>(new_capacity);
            
            // Copy all elements (may throw)
            for (size_t i = 0; i < size_; ++i) {
                new_data[i] = data_[i];
            }
            
            // Only if successful, commit changes (no-throw from here)
            data_ = std::move(new_data);
            capacity_ = new_capacity;
        }
        
        data_[size_++] = value;
    }
    
    // No-throw guarantee: never throws
    size_t size() const noexcept {
        return size_;
    }
    
    // No-throw guarantee
    void clear() noexcept {
        size_ = 0;
    }
};

void demonstrate_exception_guarantees() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 4. Exception Safety Guarantees ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. No-throw guarantee (noexcept):\n";
    std::cout << "   • Function never throws\n";
    std::cout << "   • Example: size(), clear(), swap()\n";
    std::cout << "   • Mark with 'noexcept' keyword\n\n";
    
    std::cout << "2. Strong guarantee:\n";
    std::cout << "   • Operation succeeds completely OR has no effect\n";
    std::cout << "   • Example: push_back() using copy-and-swap\n";
    std::cout << "   • All or nothing - no partial state\n\n";
    
    std::cout << "3. Basic guarantee:\n";
    std::cout << "   • Object left in valid state if exception thrown\n";
    std::cout << "   • Example: push_back() that may leave capacity changed\n";
    std::cout << "   • No resource leaks, but data may be lost\n\n";
    
    std::cout << "4. No guarantee:\n";
    std::cout << "   • May leave object in invalid state\n";
    std::cout << "   • ❌ AVOID - leads to crashes and undefined behavior\n\n";
    
    Container<int> cont;
    cont.push_back_strong(42);
    std::cout << "✓ Container with strong guarantee used successfully\n";
}

// ============================================================================
// SECTION 5: RAII and Exception Safety
// ============================================================================

class FileRAII {
private:
    FILE* file_;
    std::string filename_;
    
public:
    explicit FileRAII(const std::string& filename) 
        : file_(nullptr), filename_(filename) {
        
        file_ = fopen(filename.c_str(), "w");
        if (!file_) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        std::cout << "  [RAII] File opened: " << filename << "\n";
    }
    
    ~FileRAII() {
        if (file_) {
            fclose(file_);
            std::cout << "  [RAII] File closed: " << filename_ << "\n";
        }
    }
    
    void write(const std::string& data) {
        if (!file_) {
            throw std::runtime_error("File not open");
        }
        
        if (data == "ERROR") {
            throw std::runtime_error("Simulated write error");
        }
        
        fprintf(file_, "%s\n", data.c_str());
    }
    
    // Delete copy/move to enforce RAII
    FileRAII(const FileRAII&) = delete;
    FileRAII& operator=(const FileRAII&) = delete;
};

void demonstrate_raii_exception_safety() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 5. RAII and Exception Safety ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Without RAII (manual cleanup - leak on exception):\n\n";
    try {
        FILE* f = fopen("/tmp/test.txt", "w");
        std::cout << "  File opened\n";
        // If exception thrown here, file never closed! ❌
        throw std::runtime_error("Oops!");
        fclose(f);  // Never executed
    }
    catch (...) {
        std::cout << "  ❌ File leaked - never closed!\n\n";
    }
    
    std::cout << "With RAII (automatic cleanup - safe):\n\n";
    try {
        FileRAII file("/tmp/test_raii.txt");
        file.write("Hello");
        file.write("ERROR");  // Throws exception
        file.write("World");  // Never executed
    }
    catch (const std::exception& e) {
        std::cout << "  ⚠️  Exception: " << e.what() << "\n";
        std::cout << "  ✓ RAII destructor automatically closed file!\n";
    }
    
    std::cout << "\n✓ RAII guarantees resource cleanup even with exceptions\n";
}

// ============================================================================
// SECTION 6: std::exception_ptr and Thread Exception Propagation
// ============================================================================

#include <thread>

std::exception_ptr worker_exception = nullptr;

void worker_thread_function() {
    try {
        std::cout << "  [Worker] Doing some work...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Simulate error
        throw std::runtime_error("Worker thread encountered an error!");
    }
    catch (...) {
        std::cout << "  [Worker] Exception caught, storing for main thread\n";
        worker_exception = std::current_exception();
    }
}

void demonstrate_exception_ptr() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 6. std::exception_ptr (Thread Exception Propagation) ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Problem: Exceptions can't cross thread boundaries\n";
    std::cout << "Solution: std::exception_ptr + std::current_exception()\n\n";
    
    std::cout << "Starting worker thread...\n";
    
    worker_exception = nullptr;
    std::thread worker(worker_thread_function);
    worker.join();
    
    std::cout << "\nMain thread checking for worker exception...\n";
    
    if (worker_exception) {
        try {
            std::rethrow_exception(worker_exception);
        }
        catch (const std::exception& e) {
            std::cout << "✓ Worker exception received: " << e.what() << "\n";
        }
    }
    
    std::cout << "\n✓ Exception successfully propagated from worker to main thread\n";
}

// ============================================================================
// SECTION 7: Custom Exception Hierarchy
// ============================================================================

class ApplicationException : public std::runtime_error {
protected:
    int error_code_;
    system_clock::time_point timestamp_;
    
public:
    ApplicationException(int code, const std::string& msg)
        : std::runtime_error(msg),
          error_code_(code),
          timestamp_(system_clock::now()) {}
    
    int get_error_code() const { return error_code_; }
    
    std::string get_timestamp() const {
        auto time = system_clock::to_time_t(timestamp_);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

class NetworkError : public ApplicationException {
public:
    NetworkError(int code, const std::string& msg)
        : ApplicationException(code, msg) {}
};

class FileSystemError : public ApplicationException {
public:
    FileSystemError(int code, const std::string& msg)
        : ApplicationException(code, msg) {}
};

class BusinessLogicError : public ApplicationException {
public:
    BusinessLogicError(int code, const std::string& msg)
        : ApplicationException(code, msg) {}
};

void demonstrate_exception_hierarchy() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 7. Custom Exception Hierarchy ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Exception Hierarchy:\n";
    std::cout << "  ApplicationException (base)\n";
    std::cout << "  ├─ NetworkError\n";
    std::cout << "  ├─ FileSystemError\n";
    std::cout << "  └─ BusinessLogicError\n\n";
    
    try {
        throw NetworkError(503, "Service unavailable");
    }
    catch (const NetworkError& e) {
        std::cout << "Caught NetworkError:\n";
        std::cout << "  Code:      " << e.get_error_code() << "\n";
        std::cout << "  Message:   " << e.what() << "\n";
        std::cout << "  Timestamp: " << e.get_timestamp() << "\n";
    }
    catch (const ApplicationException& e) {
        std::cout << "Caught generic ApplicationException\n";
    }
    
    std::cout << "\n✓ Specific catch before generic catch\n";
    std::cout << "✓ Polymorphic exception handling\n";
}

// ============================================================================
// SECTION 8: Function Try Blocks (Constructor Exception Handling)
// ============================================================================

class Resource {
public:
    Resource(int id) {
        std::cout << "    [Resource " << id << "] Constructed\n";
        if (id == 2) {
            throw std::runtime_error("Resource 2 construction failed!");
        }
    }
    
    ~Resource() {
        std::cout << "    [Resource] Destroyed\n";
    }
};

class ComponentWithResources {
private:
    Resource res1_;
    Resource res2_;
    Resource res3_;
    
public:
    // Function try block for constructor
    ComponentWithResources()
    try : res1_(1), res2_(2), res3_(3)  // res2 will throw!
    {
        std::cout << "  Constructor body executed\n";
    }
    catch (const std::exception& e) {
        std::cout << "  ⚠️  Constructor caught: " << e.what() << "\n";
        std::cout << "  ⚠️  res1_ will be automatically destroyed\n";
        // Note: Exception is automatically rethrown after this catch
    }
};

void demonstrate_function_try_blocks() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 8. Function Try Blocks (Constructor Exception Handling) ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Attempting to construct ComponentWithResources...\n\n";
    
    try {
        ComponentWithResources component;
    }
    catch (const std::exception& e) {
        std::cout << "\n✓ Exception propagated to caller\n";
        std::cout << "✓ Partially constructed members automatically cleaned up\n";
    }
}

// ============================================================================
// SECTION 9: Best Practices Summary
// ============================================================================

void demonstrate_best_practices() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 9. Exception Handling Best Practices ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✓ DO:\n";
    std::cout << "─────\n";
    std::cout << "  1. Use RAII for resource management\n";
    std::cout << "  2. Catch by const reference: catch (const Ex& e)\n";
    std::cout << "  3. Throw by value: throw MyException(...)\n";
    std::cout << "  4. Inherit from std::exception hierarchy\n";
    std::cout << "  5. Mark functions noexcept when appropriate\n";
    std::cout << "  6. Use specific exceptions before generic ones\n";
    std::cout << "  7. Provide meaningful error messages\n";
    std::cout << "  8. Document what exceptions functions can throw\n";
    std::cout << "  9. Use stack traces for debugging\n";
    std::cout << "  10. Test exception paths thoroughly\n\n";
    
    std::cout << "❌ DON'T:\n";
    std::cout << "─────────\n";
    std::cout << "  1. Throw from destructors (noexcept by default in C++11)\n";
    std::cout << "  2. Catch (...) without rethrowing or logging\n";
    std::cout << "  3. Use exceptions for normal control flow\n";
    std::cout << "  4. Return error codes for exceptional situations\n";
    std::cout << "  5. Ignore exceptions silently\n";
    std::cout << "  6. Throw pointers: throw new Ex() ❌\n";
    std::cout << "  7. Catch by value (causes slicing)\n";
    std::cout << "  8. Mix exceptions and error codes inconsistently\n";
    std::cout << "  9. Throw from noexcept functions\n";
    std::cout << "  10. Forget to clean up resources (use RAII!)\n\n";
    
    std::cout << "EXCEPTION vs ERROR CODES:\n";
    std::cout << "─────────────────────────\n";
    std::cout << "Use Exceptions when:\n";
    std::cout << "  • Error is truly exceptional\n";
    std::cout << "  • Can't continue execution\n";
    std::cout << "  • Need to propagate through many layers\n";
    std::cout << "  • RAII cleanup is needed\n\n";
    
    std::cout << "Use Error Codes when:\n";
    std::cout << "  • Expected/recoverable errors\n";
    std::cout << "  • Performance-critical hot paths\n";
    std::cout << "  • C API compatibility\n";
    std::cout << "  • Safety-critical systems (ISO 26262)\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║        Advanced Exception Handling with Stack Traces               ║\n";
    std::cout << "║                                                                    ║\n";
    std::cout << "║  Comprehensive guide to modern C++ exception handling patterns    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    
    demonstrate_stack_trace_capture();
    demonstrate_exception_with_stack_trace();
    demonstrate_nested_exceptions();
    demonstrate_exception_guarantees();
    demonstrate_raii_exception_safety();
    demonstrate_exception_ptr();
    demonstrate_exception_hierarchy();
    demonstrate_function_try_blocks();
    demonstrate_best_practices();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All exception handling demonstrations completed!\n";
    std::cout << "\nKEY TAKEAWAYS:\n";
    std::cout << "  1. Real stack traces available on Linux/Windows\n";
    std::cout << "  2. RAII guarantees cleanup even with exceptions\n";
    std::cout << "  3. Nested exceptions preserve error context\n";
    std::cout << "  4. std::exception_ptr for thread exception propagation\n";
    std::cout << "  5. Always catch by const reference\n";
    std::cout << "  6. Use noexcept for no-throw guarantees\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    return 0;
}
