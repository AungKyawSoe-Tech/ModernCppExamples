// ===================================================================
// MISRA C++ CODING GUIDELINES DEMONSTRATION
// ===================================================================
// Demonstrates key MISRA C++ rules for safety-critical systems
// MISRA C++:2008 and MISRA C++:2023 guidelines
//
// MISRA C++ focuses on:
// - Avoiding undefined behavior
// - Avoiding implementation-defined behavior
// - Defensive programming practices
// - Code clarity and maintainability
// - Safety in automotive, aerospace, medical devices
//
// Build: g++ -std=c++20 -Wall -Wextra -Wpedantic -O2 -o MISRACppDemo MISRACppDemo.cpp
// ===================================================================

#include <iostream>
#include <cstdint>
#include <array>
#include <limits>
#include <memory>
#include <string>
#include <algorithm>  // for std::copy_n

// ===================================================================
// RULE CATEGORY 1: TYPES AND DECLARATIONS
// ===================================================================

namespace types_and_declarations {

// MISRA Rule 3-9-1: Use fixed-width integer types from <cstdint>
// Rationale: Ensures portability across platforms

// ❌ BAD: Platform-dependent types
void bad_types() {
    std::cout << "\n❌ BAD: Platform-dependent types\n";
    
    int x = 100;           // Size undefined (16, 32, or 64 bits?)
    long y = 1000;         // Size undefined (32 or 64 bits?)
    unsigned int z = 500;  // Size undefined
    
    std::cout << "   int size: " << sizeof(x) << " bytes (undefined!)\n";
    std::cout << "   long size: " << sizeof(y) << " bytes (undefined!)\n";
}

// ✅ GOOD: Fixed-width types
void good_types() {
    std::cout << "\n✅ GOOD: Fixed-width types\n";
    
    int32_t x = 100;       // Always 32 bits
    int64_t y = 1000;      // Always 64 bits
    uint32_t z = 500;      // Always unsigned 32 bits
    
    std::cout << "   int32_t: " << x << " (always 32 bits)\n";
    std::cout << "   int64_t: " << y << " (always 64 bits)\n";
    std::cout << "   uint32_t: " << z << " (always unsigned 32 bits)\n";
}

// MISRA Rule 4-5-1: Expressions with type bool shall not be used with operators other than
// =, ==, !=, !, &&, ||

// ❌ BAD: Arithmetic on bool
void bad_bool_usage() {
    std::cout << "\n❌ BAD: Arithmetic on bool\n";
    
    bool flag1 = true;
    bool flag2 = false;
    
    // int result = flag1 + flag2;  // MISRA violation!
    // int value = flag1 * 5;       // MISRA violation!
    
    std::cout << "   Arithmetic on bool is prohibited by MISRA\n";
}

// ✅ GOOD: Proper bool usage
void good_bool_usage() {
    std::cout << "\n✅ GOOD: Proper bool usage\n";
    
    bool flag1 = true;
    bool flag2 = false;
    
    bool result = flag1 && flag2;  // OK: logical operator
    bool equal = (flag1 == flag2); // OK: comparison
    bool negate = !flag1;          // OK: logical NOT
    
    std::cout << "   flag1 && flag2 = " << result << "\n";
    std::cout << "   flag1 == flag2 = " << equal << "\n";
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "MISRA CATEGORY 1: TYPES AND DECLARATIONS\n";
    std::cout << std::string(70, '=') << "\n";
    
    bad_types();
    good_types();
    bad_bool_usage();
    good_bool_usage();
}

} // namespace types_and_declarations

// ===================================================================
// RULE CATEGORY 2: EXPRESSIONS AND OPERATORS
// ===================================================================

