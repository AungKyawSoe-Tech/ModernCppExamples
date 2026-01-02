// ===================================================================
// COMPREHENSIVE TUPLES AND STRUCTURED BINDINGS
// ===================================================================
// This example demonstrates modern C++ tuple features and best practices.
//
// TOPICS COVERED:
// 1. Basic tuple operations (C++11)
// 2. std::tie and unpacking
// 3. Structured bindings (C++17)
// 4. std::apply (C++17)
// 5. Tuple comparison and algorithms
// 6. Tuple concatenation
// 7. std::tuple_cat
// 8. Real-world use cases
// 9. Performance considerations
// 10. When to use tuples vs structs
// ===================================================================

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>

// ===================================================================
// SECTION 1: BASIC TUPLE OPERATIONS
// ===================================================================

void demonstrate_basic_tuples() {
    std::cout << "\n=== 1. BASIC TUPLE OPERATIONS ===" << std::endl;
    
    // Creating tuples
    std::tuple<int, std::string, double> person1{25, "Alice", 5.6};
    auto person2 = std::make_tuple(30, "Bob", 6.0);
    
    // Accessing elements with std::get
    std::cout << "\nPerson 1: " << std::get<0>(person1) << " years, "
              << std::get<1>(person1) << ", " 
              << std::get<2>(person1) << "ft" << std::endl;
    
    // Accessing by type (C++14) - only if type is unique!
    std::cout << "Name: " << std::get<std::string>(person1) << std::endl;
    
    // Modifying elements
    std::get<0>(person1) = 26;
    std::cout << "After birthday: " << std::get<0>(person1) << " years" << std::endl;
    
    // Tuple size
    std::cout << "\nTuple size: " << std::tuple_size<decltype(person1)>::value << std::endl;
    
    std::cout << "\n💡 KEY POINTS:" << std::endl;
    std::cout << "   • std::get<N>(tuple) - access by index (compile-time)" << std::endl;
    std::cout << "   • std::get<Type>(tuple) - access by type (if unique)" << std::endl;
    std::cout << "   • Zero runtime overhead vs struct" << std::endl;
}

// ===================================================================
// SECTION 2: std::tie AND UNPACKING
// ===================================================================

std::tuple<int, int, int> divide_full(int dividend, int divisor) {
    return {dividend / divisor, dividend % divisor, dividend};
}

void demonstrate_tie() {
    std::cout << "\n=== 2. std::tie AND UNPACKING ===" << std::endl;
    
    // Old way: extract tuple elements
    auto result = divide_full(17, 5);
    int quotient = std::get<0>(result);
    int remainder = std::get<1>(result);
    int original = std::get<2>(result);
    
    std::cout << "\nOld way: 17 / 5 = " << quotient 
              << " remainder " << remainder << std::endl;
    
    // Better: std::tie (C++11)
    int q, r, o;
    std::tie(q, r, o) = divide_full(17, 5);
    std::cout << "With tie: 17 / 5 = " << q << " remainder " << r << std::endl;
    
    // Ignore some values with std::ignore
    std::tie(q, std::ignore, std::ignore) = divide_full(20, 3);
    std::cout << "Quotient only: 20 / 3 = " << q << std::endl;
    
    // Using tie for swap
    int a = 10, b = 20;
    std::cout << "\nBefore swap: a=" << a << ", b=" << b << std::endl;
    std::tie(a, b) = std::tie(b, a);
    std::cout << "After swap: a=" << a << ", b=" << b << std::endl;
    
    std::cout << "\n💡 std::tie benefits:" << std::endl;
    std::cout << "   • Unpack multiple return values" << std::endl;
    std::cout << "   • Use std::ignore for unwanted values" << std::endl;
    std::cout << "   • Create reference tuples" << std::endl;
}

// ===================================================================
// SECTION 3: STRUCTURED BINDINGS (C++17)
// ===================================================================

struct Sensor {
    int id;
    double temperature;
    double humidity;
    std::string location;
};

std::tuple<double, double, double> compute_statistics(const std::vector<double>& values) {
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double mean = sum / values.size();
    double min = *std::min_element(values.begin(), values.end());
    double max = *std::max_element(values.begin(), values.end());
    return {mean, min, max};
}

