// ===================================================================
// NOEXCEPT BEST PRACTICES IN MODERN C++
// ===================================================================
// Comprehensive guide to when and when NOT to use noexcept
//
// Topics Covered:
// 1. What is noexcept and why it matters
// 2. When to ALWAYS use noexcept
// 3. When to NEVER use noexcept
// 4. Conditional noexcept
// 5. Performance implications
// 6. std::vector optimization with noexcept
// 7. Exception safety guarantees
// 8. Best practices and guidelines
//
// Build: g++ -std=c++20 -Wall -Wextra -O2 -o NoexceptBestPractices NoexceptBestPractices.cpp
// ===================================================================

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <type_traits>
#include <chrono>
#include <cmath>  // for std::log2
#include <algorithm>  // for std::copy_n

// ===================================================================
// SECTION 1: WHAT IS NOEXCEPT?
// ===================================================================

namespace what_is_noexcept {

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 1: WHAT IS NOEXCEPT?\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 DEFINITION:\n";
    std::cout << "   noexcept is a specifier that tells the compiler:\n";
    std::cout << "   'This function will NEVER throw an exception'\n\n";
    
    std::cout << "💡 WHAT HAPPENS IF YOU LIE?\n";
    std::cout << "   If a noexcept function throws:\n";
    std::cout << "   1. std::terminate() is called immediately\n";
    std::cout << "   2. Stack unwinding does NOT happen\n";
    std::cout << "   3. Destructors are NOT called\n";
    std::cout << "   4. Program terminates (no recovery possible)\n\n";
    
    std::cout << "🎯 WHY USE NOEXCEPT?\n";
    std::cout << "   1. Performance: Compiler can optimize more aggressively\n";
    std::cout << "   2. std::vector uses move only if noexcept\n";
    std::cout << "   3. Documents that function is exception-safe\n";
    std::cout << "   4. Enables certain optimizations at call sites\n";
}

} // namespace what_is_noexcept

// ===================================================================
// SECTION 2: WHEN TO ALWAYS USE NOEXCEPT
// ===================================================================

namespace always_use_noexcept {

// RULE 1: DESTRUCTORS - Always noexcept (implicit in C++)
class Resource {
private:
    int* data_;
    
public:
    Resource() : data_(new int(42)) {
        std::cout << "   [Resource] Constructor\n";
    }
    
    // Destructors are implicitly noexcept
    ~Resource() noexcept {  // Explicit is good for documentation
        delete data_;
        std::cout << "   [Resource] Destructor (noexcept)\n";
    }
};

// RULE 2: MOVE CONSTRUCTORS AND MOVE ASSIGNMENT
// Critical for std::vector optimization!
class MoveableResource {
private:
    int* data_;
    size_t size_;
    
public:
    explicit MoveableResource(size_t size) 
        : data_(new int[size]), size_(size) {
        std::cout << "   [MoveableResource] Constructor\n";
    }
    
    ~MoveableResource() noexcept {
        delete[] data_;
    }
    
    // ✅ Move constructor - ALWAYS mark noexcept
    MoveableResource(MoveableResource&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "   [MoveableResource] Move constructor (noexcept)\n";
    }
    
    // ✅ Move assignment - ALWAYS mark noexcept
    MoveableResource& operator=(MoveableResource&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        std::cout << "   [MoveableResource] Move assignment (noexcept)\n";
        return *this;
    }
    
    // Copy operations (not noexcept - can throw on allocation)
    MoveableResource(const MoveableResource& other)
        : data_(new int[other.size_]), size_(other.size_) {
        std::copy_n(other.data_, size_, data_);
        std::cout << "   [MoveableResource] Copy constructor (can throw)\n";
    }
    
    MoveableResource& operator=(const MoveableResource& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = new int[size_];  // Can throw!
            std::copy_n(other.data_, size_, data_);
        }
        std::cout << "   [MoveableResource] Copy assignment (can throw)\n";
        return *this;
    }
};

// RULE 3: SWAP FUNCTIONS - Always noexcept
class Swappable {
private:
    int value_;
    
public:
    explicit Swappable(int val) : value_(val) {}
    
    // ✅ Swap should always be noexcept
    friend void swap(Swappable& a, Swappable& b) noexcept {
        using std::swap;
        swap(a.value_, b.value_);
        std::cout << "   [Swappable] Swapped (noexcept)\n";
    }
};

// RULE 4: Simple getters and setters (no allocation, no complex logic)
class DataHolder {
private:
    int value_;
    
public:
    // ✅ Simple getter - noexcept
    int getValue() const noexcept {
        return value_;
    }
    
