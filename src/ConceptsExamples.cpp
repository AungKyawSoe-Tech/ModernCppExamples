#include <iostream>
#include <string>
#include <vector>
#include <concepts>
#include <type_traits>
#include <algorithm>
#include <iterator>

// ===================================================================
// 1. BASIC CONCEPT DEFINITION
// ===================================================================
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<Numeric T>
T add(T a, T b) {
    return a + b;
}

void example_basic_concept() {
    std::cout << "\n=== 1. BASIC CONCEPT DEFINITION ===" << std::endl;
    
    std::cout << "add(10, 20) = " << add(10, 20) << std::endl;
    std::cout << "add(3.14, 2.86) = " << add(3.14, 2.86) << std::endl;
    
    // This would fail to compile:
    // add(std::string("hello"), std::string("world"));
}

// ===================================================================
// 2. STANDARD LIBRARY CONCEPTS
// ===================================================================
template<std::integral T>
T multiply(T a, T b) {
    return a * b;
}

template<std::floating_point T>
T divide(T a, T b) {
    return a / b;
}

void example_standard_concepts() {
    std::cout << "\n=== 2. STANDARD LIBRARY CONCEPTS ===" << std::endl;
    
    std::cout << "multiply(5, 6) = " << multiply(5, 6) << std::endl;
    std::cout << "divide(10.0, 3.0) = " << divide(10.0, 3.0) << std::endl;
    
    // multiply(3.14, 2.0);  // ERROR: requires integral type
    // divide(10, 3);        // ERROR: requires floating point type
}

// ===================================================================
// 3. COMPOUND CONCEPTS
// ===================================================================
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

template<typename T>
concept Printable = requires(T t) {
    { std::cout << t } -> std::convertible_to<std::ostream&>;
};

template<typename T>
concept AddableAndPrintable = Addable<T> && Printable<T>;

template<AddableAndPrintable T>
void print_sum(T a, T b) {
    std::cout << a << " + " << b << " = " << (a + b) << std::endl;
}

void example_compound_concepts() {
    std::cout << "\n=== 3. COMPOUND CONCEPTS ===" << std::endl;
    
    print_sum(10, 20);
    print_sum(3.14, 2.86);
    print_sum(std::string("Hello "), std::string("World"));
}

// ===================================================================
// 4. REQUIRES CLAUSE
// ===================================================================
template<typename T>
T max_value(T a, T b) requires std::totally_ordered<T> {
    return (a > b) ? a : b;
}

