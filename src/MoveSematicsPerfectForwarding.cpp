// ===================================================================
// IMPLEMENTING MOVE SEMANTICS AND PERFECT FORWARDING
// ===================================================================
// A comprehensive guide to C++11/14/17/20 move semantics and perfect forwarding
//
// Topics Covered:
// 1. Lvalues vs Rvalues - Understanding value categories
// 2. Move Semantics - Efficient resource transfer
// 3. std::move - What it really does
// 4. Perfect Forwarding - Preserving value categories
// 5. Rule of Zero/Three/Five
// 6. Real-World Examples
// 7. Performance Comparison
//
// Build: g++ -std=c++20 -Wall -Wextra -O2 -o MoveSematicsPerfectForwarding MoveSematicsPerfectForwarding.cpp
// ===================================================================

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>  // std::move, std::forward
#include <chrono>

// ===================================================================
// SECTION 1: LVALUES vs RVALUES
// ===================================================================

namespace lvalues_rvalues {

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 1: LVALUES vs RVALUES\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 THEORY:\n";
    std::cout << "   Lvalue = Has a NAME and PERSISTENT ADDRESS\n";
    std::cout << "   Rvalue = TEMPORARY object without persistent address\n\n";
    
    // LVALUES - have names, can take address
    int x = 42;
    std::string name = "Hello";
    
    std::cout << "✅ LVALUES (have names, addressable):\n";
    std::cout << "   int x = 42;            // x is lvalue, address: " << &x << "\n";
    std::cout << "   string name = \"Hello\"; // name is lvalue, address: " << &name << "\n\n";
    
    // RVALUES - temporaries
    std::cout << "✅ RVALUES (temporaries, no persistent address):\n";
    std::cout << "   42                     // Literal\n";
    std::cout << "   x + 1                  // Expression result\n";
    std::cout << "   string(\"World\")        // Temporary object\n\n";
    
    // REFERENCE BINDING
    std::cout << "📝 REFERENCE BINDING:\n\n";
    
    int y = 10;
    
    // Lvalue reference - binds to lvalue
    int& lref = y;
    std::cout << "   int& lref = y;         // ✅ OK: lvalue ref binds to lvalue\n";
    // int& lref2 = 42;    // ❌ ERROR: lvalue ref cannot bind to rvalue
    
    // Rvalue reference - binds to rvalue
    int&& rref = 42;
    std::cout << "   int&& rref = 42;       // ✅ OK: rvalue ref binds to rvalue\n";
    // int&& rref2 = y;    // ❌ ERROR: rvalue ref cannot bind to lvalue
    
    // Special case: const lvalue reference can bind to both!
    const int& cref = 42;
    std::cout << "   const int& cref = 42;  // ✅ OK: const lvalue ref extends lifetime\n\n";
    
    std::cout << "💡 WHY const& accepts rvalues:\n";
    std::cout << "   This is why function parameters use 'const T&' for read-only access.\n";
    std::cout << "   They can accept BOTH lvalues and rvalues!\n";
}

} // namespace lvalues_rvalues

// ===================================================================
// SECTION 2: MOVE SEMANTICS - Basic Implementation
// ===================================================================

namespace move_semantics {

class Resource {
private:
    int* data_;
    size_t size_;
    
public:
    // Constructor
    explicit Resource(size_t size = 0) 
        : data_(size > 0 ? new int[size] : nullptr), size_(size) {
        std::cout << "   [Resource] Constructor: allocated " << size_ << " ints\n";
    }
    
    // Destructor
    ~Resource() {
        if (data_) {
            std::cout << "   [Resource] Destructor: freeing " << size_ << " ints\n";
            delete[] data_;
        }
    }
    
    // Copy Constructor - EXPENSIVE! (O(n))
    Resource(const Resource& other) : size_(other.size_) {
        std::cout << "   [Resource] Copy Constructor - EXPENSIVE! Copying " 
                  << size_ << " ints\n";
        if (size_ > 0) {
            data_ = new int[size_];
            std::copy_n(other.data_, size_, data_);
        } else {
            data_ = nullptr;
        }
    }
    
