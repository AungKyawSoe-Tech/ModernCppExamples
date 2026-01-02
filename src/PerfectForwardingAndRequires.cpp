// ===================================================================
// ADVANCED PERFECT FORWARDING, CONCEPTS, AND REQUIRES-EXPRESSIONS
// ===================================================================
// This example covers:
// 1. std::forward and perfect forwarding with concepts
// 2. "requires requires" - nested requires explained
// 3. Requirement clauses vs requirement expressions
// 4. When to use and when to avoid "requires requires"
// 5. Embedded systems considerations
//
// TOPICS:
// - Perfect forwarding with universal references
// - Concepts constraining forwarding functions
// - requires clause (starts constraint)
// - requires expression (tests validity)
// - SFINAE vs Concepts comparison
// - Zero-cost abstractions for embedded systems
// ===================================================================

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <concepts>
#include <utility>

// ===================================================================
// SECTION 1: PERFECT FORWARDING BASICS WITH std::forward
// ===================================================================

// Without perfect forwarding - INEFFICIENT
template<typename T>
void call_by_value(T arg) {
    std::cout << "  Value parameter (copy made)" << std::endl;
}

// With perfect forwarding - EFFICIENT
template<typename T>
void call_with_forward(T&& arg) {
    std::cout << "  Universal reference with forward" << std::endl;
    // std::forward<T>(arg) preserves value category:
    // - lvalue stays lvalue (no move)
    // - rvalue stays rvalue (enables move)
}

void demonstrate_perfect_forwarding() {
    std::cout << "\n=== 1. PERFECT FORWARDING BASICS ===" << std::endl;
    
    std::string lvalue = "Hello";
    
    std::cout << "\nCalling by value:" << std::endl;
    call_by_value(lvalue);              // Copy
    call_by_value(std::string("World")); // Copy + move
    
    std::cout << "\nCalling with forward:" << std::endl;
    call_with_forward(lvalue);              // No copy
    call_with_forward(std::string("World")); // No copy
    
    std::cout << "\n💡 KEY POINT:" << std::endl;
    std::cout << "   T&& in template = universal reference" << std::endl;
    std::cout << "   std::forward<T> preserves value category" << std::endl;
}

// ===================================================================
// SECTION 2: REQUIRES CLAUSE VS REQUIRES EXPRESSION
// ===================================================================

// TERMINOLOGY:
// - requires clause: Introduces a constraint (keyword 'requires')
// - requires expression: Tests if expressions are valid

// Example 1: requires clause WITHOUT requires expression
template<typename T>
requires std::integral<T>  // <- This is a requires CLAUSE
T square(T x) {
    return x * x;
}

// Example 2: requires clause WITH requires expression
template<typename T>
requires requires(T x) {   // <- First 'requires' = clause, second = expression
    x + x;                 // Check if x can be added
    x * x;                 // Check if x can be multiplied
}
T compute(T x) {
    return x * x + x;
}

void demonstrate_requires_syntax() {
    std::cout << "\n=== 2. REQUIRES CLAUSE VS REQUIRES EXPRESSION ===" << std::endl;
    
    std::cout << "\n📚 TERMINOLOGY:" << std::endl;
    std::cout << "   requires clause:      'requires <constraint>'" << std::endl;
    std::cout << "   requires expression:  'requires(params) { tests; }'" << std::endl;
    
    std::cout << "\n✅ square(5) = " << square(5) << std::endl;
    std::cout << "✅ compute(5) = " << compute(5) << std::endl;
    
    // square(3.14);   // ERROR: not integral
    // compute("hi");  // ERROR: can't add/multiply strings
}

// ===================================================================
// SECTION 3: THE "REQUIRES REQUIRES" PATTERN
// ===================================================================

// WHAT IS "requires requires"?
//
// Template<typename T>
// requires requires(T t) { ... }
//          ^       ^
//          |       |
//          |       +-- requires EXPRESSION (tests code validity)
//          +---------- requires CLAUSE (introduces constraint)
//
// WHEN TO USE:
// - You need to test if specific operations are valid
// - Standard concepts don't cover your needs
// - You're checking syntax, not just type properties

// Example: Check if type has specific member function
template<typename T>
requires requires(T obj) {           // "requires requires"
    { obj.serialize() } -> std::same_as<std::string>;
}
void save(const T& obj) {
    std::string data = obj.serialize();
    std::cout << "   Saved: " << data << std::endl;
}