    // ✅ Simple setter - noexcept
    void setValue(int val) noexcept {
        value_ = val;
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 2: WHEN TO ALWAYS USE NOEXCEPT\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✅ ALWAYS USE NOEXCEPT FOR:\n\n";
    
    std::cout << "1️⃣ DESTRUCTORS (implicit noexcept):\n";
    {
        Resource r;
    }
    std::cout << "\n";
    
    std::cout << "2️⃣ MOVE OPERATIONS (critical for std::vector):\n";
    MoveableResource mr1(10);
    MoveableResource mr2 = std::move(mr1);
    std::cout << "\n";
    
    std::cout << "3️⃣ SWAP FUNCTIONS:\n";
    Swappable s1(10), s2(20);
    swap(s1, s2);
    std::cout << "\n";
    
    std::cout << "4️⃣ SIMPLE GETTERS/SETTERS (no allocation):\n";
    DataHolder dh;
    dh.setValue(42);
    std::cout << "   getValue() = " << dh.getValue() << " (noexcept)\n\n";
    
    std::cout << "💡 WHY THESE MUST BE NOEXCEPT:\n";
    std::cout << "   • Destructors: Can't handle exceptions during cleanup\n";
    std::cout << "   • Move ops: std::vector falls back to copy if not noexcept\n";
    std::cout << "   • Swap: Used in exception-safe code patterns\n";
    std::cout << "   • Simple ops: No reason to throw, performance benefit\n";
}

} // namespace always_use_noexcept

// ===================================================================
// SECTION 3: WHEN TO NEVER USE NOEXCEPT
// ===================================================================

namespace never_use_noexcept {

// ❌ NEVER mark noexcept if function can throw!

// DON'T use noexcept: Functions that allocate memory
class Container {
private:
    std::vector<int> data_;
    
public:
    // ❌ DON'T mark noexcept - can throw std::bad_alloc
    void addElement(int value) {  // No noexcept!
        data_.push_back(value);  // Can throw
        std::cout << "   [Container] Added element (can throw)\n";
    }
    
    // ❌ DON'T mark noexcept - can throw on allocation
    Container(size_t size) : data_(size) {  // No noexcept!
        std::cout << "   [Container] Constructor (can throw)\n";
    }
};

// DON'T use noexcept: Functions that perform I/O
class FileHandler {
public:
    // ❌ DON'T mark noexcept - I/O can fail
    void writeToFile(const std::string& filename, const std::string& data) {
        std::cout << "   [FileHandler] Writing to file (can throw)\n";
        // File operations can throw
        // if (!success) throw std::runtime_error("Write failed");
    }
    
    // ❌ DON'T mark noexcept - I/O can fail
    std::string readFromFile(const std::string& filename) {
        std::cout << "   [FileHandler] Reading from file (can throw)\n";
        // File operations can throw
        return "data";
    }
};

// DON'T use noexcept: Functions that validate input
class Validator {
public:
    // ❌ DON'T mark noexcept - validation can throw
    void validateAge(int age) {
        if (age < 0 || age > 150) {
            throw std::invalid_argument("Invalid age");
        }
        std::cout << "   [Validator] Age validated (can throw)\n";
    }
    
    // ❌ DON'T mark noexcept - validation can throw
    void checkNonEmpty(const std::string& str) {
        if (str.empty()) {
            throw std::invalid_argument("String cannot be empty");
        }
        std::cout << "   [Validator] String validated (can throw)\n";
    }
};

// DON'T use noexcept: Functions calling other non-noexcept functions
class Processor {
private:
    Container container_;
    
public:
    Processor() : container_(10) {}
    
    // ❌ DON'T mark noexcept - calls non-noexcept function
    void process(int value) {  // No noexcept!
        container_.addElement(value);  // Can throw
        std::cout << "   [Processor] Processed (can throw)\n";
    }
};

// DON'T use noexcept: Functions that throw std::runtime_error
class RuntimeErrorExample {
public:
    // ❌ DANGEROUS: Marked noexcept but throws std::runtime_error
    // void dangerousFunction() noexcept {
    //     throw std::runtime_error("This causes std::terminate()!");
    //     // If this runs, program terminates immediately!
    // }
    
    // ✅ CORRECT: NOT marked noexcept, can throw
    void safeFunction(bool shouldFail) {
        if (shouldFail) {
            throw std::runtime_error("Safe to throw - not noexcept");
        }
        std::cout << "   [RuntimeErrorExample] Executed successfully\n";
    }
    
    // ❌ WRONG: Claims noexcept but allocates (can throw bad_alloc)
    // std::vector<int> wrongNoexcept(size_t size) noexcept {
    //     return std::vector<int>(size);  // Can throw! Causes terminate!
    // }
    