void demonstrate_structured_bindings() {
    std::cout << "\n=== 3. STRUCTURED BINDINGS (C++17) ===" << std::endl;
    
    // Structured bindings - most modern and readable!
    auto [quotient, remainder, original] = divide_full(25, 7);
    std::cout << "\n25 / 7 = " << quotient << " remainder " << remainder << std::endl;
    
    // Works with pairs
    std::map<std::string, int> scores{{"Alice", 95}, {"Bob", 87}};
    for (const auto& [name, score] : scores) {
        std::cout << name << ": " << score << " points" << std::endl;
    }
    
    // Works with structs
    Sensor sensor{101, 22.5, 65.0, "Lab-A"};
    auto [id, temp, humid, loc] = sensor;
    std::cout << "\nSensor " << id << " at " << loc 
              << ": " << temp << "°C, " << humid << "% humidity" << std::endl;
    
    // With functions
    std::vector<double> temps{20.1, 22.5, 21.8, 23.0, 19.5};
    auto [mean, min, max] = compute_statistics(temps);
    std::cout << "\nTemperature stats: mean=" << mean 
              << ", min=" << min << ", max=" << max << std::endl;
    
    std::cout << "\n💡 BEST PRACTICE:" << std::endl;
    std::cout << "   • Use structured bindings (C++17) over std::tie" << std::endl;
    std::cout << "   • More readable, less verbose" << std::endl;
    std::cout << "   • Works with tuples, pairs, structs, arrays" << std::endl;
}

// ===================================================================
// SECTION 4: std::apply (C++17)
// ===================================================================

int add_three(int a, int b, int c) {
    return a + b + c;
}

double multiply(double x, double y, double z) {
    return x * y * z;
}

void demonstrate_apply() {
    std::cout << "\n=== 4. std::apply (C++17) ===" << std::endl;
    
    // std::apply - call function with tuple as arguments
    std::tuple<int, int, int> args{10, 20, 30};
    int sum = std::apply(add_three, args);
    std::cout << "\nsum of (10, 20, 30) = " << sum << std::endl;
    
    std::tuple<double, double, double> factors{2.0, 3.0, 4.0};
    double product = std::apply(multiply, factors);
    std::cout << "product of (2.0, 3.0, 4.0) = " << product << std::endl;
    
    // With lambdas
    auto printer = [](auto... args) {
        std::cout << "Values: ";
        ((std::cout << args << " "), ...);
        std::cout << std::endl;
    };
    
    std::apply(printer, std::make_tuple(1, "hello", 3.14, 'X'));
    
    std::cout << "\n💡 std::apply use cases:" << std::endl;
    std::cout << "   • Call function with tuple arguments" << std::endl;
    std::cout << "   • Forward arguments stored in tuple" << std::endl;
    std::cout << "   • Unpack tuple into function call" << std::endl;
}

// ===================================================================
// SECTION 5: TUPLE COMPARISON AND ALGORITHMS
// ===================================================================

void demonstrate_tuple_comparison() {
    std::cout << "\n=== 5. TUPLE COMPARISON ===" << std::endl;
    
    std::tuple<int, std::string> t1{1, "apple"};
    std::tuple<int, std::string> t2{1, "banana"};
    std::tuple<int, std::string> t3{2, "apple"};
    
    // Lexicographic comparison (left to right)
    std::cout << "\nComparisons:" << std::endl;
    std::cout << "   (1, 'apple') < (1, 'banana'): " 
              << std::boolalpha << (t1 < t2) << std::endl;
    std::cout << "   (1, 'apple') < (2, 'apple'): " << (t1 < t3) << std::endl;
    std::cout << "   (1, 'banana') < (2, 'apple'): " << (t2 < t3) << std::endl;
    
    // Sorting with tuples
    std::vector<std::tuple<int, std::string, double>> students{
        {85, "Charlie", 3.2},
        {92, "Alice", 3.8},
        {85, "Bob", 3.5},
        {92, "David", 3.6}
    };
    
    std::cout << "\nBefore sort:" << std::endl;
    for (const auto& [score, name, gpa] : students) {
        std::cout << "   " << name << ": " << score << ", GPA " << gpa << std::endl;
    }
    
    // Sort by tuple (score desc, then name asc)
    std::sort(students.begin(), students.end(), 
              [](const auto& a, const auto& b) {
                  return std::tie(std::get<0>(b), std::get<1>(a)) < 
                         std::tie(std::get<0>(a), std::get<1>(b));
              });
    
    std::cout << "\nAfter sort (score desc, name asc):" << std::endl;
    for (const auto& [score, name, gpa] : students) {
        std::cout << "   " << name << ": " << score << ", GPA " << gpa << std::endl;
    }
    
    std::cout << "\n💡 Tuple comparison:" << std::endl;
    std::cout << "   • Lexicographic (element by element)" << std::endl;
    std::cout << "   • Perfect for sorting by multiple keys" << std::endl;
    std::cout << "   • Use std::tie for custom comparison" << std::endl;
}

