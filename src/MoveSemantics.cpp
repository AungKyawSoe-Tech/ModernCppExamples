// ===================================================================
// std::move AND MOVE SEMANTICS - COMPREHENSIVE GUIDE
// ===================================================================
// Topics covered:
// 1. Lvalues vs Rvalues - What they are and why they matter
// 2. std::move - What it actually does (cast to rvalue reference)
// 3. Move constructor vs Copy constructor - Performance comparison
// 4. Why move semantics are important - Real-world scenarios
// 5. Common pitfalls and best practices
// 6. Perfect forwarding with std::forward
// ===================================================================

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <utility>  // for std::move, std::forward

// ===================================================================
// 1. LVALUES vs RVALUES - Understanding the Basics
// ===================================================================

void demonstrate_lvalue_rvalue() {
    std::cout << "\n=== 1. LVALUES vs RVALUES ===" << std::endl;
    
    std::cout << "\n📚 Theory:" << std::endl;
    std::cout << "   LVALUE = 'Locator value' - has a name, addressable, persists" << std::endl;
    std::cout << "   RVALUE = 'Right-hand value' - temporary, about to expire" << std::endl;
    
    // LVALUES - have names, can take address
    int x = 42;           // x is an lvalue
    int* ptr = &x;        // Can take address of x
    std::string name = "John";  // name is an lvalue
    
    std::cout << "\n✅ LVALUES (have names, addressable):" << std::endl;
    std::cout << "   int x = 42;           // x is lvalue, address: " << &x << std::endl;
    std::cout << "   string name = \"John\"; // name is lvalue, address: " << &name << std::endl;
    
    // RVALUES - temporaries, no name, about to be destroyed
    std::cout << "\n✅ RVALUES (temporaries, no name):" << std::endl;
    std::cout << "   42             // literal rvalue" << std::endl;
    std::cout << "   x + 5          // expression result is rvalue" << std::endl;
    std::cout << "   string(\"hi\")   // temporary object is rvalue" << std::endl;
    
    // x = 42;        // OK: x is lvalue, 42 is rvalue
    // 42 = x;        // ERROR: 42 is rvalue, can't assign to rvalue
    // &42;           // ERROR: can't take address of rvalue
    
    std::cout << "\n💡 Key insight:" << std::endl;
    std::cout << "   • Lvalues appear on LEFT of assignment: x = ..." << std::endl;
    std::cout << "   • Rvalues appear on RIGHT of assignment: ... = 42" << std::endl;
    std::cout << "   • Rvalues are about to die → safe to 'steal' their resources!" << std::endl;
}

// ===================================================================
// 2. std::move - What It Actually Does
// ===================================================================

void demonstrate_std_move() {
    std::cout << "\n=== 2. WHAT IS std::move? ===" << std::endl;
    
    std::cout << "\n🔍 std::move is NOT:" << std::endl;
    std::cout << "   ❌ Moving anything" << std::endl;
    std::cout << "   ❌ Transferring ownership" << std::endl;
    std::cout << "   ❌ Doing any work" << std::endl;
    
    std::cout << "\n✅ std::move IS:" << std::endl;
    std::cout << "   • A CAST from lvalue to rvalue reference" << std::endl;
    std::cout << "   • Tells compiler: 'This object is about to expire'" << std::endl;
    std::cout << "   • Enables move constructor/assignment to be called" << std::endl;
    
    std::cout << "\n📝 Conceptual implementation:" << std::endl;
    std::cout << "   template<typename T>" << std::endl;
    std::cout << "   typename remove_reference<T>::type&& move(T&& arg) {" << std::endl;
    std::cout << "       return static_cast<typename remove_reference<T>::type&&>(arg);" << std::endl;
    std::cout << "   }" << std::endl;
    
    std::string str1 = "Hello";
    
    std::cout << "\n🧪 Example:" << std::endl;
    std::cout << "   string str1 = \"Hello\";  // str1 is lvalue" << std::endl;
    std::cout << "   string str2 = str1;      // COPY: str1 is lvalue → copy constructor" << std::endl;
    std::cout << "   string str3 = std::move(str1); // MOVE: std::move(str1) is rvalue → move constructor" << std::endl;
    
    std::string str2 = str1;           // Copy
    std::string str3 = std::move(str1); // Move (str1 now in valid but unspecified state)
    
    std::cout << "\n   After move:" << std::endl;
    std::cout << "   str1 = \"" << str1 << "\" (moved-from, valid but empty)" << std::endl;
    std::cout << "   str2 = \"" << str2 << "\" (copy, unchanged)" << std::endl;
    std::cout << "   str3 = \"" << str3 << "\" (move, got str1's data)" << std::endl;
}

