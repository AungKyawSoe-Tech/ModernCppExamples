#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <optional>
#include <variant>
#include <any>
#include <string_view>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <memory>
#include <functional>

// ===================================================================
// C++17 COMPREHENSIVE EXAMPLES
// ===================================================================

// ===================================================================
// 1. TEMPLATE ARGUMENT DEDUCTION FOR CLASS TEMPLATES
// ===================================================================
void example_template_argument_deduction() {
    std::cout << "\n=== 1. TEMPLATE ARGUMENT DEDUCTION ===" << std::endl;
    
    std::pair p1{1, 2.0};          // std::pair<int, double>
    std::tuple t1{1, "hello", 3.14}; // std::tuple<int, const char*, double>
    std::vector v1{1, 2, 3, 4};    // std::vector<int>
    
    std::cout << "std::pair deduced: (" << p1.first << ", " << p1.second << ")" << std::endl;
    std::cout << "std::vector deduced with " << v1.size() << " elements" << std::endl;
}

// ===================================================================
// 2. STRUCTURED BINDINGS
// ===================================================================
void example_structured_bindings() {
    std::cout << "\n=== 2. STRUCTURED BINDINGS ===" << std::endl;
    
    std::pair<int, std::string> pair{1, "hello"};
    auto [num, str] = pair;
    std::cout << "Pair: " << num << ", " << str << std::endl;
    
    std::tuple<int, double, std::string> tup{42, 3.14, "world"};
    auto [i, d, s] = tup;
    std::cout << "Tuple: " << i << ", " << d << ", " << s << std::endl;
    
    std::map<int, std::string> m{{1, "one"}, {2, "two"}};
    for (const auto& [key, value] : m) {
        std::cout << "Map: " << key << " -> " << value << std::endl;
    }
}

// ===================================================================
// 3. FOLDING EXPRESSIONS
// ===================================================================
template <typename... Args>
auto sum_fold(Args... args) {
    return (... + args);  // Unary left fold
}

template <typename... Args>
bool all_true(Args... args) {
    return (... && args);  // Unary left fold
}

template <typename... Args>
void print_all(Args... args) {
    ((std::cout << args << " "), ...);  // Binary left fold
    std::cout << std::endl;
}

void example_folding_expressions() {
    std::cout << "\n=== 3. FOLDING EXPRESSIONS ===" << std::endl;
    
    std::cout << "sum_fold(1, 2, 3, 4, 5) = " << sum_fold(1, 2, 3, 4, 5) << std::endl;
    std::cout << "all_true(true, true, true) = " << std::boolalpha << all_true(true, true, true) << std::endl;
    std::cout << "all_true(true, false, true) = " << all_true(true, false, true) << std::endl;
    std::cout << "print_all: ";
    print_all(1, "hello", 3.14, "world");
}

// ===================================================================
// 4. CONSTEXPR IF
// ===================================================================
template <typename T>
auto get_value(T t) {
    if constexpr (std::is_pointer_v<T>) {
        return *t;  // Dereference pointer
    } else {
        return t;   // Return value directly
    }
}

void example_constexpr_if() {
    std::cout << "\n=== 4. CONSTEXPR IF ===" << std::endl;
    
    int x = 42;
    int* ptr = &x;
    
    std::cout << "get_value(10) = " << get_value(10) << std::endl;
    std::cout << "get_value(ptr) = " << get_value(ptr) << std::endl;
}

// ===================================================================
// 5. CONSTEXPR LAMBDA
// ===================================================================
void example_constexpr_lambda() {
    std::cout << "\n=== 5. CONSTEXPR LAMBDA ===" << std::endl;
    
    constexpr auto square = [](int n) { return n * n; };
    constexpr int result = square(5);
    
    std::cout << "Compile-time square(5) = " << result << std::endl;
    static_assert(square(4) == 16, "Square function test");
}

// ===================================================================
// 6. INLINE VARIABLES
// ===================================================================
inline int global_counter = 0;

struct Config {
    inline static const std::string app_name = "MyApp";
    inline static const int version = 1;
};

void example_inline_variables() {
    std::cout << "\n=== 6. INLINE VARIABLES ===" << std::endl;
    
    global_counter++;
    std::cout << "Global counter: " << global_counter << std::endl;
    std::cout << "App name: " << Config::app_name << std::endl;
    std::cout << "Version: " << Config::version << std::endl;
}

// ===================================================================
// 7. NESTED NAMESPACES
// ===================================================================
namespace A::B::C {
    void nested_function() {
        std::cout << "Inside A::B::C namespace" << std::endl;
    }
}

