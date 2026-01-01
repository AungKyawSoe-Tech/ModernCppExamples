// ===================================================================
// ERROR HANDLING STRATEGIES - Following Bjarne Stroustrup's Guidance
// Based on "A Tour of C++", 3rd Edition, Chapter 4
// ===================================================================
// 
// Three approaches to error handling:
// 1. RETURN VALUES - When function can't complete the task
// 2. EXCEPTIONS - When constructor can't establish invariants
// 3. TERMINATION - When continuing would be dangerous
//
// Key Principles:
// - Constructors cannot return values → must throw or terminate
// - Partially constructed objects are dangerous
// - RAII ensures proper cleanup even with exceptions
// - Return values for expected/recoverable errors
// - Exceptions for unexpected/exceptional situations
// - Terminate for programming errors/invariant violations
// ===================================================================

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <cassert>

// ===================================================================
// 1. RETURN VALUES - For Expected/Recoverable Errors
// ===================================================================

// Use case: Function can't complete task, but caller can handle it
// Appropriate when: Error is expected and can be recovered

class ConfigParser {
public:
    // Return std::optional - error is expected (file might not exist)
    static std::optional<std::string> read_config_value(const std::string& key) {
        std::cout << "[1.1] Reading config key: " << key << std::endl;
        
        // Simulate config lookup
        if (key == "server_url") {
            return "http://localhost:8080";
        }
        if (key == "timeout") {
            return "30";
        }
        
        // Key not found - this is EXPECTED, not exceptional
        std::cout << "      Config key '" << key << "' not found (expected scenario)" << std::endl;
        return std::nullopt;  // Caller can provide default or retry
    }
    
    // Return int with special value - traditional C-style
    static int parse_integer(const std::string& str, int default_value = -1) {
        std::cout << "[1.2] Parsing integer from: " << str << std::endl;
        
        try {
            return std::stoi(str);
        } catch (...) {
            // Parsing failed - return sentinel value
            std::cout << "      Parse failed, returning default: " << default_value << std::endl;
            return default_value;
        }
    }
    
    // Return bool with output parameter - when you need to return value too
    static bool try_connect(const std::string& url, int& out_status_code) {
        std::cout << "[1.3] Attempting connection to: " << url << std::endl;
        
        // Simulate connection attempt
        if (url.find("localhost") != std::string::npos) {
            out_status_code = 200;
            std::cout << "      Connected successfully, status: " << out_status_code << std::endl;
            return true;  // Success
        }
        
        out_status_code = 404;
        std::cout << "      Connection failed, status: " << out_status_code << std::endl;
        return false;  // Expected failure - caller can retry
    }
};

void demonstrate_return_values() {
    std::cout << "\n=== 1. RETURN VALUES - Expected/Recoverable Errors ===" << std::endl;
    std::cout << "Use when: Function can't complete, but caller can handle it" << std::endl;
    
    // Example 1: Optional value
    auto url = ConfigParser::read_config_value("server_url");
    if (url) {
        std::cout << "✅ Found URL: " << *url << std::endl;
    } else {
        std::cout << "ℹ️  Using default URL" << std::endl;
    }
    
    // Missing key - caller provides default
    auto missing = ConfigParser::read_config_value("missing_key");
    std::string final_value = missing.value_or("default_value");
    std::cout << "✅ Using value: " << final_value << std::endl;
    
    // Example 2: Integer parsing with default
    int timeout = ConfigParser::parse_integer("abc", 10);
    std::cout << "✅ Timeout value: " << timeout << " seconds" << std::endl;
    
    // Example 3: Try pattern
    int status_code;
    if (ConfigParser::try_connect("http://invalid.url", status_code)) {
        std::cout << "✅ Connected" << std::endl;
    } else {
        std::cout << "ℹ️  Connection failed with code: " << status_code << " (retry possible)" << std::endl;
    }
    
    std::cout << "\n💡 When to use RETURN VALUES:" << std::endl;
    std::cout << "   • Error is EXPECTED (file not found, network timeout)" << std::endl;
    std::cout << "   • Caller can RECOVER (retry, use default, ask user)" << std::endl;
    std::cout << "   • Normal flow of control" << std::endl;
    std::cout << "   • Performance-critical code" << std::endl;
}

