#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <array>
#include <set>
#include <utility>
#include <algorithm>

// ===================================================================
// 1. BASIC STRUCTURED BINDINGS WITH PAIR
// ===================================================================
void example_basic_structured_bindings() {
    std::cout << "\n=== 1. BASIC STRUCTURED BINDINGS WITH PAIR ===" << std::endl;
    
    std::pair<int, std::string> person{25, "Alice"};
    
    // Old way (C++11/14)
    int age_old = person.first;
    std::string name_old = person.second;
    std::cout << "Old way: " << name_old << " is " << age_old << " years old" << std::endl;
    
    // New way with structured bindings (C++17)
    auto [age, name] = person;
    std::cout << "Structured binding: " << name << " is " << age << " years old" << std::endl;
}

// ===================================================================
// 2. STRUCTURED BINDINGS WITH TUPLE
// ===================================================================
void example_tuple_bindings() {
    std::cout << "\n=== 2. STRUCTURED BINDINGS WITH TUPLE ===" << std::endl;
    
    std::tuple<int, double, std::string, bool> data{42, 3.14, "Hello", true};
    
    // Structured binding with tuple
    auto [num, pi, text, flag] = data;
    
    std::cout << "Number: " << num << std::endl;
    std::cout << "Pi: " << pi << std::endl;
    std::cout << "Text: " << text << std::endl;
    std::cout << "Flag: " << std::boolalpha << flag << std::endl;
}

// ===================================================================
// 3. STRUCTURED BINDINGS WITH ARRAYS
// ===================================================================
void example_array_bindings() {
    std::cout << "\n=== 3. STRUCTURED BINDINGS WITH ARRAYS ===" << std::endl;
    
    // C-array
    int c_array[3] = {10, 20, 30};
    auto [a, b, c] = c_array;
    std::cout << "C-array elements: " << a << ", " << b << ", " << c << std::endl;
    
    // std::array
    std::array<std::string, 3> std_array = {"Red", "Green", "Blue"};
    auto [color1, color2, color3] = std_array;
    std::cout << "Colors: " << color1 << ", " << color2 << ", " << color3 << std::endl;
}

// ===================================================================
// 4. STRUCTURED BINDINGS WITH STRUCTS
// ===================================================================
struct Point {
    int x;
    int y;
    int z;
};

struct Employee {
    std::string name;
    int id;
    double salary;
};

void example_struct_bindings() {
    std::cout << "\n=== 4. STRUCTURED BINDINGS WITH STRUCTS ===" << std::endl;
    
    Point p{10, 20, 30};
    auto [x, y, z] = p;
    std::cout << "Point coordinates: x=" << x << ", y=" << y << ", z=" << z << std::endl;
    
    Employee emp{"John Doe", 12345, 75000.50};
    auto [emp_name, emp_id, emp_salary] = emp;
    std::cout << "Employee: " << emp_name << " (ID: " << emp_id 
              << ", Salary: $" << emp_salary << ")" << std::endl;
}

// ===================================================================
// 5. STRUCTURED BINDINGS WITH MAP ITERATION
// ===================================================================
void example_map_iteration() {
    std::cout << "\n=== 5. STRUCTURED BINDINGS WITH MAP ITERATION ===" << std::endl;
    
    std::map<std::string, int> age_map = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35}
    };
    
    // Old way
    std::cout << "Old way:" << std::endl;
    for (const auto& pair : age_map) {
        std::cout << "  " << pair.first << " is " << pair.second << " years old" << std::endl;
    }
    
    // New way with structured bindings
    std::cout << "With structured bindings:" << std::endl;
    for (const auto& [name, age] : age_map) {
        std::cout << "  " << name << " is " << age << " years old" << std::endl;
    }
}

// ===================================================================
// 6. STRUCTURED BINDINGS WITH REFERENCES
// ===================================================================
void example_reference_bindings() {
    std::cout << "\n=== 6. STRUCTURED BINDINGS WITH REFERENCES ===" << std::endl;
    
    std::pair<int, std::string> data{100, "Original"};
    
    // Non-reference binding (copy)
    auto [val1, str1] = data;
    val1 = 200;
    str1 = "Modified Copy";
    std::cout << "Original after copy modification: " 
              << data.first << ", " << data.second << std::endl;
    
    // Reference binding
    auto& [val2, str2] = data;
    val2 = 300;
    str2 = "Modified Reference";
    std::cout << "Original after reference modification: " 
              << data.first << ", " << data.second << std::endl;
}

// ===================================================================
// 7. STRUCTURED BINDINGS WITH CONST
// ===================================================================
void example_const_bindings() {
    std::cout << "\n=== 7. STRUCTURED BINDINGS WITH CONST ===" << std::endl;
    
    const std::pair<int, std::string> data{42, "Constant"};
    
    // Const structured binding
    const auto& [value, text] = data;
    std::cout << "Const binding: " << value << ", " << text << std::endl;
    
    // value = 100;  // ERROR: cannot modify const
}

// ===================================================================
// 8. STRUCTURED BINDINGS WITH FUNCTION RETURNS
// ===================================================================
std::tuple<int, double, std::string> get_data() {
    return {42, 3.14159, "Function Return"};
}