    // Copy Assignment - EXPENSIVE! (O(n))
    Resource& operator=(const Resource& other) {
        std::cout << "   [Resource] Copy Assignment - EXPENSIVE! Copying " 
                  << other.size_ << " ints\n";
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            if (size_ > 0) {
                data_ = new int[size_];
                std::copy_n(other.data_, size_, data_);
            } else {
                data_ = nullptr;
            }
        }
        return *this;
    }
    
    // Move Constructor - FAST! (O(1))
    Resource(Resource&& other) noexcept 
        : data_(other.data_), size_(other.size_) {
        std::cout << "   [Resource] Move Constructor - FAST! O(1) pointer transfer\n";
        
        // Leave source in valid state
        other.data_ = nullptr;
        other.size_ = 0;
    }
    
    // Move Assignment - FAST! (O(1))
    Resource& operator=(Resource&& other) noexcept {
        std::cout << "   [Resource] Move Assignment - FAST! O(1) pointer transfer\n";
        if (this != &other) {
            // Clean up existing resource
            delete[] data_;
            
            // Transfer ownership
            data_ = other.data_;
            size_ = other.size_;
            
            // Leave source in valid state
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    size_t size() const { return size_; }
    
    int* get_data() { return data_; }
    const int* get_data() const { return data_; }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 2: MOVE SEMANTICS\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 THE PROBLEM: Expensive Copies\n";
    std::cout << "   Copy constructor/assignment performs deep copy (O(n))\n";
    std::cout << "   For large objects, this is 10x-1000x slower than necessary!\n\n";
    
    std::cout << "🔧 Demonstrating COPY vs MOVE:\n\n";
    
    // Copy Constructor
    std::cout << "1️⃣ COPY Constructor (expensive):\n";
    Resource r1(5);
    Resource r2 = r1;  // Copy constructor
    std::cout << "   Result: r1 still valid with " << r1.size() << " ints\n\n";
    
    // Move Constructor
    std::cout << "2️⃣ MOVE Constructor (fast):\n";
    Resource r3(5);
    Resource r4 = std::move(r3);  // Move constructor
    std::cout << "   Result: r3 moved-from (size=" << r3.size() 
              << "), r4 owns data (" << r4.size() << " ints)\n\n";
    
    // Copy Assignment
    std::cout << "3️⃣ COPY Assignment (expensive):\n";
    Resource r5(3);
    Resource r6(7);
    r6 = r5;  // Copy assignment
    std::cout << "   Result: Both valid, r6 has " << r6.size() << " ints\n\n";
    
    // Move Assignment
    std::cout << "4️⃣ MOVE Assignment (fast):\n";
    Resource r7(3);
    Resource r8(7);
    r8 = std::move(r7);  // Move assignment
    std::cout << "   Result: r7 moved-from (size=" << r7.size() 
              << "), r8 owns data (" << r8.size() << " ints)\n\n";
    
    std::cout << "💡 KEY INSIGHT:\n";
    std::cout << "   Move operations transfer ownership in O(1) time!\n";
    std::cout << "   Copy operations duplicate data in O(n) time.\n";
    std::cout << "   Always mark move operations 'noexcept' for std::vector optimization!\n";
}

} // namespace move_semantics

// ===================================================================
// SECTION 3: UNDERSTANDING std::move
// ===================================================================

namespace understanding_move {

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 3: UNDERSTANDING std::move\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 WHAT IS std::move?\n";
    std::cout << "   std::move does NOT move anything!\n";
    std::cout << "   It's just a CAST to rvalue reference (T&&)\n";
    std::cout << "   It tells compiler: 'this object can be moved from'\n\n";
    
    std::cout << "   Simplified implementation:\n";
    std::cout << "   template<typename T>\n";
    std::cout << "   T&& move(T&& t) noexcept {\n";
    std::cout << "       return static_cast<T&&>(t);\n";
    std::cout << "   }\n\n";
    