// ===================================================================
// 2. EXCEPTIONS - For Constructors and Unexpected Errors
// ===================================================================

// Problem: Partially constructed objects are dangerous!

class FileHandler_BAD {
    std::string filename;
    FILE* file_ptr;  // Raw pointer - dangerous!
    bool is_valid;   // Flag to track construction success
    
public:
    // ❌ BAD: Constructor can't return error, uses flag instead
    FileHandler_BAD(const std::string& fname) 
        : filename(fname), file_ptr(nullptr), is_valid(false) {
        
        std::cout << "\n[2.1 BAD] Attempting to open file: " << fname << std::endl;
        
        file_ptr = fopen(fname.c_str(), "r");
        if (file_ptr == nullptr) {
            // ❌ PROBLEM: Can't return error from constructor
            // Object is partially constructed with invalid state!
            std::cout << "      ❌ File open failed, but object still created!" << std::endl;
            is_valid = false;  // Caller must check this flag
            return;
        }
        
        is_valid = true;
    }
    
    // Every member function must check is_valid!
    bool read_line(std::string& line) {
        if (!is_valid || !file_ptr) {
            return false;  // Undefined behavior if caller forgets to check!
        }
        // ... read logic
        return true;
    }
    
    ~FileHandler_BAD() {
        if (file_ptr) {
            fclose(file_ptr);
        }
    }
    
    bool valid() const { return is_valid; }
};

// Solution: Use RAII and throw from constructor

class FileHandler_GOOD {
    std::string filename;
    std::unique_ptr<FILE, decltype(&fclose)> file_ptr;  // RAII wrapper
    
public:
    // ✅ GOOD: Constructor either succeeds completely or throws
    FileHandler_GOOD(const std::string& fname) 
        : filename(fname), file_ptr(nullptr, &fclose) {
        
        std::cout << "\n[2.2 GOOD] Attempting to open file: " << fname << std::endl;
        
        FILE* raw_ptr = fopen(fname.c_str(), "r");
        if (raw_ptr == nullptr) {
            // ✅ THROW: Constructor can't establish invariant
            std::cout << "      ✅ Throwing exception - no partial object!" << std::endl;
            throw std::runtime_error("Failed to open file: " + fname);
        }
        
        file_ptr.reset(raw_ptr);
        std::cout << "      ✅ File opened successfully, object fully constructed" << std::endl;
        
        // Invariant established: file_ptr is valid
        // No need for is_valid flag - if object exists, it's valid!
    }
    
    // No need to check validity - if object exists, it's valid
    std::string read_line() {
        // Can safely use file_ptr - invariant guaranteed by constructor
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), file_ptr.get())) {
            return std::string(buffer);
        }
        return "";
    }
    
    // Destructor automatically called even if constructor throws
    // unique_ptr ensures file is closed
    ~FileHandler_GOOD() {
        std::cout << "      Closing file: " << filename << std::endl;
        // file_ptr's destructor automatically closes file
    }
};

// Another example: Vector with invariants

class BoundedVector_BAD {
    std::vector<int> data;
    size_t max_size;
    bool is_valid;
    
public:
    // ❌ BAD: Doesn't enforce invariant in constructor
    BoundedVector_BAD(size_t max_sz, const std::vector<int>& initial)
        : max_size(max_sz), is_valid(false) {
        
        std::cout << "\n[2.3 BAD] Creating bounded vector, max_size=" 
                  << max_sz << ", initial.size()=" << initial.size() << std::endl;
        
        if (initial.size() > max_sz) {
            // ❌ PROBLEM: Partial construction!
            std::cout << "      ❌ Initial size exceeds max, but object created anyway!" << std::endl;
            is_valid = false;
            return;
        }
        
        data = initial;
        is_valid = true;
    }
    
    bool push_back(int value) {
        if (!is_valid) return false;  // Must check everywhere!
        if (data.size() >= max_size) return false;
        data.push_back(value);
        return true;
    }
    
    bool valid() const { return is_valid; }
};

