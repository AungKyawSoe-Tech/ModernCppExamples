#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

// ===================================================================
// EXCEPTION WITH SOURCE LOCATION TRACKING
// ===================================================================
// Demonstrates how to capture and display source file name and line
// number when exceptions are thrown.
// Format: "ModuleName->Line #123: Error message"
// ===================================================================

// ===================================================================
// 1. BASIC EXCEPTION WITH SOURCE LOCATION
// ===================================================================

class SourceLocationException : public std::runtime_error {
private:
    std::string source_file;
    int source_line;
    std::string formatted_message;
    
    std::string format_message(const std::string& file, int line, const std::string& msg) {
        // Extract just the filename without path
        size_t last_slash = file.find_last_of("/\\");
        std::string filename = (last_slash != std::string::npos) 
            ? file.substr(last_slash + 1) 
            : file;
        
        std::ostringstream oss;
        oss << filename << "->Line #" << line << ": " << msg;
        return oss.str();
    }
    
public:
    SourceLocationException(const std::string& file, int line, const std::string& message)
        : std::runtime_error(format_message(file, line, message)),
          source_file(file),
          source_line(line),
          formatted_message(format_message(file, line, message)) {}
    
    const std::string& get_source_file() const noexcept { return source_file; }
    int get_source_line() const noexcept { return source_line; }
    const char* what() const noexcept override { return formatted_message.c_str(); }
};

// Macro to automatically capture __FILE__ and __LINE__
#define THROW_WITH_LOCATION(message) \
    throw SourceLocationException(__FILE__, __LINE__, message)

void demonstrate_basic_location() {
    std::cout << "\n=== 1. BASIC SOURCE LOCATION TRACKING ===" << std::endl;
    
    try {
        std::cout << "\nAbout to throw exception with location..." << std::endl;
        THROW_WITH_LOCATION("Something went wrong!");
    }
    catch (const SourceLocationException& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
        std::cout << "  Source file: " << e.get_source_file() << std::endl;
        std::cout << "  Source line: " << e.get_source_line() << std::endl;
    }
}

// ===================================================================
// 2. ENHANCED EXCEPTION WITH FUNCTION NAME
// ===================================================================

class DetailedException : public std::runtime_error {
private:
    std::string source_file;
    int source_line;
    std::string function_name;
    std::string error_message;
    std::string formatted_message;
    
    std::string format_message() {
        size_t last_slash = source_file.find_last_of("/\\");
        std::string filename = (last_slash != std::string::npos) 
            ? source_file.substr(last_slash + 1) 
            : source_file;
        
        std::ostringstream oss;
        oss << filename << "->Line #" << source_line;
        if (!function_name.empty()) {
            oss << " [" << function_name << "]";
        }
        oss << ": " << error_message;
        return oss.str();
    }
    
public:
    DetailedException(const std::string& file, int line, const std::string& function,
                     const std::string& message)
        : std::runtime_error(""),
          source_file(file),
          source_line(line),
          function_name(function),
          error_message(message) {
        formatted_message = format_message();
    }
    
    const std::string& get_source_file() const noexcept { return source_file; }
    int get_source_line() const noexcept { return source_line; }
    const std::string& get_function_name() const noexcept { return function_name; }
    const char* what() const noexcept override { return formatted_message.c_str(); }
};

// Macros for different detail levels
#define THROW_DETAILED(message) \
    throw DetailedException(__FILE__, __LINE__, __FUNCTION__, message)

#define THROW_DETAILED_CUSTOM(message, function) \
    throw DetailedException(__FILE__, __LINE__, function, message)

void some_function_that_fails() {
    THROW_DETAILED("Failed to process data");
}

void demonstrate_detailed_location() {
    std::cout << "\n=== 2. DETAILED EXCEPTION WITH FUNCTION NAME ===" << std::endl;
    
    try {
        std::cout << "\nCalling function that will throw..." << std::endl;
        some_function_that_fails();
    }
    catch (const DetailedException& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
        std::cout << "  Module: " << e.get_source_file() << std::endl;
        std::cout << "  Line: " << e.get_source_line() << std::endl;
        std::cout << "  Function: " << e.get_function_name() << std::endl;
    }
}

// ===================================================================
// 3. EXCEPTION HIERARCHY WITH SOURCE LOCATION
// ===================================================================

// Base exception with source location
class BaseLocationException : public std::runtime_error {
protected:
    std::string source_file;
    int source_line;
    std::string function_name;
    std::string error_type;
    std::string error_message;
    