    std::cout << "🔧 DEMONSTRATION:\n\n";
    
    // std::move with string
    std::cout << "1️⃣ std::move with std::string:\n";
    std::string s1 = "Hello, World!";
    std::cout << "   Before: s1 = \"" << s1 << "\"\n";
    
    std::string s2 = std::move(s1);  // Move constructor called
    std::cout << "   After move:\n";
    std::cout << "     s1 = \"" << s1 << "\" (moved-from state)\n";
    std::cout << "     s2 = \"" << s2 << "\" (owns the data)\n\n";
    
    // Moved-from state
    std::cout << "2️⃣ MOVED-FROM STATE:\n";
    std::cout << "   After std::move, object is in 'valid but unspecified' state\n";
    std::cout << "   ✅ SAFE: Destroy it, assign to it\n";
    std::cout << "   ❌ UNSAFE: Don't use its value!\n\n";
    
    s1 = "New Value";  // Safe: reassignment
    std::cout << "   s1 reassigned: \"" << s1 << "\" (valid again)\n\n";
    
    // Common mistakes
    std::cout << "⚠️ COMMON MISTAKES:\n\n";
    
    std::cout << "❌ MISTAKE 1: Using object after move\n";
    std::cout << "   string s3 = \"test\";\n";
    std::cout << "   string s4 = std::move(s3);\n";
    std::cout << "   cout << s3;  // ❌ WRONG! Undefined behavior!\n\n";
    
    std::cout << "❌ MISTAKE 2: Returning std::move(local)\n";
    std::cout << "   string func() {\n";
    std::cout << "       string s = \"hello\";\n";
    std::cout << "       return std::move(s);  // ❌ Prevents RVO!\n";
    std::cout << "   }\n";
    std::cout << "   ✅ Correct: return s;  // RVO or move, NOT copy\n\n";
    
    std::cout << "❌ MISTAKE 3: Moving from const\n";
    std::cout << "   const string cs = \"test\";\n";
    std::cout << "   string s = std::move(cs);  // ❌ Calls COPY, not move!\n\n";
    
    std::cout << "💡 KEY TAKEAWAY:\n";
    std::cout << "   std::move is a PERMISSION to move, not a command.\n";
    std::cout << "   The actual move happens in the move constructor/assignment.\n";
}

} // namespace understanding_move

// ===================================================================
// SECTION 4: PERFECT FORWARDING
// ===================================================================

namespace perfect_forwarding {

class Widget {
public:
    Widget(const std::string& name) {
        std::cout << "   [Widget] Constructed with LVALUE: \"" << name << "\" (copied)\n";
    }
    
