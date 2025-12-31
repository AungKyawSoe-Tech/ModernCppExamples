#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <functional>

namespace ranges = std::ranges;
namespace views = std::views;

// ===================================================================
// 1. BASIC RANGES AND VIEWS
// ===================================================================
void example_basic_ranges() {
    std::cout << "\n=== 1. BASIC RANGES AND VIEWS ===" << std::endl;
    
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    std::cout << "Original: ";
    for (int n : numbers) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    // Using ranges::for_each
    std::cout << "Using ranges::for_each: ";
    ranges::for_each(numbers, [](int n) {
        std::cout << n << " ";
    });
    std::cout << std::endl;
}

// ===================================================================
// 2. FILTER VIEW
// ===================================================================
void example_filter_view() {
    std::cout << "\n=== 2. FILTER VIEW ===" << std::endl;
    
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Filter even numbers
    auto even_numbers = numbers | views::filter([](int n) { return n % 2 == 0; });
    
    std::cout << "Even numbers: ";
    for (int n : even_numbers) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    // Filter numbers greater than 5
    auto greater_than_5 = numbers | views::filter([](int n) { return n > 5; });
    
    std::cout << "Greater than 5: ";
    for (int n : greater_than_5) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 3. TRANSFORM VIEW
// ===================================================================
void example_transform_view() {
    std::cout << "\n=== 3. TRANSFORM VIEW ===" << std::endl;
    
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    
    // Square each number
    auto squared = numbers | views::transform([](int n) { return n * n; });
    
    std::cout << "Squared: ";
    for (int n : squared) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    // Convert to strings
    auto strings = numbers | views::transform([](int n) { return std::to_string(n); });
    
    std::cout << "As strings: ";
    for (const auto& s : strings) {
        std::cout << "'" << s << "' ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 4. TAKE AND DROP VIEWS
// ===================================================================
void example_take_drop_views() {
    std::cout << "\n=== 4. TAKE AND DROP VIEWS ===" << std::endl;
    
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Take first 5 elements
    auto first_five = numbers | views::take(5);
    std::cout << "First 5: ";
    for (int n : first_five) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    // Drop first 5 elements
    auto skip_five = numbers | views::drop(5);
    std::cout << "Skip 5: ";
    for (int n : skip_five) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    // Take while condition is true
    auto take_while_small = numbers | views::take_while([](int n) { return n < 6; });
    std::cout << "Take while < 6: ";
    for (int n : take_while_small) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 5. COMPOSING VIEWS
// ===================================================================
void example_composing_views() {
    std::cout << "\n=== 5. COMPOSING VIEWS ===" << std::endl;
    
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Chain multiple operations: filter even, square, take first 3
    auto result = numbers 
        | views::filter([](int n) { return n % 2 == 0; })
        | views::transform([](int n) { return n * n; })
        | views::take(3);
    
    std::cout << "Even, squared, first 3: ";
    for (int n : result) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 6. REVERSE VIEW
// ===================================================================
void example_reverse_view() {
    std::cout << "\n=== 6. REVERSE VIEW ===" << std::endl;
    
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    
    auto reversed = numbers | views::reverse;
    
    std::cout << "Reversed: ";
    for (int n : reversed) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 7. IOTA VIEW (GENERATE SEQUENCE)
// ===================================================================
void example_iota_view() {
    std::cout << "\n=== 7. IOTA VIEW (GENERATE SEQUENCE) ===" << std::endl;
    
    // Generate numbers from 1 to 10
    auto range1 = views::iota(1, 11);
    std::cout << "Iota 1 to 10: ";
    for (int n : range1) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    // Infinite range (take first 5)
    auto infinite_range = views::iota(1) | views::take(5);
    std::cout << "Infinite iota, take 5: ";
    for (int n : infinite_range) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 8. SPLIT VIEW
// ===================================================================
void example_split_view() {
    std::cout << "\n=== 8. SPLIT VIEW ===" << std::endl;
    
    std::string text = "Hello World C++ Ranges";
    
    auto words = text | views::split(' ');
    
    std::cout << "Split by space:" << std::endl;
    for (const auto& word : words) {
        std::cout << "  '";
        for (char c : word) {
            std::cout << c;
        }
        std::cout << "'" << std::endl;
    }
}

// ===================================================================
// 9. JOIN VIEW
// ===================================================================
void example_join_view() {
    std::cout << "\n=== 9. JOIN VIEW ===" << std::endl;
    
    std::vector<std::vector<int>> nested = {
        {1, 2, 3},
        {4, 5},
        {6, 7, 8, 9}
    };
    
    auto flattened = nested | views::join;
    
    std::cout << "Flattened: ";
    for (int n : flattened) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 10. KEYS AND VALUES VIEWS (WITH MAP)
// ===================================================================
void example_keys_values_views() {
    std::cout << "\n=== 10. KEYS AND VALUES VIEWS ===" << std::endl;
    
    std::vector<std::pair<std::string, int>> data = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35}
    };
    
    auto keys = data | views::keys;
    std::cout << "Keys: ";
    for (const auto& key : keys) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    
    auto values = data | views::values;
    std::cout << "Values: ";
    for (int value : values) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 11. RANGES ALGORITHMS
// ===================================================================
void example_ranges_algorithms() {
    std::cout << "\n=== 11. RANGES ALGORITHMS ===" << std::endl;
    
    std::vector<int> numbers = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    
    // Sort using ranges
    ranges::sort(numbers);
    std::cout << "Sorted: ";
    for (int n : numbers) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    
    // Find element
    auto it = ranges::find(numbers, 5);
    if (it != numbers.end()) {
        std::cout << "Found 5 at position: " << std::distance(numbers.begin(), it) << std::endl;
    }
    
    // Count elements greater than 5
    auto count = ranges::count_if(numbers, [](int n) { return n > 5; });
    std::cout << "Elements > 5: " << count << std::endl;
}

// ===================================================================
// 12. COMMON VIEW
// ===================================================================
void example_common_view() {
    std::cout << "\n=== 12. COMMON VIEW ===" << std::endl;
    
    auto range = views::iota(1, 11) | views::filter([](int n) { return n % 2 == 0; });
    
    // Convert to common range (can use with legacy algorithms)
    auto common_range = range | views::common;
    
    std::cout << "Common view result: ";
    for (int n : common_range) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 13. ENUMERATE VIEW (C++23 preview style)
// ===================================================================
void example_enumerate_style() {
    std::cout << "\n=== 13. ENUMERATE STYLE ===" << std::endl;
    
    std::vector<std::string> fruits = {"Apple", "Banana", "Cherry", "Date"};
    
    // Manual enumeration with iota and zip_transform style
    int index = 0;
    for (const auto& fruit : fruits) {
        std::cout << index++ << ": " << fruit << std::endl;
    }
}

// ===================================================================
// 14. COMPLEX PIPELINE
// ===================================================================
void example_complex_pipeline() {
    std::cout << "\n=== 14. COMPLEX PIPELINE ===" << std::endl;
    
    // Generate numbers, filter, transform, and take
    auto pipeline = views::iota(1, 101)
        | views::filter([](int n) { return n % 3 == 0; })  // Divisible by 3
        | views::transform([](int n) { return n * n; })     // Square
        | views::take(5);                                   // First 5
    
    std::cout << "First 5 squares of numbers divisible by 3:" << std::endl;
    for (int n : pipeline) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// 15. CONVERTING VIEWS TO CONTAINERS
// ===================================================================
void example_to_container() {
    std::cout << "\n=== 15. CONVERTING VIEWS TO CONTAINERS ===" << std::endl;
    
    auto view = views::iota(1, 11) 
        | views::filter([](int n) { return n % 2 == 0; })
        | views::transform([](int n) { return n * 2; });
    
    // Convert to vector
    std::vector<int> result(view.begin(), view.end());
    
    std::cout << "Converted to vector: ";
    for (int n : result) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    C++20 RANGES EXAMPLES" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    example_basic_ranges();
    example_filter_view();
    example_transform_view();
    example_take_drop_views();
    example_composing_views();
    example_reverse_view();
    example_iota_view();
    example_split_view();
    example_join_view();
    example_keys_values_views();
    example_ranges_algorithms();
    example_common_view();
    example_enumerate_style();
    example_complex_pipeline();
    example_to_container();
    
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    ALL EXAMPLES COMPLETED" << std::endl;
    std::cout << "===============================================\n" << std::endl;
    
    return 0;
}
