#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <vector>

// ===================================================================
// C++ SPECIAL MEMBER FUNCTIONS: RULE OF 3, 5, AND 0
// ===================================================================

// ===================================================================
// 1. RULE OF ZERO
// ===================================================================
// If you can avoid defining any special member functions (destructor,
// copy/move constructors, copy/move assignment), do so. Let the compiler
// generate them automatically. Use RAII wrappers like std::unique_ptr,
// std::shared_ptr, std::vector, std::string, etc.

class RuleOfZeroExample {
private:
    std::string name;
    std::vector<int> data;
    std::unique_ptr<int> ptr;
    
public:
    RuleOfZeroExample(const std::string& n, int value) 
        : name(n), data{1, 2, 3, 4, 5}, ptr(std::make_unique<int>(value)) {
        std::cout << "RuleOfZero: Constructor for " << name << std::endl;
    }
    
    // No destructor needed - std::string, std::vector, std::unique_ptr
    // handle cleanup automatically
    
    // No copy constructor needed - compiler generates correct one
    // (note: std::unique_ptr makes this non-copyable by default)
    
    // No copy assignment needed
    
    // No move constructor needed - compiler generates efficient one
    
    // No move assignment needed
    
    void display() const {
        std::cout << "  Name: " << name << ", Value: " << *ptr 
                  << ", Data size: " << data.size() << std::endl;
    }
};

void example_rule_of_zero() {
    std::cout << "\n=== RULE OF ZERO ===" << std::endl;
    std::cout << "Use RAII wrappers. No special member functions needed!\n" << std::endl;
    
    RuleOfZeroExample obj1("Object1", 42);
    obj1.display();
    
    // Move semantics work automatically
    RuleOfZeroExample obj2 = std::move(obj1);
    obj2.display();
    
    std::cout << "\n✓ Compiler-generated special members handle everything correctly" << std::endl;
}

// ===================================================================
// 2. RULE OF THREE (Pre-C++11)
// ===================================================================
// If you define any of the following, you should define all three:
// 1. Destructor
// 2. Copy constructor
// 3. Copy assignment operator

class RuleOfThreeExample {
private:
    int* data;
    size_t size;
    std::string name;
    
public:
    // Constructor
    RuleOfThreeExample(const std::string& n, size_t s) 
        : data(new int[s]), size(s), name(n) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<int>(i);
        }
        std::cout << "RuleOfThree: Constructor for " << name 
                  << " (size=" << size << ")" << std::endl;
    }
    
    // 1. Destructor
    ~RuleOfThreeExample() {
        std::cout << "RuleOfThree: Destructor for " << name << std::endl;
        delete[] data;
    }
    
    // 2. Copy constructor
    RuleOfThreeExample(const RuleOfThreeExample& other) 
        : data(new int[other.size]), size(other.size), name(other.name + "_copy") {
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        std::cout << "RuleOfThree: Copy constructor for " << name << std::endl;
    }
    
    // 3. Copy assignment operator
    RuleOfThreeExample& operator=(const RuleOfThreeExample& other) {
        std::cout << "RuleOfThree: Copy assignment for " << name << std::endl;
        
        if (this != &other) {
            // Free existing resource
            delete[] data;
            
            // Allocate new resource and copy
            size = other.size;
            data = new int[size];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
            name = other.name + "_assigned";
        }
        return *this;
    }
    
    void display() const {
        std::cout << "  " << name << " data: [";
        for (size_t i = 0; i < std::min(size, size_t(5)); ++i) {
            std::cout << data[i] << (i < std::min(size, size_t(5)) - 1 ? ", " : "");
        }
        std::cout << (size > 5 ? "..." : "") << "]" << std::endl;
    }
};

void example_rule_of_three() {
    std::cout << "\n=== RULE OF THREE ===" << std::endl;
    std::cout << "Define: Destructor, Copy Constructor, Copy Assignment\n" << std::endl;
    
    RuleOfThreeExample obj1("Original", 10);
    obj1.display();
    
    // Copy constructor
    RuleOfThreeExample obj2 = obj1;
    obj2.display();
    
    // Copy assignment
    RuleOfThreeExample obj3("Another", 5);
    obj3 = obj1;
    obj3.display();
    
    std::cout << "\n✓ All three special members ensure proper resource management" << std::endl;
}

