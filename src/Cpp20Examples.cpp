#include <iostream>
#include <string>
#include <vector>
#include <span>
#include <ranges>
#include <algorithm>
#include <concepts>
#include <compare>
#include <numbers>
#include <bit>
#include <bitset>
#include <array>
#include <set>
#include <numeric>

// ===================================================================
// C++20 COMPREHENSIVE EXAMPLES
// ===================================================================

// ===================================================================
// 1. CONCEPTS
// ===================================================================
template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept SignedIntegral = Integral<T> && std::is_signed_v<T>;

template <Integral T>
T add(T a, T b) {
    return a + b;
}

template <SignedIntegral T>
T negate(T value) {
    return -value;
}

void example_concepts() {
    std::cout << "\n=== 1. CONCEPTS ===" << std::endl;
    
    std::cout << "add(10, 20) = " << add(10, 20) << std::endl;
    std::cout << "add(5U, 3U) = " << add(5U, 3U) << std::endl;
    std::cout << "negate(42) = " << negate(42) << std::endl;
    
    // add(3.14, 2.5);  // Error: double doesn't satisfy Integral
}

// ===================================================================
// 2. THREE-WAY COMPARISON (SPACESHIP OPERATOR)
// ===================================================================
struct Point {
    int x, y;
    auto operator<=>(const Point&) const = default;
};

void example_three_way_comparison() {
    std::cout << "\n=== 2. THREE-WAY COMPARISON ===" << std::endl;
    
    Point p1{1, 2}, p2{1, 3}, p3{1, 2};
    
    std::cout << "p1 == p3: " << std::boolalpha << (p1 == p3) << std::endl;
    std::cout << "p1 != p2: " << (p1 != p2) << std::endl;
    std::cout << "p1 < p2: " << (p1 < p2) << std::endl;
    
    int a = 10, b = 20;
    auto result = a <=> b;
    if (result < 0) {
        std::cout << "a < b" << std::endl;
    }
}

// ===================================================================
// 3. DESIGNATED INITIALIZERS
// ===================================================================
struct Config {
    int width;
    int height;
    std::string title;
};

void example_designated_initializers() {
    std::cout << "\n=== 3. DESIGNATED INITIALIZERS ===" << std::endl;
    
    Config cfg{
        .width = 1920,
        .height = 1080,
        .title = "My Window"
    };
    
    std::cout << "Config: " << cfg.width << "x" << cfg.height 
              << " - " << cfg.title << std::endl;
}

// ===================================================================
// 4. TEMPLATE SYNTAX FOR LAMBDAS
// ===================================================================
void example_template_lambdas() {
    std::cout << "\n=== 4. TEMPLATE SYNTAX FOR LAMBDAS ===" << std::endl;
    
    auto generic_add = []<typename T>(T a, T b) {
        return a + b;
    };
    
    std::cout << "generic_add(10, 20) = " << generic_add(10, 20) << std::endl;
    std::cout << "generic_add(1.5, 2.5) = " << generic_add(1.5, 2.5) << std::endl;
    
    auto print_type = []<typename T>(T value) {
        if constexpr (std::is_integral_v<T>) {
            std::cout << "Integer: " << value << std::endl;
        } else {
            std::cout << "Non-integer: " << value << std::endl;
        }
    };
    
    print_type(42);
    print_type(3.14);
}