    Widget(std::string&& name) {
        std::cout << "   [Widget] Constructed with RVALUE: \"" << name << "\" (moved)\n";
    }
};

// Bad: Always copies
template<typename T>
void bad_wrapper(T arg) {
    // arg is always an lvalue here, even if rvalue was passed!
    Widget w(arg);  // Always calls Widget(const string&)
}

// Good: Perfect forwarding
template<typename T>
void good_wrapper(T&& arg) {  // Universal/Forwarding reference
    // std::forward preserves the value category
    Widget w(std::forward<T>(arg));
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 4: PERFECT FORWARDING\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 THE PROBLEM:\n";
    std::cout << "   When passing arguments through wrapper functions,\n";
    std::cout << "   we lose information about whether they were lvalues or rvalues.\n\n";
    
    std::cout << "🔧 DEMONSTRATION:\n\n";
    
    std::cout << "1️⃣ WITHOUT Perfect Forwarding (bad_wrapper):\n";
    std::string s1 = "lvalue";
    bad_wrapper(s1);                    // Copies (expected)
    bad_wrapper(std::string("rvalue")); // Still COPIES (not desired!)
    std::cout << "\n";
    
    std::cout << "2️⃣ WITH Perfect Forwarding (good_wrapper):\n";
    std::string s2 = "lvalue";
    good_wrapper(s2);                    // Copies (as expected)
    good_wrapper(std::string("rvalue")); // MOVES (optimized!)
    std::cout << "\n";
    
    std::cout << "📝 UNIVERSAL REFERENCE (T&&):\n";
    std::cout << "   template<typename T>\n";
    std::cout << "   void func(T&& arg);  // NOT an rvalue reference!\n\n";
    std::cout << "   It's a UNIVERSAL REFERENCE (also called forwarding reference)\n";
    std::cout << "   Can bind to BOTH lvalues and rvalues\n\n";
    
    std::cout << "   Reference Collapsing Rules:\n";
    std::cout << "   T&  &&  ->  T&    (lvalue reference)\n";
    std::cout << "   T&& &&  ->  T&&   (rvalue reference)\n\n";
    
    std::cout << "💡 WHEN TO USE:\n";
    std::cout << "   std::move    - UNCONDITIONAL cast to rvalue\n";
    std::cout << "                  Use when you KNOW you have an lvalue to transfer\n";
    std::cout << "   std::forward - CONDITIONAL forwarding preserving value category\n";
    std::cout << "                  Use with T&& (universal reference) in templates\n";
}

// Example: Factory function
template<typename T, typename... Args>
std::unique_ptr<T> make_unique_custom(Args&&... args) {
    std::cout << "   [Factory] Creating object with perfect forwarding\n";
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

void demonstrate_factory() {
    std::cout << "\n3️⃣ Factory Pattern with Perfect Forwarding:\n";
    
    std::string name = "Gadget";
    auto w1 = make_unique_custom<Widget>(name);           // Forwards as lvalue
    auto w2 = make_unique_custom<Widget>("Gizmo");        // Forwards as rvalue
    auto w3 = make_unique_custom<Widget>(std::move(name)); // Forwards as rvalue
}

} // namespace perfect_forwarding

// ===================================================================
// SECTION 5: RULE OF ZERO/THREE/FIVE
// ===================================================================

namespace rule_of_five {

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 5: RULE OF ZERO/THREE/FIVE\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 RULE OF ZERO (PREFERRED!):\n";
    std::cout << "   Don't manage resources manually!\n";
    std::cout << "   Use std::vector, std::string, std::unique_ptr, etc.\n\n";
    
    std::cout << "   class Widget {\n";
    std::cout << "       std::string name_;        // Self-managing\n";
    std::cout << "       std::vector<int> data_;   // Self-managing\n";
    std::cout << "       // No special members needed!\n";
    std::cout << "   };\n\n";
    
    std::cout << "📚 RULE OF THREE (C++98):\n";
    std::cout << "   If you define ONE, define ALL THREE:\n";
    std::cout << "   1. Destructor\n";
    std::cout << "   2. Copy Constructor\n";
    std::cout << "   3. Copy Assignment Operator\n\n";
    
    std::cout << "📚 RULE OF FIVE (C++11+):\n";
    std::cout << "   If managing resources, define ALL FIVE:\n";
    std::cout << "   1. Destructor\n";
    std::cout << "   2. Copy Constructor\n";
    std::cout << "   3. Copy Assignment Operator\n";
    std::cout << "   4. Move Constructor (mark noexcept!)\n";
    std::cout << "   5. Move Assignment Operator (mark noexcept!)\n\n";
    
    std::cout << "   class Resource {\n";
    std::cout << "   public:\n";
    std::cout << "       ~Resource();                           // 1\n";
    std::cout << "       Resource(const Resource&);             // 2\n";
    std::cout << "       Resource& operator=(const Resource&);  // 3\n";
    std::cout << "       Resource(Resource&&) noexcept;         // 4\n";
    std::cout << "       Resource& operator=(Resource&&) noexcept; // 5\n";
    std::cout << "   };\n\n";
    