// ===================================================================
// 3. RULE OF FIVE (C++11 and later)
// ===================================================================
// If you define any of the following, you should define all five:
// 1. Destructor
// 2. Copy constructor
// 3. Copy assignment operator
// 4. Move constructor
// 5. Move assignment operator

class RuleOfFiveExample {
private:
    int* data;
    size_t size;
    std::string name;
    
public:
    // Constructor
    RuleOfFiveExample(const std::string& n, size_t s) 
        : data(new int[s]), size(s), name(n) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<int>(i * 10);
        }
        std::cout << "RuleOfFive: Constructor for " << name 
                  << " (size=" << size << ")" << std::endl;
    }
    
    // 1. Destructor
    ~RuleOfFiveExample() {
        std::cout << "RuleOfFive: Destructor for " << name << std::endl;
        delete[] data;
    }
    
    // 2. Copy constructor
    RuleOfFiveExample(const RuleOfFiveExample& other) 
        : data(new int[other.size]), size(other.size), name(other.name + "_copy") {
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        std::cout << "RuleOfFive: Copy constructor for " << name << std::endl;
    }
    
    // 3. Copy assignment operator
    RuleOfFiveExample& operator=(const RuleOfFiveExample& other) {
        std::cout << "RuleOfFive: Copy assignment for " << name << std::endl;
        
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new int[size];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
            name = other.name + "_copy_assigned";
        }
        return *this;
    }
    
    // 4. Move constructor
    RuleOfFiveExample(RuleOfFiveExample&& other) noexcept
        : data(other.data), size(other.size), name(std::move(other.name) + "_moved") {
        other.data = nullptr;
        other.size = 0;
        std::cout << "RuleOfFive: Move constructor for " << name << std::endl;
    }
    
    // 5. Move assignment operator
    RuleOfFiveExample& operator=(RuleOfFiveExample&& other) noexcept {
        std::cout << "RuleOfFive: Move assignment for " << name << std::endl;
        
        if (this != &other) {
            delete[] data;
            
            data = other.data;
            size = other.size;
            name = std::move(other.name) + "_move_assigned";
            
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
    
    void display() const {
        if (data && size > 0) {
            std::cout << "  " << name << " data: [";
            for (size_t i = 0; i < std::min(size, size_t(5)); ++i) {
                std::cout << data[i] << (i < std::min(size, size_t(5)) - 1 ? ", " : "");
            }
            std::cout << (size > 5 ? "..." : "") << "]" << std::endl;
        } else {
            std::cout << "  " << name << " (moved-from, empty)" << std::endl;
        }
    }
};

void example_rule_of_five() {
    std::cout << "\n=== RULE OF FIVE ===" << std::endl;
    std::cout << "Define: Destructor, Copy Constructor, Copy Assignment, "
              << "Move Constructor, Move Assignment\n" << std::endl;
    
    RuleOfFiveExample obj1("Original", 8);
    obj1.display();
    
    // Copy constructor
    RuleOfFiveExample obj2 = obj1;
    obj2.display();
    
    // Move constructor
    RuleOfFiveExample obj3 = std::move(obj1);
    obj3.display();
    std::cout << "After move:" << std::endl;
    obj1.display();  // obj1 is in valid but moved-from state
    
    // Copy assignment
    RuleOfFiveExample obj4("Target1", 3);
    obj4 = obj2;
    obj4.display();
    
    // Move assignment
    RuleOfFiveExample obj5("Target2", 3);
    obj5 = std::move(obj2);
    obj5.display();
    std::cout << "After move:" << std::endl;
    obj2.display();  // obj2 is in valid but moved-from state
    
    std::cout << "\n✓ All five special members handle both copy and move semantics" << std::endl;
}

// ===================================================================
// 4. MODERN C++17/20 RELEVANCE
// ===================================================================

#include <optional>
#include <variant>

class ModernCpp17Example {
private:
    std::string name;
    std::vector<int> data;
    std::optional<std::string> description;  // C++17
    std::variant<int, double, std::string> value;  // C++17
    
public:
    ModernCpp17Example(const std::string& n)
        : name(n), data{1, 2, 3}, description("A modern class"), value(42) {
        std::cout << "Modern C++17 class: " << name << std::endl;
    }
    
    // NO special member functions needed!
    // Compiler generates everything correctly, including:
    // - Destructor (cleans up all RAII members)
    // - Copy constructor (deep copies std::string, std::vector, std::optional, std::variant)
    // - Copy assignment (properly assigns all members)
    // - Move constructor (efficient moves for all members)
    // - Move assignment (efficient move assigns)
    
    void display() const {
        std::cout << "  Name: " << name;
        if (description) {
            std::cout << ", Desc: " << *description;
        }
        std::cout << ", Variant holds: ";
        std::visit([](auto&& arg) { std::cout << arg; }, value);
        std::cout << std::endl;
    }
};

void example_modern_cpp_relevance() {
    std::cout << "\n=== MODERN C++17/20 RELEVANCE ===" << std::endl;
    
    std::cout << "\n🎯 RULE OF ZERO IS MORE RELEVANT THAN EVER!" << std::endl;
    std::cout << "\nC++17/20 additions that strengthen Rule of Zero:" << std::endl;
    std::cout << "  • std::optional<T> - Optional values (no more raw pointers!)" << std::endl;
    std::cout << "  • std::variant<Ts...> - Type-safe unions" << std::endl;
    std::cout << "  • std::any - Type-safe void*" << std::endl;
    std::cout << "  • std::string_view - Non-owning string references" << std::endl;
    std::cout << "  • std::span<T> (C++20) - Non-owning array views" << std::endl;
    std::cout << "  • std::make_shared array support (C++20)" << std::endl;
    
    ModernCpp17Example obj1("Original");
    obj1.display();
    
    std::cout << "\nCopying (compiler-generated):" << std::endl;
    ModernCpp17Example obj2 = obj1;
    obj2.display();
    
    std::cout << "\nMoving (compiler-generated):" << std::endl;
    ModernCpp17Example obj3 = std::move(obj1);
    obj3.display();
    
    std::cout << "\n✓ Zero manual resource management, all type-safe!" << std::endl;
}

// ===================================================================
// 5. COMPARISON: WHEN TO USE WHICH RULE
// ===================================================================

void example_comparison() {
    std::cout << "\n=== WHEN TO USE WHICH RULE (2026 PERSPECTIVE) ===" << std::endl;
    
    std::cout << "\n📋 RULE OF ZERO (STRONGLY PREFERRED - 95%+ of cases):" << std::endl;
    std::cout << "  ✓ Default choice for ALL new C++17/20 code" << std::endl;
    std::cout << "  ✓ Use std::unique_ptr, std::shared_ptr, std::vector, std::string" << std::endl;
    std::cout << "  ✓ Use std::optional, std::variant, std::any (C++17)" << std::endl;
    std::cout << "  ✓ Use std::span for views (C++20)" << std::endl;
    std::cout << "  ✓ Let compiler generate ALL special members" << std::endl;
    std::cout << "  ✓ Zero bugs from manual memory management" << std::endl;
    std::cout << "  ⚡ MOST RELEVANT IN MODERN C++!" << std::endl;
    
    std::cout << "\n📋 RULE OF THREE (LARGELY OBSOLETE):" << std::endl;
    std::cout << "  ⚠ Pre-C++11 guideline - avoid in new code" << std::endl;
    std::cout << "  ✓ Maintain in legacy codebases only" << std::endl;
    std::cout << "  ✓ Refactor to Rule of Zero when possible" << std::endl;
    std::cout << "  ❌ Don't use for new C++17/20 code" << std::endl;
    
    std::cout << "\n📋 RULE OF FIVE (RARELY NEEDED - <5% of cases):" << std::endl;
    std::cout << "  ✓ Only when Rule of Zero is impossible" << std::endl;
    std::cout << "  ✓ Custom allocators or exotic resources" << std::endl;
    std::cout << "  ✓ Performance-critical kernel/driver code" << std::endl;
    std::cout << "  ✓ Interop with C libraries or hardware" << std::endl;
    std::cout << "  ⚠ Consider if you really need it first" << std::endl;
    
    std::cout << "\n💡 MODERN C++17/20 BEST PRACTICES:" << std::endl;
    std::cout << "  1. ALWAYS start with Rule of Zero" << std::endl;
    std::cout << "  2. Use std::optional/variant instead of raw pointers" << std::endl;
    std::cout << "  3. Use std::unique_ptr for ownership" << std::endl;
    std::cout << "  4. Use std::span for non-owning views (C++20)" << std::endl;
    std::cout << "  5. Only implement special members if profiling proves necessary" << std::endl;
    std::cout << "  6. Use = default and = delete for explicit intent" << std::endl;
    std::cout << "  7. Mark move operations noexcept (enables optimizations)" << std::endl;
    
    std::cout << "\n🎯 REALITY CHECK (2026):" << std::endl;
    std::cout << "  • 95%+ of classes should use Rule of Zero" << std::endl;
    std::cout << "  • <5% need Rule of Five (custom resource managers)" << std::endl;
    std::cout << "  • Rule of Three is legacy knowledge only" << std::endl;
    std::cout << "  • Modern C++ makes manual memory management unnecessary" << std::endl;
}

// ===================================================================
// 5. EXPLICIT CONTROL: DEFAULT AND DELETE
// ===================================================================

class DefaultAndDeleteExample {
private:
    std::unique_ptr<int> ptr;
    
public:
    DefaultAndDeleteExample() : ptr(std::make_unique<int>(42)) {}
    
    // Explicitly defaulted destructor
    ~DefaultAndDeleteExample() = default;
    
    // Deleted copy operations (non-copyable)
    DefaultAndDeleteExample(const DefaultAndDeleteExample&) = delete;
    DefaultAndDeleteExample& operator=(const DefaultAndDeleteExample&) = delete;
    
    // Explicitly defaulted move operations
    DefaultAndDeleteExample(DefaultAndDeleteExample&&) = default;
    DefaultAndDeleteExample& operator=(DefaultAndDeleteExample&&) = default;
    
    int getValue() const { return *ptr; }
};

void example_default_and_delete() {
    std::cout << "\n=== DEFAULT AND DELETE ===" << std::endl;
    std::cout << "Explicitly control which special members are available\n" << std::endl;
    
    DefaultAndDeleteExample obj1;
    std::cout << "Created obj1, value: " << obj1.getValue() << std::endl;
    
    // DefaultAndDeleteExample obj2 = obj1;  // ERROR: Copy deleted
    
    DefaultAndDeleteExample obj3 = std::move(obj1);  // OK: Move defaulted
    std::cout << "Moved to obj3, value: " << obj3.getValue() << std::endl;
    
    std::cout << "\n✓ = delete prevents copying, = default enables moving" << std::endl;
}

// ===================================================================
// 6. EXPLICIT KEYWORD - PREVENTING IMPLICIT CONVERSIONS
// ===================================================================
// The explicit keyword prevents implicit type conversions that could
// lead to unexpected behavior or resource management issues

// BAD: Without explicit - allows implicit conversions
class ImplicitBuffer {
private:
    std::unique_ptr<int[]> data;
    size_t size;
    
public:
    // Constructor without explicit - DANGEROUS!
    ImplicitBuffer(size_t s) : data(std::make_unique<int[]>(s)), size(s) {
        std::cout << "ImplicitBuffer: Allocated " << size << " ints" << std::endl;
    }
    
    size_t getSize() const { return size; }
};

// GOOD: With explicit - prevents implicit conversions
class ExplicitBuffer {
private:
    std::unique_ptr<int[]> data;
    size_t size;
    
public:
    // Constructor with explicit - SAFE!
    explicit ExplicitBuffer(size_t s) : data(std::make_unique<int[]>(s)), size(s) {
        std::cout << "ExplicitBuffer: Allocated " << size << " ints" << std::endl;
    }
    
    size_t getSize() const { return size; }
};

// Example with conversion operators
class SmartInt {
private:
    int value;
    
public:
    explicit SmartInt(int v) : value(v) {}
    
    // C++11: explicit conversion operator
    explicit operator int() const { return value; }
    
    // Implicit conversion would be dangerous for bool
    explicit operator bool() const { return value != 0; }
};

void processBuffer(const ExplicitBuffer& buf) {
    std::cout << "  Processing buffer of size: " << buf.getSize() << std::endl;
}

void processImplicitBuffer(const ImplicitBuffer& buf) {
    std::cout << "  Processing implicit buffer of size: " << buf.getSize() << std::endl;
}

void example_explicit_keyword() {
    std::cout << "\n=== EXPLICIT KEYWORD ===" << std::endl;
    std::cout << "Prevents dangerous implicit conversions\n" << std::endl;
    
    // Without explicit - COMPILES BUT DANGEROUS!
    std::cout << "Without explicit keyword:" << std::endl;
    ImplicitBuffer buf1(10);  // OK: Direct initialization
    
    // DANGEROUS: Implicit conversion from int to ImplicitBuffer!
    processImplicitBuffer(100);  // Creates temporary ImplicitBuffer(100) - wasteful!
    
    // Can even do this nonsense:
    ImplicitBuffer buf2 = 50;  // Implicit conversion - looks like int assignment!
    std::cout << "  Created buffer with = 50 (confusing!)" << std::endl;
    
    std::cout << "\nWith explicit keyword:" << std::endl;
    ExplicitBuffer buf3(10);  // OK: Direct initialization
    
    // processBuffer(100);  // ERROR: Cannot implicitly convert int to ExplicitBuffer
    // ExplicitBuffer buf4 = 50;  // ERROR: Cannot use copy initialization
    
    std::cout << "  ✓ Must use direct initialization: ExplicitBuffer(100)" << std::endl;
    processBuffer(ExplicitBuffer(100));  // OK: Explicit construction
    
    // Explicit conversion operators (C++11)
    std::cout << "\nExplicit conversion operators:" << std::endl;
    SmartInt smart(42);
    
    // int x = smart;  // ERROR: Cannot implicitly convert to int
    int x = static_cast<int>(smart);  // OK: Explicit cast required
    std::cout << "  Explicit cast to int: " << x << std::endl;
    
    // if (smart) { }  // OK: Contextual conversion to bool allowed
    std::cout << "  ✓ Contextual bool conversion works in if/while statements" << std::endl;
    
    std::cout << "\n💡 MODERN C++ BEST PRACTICES:" << std::endl;
    std::cout << "  ✓ ALWAYS use explicit for single-parameter constructors" << std::endl;
    std::cout << "  ✓ Use explicit for conversion operators (except bool)" << std::endl;
    std::cout << "  ✓ Prevents accidental temporary object creation" << std::endl;
    std::cout << "  ✓ Prevents resource allocation surprises" << std::endl;
    std::cout << "  ✓ Makes code intent crystal clear" << std::endl;
    
    std::cout << "\n⚠️  WHEN IMPLICIT IS OK:" << std::endl;
    std::cout << "  • String literals to std::string: std::string s = \"hello\";" << std::endl;
    std::cout << "  • Initializer lists: std::vector<int> v = {1, 2, 3};" << std::endl;
    std::cout << "  • Copy/move constructors (never explicit)" << std::endl;
}

// ===================================================================
// 7. POLICY-BASED DESIGN - PARAMETERIZED RESOURCE MANAGEMENT
// ===================================================================
// Modern C++ technique: Use template parameters to customize behavior
// without runtime overhead or virtual functions

// Policy 1: Deletion Strategies
struct ArrayDelete {
    template<typename T>
    void operator()(T* ptr) const {
        delete[] ptr;
        std::cout << "    ArrayDelete: delete[] called" << std::endl;
    }
};

struct SingleDelete {
    template<typename T>
    void operator()(T* ptr) const {
        delete ptr;
        std::cout << "    SingleDelete: delete called" << std::endl;
    }
};

struct NoOpDelete {
    template<typename T>
    void operator()(T*) const {
        std::cout << "    NoOpDelete: no deletion (externally managed)" << std::endl;
    }
};

// Policy 2: Copy Strategies
struct DeepCopy {
    template<typename T>
    static T* copy(const T* src, size_t size) {
        std::cout << "    DeepCopy: allocating and copying " << size << " elements" << std::endl;
        T* dest = new T[size];
        std::copy(src, src + size, dest);
        return dest;
    }
};

struct ShallowCopy {
    template<typename T>
    static T* copy(const T* src, size_t) {
        std::cout << "    ShallowCopy: returning same pointer (reference counting)" << std::endl;
        return const_cast<T*>(src);  // Warning: For demo only!
    }
};

// Policy 3: Thread Safety
struct NoThreadSafety {
    void lock() const { /* no-op */ }
    void unlock() const { /* no-op */ }
};

struct BasicThreadSafety {
    void lock() const { 
        std::cout << "    BasicThreadSafety: acquiring lock" << std::endl;
    }
    void unlock() const { 
        std::cout << "    BasicThreadSafety: releasing lock" << std::endl;
    }
};

// Policy-based Resource Manager
template<
    typename T,
    typename DeletePolicy = ArrayDelete,
    typename CopyPolicy = DeepCopy,
    typename ThreadPolicy = NoThreadSafety
>
class PolicyBasedBuffer {
private:
    T* data_;
    size_t size_;
    DeletePolicy deleter_;
    ThreadPolicy thread_policy_;
    
public:
    // Constructor
    PolicyBasedBuffer(size_t s) : data_(new T[s]), size_(s) {
        std::cout << "  PolicyBasedBuffer: Constructor (size=" << size_ << ")" << std::endl;
    }
    
    // Destructor
    ~PolicyBasedBuffer() {
        std::cout << "  PolicyBasedBuffer: Destructor" << std::endl;
        thread_policy_.lock();
        deleter_(data_);
        thread_policy_.unlock();
    }
    
    // Copy constructor (uses CopyPolicy)
    PolicyBasedBuffer(const PolicyBasedBuffer& other) 
        : data_(CopyPolicy::copy(other.data_, other.size_)), 
          size_(other.size_) {
        std::cout << "  PolicyBasedBuffer: Copy constructor" << std::endl;
    }
    
    // Copy assignment
    PolicyBasedBuffer& operator=(const PolicyBasedBuffer& other) {
        if (this != &other) {
            std::cout << "  PolicyBasedBuffer: Copy assignment" << std::endl;
            thread_policy_.lock();
            deleter_(data_);
            data_ = CopyPolicy::copy(other.data_, other.size_);
            size_ = other.size_;
            thread_policy_.unlock();
        }
        return *this;
    }
    
    // Move constructor
    PolicyBasedBuffer(PolicyBasedBuffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "  PolicyBasedBuffer: Move constructor" << std::endl;
    }
    
    // Move assignment
    PolicyBasedBuffer& operator=(PolicyBasedBuffer&& other) noexcept {
        if (this != &other) {
            std::cout << "  PolicyBasedBuffer: Move assignment" << std::endl;
            thread_policy_.lock();
            deleter_(data_);
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
            thread_policy_.unlock();
        }
        return *this;
    }
    
    size_t size() const { return size_; }
    T* data() { return data_; }
};

// Type aliases for common configurations
template<typename T>
using ArrayBuffer = PolicyBasedBuffer<T, ArrayDelete, DeepCopy, NoThreadSafety>;

template<typename T>
using ThreadSafeBuffer = PolicyBasedBuffer<T, ArrayDelete, DeepCopy, BasicThreadSafety>;

template<typename T>
using SingleObjectBuffer = PolicyBasedBuffer<T, SingleDelete, DeepCopy, NoThreadSafety>;

void example_policy_based_design() {
    std::cout << "\n=== POLICY-BASED DESIGN ===" << std::endl;
    std::cout << "Parameterize behavior with template policies\n" << std::endl;
    
    std::cout << "1. Standard array buffer (ArrayDelete + DeepCopy):" << std::endl;
    {
        ArrayBuffer<int> buf1(10);
        std::cout << "   Copying..." << std::endl;
        ArrayBuffer<int> buf2 = buf1;
        std::cout << "   Moving..." << std::endl;
        ArrayBuffer<int> buf3 = std::move(buf1);
        std::cout << "   Cleanup:" << std::endl;
    }
    
    std::cout << "\n2. Thread-safe buffer (with BasicThreadSafety):" << std::endl;
    {
        ThreadSafeBuffer<int> buf(5);
        std::cout << "   Cleanup with thread safety:" << std::endl;
    }
    
    std::cout << "\n3. Custom policy combination (NoOpDelete for externally managed):" << std::endl;
    {
        PolicyBasedBuffer<int, NoOpDelete, DeepCopy, NoThreadSafety> buf(3);
        std::cout << "   Cleanup (no actual deletion):" << std::endl;
    }
    
    std::cout << "\n💡 ADVANTAGES OF POLICY-BASED DESIGN:" << std::endl;
    std::cout << "  ✓ Zero runtime overhead (compile-time selection)" << std::endl;
    std::cout << "  ✓ No virtual functions or vtables needed" << std::endl;
    std::cout << "  ✓ Highly composable and reusable" << std::endl;
    std::cout << "  ✓ Type-safe customization" << std::endl;
    std::cout << "  ✓ Optimized for each configuration" << std::endl;
    
    std::cout << "\n📚 REAL-WORLD USES:" << std::endl;
    std::cout << "  • std::unique_ptr<T, Deleter> - Custom deleters" << std::endl;
    std::cout << "  • STL containers with Allocator - Custom allocators" << std::endl;
    std::cout << "  • Thread-safety policies in concurrent code" << std::endl;
    std::cout << "  • Logging/tracing policies" << std::endl;
    std::cout << "  • Different storage strategies (stack vs heap)" << std::endl;
    
    std::cout << "\n🎯 MODERN C++ ALTERNATIVE:" << std::endl;
    std::cout << "  Use std::unique_ptr<T, Deleter> for most cases:" << std::endl;
    std::cout << "    auto ptr = std::unique_ptr<int[], ArrayDelete>(new int[10]);" << std::endl;
}

// ===================================================================
// 8. COPY-AND-SWAP IDIOM
// ===================================================================
// An elegant way to implement assignment operators

class CopyAndSwapExample {
private:
    int* data;
    size_t size;
    std::string name;
    
public:
    CopyAndSwapExample(const std::string& n, size_t s)
        : data(new int[s]), size(s), name(n) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<int>(i);
        }
    }
    
    ~CopyAndSwapExample() {
        delete[] data;
    }
    
    // Copy constructor
    CopyAndSwapExample(const CopyAndSwapExample& other)
        : data(new int[other.size]), size(other.size), name(other.name) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }
    
    // Move constructor
    CopyAndSwapExample(CopyAndSwapExample&& other) noexcept
        : data(other.data), size(other.size), name(std::move(other.name)) {
        other.data = nullptr;
        other.size = 0;
    }
    
    // Swap function
    friend void swap(CopyAndSwapExample& first, CopyAndSwapExample& second) noexcept {
        using std::swap;
        swap(first.data, second.data);
        swap(first.size, second.size);
        swap(first.name, second.name);
    }
    
    // Unified assignment operator (handles both copy and move)
    CopyAndSwapExample& operator=(CopyAndSwapExample other) {
        swap(*this, other);
        return *this;
    }
    
    void display() const {
        if (data) {
            std::cout << "  " << name << " [size=" << size << "]" << std::endl;
        } else {
            std::cout << "  " << name << " (empty)" << std::endl;
        }
    }
};

