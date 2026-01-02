// ===================================================================
// VARIADIC TEMPLATES WITH COMPILE-TIME RECURSION
// ===================================================================
// This example demonstrates how variadic templates enable compile-time
// recursion and metaprogramming, eliminating runtime overhead.
//
// TOPICS COVERED:
// 1. Basic variadic template recursion
// 2. Compile-time computation (constexpr)
// 3. Type manipulation at compile-time
// 4. Fold expressions (C++17)
// 5. Practical use cases
// 6. Embedded systems applications
//
// KEY CONCEPT: Template recursion happens during compilation,
// resulting in ZERO runtime cost - perfect for embedded systems!
// ===================================================================

#include <iostream>
#include <string>
#include <type_traits>
#include <array>
#include <tuple>
#include <cstdint>
#include <limits>
#include <bitset>

// ===================================================================
// SECTION 1: BASIC VARIADIC RECURSION - COMPILE-TIME
// ===================================================================

// Base case: no arguments
void print_recursive() {
    std::cout << std::endl;
}

// Recursive case: process first argument, recurse on rest
template<typename First, typename... Rest>
void print_recursive(First first, Rest... rest) {
    std::cout << first << " ";
    print_recursive(rest...);  // Compile-time recursion
}

void demonstrate_basic_recursion() {
    std::cout << "\n=== 1. BASIC VARIADIC RECURSION ===" << std::endl;
    
    std::cout << "Printing: ";
    print_recursive(1, "hello", 3.14, 'X', "world");
    
    std::cout << "\n💡 KEY POINT:" << std::endl;
    std::cout << "   • Compiler generates 5 functions at compile-time" << std::endl;
    std::cout << "   • Each function handles one argument type" << std::endl;
    std::cout << "   • Zero runtime overhead - all resolved at compile-time" << std::endl;
    std::cout << "   • No loops, no vtables, no dynamic dispatch" << std::endl;
}

// ===================================================================
// SECTION 2: COMPILE-TIME COMPUTATION - SUM
// ===================================================================

// Base case
constexpr int sum() {
    return 0;
}

// Recursive case
template<typename First, typename... Rest>
constexpr auto sum(First first, Rest... rest) {
    return first + sum(rest...);
}

// Compile-time maximum
constexpr int max_value() {
    return -2147483648;  // INT_MIN
}

template<typename First, typename... Rest>
constexpr auto max_value(First first, Rest... rest) {
    auto rest_max = max_value(rest...);
    return (first > rest_max) ? first : rest_max;
}

void demonstrate_compile_time_computation() {
    std::cout << "\n=== 2. COMPILE-TIME COMPUTATION ===" << std::endl;
    
    // These are computed at COMPILE TIME!
    constexpr int total = sum(1, 2, 3, 4, 5);
    constexpr int max_val = max_value(10, 25, 15, 30, 5);
    
    std::cout << "sum(1,2,3,4,5) = " << total << " (computed at compile-time!)" << std::endl;
    std::cout << "max(10,25,15,30,5) = " << max_val << " (computed at compile-time!)" << std::endl;
    
    std::cout << "\n💰 COST ANALYSIS:" << std::endl;
    std::cout << "   Runtime cost: ZERO" << std::endl;
    std::cout << "   Binary size: Just the constant values" << std::endl;
    std::cout << "   CPU cycles: ZERO (values are literals in binary)" << std::endl;
    
    // Verify with static_assert (compile-time only!)
    static_assert(sum(1, 2, 3, 4, 5) == 15, "Sum should be 15");
    static_assert(max_value(10, 25, 15, 30, 5) == 30, "Max should be 30");
    std::cout << "   ✅ static_assert passed - verified at compile-time!" << std::endl;
}

// ===================================================================
// SECTION 3: TYPE MANIPULATION - COMPILE-TIME
// ===================================================================

// Check if all types are integral
template<typename... Types>
struct all_integral;

// Base case: empty parameter pack
template<>
struct all_integral<> : std::true_type {};

// Recursive case
template<typename First, typename... Rest>
struct all_integral<First, Rest...> {
    static constexpr bool value = 
        std::is_integral_v<First> && all_integral<Rest...>::value;
};

// Helper variable template (C++17)
template<typename... Types>
inline constexpr bool all_integral_v = all_integral<Types...>::value;

// Check if all types are the same
template<typename... Types>
struct all_same;

template<typename T>
struct all_same<T> : std::true_type {};

template<typename First, typename Second, typename... Rest>
struct all_same<First, Second, Rest...> {
    static constexpr bool value = 
        std::is_same_v<First, Second> && all_same<Second, Rest...>::value;
};

template<typename... Types>
inline constexpr bool all_same_v = all_same<Types...>::value;

// Get size of largest type
template<typename... Types>
struct max_sizeof;

template<typename T>
struct max_sizeof<T> {
    static constexpr size_t value = sizeof(T);
};

template<typename First, typename... Rest>
struct max_sizeof<First, Rest...> {
    static constexpr size_t value = 
        (sizeof(First) > max_sizeof<Rest...>::value) 
            ? sizeof(First) 
            : max_sizeof<Rest...>::value;
};