    std::cout << "🔧 DELETE UNWANTED OPERATIONS:\n";
    std::cout << "   Make class non-copyable but movable:\n\n";
    std::cout << "   class NonCopyable {\n";
    std::cout << "   public:\n";
    std::cout << "       NonCopyable(const NonCopyable&) = delete;\n";
    std::cout << "       NonCopyable& operator=(const NonCopyable&) = delete;\n";
    std::cout << "       NonCopyable(NonCopyable&&) = default;\n";
    std::cout << "       NonCopyable& operator=(NonCopyable&&) = default;\n";
    std::cout << "   };\n\n";
    
    std::cout << "💡 BEST PRACTICE:\n";
    std::cout << "   1. Prefer Rule of Zero (use standard library types)\n";
    std::cout << "   2. If managing resources, use Rule of Five\n";
    std::cout << "   3. Always mark move operations noexcept\n";
    std::cout << "   4. Use =delete for unwanted operations\n";
    std::cout << "   5. Use =default when compiler-generated is correct\n";
}

} // namespace rule_of_five

// ===================================================================
// SECTION 6: REAL-WORLD EXAMPLES
// ===================================================================

namespace real_world_examples {

void demonstrate_vector() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 6: REAL-WORLD EXAMPLES\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1️⃣ std::vector Push Operations:\n\n";
    
    std::vector<std::string> vec;
    vec.reserve(3);
    
    std::string s1 = "String1";
    std::string s2 = "String2";
    
    std::cout << "   vec.push_back(s1);            // Copy (s1 still valid)\n";
    vec.push_back(s1);
    std::cout << "   s1 after: \"" << s1 << "\"\n\n";
    
    std::cout << "   vec.push_back(std::move(s2)); // Move (s2 now empty)\n";
    vec.push_back(std::move(s2));
    std::cout << "   s2 after: \"" << s2 << "\" (moved-from)\n\n";
    
    std::cout << "   vec.emplace_back(\"String3\");  // Construct in-place\n";
    vec.emplace_back("String3");
    std::cout << "\n";
}

void demonstrate_rvo() {
    std::cout << "2️⃣ Return Value Optimization (RVO):\n\n";
    
    std::cout << "   ✅ CORRECT:\n";
    std::cout << "   vector<int> create() {\n";
    std::cout << "       vector<int> result(1000);\n";
    std::cout << "       return result;  // RVO or move (NOT copy!)\n";
    std::cout << "   }\n\n";
    
    std::cout << "   ❌ WRONG:\n";
    std::cout << "   vector<int> create() {\n";
    std::cout << "       vector<int> result(1000);\n";
    std::cout << "       return std::move(result);  // Prevents RVO!\n";
    std::cout << "   }\n\n";
}

void demonstrate_unique_ptr() {
    std::cout << "3️⃣ Unique Ownership Transfer:\n\n";
    
    std::cout << "   auto ptr1 = make_unique<int>(42);\n";
    auto ptr1 = std::make_unique<int>(42);
    std::cout << "   ptr1 owns: " << *ptr1 << "\n\n";
    
    std::cout << "   auto ptr2 = std::move(ptr1);  // Transfer ownership\n";
    auto ptr2 = std::move(ptr1);
    std::cout << "   ptr1 is now: " << (ptr1 ? "valid" : "nullptr") << "\n";
    std::cout << "   ptr2 owns: " << *ptr2 << "\n\n";
}

void demonstrate_factory() {
    std::cout << "4️⃣ Factory Functions with Perfect Forwarding:\n\n";
    
    std::cout << "   See Section 4 for detailed factory examples!\n\n";
}

void demonstrate_all() {
    demonstrate_vector();
    demonstrate_rvo();
    demonstrate_unique_ptr();
    demonstrate_factory();
}

} // namespace real_world_examples

// ===================================================================
// SECTION 7: PERFORMANCE COMPARISON
// ===================================================================

namespace performance_comparison {

std::vector<int> create_large_vector(size_t size) {
    std::vector<int> result(size);
    for (size_t i = 0; i < size; ++i) {
        result[i] = static_cast<int>(i);
    }
    return result;  // RVO or move
}

void benchmark_copy_vs_move() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 7: PERFORMANCE COMPARISON\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    const size_t size = 1000000;  // 1 million integers
    const int iterations = 100;
    