    // ✅ CORRECT: Not marked noexcept
    std::vector<int> correctVersion(size_t size) {
        std::cout << "   [RuntimeErrorExample] Creating vector (can throw bad_alloc)\n";
        return std::vector<int>(size);
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 3: WHEN TO NEVER USE NOEXCEPT\n";
    std::cout << "5️⃣ FUNCTIONS THAT THROW std::runtime_error:\n";
    RuntimeErrorExample rte;
    try {
        rte.safeFunction(false);
        std::cout << "   Success case handled\n";
    } catch (const std::runtime_error& e) {
        std::cout << "   Caught: " << e.what() << "\n";
    }
    
    auto vec = rte.correctVersion(10);
    std::cout << "   Vector created with size: " << vec.size() << "\n\n";
    
    std::cout << "⚠️  CRITICAL: If safeFunction() was marked noexcept:\n";
    std::cout << "   • Throwing std::runtime_error would call std::terminate()\n";
    std::cout << "   • No catch block would execute\n";
    std::cout << "   • No stack unwinding, no destructors called\n";
    std::cout << "   • Program crashes immediately\n\n";
    
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "❌ NEVER USE NOEXCEPT FOR:\n\n";
    
    std::cout << "1️⃣ FUNCTIONS THAT ALLOCATE MEMORY:\n";
    Container c(10);
    c.addElement(42);
    std::cout << "   Allocation can throw std::bad_alloc\n\n";
    
    std::cout << "2️⃣ FUNCTIONS THAT PERFORM I/O:\n";
    FileHandler fh;
    fh.writeToFile("test.txt", "data");
    std::cout << "   I/O operations can fail and throw\n\n";
    
    std::cout << "3️⃣ FUNCTIONS THAT VALIDATE INPUT:\n";
    Validator v;
    try {
        v.checkNonEmpty("hello");
        std::cout << "   Validation passed\n";
    } catch (...) {
        std::cout << "   Validation can throw\n";
    }
    std::cout << "\n";
    
    std::cout << "4️⃣ FUNCTIONS CALLING NON-NOEXCEPT FUNCTIONS:\n";
    Processor p;
    p.process(100);
    std::cout << "   Chain of calls inherits throwing behavior\n\n";
    
    std::cout << "💡 WHY NEVER MARK THESE NOEXCEPT:\n";
    std::cout << "   • Lying about noexcept causes std::terminate()\n";
    std::cout << "   • No recovery possible if exception thrown\n";
    std::cout << "   • Better to propagate exception to caller\n";
    std::cout << "   • Allows proper error handling up the stack\n";
}

} // namespace never_use_noexcept

// ===================================================================
// SECTION 4: CONDITIONAL NOEXCEPT
// ===================================================================

namespace conditional_noexcept {

// Use conditional noexcept for template operations
template<typename T>
class Wrapper {
private:
    T value_;
    
public:
    explicit Wrapper(const T& val) : value_(val) {}
    
    // Conditional noexcept based on T's move constructor
    Wrapper(Wrapper&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(other.value_)) {
        std::cout << "   [Wrapper] Move constructor (conditionally noexcept)\n";
    }
    
    // Conditional noexcept based on T's swap
    void swap(Wrapper& other) noexcept(std::is_nothrow_swappable_v<T>) {
        using std::swap;
        swap(value_, other.value_);
        std::cout << "   [Wrapper] Swap (conditionally noexcept)\n";
    }
};

// Example: std::pair uses conditional noexcept
template<typename T1, typename T2>
class MyPair {
private:
    T1 first_;
    T2 second_;
    
public:
    MyPair(const T1& f, const T2& s) : first_(f), second_(s) {}
    