// Example: Check if type supports arithmetic and comparison
template<typename T>
requires requires(T a, T b) {        // "requires requires"
    { a + b } -> std::convertible_to<T>;
    { a - b } -> std::convertible_to<T>;
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
}
T clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Test classes
class Serializable {
public:
    std::string serialize() const { return "Serializable{data}"; }
};

void demonstrate_requires_requires() {
    std::cout << "\n=== 3. THE 'REQUIRES REQUIRES' PATTERN ===" << std::endl;
    
    std::cout << "\n📖 EXPLANATION:" << std::endl;
    std::cout << "   First 'requires':  Starts the constraint clause" << std::endl;
    std::cout << "   Second 'requires': Begins the expression testing code" << std::endl;
    std::cout << "   Inside { ... }:    Operations that must be valid" << std::endl;
    
    std::cout << "\n✅ Using save() with Serializable:" << std::endl;
    Serializable obj;
    save(obj);
    
    std::cout << "\n✅ Using clamp() with int:" << std::endl;
    std::cout << "   clamp(150, 0, 100) = " << clamp(150, 0, 100) << std::endl;
    std::cout << "   clamp(-10, 0, 100) = " << clamp(-10, 0, 100) << std::endl;
    std::cout << "   clamp(50, 0, 100) = " << clamp(50, 0, 100) << std::endl;
}

// ===================================================================
// SECTION 4: PERFECT FORWARDING WITH CONCEPTS
// ===================================================================

// Constrain what types can be forwarded
template<typename T>
concept Movable = std::is_move_constructible_v<T>;

template<typename T>
concept Copyable = std::is_copy_constructible_v<T>;