    std::cout << "🚀 Benchmark Configuration:\n";
    std::cout << "   Buffer size: " << size << " integers\n";
    std::cout << "   Iterations: " << iterations << "\n\n";
    
    // Benchmark COPY
    std::cout << "1️⃣ Testing COPY operations...\n";
    auto start_copy = std::chrono::high_resolution_clock::now();
    
    std::vector<int> source = create_large_vector(size);
    for (int i = 0; i < iterations; ++i) {
        std::vector<int> dest = source;  // Copy
        (void)dest;  // Prevent optimization
    }
    
    auto end_copy = std::chrono::high_resolution_clock::now();
    auto copy_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_copy - start_copy).count();
    
    // Benchmark MOVE
    std::cout << "2️⃣ Testing MOVE operations...\n";
    auto start_move = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        std::vector<int> source_temp = create_large_vector(size);
        std::vector<int> dest = std::move(source_temp);  // Move
        (void)dest;  // Prevent optimization
    }
    
    auto end_move = std::chrono::high_resolution_clock::now();
    auto move_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_move - start_move).count();
    
    // Results
    std::cout << "\n📊 RESULTS:\n";
    std::cout << "   Copy time: " << copy_time << " ms\n";
    std::cout << "   Move time: " << move_time << " ms\n";
    std::cout << "   Speedup:   " << (double)copy_time / move_time << "x faster!\n\n";
    
    std::cout << "💡 INTERPRETATION:\n";
    std::cout << "   Copy: O(n) - copies all " << size << " integers\n";
    std::cout << "   Move: O(1) - just transfers pointer ownership\n";
    std::cout << "   For large objects, move is 10x-1000x+ faster!\n";
}

} // namespace performance_comparison

// ===================================================================
// MAIN - Run All Demonstrations
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     IMPLEMENTING MOVE SEMANTICS AND PERFECT FORWARDING          ║\n";
    std::cout << "║            Comprehensive Guide to Modern C++ Features           ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    try {
        // Section 1: Lvalues vs Rvalues
        lvalues_rvalues::demonstrate();
        
        // Section 2: Move Semantics
        move_semantics::demonstrate();
        
        // Section 3: Understanding std::move
        understanding_move::demonstrate();
        
        // Section 4: Perfect Forwarding
        perfect_forwarding::demonstrate();
        perfect_forwarding::demonstrate_factory();
        
        // Section 5: Rule of Zero/Three/Five
        rule_of_five::demonstrate();
        
        // Section 6: Real-World Examples
        real_world_examples::demonstrate_all();
        
        // Section 7: Performance Comparison
        performance_comparison::benchmark_copy_vs_move();
        
        // Summary
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "SUMMARY: KEY TAKEAWAYS\n";
        std::cout << std::string(70, '=') << "\n\n";
        
        std::cout << "✅ Move Semantics:\n";
        std::cout << "   • Enables O(1) resource transfer instead of O(n) copy\n";
        std::cout << "   • Always mark move operations noexcept\n";
        std::cout << "   • 10x-1000x+ performance improvement for large objects\n\n";
        
        std::cout << "✅ Perfect Forwarding:\n";
        std::cout << "   • Preserves lvalue/rvalue category through templates\n";
        std::cout << "   • Use T&& (universal reference) with std::forward<T>\n";
        std::cout << "   • Essential for factory functions and wrappers\n\n";
        
        std::cout << "✅ Best Practices:\n";
        std::cout << "   • Prefer Rule of Zero (use std types)\n";
        std::cout << "   • If managing resources, implement Rule of Five\n";
        std::cout << "   • Return by value (trust RVO and move)\n";
        std::cout << "   • Use std::move for explicit ownership transfer\n";
        std::cout << "   • Use std::forward in template forwarding\n";
        std::cout << "   • Don't use objects after moving from them\n\n";
        
        std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    ALL EXAMPLES COMPLETED!                       ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