namespace expressions_and_operators {

// MISRA Rule 5-0-21: Bitwise operators shall only be applied to operands of unsigned type

// ❌ BAD: Bitwise operations on signed integers
void bad_bitwise() {
    std::cout << "\n❌ BAD: Bitwise operations on signed integers\n";
    
    int32_t signed_val = -1;
    // int32_t result = signed_val << 2;  // MISRA violation! Undefined behavior
    // int32_t mask = signed_val & 0xFF;  // MISRA violation!
    
    std::cout << "   Bitwise ops on signed integers can cause undefined behavior\n";
}

// ✅ GOOD: Bitwise operations on unsigned integers
void good_bitwise() {
    std::cout << "\n✅ GOOD: Bitwise operations on unsigned integers\n";
    
    uint32_t unsigned_val = 0xF0;
    uint32_t result = unsigned_val << 2;  // OK: unsigned
    uint32_t mask = unsigned_val & 0xFF;  // OK: unsigned
    
    std::cout << "   unsigned_val << 2 = 0x" << std::hex << result << "\n";
    std::cout << "   unsigned_val & 0xFF = 0x" << mask << std::dec << "\n";
}

// MISRA Rule 5-0-5: There shall be no implicit floating-integral conversions

// ❌ BAD: Implicit conversions
void bad_conversions() {
    std::cout << "\n❌ BAD: Implicit floating-integral conversions\n";
    
    float f = 3.14f;
    // int32_t i = f;        // MISRA violation! Implicit conversion
    // float result = 5 + f; // MISRA violation! Mixed type arithmetic
    
    std::cout << "   Implicit float-int conversions lose precision\n";
}

// ✅ GOOD: Explicit conversions
void good_conversions() {
    std::cout << "\n✅ GOOD: Explicit conversions\n";
    
    float f = 3.14f;
    int32_t i = static_cast<int32_t>(f);  // OK: explicit cast
    float result = static_cast<float>(5) + f;  // OK: explicit cast
    
    std::cout << "   static_cast<int32_t>(3.14f) = " << i << "\n";
    std::cout << "   Explicit casts make intent clear\n";
}

// MISRA Rule 5-0-6: An implicit integral or floating-point conversion shall not reduce
// the size of the underlying type

// ❌ BAD: Narrowing conversions
void bad_narrowing() {
    std::cout << "\n❌ BAD: Narrowing conversions\n";
    
    int64_t large = 1000000;
    // int16_t small = large;  // MISRA violation! Data loss possible
    
    std::cout << "   Narrowing conversions can lose data\n";
}

// ✅ GOOD: Safe conversions or explicit narrowing
void good_narrowing() {
    std::cout << "\n✅ GOOD: Safe conversions with range checks\n";
    
    int64_t large = 1000;
    
    // Check before narrowing
    if (large <= std::numeric_limits<int16_t>::max() && 
        large >= std::numeric_limits<int16_t>::min()) {
        int16_t small = static_cast<int16_t>(large);  // OK: checked and explicit
        std::cout << "   Safe narrowing: " << small << "\n";
    } else {
        std::cout << "   Value out of range for int16_t\n";
    }
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "MISRA CATEGORY 2: EXPRESSIONS AND OPERATORS\n";
    std::cout << std::string(70, '=') << "\n";
    
    bad_bitwise();
    good_bitwise();
    bad_conversions();
    good_conversions();
    bad_narrowing();
    good_narrowing();
}

} // namespace expressions_and_operators

// ===================================================================
// RULE CATEGORY 3: STATEMENTS
// ===================================================================

namespace statements {

// MISRA Rule 6-4-1: An if (condition) construct shall be followed by a compound statement
// MISRA Rule 6-4-4: A switch statement shall have at least two case clauses

// ❌ BAD: Single-statement if without braces
void bad_if_statements() {
    std::cout << "\n❌ BAD: Single-statement if without braces\n";
    
    int32_t value = 5;
    
    // if (value > 0)
    //     std::cout << "Positive\n";  // MISRA violation! No braces
    
    std::cout << "   Missing braces can lead to errors during maintenance\n";
}

// ✅ GOOD: Always use braces
void good_if_statements() {
    std::cout << "\n✅ GOOD: Always use braces\n";
    
    int32_t value = 5;
    
    if (value > 0) {  // OK: compound statement
        std::cout << "   Positive\n";
    } else {
        std::cout << "   Non-positive\n";
    }
}

// MISRA Rule 6-4-5: An unconditional throw or break shall terminate every non-empty
// switch clause

// ❌ BAD: Fall-through in switch
void bad_switch(int32_t value) {
    std::cout << "\n❌ BAD: Fall-through in switch\n";
    
    // switch (value) {
    //     case 1:
    //         std::cout << "One\n";
    //         // Fall-through! MISRA violation
    //     case 2:
    //         std::cout << "Two\n";
    //         break;
    //     default:
    //         std::cout << "Other\n";
    // }
    
    std::cout << "   Fall-through is error-prone\n";
}

// ✅ GOOD: Explicit break in every case
void good_switch(int32_t value) {
    std::cout << "\n✅ GOOD: Explicit break in every case\n";
    
    switch (value) {
        case 1: {
            std::cout << "   One\n";
            break;
        }
        case 2: {
            std::cout << "   Two\n";
            break;
        }
        default: {
            std::cout << "   Other\n";
            break;
        }
    }
}

// MISRA Rule 6-6-1, 6-6-2: Any label referenced by a goto shall be in the same block
// or enclosing block

// ❌ BAD: goto usage (generally prohibited)
void bad_goto() {
    std::cout << "\n❌ BAD: goto usage\n";
    std::cout << "   MISRA strongly discourages or prohibits goto\n";
    std::cout << "   Use structured programming instead\n";
}

// ✅ GOOD: Structured programming
void good_structured() {
    std::cout << "\n✅ GOOD: Structured programming\n";
    
    bool error_occurred = false;
    
    // Instead of goto for error handling, use early return
    if (error_occurred) {
        std::cout << "   Error handled with early return\n";
        return;
    }
    
    std::cout << "   Normal execution path\n";
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "MISRA CATEGORY 3: STATEMENTS\n";
    std::cout << std::string(70, '=') << "\n";
    
    bad_if_statements();
    good_if_statements();
    bad_switch(1);
    good_switch(1);
    bad_goto();
    good_structured();
}

} // namespace statements