// ===================================================================
// 5. RANGE-BASED FOR LOOP WITH INITIALIZER
// ===================================================================
void example_range_for_initializer() {
    std::cout << "\n=== 5. RANGE-BASED FOR WITH INITIALIZER ===" << std::endl;
    
    for (std::vector<int> vec = {1, 2, 3, 4, 5}; auto& elem : vec) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 6. [[LIKELY]] AND [[UNLIKELY]] ATTRIBUTES
// ===================================================================
int predict_branch(int value) {
    if (value > 0) [[likely]] {
        return value * 2;
    } else [[unlikely]] {
        return value * 10;
    }
}

void example_likely_unlikely() {
    std::cout << "\n=== 6. [[LIKELY]] AND [[UNLIKELY]] ===" << std::endl;
    
    std::cout << "predict_branch(5) = " << predict_branch(5) << std::endl;
    std::cout << "predict_branch(-2) = " << predict_branch(-2) << std::endl;
}

// ===================================================================
// 7. CONSTEXPR VIRTUAL FUNCTIONS
// ===================================================================
struct Base {
    virtual constexpr int get_value() const {
        return 10;
    }
};

struct Derived : Base {
    constexpr int get_value() const override {
        return 20;
    }
};

void example_constexpr_virtual() {
    std::cout << "\n=== 7. CONSTEXPR VIRTUAL FUNCTIONS ===" << std::endl;
    
    constexpr Derived d;
    constexpr int value = d.get_value();
    
    std::cout << "Constexpr virtual function result: " << value << std::endl;
}

// ===================================================================
// 8. EXPLICIT(BOOL)
// ===================================================================
struct MyInt {
    int value;
    explicit(sizeof(int) > 4) MyInt(int v) : value(v) {}
};

void example_explicit_bool() {
    std::cout << "\n=== 8. EXPLICIT(BOOL) ===" << std::endl;
    
    MyInt mi{42};  // Always requires explicit construction based on condition
    std::cout << "MyInt created with value: " << mi.value << std::endl;
}

// ===================================================================
// 9. IMMEDIATE FUNCTIONS (CONSTEVAL)
// ===================================================================
consteval int square(int n) {
    return n * n;
}

void example_immediate_functions() {
    std::cout << "\n=== 9. IMMEDIATE FUNCTIONS ===" << std::endl;
    
    constexpr int result = square(5);  // Must be evaluated at compile time
    std::cout << "Consteval square(5) = " << result << std::endl;
}

// ===================================================================
// 10. USING ENUM
// ===================================================================
enum class Color { Red, Green, Blue };

void example_using_enum() {
    std::cout << "\n=== 10. USING ENUM ===" << std::endl;
    
    using enum Color;
    Color c = Red;  // No need for Color::Red
    
    switch (c) {
        using enum Color;  // Can use in switch scope
        case Red:
            std::cout << "Color is Red" << std::endl;
            break;
        case Green:
            std::cout << "Color is Green" << std::endl;
            break;
        case Blue:
            std::cout << "Color is Blue" << std::endl;
            break;
    }
}

// ===================================================================
// 11. CHAR8_T
// ===================================================================
void example_char8_t() {
    std::cout << "\n=== 11. CHAR8_T ===" << std::endl;
    
    char8_t utf8_char = u8'A';
    const char8_t* utf8_str = u8"Hello UTF-8";
    
    std::cout << "UTF-8 character type created" << std::endl;
    std::cout << "UTF-8 string type created" << std::endl;
}

// ===================================================================
// 12. CONSTINIT
// ===================================================================
constinit int global_value = 42;

void example_constinit() {
    std::cout << "\n=== 12. CONSTINIT ===" << std::endl;
    
    std::cout << "Constinit global value: " << global_value << std::endl;
    global_value = 100;  // Can be modified at runtime
    std::cout << "Modified value: " << global_value << std::endl;
}

// ===================================================================
// 13. STD::SPAN
// ===================================================================
void process_span(std::span<int> s) {
    for (auto& elem : s) {
        elem *= 2;
    }
}

void example_span() {
    std::cout << "\n=== 13. STD::SPAN ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::span<int> s{vec};
    
    std::cout << "Before: ";
    for (auto v : vec) std::cout << v << " ";
    std::cout << std::endl;
    
    process_span(s);
    
    std::cout << "After: ";
    for (auto v : vec) std::cout << v << " ";
    std::cout << std::endl;
}

// ===================================================================
// 14. RANGES
// ===================================================================
void example_ranges() {
    std::cout << "\n=== 14. RANGES ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    auto even = vec | std::views::filter([](int n) { return n % 2 == 0; });
    auto squared = even | std::views::transform([](int n) { return n * n; });
    
    std::cout << "Even numbers squared: ";
    for (auto v : squared) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 15. BIT OPERATIONS
// ===================================================================
void example_bit_operations() {
    std::cout << "\n=== 15. BIT OPERATIONS ===" << std::endl;
    
    unsigned int value = 0b10110100;
    
    std::cout << "popcount(0b10110100) = " << std::popcount(value) << std::endl;
    std::cout << "has_single_bit(8) = " << std::boolalpha << std::has_single_bit(8u) << std::endl;
    std::cout << "bit_width(7) = " << std::bit_width(7u) << std::endl;
    std::cout << "rotl(0b10110100, 2) = " << std::rotl(value, 2) << std::endl;
}

// ===================================================================
// 16. MATH CONSTANTS
// ===================================================================
void example_math_constants() {
    std::cout << "\n=== 16. MATH CONSTANTS ===" << std::endl;
    
    std::cout << "pi = " << std::numbers::pi << std::endl;
    std::cout << "e = " << std::numbers::e << std::endl;
    std::cout << "sqrt2 = " << std::numbers::sqrt2 << std::endl;
    std::cout << "ln2 = " << std::numbers::ln2 << std::endl;
}

// ===================================================================
// 17. STD::IS_CONSTANT_EVALUATED
// ===================================================================
constexpr int compute_value() {
    if (std::is_constant_evaluated()) {
        return 42;  // Compile-time
    } else {
        return 100; // Runtime
    }
}

void example_is_constant_evaluated() {
    std::cout << "\n=== 17. STD::IS_CONSTANT_EVALUATED ===" << std::endl;
    
    constexpr int compile_time = compute_value();
    int runtime = compute_value();
    
    std::cout << "Compile-time: " << compile_time << std::endl;
    std::cout << "Runtime: " << runtime << std::endl;
}

// ===================================================================
// 18. STARTS_WITH / ENDS_WITH
// ===================================================================
void example_starts_ends_with() {
    std::cout << "\n=== 18. STARTS_WITH / ENDS_WITH ===" << std::endl;
    
    std::string str = "Hello, World!";
    
    std::cout << "starts_with('Hello'): " << std::boolalpha 
              << str.starts_with("Hello") << std::endl;
    std::cout << "ends_with('World!'): " << str.ends_with("World!") << std::endl;
}

// ===================================================================
// 19. ASSOCIATIVE CONTAINER .CONTAINS()
// ===================================================================
void example_contains() {
    std::cout << "\n=== 19. ASSOCIATIVE CONTAINER .CONTAINS() ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::set<int> s{vec.begin(), vec.end()};
    
    std::cout << "Set contains 3: " << std::boolalpha << s.contains(3) << std::endl;
    std::cout << "Set contains 10: " << s.contains(10) << std::endl;
}

// ===================================================================
// 20. STD::MIDPOINT
// ===================================================================
void example_midpoint() {
    std::cout << "\n=== 20. STD::MIDPOINT ===" << std::endl;
    
    std::cout << "midpoint(10, 20) = " << std::midpoint(10, 20) << std::endl;
    std::cout << "midpoint(1.0, 5.0) = " << std::midpoint(1.0, 5.0) << std::endl;
}

// ===================================================================
// 21. STD::TO_ARRAY
// ===================================================================
void example_to_array() {
    std::cout << "\n=== 21. STD::TO_ARRAY ===" << std::endl;
    
    auto arr = std::to_array({1, 2, 3, 4, 5});
    
    std::cout << "Array from initializer list: ";
    for (auto v : arr) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 22. LAMBDA CAPTURE OF PARAMETER PACK
// ===================================================================
template <typename... Args>
auto make_lambda(Args... args) {
    return [...args = std::move(args)] {
        ((std::cout << args << " "), ...);
        std::cout << std::endl;
    };
}

void example_lambda_capture_pack() {
    std::cout << "\n=== 22. LAMBDA CAPTURE OF PARAMETER PACK ===" << std::endl;
    
    auto lambda = make_lambda(1, 2, 3, "hello", 4.5);
    std::cout << "Lambda with captured pack: ";
    lambda();
}

// ===================================================================
// 23. CLASS TYPES IN NON-TYPE TEMPLATE PARAMETERS
// ===================================================================
struct CompileTimeValue {
    int value;
    constexpr CompileTimeValue(int v) : value(v) {}
};

template <CompileTimeValue val>
void print_compile_time() {
    std::cout << "Compile-time value: " << val.value << std::endl;
}

void example_class_non_type_template() {
    std::cout << "\n=== 23. CLASS TYPES IN NON-TYPE TEMPLATE ===" << std::endl;
    
    print_compile_time<CompileTimeValue{42}>();
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    C++20 FEATURES COMPREHENSIVE EXAMPLES" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    example_concepts();
    example_three_way_comparison();
    example_designated_initializers();
    example_template_lambdas();
    example_range_for_initializer();
    example_likely_unlikely();
    example_constexpr_virtual();
    example_explicit_bool();
    example_immediate_functions();
    example_using_enum();
    example_char8_t();
    example_constinit();
    example_span();
    example_ranges();
    example_bit_operations();
    example_math_constants();
    example_is_constant_evaluated();
    example_starts_ends_with();
    example_contains();
    example_midpoint();
    example_to_array();
    example_lambda_capture_pack();
    example_class_non_type_template();
    
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    ALL C++20 EXAMPLES COMPLETED" << std::endl;
    std::cout << "===============================================\n" << std::endl;
    
    return 0;
}