template<typename... Types>
inline constexpr size_t max_sizeof_v = max_sizeof<Types...>::value;

void demonstrate_type_manipulation() {
    std::cout << "\n=== 3. TYPE MANIPULATION AT COMPILE-TIME ===" << std::endl;
    
    std::cout << "\nall_integral_v:" << std::endl;
    std::cout << "   <int, long, short> = " 
              << std::boolalpha << all_integral_v<int, long, short> << std::endl;
    std::cout << "   <int, double, char> = " 
              << all_integral_v<int, double, char> << std::endl;
    
    std::cout << "\nall_same_v:" << std::endl;
    std::cout << "   <int, int, int> = " 
              << all_same_v<int, int, int> << std::endl;
    std::cout << "   <int, long, int> = " 
              << all_same_v<int, long, int> << std::endl;
    
    std::cout << "\nmax_sizeof_v:" << std::endl;
    std::cout << "   <char, int, long> = " 
              << max_sizeof_v<char, int, long> << " bytes" << std::endl;
    std::cout << "   <double, int, float> = " 
              << max_sizeof_v<double, int, float> << " bytes" << std::endl;
    
    std::cout << "\n💡 ALL COMPUTED AT COMPILE-TIME!" << std::endl;
    std::cout << "   No runtime type checking needed" << std::endl;
    std::cout << "   Perfect for template constraints" << std::endl;
}

// ===================================================================
// SECTION 4: FOLD EXPRESSIONS (C++17) - SIMPLIFIED RECURSION
// ===================================================================

// Old way: explicit recursion
template<typename... Args>
constexpr auto sum_old_way(Args... args) {
    return (args + ...);  // Fold expression!
}

// Fold expressions for logical operations
template<typename... Args>
constexpr bool all_positive(Args... args) {
    return ((args > 0) && ...);  // Fold with &&
}

template<typename... Args>
constexpr bool any_negative(Args... args) {
    return ((args < 0) || ...);  // Fold with ||
}

// Fold for comma operator (call function for each)
template<typename... Args>
void print_all_fold(Args... args) {
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
}

void demonstrate_fold_expressions() {
    std::cout << "\n=== 4. FOLD EXPRESSIONS (C++17) ===" << std::endl;
    
    std::cout << "\nArithmetic folds:" << std::endl;
    std::cout << "   sum(1,2,3,4,5) = " << sum_old_way(1, 2, 3, 4, 5) << std::endl;
    
    std::cout << "\nLogical folds:" << std::endl;
    std::cout << "   all_positive(1,2,3) = " 
              << std::boolalpha << all_positive(1, 2, 3) << std::endl;
    std::cout << "   all_positive(1,-2,3) = " 
              << all_positive(1, -2, 3) << std::endl;
    std::cout << "   any_negative(1,2,3) = " 
              << any_negative(1, 2, 3) << std::endl;
    std::cout << "   any_negative(1,-2,3) = " 
              << any_negative(1, -2, 3) << std::endl;
    
    std::cout << "\nPrint with fold: ";
    print_all_fold(1, "hello", 3.14, "world");
    
    std::cout << "\n💡 FOLD EXPRESSIONS:" << std::endl;
    std::cout << "   • Simpler than explicit recursion" << std::endl;
    std::cout << "   • Still compile-time only" << std::endl;
    std::cout << "   • More readable code" << std::endl;
    std::cout << "   • Zero runtime cost" << std::endl;
}

// ===================================================================
// SECTION 5: PRACTICAL USE CASE - TYPE-SAFE PRINTF
// ===================================================================

// Validate format string at compile-time
template<typename... Args>
void safe_printf(const char* format, Args... args) {
    // Count format specifiers
    int format_count = 0;
    for (const char* p = format; *p; ++p) {
        if (*p == '%' && *(p+1) != '%') {
            ++format_count;
        }
    }
    
    int arg_count = sizeof...(args);
    
    if (format_count != arg_count) {
        std::cout << "ERROR: Format string has " << format_count 
                  << " specifiers but " << arg_count << " arguments!" << std::endl;
        return;
    }
    
    printf(format, args...);
}

void demonstrate_safe_printf() {
    std::cout << "\n=== 5. TYPE-SAFE PRINTF ===" << std::endl;
    
    std::cout << "\n✅ Correct usage:" << std::endl;
    safe_printf("Integer: %d, String: %s, Float: %.2f\n", 42, "hello", 3.14);
    
    std::cout << "\n❌ Incorrect usage (caught at runtime):" << std::endl;
    safe_printf("Two specifiers: %d %s\n", 42);  // Missing argument!
    
    std::cout << "\n💡 BETTER APPROACH:" << std::endl;
    std::cout << "   Use C++20 std::format for complete type safety!" << std::endl;
    std::cout << "   Or compile-time format string validation" << std::endl;
}

// ===================================================================
// SECTION 6: COMPILE-TIME ARRAY INITIALIZATION
// ===================================================================

// Create array with compile-time computed values
template<typename... Args>
constexpr auto make_array(Args... args) {
    return std::array<std::common_type_t<Args...>, sizeof...(Args)>{args...};
}