void example_copy_and_swap() {
    std::cout << "\n=== COPY-AND-SWAP IDIOM ===" << std::endl;
    std::cout << "Elegant and exception-safe assignment operator\n" << std::endl;
    
    CopyAndSwapExample obj1("Object1", 10);
    CopyAndSwapExample obj2("Object2", 5);
    
    obj1.display();
    obj2.display();
    
    std::cout << "\nAssigning obj1 = obj2 (copy):" << std::endl;
    obj1 = obj2;
    obj1.display();
    
    std::cout << "\nAssigning obj1 = std::move(obj2) (move):" << std::endl;
    obj1 = std::move(obj2);
    obj1.display();
    obj2.display();
    
    std::cout << "\n✓ Single assignment operator handles both copy and move!" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  C++ SPECIAL MEMBER FUNCTIONS: RULE OF 3, 5, AND 0" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    example_rule_of_zero();
    example_rule_of_three();
    example_rule_of_five();
    example_modern_cpp_relevance();
    example_comparison();
    example_default_and_delete();
    example_explicit_keyword();
    example_policy_based_design();
    example_copy_and_swap();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  SUMMARY" << std::endl;
    std::cout << "==🎯 RULE OF ZERO: Use in 95%+ of modern C++17/20 code!" << std::endl;
    std::cout << "   - std::unique_ptr, std::optional, std::variant, std::vector, etc." << std::endl;
    std::cout << "   - Zero manual memory management = zero bugs" << std::endl;
    
    std::cout << "\n📚 RULE OF THREE: Legacy guideline (pre-C++11)" << std::endl;
    std::cout << "   - Maintain in old codebases only" << std::endl;
    std::cout << "   - Refactor to Rule of Zero when possible" << std::endl;
    
    std::cout << "\n⚙️  RULE OF FIVE: Rarely needed (<5% of cases)" << std::endl;
    std::cout << "   - Only for custom resource managers" << std::endl;
    std::cout << "   - Performance-critical scenarios" << std::endl;
    
    std::cout << "\n💡 USE = DEFAULT AND = DELETE FOR EXPLICIT INTENT" << std::endl;
    std::cout << "✓ Copy-and-swap idiom when you need Rule of Five" << std::endl;
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
