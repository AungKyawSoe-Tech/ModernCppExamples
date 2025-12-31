#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

// ===================================================================
// 1. BASIC OPTIONAL USAGE
// ===================================================================
void example_basic_optional() {
    std::cout << "\n=== 1. BASIC OPTIONAL USAGE ===" << std::endl;
    
    std::optional<int> maybe_value;  // Empty optional
    
    std::cout << "Has value: " << (maybe_value.has_value() ? "Yes" : "No") << std::endl;
    
    maybe_value = 42;  // Assign value
    std::cout << "Has value: " << (maybe_value.has_value() ? "Yes" : "No") << std::endl;
    std::cout << "Value: " << *maybe_value << std::endl;
    std::cout << "Value (using .value()): " << maybe_value.value() << std::endl;
}

// ===================================================================
// 2. OPTIONAL WITH VALUE_OR
// ===================================================================
void example_value_or() {
    std::cout << "\n=== 2. OPTIONAL WITH VALUE_OR ===" << std::endl;
    
    std::optional<int> empty_opt;
    std::optional<int> filled_opt = 100;
    
    std::cout << "Empty optional value_or(0): " << empty_opt.value_or(0) << std::endl;
    std::cout << "Filled optional value_or(0): " << filled_opt.value_or(0) << std::endl;
    
    std::optional<std::string> empty_str;
    std::cout << "Empty string value_or: " << empty_str.value_or("default") << std::endl;
}

// ===================================================================
// 3. OPTIONAL AS FUNCTION RETURN TYPE
// ===================================================================
std::optional<int> find_value(const std::vector<int>& vec, int target) {
    auto it = std::find(vec.begin(), vec.end(), target);
    if (it != vec.end()) {
        return *it;
    }
    return std::nullopt;  // Return empty optional
}

std::optional<std::string> get_name_by_id(int id) {
    std::map<int, std::string> database = {
        {1, "Alice"},
        {2, "Bob"},
        {3, "Charlie"}
    };
    
    auto it = database.find(id);
    if (it != database.end()) {
        return it->second;
    }
    return std::nullopt;
}

void example_optional_return() {
    std::cout << "\n=== 3. OPTIONAL AS FUNCTION RETURN TYPE ===" << std::endl;
    
    std::vector<int> numbers = {10, 20, 30, 40, 50};
    
    auto result1 = find_value(numbers, 30);
    if (result1) {
        std::cout << "Found: " << *result1 << std::endl;
    } else {
        std::cout << "Not found" << std::endl;
    }
    
    auto result2 = find_value(numbers, 99);
    if (result2) {
        std::cout << "Found: " << *result2 << std::endl;
    } else {
        std::cout << "Not found" << std::endl;
    }
    
    auto name1 = get_name_by_id(2);
    std::cout << "ID 2: " << name1.value_or("Unknown") << std::endl;
    
    auto name2 = get_name_by_id(99);
    std::cout << "ID 99: " << name2.value_or("Unknown") << std::endl;
}

// ===================================================================
// 4. OPTIONAL WITH IF STATEMENT
// ===================================================================
void example_optional_if() {
    std::cout << "\n=== 4. OPTIONAL WITH IF STATEMENT ===" << std::endl;
    
    auto maybe_name = get_name_by_id(1);
    
    // C++17 if with initializer
    if (auto name = get_name_by_id(1); name.has_value()) {
        std::cout << "Found name: " << *name << std::endl;
    } else {
        std::cout << "Name not found" << std::endl;
    }
    
    if (auto name = get_name_by_id(999); name) {
        std::cout << "Found name: " << *name << std::endl;
    } else {
        std::cout << "Name not found" << std::endl;
    }
}

// ===================================================================
// 5. OPTIONAL WITH EXCEPTIONS
// ===================================================================
void example_optional_exceptions() {
    std::cout << "\n=== 5. OPTIONAL WITH EXCEPTIONS ===" << std::endl;
    
    std::optional<int> empty_opt;
    std::optional<int> filled_opt = 42;
    
    try {
        std::cout << "Filled optional value: " << filled_opt.value() << std::endl;
        std::cout << "Empty optional value: " << empty_opt.value() << std::endl;  // Throws!
    } catch (const std::bad_optional_access& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }
}

// ===================================================================
// 6. OPTIONAL WITH CUSTOM TYPES
// ===================================================================
struct Person {
    std::string name;
    int age;
    
    Person(const std::string& n, int a) : name(n), age(a) {}
    
    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        os << p.name << " (age " << p.age << ")";
        return os;
    }
};

std::optional<Person> find_person(const std::string& name) {
    std::vector<Person> people = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35}
    };
    
    auto it = std::find_if(people.begin(), people.end(),
        [&name](const Person& p) { return p.name == name; });
    
    if (it != people.end()) {
        return *it;
    }
    return std::nullopt;
}

void example_optional_custom_type() {
    std::cout << "\n=== 6. OPTIONAL WITH CUSTOM TYPES ===" << std::endl;
    
    auto person1 = find_person("Alice");
    if (person1) {
        std::cout << "Found: " << *person1 << std::endl;
    }
    
    auto person2 = find_person("David");
    if (person2) {
        std::cout << "Found: " << *person2 << std::endl;
    } else {
        std::cout << "Person not found" << std::endl;
    }
}