template<typename T>
requires std::integral<T>
T factorial(T n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

void example_requires_clause() {
    std::cout << "\n=== 4. REQUIRES CLAUSE ===" << std::endl;
    
    std::cout << "max(10, 20) = " << max_value(10, 20) << std::endl;
    std::cout << "max(3.14, 2.71) = " << max_value(3.14, 2.71) << std::endl;
    
    std::cout << "factorial(5) = " << factorial(5) << std::endl;
}

// ===================================================================
// 5. CONCEPTS WITH MULTIPLE REQUIREMENTS
// ===================================================================
template<typename T>
concept Container = requires(T t) {
    typename T::value_type;
    typename T::iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.end() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<std::size_t>;
};

template<Container C>
void print_container(const C& container, const std::string& name) {
    std::cout << name << " (size " << container.size() << "): ";
    for (const auto& elem : container) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

void example_container_concept() {
    std::cout << "\n=== 5. CONCEPTS WITH MULTIPLE REQUIREMENTS ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    print_container(vec, "Vector");
    
    std::vector<std::string> words = {"Hello", "Modern", "C++"};
    print_container(words, "Strings");
}

// ===================================================================
// 6. CONCEPT SUBSUMPTION
// ===================================================================
template<typename T>
concept Number = std::is_arithmetic_v<T>;

template<typename T>
concept Integer = Number<T> && std::is_integral_v<T>;

template<Number T>
void process(T value) {
    std::cout << "Processing number: " << value << std::endl;
}

template<Integer T>
void process(T value) {
    std::cout << "Processing integer: " << value << " (doubled: " << value * 2 << ")" << std::endl;
}

void example_concept_subsumption() {
    std::cout << "\n=== 6. CONCEPT SUBSUMPTION ===" << std::endl;
    
    process(42);        // Calls Integer version
    process(3.14);      // Calls Number version
}

// ===================================================================
// 7. CUSTOM CONCEPT FOR RANGES
// ===================================================================
template<typename T>
concept Sortable = requires(T container) {
    { container.begin() } -> std::input_or_output_iterator;
    { container.end() } -> std::input_or_output_iterator;
    requires std::sortable<decltype(container.begin())>;
};

template<Sortable C>
void sort_and_print(C& container) {
    std::sort(container.begin(), container.end());
    std::cout << "Sorted: ";
    for (const auto& elem : container) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

void example_sortable_concept() {
    std::cout << "\n=== 7. CUSTOM CONCEPT FOR RANGES ===" << std::endl;
    
    std::vector<int> numbers = {5, 2, 8, 1, 9};
    sort_and_print(numbers);
}

// ===================================================================
// 8. CONCEPT WITH CLASS TEMPLATE
// ===================================================================
template<typename T>
concept Multipliable = requires(T a, T b) {
    { a * b } -> std::convertible_to<T>;
};

template<Multipliable T>
class Point2D {
private:
    T x, y;
    
public:
    Point2D(T x_val, T y_val) : x(x_val), y(y_val) {}
    
    T dot_product(const Point2D& other) const {
        return x * other.x + y * other.y;
    }
    
    void print() const {
        std::cout << "Point(" << x << ", " << y << ")" << std::endl;
    }
};

void example_concept_class_template() {
    std::cout << "\n=== 8. CONCEPT WITH CLASS TEMPLATE ===" << std::endl;
    
    Point2D<int> p1(3, 4);
    Point2D<int> p2(5, 6);
    
    p1.print();
    p2.print();
    std::cout << "Dot product: " << p1.dot_product(p2) << std::endl;
}

// ===================================================================
// 9. CONCEPTS WITH LOGICAL OPERATORS
// ===================================================================
template<typename T>
concept SignedIntegral = std::integral<T> && std::is_signed_v<T>;

template<typename T>
concept UnsignedIntegral = std::integral<T> && std::is_unsigned_v<T>;

template<SignedIntegral T>
T abs_value(T value) {
    return value < 0 ? -value : value;
}

template<UnsignedIntegral T>
T abs_value(T value) {
    return value;  // Already positive
}

void example_logical_concepts() {
    std::cout << "\n=== 9. CONCEPTS WITH LOGICAL OPERATORS ===" << std::endl;
    
    std::cout << "abs(-42) = " << abs_value(-42) << std::endl;
    std::cout << "abs(42u) = " << abs_value(42u) << std::endl;
}

// ===================================================================
// 10. CONCEPT FOR CALLABLE TYPES
// ===================================================================
template<typename F, typename T>
concept Predicate = std::predicate<F, T>;

template<typename C, typename Pred>
requires Container<C> && Predicate<Pred, typename C::value_type>
void filter_and_print(const C& container, Pred pred, const std::string& name) {
    std::cout << name << ": ";
    for (const auto& elem : container) {
        if (pred(elem)) {
            std::cout << elem << " ";
        }
    }
    std::cout << std::endl;
}

void example_predicate_concept() {
    std::cout << "\n=== 10. CONCEPT FOR CALLABLE TYPES ===" << std::endl;
    
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    filter_and_print(numbers, [](int n) { return n % 2 == 0; }, "Even numbers");
    filter_and_print(numbers, [](int n) { return n > 5; }, "Greater than 5");
}

// ===================================================================
// 11. CONCEPT FOR CUSTOM TYPES
// ===================================================================
template<typename T>
concept Drawable = requires(T obj) {
    { obj.draw() } -> std::same_as<void>;
    { obj.get_name() } -> std::convertible_to<std::string>;
};

class Circle {
public:
    void draw() const {
        std::cout << "Drawing a circle" << std::endl;
    }
    
    std::string get_name() const {
        return "Circle";
    }
};

class Rectangle {
public:
    void draw() const {
        std::cout << "Drawing a rectangle" << std::endl;
    }
    
    std::string get_name() const {
        return "Rectangle";
    }
};

template<Drawable T>
void render(const T& shape) {
    std::cout << "Rendering: " << shape.get_name() << std::endl;
    shape.draw();
}

void example_custom_type_concept() {
    std::cout << "\n=== 11. CONCEPT FOR CUSTOM TYPES ===" << std::endl;
    
    Circle circle;
    Rectangle rect;
    
    render(circle);
    render(rect);
}

// ===================================================================
// 12. ADVANCED CONCEPT WITH NESTED REQUIREMENTS
// ===================================================================
template<typename T>
concept Arithmetic = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a - b } -> std::convertible_to<T>;
    { a * b } -> std::convertible_to<T>;
    { a / b } -> std::convertible_to<T>;
};

template<Arithmetic T>
class Calculator {
private:
    T value;
    
public:
    Calculator(T val) : value(val) {}
    
    T add(T other) { return value + other; }
    T subtract(T other) { return value - other; }
    T multiply(T other) { return value * other; }
    T divide(T other) { return value / other; }
    
    void print() const {
        std::cout << "Calculator value: " << value << std::endl;
    }
};

void example_arithmetic_concept() {
    std::cout << "\n=== 12. ADVANCED CONCEPT WITH NESTED REQUIREMENTS ===" << std::endl;
    
    Calculator<int> calc_int(100);
    calc_int.print();
    std::cout << "100 + 50 = " << calc_int.add(50) << std::endl;
    std::cout << "100 * 2 = " << calc_int.multiply(2) << std::endl;
    
    Calculator<double> calc_double(10.5);
    calc_double.print();
    std::cout << "10.5 / 2.0 = " << calc_double.divide(2.0) << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    C++20 CONCEPTS EXAMPLES" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    example_basic_concept();
    example_standard_concepts();
    example_compound_concepts();
    example_requires_clause();
    example_container_concept();
    example_concept_subsumption();
    example_sortable_concept();
    example_concept_class_template();
    example_logical_concepts();
    example_predicate_concept();
    example_custom_type_concept();
    example_arithmetic_concept();
    
    std::cout << "\n===============================================" << std::endl;
    std::cout << "    ALL EXAMPLES COMPLETED" << std::endl;
    std::cout << "===============================================\n" << std::endl;
    
    return 0;
}