    std::string format_message() const {
        size_t last_slash = source_file.find_last_of("/\\");
        std::string filename = (last_slash != std::string::npos) 
            ? source_file.substr(last_slash + 1) 
            : source_file;
        
        std::ostringstream oss;
        oss << "[" << error_type << "] ";
        oss << filename << "->Line #" << source_line;
        if (!function_name.empty()) {
            oss << " in " << function_name << "()";
        }
        oss << ": " << error_message;
        return oss.str();
    }
    
public:
    BaseLocationException(const std::string& file, int line, const std::string& function,
                         const std::string& type, const std::string& message)
        : std::runtime_error(message),
          source_file(file),
          source_line(line),
          function_name(function),
          error_type(type),
          error_message(message) {}
    
    virtual const char* what() const noexcept override {
        static std::string formatted;
        formatted = format_message();
        return formatted.c_str();
    }
    
    const std::string& get_source_file() const noexcept { return source_file; }
    int get_source_line() const noexcept { return source_line; }
    const std::string& get_function_name() const noexcept { return function_name; }
    const std::string& get_error_type() const noexcept { return error_type; }
};

// Specific exception types
class CameraException : public BaseLocationException {
public:
    CameraException(const std::string& file, int line, const std::string& function,
                   const std::string& message)
        : BaseLocationException(file, line, function, "CameraError", message) {}
};

class NetworkException : public BaseLocationException {
public:
    NetworkException(const std::string& file, int line, const std::string& function,
                    const std::string& message)
        : BaseLocationException(file, line, function, "NetworkError", message) {}
};

class ValidationException : public BaseLocationException {
public:
    ValidationException(const std::string& file, int line, const std::string& function,
                       const std::string& message)
        : BaseLocationException(file, line, function, "ValidationError", message) {}
};

// Macros for specific exception types
#define THROW_CAMERA_ERROR(message) \
    throw CameraException(__FILE__, __LINE__, __FUNCTION__, message)

#define THROW_NETWORK_ERROR(message) \
    throw NetworkException(__FILE__, __LINE__, __FUNCTION__, message)

#define THROW_VALIDATION_ERROR(message) \
    throw ValidationException(__FILE__, __LINE__, __FUNCTION__, message)

void camera_capture() {
    THROW_CAMERA_ERROR("Failed to capture image - device not responding");
}

void network_send_data() {
    THROW_NETWORK_ERROR("Connection timeout after 30 seconds");
}

void validate_input(int value) {
    if (value < 0) {
        THROW_VALIDATION_ERROR("Value must be non-negative");
    }
}

void demonstrate_exception_hierarchy() {
    std::cout << "\n=== 3. EXCEPTION HIERARCHY WITH SOURCE LOCATION ===" << std::endl;
    
    // Camera exception
    try {
        std::cout << "\nTesting camera exception..." << std::endl;
        camera_capture();
    }
    catch (const CameraException& e) {
        std::cout << "Caught: " << e.what() << std::endl;
    }
    
    // Network exception
    try {
        std::cout << "\nTesting network exception..." << std::endl;
        network_send_data();
    }
    catch (const NetworkException& e) {
        std::cout << "Caught: " << e.what() << std::endl;
    }
    
    // Validation exception
    try {
        std::cout << "\nTesting validation exception..." << std::endl;
        validate_input(-5);
    }
    catch (const ValidationException& e) {
        std::cout << "Caught: " << e.what() << std::endl;
    }
}

// ===================================================================
// 4. STACK TRACE SIMULATION
// ===================================================================

class StackTraceException : public std::runtime_error {
private:
    struct StackFrame {
        std::string file;
        int line;
        std::string function;
        
        std::string format() const {
            size_t last_slash = file.find_last_of("/\\");
            std::string filename = (last_slash != std::string::npos) 
                ? file.substr(last_slash + 1) 
                : file;
            return filename + "->Line #" + std::to_string(line) + " in " + function + "()";
        }
    };
    
    std::vector<StackFrame> stack_trace;
    std::string error_message;
    
    std::string format_full_message() const {
        std::ostringstream oss;
        oss << "Exception: " << error_message << "\n";
        oss << "Stack trace:\n";
        for (size_t i = 0; i < stack_trace.size(); ++i) {
            oss << "  #" << i << " " << stack_trace[i].format() << "\n";
        }
        return oss.str();
    }
    
public:
    StackTraceException(const std::string& message)
        : std::runtime_error(message), error_message(message) {}
    
    void add_frame(const std::string& file, int line, const std::string& function) {
        stack_trace.push_back({file, line, function});
    }
    
