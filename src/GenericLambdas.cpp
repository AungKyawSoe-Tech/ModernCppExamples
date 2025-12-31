#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <map>
#include <memory>

// ===================================================================
// 1. BASIC GENERIC LAMBDA (C++14)
// ===================================================================
void example_basic_generic_lambda() {
    std::cout << "\n=== 1. BASIC GENERIC LAMBDA (C++14) ===" << std::endl;
    
    // Generic lambda that works with any type
    auto identity = [](auto x) {
        return x;
    };
    
    std::cout << "Identity int: " << identity(42) << std::endl;
    std::cout << "Identity double: " << identity(3.14) << std::endl;
    std::cout << "Identity string: " << identity(std::string("Hello")) << std::endl;
    std::cout << "Identity char: " << identity('A') << std::endl;
}

// ===================================================================
// 2. GENERIC LAMBDA WITH TYPE OPERATIONS
// ===================================================================
void example_generic_lambda_operations() {
    std::cout << "\n=== 2. GENERIC LAMBDA WITH TYPE OPERATIONS ===" << std::endl;
    
    // Generic lambda that adds two values
    auto add = [](auto a, auto b) {
        return a + b;
    };
    
    std::cout << "Add ints: " << add(10, 20) << std::endl;
    std::cout << "Add doubles: " << add(3.14, 2.86) << std::endl;
    std::cout << "Add strings: " << add(std::string("Hello "), std::string("World")) << std::endl;
    
    // Generic lambda for multiplication
    auto multiply = [](auto a, auto b) {
        return a * b;
    };
    
    std::cout << "Multiply ints: " << multiply(5, 6) << std::endl;
    std::cout << "Multiply doubles: " << multiply(2.5, 4.0) << std::endl;
}

// ===================================================================
// 3. GENERIC LAMBDA WITH CONTAINERS
// ===================================================================
void example_generic_lambda_containers() {
    std::cout << "\n=== 3. GENERIC LAMBDA WITH CONTAINERS ===" << std::endl;
    
    // Generic lambda to print container elements
    auto print_container = [](const auto& container, const std::string& name) {
        std::cout << name << ": ";
        for (const auto& elem : container) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    };
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::vector<std::string> vec_str = {"Hello", "World", "C++14"};
    
    print_container(vec, "Vector<int>");
    print_container(vec_str, "Vector<string>");
}