// ===================================================================
// SECTION 6: std::tuple_cat AND CONCATENATION
// ===================================================================

void demonstrate_tuple_cat() {
    std::cout << "\n=== 6. std::tuple_cat ===" << std::endl;
    
    std::tuple<int, std::string> t1{42, "answer"};
    std::tuple<double, char> t2{3.14, 'X'};
    
    // Concatenate tuples
    auto combined = std::tuple_cat(t1, t2);
    auto [num, text, pi, letter] = combined;
    
    std::cout << "\nCombined tuple: " << num << ", " << text 
              << ", " << pi << ", " << letter << std::endl;
    
    // Combine multiple tuples
    std::tuple<int> id{100};
    std::tuple<std::string, std::string> name{"John", "Doe"};
    std::tuple<int> age{30};
    
    auto person = std::tuple_cat(id, name, age);
    std::cout << "\nPerson: ID=" << std::get<0>(person)
              << ", Name=" << std::get<1>(person) << " " << std::get<2>(person)
              << ", Age=" << std::get<3>(person) << std::endl;
    
    std::cout << "\n💡 std::tuple_cat:" << std::endl;
    std::cout << "   • Concatenate multiple tuples" << std::endl;
    std::cout << "   • All done at compile-time" << std::endl;
    std::cout << "   • Zero runtime overhead" << std::endl;
}

// ===================================================================
// SECTION 7: REAL-WORLD USE CASE - PARTICLE SYSTEM
// ===================================================================

using State = std::tuple<double, double, double, double, double>;
using ParticleWeight = double;
using Particle = std::map<ParticleWeight, State>;
using Particles = std::vector<Particle>;

double compute_total_weight(const Particles& particles) {
    double total = 0.0;
    for (const auto& particle : particles) {
        for (const auto& [weight, state] : particle) {
            total += weight;
        }
    }
    return total;
}

void print_particle_state(const State& state) {
    auto [x, y, z, vx, vy] = state;
    std::cout << "   Position(" << x << "," << y << "," << z << ") "
              << "Velocity(" << vx << "," << vy << ")" << std::endl;
}

void demonstrate_particle_system() {
    std::cout << "\n=== 7. REAL-WORLD: PARTICLE SYSTEM ===" << std::endl;
    
    Particles particles;
    
    // Create particles with weighted states
    for (int i = 0; i < 4; i++) {
        State state = std::make_tuple(i*1.0, i*1.1, i*1.2, i*0.5, i*0.6);
        Particle particle;
        particle.emplace(5.0 + i, state);
        particles.push_back(particle);
    }
    
    std::cout << "\nParticle states:" << std::endl;
    for (size_t i = 0; i < particles.size(); i++) {
        for (const auto& [weight, state] : particles[i]) {
            std::cout << "Particle " << i << " (weight=" << weight << "):" << std::endl;
            print_particle_state(state);
        }
    }
    
    double total_weight = compute_total_weight(particles);
    std::cout << "\nTotal weight: " << total_weight << std::endl;
    
    std::cout << "\n💡 Why use tuples here:" << std::endl;
    std::cout << "   • Simple data aggregation" << std::endl;
    std::cout << "   • No need for named struct" << std::endl;
    std::cout << "   • Easy to decompose with structured bindings" << std::endl;
}

// ===================================================================
// SECTION 8: TUPLE VS STRUCT - WHEN TO USE EACH
// ===================================================================