// Fibonacci at compile-time
constexpr int fib(int n) {
    return (n <= 1) ? n : fib(n-1) + fib(n-2);
}

// Generate array of first N fibonacci numbers
template<size_t... Is>
constexpr auto fib_array_impl(std::index_sequence<Is...>) {
    return std::array<int, sizeof...(Is)>{fib(Is)...};
}

template<size_t N>
constexpr auto fib_array() {
    return fib_array_impl(std::make_index_sequence<N>{});
}

void demonstrate_compile_time_array() {
    std::cout << "\n=== 6. COMPILE-TIME ARRAY INITIALIZATION ===" << std::endl;
    
    // Array created at compile-time!
    constexpr auto arr = make_array(10, 20, 30, 40, 50);
    std::cout << "\nArray: ";
    for (auto val : arr) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // Fibonacci array computed at compile-time!
    constexpr auto fibs = fib_array<10>();
    std::cout << "\nFirst 10 Fibonacci numbers (compile-time): ";
    for (auto val : fibs) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\n💰 PERFORMANCE:" << std::endl;
    std::cout << "   • Arrays are in .rodata section (read-only)" << std::endl;
    std::cout << "   • No initialization code at runtime" << std::endl;
    std::cout << "   • Values embedded in binary" << std::endl;
}

// ===================================================================
// SECTION 7: EMBEDDED SYSTEMS - REGISTER CONFIGURATION
// ===================================================================

// Compile-time register bit field computation
template<uint32_t... Bits>
struct RegisterBits {
    static constexpr uint32_t value = ((1u << Bits) | ...);
};

// Compile-time register configuration
enum class GPIO_Pin : uint8_t {
    Pin0 = 0, Pin1 = 1, Pin2 = 2, Pin3 = 3,
    Pin4 = 4, Pin5 = 5, Pin6 = 6, Pin7 = 7
};

template<GPIO_Pin... Pins>
struct GPIO_Config {
    static constexpr uint8_t mask = ((1u << static_cast<uint8_t>(Pins)) | ...);
    
    static void set_output() {
        // This would write to actual hardware register
        std::cout << "   Setting pins 0x" << std::hex 
                  << static_cast<int>(mask) << std::dec 
                  << " as output" << std::endl;
    }
};

// Compile-time validation of peripheral configuration
template<uint32_t ClockFreq, uint32_t DesiredBaud>
struct UART_Divider {
    static constexpr uint32_t divider = ClockFreq / (16 * DesiredBaud);
    static constexpr uint32_t actual_baud = ClockFreq / (16 * divider);
    static constexpr uint32_t error_percent = 
        (actual_baud > DesiredBaud) 
            ? ((actual_baud - DesiredBaud) * 100 / DesiredBaud)
            : ((DesiredBaud - actual_baud) * 100 / DesiredBaud);
    
    static_assert(divider > 0 && divider < 65536, 
                  "UART divider out of range!");
    static_assert(error_percent < 3,
                  "Baud rate error exceeds 3%!");
};

void demonstrate_embedded_usage() {
    std::cout << "\n=== 7. EMBEDDED SYSTEMS APPLICATIONS ===" << std::endl;
    
    std::cout << "\n📟 Register bit configuration (compile-time):" << std::endl;
    constexpr uint32_t bits = RegisterBits<0, 2, 4, 7>::value;
    std::cout << "   Bits 0,2,4,7 = 0x" << std::hex << bits << std::dec 
              << " (0b" << std::bitset<8>(bits) << ")" << std::endl;
    
    std::cout << "\n📟 GPIO configuration (compile-time):" << std::endl;
    using MyGPIO = GPIO_Config<GPIO_Pin::Pin0, GPIO_Pin::Pin3, GPIO_Pin::Pin7>;
    std::cout << "   GPIO mask: 0x" << std::hex 
              << static_cast<int>(MyGPIO::mask) << std::dec << std::endl;
    MyGPIO::set_output();
    
    std::cout << "\n📟 UART configuration (compile-time validated):" << std::endl;
    using UART = UART_Divider<16000000, 9600>;
    std::cout << "   Clock: 16MHz, Baud: 9600" << std::endl;
    std::cout << "   Divider: " << UART::divider 
              << " (computed at compile-time)" << std::endl;
    
    std::cout << "\n💡 EMBEDDED BENEFITS:" << std::endl;
    std::cout << "   ✅ Configuration errors caught at compile-time" << std::endl;
    std::cout << "   ✅ Zero runtime overhead" << std::endl;
    std::cout << "   ✅ No magic numbers - everything is named" << std::endl;
    std::cout << "   ✅ Hardware constraints validated at compile-time" << std::endl;
}

// ===================================================================
// SECTION 8: TUPLE OPERATIONS - COMPILE-TIME RECURSION
// ===================================================================

// Print tuple recursively at compile-time
template<size_t Index = 0, typename... Types>
void print_tuple(const std::tuple<Types...>& t) {
    if constexpr (Index < sizeof...(Types)) {
        std::cout << std::get<Index>(t);
        if constexpr (Index + 1 < sizeof...(Types)) {
            std::cout << ", ";
        }
        print_tuple<Index + 1>(t);
    }
}