// ===================================================================
// 3. COPY vs MOVE - Performance Comparison
// ===================================================================

class LargeObject {
private:
    int* data;
    size_t size;
    std::string name;
    
public:
    // Constructor
    LargeObject(const std::string& n, size_t s) 
        : size(s), name(n) {
        data = new int[size];
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<int>(i);
        }
        std::cout << "   🏗️  Constructor: " << name 
                  << " (" << size << " elements)" << std::endl;
    }
    
    // Destructor
    ~LargeObject() {
        delete[] data;
        std::cout << "   💀 Destructor: " << name << std::endl;
    }
    
    // Copy constructor (EXPENSIVE)
    LargeObject(const LargeObject& other) 
        : size(other.size), name(other.name + "_copy") {
        data = new int[size];
        // Copy every element - EXPENSIVE!
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        std::cout << "   📋 COPY constructor: " << name 
                  << " (" << size << " elements copied)" << std::endl;
    }
    
    // Move constructor (CHEAP)
    LargeObject(LargeObject&& other) noexcept
        : data(other.data), size(other.size), name(other.name + "_moved") {
        // Steal the data pointer - CHEAP!
        other.data = nullptr;
        other.size = 0;
        std::cout << "   ⚡ MOVE constructor: " << name 
                  << " (pointer stolen, zero copy!)" << std::endl;
    }
    
    // Copy assignment (EXPENSIVE)
    LargeObject& operator=(const LargeObject& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            name = other.name + "_copy_assigned";
            data = new int[size];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
            std::cout << "   📋 COPY assignment: " << name << std::endl;
        }
        return *this;
    }
    
    // Move assignment (CHEAP)
    LargeObject& operator=(LargeObject&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            name = other.name + "_move_assigned";
            other.data = nullptr;
            other.size = 0;
            std::cout << "   ⚡ MOVE assignment: " << name << std::endl;
        }
        return *this;
    }
    
    size_t get_size() const { return size; }
};

void demonstrate_copy_vs_move() {
    std::cout << "\n=== 3. COPY vs MOVE CONSTRUCTORS ===" << std::endl;
    
    std::cout << "\n--- Copy Constructor (expensive) ---" << std::endl;
    {
        LargeObject obj1("obj1", 1000);
        LargeObject obj2 = obj1;  // COPY: obj1 is lvalue
        std::cout << "   Result: obj1 still has data, obj2 copied it" << std::endl;
    }
    
    std::cout << "\n--- Move Constructor (cheap) ---" << std::endl;
    {
        LargeObject obj3("obj3", 1000);
        LargeObject obj4 = std::move(obj3);  // MOVE: std::move(obj3) is rvalue
        std::cout << "   Result: obj3 empty (moved-from), obj4 stole its data" << std::endl;
    }
    
    std::cout << "\n--- Assignment operators ---" << std::endl;
    {
        LargeObject obj5("obj5", 500);
        LargeObject obj6("obj6", 500);
        LargeObject obj7("obj7", 500);
        
        std::cout << "\nCopy assignment:" << std::endl;
        obj6 = obj5;  // Copy assignment
        
        std::cout << "\nMove assignment:" << std::endl;
        obj7 = std::move(obj5);  // Move assignment
    }
    
    std::cout << "\n💡 Performance impact:" << std::endl;
    std::cout << "   • Copy: O(n) - copies every element" << std::endl;
    std::cout << "   • Move: O(1) - just copies pointers" << std::endl;
    std::cout << "   • For 1,000,000 elements: Copy = 1M ops, Move = 3 ops!" << std::endl;
}

// ===================================================================
// 4. WHY MOVE SEMANTICS ARE IMPORTANT
// ===================================================================

// Scenario 1: Returning large objects from functions

std::vector<int> create_large_vector_without_move() {
    std::vector<int> vec(1000000, 42);
    // Before C++11: Copy entire vector on return (SLOW!)
    // With C++11: RVO or move (FAST!)
    return vec;  // Move constructor called (or RVO)
}

// Scenario 2: Storing move-only types in containers

class MoveOnlyType {
    std::unique_ptr<int> ptr;
    
public:
    MoveOnlyType(int value) : ptr(std::make_unique<int>(value)) {
        std::cout << "   MoveOnlyType created with value: " << *ptr << std::endl;
    }
    