class BoundedVector_GOOD {
    std::vector<int> data;
    size_t max_size;
    
public:
    // ✅ GOOD: Enforces invariant, throws if violated
    BoundedVector_GOOD(size_t max_sz, const std::vector<int>& initial)
        : max_size(max_sz) {
        
        std::cout << "\n[2.4 GOOD] Creating bounded vector, max_size=" 
                  << max_sz << ", initial.size()=" << initial.size() << std::endl;
        
        if (initial.size() > max_sz) {
            // ✅ THROW: Invariant cannot be established
            std::cout << "      ✅ Throwing - initial size exceeds maximum!" << std::endl;
            throw std::invalid_argument(
                "Initial size " + std::to_string(initial.size()) + 
                " exceeds maximum " + std::to_string(max_sz)
            );
        }
        
        data = initial;
        std::cout << "      ✅ Bounded vector created, invariant satisfied" << std::endl;
        
        // Invariant: data.size() <= max_size
        // Guaranteed for object's entire lifetime!
    }
    
    void push_back(int value) {
        // Invariant guaranteed - no need to check is_valid
        if (data.size() >= max_size) {
            throw std::length_error("Vector at maximum capacity");
        }
        data.push_back(value);
    }
    
    size_t size() const { return data.size(); }
};

// RAII Example: Exception safety with resource management

class ResourceManager {
    std::unique_ptr<int[]> buffer;
    FILE* log_file;
    bool initialized;
    
public:
    // ✅ RAII: Resources acquired in constructor
    ResourceManager(size_t buffer_size, const std::string& log_path)
        : buffer(nullptr), log_file(nullptr), initialized(false) {
        
        std::cout << "\n[2.5] Constructing ResourceManager..." << std::endl;
        
        // Step 1: Allocate buffer (RAII with unique_ptr)
        buffer = std::make_unique<int[]>(buffer_size);
        std::cout << "      ✅ Buffer allocated" << std::endl;
        
        // Step 2: Open log file
        log_file = fopen(log_path.c_str(), "a");
        if (!log_file) {
            std::cout << "      ❌ Log file open failed - throwing..." << std::endl;
            // buffer automatically deallocated by unique_ptr destructor!
            throw std::runtime_error("Failed to open log file");
        }
        std::cout << "      ✅ Log file opened" << std::endl;
        
        initialized = true;
        std::cout << "      ✅ ResourceManager fully constructed" << std::endl;
    }
    
    ~ResourceManager() {
        std::cout << "      🧹 Cleaning up ResourceManager..." << std::endl;
        
        if (log_file) {
            fclose(log_file);
            std::cout << "      🧹 Log file closed" << std::endl;
        }
        
        // buffer automatically deallocated by unique_ptr
        std::cout << "      🧹 Buffer deallocated" << std::endl;
    }
    
    // Disable copy to prevent resource issues
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    // Enable move for transfer of ownership
    ResourceManager(ResourceManager&& other) noexcept
        : buffer(std::move(other.buffer))
        , log_file(other.log_file)
        , initialized(other.initialized) {
        other.log_file = nullptr;
        other.initialized = false;
    }
};