std::pair<bool, std::string> validate_input(int value) {
    if (value > 0) {
        return {true, "Valid"};
    } else {
        return {false, "Invalid: must be positive"};
    }
}

void example_function_return_bindings() {
    std::cout << "\n=== 8. STRUCTURED BINDINGS WITH FUNCTION RETURNS ===" << std::endl;
    
    auto [num, pi, message] = get_data();
    std::cout << "From function: " << num << ", " << pi << ", " << message << std::endl;
    
    auto [is_valid, error_msg] = validate_input(10);
    std::cout << "Validation: " << std::boolalpha << is_valid << " - " << error_msg << std::endl;
    
    auto [is_valid2, error_msg2] = validate_input(-5);
    std::cout << "Validation: " << std::boolalpha << is_valid2 << " - " << error_msg2 << std::endl;
}

// ===================================================================
// 9. STRUCTURED BINDINGS WITH NESTED STRUCTURES
// ===================================================================
struct Address {
    std::string street;
    std::string city;
    int zipcode;
};

struct Person {
    std::string name;
    int age;
    Address address;
};

void example_nested_bindings() {
    std::cout << "\n=== 9. STRUCTURED BINDINGS WITH NESTED STRUCTURES ===" << std::endl;
    
    Person person{"John Smith", 30, {"123 Main St", "Springfield", 12345}};
    
    auto [name, age, address] = person;
    auto [street, city, zipcode] = address;
    
    std::cout << "Person: " << name << ", Age: " << age << std::endl;
    std::cout << "Address: " << street << ", " << city << " " << zipcode << std::endl;
}

// ===================================================================
// 10. STRUCTURED BINDINGS WITH STL ALGORITHMS
// ===================================================================
void example_algorithms_with_bindings() {
    std::cout << "\n=== 10. STRUCTURED BINDINGS WITH STL ALGORITHMS ===" << std::endl;
    
    std::vector<std::pair<std::string, int>> scores = {
        {"Alice", 95},
        {"Bob", 87},
        {"Charlie", 92},
        {"David", 88}
    };
    
    // Find student with highest score
    auto max_it = std::max_element(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) {
            auto [name1, score1] = a;
            auto [name2, score2] = b;
            return score1 < score2;
        });
    
    if (max_it != scores.end()) {
        auto [top_student, top_score] = *max_it;
        std::cout << "Top student: " << top_student << " with score " << top_score << std::endl;
    }
    
    // Print all with structured bindings
    std::cout << "All scores:" << std::endl;
    std::for_each(scores.begin(), scores.end(),
        [](const auto& entry) {
            auto [name, score] = entry;
            std::cout << "  " << name << ": " << score << std::endl;
        });
}

// ===================================================================
// 11. STRUCTURED BINDINGS WITH INSERT OPERATIONS
// ===================================================================
void example_insert_bindings() {
    std::cout << "\n=== 11. STRUCTURED BINDINGS WITH INSERT OPERATIONS ===" << std::endl;
    
    std::map<int, std::string> id_map;
    
    // Insert returns pair<iterator, bool>
    auto [it1, inserted1] = id_map.insert({1, "First"});
    std::cout << "Insert 1: " << (inserted1 ? "Success" : "Failed") << std::endl;
    
    auto [it2, inserted2] = id_map.insert({1, "Duplicate"});
    std::cout << "Insert duplicate 1: " << (inserted2 ? "Success" : "Failed") << std::endl;
    
    auto [it3, inserted3] = id_map.insert({2, "Second"});
    std::cout << "Insert 2: " << (inserted3 ? "Success" : "Failed") << std::endl;
    
    std::cout << "Map contents:" << std::endl;
    for (const auto& [id, value] : id_map) {
        std::cout << "  " << id << ": " << value << std::endl;
    }
}

// ===================================================================
// 12. STRUCTURED BINDINGS WITH MULTIPLE RETURNS
// ===================================================================
struct Stats {
    double mean;
    double median;
    double stddev;
};

Stats calculate_stats(const std::vector<int>& data) {
    double sum = 0;
    for (int val : data) sum += val;
    double mean = sum / data.size();
    
    return {mean, static_cast<double>(data[data.size()/2]), 10.5}; // simplified
}

void example_multiple_return_values() {
    std::cout << "\n=== 12. STRUCTURED BINDINGS WITH MULTIPLE RETURNS ===" << std::endl;
    
    std::vector<int> dataset = {10, 20, 30, 40, 50};
    
    auto [mean, median, stddev] = calculate_stats(dataset);
    
    std::cout << "Statistics:" << std::endl;
    std::cout << "  Mean: " << mean << std::endl;
    std::cout << "  Median: " << median << std::endl;
    std::cout << "  Std Dev: " << stddev << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    C++17 STRUCTURED BINDINGS EXAMPLES" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    example_basic_structured_bindings();
    example_tuple_bindings();
    example_array_bindings();
    example_struct_bindings();
    example_map_iteration();
    example_reference_bindings();
    example_const_bindings();
    example_function_return_bindings();
    example_nested_bindings();
    example_algorithms_with_bindings();
    example_insert_bindings();
    example_multiple_return_values();
    
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    ALL EXAMPLES COMPLETED" << std::endl;
    std::cout << "===============================================\n" << std::endl;
    
    return 0;
}
