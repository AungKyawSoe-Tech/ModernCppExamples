#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <variant>
#include <fstream>
#include <system_error>
#include <cassert>
#include <type_traits>

// ===================================================================
// C++ ERROR HANDLING: COMPREHENSIVE EXAMPLES
// ===================================================================
// Demonstrates:
// 1. Throw by value, catch by reference (BEST PRACTICE)
// 2. Standard exception hierarchy
// 3. Custom exceptions
// 4. Exception safety guarantees
// 5. Compile-time error detection
// 6. Alternative error handling patterns
// ===================================================================

// ===================================================================
// 1. STANDARD EXCEPTIONS - THROW BY VALUE, CATCH BY REFERENCE
// ===================================================================

void demonstrate_basic_exception_handling() {
    std::cout << "\n=== 1. BASIC EXCEPTION HANDLING ===" << std::endl;
    std::cout << "Rule: THROW BY VALUE, CATCH BY REFERENCE" << std::endl;
    
    // Example 1: std::runtime_error
    try {
        std::cout << "\nThrowing std::runtime_error..." << std::endl;
        throw std::runtime_error("Something went wrong at runtime!");  // Throw by value
    }
    catch (const std::runtime_error& e) {  // Catch by const reference
        std::cout << "Caught runtime_error: " << e.what() << std::endl;
    }
    
    // Example 2: std::logic_error
    try {
        std::cout << "\nThrowing std::logic_error..." << std::endl;
        throw std::logic_error("Logic error in the program!");  // Throw by value
    }
    catch (const std::logic_error& e) {  // Catch by const reference
        std::cout << "Caught logic_error: " << e.what() << std::endl;
    }
    
    // Example 3: std::invalid_argument
    try {
        std::cout << "\nThrowing std::invalid_argument..." << std::endl;
        throw std::invalid_argument("Invalid argument provided!");  // Throw by value
    }
    catch (const std::invalid_argument& e) {  // Catch by const reference
        std::cout << "Caught invalid_argument: " << e.what() << std::endl;
    }
    
    // Example 4: std::out_of_range
    try {
        std::cout << "\nThrowing std::out_of_range..." << std::endl;
        throw std::out_of_range("Index out of range!");  // Throw by value
    }
    catch (const std::out_of_range& e) {  // Catch by const reference
        std::cout << "Caught out_of_range: " << e.what() << std::endl;
    }
}

// ===================================================================
// 2. STANDARD EXCEPTION HIERARCHY
// ===================================================================

void demonstrate_exception_hierarchy() {
    std::cout << "\n=== 2. EXCEPTION HIERARCHY ===" << std::endl;
    std::cout << "std::exception (base)" << std::endl;
    std::cout << "├── std::logic_error" << std::endl;
    std::cout << "│   ├── std::invalid_argument" << std::endl;
    std::cout << "│   ├── std::domain_error" << std::endl;
    std::cout << "│   ├── std::length_error" << std::endl;
    std::cout << "│   └── std::out_of_range" << std::endl;
    std::cout << "└── std::runtime_error" << std::endl;
    std::cout << "    ├── std::range_error" << std::endl;
    std::cout << "    ├── std::overflow_error" << std::endl;
    std::cout << "    └── std::underflow_error" << std::endl;
    
    // Catch base class to handle all derived exceptions
    try {
        std::cout << "\nThrowing derived exception..." << std::endl;
        throw std::overflow_error("Integer overflow occurred!");
    }
    catch (const std::runtime_error& e) {  // Catches std::overflow_error (derived)
        std::cout << "Caught via base class runtime_error: " << e.what() << std::endl;
    }
    
    // Multiple catch blocks - order matters!
    try {
        std::cout << "\nDemonstrating catch order..." << std::endl;
        throw std::invalid_argument("Bad argument");
    }
    catch (const std::invalid_argument& e) {  // Specific exception first
        std::cout << "Caught specific: invalid_argument: " << e.what() << std::endl;
    }
    catch (const std::logic_error& e) {  // Base class second
        std::cout << "Caught base: logic_error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {  // Most general last
        std::cout << "Caught most general: exception: " << e.what() << std::endl;
    }
}

// ===================================================================
// 3. CUSTOM EXCEPTIONS
// ===================================================================

// Custom exception class - inherit from std::exception
class CameraException : public std::runtime_error {
private:
    int error_code;
    
public:
    explicit CameraException(const std::string& message, int code = 0)
        : std::runtime_error(message), error_code(code) {}
    
    int get_error_code() const noexcept { return error_code; }
};