// Sum all tuple elements (if numeric)
template<size_t Index = 0, typename... Types>
auto sum_tuple(const std::tuple<Types...>& t) {
    if constexpr (Index < sizeof...(Types)) {
        if constexpr (Index + 1 < sizeof...(Types)) {
            return std::get<Index>(t) + sum_tuple<Index + 1>(t);
        } else {
            return std::get<Index>(t);
        }
    } else {
        return 0;
    }
}

void demonstrate_tuple_operations() {
    std::cout << "\n=== 8. TUPLE OPERATIONS WITH RECURSION ===" << std::endl;
    
    auto mixed_tuple = std::make_tuple(1, "hello", 3.14, 'X');
    std::cout << "\nTuple: ";
    print_tuple(mixed_tuple);
    std::cout << std::endl;
    
    auto numeric_tuple = std::make_tuple(10, 20, 30, 40, 50);
    std::cout << "\nNumeric tuple: ";
    print_tuple(numeric_tuple);
    std::cout << std::endl;
    
    auto total = sum_tuple(numeric_tuple);
    std::cout << "Sum: " << total << std::endl;
    
    std::cout << "\n💡 if constexpr:" << std::endl;
    std::cout << "   • Compile-time conditional" << std::endl;
    std::cout << "   • Dead branches eliminated by compiler" << std::endl;
    std::cout << "   • Perfect for template recursion termination" << std::endl;
}

// ===================================================================
// SECTION 9: WHEN TO USE COMPILE-TIME RECURSION
// ===================================================================

void explain_when_to_use() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "WHEN TO USE COMPILE-TIME RECURSION:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n✅ USE FOR:" << std::endl;
    std::cout << "\n1. TYPE COMPUTATIONS:" << std::endl;
    std::cout << "   • Finding largest type in parameter pack" << std::endl;
    std::cout << "   • Type trait combinations" << std::endl;
    std::cout << "   • Template metaprogramming" << std::endl;
    
    std::cout << "\n2. COMPILE-TIME CONSTANTS:" << std::endl;
    std::cout << "   • Mathematical computations (Fibonacci, factorial)" << std::endl;
    std::cout << "   • Configuration values" << std::endl;
    std::cout << "   • Lookup tables" << std::endl;
    
    std::cout << "\n3. HETEROGENEOUS COLLECTIONS:" << std::endl;
    std::cout << "   • Tuple operations" << std::endl;
    std::cout << "   • Variant handling" << std::endl;
    std::cout << "   • Type-safe function wrappers" << std::endl;
    
    std::cout << "\n4. EMBEDDED SYSTEMS:" << std::endl;
    std::cout << "   • Hardware register configuration" << std::endl;
    std::cout << "   • Peripheral setup validation" << std::endl;
    std::cout << "   • Zero-overhead abstractions" << std::endl;
    std::cout << "   • Compile-time constraint checking" << std::endl;
    
    std::cout << "\n5. CODE GENERATION:" << std::endl;
    std::cout << "   • Unrolling loops at compile-time" << std::endl;
    std::cout << "   • Generating specialized functions" << std::endl;
    std::cout << "   • Avoiding runtime dispatch" << std::endl;
    
    std::cout << "\n❌ DON'T USE FOR:" << std::endl;
    std::cout << "\n1. RUNTIME DATA:" << std::endl;
    std::cout << "   • User input processing" << std::endl;
    std::cout << "   • Dynamic collections" << std::endl;
    std::cout << "   • Data-dependent logic" << std::endl;
    
    std::cout << "\n2. EXCESSIVE RECURSION:" << std::endl;
    std::cout << "   • Deep recursion slows compilation" << std::endl;
    std::cout << "   • Can cause compiler errors (recursion limits)" << std::endl;
    std::cout << "   • Use fold expressions when possible" << std::endl;
    
    std::cout << "\n3. SIMPLE CASES:" << std::endl;
    std::cout << "   • Use standard library when available" << std::endl;
    std::cout << "   • Don't reinvent the wheel" << std::endl;
    
    std::cout << "\n💡 MODERN C++ ALTERNATIVES:" << std::endl;
    std::cout << "   • C++17: Fold expressions (replace simple recursion)" << std::endl;
    std::cout << "   • C++17: if constexpr (cleaner recursion)" << std::endl;
    std::cout << "   • C++20: Concepts (constrain templates)" << std::endl;
    std::cout << "   • C++20: consteval (force compile-time evaluation)" << std::endl;
}

// ===================================================================
// SECTION 10: PERFORMANCE COMPARISON
// ===================================================================

// Runtime version
int sum_runtime(int* values, size_t count) {
    int total = 0;
    for (size_t i = 0; i < count; ++i) {
        total += values[i];
    }
    return total;
}

