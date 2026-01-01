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
// 7. COPY-AND-SWAP IDIOM
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
