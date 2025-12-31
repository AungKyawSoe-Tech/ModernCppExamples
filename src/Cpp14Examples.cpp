#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <utility>

// ===================================================================
// C++14 LANGUAGE FEATURES
// ===================================================================

// ===================================================================
// 1. BINARY LITERALS
// ===================================================================
void example_binary_literals() {
    std::cout << "\n=== 1. BINARY LITERALS ===" << std::endl;
    
    int a = 0b110;              // Binary literal
    int b = 0b1111'1111;        // Binary with digit separator
    int c = 0b1010'0101'1100;   // Larger binary number
    
    std::cout << "0b110 = " << a << std::endl;
    std::cout << "0b1111'1111 = " << b << std::endl;
    std::cout << "0b1010'0101'1100 = " << c << std::endl;
}

// ===================================================================
// 2. GENERIC LAMBDA EXPRESSIONS
// ===================================================================
void example_generic_lambdas() {
    std::cout << "\n=== 2. GENERIC LAMBDA EXPRESSIONS ===" << std::endl;
    
    auto identity = [](auto x) { return x; };
    
    int three = identity(3);
    std::string foo = identity(std::string("foo"));
    double pi = identity(3.14);
    
    std::cout << "Identity(3) = " << three << std::endl;
    std::cout << "Identity(\"foo\") = " << foo << std::endl;
    std::cout << "Identity(3.14) = " << pi << std::endl;
    
    // Generic lambda with multiple parameters
    auto add = [](auto a, auto b) { return a + b; };
    std::cout << "add(10, 20) = " << add(10, 20) << std::endl;
    std::cout << "add(1.5, 2.5) = " << add(1.5, 2.5) << std::endl;
}

// ===================================================================
// 3. LAMBDA CAPTURE INITIALIZERS
// ===================================================================
void example_lambda_capture_initializers() {
    std::cout << "\n=== 3. LAMBDA CAPTURE INITIALIZERS ===" << std::endl;
    
    // Initialize capture with expression
    int factory_value = 2;
    auto f = [x = factory_value * 10] { return x; };
    std::cout << "Lambda with initialized capture: " << f() << std::endl;
    
    // Generator with mutable capture
    auto generator = [x = 0]() mutable {
        return x++;
    };
    std::cout << "Generator call 1: " << generator() << std::endl;
    std::cout << "Generator call 2: " << generator() << std::endl;
    std::cout << "Generator call 3: " << generator() << std::endl;
    
    // Move-only types in lambda
    auto ptr = std::make_unique<int>(42);
    auto task = [p = std::move(ptr)] { 
        std::cout << "Value in lambda: " << *p << std::endl;
    };
    task();
    std::cout << "Original ptr is " << (ptr ? "valid" : "null") << std::endl;
}

// ===================================================================
// 4. RETURN TYPE DEDUCTION
// ===================================================================
auto multiply(int a, int b) {
    return a * b;  // Return type deduced as int
}

template <typename T>
auto add_values(T a, T b) {
    return a + b;  // Return type deduced from expression
}

void example_return_type_deduction() {
    std::cout << "\n=== 4. RETURN TYPE DEDUCTION ===" << std::endl;
    
    std::cout << "multiply(5, 6) = " << multiply(5, 6) << std::endl;
    std::cout << "add_values(10, 20) = " << add_values(10, 20) << std::endl;
    std::cout << "add_values(1.5, 2.5) = " << add_values(1.5, 2.5) << std::endl;
    
    // Lambda with auto return type
    auto square = [](auto x) { return x * x; };
    std::cout << "square(7) = " << square(7) << std::endl;
}

// ===================================================================
// 5. DECLTYPE(AUTO)
// ===================================================================
template <typename Container>
decltype(auto) get_first(Container& c) {
    return c[0];  // Returns reference if c[0] is a reference
}

void example_decltype_auto() {
    std::cout << "\n=== 5. DECLTYPE(AUTO) ===" << std::endl;
    
    const int x = 0;
    auto x1 = x;              // int
    decltype(auto) x2 = x;    // const int
    
    std::cout << "auto strips const, decltype(auto) preserves it" << std::endl;
    
    std::vector<int> vec = {1, 2, 3};
    decltype(auto) first = get_first(vec);
    first = 100;  // Modifies vec[0]
    std::cout << "Modified first element: " << vec[0] << std::endl;
}