// ===================================================================
// RULE CATEGORY 4: FUNCTIONS
// ===================================================================

namespace functions {

// MISRA Rule 8-4-2: All exit paths from a function with non-void return type shall have
// an explicit return statement

// ❌ BAD: Missing return in some paths
// int32_t bad_function(int32_t value) {
//     if (value > 0) {
//         return value;
//     }
//     // MISRA violation! Missing return for value <= 0
// }

// ✅ GOOD: All paths have explicit return
int32_t good_function(int32_t value) {
    if (value > 0) {
        return value;
    } else {
        return 0;
    }
}

// MISRA Rule 8-4-4: A function with no parameters shall be declared with parameter (void)

// ❌ BAD: Empty parameter list in C++
// void bad_no_params();  // In C++, this is OK, but MISRA prefers explicit void

// ✅ GOOD: Explicit void for clarity (C-style, but clearer)
void good_no_params(void) {
    std::cout << "   Function with explicit void parameter list\n";
}

// MISRA Rule 7-1-1: A variable that is not modified shall be const qualified

// ❌ BAD: Non-const variable that isn't modified
void bad_const_usage() {
    std::cout << "\n❌ BAD: Non-const variable that isn't modified\n";
    
    int32_t value = 100;  // MISRA violation! Should be const
    std::cout << "   Value: " << value << "\n";
}

// ✅ GOOD: const-qualified for immutable data
void good_const_usage() {
    std::cout << "\n✅ GOOD: const-qualified for immutable data\n";
    
    const int32_t value = 100;  // OK: const qualifier
    std::cout << "   Value: " << value << "\n";
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "MISRA CATEGORY 4: FUNCTIONS\n";
    std::cout << std::string(70, '=') << "\n";
    
    std::cout << "\n✅ GOOD: All paths return a value\n";
    std::cout << "   good_function(5) = " << good_function(5) << "\n";
    std::cout << "   good_function(-5) = " << good_function(-5) << "\n";
    
    good_no_params();
    
    bad_const_usage();
    good_const_usage();
}

} // namespace functions

// ===================================================================
// RULE CATEGORY 5: ARRAYS AND POINTERS
// ===================================================================

namespace arrays_and_pointers {

// MISRA Rule 5-0-15: Array indexing shall be the only form of pointer arithmetic

// ❌ BAD: Pointer arithmetic
void bad_pointer_arithmetic() {
    std::cout << "\n❌ BAD: Pointer arithmetic\n";
    
    int32_t arr[5] = {1, 2, 3, 4, 5};
    int32_t* ptr = arr;
    
    // ptr++;           // MISRA violation!
    // int32_t val = *(ptr + 2);  // MISRA violation!
    
    std::cout << "   Pointer arithmetic is error-prone\n";
}

// ✅ GOOD: Array indexing
void good_array_indexing() {
    std::cout << "\n✅ GOOD: Array indexing\n";
    
    std::array<int32_t, 5> arr = {1, 2, 3, 4, 5};
    
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << "   arr[" << i << "] = " << arr[i] << "\n";
    }
}

// MISRA Rule 5-0-16: The result of pointer subtraction shall not be used