void example_nested_namespaces() {
    std::cout << "\n=== 7. NESTED NAMESPACES ===" << std::endl;
    A::B::C::nested_function();
}

// ===================================================================
// 8. SELECTION STATEMENTS WITH INITIALIZER
// ===================================================================
void example_selection_with_initializer() {
    std::cout << "\n=== 8. SELECTION WITH INITIALIZER ===" << std::endl;
    
    std::map<int, std::string> m{{1, "one"}, {2, "two"}};
    
    if (auto it = m.find(1); it != m.end()) {
        std::cout << "Found: " << it->second << std::endl;
    }
    
    switch (auto value = 42; value) {
        case 42:
            std::cout << "Value is 42" << std::endl;
            break;
        default:
            std::cout << "Value is not 42" << std::endl;
    }
}

// ===================================================================
// 9. [[FALLTHROUGH]], [[NODISCARD]], [[MAYBE_UNUSED]] ATTRIBUTES
// ===================================================================
[[nodiscard]] int important_function() {
    return 42;
}

void example_attributes([[maybe_unused]] int unused_param) {
    std::cout << "\n=== 9. ATTRIBUTES ===" << std::endl;
    
    int result = important_function();  // Must use result or compiler warns
    std::cout << "Result: " << result << std::endl;
    
    int value = 2;
    switch (value) {
        case 1:
            std::cout << "Case 1" << std::endl;
            [[fallthrough]];
        case 2:
            std::cout << "Case 2 (maybe from fallthrough)" << std::endl;
            break;
    }
}

// ===================================================================
// 10. STD::OPTIONAL
// ===================================================================
std::optional<int> try_parse_int(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return std::nullopt;
    }
}

void example_optional() {
    std::cout << "\n=== 10. STD::OPTIONAL ===" << std::endl;
    
    std::optional<int> opt1 = 42;
    std::optional<int> opt2 = std::nullopt;
    
    if (opt1) {
        std::cout << "opt1 has value: " << *opt1 << std::endl;
    }
    
    std::cout << "opt2 value or default: " << opt2.value_or(-1) << std::endl;
    
    auto result1 = try_parse_int("123");
    auto result2 = try_parse_int("abc");
    
    std::cout << "Parse '123': " << (result1 ? std::to_string(*result1) : "failed") << std::endl;
    std::cout << "Parse 'abc': " << (result2 ? std::to_string(*result2) : "failed") << std::endl;
}

// ===================================================================
// 11. STD::VARIANT
// ===================================================================
void example_variant() {
    std::cout << "\n=== 11. STD::VARIANT ===" << std::endl;
    
    std::variant<int, double, std::string> var;
    
    var = 42;
    std::cout << "Variant holds int: " << std::get<int>(var) << std::endl;
    
    var = 3.14;
    std::cout << "Variant holds double: " << std::get<double>(var) << std::endl;
    
    var = "hello";
    std::cout << "Variant holds string: " << std::get<std::string>(var) << std::endl;
    
    std::visit([](auto&& arg) {
        std::cout << "Visiting variant: " << arg << std::endl;
    }, var);
}

// ===================================================================
// 12. STD::ANY
// ===================================================================
void example_any() {
    std::cout << "\n=== 12. STD::ANY ===" << std::endl;
    
    std::any a = 42;
    std::cout << "Any holds int: " << std::any_cast<int>(a) << std::endl;
    
    a = std::string("hello");
    std::cout << "Any holds string: " << std::any_cast<std::string>(a) << std::endl;
    
    if (a.type() == typeid(std::string)) {
        std::cout << "Confirmed: any contains string" << std::endl;
    }
}

// ===================================================================
// 13. STD::STRING_VIEW
// ===================================================================
void print_string_view(std::string_view sv) {
    std::cout << "String view: " << sv << " (length: " << sv.length() << ")" << std::endl;
}

void example_string_view() {
    std::cout << "\n=== 13. STD::STRING_VIEW ===" << std::endl;
    
    std::string str = "Hello, World!";
    std::string_view sv = str;
    
    print_string_view(sv);
    print_string_view("Literal string");
    print_string_view(sv.substr(0, 5));
}

// ===================================================================
// 14. STD::FILESYSTEM
// ===================================================================
void example_filesystem() {
    std::cout << "\n=== 14. STD::FILESYSTEM ===" << std::endl;
    
    namespace fs = std::filesystem;
    
    fs::path p = fs::current_path();
    std::cout << "Current path: " << p << std::endl;
    
    fs::path example_path = "example.txt";
    std::cout << "Filename: " << example_path.filename() << std::endl;
    std::cout << "Extension: " << example_path.extension() << std::endl;
}