void explain_tuple_vs_struct() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "TUPLE VS STRUCT - WHEN TO USE EACH:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n✅ USE TUPLES WHEN:" << std::endl;
    std::cout << "\n1. RETURNING MULTIPLE VALUES:" << std::endl;
    std::cout << "   ✅ std::tuple<int, int> divide(int a, int b) { ... }" << std::endl;
    std::cout << "   • Quick and easy multiple returns" << std::endl;
    std::cout << "   • Use structured bindings to unpack" << std::endl;
    
    std::cout << "\n2. TEMPORARY DATA GROUPING:" << std::endl;
    std::cout << "   • Local scope only" << std::endl;
    std::cout << "   • No need for named type" << std::endl;
    std::cout << "   • One-off data combinations" << std::endl;
    
    std::cout << "\n3. GENERIC PROGRAMMING:" << std::endl;
    std::cout << "   • Template metaprogramming" << std::endl;
    std::cout << "   • Variadic template arguments" << std::endl;
    std::cout << "   • Type computations" << std::endl;
    
    std::cout << "\n4. QUICK PROTOTYPING:" << std::endl;
    std::cout << "   • Experimenting with data structures" << std::endl;
    std::cout << "   • Before defining proper types" << std::endl;
    
    std::cout << "\n✅ USE STRUCTS WHEN:" << std::endl;
    std::cout << "\n1. NAMED SEMANTICS MATTER:" << std::endl;
    std::cout << "   ✅ struct Person { int age; string name; double height; };" << std::endl;
    std::cout << "   • Clear what each field means" << std::endl;
    std::cout << "   • Self-documenting code" << std::endl;
    
    std::cout << "\n2. MEMBER FUNCTIONS NEEDED:" << std::endl;
    std::cout << "   • Methods to manipulate data" << std::endl;
    std::cout << "   • Encapsulation" << std::endl;
    std::cout << "   • Invariants to maintain" << std::endl;
    
    std::cout << "\n3. PUBLIC API:" << std::endl;
    std::cout << "   • Library interfaces" << std::endl;
    std::cout << "   • Long-term maintainability" << std::endl;
    std::cout << "   • Clear documentation" << std::endl;
    
    std::cout << "\n4. COMPLEX TYPES:" << std::endl;
    std::cout << "   • Many fields (>5)" << std::endl;
    std::cout << "   • Constructors needed" << std::endl;
    std::cout << "   • Relationships with other types" << std::endl;
    
    std::cout << "\n⚠️ ANTI-PATTERNS:" << std::endl;
    std::cout << "\n❌ DON'T: tuple<int, int, int, int, int, int, int>" << std::endl;
    std::cout << "   Too many fields - use struct!" << std::endl;
    std::cout << "\n❌ DON'T: Use tuples in public API" << std::endl;
    std::cout << "   Hard to understand: get<2>(result) means what?" << std::endl;
    std::cout << "\n❌ DON'T: Store tuples long-term" << std::endl;
    std::cout << "   Use proper named types for persistence" << std::endl;
    
    std::cout << "\n💡 GOLDEN RULE:" << std::endl;
    std::cout << "   Tuples for quick local returns," << std::endl;
    std::cout << "   Structs for everything else!" << std::endl;
}

// ===================================================================
// SECTION 9: PERFORMANCE CONSIDERATIONS
// ===================================================================

struct Point3D {
    double x, y, z;
};

void demonstrate_performance() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "PERFORMANCE CONSIDERATIONS:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    using TuplePoint = std::tuple<double, double, double>;
    
    std::cout << "\n📊 MEMORY LAYOUT:" << std::endl;
    std::cout << "   struct Point3D:  " << sizeof(Point3D) << " bytes" << std::endl;
    std::cout << "   tuple<d,d,d>:    " << sizeof(TuplePoint) << " bytes" << std::endl;
    std::cout << "   ✅ SAME SIZE - no overhead!" << std::endl;
    
    std::cout << "\n📊 RUNTIME PERFORMANCE:" << std::endl;
    std::cout << "   Struct: point.x (direct access)" << std::endl;
    std::cout << "   Tuple:  std::get<0>(point) (compile-time index)" << std::endl;
    std::cout << "   ✅ SAME SPEED - both inline to same code!" << std::endl;
    
    std::cout << "\n📊 COMPILATION TIME:" << std::endl;
    std::cout << "   Struct: Fast - simple type" << std::endl;
    std::cout << "   Tuple:  Slower - template instantiation" << std::endl;
    std::cout << "   ⚠️ Tuples increase compile time slightly" << std::endl;
    
    std::cout << "\n💡 PERFORMANCE TIPS:" << std::endl;
    std::cout << "   • Tuples have ZERO runtime overhead" << std::endl;
    std::cout << "   • std::get<N> is compile-time constant" << std::endl;
    std::cout << "   • Use references to avoid copies" << std::endl;
    std::cout << "   • Move semantics work perfectly" << std::endl;
}