// ❌ BAD: Pointer subtraction
void bad_pointer_subtraction() {
    std::cout << "\n❌ BAD: Pointer subtraction\n";
    
    int32_t arr[5] = {1, 2, 3, 4, 5};
    int32_t* p1 = &arr[0];
    int32_t* p2 = &arr[3];
    
    // ptrdiff_t diff = p2 - p1;  // MISRA violation!
    
    std::cout << "   Pointer subtraction should be avoided\n";
}

// ✅ GOOD: Use indices instead
void good_index_difference() {
    std::cout << "\n✅ GOOD: Use indices instead\n";
    
    std::array<int32_t, 5> arr = {1, 2, 3, 4, 5};
    size_t idx1 = 0;
    size_t idx2 = 3;
    
    size_t diff = idx2 - idx1;
    std::cout << "   Index difference: " << diff << "\n";
}

// MISRA Rule: Use std::array instead of C-style arrays

// ❌ BAD: C-style arrays
void bad_c_arrays() {
    std::cout << "\n❌ BAD: C-style arrays\n";
    
    int32_t arr[10];  // MISRA prefers std::array
    arr[0] = 1;
    
    std::cout << "   C-style arrays lack bounds checking\n";
}

// ✅ GOOD: std::array with bounds checking
void good_std_array() {
    std::cout << "\n✅ GOOD: std::array with bounds checking\n";
    
    std::array<int32_t, 10> arr{};
    arr.at(0) = 1;  // Bounds-checked access
    
    std::cout << "   arr.at(0) = " << arr.at(0) << "\n";
    std::cout << "   std::array provides bounds checking with at()\n";
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "MISRA CATEGORY 5: ARRAYS AND POINTERS\n";
    std::cout << std::string(70, '=') << "\n";
    
    bad_pointer_arithmetic();
    good_array_indexing();
    bad_pointer_subtraction();
    good_index_difference();
    bad_c_arrays();
    good_std_array();
}

} // namespace arrays_and_pointers

// ===================================================================
// RULE CATEGORY 6: CLASSES
// ===================================================================

namespace classes {

// MISRA Rule 12-8-1: A copy constructor shall only initialize its base classes and the
// non-static members of the class of which it is a member

// ✅ GOOD: Proper class design
class Resource {
private:
    int32_t* data_;
    size_t size_;
    
public:
    // Constructor
    explicit Resource(size_t size) 
        : data_(new int32_t[size]), size_(size) {
        std::cout << "   [Resource] Constructor: allocated " << size_ << " ints\n";
    }
    
    // Destructor
    ~Resource() {
        delete[] data_;
        std::cout << "   [Resource] Destructor: freed memory\n";
    }
    
    // MISRA Rule: Define all special members or none (Rule of Five)
    Resource(const Resource& other) 
        : data_(new int32_t[other.size_]), size_(other.size_) {
        std::copy_n(other.data_, size_, data_);
        std::cout << "   [Resource] Copy constructor\n";
    }
    
    Resource& operator=(const Resource& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = new int32_t[size_];
            std::copy_n(other.data_, size_, data_);
        }
        std::cout << "   [Resource] Copy assignment\n";
        return *this;
    }
    
    Resource(Resource&& other) noexcept 
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "   [Resource] Move constructor\n";
    }
    
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        std::cout << "   [Resource] Move assignment\n";
        return *this;
    }
    
    size_t size() const { return size_; }
};

// MISRA Rule: Virtual destructor for base classes

// ❌ BAD: Base class without virtual destructor
class BadBase {
public:
    ~BadBase() {  // MISRA violation! Should be virtual
        std::cout << "   [BadBase] Destructor\n";
    }
};

// ✅ GOOD: Virtual destructor for polymorphic base
class GoodBase {
public:
    virtual ~GoodBase() {
        std::cout << "   [GoodBase] Virtual destructor\n";
    }
    
    virtual void process() = 0;
};

class Derived : public GoodBase {
public:
    void process() override {
        std::cout << "   [Derived] Processing\n";
    }
    
    ~Derived() override {
        std::cout << "   [Derived] Destructor\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "MISRA CATEGORY 6: CLASSES\n";
    std::cout << std::string(70, '=') << "\n";
    
    std::cout << "\n✅ GOOD: Rule of Five implementation\n";
    {
        Resource r1(5);
        Resource r2 = r1;  // Copy
        Resource r3 = std::move(r1);  // Move
    }
    
    std::cout << "\n✅ GOOD: Virtual destructor for polymorphism\n";
    {
        std::unique_ptr<GoodBase> ptr = std::make_unique<Derived>();
        ptr->process();
    }
}

} // namespace classes