// ===================================================================
// 6. RELAXED CONSTEXPR FUNCTIONS
// ===================================================================
constexpr int factorial(int n) {
    // C++14 allows multiple statements in constexpr
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

void example_relaxed_constexpr() {
    std::cout << "\n=== 6. RELAXED CONSTEXPR FUNCTIONS ===" << std::endl;
    
    constexpr int fact5 = factorial(5);
    constexpr int fib10 = fibonacci(10);
    
    std::cout << "factorial(5) = " << fact5 << std::endl;
    std::cout << "fibonacci(10) = " << fib10 << std::endl;
}

// ===================================================================
// 7. VARIABLE TEMPLATES
// ===================================================================
template <typename T>
constexpr T pi = T(3.1415926535897932385);

template <typename T>
constexpr T e = T(2.7182818284590452354);

void example_variable_templates() {
    std::cout << "\n=== 7. VARIABLE TEMPLATES ===" << std::endl;
    
    std::cout << "pi<float> = " << pi<float> << std::endl;
    std::cout << "pi<double> = " << pi<double> << std::endl;
    std::cout << "e<float> = " << e<float> << std::endl;
    std::cout << "e<double> = " << e<double> << std::endl;
}

// ===================================================================
// 8. [[DEPRECATED]] ATTRIBUTE
// ===================================================================
[[deprecated("Use new_function() instead")]]
void old_function() {
    std::cout << "This is deprecated!" << std::endl;
}

void new_function() {
    std::cout << "This is the new function!" << std::endl;
}

void example_deprecated_attribute() {
    std::cout << "\n=== 8. [[DEPRECATED]] ATTRIBUTE ===" << std::endl;
    
    // old_function();  // Would generate compiler warning
    new_function();
    std::cout << "Using new function instead of deprecated one" << std::endl;
}

// ===================================================================
// C++14 LIBRARY FEATURES
// ===================================================================

// ===================================================================
// 9. USER-DEFINED LITERALS FOR STANDARD LIBRARY
// ===================================================================
void example_user_defined_literals() {
    std::cout << "\n=== 9. USER-DEFINED LITERALS ===" << std::endl;
    
    using namespace std::string_literals;
    
    auto str = "Hello"s;  // std::string
    
    std::cout << "String literal type: std::string" << std::endl;
    std::cout << "User-defined string literals work in C++14" << std::endl;
}

// ===================================================================
// 10. STD::MAKE_UNIQUE
// ===================================================================
void example_make_unique() {
    std::cout << "\n=== 10. STD::MAKE_UNIQUE ===" << std::endl;
    
    auto ptr1 = std::make_unique<int>(42);
    auto ptr2 = std::make_unique<std::string>("Hello");
    auto ptr3 = std::make_unique<std::vector<int>>(5, 10);
    
    std::cout << "*ptr1 = " << *ptr1 << std::endl;
    std::cout << "*ptr2 = " << *ptr2 << std::endl;
    std::cout << "ptr3 size = " << ptr3->size() << std::endl;
    
    // Array version
    auto arr = std::make_unique<int[]>(5);
    for (int i = 0; i < 5; ++i) {
        arr[i] = i * 10;
    }
    std::cout << "Array elements: ";
    for (int i = 0; i < 5; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 11. COMPILE-TIME INTEGER SEQUENCES
// ===================================================================
template <typename T, T... Ints>
void print_sequence(std::integer_sequence<T, Ints...>) {
    std::cout << "Sequence: ";
    int dummy[] = {0, (std::cout << Ints << " ", 0)...};
    (void)dummy;
    std::cout << std::endl;
}

void example_integer_sequences() {
    std::cout << "\n=== 11. COMPILE-TIME INTEGER SEQUENCES ===" << std::endl;
    
    using Seq = std::integer_sequence<int, 0, 1, 2, 3, 4>;
    using IdxSeq = std::make_index_sequence<5>;
    
    std::cout << "Integer sequence example (needs C++17 fold for printing)" << std::endl;
    std::cout << "std::make_index_sequence<5> creates indices 0,1,2,3,4" << std::endl;
}

// ===================================================================
// 12. DIGIT SEPARATORS
// ===================================================================
void example_digit_separators() {
    std::cout << "\n=== 12. DIGIT SEPARATORS ===" << std::endl;
    
    int decimal = 1'000'000;
    int hex = 0xDEAD'BEEF;
    int binary = 0b1010'1010'1010;
    double floating = 3.141'592'653'589;
    
    std::cout << "1'000'000 = " << decimal << std::endl;
    std::cout << "0xDEAD'BEEF = " << hex << std::endl;
    std::cout << "0b1010'1010'1010 = " << binary << std::endl;
    std::cout << "3.141'592'653'589 = " << floating << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    C++14 FEATURES COMPREHENSIVE EXAMPLES" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    // Language Features
    example_binary_literals();
    example_generic_lambdas();
    example_lambda_capture_initializers();
    example_return_type_deduction();
    example_decltype_auto();
    example_relaxed_constexpr();
    example_variable_templates();
    example_deprecated_attribute();
    
    // Library Features
    example_user_defined_literals();
    example_make_unique();
    example_integer_sequences();
    example_digit_separators();
    
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    ALL C++14 EXAMPLES COMPLETED" << std::endl;
    std::cout << "===============================================\n" << std::endl;
    
    return 0;
}