    // Delete copy constructor and copy assignment
    MoveOnlyType(const MoveOnlyType&) = delete;
    MoveOnlyType& operator=(const MoveOnlyType&) = delete;
    
    // Enable move constructor and move assignment
    MoveOnlyType(MoveOnlyType&&) noexcept = default;
    MoveOnlyType& operator=(MoveOnlyType&&) noexcept = default;
    
    int get_value() const { return *ptr; }
};

void demonstrate_why_move_important() {
    std::cout << "\n=== 4. WHY MOVE SEMANTICS ARE IMPORTANT ===" << std::endl;
    
    // Scenario 1: Return values
    std::cout << "\n📦 Scenario 1: Returning large objects" << std::endl;
    std::cout << "   Before C++11: Expensive copy on return" << std::endl;
    std::cout << "   With C++11: Move or RVO (Return Value Optimization)" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    auto vec = create_large_vector_without_move();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "   Created vector with " << vec.size() 
              << " elements in " << duration.count() << " μs" << std::endl;
    std::cout << "   (Would be much slower without move/RVO)" << std::endl;
    
    // Scenario 2: Move-only types
    std::cout << "\n🔒 Scenario 2: Move-only types (unique_ptr, thread, etc.)" << std::endl;
    {
        MoveOnlyType obj1(42);
        // MoveOnlyType obj2 = obj1;  // ERROR: Copy deleted
        MoveOnlyType obj2 = std::move(obj1);  // OK: Move
        std::cout << "   ✅ Moved unique ownership from obj1 to obj2" << std::endl;
    }
    
    // Scenario 3: Container operations
    std::cout << "\n📚 Scenario 3: Container optimizations" << std::endl;
    {
        std::vector<std::string> names;
        names.reserve(3);
        
        std::string name1 = "Alice";
        std::string name2 = "Bob";
        
        std::cout << "   push_back(name1): COPY (name1 still needed)" << std::endl;
        names.push_back(name1);  // Copy
        
        std::cout << "   push_back(std::move(name2)): MOVE (name2 not needed)" << std::endl;
        names.push_back(std::move(name2));  // Move
        
        std::cout << "   push_back(\"Charlie\"): MOVE (temporary rvalue)" << std::endl;
        names.push_back("Charlie");  // Move from temporary
        
        std::cout << "\n   After operations:" << std::endl;
        std::cout << "   name1 = \"" << name1 << "\" (still valid, was copied)" << std::endl;
        std::cout << "   name2 = \"" << name2 << "\" (moved-from, empty)" << std::endl;
    }
    
    // Scenario 4: Swapping
    std::cout << "\n🔄 Scenario 4: Efficient swapping" << std::endl;
    {
        std::vector<int> vec1(1000, 1);
        std::vector<int> vec2(2000, 2);
        
        std::cout << "   Before swap: vec1.size()=" << vec1.size() 
                  << ", vec2.size()=" << vec2.size() << std::endl;
        
        // std::swap uses move semantics internally
        std::swap(vec1, vec2);  // O(1) with moves, not O(n) with copies!
        
        std::cout << "   After swap:  vec1.size()=" << vec1.size() 
                  << ", vec2.size()=" << vec2.size() << std::endl;
        std::cout << "   (Swap is O(1) thanks to move semantics)" << std::endl;
    }
}

// ===================================================================
// 5. COMMON PITFALLS AND BEST PRACTICES
// ===================================================================