class CameraNotConnectedException : public CameraException {
public:
    explicit CameraNotConnectedException(const std::string& camera_name)
        : CameraException("Camera not connected: " + camera_name, 100) {}
};

class CameraConfigurationException : public CameraException {
public:
    explicit CameraConfigurationException(const std::string& config_error)
        : CameraException("Configuration error: " + config_error, 200) {}
};

void demonstrate_custom_exceptions() {
    std::cout << "\n=== 3. CUSTOM EXCEPTIONS ===" << std::endl;
    
    // Throw custom exception by value
    try {
        std::cout << "\nThrowing CameraNotConnectedException..." << std::endl;
        throw CameraNotConnectedException("USB-CAM-001");  // Throw by value
    }
    catch (const CameraNotConnectedException& e) {  // Catch by const reference
        std::cout << "Caught: " << e.what() << std::endl;
        std::cout << "Error code: " << e.get_error_code() << std::endl;
    }
    
    // Catch hierarchy with custom exceptions
    try {
        std::cout << "\nThrowing CameraConfigurationException..." << std::endl;
        throw CameraConfigurationException("Invalid resolution: 9999x9999");
    }
    catch (const CameraException& e) {  // Catch base custom exception
        std::cout << "Caught via base CameraException: " << e.what() << std::endl;
        std::cout << "Error code: " << e.get_error_code() << std::endl;
    }
    catch (const std::exception& e) {  // Catch any std::exception
        std::cout << "Caught via std::exception: " << e.what() << std::endl;
    }
}

// ===================================================================
// 4. EXCEPTION SAFETY GUARANTEES
// ===================================================================

class SafeVector {
private:
    std::vector<int> data;
    
public:
    // Basic guarantee: No resources leaked, but object state may change
    void push_back_basic(int value) {
        data.push_back(value);  // May throw, but vector cleans up
    }
    
    // Strong guarantee: Operation succeeds or has no effect (rollback)
    void push_back_strong(int value) {
        std::vector<int> temp = data;  // Copy current state
        temp.push_back(value);         // Modify copy (if this throws, original unchanged)
        data = std::move(temp);        // Commit (noexcept move)
    }
    
    // No-throw guarantee: Marked noexcept
    size_t size() const noexcept {
        return data.size();
    }
    
    // No-throw guarantee: Never throws
    void clear() noexcept {
        data.clear();
    }
    
    int& at(size_t index) {
        return data.at(index);  // May throw std::out_of_range
    }
};

void demonstrate_exception_safety() {
    std::cout << "\n=== 4. EXCEPTION SAFETY GUARANTEES ===" << std::endl;
    
    std::cout << "\nException Safety Levels:" << std::endl;
    std::cout << "1. No-throw guarantee (noexcept): Never throws" << std::endl;
    std::cout << "2. Strong guarantee: Rollback on exception" << std::endl;
    std::cout << "3. Basic guarantee: No resource leaks" << std::endl;
    std::cout << "4. No guarantee: May leak resources" << std::endl;
    
    SafeVector vec;
    vec.push_back_basic(1);
    vec.push_back_basic(2);
    
    try {
        std::cout << "\nAccessing valid index: vec.at(0)" << std::endl;
        std::cout << "Value: " << vec.at(0) << std::endl;
        
        std::cout << "\nAccessing invalid index: vec.at(100)" << std::endl;
        vec.at(100);  // Throws std::out_of_range
    }
    catch (const std::out_of_range& e) {
        std::cout << "Caught: " << e.what() << std::endl;
        std::cout << "Vector is still valid, size: " << vec.size() << std::endl;
    }
}

// ===================================================================
// 5. RAII AND EXCEPTION SAFETY
// ===================================================================

class FileHandle {
private:
    std::string filename;
    FILE* file;
    
public:
    explicit FileHandle(const std::string& fname, const char* mode) 
        : filename(fname), file(nullptr) {
        file = fopen(fname.c_str(), mode);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + fname);
        }
        std::cout << "File opened: " << filename << std::endl;
    }
    
    ~FileHandle() {
        if (file) {
            fclose(file);
            std::cout << "File closed: " << filename << std::endl;
        }
    }
    
    // Delete copy operations
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    
    // Move operations
    FileHandle(FileHandle&& other) noexcept 
        : filename(std::move(other.filename)), file(other.file) {
        other.file = nullptr;
    }
    
    FILE* get() const noexcept { return file; }
};