void demonstrate_exceptions() {
    std::cout << "\n=== 2. EXCEPTIONS - Constructors and Invariants ===" << std::endl;
    std::cout << "Use when: Constructor can't establish object invariants" << std::endl;
    
    // Example 2.1: BAD - Partially constructed object
    std::cout << "\n--- Demonstrating BAD approach ---" << std::endl;
    {
        FileHandler_BAD bad_handler("nonexistent_file.txt");
        if (!bad_handler.valid()) {
            std::cout << "❌ Object exists but is invalid - DANGEROUS!" << std::endl;
            std::cout << "   Every function must check valid() flag" << std::endl;
        }
    }
    
    // Example 2.2: GOOD - Exception prevents partial construction
    std::cout << "\n--- Demonstrating GOOD approach ---" << std::endl;
    try {
        FileHandler_GOOD good_handler("nonexistent_file.txt");
        // This line never executes if file doesn't exist
        std::cout << "✅ Object constructed, guaranteed valid" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✅ Exception caught: " << e.what() << std::endl;
        std::cout << "   No object exists - no invalid state possible!" << std::endl;
    }
    
    // Example 2.3: BAD - Invariant violation
    std::cout << "\n--- Bounded Vector: BAD approach ---" << std::endl;
    {
        std::vector<int> too_large = {1, 2, 3, 4, 5};
        BoundedVector_BAD bad_vec(3, too_large);  // max_size=3, but initial has 5
        if (!bad_vec.valid()) {
            std::cout << "❌ Invalid object exists with violated invariant!" << std::endl;
        }
    }
    
    // Example 2.4: GOOD - Exception prevents invariant violation
    std::cout << "\n--- Bounded Vector: GOOD approach ---" << std::endl;
    try {
        std::vector<int> too_large = {1, 2, 3, 4, 5};
        BoundedVector_GOOD good_vec(3, too_large);  // Will throw
    } catch (const std::invalid_argument& e) {
        std::cout << "✅ Exception caught: " << e.what() << std::endl;
        std::cout << "   Invariant violation prevented!" << std::endl;
    }
    
    // Example 2.5: RAII ensures cleanup even with exceptions
    std::cout << "\n--- RAII with Exception ---" << std::endl;
    try {
        ResourceManager mgr(1024, "/invalid/path/log.txt");  // Will fail
    } catch (const std::exception& e) {
        std::cout << "✅ Exception caught: " << e.what() << std::endl;
        std::cout << "✅ Buffer was automatically cleaned up (RAII)!" << std::endl;
    }
    
    std::cout << "\n💡 When to use EXCEPTIONS:" << std::endl;
    std::cout << "   • Constructor can't establish invariants" << std::endl;
    std::cout << "   • Partially constructed object would be dangerous" << std::endl;
    std::cout << "   • Error is UNEXPECTED (programming error, resource exhaustion)" << std::endl;
    std::cout << "   • Can't return error value (constructors, operators)" << std::endl;
    std::cout << "   • RAII ensures cleanup even if exception thrown" << std::endl;
}

// ===================================================================
// 3. TERMINATION - For Unrecoverable Errors
// ===================================================================

// Use case: Programming error or invariant violation where continuing is dangerous

class CriticalSystem {
    int* data_ptr;
    size_t size;
    
    // Internal invariant check
    void check_invariant() const {
        if (data_ptr == nullptr && size > 0) {
            // PROGRAMMING ERROR: Inconsistent state!
            std::cerr << "\n💀 FATAL: Invariant violated - nullptr with size > 0" << std::endl;
            std::cerr << "   This should never happen - terminating!" << std::endl;
            std::terminate();  // Cannot continue safely
        }
    }
    
public:
    CriticalSystem(size_t sz) : data_ptr(nullptr), size(sz) {
        if (sz == 0) {
            std::cerr << "\n💀 FATAL: Zero-size system not allowed" << std::endl;
            std::terminate();  // Precondition violation
        }
        
        data_ptr = new int[sz];
        std::cout << "[3.1] CriticalSystem constructed with size " << sz << std::endl;
    }
    
    void set_value(size_t index, int value) {
        check_invariant();  // Verify internal consistency
        
        if (index >= size) {
            // PROGRAMMING ERROR: Out of bounds access
            std::cerr << "\n💀 FATAL: Index " << index << " out of bounds [0, " 
                      << size << ")" << std::endl;
            std::cerr << "   This indicates a bug - terminating!" << std::endl;
            std::abort();  // Cannot recover from logic error
        }
        
        data_ptr[index] = value;
    }
    
    int get_value(size_t index) const {
        // Use assert for debug builds (removed in release with NDEBUG)
        assert(index < size && "Index out of bounds");
        
        if (index >= size) {
            std::cerr << "\n💀 FATAL: Index out of bounds in release build" << std::endl;
            std::terminate();
        }
        
        return data_ptr[index];
    }
    
    ~CriticalSystem() {
        delete[] data_ptr;
    }
};

// Comparison: When to use each approach

class SafetyLevel {
public:
    enum class Level {
        INFO,       // Normal operation
        WARNING,    // Something unexpected but recoverable
        ERROR,      // Error that can be handled
        CRITICAL,   // Error requiring immediate attention
        FATAL       // Cannot continue
    };
    