    const char* what() const noexcept override {
        static std::string formatted;
        formatted = format_full_message();
        return formatted.c_str();
    }
    
    const std::vector<StackFrame>& get_stack_trace() const { return stack_trace; }
};

// Macro to add current location to stack trace
#define ADD_STACK_FRAME(exception) \
    exception.add_frame(__FILE__, __LINE__, __FUNCTION__)

void level3_function() {
    throw StackTraceException("Critical error in deepest function");
}

void level2_function() {
    try {
        level3_function();
    }
    catch (StackTraceException& e) {
        ADD_STACK_FRAME(e);
        throw;  // Rethrow
    }
}

void level1_function() {
    try {
        level2_function();
    }
    catch (StackTraceException& e) {
        ADD_STACK_FRAME(e);
        throw;  // Rethrow
    }
}

void demonstrate_stack_trace() {
    std::cout << "\n=== 4. STACK TRACE SIMULATION ===" << std::endl;
    
    try {
        std::cout << "\nCalling nested functions..." << std::endl;
        level1_function();
    }
    catch (const StackTraceException& e) {
        std::cout << "\nCaught exception with stack trace:" << std::endl;
        std::cout << e.what() << std::endl;
    }
}

// ===================================================================
// 5. C++20 std::source_location (MODERN APPROACH)
// ===================================================================

#if __cpp_lib_source_location >= 201907L
#include <source_location>

class ModernLocationException : public std::runtime_error {
private:
    std::source_location location;
    std::string error_message;
    
    std::string format_message() const {
        std::string filename = location.file_name();
        size_t last_slash = filename.find_last_of("/\\");
        if (last_slash != std::string::npos) {
            filename = filename.substr(last_slash + 1);
        }
        
        std::ostringstream oss;
        oss << filename << "->Line #" << location.line() 
            << " in " << location.function_name() 
            << ": " << error_message;
        return oss.str();
    }
    
public:
    ModernLocationException(const std::string& message,
                           std::source_location loc = std::source_location::current())
        : std::runtime_error(message),
          location(loc),
          error_message(message) {}
    
    const char* what() const noexcept override {
        static std::string formatted;
        formatted = format_message();
        return formatted.c_str();
    }
    
    const std::source_location& get_location() const { return location; }
};

void modern_function_that_fails() {
    throw ModernLocationException("Modern exception with automatic location capture");
}

void demonstrate_modern_location() {
    std::cout << "\n=== 5. C++20 std::source_location ===" << std::endl;
    std::cout << "Automatic location capture without macros!" << std::endl;
    
    try {
        std::cout << "\nThrowing modern exception..." << std::endl;
        modern_function_that_fails();
    }
    catch (const ModernLocationException& e) {
        std::cout << "Caught: " << e.what() << std::endl;
        auto loc = e.get_location();
        std::cout << "  File: " << loc.file_name() << std::endl;
        std::cout << "  Line: " << loc.line() << std::endl;
        std::cout << "  Column: " << loc.column() << std::endl;
        std::cout << "  Function: " << loc.function_name() << std::endl;
    }
}
#else
void demonstrate_modern_location() {
    std::cout << "\n=== 5. C++20 std::source_location ===" << std::endl;
    std::cout << "⚠️  std::source_location not available (requires C++20)" << std::endl;
    std::cout << "Compiler support needed: GCC 11+, Clang 16+, MSVC 2019+" << std::endl;
}
#endif

// ===================================================================
// 6. PRACTICAL EXAMPLE: IMAGE PROCESSING WITH LOCATION TRACKING
// ===================================================================

class ImageProcessingException : public std::runtime_error {
private:
    std::string module;
    int line;
    std::string operation;
    std::string details;
    
public:
    ImageProcessingException(const std::string& file, int ln, 
                            const std::string& op, const std::string& det)
        : std::runtime_error(""),
          module(extract_filename(file)),
          line(ln),
          operation(op),
          details(det) {}
    
    const char* what() const noexcept override {
        static std::string msg;
        msg = module + "->Line #" + std::to_string(line) + 
              " [" + operation + "]: " + details;
        return msg.c_str();
    }
    
    static std::string extract_filename(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        return (pos != std::string::npos) ? path.substr(pos + 1) : path;
    }
};

#define THROW_IMAGE_ERROR(operation, details) \
    throw ImageProcessingException(__FILE__, __LINE__, operation, details)

class ImageProcessor {
public:
    void load_image(const std::string& filename) {
        if (filename.empty()) {
            THROW_IMAGE_ERROR("LoadImage", "Filename cannot be empty");
        }
        // Simulate file not found
        THROW_IMAGE_ERROR("LoadImage", "File not found: " + filename);
    }
    