void demonstrate_pitfalls() {
    std::cout << "\n=== 5. COMMON PITFALLS ===" << std::endl;
    
    // Pitfall 1: Using moved-from object
    std::cout << "\n❌ Pitfall 1: Using moved-from object" << std::endl;
    {
        std::string str1 = "Hello";
        std::string str2 = std::move(str1);
        
        std::cout << "   str1 after move: \"" << str1 << "\"" << std::endl;
        std::cout << "   ⚠️  str1 is in valid but unspecified state!" << std::endl;
        std::cout << "   ✅ Safe: Check if empty or reassign" << std::endl;
        std::cout << "   ❌ Unsafe: Assume it has specific content" << std::endl;
        
        // Safe operations after move:
        str1.clear();        // OK
        str1 = "New value";  // OK
        if (str1.empty()) {} // OK
        // str1[0];          // UNSAFE! May be undefined behavior
    }
    
    // Pitfall 2: Moving const objects
    std::cout << "\n❌ Pitfall 2: Can't move from const" << std::endl;
    {
        const std::string str1 = "Hello";
        std::string str2 = std::move(str1);  // Calls COPY, not move!
        
        std::cout << "   const objects can't be moved" << std::endl;
        std::cout << "   std::move on const → copy constructor called" << std::endl;
    }
    
    // Pitfall 3: Unnecessary moves
    std::cout << "\n❌ Pitfall 3: Unnecessary std::move on return" << std::endl;
    std::cout << R"(
   // BAD - prevents RVO (Return Value Optimization)
   string bad_function() {
       string result = "value";
       return std::move(result);  // Don't do this!
   }
   
   // GOOD - enables RVO
   string good_function() {
       string result = "value";
       return result;  // Compiler automatically moves/optimizes
   }
)" << std::endl;
    
    // Best practices
    std::cout << "\n✅ BEST PRACTICES:" << std::endl;
    std::cout << "   1. Mark move constructors 'noexcept'" << std::endl;
    std::cout << "      → Enables optimizations (vector resize)" << std::endl;
    std::cout << "   2. Don't std::move on return values" << std::endl;
    std::cout << "      → Compiler does RVO or automatic move" << std::endl;
    std::cout << "   3. Move when you know object won't be used again" << std::endl;
    std::cout << "      → push_back(std::move(obj))" << std::endl;
    std::cout << "   4. Don't use moved-from objects" << std::endl;
    std::cout << "      → Unless reassigning or destroying" << std::endl;
    std::cout << "   5. Move-only types (unique_ptr) can't be copied" << std::endl;
    std::cout << "      → Use std::move to transfer ownership" << std::endl;
}

// ===================================================================
// 6. std::forward - PERFECT FORWARDING
// ===================================================================

void process(int& x) {
    std::cout << "   process(int&): lvalue " << x << std::endl;
}

void process(int&& x) {
    std::cout << "   process(int&&): rvalue " << x << std::endl;
}

// Without perfect forwarding - loses rvalue-ness
template<typename T>
void wrapper_bad(T&& arg) {
    // arg is always an lvalue inside the function (even if T&& is rvalue ref)
    process(arg);  // Always calls process(int&)
}

// With perfect forwarding - preserves value category
template<typename T>
void wrapper_good(T&& arg) {
    // std::forward preserves lvalue/rvalue-ness
    process(std::forward<T>(arg));  // Calls correct overload
}

void demonstrate_perfect_forwarding() {
    std::cout << "\n=== 6. std::forward - PERFECT FORWARDING ===" << std::endl;
    
    std::cout << "\n📚 Problem: Template functions lose value category" << std::endl;
    std::cout << "   Template parameter T&& is 'forwarding reference'" << std::endl;
    std::cout << "   But inside function, arg is always lvalue!" << std::endl;
    
    int x = 42;
    
    std::cout << "\n❌ Without std::forward:" << std::endl;
    wrapper_bad(x);           // lvalue
    wrapper_bad(100);         // rvalue, but treated as lvalue!
    
    std::cout << "\n✅ With std::forward:" << std::endl;
    wrapper_good(x);          // Forwards as lvalue
    wrapper_good(200);        // Forwards as rvalue
    
    std::cout << "\n💡 Key differences:" << std::endl;
    std::cout << "   • std::move:    ALWAYS casts to rvalue" << std::endl;
    std::cout << "   • std::forward: Casts to rvalue ONLY if original was rvalue" << std::endl;
    std::cout << "                   (preserves value category)" << std::endl;
    
    std::cout << "\n📝 When to use:" << std::endl;
    std::cout << "   • std::move:    When you know object is done being used" << std::endl;
    std::cout << "   • std::forward: In template functions forwarding arguments" << std::endl;
}

// ===================================================================
// 7. REAL-WORLD EXAMPLE: Factory Pattern
// ===================================================================

class Resource {
    std::string name;
    std::vector<int> data;
    
public:
    Resource(const std::string& n) : name(n) {
        data.resize(1000, 42);
        std::cout << "   Resource created: " << name << std::endl;
    }
    
    Resource(const Resource& other) 
        : name(other.name + "_copy"), data(other.data) {
        std::cout << "   Resource COPIED: " << name << std::endl;
    }
    
    Resource(Resource&& other) noexcept
        : name(std::move(other.name)), data(std::move(other.data)) {
        std::cout << "   Resource MOVED: " << name << std::endl;
    }
    
    const std::string& get_name() const { return name; }
};