void demonstrate_performance_comparison() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "PERFORMANCE: COMPILE-TIME VS RUNTIME:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n📊 COMPILE-TIME (variadic template):" << std::endl;
    constexpr int compile_time_sum = sum(1, 2, 3, 4, 5);
    std::cout << "   Result: " << compile_time_sum << std::endl;
    std::cout << "   Binary: MOV EAX, 15  (single instruction!)" << std::endl;
    std::cout << "   Cost: 0 CPU cycles at runtime" << std::endl;
    std::cout << "   Size: ~4 bytes (just the constant)" << std::endl;
    
    std::cout << "\n📊 RUNTIME (loop):" << std::endl;
    int values[] = {1, 2, 3, 4, 5};
    int runtime_sum = sum_runtime(values, 5);
    std::cout << "   Result: " << runtime_sum << std::endl;
    std::cout << "   Binary: ~20+ instructions (loop, increment, add)" << std::endl;
    std::cout << "   Cost: ~10+ CPU cycles" << std::endl;
    std::cout << "   Size: ~40+ bytes (loop code)" << std::endl;
    
    std::cout << "\n🚀 SPEEDUP: Infinite (0 vs ~10 cycles)" << std::endl;
    std::cout << "📉 SIZE: 10x smaller (4 vs 40+ bytes)" << std::endl;
    
    std::cout << "\n💡 WHEN COMPILE-TIME WINS:" << std::endl;
    std::cout << "   • Values known at compile-time" << std::endl;
    std::cout << "   • Constant configuration" << std::endl;
    std::cout << "   • Type computations" << std::endl;
    std::cout << "   • Safety-critical embedded systems" << std::endl;
}

// ===================================================================
// SECTION 11: DRAWBACKS AND PITFALLS
// ===================================================================

// Pitfall 1: Deep recursion causes slow compilation
template<int N>
struct Fibonacci {
    static constexpr int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
};

template<> struct Fibonacci<0> { static constexpr int value = 0; };
template<> struct Fibonacci<1> { static constexpr int value = 1; };

// Pitfall 2: Exponential template instantiations
template<typename... Args>
struct InstantiationExplosion {
    // Each call creates multiple instantiations
    using type = std::tuple<Args..., Args...>;  // Doubles the types!
};

// Pitfall 3: Cryptic error messages
template<typename... Args>
constexpr auto bad_function(Args... args) {
    // This will produce a HORRIBLE error message if types don't support operator+
    return (args + ...);
}

// Pitfall 4: Code bloat - each instantiation generates code
template<typename T>
T generic_sort(T value) {
    // Even though this does nothing different, compiler generates
    // separate code for EACH type instantiation
    std::cout << "Sorting single value: " << value << std::endl;
    return value;
}

// Pitfall 5: Hidden recursion limits
template<int N>
struct DeepRecursion {
    static constexpr int value = DeepRecursion<N-1>::value + 1;
};
template<> struct DeepRecursion<0> { static constexpr int value = 0; };