// ===================================================================
// RULE CATEGORY 7: EXCEPTION HANDLING
// ===================================================================

namespace exception_handling {

// MISRA Rule 15-1-1: Only throw exceptions for exceptional conditions
// MISRA Rule 15-3-1: Exceptions shall be used only for error handling

// ❌ BAD: Using exceptions for control flow
void bad_exception_usage() {
    std::cout << "\n❌ BAD: Using exceptions for control flow\n";
    
    // try {
    //     // Don't use exceptions for normal flow!
    //     throw 42;
    // } catch (int value) {
    //     // Control flow via exception (BAD!)
    // }
    
    std::cout << "   Exceptions should only be used for errors\n";
}

// ✅ GOOD: Exceptions for error handling only
void good_exception_usage() {
    std::cout << "\n✅ GOOD: Exceptions for error handling\n";
    
    try {
        // Simulate error condition
        throw std::runtime_error("Genuine error occurred");
    } catch (const std::exception& e) {
        std::cout << "   Caught exception: " << e.what() << "\n";
    }
}

// MISRA Note: Many safety-critical systems disable exceptions entirely
void no_exceptions_approach() {
    std::cout << "\n✅ ALTERNATIVE: Error codes (no exceptions)\n";
    std::cout << "   Many MISRA-compliant projects compile with -fno-exceptions\n";
    std::cout << "   Use error codes, std::optional, or std::expected instead\n";
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "MISRA CATEGORY 7: EXCEPTION HANDLING\n";
    std::cout << std::string(70, '=') << "\n";
    
    bad_exception_usage();
    good_exception_usage();
    no_exceptions_approach();
}

} // namespace exception_handling

// ===================================================================
// MAIN - Demonstrate All MISRA C++ Categories
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                 MISRA C++ CODING GUIDELINES                      ║\n";
    std::cout << "║           Safety-Critical Software Development                   ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    try {
        types_and_declarations::demonstrate();
        expressions_and_operators::demonstrate();
        statements::demonstrate();
        functions::demonstrate();
        arrays_and_pointers::demonstrate();
        classes::demonstrate();
        exception_handling::demonstrate();
        
        // Summary
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "SUMMARY: KEY MISRA C++ PRINCIPLES\n";
        std::cout << std::string(70, '=') << "\n\n";
        
        std::cout << "🎯 MISRA C++ OBJECTIVES:\n";
        std::cout << "   • Avoid undefined behavior\n";
        std::cout << "   • Avoid implementation-defined behavior\n";
        std::cout << "   • Maximize portability\n";
        std::cout << "   • Enhance code clarity and maintainability\n";
        std::cout << "   • Enable thorough code review and testing\n\n";
        
        std::cout << "📋 KEY PRACTICES:\n";
        std::cout << "   1. Use fixed-width types (int32_t, uint32_t)\n";
        std::cout << "   2. Always use braces for if/while/for\n";
        std::cout << "   3. No pointer arithmetic (use array indexing)\n";
        std::cout << "   4. Explicit conversions (avoid implicit casts)\n";
        std::cout << "   5. const-qualify immutable data\n";
        std::cout << "   6. Virtual destructors for polymorphic classes\n";
        std::cout << "   7. Rule of Five for resource-managing classes\n";
        std::cout << "   8. Prefer std::array over C-style arrays\n\n";
        
        std::cout << "🏭 APPLICATION DOMAINS:\n";
        std::cout << "   • Automotive (ISO 26262)\n";
        std::cout << "   • Aerospace (DO-178C)\n";
        std::cout << "   • Medical devices (IEC 62304)\n";
        std::cout << "   • Industrial control systems\n";
        std::cout << "   • Railway (EN 50128)\n\n";
        
        std::cout << "⚠️  NOTE:\n";
        std::cout << "   Many MISRA-compliant projects also:\n";
        std::cout << "   • Disable exceptions (-fno-exceptions)\n";
        std::cout << "   • Disable RTTI (-fno-rtti)\n";
        std::cout << "   • Limit or prohibit dynamic memory allocation\n";
        std::cout << "   • Use static analysis tools (PC-lint, Coverity, etc.)\n\n";
        
        std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║            ALL MISRA C++ CATEGORIES DEMONSTRATED!                ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