// ===================================================================
// SECTION 10: ADVANCED TECHNIQUES
// ===================================================================

// Tuple element type at index
template<size_t I, typename Tuple>
using tuple_element_t = typename std::tuple_element<I, Tuple>::type;

void demonstrate_advanced() {
    std::cout << "\n=== 10. ADVANCED TECHNIQUES ===" << std::endl;
    
    // Type introspection
    using MyTuple = std::tuple<int, double, std::string>;
    
    std::cout << "\nTuple type information:" << std::endl;
    std::cout << "   Size: " << std::tuple_size_v<MyTuple> << std::endl;
    
    // Get element types
    using FirstType = tuple_element_t<0, MyTuple>;
    using SecondType = tuple_element_t<1, MyTuple>;
    
    std::cout << "   First element is int: " 
              << std::boolalpha << std::is_same_v<FirstType, int> << std::endl;
    std::cout << "   Second element is double: " 
              << std::is_same_v<SecondType, double> << std::endl;
    
    // Forward as tuple (perfect forwarding)
    auto forward_example = [](auto&&... args) {
        auto tuple = std::forward_as_tuple(args...);
        std::cout << "\n   Created reference tuple of size " 
                  << std::tuple_size_v<decltype(tuple)> << std::endl;
    };
    
    int x = 10;
    double y = 20.5;
    forward_example(x, y, std::string("test"));
    
    std::cout << "\n💡 Advanced features:" << std::endl;
    std::cout << "   • std::tuple_element - get element type" << std::endl;
    std::cout << "   • std::tuple_size - get tuple size" << std::endl;
    std::cout << "   • std::forward_as_tuple - create reference tuple" << std::endl;
    std::cout << "   • Perfect for template metaprogramming" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║         COMPREHENSIVE TUPLES AND STRUCTURED BINDINGS             ║\n";
    std::cout << "║                                                                  ║\n";
    std::cout << "║  Modern C++ techniques for heterogeneous data                   ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    demonstrate_basic_tuples();
    demonstrate_tie();
    demonstrate_structured_bindings();
    demonstrate_apply();
    demonstrate_tuple_comparison();
    demonstrate_tuple_cat();
    demonstrate_particle_system();
    explain_tuple_vs_struct();
    demonstrate_performance();
    demonstrate_advanced();
    
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "SUMMARY:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n✅ KEY TAKEAWAYS:" << std::endl;
    std::cout << "\n1. MODERN SYNTAX:" << std::endl;
    std::cout << "   • Prefer structured bindings (C++17) over std::tie" << std::endl;
    std::cout << "   • Use std::apply for tuple-to-function unpacking" << std::endl;
    std::cout << "   • auto [a, b, c] = func(); // Most readable!" << std::endl;
    
    std::cout << "\n2. WHEN TO USE:" << std::endl;
    std::cout << "   ✅ Multiple return values" << std::endl;
    std::cout << "   ✅ Temporary data grouping" << std::endl;
    std::cout << "   ✅ Generic programming" << std::endl;
    std::cout << "   ❌ Public APIs (use structs)" << std::endl;
    std::cout << "   ❌ Many fields (>5) (use structs)" << std::endl;
    
    std::cout << "\n3. PERFORMANCE:" << std::endl;
    std::cout << "   • Zero runtime overhead vs struct" << std::endl;
    std::cout << "   • std::get<N> is compile-time" << std::endl;
    std::cout << "   • Slightly slower compilation" << std::endl;
    
    std::cout << "\n4. BEST PRACTICES:" << std::endl;
    std::cout << "   • Keep tuples small (<= 5 elements)" << std::endl;
    std::cout << "   • Use for local/temporary data" << std::endl;
    std::cout << "   • Switch to struct when semantics matter" << std::endl;
    std::cout << "   • Document what tuple fields mean!" << std::endl;
    
    std::cout << "\n✅ Tuples: Quick and powerful for the right use cases!\n" << std::endl;
    
    return 0;
}