void demonstrate_drawbacks_and_pitfalls() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "DRAWBACKS AND POTENTIAL PITFALLS:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n⚠️ PITFALL 1: SLOW COMPILATION" << std::endl;
    std::cout << "   Problem: Deep template recursion is SLOW to compile" << std::endl;
    std::cout << "   Example: Fibonacci<40>::value takes FOREVER to compile!" << std::endl;
    
    // Safe to compute small values
    constexpr int fib10 = Fibonacci<10>::value;
    std::cout << "   Fibonacci<10> = " << fib10 << " (tolerable)" << std::endl;
    
    // Fibonacci<40> would take minutes to compile!
    // constexpr int fib40 = Fibonacci<40>::value;  // DON'T DO THIS!
    
    std::cout << "\n   💡 Solution:" << std::endl;
    std::cout << "      • Use constexpr functions instead of template recursion" << std::endl;
    std::cout << "      • constexpr int fib(int n) { return (n<=1) ? n : fib(n-1)+fib(n-2); }" << std::endl;
    std::cout << "      • Much faster compilation, same runtime performance" << std::endl;
    
    std::cout << "\n⚠️ PITFALL 2: CRYPTIC ERROR MESSAGES" << std::endl;
    std::cout << "   Problem: Template errors are EXTREMELY hard to read" << std::endl;
    std::cout << "   Example error (trying to add strings with +):" << std::endl;
    std::cout << "      error: invalid operands to binary expression" << std::endl;
    std::cout << "      ('std::string' and 'std::string')" << std::endl;
    std::cout << "      in instantiation of function template specialization" << std::endl;
    std::cout << "      'bad_function<std::string, std::string>' requested here" << std::endl;
    std::cout << "      ... 50 more lines of template instantiation backtrace ..." << std::endl;
    
    std::cout << "\n   💡 Solution:" << std::endl;
    std::cout << "      • Use C++20 concepts to constrain templates EARLY" << std::endl;
    std::cout << "      • Add static_assert with clear messages" << std::endl;
    std::cout << "      • Use std::enable_if with meaningful names" << std::endl;
    
    std::cout << "\n⚠️ PITFALL 3: CODE BLOAT" << std::endl;
    std::cout << "   Problem: Each template instantiation generates SEPARATE code" << std::endl;
    
    // Each call generates separate function in binary
    generic_sort(10);
    generic_sort(20.5);
    generic_sort(std::string("hello"));
    
    std::cout << "\n   Impact on binary size:" << std::endl;
    std::cout << "      • generic_sort<int>     : ~50 bytes" << std::endl;
    std::cout << "      • generic_sort<double>  : ~50 bytes" << std::endl;
    std::cout << "      • generic_sort<string>  : ~80 bytes" << std::endl;
    std::cout << "      • Total                 : ~180 bytes for same logic!" << std::endl;
    
    std::cout << "\n   💡 Solution:" << std::endl;
    std::cout << "      • Use type erasure for runtime polymorphism" << std::endl;
    std::cout << "      • Extract common code into non-template functions" << std::endl;
    std::cout << "      • Use extern template to prevent duplicate instantiations" << std::endl;
    
    std::cout << "\n⚠️ PITFALL 4: COMPILER RECURSION LIMITS" << std::endl;
    std::cout << "   Problem: Compilers have hard limits on template depth" << std::endl;
    std::cout << "   Typical limit: 256-1024 instantiation depth" << std::endl;
    
    // This works (small depth)
    constexpr int depth_100 = DeepRecursion<100>::value;
    std::cout << "   DeepRecursion<100> = " << depth_100 << " (works)" << std::endl;
    
    // DeepRecursion<2000> would hit the limit!
    // constexpr int depth_2000 = DeepRecursion<2000>::value;  // ERROR!
    
    std::cout << "\n   Error you'd see:" << std::endl;
    std::cout << "      fatal error: recursive template instantiation exceeded" << std::endl;
    std::cout << "      maximum depth of 1024" << std::endl;
    
    std::cout << "\n   💡 Solution:" << std::endl;
    std::cout << "      • Use constexpr functions instead" << std::endl;
    std::cout << "      • Use fold expressions (no recursion)" << std::endl;
    std::cout << "      • Increase limit: -ftemplate-depth=2000 (not recommended!)" << std::endl;
    
    std::cout << "\n⚠️ PITFALL 5: DEBUGGING NIGHTMARE" << std::endl;
    std::cout << "   Problem: Can't step through template code" << std::endl;
    std::cout << "   • Debugger shows instantiated code, not template" << std::endl;
    std::cout << "   • Type names are mangled and unreadable" << std::endl;
    std::cout << "   • Call stack filled with template instantiation frames" << std::endl;
    
    std::cout << "\n   💡 Solution:" << std::endl;
    std::cout << "      • Use std::cout for compile-time debugging" << std::endl;
    std::cout << "      • Use static_assert to validate assumptions" << std::endl;
    std::cout << "      • Test with simple types first" << std::endl;
    std::cout << "      • Use __PRETTY_FUNCTION__ to see instantiated types" << std::endl;
    
    std::cout << "\n⚠️ PITFALL 6: EXCESSIVE MEMORY DURING COMPILATION" << std::endl;
    std::cout << "   Problem: Complex templates use LOTS of compiler memory" << std::endl;
    std::cout << "   Impact:" << std::endl;
    std::cout << "      • Simple project: ~500MB compiler memory" << std::endl;
    std::cout << "      • Heavy template use: 4GB+ compiler memory" << std::endl;
    std::cout << "      • Can cause out-of-memory errors in CI/CD" << std::endl;
    
    std::cout << "\n   💡 Solution:" << std::endl;
    std::cout << "      • Split template-heavy code across files" << std::endl;
    std::cout << "      • Use forward declarations" << std::endl;
    std::cout << "      • Use precompiled headers for common instantiations" << std::endl;
    std::cout << "      • Consider compilation time in design decisions" << std::endl;
}

// ===================================================================
// SECTION 12: REAL-WORLD EXAMPLES OF THINGS GOING WRONG
// ===================================================================

// Example 1: Accidental O(2^N) compilation complexity
template<int N, typename... Args>
struct BadMetaFunction {
    // Each level DOUBLES the work - exponential explosion!
    using result = typename BadMetaFunction<N-1, Args..., Args...>::result;
};

template<typename... Args>
struct BadMetaFunction<0, Args...> {
    using result = std::tuple<Args...>;
};

// Example 2: Forgetting base case - infinite recursion
// template<typename T, typename... Rest>
// struct ForgetBaseCase {
//     static constexpr int value = 1 + ForgetBaseCase<Rest...>::value;
//     // OOPS! No base case for empty pack!
// };

// Example 3: Ambiguous overload resolution
template<typename T>
void overload_problem(T value) {
    std::cout << "Generic version" << std::endl;
}

template<typename T, typename... Args>
void overload_problem(T first, Args... rest) {
    std::cout << "Variadic version" << std::endl;
}

// overload_problem(42);  // Ambiguous! Which one to call?

// Example 4: Type deduction failure
template<typename... Args>
auto broken_deduction(Args... args) {
    // This fails if args is empty!
    // auto first = args...;  // ERROR: can't expand to nothing
    return 0;
}