    // Different error handling based on severity
    static void handle_error(Level level, const std::string& message) {
        switch (level) {
            case Level::INFO:
                // Just log - continue normally
                std::cout << "\nℹ️  INFO: " << message << std::endl;
                break;
                
            case Level::WARNING:
                // Log and return - caller handles it
                std::cout << "\n⚠️  WARNING: " << message << std::endl;
                // Return value or optional
                break;
                
            case Level::ERROR:
                // Throw exception - caller can catch and recover
                std::cout << "\n❌ ERROR: " << message << std::endl;
                throw std::runtime_error(message);
                
            case Level::CRITICAL:
                // Log critical error, attempt emergency cleanup
                std::cerr << "\n🔴 CRITICAL: " << message << std::endl;
                std::cerr << "   Attempting emergency save..." << std::endl;
                // ... emergency cleanup ...
                throw std::runtime_error("Critical: " + message);
                
            case Level::FATAL:
                // Cannot continue - terminate immediately
                std::cerr << "\n💀 FATAL: " << message << std::endl;
                std::cerr << "   Cannot continue safely - terminating!" << std::endl;
                std::terminate();
        }
    }
};

void demonstrate_termination() {
    std::cout << "\n=== 3. TERMINATION - Unrecoverable Errors ===" << std::endl;
    std::cout << "Use when: Continuing would be dangerous or impossible" << std::endl;
    
    // Example: Normal operation
    std::cout << "\n--- Normal operation ---" << std::endl;
    {
        CriticalSystem sys(10);
        sys.set_value(0, 42);
        sys.set_value(5, 100);
        std::cout << "✅ Values set successfully" << std::endl;
    }
    
    // Example: Safety levels
    std::cout << "\n--- Different safety levels ---" << std::endl;
    
    // INFO - just logging
    SafetyLevel::handle_error(SafetyLevel::Level::INFO, "System started");
    
    // WARNING - caller can ignore or handle
    SafetyLevel::handle_error(SafetyLevel::Level::WARNING, "Cache miss (using fallback)");
    
    // ERROR - throw exception
    try {
        SafetyLevel::handle_error(SafetyLevel::Level::ERROR, "Network timeout");
    } catch (const std::exception& e) {
        std::cout << "✅ Caught and recovered from ERROR: " << e.what() << std::endl;
    }
    
    std::cout << "\n💡 When to use TERMINATION:" << std::endl;
    std::cout << "   • PROGRAMMING ERROR detected (invariant violated)" << std::endl;
    std::cout << "   • Precondition violated (contract broken)" << std::endl;
    std::cout << "   • Continuing would corrupt data or cause undefined behavior" << std::endl;
    std::cout << "   • Use assert() in debug builds (removed in release)" << std::endl;
    std::cout << "   • Use std::terminate() or std::abort() for fatal errors" << std::endl;
    
    // Note: The examples below would terminate if uncommented
    std::cout << "\n⚠️  The following would terminate (commented out):" << std::endl;
    std::cout << "   // CriticalSystem sys(0);  // Zero size - would terminate" << std::endl;
    std::cout << "   // sys.set_value(999, 42);  // Out of bounds - would terminate" << std::endl;
}

// ===================================================================
// SUMMARY: Decision Guide
// ===================================================================