// ===================================================================
// 4. GENERIC LAMBDA WITH STL ALGORITHMS
// ===================================================================
void example_generic_lambda_algorithms() {
    std::cout << "\n=== 4. GENERIC LAMBDA WITH STL ALGORITHMS ===" << std::endl;
    
    std::vector<int> numbers = {5, 2, 8, 1, 9, 3};
    
    // Generic lambda for comparison
    auto greater_than = [](auto a, auto b) {
        return a > b;
    };
    
    std::sort(numbers.begin(), numbers.end(), greater_than);
    
    std::cout << "Sorted (descending): ";
    for (auto n : numbers) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    // Generic lambda for transformation
    auto square = [](auto x) {
        return x * x;
    };
    
    std::vector<int> squared(numbers.size());
    std::transform(numbers.begin(), numbers.end(), squared.begin(), square);
    
    std::cout << "Squared: ";
    for (auto n : squared) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 5. GENERIC LAMBDA WITH MULTIPLE AUTO PARAMETERS
// ===================================================================
void example_multiple_auto_parameters() {
    std::cout << "\n=== 5. GENERIC LAMBDA WITH MULTIPLE AUTO PARAMETERS ===" << std::endl;
    
    // Generic lambda with three different types
    auto combine = [](auto a, auto b, auto c) {
        std::cout << "Types can differ: " << a << ", " << b << ", " << c << std::endl;
        return true;
    };
    
    combine(42, 3.14, "Hello");
    combine(std::string("World"), 100, 'X');
}

// ===================================================================
// 6. GENERIC LAMBDA WITH VARIADIC PARAMETERS
// ===================================================================
void example_generic_lambda_variadic() {
    std::cout << "\n=== 6. GENERIC LAMBDA WITH VARIADIC PARAMETERS ===" << std::endl;
    
    // Generic lambda that sums any number of arguments
    auto sum_all = [](auto... args) {
        return (args + ...);  // C++17 fold expression
    };
    
    std::cout << "Sum of ints: " << sum_all(1, 2, 3, 4, 5) << std::endl;
    std::cout << "Sum of doubles: " << sum_all(1.1, 2.2, 3.3) << std::endl;
    std::cout << "Sum mixed: " << sum_all(1, 2.5, 3) << std::endl;
}

// ===================================================================
// 7. GENERIC LAMBDA FOR FACTORY PATTERN
// ===================================================================
void example_generic_lambda_factory() {
    std::cout << "\n=== 7. GENERIC LAMBDA FOR FACTORY PATTERN ===" << std::endl;
    
    // Generic factory lambda
    auto make_unique_ptr = [](auto value) {
        using T = decltype(value);
        return std::make_unique<T>(value);
    };
    
    auto int_ptr = make_unique_ptr(42);
    auto str_ptr = make_unique_ptr(std::string("Hello"));
    auto double_ptr = make_unique_ptr(3.14);
    
    std::cout << "Int unique_ptr: " << *int_ptr << std::endl;
    std::cout << "String unique_ptr: " << *str_ptr << std::endl;
    std::cout << "Double unique_ptr: " << *double_ptr << std::endl;
}

// ===================================================================
// 8. GENERIC LAMBDA WITH MAP OPERATIONS
// ===================================================================
void example_generic_lambda_map() {
    std::cout << "\n=== 8. GENERIC LAMBDA WITH MAP OPERATIONS ===" << std::endl;
    
    std::map<std::string, int> age_map = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35}
    };
    
    // Generic lambda to print key-value pairs
    auto print_map = [](const auto& map, const std::string& name) {
        std::cout << name << ":" << std::endl;
        for (const auto& [key, value] : map) {
            std::cout << "  " << key << " -> " << value << std::endl;
        }
    };
    
    print_map(age_map, "Age Map");
    
    // Generic lambda for filtering
    auto filter_by_value = [](const auto& map, auto threshold) {
        std::vector<typename std::decay<decltype(map)>::type::key_type> result;
        for (const auto& [key, value] : map) {
            if (value > threshold) {
                result.push_back(key);
            }
        }
        return result;
    };
    
    auto filtered = filter_by_value(age_map, 28);
    std::cout << "People older than 28: ";
    for (const auto& name : filtered) {
        std::cout << name << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 9. GENERIC LAMBDA AS PREDICATE
// ===================================================================
void example_generic_lambda_predicate() {
    std::cout << "\n=== 9. GENERIC LAMBDA AS PREDICATE ===" << std::endl;
    
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Generic lambda predicate
    auto is_even = [](auto n) {
        return n % 2 == 0;
    };
    
    auto count = std::count_if(numbers.begin(), numbers.end(), is_even);
    std::cout << "Even numbers count: " << count << std::endl;
    
    // Find first even number
    auto it = std::find_if(numbers.begin(), numbers.end(), is_even);
    if (it != numbers.end()) {
        std::cout << "First even number: " << *it << std::endl;
    }
}

// ===================================================================
// 10. GENERIC LAMBDA WITH RETURN TYPE DEDUCTION
// ===================================================================
void example_return_type_deduction() {
    std::cout << "\n=== 10. GENERIC LAMBDA WITH RETURN TYPE DEDUCTION ===" << std::endl;
    
    // Lambda with auto return type - deduced from expression
    auto double_value = [](auto x) {
        return x * 2;
    };
    
    // Lambda with explicit return type
    auto to_string_value = [](auto x) -> std::string {
        return std::to_string(x);
    };
    
    std::cout << "Double int: " << double_value(21) << std::endl;
    std::cout << "Double double: " << double_value(3.14) << std::endl;
    std::cout << "To string (int): " << to_string_value(42) << std::endl;
    std::cout << "To string (double): " << to_string_value(3.14159) << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    C++14 GENERIC LAMBDAS EXAMPLES" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    example_basic_generic_lambda();
    example_generic_lambda_operations();
    example_generic_lambda_containers();
    example_generic_lambda_algorithms();
    example_multiple_auto_parameters();
    example_generic_lambda_variadic();
    example_generic_lambda_factory();
    example_generic_lambda_map();
    example_generic_lambda_predicate();
    example_return_type_deduction();
    
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    ALL EXAMPLES COMPLETED" << std::endl;
    std::cout << "===============================================\n" << std::endl;
    
    return 0;
}