void demonstrate_real_world_failures() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "REAL-WORLD EXAMPLES OF FAILURES:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n💥 FAILURE 1: EXPONENTIAL COMPILATION TIME" << std::endl;
    std::cout << "   Code: BadMetaFunction<N, Types...>" << std::endl;
    std::cout << "   Problem: Each recursion DOUBLES the type list" << std::endl;
    std::cout << "   Complexity: O(2^N) template instantiations!" << std::endl;
    std::cout << "   Result:" << std::endl;
    std::cout << "      • N=5:  32 instantiations   (0.1s)" << std::endl;
    std::cout << "      • N=10: 1024 instantiations (5s)" << std::endl;
    std::cout << "      • N=15: 32768 instantiations (MINUTES!)" << std::endl;
    std::cout << "      • N=20: Out of memory / compiler crash" << std::endl;
    
    std::cout << "\n💥 FAILURE 2: MISSING BASE CASE" << std::endl;
    std::cout << "   Error message:" << std::endl;
    std::cout << "      fatal error: template instantiation depth exceeds maximum" << std::endl;
    std::cout << "      note: use -ftemplate-depth= to increase the maximum" << std::endl;
    std::cout << "   Root cause: Forgot to specialize for empty parameter pack" << std::endl;
    std::cout << "   Fix: Always provide template<> specialization" << std::endl;
    
    std::cout << "\n💥 FAILURE 3: AMBIGUOUS OVERLOADS" << std::endl;
    std::cout << "   Problem: overload_problem(42)" << std::endl;
    std::cout << "   Both match:" << std::endl;
    std::cout << "      • void overload_problem(T)           // T=int" << std::endl;
    std::cout << "      • void overload_problem(T, Args...)  // T=int, Args=<>" << std::endl;
    std::cout << "   Solution: Use SFINAE or concepts to disambiguate" << std::endl;
    
    std::cout << "\n💥 FAILURE 4: EMPTY PARAMETER PACK" << std::endl;
    std::cout << "   Problem: broken_deduction() called with no arguments" << std::endl;
    std::cout << "   Can't expand empty pack in certain contexts" << std::endl;
    std::cout << "   Solution: Always check sizeof...(Args) or provide base case" << std::endl;
    
    std::cout << "\n💥 FAILURE 5: TYPE DEDUCTION CONFUSION" << std::endl;
    std::cout << "   Problem: auto deduction with parameter packs" << std::endl;
    std::cout << "   Example:" << std::endl;
    std::cout << "      template<typename... Args>" << std::endl;
    std::cout << "      auto func(Args... args) {" << std::endl;
    std::cout << "          return args;  // Which one? ERROR!" << std::endl;
    std::cout << "      }" << std::endl;
    std::cout << "   Solution: Be explicit about which argument to return" << std::endl;
}

// ===================================================================
// SECTION 13: BEST PRACTICES TO AVOID PITFALLS
// ===================================================================