    // noexcept only if both T1 and T2 have noexcept move constructors
    MyPair(MyPair&& other) noexcept(
        std::is_nothrow_move_constructible_v<T1> &&
        std::is_nothrow_move_constructible_v<T2>)
        : first_(std::move(other.first_))
        , second_(std::move(other.second_)) {
        std::cout << "   [MyPair] Move (conditionally noexcept)\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 4: CONDITIONAL NOEXCEPT\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 CONDITIONAL NOEXCEPT:\n";
    std::cout << "   Use noexcept(condition) for templates\n";
    std::cout << "   noexcept status depends on template parameter\n\n";
    
    std::cout << "✅ EXAMPLE 1: Wrapper with int (noexcept move):\n";
    Wrapper<int> w1(42);
    Wrapper<int> w2(std::move(w1));
    std::cout << "   int move is noexcept: " 
              << std::is_nothrow_move_constructible_v<int> << "\n\n";
    
    std::cout << "✅ EXAMPLE 2: Wrapper with std::string (noexcept move):\n";
    Wrapper<std::string> w3("hello");
    Wrapper<std::string> w4(std::move(w3));
    std::cout << "   string move is noexcept: " 
              << std::is_nothrow_move_constructible_v<std::string> << "\n\n";
    
    std::cout << "✅ EXAMPLE 3: MyPair with noexcept types:\n";
    MyPair<int, int> p1(10, 20);
    MyPair<int, int> p2(std::move(p1));
    std::cout << "\n";
    
    std::cout << "💡 WHEN TO USE CONDITIONAL NOEXCEPT:\n";
    std::cout << "   • Template classes wrapping user types\n";
    std::cout << "   • Want to preserve noexcept guarantee when possible\n";
    std::cout << "   • Standard library containers use this extensively\n";
}

} // namespace conditional_noexcept

// ===================================================================
// SECTION 5: STD::VECTOR OPTIMIZATION WITH NOEXCEPT
// ===================================================================

namespace vector_optimization {

// Class WITHOUT noexcept move constructor
class WithoutNoexcept {
private:
    int* data_;
    
public:
    explicit WithoutNoexcept(int val = 0) : data_(new int(val)) {
        // std::cout << "   [WithoutNoexcept] Constructor\n";
    }
    
    ~WithoutNoexcept() {
        delete data_;
    }
    
    // ❌ Move constructor WITHOUT noexcept
    WithoutNoexcept(WithoutNoexcept&& other) 
        : data_(other.data_) {
        other.data_ = nullptr;
        std::cout << "   [WithoutNoexcept] MOVE constructor (not noexcept)\n";
    }
    
    // Copy constructor
    WithoutNoexcept(const WithoutNoexcept& other) 
        : data_(new int(*other.data_)) {
        std::cout << "   [WithoutNoexcept] COPY constructor\n";
    }
    
    WithoutNoexcept& operator=(WithoutNoexcept&&) = default;
    WithoutNoexcept& operator=(const WithoutNoexcept&) = default;
};

// Class WITH noexcept move constructor
class WithNoexcept {
private:
    int* data_;
    
public:
    explicit WithNoexcept(int val = 0) : data_(new int(val)) {
        // std::cout << "   [WithNoexcept] Constructor\n";
    }
    
    ~WithNoexcept() {
        delete data_;
    }
    
    // ✅ Move constructor WITH noexcept
    WithNoexcept(WithNoexcept&& other) noexcept
        : data_(other.data_) {
        other.data_ = nullptr;
        std::cout << "   [WithNoexcept] MOVE constructor (noexcept)\n";
    }
    
    // Copy constructor
    WithNoexcept(const WithNoexcept& other) 
        : data_(new int(*other.data_)) {
        std::cout << "   [WithNoexcept] COPY constructor\n";
    }
    
    WithNoexcept& operator=(WithNoexcept&&) noexcept = default;
    WithNoexcept& operator=(const WithNoexcept&) = default;
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 5: STD::VECTOR OPTIMIZATION WITH NOEXCEPT\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "🎯 THE CRITICAL DIFFERENCE:\n";
    std::cout << "   std::vector uses MOVE only if noexcept\n";
    std::cout << "   Otherwise, it uses COPY for exception safety\n\n";
    
    std::cout << "❌ WITHOUT noexcept move (vector uses COPY on resize):\n";
    {
        std::vector<WithoutNoexcept> vec;
        vec.reserve(2);
        vec.emplace_back(1);
        vec.emplace_back(2);
        
        std::cout << "   Resizing vector (triggers reallocation):\n";
        vec.emplace_back(3);  // Triggers resize - uses COPY!
    }
    std::cout << "\n";
    
    std::cout << "✅ WITH noexcept move (vector uses MOVE on resize):\n";
    {
        std::vector<WithNoexcept> vec;
        vec.reserve(2);
        vec.emplace_back(1);
        vec.emplace_back(2);
        
        std::cout << "   Resizing vector (triggers reallocation):\n";
        vec.emplace_back(3);  // Triggers resize - uses MOVE!
    }
    std::cout << "\n";
    