void demonstrate_raii() {
    std::cout << "\n=== 5. RAII AND EXCEPTION SAFETY ===" << std::endl;
    
    try {
        std::cout << "\nOpening file (will throw)..." << std::endl;
        FileHandle handle("nonexistent_file_xyz.txt", "r");
        std::cout << "This won't print" << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Caught: " << e.what() << std::endl;
        std::cout << "No resource leak - file was never opened!" << std::endl;
    }
    
    std::cout << "\nDemonstrating automatic cleanup with scope:" << std::endl;
    try {
        // Create temporary file for demo
        {
            FileHandle handle1("temp_test.txt", "w");
            fprintf(handle1.get(), "Test data\n");
            // File automatically closed when handle1 goes out of scope
        }
        std::cout << "File closed automatically (RAII)" << std::endl;
        
        // Clean up temp file
        std::remove("temp_test.txt");
    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
}

// ===================================================================
// 6. COMPILE-TIME ERROR DETECTION
// ===================================================================

// Static assertions - compile-time checks
template<typename T>
class Buffer {
private:
    std::vector<T> data;
    
public:
    // Compile-time check: Only allow trivially copyable types
    static_assert(std::is_trivially_copyable_v<T>, 
                  "Buffer only works with trivially copyable types!");
    
    Buffer(size_t size) : data(size) {}
    
    T* get_data() { return data.data(); }
};

// constexpr for compile-time evaluation
constexpr int divide_compile_time(int a, int b) {
    // This will cause compile error if b is 0 at compile time
    return (b == 0) ? throw std::invalid_argument("Division by zero!") : a / b;
}

// C++20 Concepts for compile-time type checking
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<Numeric T>
T safe_divide(T a, T b) {
    if (b == T(0)) {
        throw std::invalid_argument("Division by zero!");
    }
    return a / b;
}

void demonstrate_compile_time_checks() {
    std::cout << "\n=== 6. COMPILE-TIME ERROR DETECTION ===" << std::endl;
    
    std::cout << "\n1. Static Assertions:" << std::endl;
    Buffer<int> int_buffer(10);  // OK: int is trivially copyable
    std::cout << "   ✅ Buffer<int> compiles (trivially copyable)" << std::endl;
    
    // This would NOT compile:
    // Buffer<std::string> string_buffer(10);  // ERROR: string not trivially copyable
    std::cout << "   ❌ Buffer<std::string> would fail to compile!" << std::endl;
    
    std::cout << "\n2. constexpr Functions:" << std::endl;
    constexpr int result = divide_compile_time(10, 2);  // OK at compile time
    std::cout << "   10 / 2 = " << result << " (evaluated at compile time)" << std::endl;
    
    // This would NOT compile:
    // constexpr int error = divide_compile_time(10, 0);  // Compile error!
    std::cout << "   10 / 0 would fail at compile time!" << std::endl;
    
    std::cout << "\n3. Concepts (C++20):" << std::endl;
    try {
        int a = safe_divide(10, 2);
        std::cout << "   10 / 2 = " << a << std::endl;
        
        double b = safe_divide(10.0, 0.0);  // Runtime error
        std::cout << "   This won't print: " << b << std::endl;
    }
    catch (const std::invalid_argument& e) {
        std::cout << "   Caught: " << e.what() << std::endl;
    }
    
    // This would NOT compile:
    // safe_divide(std::string("10"), std::string("2"));  // Concept constraint violated!
    std::cout << "   safe_divide with std::string would fail to compile!" << std::endl;
}

// ===================================================================
// 7. NOEXCEPT SPECIFICATION
// ===================================================================

class NoexceptDemo {
public:
    // Guaranteed not to throw
    int get_value() const noexcept {
        return 42;
    }
    
    // May throw
    int divide(int a, int b) {
        if (b == 0) {
            throw std::invalid_argument("Division by zero");
        }
        return a / b;
    }
    
    // Conditionally noexcept based on template parameter
    template<typename T>
    void swap(T& a, T& b) noexcept(std::is_nothrow_move_constructible_v<T>) {
        T temp = std::move(a);
        a = std::move(b);
        b = std::move(temp);
    }
};

void demonstrate_noexcept() {
    std::cout << "\n=== 7. NOEXCEPT SPECIFICATION ===" << std::endl;
    
    NoexceptDemo demo;
    
    std::cout << "\nnoexcept functions:" << std::endl;
    std::cout << "  get_value() is noexcept: " 
              << noexcept(demo.get_value()) << std::endl;
    std::cout << "  divide() is noexcept: " 
              << noexcept(demo.divide(10, 2)) << std::endl;
    
    std::cout << "\nBenefits of noexcept:" << std::endl;
    std::cout << "  • Compiler optimizations" << std::endl;
    std::cout << "  • Move constructors in std::vector" << std::endl;
    std::cout << "  • Enables certain optimizations in algorithms" << std::endl;
    std::cout << "  • Self-documenting code" << std::endl;
    
    std::cout << "\n⚠️  If noexcept function throws: std::terminate() is called!" << std::endl;
}

// ===================================================================
// 8. ALTERNATIVE ERROR HANDLING: std::optional
// ===================================================================

std::optional<int> safe_divide_optional(int a, int b) {
    if (b == 0) {
        return std::nullopt;  // Return empty optional instead of throwing
    }
    return a / b;
}

void demonstrate_optional() {
    std::cout << "\n=== 8. ERROR HANDLING WITH std::optional ===" << std::endl;
    std::cout << "Alternative to exceptions for expected failures" << std::endl;
    
    auto result1 = safe_divide_optional(10, 2);
    if (result1) {
        std::cout << "\n10 / 2 = " << *result1 << " ✅" << std::endl;
    } else {
        std::cout << "\n10 / 2 failed ❌" << std::endl;
    }
    
    auto result2 = safe_divide_optional(10, 0);
    if (result2) {
        std::cout << "10 / 0 = " << *result2 << " ✅" << std::endl;
    } else {
        std::cout << "10 / 0 failed (expected) ❌" << std::endl;
    }
    
    std::cout << "\nBenefits:" << std::endl;
    std::cout << "  • No exception overhead" << std::endl;
    std::cout << "  • Makes errors explicit in return type" << std::endl;
    std::cout << "  • Good for expected failures" << std::endl;
}

// ===================================================================
// 9. ALTERNATIVE ERROR HANDLING: std::variant
// ===================================================================

struct Error {
    int code;
    std::string message;
};

std::variant<int, Error> safe_divide_variant(int a, int b) {
    if (b == 0) {
        return Error{1, "Division by zero"};
    }
    return a / b;
}

void demonstrate_variant() {
    std::cout << "\n=== 9. ERROR HANDLING WITH std::variant ===" << std::endl;
    std::cout << "Return either result or error" << std::endl;
    
    auto result1 = safe_divide_variant(10, 2);
    if (std::holds_alternative<int>(result1)) {
        std::cout << "\n10 / 2 = " << std::get<int>(result1) << " ✅" << std::endl;
    } else {
        const auto& err = std::get<Error>(result1);
        std::cout << "\nError " << err.code << ": " << err.message << " ❌" << std::endl;
    }
    
    auto result2 = safe_divide_variant(10, 0);
    if (std::holds_alternative<int>(result2)) {
        std::cout << "10 / 0 = " << std::get<int>(result2) << " ✅" << std::endl;
    } else {
        const auto& err = std::get<Error>(result2);
        std::cout << "Error " << err.code << ": " << err.message << " ❌" << std::endl;
    }
    
    std::cout << "\nBenefits:" << std::endl;
    std::cout << "  • Can return detailed error information" << std::endl;
    std::cout << "  • Type-safe error handling" << std::endl;
    std::cout << "  • No exception overhead" << std::endl;
}

// ===================================================================
// 10. RETHROWING AND NESTED EXCEPTIONS
// ===================================================================

void inner_function() {
    throw std::runtime_error("Error in inner function");
}

void middle_function() {
    try {
        inner_function();
    }
    catch (...) {
        std::cout << "middle_function: Caught exception, rethrowing..." << std::endl;
        throw;  // Rethrow the same exception
    }
}

void demonstrate_rethrowing() {
    std::cout << "\n=== 10. RETHROWING EXCEPTIONS ===" << std::endl;
    
    try {
        middle_function();
    }
    catch (const std::runtime_error& e) {
        std::cout << "Caught in demonstrate_rethrowing: " << e.what() << std::endl;
    }
    
    std::cout << "\nRethrow with throw; (not throw e;)" << std::endl;
    std::cout << "  throw;  - Rethrows original exception (correct)" << std::endl;
    std::cout << "  throw e; - Creates new exception (slicing!)" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  C++ ERROR HANDLING: COMPREHENSIVE GUIDE" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    demonstrate_basic_exception_handling();
    demonstrate_exception_hierarchy();
    demonstrate_custom_exceptions();
    demonstrate_exception_safety();
    demonstrate_raii();
    demonstrate_compile_time_checks();
    demonstrate_noexcept();
    demonstrate_optional();
    demonstrate_variant();
    demonstrate_rethrowing();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  BEST PRACTICES SUMMARY" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n✅ EXCEPTION HANDLING RULES:" << std::endl;
    std::cout << "\n1. THROW BY VALUE, CATCH BY REFERENCE" << std::endl;
    std::cout << "   ✅ throw std::runtime_error(\"message\");" << std::endl;
    std::cout << "   ✅ catch (const std::runtime_error& e)" << std::endl;
    std::cout << "   ❌ throw new std::runtime_error(\"message\");  // NO!" << std::endl;
    std::cout << "   ❌ catch (std::runtime_error e)  // Slicing!" << std::endl;
    
    std::cout << "\n2. CATCH ORDER MATTERS" << std::endl;
    std::cout << "   • Catch specific exceptions first" << std::endl;
    std::cout << "   • Catch base classes last" << std::endl;
    std::cout << "   • catch(...) for unknown exceptions" << std::endl;
    
    std::cout << "\n3. USE STANDARD EXCEPTIONS" << std::endl;
    std::cout << "   • std::runtime_error - Runtime failures" << std::endl;
    std::cout << "   • std::logic_error - Programming errors" << std::endl;
    std::cout << "   • std::invalid_argument - Bad function arguments" << std::endl;
    std::cout << "   • std::out_of_range - Index out of bounds" << std::endl;
    
    std::cout << "\n4. RAII FOR EXCEPTION SAFETY" << std::endl;
    std::cout << "   • Use smart pointers (std::unique_ptr, std::shared_ptr)" << std::endl;
    std::cout << "   • RAII wrappers for resources" << std::endl;
    std::cout << "   • Destructors never throw" << std::endl;
    
    std::cout << "\n5. NOEXCEPT FOR NON-THROWING FUNCTIONS" << std::endl;
    std::cout << "   • Mark functions that never throw" << std::endl;
    std::cout << "   • Enables compiler optimizations" << std::endl;
    std::cout << "   • Move constructors should be noexcept" << std::endl;
    
    std::cout << "\n6. COMPILE-TIME ERROR DETECTION" << std::endl;
    std::cout << "   • static_assert for compile-time checks" << std::endl;
    std::cout << "   • constexpr for compile-time evaluation" << std::endl;
    std::cout << "   • Concepts (C++20) for type constraints" << std::endl;
    std::cout << "   • Template SFINAE for type checking" << std::endl;
    
    std::cout << "\n7. ALTERNATIVES TO EXCEPTIONS" << std::endl;
    std::cout << "   • std::optional<T> - For expected failures" << std::endl;
    std::cout << "   • std::variant<T, Error> - Return result or error" << std::endl;
    std::cout << "   • Error codes - For performance-critical code" << std::endl;
    std::cout << "   • std::expected<T, E> (C++23) - Best of both worlds" << std::endl;
    
    std::cout << "\n📊 WHEN TO USE WHAT:" << std::endl;
    std::cout << "\nExceptions:" << std::endl;
    std::cout << "  ✅ Unexpected errors (file not found, network failure)" << std::endl;
    std::cout << "  ✅ Constructor failures" << std::endl;
    std::cout << "  ✅ Deep call stacks (error propagation)" << std::endl;
    std::cout << "  ❌ Performance-critical code" << std::endl;
    std::cout << "  ❌ Expected failures (validation)" << std::endl;
    
    std::cout << "\nstd::optional:" << std::endl;
    std::cout << "  ✅ Expected failures (search not found)" << std::endl;
    std::cout << "  ✅ Optional return values" << std::endl;
    std::cout << "  ✅ Performance-critical code" << std::endl;
    std::cout << "  ❌ Need detailed error information" << std::endl;
    
    std::cout << "\nstd::variant<T, Error>:" << std::endl;
    std::cout << "  ✅ Need detailed error information" << std::endl;
    std::cout << "  ✅ Multiple error types" << std::endl;
    std::cout << "  ✅ Performance-critical code" << std::endl;
    std::cout << "  ❌ Simple success/failure cases" << std::endl;
    
    std::cout << "\nCompile-time checks:" << std::endl;
    std::cout << "  ✅ Type constraints" << std::endl;
    std::cout << "  ✅ API misuse prevention" << std::endl;
    std::cout << "  ✅ Zero runtime cost" << std::endl;
    std::cout << "  ✅ Early error detection" << std::endl;
    
    std::cout << "\n⚡ PERFORMANCE NOTES:" << std::endl;
    std::cout << "  • Exceptions have zero cost if not thrown (modern compilers)" << std::endl;
    std::cout << "  • Throwing exception is expensive (~1000x slower than return)" << std::endl;
    std::cout << "  • noexcept enables optimizations (especially in std::vector)" << std::endl;
    std::cout << "  • Compile-time checks have zero runtime cost" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