void explain_best_practices() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "BEST PRACTICES TO AVOID PITFALLS:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n✅ DO:" << std::endl;
    
    std::cout << "\n1. USE FOLD EXPRESSIONS (C++17+):" << std::endl;
    std::cout << "   ❌ BAD:  Explicit recursion for sum" << std::endl;
    std::cout << "      template<typename T>" << std::endl;
    std::cout << "      T sum(T t) { return t; }" << std::endl;
    std::cout << "      template<typename T, typename... Args>" << std::endl;
    std::cout << "      T sum(T t, Args... args) { return t + sum(args...); }" << std::endl;
    std::cout << "\n   ✅ GOOD: Fold expression" << std::endl;
    std::cout << "      template<typename... Args>" << std::endl;
    std::cout << "      auto sum(Args... args) { return (args + ...); }" << std::endl;
    
    std::cout << "\n2. USE CONSTEXPR FUNCTIONS OVER TEMPLATE RECURSION:" << std::endl;
    std::cout << "   ❌ BAD:  template<int N> struct Fib { ... };" << std::endl;
    std::cout << "   ✅ GOOD: constexpr int fib(int n) { ... }" << std::endl;
    std::cout << "   Benefits: Faster compilation, clearer code, easier to debug" << std::endl;
    
    std::cout << "\n3. USE CONCEPTS TO CONSTRAIN EARLY (C++20):" << std::endl;
    std::cout << "   ❌ BAD:  template<typename T> void func(T t) { t + t; }" << std::endl;
    std::cout << "   ✅ GOOD: template<std::integral T> void func(T t) { t + t; }" << std::endl;
    std::cout << "   Benefits: Clear error messages, compile-time validation" << std::endl;
    
    std::cout << "\n4. ALWAYS PROVIDE BASE CASE:" << std::endl;
    std::cout << "   ❌ BAD:  Only recursive case" << std::endl;
    std::cout << "   ✅ GOOD: template<> struct Base { }; + recursive case" << std::endl;
    std::cout << "   Benefits: Avoid infinite recursion errors" << std::endl;
    
    std::cout << "\n5. USE static_assert WITH CLEAR MESSAGES:" << std::endl;
    std::cout << "   ✅ GOOD: static_assert(sizeof...(Args) > 0, \"Need at least one argument\");" << std::endl;
    std::cout << "   Benefits: Catch errors early with helpful messages" << std::endl;
    
    std::cout << "\n6. TEST WITH SIMPLE TYPES FIRST:" << std::endl;
    std::cout << "   • Start with int, double before complex types" << std::endl;
    std::cout << "   • Verify logic works before adding complexity" << std::endl;
    std::cout << "   • Use small parameter counts during development" << std::endl;
    
    std::cout << "\n7. MEASURE COMPILATION TIME:" << std::endl;
    std::cout << "   • Use -ftime-report (GCC/Clang)" << std::endl;
    std::cout << "   • Track build times in CI/CD" << std::endl;
    std::cout << "   • Set reasonable limits (e.g., <10s per file)" << std::endl;
    
    std::cout << "\n8. DOCUMENT CONSTRAINTS:" << std::endl;
    std::cout << "   • Specify required type traits" << std::endl;
    std::cout << "   • Document recursion depth limits" << std::endl;
    std::cout << "   • Provide usage examples" << std::endl;
    
    std::cout << "\n❌ DON'T:" << std::endl;
    std::cout << "\n1. DON'T use template recursion for deep computations" << std::endl;
    std::cout << "2. DON'T instantiate with many types unless necessary" << std::endl;
    std::cout << "3. DON'T ignore compilation time during development" << std::endl;
    std::cout << "4. DON'T use variadic templates for runtime data" << std::endl;
    std::cout << "5. DON'T nest template recursion deeply" << std::endl;
    
    std::cout << "\n💡 GOLDEN RULE:" << std::endl;
    std::cout << "   If it takes more than 1 second to compile," << std::endl;
    std::cout << "   you're probably doing it wrong!" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║       VARIADIC TEMPLATES & COMPILE-TIME RECURSION               ║\n";
    std::cout << "║                                                                  ║\n";
    std::cout << "║  Zero runtime cost • Type safety • Perfect for embedded        ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    demonstrate_basic_recursion();
    demonstrate_compile_time_computation();
    demonstrate_type_manipulation();
    demonstrate_fold_expressions();
    demonstrate_safe_printf();
    demonstrate_compile_time_array();
    demonstrate_embedded_usage();
    demonstrate_tuple_operations();
    explain_when_to_use();
    demonstrate_performance_comparison();
    demonstrate_drawbacks_and_pitfalls();
    demonstrate_real_world_failures();
    explain_best_practices();
    
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "SUMMARY:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n✅ KEY TAKEAWAYS:" << std::endl;
    std::cout << "\n1. COMPILE-TIME RECURSION:" << std::endl;
    std::cout << "   • All computation happens during compilation" << std::endl;
    std::cout << "   • Zero runtime cost" << std::endl;
    std::cout << "   • Results embedded in binary as constants" << std::endl;
    
    std::cout << "\n2. VARIADIC TEMPLATES:" << std::endl;
    std::cout << "   • Handle any number of arguments" << std::endl;
    std::cout << "   • Type-safe heterogeneous processing" << std::endl;
    std::cout << "   • Perfect for metaprogramming" << std::endl;
    
    std::cout << "\n3. EMBEDDED SYSTEMS:" << std::endl;
    std::cout << "   • Validate hardware configurations at compile-time" << std::endl;
    std::cout << "   • Zero-overhead abstractions" << std::endl;
    std::cout << "   • Catch errors before deployment" << std::endl;
    std::cout << "   • Minimal binary size impact" << std::endl;
    
    std::cout << "\n4. MODERN FEATURES:" << std::endl;
    std::cout << "   • Use fold expressions (C++17) when possible" << std::endl;
    std::cout << "   • Use if constexpr for cleaner recursion" << std::endl;
    std::cout << "   • Use constexpr/consteval for clarity" << std::endl;
    std::cout << "   • Combine with concepts for constraints" << std::endl;
    
    std::cout << "\n⚠️ CRITICAL PITFALLS TO AVOID:" << std::endl;
    std::cout << "\n1. COMPILATION TIME:" << std::endl;
    std::cout << "   • Template recursion can be VERY slow" << std::endl;
    std::cout << "   • Prefer constexpr functions over template recursion" << std::endl;
    std::cout << "   • Use fold expressions to eliminate recursion" << std::endl;
    
    std::cout << "\n2. CODE BLOAT:" << std::endl;
    std::cout << "   • Each instantiation = separate code in binary" << std::endl;
    std::cout << "   • Can increase binary size significantly" << std::endl;
    std::cout << "   • Extract common code to non-template functions" << std::endl;
    
    std::cout << "\n3. ERROR MESSAGES:" << std::endl;
    std::cout << "   • Template errors are cryptic and verbose" << std::endl;
    std::cout << "   • Use concepts (C++20) for better errors" << std::endl;
    std::cout << "   • Add static_assert with clear messages" << std::endl;
    
    std::cout << "\n4. RECURSION LIMITS:" << std::endl;
    std::cout << "   • Compilers limit template depth (256-1024)" << std::endl;
    std::cout << "   • Don't increase limits - redesign instead!" << std::endl;
    std::cout << "   • Always provide proper base cases" << std::endl;
    
    std::cout << "\n✅ Compile-Time = Type Safety + Zero Cost!\n" << std::endl;
    std::cout << "⚠️ But use responsibly - compilation time matters!\n" << std::endl;
    
    return 0;
}