    std::cout << "📊 PERFORMANCE IMPACT:\n";
    std::cout << "   Without noexcept: O(n) COPY during resize\n";
    std::cout << "   With noexcept:    O(n) MOVE during resize (much faster!)\n";
    std::cout << "   For 1000 objects: 1000x COPY vs 1000x MOVE\n";
    std::cout << "   Speedup can be 10x-100x+ depending on object size!\n";
}

} // namespace vector_optimization

// ===================================================================
// SECTION 6: PERFORMANCE COMPARISON
// ===================================================================

namespace performance_comparison {

class HeavyObject {
private:
    std::vector<int> data_;
    
public:
    explicit HeavyObject(size_t size = 1000) : data_(size, 42) {}
    
    // Copy constructor (expensive)
    HeavyObject(const HeavyObject& other) : data_(other.data_) {}
    
    // Move constructor (cheap) - WITH noexcept
    HeavyObject(HeavyObject&& other) noexcept : data_(std::move(other.data_)) {}
    
    HeavyObject& operator=(const HeavyObject&) = default;
    HeavyObject& operator=(HeavyObject&&) noexcept = default;
};

void benchmark() {
    const size_t num_objects = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<HeavyObject> vec;
    for (size_t i = 0; i < num_objects; ++i) {
        vec.emplace_back(1000);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "   Created " << num_objects << " objects\n";
    std::cout << "   Total resizes performed: ~" 
              << static_cast<int>(std::log2(num_objects)) << "\n";
    std::cout << "   Time: " << duration.count() << " ms\n";
    std::cout << "   Because move is noexcept, vector uses MOVE on resize\n";
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 6: PERFORMANCE COMPARISON\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "🚀 BENCHMARK: Vector growth with noexcept move\n";
    benchmark();
    std::cout << "\n";
    
    std::cout << "💡 WITHOUT NOEXCEPT:\n";
    std::cout << "   Same test would use COPY instead of MOVE\n";
    std::cout << "   Could be 10x-100x slower!\n";
}

} // namespace performance_comparison

// ===================================================================
// SECTION 7: BEST PRACTICES SUMMARY
// ===================================================================

namespace best_practices {

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 7: BEST PRACTICES SUMMARY\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✅ ALWAYS MARK NOEXCEPT:\n";
    std::cout << "   1. Destructors (implicit, but explicit is clearer)\n";
    std::cout << "   2. Move constructors\n";
    std::cout << "   3. Move assignment operators\n";
    std::cout << "   4. Swap functions\n";
    std::cout << "   5. Simple getters/setters (no allocation)\n";
    std::cout << "   6. Default constructors (if they don't allocate)\n\n";
    
    std::cout << "❌ NEVER MARK NOEXCEPT:\n";
    std::cout << "   1. Functions that allocate memory\n";
    std::cout << "   2. Functions that perform I/O\n";
    std::cout << "   3. Functions that validate/throw on bad input\n";
    std::cout << "   4. Copy constructors/assignment (allocation can fail)\n";
    std::cout << "   5. Functions calling non-noexcept functions\n\n";
    
    std::cout << "🔄 USE CONDITIONAL NOEXCEPT:\n";
    std::cout << "   1. Template functions wrapping user types\n";
    std::cout << "   2. Operations depending on template parameter traits\n";
    std::cout << "   3. When you want to preserve noexcept when possible\n\n";
    
    std::cout << "⚠️  CRITICAL WARNINGS:\n";
    std::cout << "   • Lying about noexcept = std::terminate() (instant crash)\n";
    std::cout << "   • No recovery possible, no stack unwinding\n";
    std::cout << "   • When in doubt, DON'T use noexcept\n";
    std::cout << "   • Better to allow exception than cause termination\n\n";
    
    std::cout << "🎯 THE GOLDEN RULE:\n";
    std::cout << "   'Use noexcept ONLY when you're 100% certain\n";
    std::cout << "    the function will NEVER throw under ANY circumstances'\n\n";
    
    std::cout << "📈 PERFORMANCE BENEFITS:\n";
    std::cout << "   • std::vector uses move instead of copy (10x-100x faster)\n";
    std::cout << "   • Compiler can optimize more aggressively\n";
    std::cout << "   • No exception handling overhead\n";
    std::cout << "   • Better code generation at call sites\n";
}

} // namespace best_practices

// ===================================================================
// MAIN - Demonstrate All Sections
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              NOEXCEPT BEST PRACTICES IN MODERN C++               ║\n";
    std::cout << "║          When to Use and When NOT to Use noexcept                ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    try {
        what_is_noexcept::demonstrate();
        always_use_noexcept::demonstrate();
        never_use_noexcept::demonstrate();
        conditional_noexcept::demonstrate();
        vector_optimization::demonstrate();
        performance_comparison::demonstrate();
        best_practices::demonstrate();
        
        std::cout << "\n╔══════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║           ALL NOEXCEPT CONCEPTS DEMONSTRATED!                    ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