// Factory function with perfect forwarding and concepts
template<typename T, typename... Args>
requires std::constructible_from<T, Args...>  // Concept constrains construction
std::unique_ptr<T> make_unique_constrained(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// Forwarding wrapper with requires requires
template<typename Func, typename... Args>
requires requires(Func f, Args... args) {    // Check if callable
    f(std::forward<Args>(args)...);
}
auto forward_call(Func&& func, Args&&... args) {
    std::cout << "   Forwarding call..." << std::endl;
    return func(std::forward<Args>(args)...);
}

void demonstrate_forwarding_with_concepts() {
    std::cout << "\n=== 4. PERFECT FORWARDING WITH CONCEPTS ===" << std::endl;
    
    std::cout << "\n✅ make_unique_constrained:" << std::endl;
    auto ptr1 = make_unique_constrained<std::string>("Hello", 5, 'X');
    std::cout << "   Created: " << *ptr1 << std::endl;
    
    auto ptr2 = make_unique_constrained<std::vector<int>>(10, 42);
    std::cout << "   Created vector of size: " << ptr2->size() << std::endl;
    
    std::cout << "\n✅ forward_call with lambda:" << std::endl;
    auto lambda = [](int x, int y) {
        return x + y;
    };
    int result = forward_call(lambda, 10, 20);
    std::cout << "   Result: " << result << std::endl;
}

// ===================================================================
// SECTION 5: WHEN TO USE "REQUIRES REQUIRES"
// ===================================================================

void explain_when_to_use_requires_requires() {
    std::cout << "\n=== 5. WHEN TO USE 'REQUIRES REQUIRES' ===" << std::endl;
    
    std::cout << "\n✅ USE 'requires requires' WHEN:" << std::endl;
    std::cout << "   1. Testing specific syntax/operations:" << std::endl;
    std::cout << "      requires requires(T t) { t.foo(); t.bar(); }" << std::endl;
    std::cout << "\n   2. No standard concept exists:" << std::endl;
    std::cout << "      requires requires(T t) { t.serialize(); }" << std::endl;
    std::cout << "\n   3. Complex compound expressions:" << std::endl;
    std::cout << "      requires requires(T a, T b) { a + b * 2; }" << std::endl;
    std::cout << "\n   4. Return type checking:" << std::endl;
    std::cout << "      requires requires(T t) { { t.size() } -> std::same_as<size_t>; }" << std::endl;
    
    std::cout << "\n❌ AVOID 'requires requires' WHEN:" << std::endl;
    std::cout << "   1. Standard concept exists:" << std::endl;
    std::cout << "      ❌ requires requires(T t) { t == t; }" << std::endl;
    std::cout << "      ✅ requires std::equality_comparable<T>" << std::endl;
    std::cout << "\n   2. Simple type trait works:" << std::endl;
    std::cout << "      ❌ requires requires { typename T::value_type; }" << std::endl;
    std::cout << "      ✅ requires requires { typename T::value_type; }  # OK if needed" << std::endl;
    std::cout << "\n   3. Makes code harder to read:" << std::endl;
    std::cout << "      Define a named concept instead!" << std::endl;
    
    std::cout << "\n💡 BEST PRACTICE:" << std::endl;
    std::cout << "   Wrap 'requires requires' in a named concept:" << std::endl;
    std::cout << "   template<typename T>" << std::endl;
    std::cout << "   concept MyConstraint = requires(T t) { /* tests */ };" << std::endl;
}

// ===================================================================
// SECTION 6: READABILITY - NAMED CONCEPTS VS INLINE REQUIRES
// ===================================================================

// BAD: Inline requires requires - hard to read and reuse
template<typename T>
requires requires(T container) {
    { container.begin() } -> std::input_or_output_iterator;
    { container.end() } -> std::input_or_output_iterator;
    { container.size() } -> std::convertible_to<std::size_t>;
    requires std::is_same_v<typename T::value_type, int>;
}
void process_bad(const T& container) {
    std::cout << "Size: " << container.size() << std::endl;
}

// GOOD: Named concept - readable and reusable
template<typename T>
concept IntContainer = requires(T container) {
    { container.begin() } -> std::input_or_output_iterator;
    { container.end() } -> std::input_or_output_iterator;
    { container.size() } -> std::convertible_to<std::size_t>;
    requires std::is_same_v<typename T::value_type, int>;
};

template<IntContainer T>
void process_good(const T& container) {
    std::cout << "Size: " << container.size() << std::endl;
}

void demonstrate_readability() {
    std::cout << "\n=== 6. READABILITY: NAMED CONCEPTS VS INLINE ===" << std::endl;
    
    std::cout << "\n❌ BAD - Inline 'requires requires':" << std::endl;
    std::cout << "   template<typename T>" << std::endl;
    std::cout << "   requires requires(T t) { /* 10 lines */ }" << std::endl;
    std::cout << "   void func(T x) { ... }" << std::endl;
    std::cout << "   Problems: Hard to read, can't reuse, error messages unclear" << std::endl;
    
    std::cout << "\n✅ GOOD - Named concept:" << std::endl;
    std::cout << "   template<typename T>" << std::endl;
    std::cout << "   concept MyConstraint = requires(T t) { /* tests */ };" << std::endl;
    std::cout << "   \n   template<MyConstraint T>" << std::endl;
    std::cout << "   void func(T x) { ... }" << std::endl;
    std::cout << "   Benefits: Readable, reusable, clear error messages" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    process_good(vec);
}

// ===================================================================
// SECTION 7: EMBEDDED SYSTEMS CONSIDERATIONS
// ===================================================================

void explain_embedded_systems() {
    std::cout << "\n=== 7. EMBEDDED SYSTEMS CONSIDERATIONS ===" << std::endl;
    
    std::cout << "\n🎯 ARE CONCEPTS USEFUL FOR EMBEDDED SYSTEMS?" << std::endl;
    std::cout << "   ✅ YES! Here's why:" << std::endl;
    
    std::cout << "\n1. 💰 ZERO RUNTIME COST:" << std::endl;
    std::cout << "   • Concepts are compile-time only" << std::endl;
    std::cout << "   • No runtime overhead vs unconstrained templates" << std::endl;
    std::cout << "   • Generated code is identical" << std::endl;
    std::cout << "   • Same as SFINAE but cleaner" << std::endl;
    
    std::cout << "\n2. 📉 REDUCE CODE SIZE:" << std::endl;
    std::cout << "   • Catch errors early = fewer template instantiations" << std::endl;
    std::cout << "   • Better error messages = less debugging code" << std::endl;
    std::cout << "   • Explicit constraints prevent accidental instantiations" << std::endl;
    
    std::cout << "\n3. 🛡️ TYPE SAFETY:" << std::endl;
    std::cout << "   • Catch type errors at compile time" << std::endl;
    std::cout << "   • No runtime checks needed" << std::endl;
    std::cout << "   • Perfect for safety-critical systems" << std::endl;
    
    std::cout << "\n4. 📚 SELF-DOCUMENTING:" << std::endl;
    std::cout << "   • Requirements are explicit in code" << std::endl;
    std::cout << "   • No need for extensive comments" << std::endl;
    std::cout << "   • Easier code review and maintenance" << std::endl;
    
    std::cout << "\n⚠️ WHEN TO BE CAUTIOUS:" << std::endl;
    std::cout << "   1. Compiler support: Need C++20" << std::endl;
    std::cout << "   2. Compilation time: Complex concepts slow builds" << std::endl;
    std::cout << "   3. Code size: Templates can increase binary size" << std::endl;
    std::cout << "   4. Debugging: Template errors can be complex" << std::endl;
    
    std::cout << "\n💡 EMBEDDED BEST PRACTICES:" << std::endl;
    std::cout << "   ✅ Use concepts for HAL (Hardware Abstraction Layer)" << std::endl;
    std::cout << "   ✅ Constrain register access templates" << std::endl;
    std::cout << "   ✅ Type-safe peripheral interfaces" << std::endl;
    std::cout << "   ✅ Compile-time buffer size checking" << std::endl;
    std::cout << "   ❌ Avoid deep template nesting" << std::endl;
    std::cout << "   ❌ Measure binary size impact" << std::endl;
}

// ===================================================================
// SECTION 8: EMBEDDED EXAMPLE - REGISTER ACCESS
// ===================================================================

// Embedded systems often need type-safe register access
template<typename T>
concept RegisterType = requires {
    requires std::is_integral_v<T>;
    requires sizeof(T) <= 4;  // 32-bit or smaller
    requires std::is_trivially_copyable_v<T>;
};

template<RegisterType T>
class Register {
private:
    volatile T* address_;
    
public:
    explicit Register(uintptr_t addr) : address_(reinterpret_cast<volatile T*>(addr)) {}
    
    void write(T value) const {
        *address_ = value;
    }
    
    T read() const {
        return *address_;
    }
    
    void set_bit(unsigned bit) requires std::unsigned_integral<T> {
        *address_ |= (T{1} << bit);
    }
    
    void clear_bit(unsigned bit) requires std::unsigned_integral<T> {
        *address_ &= ~(T{1} << bit);
    }
};

void demonstrate_embedded_concepts() {
    std::cout << "\n=== 8. EMBEDDED EXAMPLE - REGISTER ACCESS ===" << std::endl;
    
    std::cout << "\n✅ Type-safe register access with concepts:" << std::endl;
    
    // Simulate hardware registers
    uint32_t mock_register = 0x00000000;
    Register<uint32_t> gpio_control(reinterpret_cast<uintptr_t>(&mock_register));
    
    std::cout << "   Initial value: 0x" << std::hex << mock_register << std::dec << std::endl;
    
    gpio_control.write(0xAABBCCDD);
    std::cout << "   After write:   0x" << std::hex << mock_register << std::dec << std::endl;
    
    gpio_control.set_bit(4);
    std::cout << "   After set_bit(4): 0x" << std::hex << mock_register << std::dec << std::endl;
    
    std::cout << "\n💡 Benefits:" << std::endl;
    std::cout << "   • Compile-time type checking" << std::endl;
    std::cout << "   • No runtime overhead" << std::endl;
    std::cout << "   • Can't use wrong size types" << std::endl;
    std::cout << "   • set_bit/clear_bit only for unsigned types" << std::endl;
}

// ===================================================================
// SECTION 9: COMPARISON - SFINAE VS CONCEPTS
// ===================================================================

// Old way: SFINAE (Substitution Failure Is Not An Error)
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
T old_square(T x) {
    return x * x;
}

// New way: Concepts
template<std::integral T>
T new_square(T x) {
    return x * x;
}

void compare_sfinae_vs_concepts() {
    std::cout << "\n=== 9. COMPARISON: SFINAE VS CONCEPTS ===" << std::endl;
    
    std::cout << "\n❌ OLD WAY (SFINAE):" << std::endl;
    std::cout << "   template<typename T, typename = std::enable_if_t<...>>" << std::endl;
    std::cout << "   T func(T x) { ... }" << std::endl;
    std::cout << "\n   Problems:" << std::endl;
    std::cout << "   • Hard to read and write" << std::endl;
    std::cout << "   • Terrible error messages" << std::endl;
    std::cout << "   • Verbose and error-prone" << std::endl;
    
    std::cout << "\n✅ NEW WAY (CONCEPTS):" << std::endl;
    std::cout << "   template<std::integral T>" << std::endl;
    std::cout << "   T func(T x) { ... }" << std::endl;
    std::cout << "\n   Benefits:" << std::endl;
    std::cout << "   • Clean and readable" << std::endl;
    std::cout << "   • Clear error messages" << std::endl;
    std::cout << "   • Intent is obvious" << std::endl;
    std::cout << "   • Easier to maintain" << std::endl;
    
    std::cout << "\n✅ Both work the same:" << std::endl;
    std::cout << "   old_square(5) = " << old_square(5) << std::endl;
    std::cout << "   new_square(5) = " << new_square(5) << std::endl;
    std::cout << "\n💰 Same runtime cost: ZERO" << std::endl;
}

// ===================================================================
// SECTION 10: COMPLETE EXAMPLE - THREAD-SAFE QUEUE
// ===================================================================

#include <mutex>
#include <queue>
#include <optional>

template<typename T>
concept ThreadSafeElement = requires {
    requires std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>;
    requires std::is_destructible_v<T>;
};

template<ThreadSafeElement T>
class ThreadSafeQueue {
private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
    
public:
    // Perfect forwarding with concepts
    template<typename U>
    requires std::constructible_from<T, U>
    void push(U&& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::forward<U>(item));
    }
    
    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return std::nullopt;
        }
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};