// ===================================================================
// 15. STD::CLAMP
// ===================================================================
void example_clamp() {
    std::cout << "\n=== 15. STD::CLAMP ===" << std::endl;
    
    int value = 50;
    int clamped = std::clamp(value, 10, 40);
    std::cout << "clamp(50, 10, 40) = " << clamped << std::endl;
    
    int value2 = 5;
    int clamped2 = std::clamp(value2, 10, 40);
    std::cout << "clamp(5, 10, 40) = " << clamped2 << std::endl;
}

// ===================================================================
// 16. GCD AND LCM
// ===================================================================
void example_gcd_lcm() {
    std::cout << "\n=== 16. GCD AND LCM ===" << std::endl;
    
    int a = 12, b = 18;
    std::cout << "gcd(" << a << ", " << b << ") = " << std::gcd(a, b) << std::endl;
    std::cout << "lcm(" << a << ", " << b << ") = " << std::lcm(a, b) << std::endl;
}

// ===================================================================
// 17. LAMBDA CAPTURE THIS BY VALUE
// ===================================================================
struct MyObject {
    int value = 123;
    
    auto get_value_copy() {
        return [*this] { return value; };
    }
    
    auto get_value_ref() {
        return [this] { return value; };
    }
};

void example_lambda_capture_this() {
    std::cout << "\n=== 17. LAMBDA CAPTURE THIS BY VALUE ===" << std::endl;
    
    MyObject obj;
    auto copy_lambda = obj.get_value_copy();
    auto ref_lambda = obj.get_value_ref();
    
    obj.value = 456;
    
    std::cout << "Copy lambda returns: " << copy_lambda() << " (original value)" << std::endl;
    std::cout << "Ref lambda returns: " << ref_lambda() << " (modified value)" << std::endl;
}

// ===================================================================
// 18. STD::INVOKE
// ===================================================================
int add_func(int a, int b) {
    return a + b;
}

struct Adder {
    int operator()(int a, int b) const {
        return a + b;
    }
};

void example_invoke() {
    std::cout << "\n=== 18. STD::INVOKE ===" << std::endl;
    
    std::cout << "invoke(add_func, 3, 4) = " << std::invoke(add_func, 3, 4) << std::endl;
    
    Adder adder;
    std::cout << "invoke(Adder, 5, 6) = " << std::invoke(adder, 5, 6) << std::endl;
    
    auto lambda = [](int a, int b) { return a * b; };
    std::cout << "invoke(lambda, 7, 8) = " << std::invoke(lambda, 7, 8) << std::endl;
}

// ===================================================================
// 19. STD::APPLY
// ===================================================================
int multiply(int a, int b, int c) {
    return a * b * c;
}

void example_apply() {
    std::cout << "\n=== 19. STD::APPLY ===" << std::endl;
    
    std::tuple<int, int, int> args{2, 3, 4};
    int result = std::apply(multiply, args);
    
    std::cout << "apply(multiply, {2, 3, 4}) = " << result << std::endl;
}

// ===================================================================
// 20. STD::MAKE_FROM_TUPLE
// ===================================================================
struct Point {
    int x, y, z;
    Point(int x, int y, int z) : x(x), y(y), z(z) {}
};

void example_make_from_tuple() {
    std::cout << "\n=== 20. STD::MAKE_FROM_TUPLE ===" << std::endl;
    
    std::tuple<int, int, int> coords{10, 20, 30};
    auto point = std::make_from_tuple<Point>(coords);
    
    std::cout << "Point created from tuple: (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    C++17 FEATURES COMPREHENSIVE EXAMPLES" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    example_template_argument_deduction();
    example_structured_bindings();
    example_folding_expressions();
    example_constexpr_if();
    example_constexpr_lambda();
    example_inline_variables();
    example_nested_namespaces();
    example_selection_with_initializer();
    example_attributes(42);
    example_optional();
    example_variant();
    example_any();
    example_string_view();
    example_filesystem();
    example_clamp();
    example_gcd_lcm();
    example_lambda_capture_this();
    example_invoke();
    example_apply();
    example_make_from_tuple();
    
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    ALL C++17 EXAMPLES COMPLETED" << std::endl;
    std::cout << "===============================================\n" << std::endl;
    
    return 0;
}