class ResourceFactory {
public:
    // Return by value - move semantics makes this efficient
    static Resource create_resource(const std::string& name) {
        Resource res(name);
        // Return local object - move or RVO kicks in
        return res;  // Don't use std::move here!
    }
    
    // Store in container - use move to avoid copy
    static void store_in_container(std::vector<Resource>& container, Resource&& res) {
        std::cout << "   Storing in container..." << std::endl;
        container.push_back(std::move(res));  // Move into container
    }
};

void demonstrate_real_world() {
    std::cout << "\n=== 7. REAL-WORLD: Factory Pattern ===" << std::endl;
    
    std::vector<Resource> resources;
    resources.reserve(3);
    
    std::cout << "\n--- Creating and storing resources ---" << std::endl;
    
    // Create and store - efficient with move semantics
    auto res1 = ResourceFactory::create_resource("Resource1");
    ResourceFactory::store_in_container(resources, std::move(res1));
    
    // Direct temporary - even more efficient
    ResourceFactory::store_in_container(
        resources, 
        ResourceFactory::create_resource("Resource2")
    );
    
    std::cout << "\n✅ Container now has " << resources.size() 
              << " resources" << std::endl;
    std::cout << "   All stored efficiently with move semantics!" << std::endl;
}

// ===================================================================
// MAIN
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  std::move AND MOVE SEMANTICS - COMPREHENSIVE GUIDE" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    demonstrate_lvalue_rvalue();
    demonstrate_std_move();
    demonstrate_copy_vs_move();
    demonstrate_why_move_important();
    demonstrate_pitfalls();
    demonstrate_perfect_forwarding();
    demonstrate_real_world();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  SUMMARY: std::move and Move Semantics" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n🎯 KEY CONCEPTS:" << std::endl;
    std::cout << "\n1️⃣  Lvalue vs Rvalue:" << std::endl;
    std::cout << "   • Lvalue: Has name, addressable, persists" << std::endl;
    std::cout << "   • Rvalue: Temporary, about to expire, no name" << std::endl;
    
    std::cout << "\n2️⃣  std::move:" << std::endl;
    std::cout << "   • Just a CAST from lvalue to rvalue reference" << std::endl;
    std::cout << "   • Doesn't actually move anything" << std::endl;
    std::cout << "   • Tells compiler 'safe to steal resources'" << std::endl;
    
    std::cout << "\n3️⃣  Move Constructor/Assignment:" << std::endl;
    std::cout << "   • Steals resources from source object" << std::endl;
    std::cout << "   • O(1) instead of O(n) for large objects" << std::endl;
    std::cout << "   • Should be marked 'noexcept'" << std::endl;
    
    std::cout << "\n4️⃣  Why Important:" << std::endl;
    std::cout << "   • Performance: O(1) vs O(n) for copies" << std::endl;
    std::cout << "   • Enables move-only types (unique_ptr, thread)" << std::endl;
    std::cout << "   • Efficient return values and container ops" << std::endl;
    std::cout << "   • Essential for modern C++" << std::endl;
    
    std::cout << "\n5️⃣  Best Practices:" << std::endl;
    std::cout << "   ✅ DO: Mark move operations noexcept" << std::endl;
    std::cout << "   ✅ DO: Move when object not needed anymore" << std::endl;
    std::cout << "   ✅ DO: Use std::forward in template forwarding" << std::endl;
    std::cout << "   ❌ DON'T: std::move on return values (blocks RVO)" << std::endl;
    std::cout << "   ❌ DON'T: Use moved-from objects (unless reassigning)" << std::endl;
    std::cout << "   ❌ DON'T: Move const objects (silently copies)" << std::endl;
    
    std::cout << "\n📊 PERFORMANCE IMPACT:" << std::endl;
    std::cout << "   vector<int>(1,000,000):" << std::endl;
    std::cout << "   • Copy:  ~4,000,000 bytes copied, ~1ms" << std::endl;
    std::cout << "   • Move:  ~24 bytes copied, ~1μs (1000x faster!)" << std::endl;
    
    std::cout << "\n🔧 WHEN TO USE:" << std::endl;
    std::cout << "   • Returning local objects from functions" << std::endl;
    std::cout << "   • Inserting into containers (push_back)" << std::endl;
    std::cout << "   • Transferring unique ownership (unique_ptr)" << std::endl;
    std::cout << "   • Implementing swap operations" << std::endl;
    std::cout << "   • Last use of a variable before destruction" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