void demonstrate_complete_example() {
    std::cout << "\n=== 10. COMPLETE EXAMPLE - THREAD-SAFE QUEUE ===" << std::endl;
    
    ThreadSafeQueue<std::string> queue;
    
    std::cout << "\n✅ Pushing items with perfect forwarding:" << std::endl;
    
    std::string lval = "lvalue string";
    queue.push(lval);                    // Copy (lvalue)
    std::cout << "   Pushed lvalue (copied)" << std::endl;
    
    queue.push(std::string("rvalue"));   // Move (rvalue)
    std::cout << "   Pushed rvalue (moved)" << std::endl;
    
    queue.push("literal");               // Construct in place
    std::cout << "   Pushed literal (constructed)" << std::endl;
    
    std::cout << "\n✅ Popping items:" << std::endl;
    while (auto item = queue.try_pop()) {
        std::cout << "   Popped: " << *item << std::endl;
    }
    
    std::cout << "\n💡 Combines:" << std::endl;
    std::cout << "   • Concepts (ThreadSafeElement)" << std::endl;
    std::cout << "   • Perfect forwarding (std::forward)" << std::endl;
    std::cout << "   • requires clause (std::constructible_from)" << std::endl;
    std::cout << "   • Modern C++ (std::optional, std::mutex)" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     ADVANCED PERFECT FORWARDING & CONCEPTS                       ║\n";
    std::cout << "║                                                                  ║\n";
    std::cout << "║  Topics: std::forward, requires requires, embedded systems      ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    demonstrate_perfect_forwarding();
    demonstrate_requires_syntax();
    demonstrate_requires_requires();
    demonstrate_forwarding_with_concepts();
    explain_when_to_use_requires_requires();
    demonstrate_readability();
    explain_embedded_systems();
    demonstrate_embedded_concepts();
    compare_sfinae_vs_concepts();
    demonstrate_complete_example();
    
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "SUMMARY:\n";
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n✅ KEY TAKEAWAYS:" << std::endl;
    std::cout << "\n1. PERFECT FORWARDING:" << std::endl;
    std::cout << "   • Use T&& (universal reference) in templates" << std::endl;
    std::cout << "   • Always use std::forward<T> when forwarding" << std::endl;
    std::cout << "   • Preserves lvalue/rvalue value category" << std::endl;
    
    std::cout << "\n2. 'REQUIRES REQUIRES':" << std::endl;
    std::cout << "   • First: starts constraint clause" << std::endl;
    std::cout << "   • Second: begins expression testing" << std::endl;
    std::cout << "   • Use for testing specific operations" << std::endl;
    std::cout << "   • Prefer named concepts for readability" << std::endl;
    
    std::cout << "\n3. EMBEDDED SYSTEMS:" << std::endl;
    std::cout << "   • Zero runtime cost (compile-time only)" << std::endl;
    std::cout << "   • Perfect for safety-critical code" << std::endl;
    std::cout << "   • Use for register access, HAL, type safety" << std::endl;
    std::cout << "   • Watch compilation time and binary size" << std::endl;
    
    std::cout << "\n4. BEST PRACTICES:" << std::endl;
    std::cout << "   • Use standard concepts when available" << std::endl;
    std::cout << "   • Name complex constraints as concepts" << std::endl;
    std::cout << "   • Prefer concepts over SFINAE" << std::endl;
    std::cout << "   • Document requirements clearly" << std::endl;
    
    std::cout << "\n✅ Modern C++ = Type Safety + Zero Cost\n" << std::endl;
    
    return 0;
}