void print_decision_guide() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  DECISION GUIDE: Which Error Handling Strategy?" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n📋 1. RETURN VALUES (std::optional, bool, error codes)" << std::endl;
    std::cout << "   ✅ Use when:" << std::endl;
    std::cout << "      • Error is EXPECTED and part of normal operation" << std::endl;
    std::cout << "      • Caller can and should handle the error" << std::endl;
    std::cout << "      • Checking for availability (file exists, key found)" << std::endl;
    std::cout << "      • Performance-critical code" << std::endl;
    std::cout << "      • Parsing/validation where failure is common" << std::endl;
    std::cout << "\n   📝 Examples:" << std::endl;
    std::cout << "      • std::optional<T> find_user(int id)" << std::endl;
    std::cout << "      • bool try_connect(const string& url, int& status)" << std::endl;
    std::cout << "      • std::expected<Value, Error> parse_config()" << std::endl;
    
    std::cout << "\n\n🎯 2. EXCEPTIONS (throw/catch)" << std::endl;
    std::cout << "   ✅ Use when:" << std::endl;
    std::cout << "      • Constructor cannot establish invariants" << std::endl;
    std::cout << "      • Partially constructed object would be dangerous" << std::endl;
    std::cout << "      • Error is UNEXPECTED (resource exhaustion, logic error)" << std::endl;
    std::cout << "      • Cannot return error value (constructors, operators)" << std::endl;
    std::cout << "      • Need to skip multiple stack frames to handler" << std::endl;
    std::cout << "      • RAII ensures automatic cleanup" << std::endl;
    std::cout << "\n   📝 Examples:" << std::endl;
    std::cout << "      • FileHandle(const string& path)  // Constructor" << std::endl;
    std::cout << "      • BoundedVector(size_t max, vector<T> initial)  // Invariant" << std::endl;
    std::cout << "      • operator[](size_t idx)  // Can't return error" << std::endl;
    
    std::cout << "\n\n💀 3. TERMINATION (std::terminate, std::abort, assert)" << std::endl;
    std::cout << "   ✅ Use when:" << std::endl;
    std::cout << "      • PROGRAMMING ERROR (bug in code)" << std::endl;
    std::cout << "      • Invariant violated (internal consistency broken)" << std::endl;
    std::cout << "      • Precondition not met (contract violated)" << std::endl;
    std::cout << "      • Continuing would cause data corruption" << std::endl;
    std::cout << "      • Undefined behavior would result" << std::endl;
    std::cout << "\n   📝 Examples:" << std::endl;
    std::cout << "      • assert(ptr != nullptr);  // Debug check" << std::endl;
    std::cout << "      • if (invariant_broken()) std::terminate();" << std::endl;
    std::cout << "      • if (out_of_bounds) std::abort();" << std::endl;
    
    std::cout << "\n\n🔍 COMPARISON TABLE:" << std::endl;
    std::cout << "┌─────────────────┬──────────────┬──────────────┬──────────────┐" << std::endl;
    std::cout << "│ Aspect          │ Return Value │ Exception    │ Termination  │" << std::endl;
    std::cout << "├─────────────────┼──────────────┼──────────────┼──────────────┤" << std::endl;
    std::cout << "│ Error frequency │ Common       │ Rare         │ Never*       │" << std::endl;
    std::cout << "│ Recoverability  │ Recoverable  │ May recover  │ Fatal        │" << std::endl;
    std::cout << "│ Caller control  │ Explicit     │ Can catch    │ None         │" << std::endl;
    std::cout << "│ Performance     │ Fast         │ Slow         │ N/A          │" << std::endl;
    std::cout << "│ Constructor use │ No           │ Yes          │ Yes (rare)   │" << std::endl;
    std::cout << "│ RAII cleanup    │ Manual       │ Automatic    │ No           │" << std::endl;
    std::cout << "└─────────────────┴──────────────┴──────────────┴──────────────┘" << std::endl;
    std::cout << "* \"Never\" = should never happen in correct program" << std::endl;
    
    std::cout << "\n\n🎓 BJARNE'S KEY INSIGHTS:" << std::endl;
    std::cout << "\n   1️⃣  \"A constructor establishes the invariant for a class\"" << std::endl;
    std::cout << "      → Must throw if invariant cannot be established" << std::endl;
    std::cout << "      → No partially constructed objects" << std::endl;
    
    std::cout << "\n   2️⃣  \"Use exceptions for exceptional circumstances\"" << std::endl;
    std::cout << "      → Not for normal control flow" << std::endl;
    std::cout << "      → When caller can't be expected to check every call" << std::endl;
    
    std::cout << "\n   3️⃣  \"Use error codes when errors are expected\"" << std::endl;
    std::cout << "      → File not found, network timeout, parse error" << std::endl;
    std::cout << "      → Caller should handle explicitly" << std::endl;
    
    std::cout << "\n   4️⃣  \"Terminate when continuing would be dangerous\"" << std::endl;
    std::cout << "      → Programming errors (bugs)" << std::endl;
    std::cout << "      → Invariant violations" << std::endl;
    
    std::cout << "\n   5️⃣  \"RAII is fundamental to exception safety\"" << std::endl;
    std::cout << "      → Resources acquired in constructor" << std::endl;
    std::cout << "      → Released in destructor" << std::endl;
    std::cout << "      → Cleanup happens even with exceptions" << std::endl;
    
    std::cout << "\n================================================================" << std::endl;
}

// ===================================================================
// MAIN
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  ERROR HANDLING STRATEGIES" << std::endl;
    std::cout << "  Following Bjarne Stroustrup's Philosophy" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    try {
        demonstrate_return_values();
        demonstrate_exceptions();
        demonstrate_termination();
        print_decision_guide();
        
        std::cout << "\n✅ All demonstrations completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Unexpected exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