    void resize_image(int width, int height) {
        if (width <= 0 || height <= 0) {
            THROW_IMAGE_ERROR("ResizeImage", 
                "Invalid dimensions: " + std::to_string(width) + "x" + std::to_string(height));
        }
    }
    
    void apply_filter(const std::string& filter_name) {
        std::vector<std::string> valid_filters = {"blur", "sharpen", "edge_detect"};
        bool found = false;
        for (const auto& f : valid_filters) {
            if (f == filter_name) {
                found = true;
                break;
            }
        }
        if (!found) {
            THROW_IMAGE_ERROR("ApplyFilter", "Unknown filter: " + filter_name);
        }
    }
};

void demonstrate_practical_example() {
    std::cout << "\n=== 6. PRACTICAL EXAMPLE: IMAGE PROCESSING ===" << std::endl;
    
    ImageProcessor processor;
    
    // Test 1: Load with empty filename
    try {
        std::cout << "\nTest 1: Loading image with empty filename..." << std::endl;
        processor.load_image("");
    }
    catch (const ImageProcessingException& e) {
        std::cout << "❌ Error: " << e.what() << std::endl;
    }
    
    // Test 2: Invalid resize dimensions
    try {
        std::cout << "\nTest 2: Resizing with invalid dimensions..." << std::endl;
        processor.resize_image(-100, 200);
    }
    catch (const ImageProcessingException& e) {
        std::cout << "❌ Error: " << e.what() << std::endl;
    }
    
    // Test 3: Unknown filter
    try {
        std::cout << "\nTest 3: Applying unknown filter..." << std::endl;
        processor.apply_filter("invalid_filter");
    }
    catch (const ImageProcessingException& e) {
        std::cout << "❌ Error: " << e.what() << std::endl;
    }
    
    // Test 4: File not found
    try {
        std::cout << "\nTest 4: Loading non-existent file..." << std::endl;
        processor.load_image("nonexistent_image.png");
    }
    catch (const ImageProcessingException& e) {
        std::cout << "❌ Error: " << e.what() << std::endl;
    }
}

// ===================================================================
// 7. DEBUGGING HELPER: EXCEPTION LOGGING
// ===================================================================

class LoggedException : public std::runtime_error {
private:
    std::string module;
    int line;
    std::string function;
    std::string message;
    
public:
    LoggedException(const std::string& file, int ln, const std::string& func,
                   const std::string& msg)
        : std::runtime_error(msg),
          module(extract_filename(file)),
          line(ln),
          function(func),
          message(msg) {
        log_exception();
    }
    
    void log_exception() const {
        std::cerr << "\n╔════════════════════════════════════════════════════════════" << std::endl;
        std::cerr << "║ EXCEPTION THROWN" << std::endl;
        std::cerr << "╠════════════════════════════════════════════════════════════" << std::endl;
        std::cerr << "║ Module:   " << module << std::endl;
        std::cerr << "║ Line:     #" << line << std::endl;
        std::cerr << "║ Function: " << function << "()" << std::endl;
        std::cerr << "║ Message:  " << message << std::endl;
        std::cerr << "╚════════════════════════════════════════════════════════════" << std::endl;
    }
    
    const char* what() const noexcept override {
        static std::string formatted;
        formatted = module + "->Line #" + std::to_string(line) + ": " + message;
        return formatted.c_str();
    }
    
    static std::string extract_filename(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        return (pos != std::string::npos) ? path.substr(pos + 1) : path;
    }
};

#define THROW_AND_LOG(message) \
    throw LoggedException(__FILE__, __LINE__, __FUNCTION__, message)

void function_with_logging() {
    THROW_AND_LOG("Critical error detected - system state corrupted");
}