// ===================================================================
// 7. OPTIONAL WITH EMPLACE
// ===================================================================
void example_optional_emplace() {
    std::cout << "\n=== 7. OPTIONAL WITH EMPLACE ===" << std::endl;
    
    std::optional<Person> opt_person;
    
    std::cout << "Before emplace: " << (opt_person.has_value() ? "Has value" : "Empty") << std::endl;
    
    opt_person.emplace("John", 28);  // Construct in-place
    
    std::cout << "After emplace: " << *opt_person << std::endl;
    
    opt_person.emplace("Jane", 32);  // Replace existing value
    std::cout << "After second emplace: " << *opt_person << std::endl;
}

// ===================================================================
// 8. OPTIONAL WITH RESET
// ===================================================================
void example_optional_reset() {
    std::cout << "\n=== 8. OPTIONAL WITH RESET ===" << std::endl;
    
    std::optional<int> value = 100;
    std::cout << "Initial value: " << *value << std::endl;
    
    value.reset();  // Clear the optional
    std::cout << "After reset: " << (value.has_value() ? "Has value" : "Empty") << std::endl;
    
    value = 200;
    std::cout << "After reassignment: " << *value << std::endl;
    
    value = std::nullopt;  // Another way to clear
    std::cout << "After nullopt: " << (value.has_value() ? "Has value" : "Empty") << std::endl;
}

// ===================================================================
// 9. OPTIONAL CHAINING WITH TRANSFORM (C++23-style)
// ===================================================================
std::optional<double> safe_sqrt(double value) {
    if (value >= 0.0) {
        return std::sqrt(value);
    }
    return std::nullopt;
}

std::optional<double> safe_divide(double numerator, double denominator) {
    if (denominator != 0.0) {
        return numerator / denominator;
    }
    return std::nullopt;
}

void example_optional_chaining() {
    std::cout << "\n=== 9. OPTIONAL CHAINING ===" << std::endl;
    
    auto result1 = safe_divide(100, 4);
    if (result1) {
        auto sqrt_result = safe_sqrt(*result1);
        if (sqrt_result) {
            std::cout << "sqrt(100/4) = " << *sqrt_result << std::endl;
        }
    }
    
    auto result2 = safe_divide(100, 0);
    std::cout << "Division by zero result: " 
              << (result2.has_value() ? "Has value" : "Empty") << std::endl;
}

// ===================================================================
// 10. OPTIONAL WITH COMPARISON
// ===================================================================
void example_optional_comparison() {
    std::cout << "\n=== 10. OPTIONAL WITH COMPARISON ===" << std::endl;
    
    std::optional<int> opt1 = 10;
    std::optional<int> opt2 = 20;
    std::optional<int> opt3 = 10;
    std::optional<int> empty;
    
    std::cout << "opt1 == opt3: " << (opt1 == opt3 ? "true" : "false") << std::endl;
    std::cout << "opt1 < opt2: " << (opt1 < opt2 ? "true" : "false") << std::endl;
    std::cout << "opt1 == 10: " << (opt1 == 10 ? "true" : "false") << std::endl;
    std::cout << "empty == nullopt: " << (empty == std::nullopt ? "true" : "false") << std::endl;
}

// ===================================================================
// 11. OPTIONAL IN CONTAINERS
// ===================================================================
void example_optional_in_containers() {
    std::cout << "\n=== 11. OPTIONAL IN CONTAINERS ===" << std::endl;
    
    std::vector<std::optional<int>> values = {
        10,
        std::nullopt,
        20,
        std::nullopt,
        30
    };
    
    std::cout << "Processing optional values:" << std::endl;
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i]) {
            std::cout << "  Index " << i << ": " << *values[i] << std::endl;
        } else {
            std::cout << "  Index " << i << ": empty" << std::endl;
        }
    }
    
    // Count non-empty values
    int count = 0;
    for (const auto& opt : values) {
        if (opt) ++count;
    }
    std::cout << "Non-empty values: " << count << std::endl;
}

// ===================================================================
// 12. OPTIONAL WITH MAKE_OPTIONAL
// ===================================================================
std::optional<Person> create_person_if_valid(const std::string& name, int age) {
    if (!name.empty() && age > 0 && age < 150) {
        return Person(name, age);
    }
    return std::nullopt;
}

void example_make_optional() {
    std::cout << "\n=== 12. OPTIONAL WITH MAKE_OPTIONAL ===" << std::endl;
    
    auto person1 = create_person_if_valid("Alice", 30);
    if (person1) {
        std::cout << "Valid person: " << *person1 << std::endl;
    }
    
    auto person2 = create_person_if_valid("", 25);
    if (person2) {
        std::cout << "Valid person: " << *person2 << std::endl;
    } else {
        std::cout << "Invalid person (empty name)" << std::endl;
    }
    
    auto person3 = create_person_if_valid("Bob", -5);
    if (person3) {
        std::cout << "Valid person: " << *person3 << std::endl;
    } else {
        std::cout << "Invalid person (negative age)" << std::endl;
    }
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    C++17 STD::OPTIONAL EXAMPLES" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    example_basic_optional();
    example_value_or();
    example_optional_return();
    example_optional_if();
    example_optional_exceptions();
    example_optional_custom_type();
    example_optional_emplace();
    example_optional_reset();
    example_optional_chaining();
    example_optional_comparison();
    example_optional_in_containers();
    example_make_optional();
    
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    ALL EXAMPLES COMPLETED" << std::endl;
    std::cout << "===============================================\n" << std::endl;
    
    return 0;
}