void demonstrate_logging() {
    std::cout << "\n=== 7. EXCEPTION LOGGING ===" << std::endl;
    
    try {
        std::cout << "\nCalling function that logs exception details..." << std::endl;
        function_with_logging();
    }
    catch (const LoggedException& e) {
        std::cout << "\nException caught in main handler" << std::endl;
        std::cout << "Formatted: " << e.what() << std::endl;
    }
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  EXCEPTION SOURCE LOCATION TRACKING" << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "Format: ModuleName->Line #XXX: Error message" << std::endl;
    
    demonstrate_basic_location();
    demonstrate_detailed_location();
    demonstrate_exception_hierarchy();
    demonstrate_stack_trace();
    demonstrate_modern_location();
    demonstrate_practical_example();
    demonstrate_logging();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  IMPLEMENTATION TECHNIQUES" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n1️⃣  PREPROCESSOR MACROS (C++11/14/17):" << std::endl;
    std::cout << "   __FILE__     - Source file path" << std::endl;
    std::cout << "   __LINE__     - Line number" << std::endl;
    std::cout << "   __FUNCTION__ - Function name (compiler extension)" << std::endl;
    std::cout << "\n   Usage:" << std::endl;
    std::cout << "   #define THROW_WITH_LOC(msg) \\" << std::endl;
    std::cout << "       throw MyException(__FILE__, __LINE__, msg)" << std::endl;
    
    std::cout << "\n2️⃣  std::source_location (C++20):" << std::endl;
    std::cout << "   • Automatic location capture" << std::endl;
    std::cout << "   • No macros needed" << std::endl;
    std::cout << "   • Default parameter trick" << std::endl;
    std::cout << "\n   Usage:" << std::endl;
    std::cout << "   throw MyException(message, std::source_location::current());" << std::endl;
    
    std::cout << "\n3️⃣  CUSTOM EXCEPTION CLASS:" << std::endl;
    std::cout << "   class MyException : public std::runtime_error {" << std::endl;
    std::cout << "       std::string file;" << std::endl;
    std::cout << "       int line;" << std::endl;
    std::cout << "   public:" << std::endl;
    std::cout << "       MyException(const string& f, int l, const string& msg);" << std::endl;
    std::cout << "       const char* what() const noexcept override;" << std::endl;
    std::cout << "   };" << std::endl;
    
    std::cout << "\n4️⃣  MESSAGE FORMATTING:" << std::endl;
    std::cout << "   Format: \"ModuleName->Line #123: Error message\"" << std::endl;
    std::cout << "   • Extract filename from path" << std::endl;
    std::cout << "   • Use std::ostringstream for formatting" << std::endl;
    std::cout << "   • Store formatted message in exception" << std::endl;
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  BEST PRACTICES" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n✅ DO:" << std::endl;
    std::cout << "   • Use macros to automatically capture location" << std::endl;
    std::cout << "   • Extract filename only (not full path)" << std::endl;
    std::cout << "   • Include function name for better context" << std::endl;
    std::cout << "   • Format consistently: Module->Line #XXX" << std::endl;
    std::cout << "   • Store location info in exception members" << std::endl;
    std::cout << "   • Use std::source_location in C++20 (no macros!)" << std::endl;
    
    std::cout << "\n❌ DON'T:" << std::endl;
    std::cout << "   • Don't include full file paths (too verbose)" << std::endl;
    std::cout << "   • Don't hardcode file/line (will be wrong)" << std::endl;
    std::cout << "   • Don't forget to extract filename from path" << std::endl;
    std::cout << "   • Don't use exceptions for control flow" << std::endl;
    
    std::cout << "\n📊 COMPARISON:" << std::endl;
    std::cout << "\nMacro Approach (C++11-17):" << std::endl;
    std::cout << "   ✅ Works with older compilers" << std::endl;
    std::cout << "   ✅ Full control over formatting" << std::endl;
    std::cout << "   ❌ Requires macros (ugly)" << std::endl;
    std::cout << "   ❌ Manual __FILE__, __LINE__ passing" << std::endl;
    
    std::cout << "\nstd::source_location (C++20):" << std::endl;
    std::cout << "   ✅ No macros needed" << std::endl;
    std::cout << "   ✅ Automatic capture via default parameter" << std::endl;
    std::cout << "   ✅ Type-safe and elegant" << std::endl;
    std::cout << "   ❌ Requires C++20 compiler support" << std::endl;
    
    std::cout << "\n💡 REAL-WORLD USAGE:" << std::endl;
    std::cout << "\nDefensive Programming:" << std::endl;
    std::cout << "   if (!validate(data)) {" << std::endl;
    std::cout << "       THROW_WITH_LOCATION(\"Invalid data format\");" << std::endl;
    std::cout << "   }" << std::endl;
    
    std::cout << "\nResource Management:" << std::endl;
    std::cout << "   if (!file.open()) {" << std::endl;
    std::cout << "       THROW_WITH_LOCATION(\"Failed to open: \" + filename);" << std::endl;
    std::cout << "   }" << std::endl;
    
    std::cout << "\nAPI Boundary Checking:" << std::endl;
    std::cout << "   if (index >= size()) {" << std::endl;
    std::cout << "       THROW_WITH_LOCATION(\"Index out of range\");" << std::endl;
    std::cout << "   }" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
