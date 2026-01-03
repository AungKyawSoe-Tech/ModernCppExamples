/*
 * CRTP vs Virtual Functions: Static vs Dynamic Polymorphism
 * 
 * This example demonstrates:
 * 1. How virtual functions work (vtable mechanism)
 * 2. Performance overhead of virtual functions
 * 3. CRTP (Curiously Recurring Template Pattern) as zero-overhead alternative
 * 4. When to use each approach
 * 5. Benchmarking and trade-offs
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>

// ============================================================================
// SECTION 1: VIRTUAL FUNCTIONS AND VTABLE MECHANISM
// ============================================================================

namespace virtual_functions {

/*
 * VTABLE (Virtual Function Table) EXPLANATION:
 * 
 * When you use virtual functions, the compiler creates:
 * 
 * 1. VTABLE (per class):
 *    - Static array of function pointers
 *    - One vtable per polymorphic class
 *    - Stored in read-only memory
 * 
 * 2. VPTR (per object):
 *    - Hidden pointer member in each object
 *    - Points to the class's vtable
 *    - Added automatically by compiler
 *    - Size overhead: 8 bytes (64-bit system)
 * 
 * MEMORY LAYOUT EXAMPLE:
 * 
 * class Base {
 *     virtual void foo();
 *     int data;
 * };
 * 
 * Object memory:  [vptr][data]
 *                   |
 *                   v
 * Vtable:        [&Base::foo][type_info*]
 * 
 * CALL MECHANISM:
 * obj.foo() becomes:
 *   1. Load vptr from object
 *   2. Index into vtable
 *   3. Load function pointer
 *   4. Call through pointer
 * 
 * PERFORMANCE COSTS:
 * • 2-3 extra memory loads (vptr + function pointer)
 * • Cannot inline (compiler doesn't know target at compile-time)
 * • Prevents devirtualization optimizations
 * • Cache misses (vtable might not be in cache)
 * • Branch prediction harder (indirect call)
 */

class AnimalBase {
public:
    virtual ~AnimalBase() = default;
    
    // Virtual function - uses vtable dispatch
    virtual void speak() const = 0;
    virtual std::string getName() const = 0;
    
    // Virtual function with implementation
    virtual void describe() const {
        std::cout << "   I am a " << getName() << "\n";
    }
};

class Dog : public AnimalBase {
public:
    void speak() const override {
        std::cout << "   🐕 Woof! Woof!\n";
    }
    
    std::string getName() const override {
        return "Dog";
    }
};

class Cat : public AnimalBase {
public:
    void speak() const override {
        std::cout << "   🐈 Meow! Meow!\n";
    }
    
    std::string getName() const override {
        return "Cat";
    }
};

class Bird : public AnimalBase {
public:
    void speak() const override {
        std::cout << "   🐦 Tweet! Tweet!\n";
    }
    
    std::string getName() const override {
        return "Bird";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 1: VIRTUAL FUNCTIONS (DYNAMIC POLYMORPHISM)\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 VTABLE MECHANISM:\n";
    std::cout << "   • Each object has hidden vptr (8 bytes overhead)\n";
    std::cout << "   • vptr points to class vtable\n";
    std::cout << "   • Function calls: load vptr -> index vtable -> call\n";
    std::cout << "   • Runtime overhead: 2-3 memory loads per virtual call\n\n";
    
    // Demonstrate dynamic polymorphism
    std::vector<std::unique_ptr<AnimalBase>> animals;
    animals.push_back(std::make_unique<Dog>());
    animals.push_back(std::make_unique<Cat>());
    animals.push_back(std::make_unique<Bird>());
    
    std::cout << "✅ ADVANTAGE: Runtime polymorphism (heterogeneous containers)\n";
    for (const auto& animal : animals) {
        animal->describe();
        animal->speak();
    }
    
    std::cout << "\n⚠️  SIZE OVERHEAD:\n";
    std::cout << "   sizeof(AnimalBase*) = " << sizeof(AnimalBase*) << " bytes\n";
    std::cout << "   sizeof(Dog) = " << sizeof(Dog) << " bytes (includes vptr)\n";
    std::cout << "   sizeof(Cat) = " << sizeof(Cat) << " bytes (includes vptr)\n";
}

} // namespace virtual_functions

// ============================================================================
// SECTION 2: STATIC MEMBER FUNCTIONS AND VTABLES
// ============================================================================

namespace static_functions {

/*
 * STATIC MEMBER FUNCTIONS AND VTABLES:
 * 
 * KEY FACT: Static member functions CANNOT be virtual!
 * 
 * WHY?
 * • Static functions don't have 'this' pointer
 * • No object instance to determine which vtable to use
 * • Called via class name, not object
 * • Resolved at compile-time, not runtime
 * 
 * IMPLICATION:
 * • Static functions don't appear in vtable
 * • Only ONE implementation exists (class-level, not object-level)
 * • Cannot be overridden in derived classes
 * • Zero runtime overhead (direct call)
 * 
 * WHEN TO USE:
 * • Factory functions
 * • Utility functions that don't need object state
 * • Performance-critical code that doesn't need polymorphism
 */

class Base {
public:
    virtual ~Base() = default;
    
    // Virtual function - in vtable
    virtual void instanceMethod() const {
        std::cout << "   Base::instanceMethod() - uses vtable\n";
    }
    
    // Static function - NOT in vtable, cannot be virtual
    static void staticMethod() {
        std::cout << "   Base::staticMethod() - no vtable, direct call\n";
    }
    
    // Non-virtual function - NOT in vtable
    void nonVirtualMethod() const {
        std::cout << "   Base::nonVirtualMethod() - no vtable, direct call\n";
    }
};

class Derived : public Base {
public:
    // Overrides virtual function - has entry in Derived's vtable
    void instanceMethod() const override {
        std::cout << "   Derived::instanceMethod() - uses vtable\n";
    }
    
    // CANNOT override static function (compile error if we tried)
    // This is a NEW function, not an override
    static void staticMethod() {
        std::cout << "   Derived::staticMethod() - different function!\n";
    }
    
    // Hides base class function (NOT override, no virtual)
    void nonVirtualMethod() const {
        std::cout << "   Derived::nonVirtualMethod() - hides base, no vtable\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 2: STATIC FUNCTIONS AND VTABLES\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 STATIC FUNCTIONS:\n";
    std::cout << "   • Cannot be virtual (no 'this' pointer)\n";
    std::cout << "   • NOT in vtable (only ONE implementation)\n";
    std::cout << "   • Called via class name, resolved at compile-time\n";
    std::cout << "   • Zero runtime overhead\n\n";
    
    Base* ptr = new Derived();
    
    std::cout << "✅ VIRTUAL FUNCTION (uses vtable):\n";
    ptr->instanceMethod(); // Calls Derived::instanceMethod via vtable
    
    std::cout << "\n✅ STATIC FUNCTION (no vtable, direct call):\n";
    Base::staticMethod();    // Calls Base::staticMethod
    Derived::staticMethod(); // Calls Derived::staticMethod (different function!)
    
    std::cout << "\n⚠️  NON-VIRTUAL FUNCTION (no override, no vtable):\n";
    ptr->nonVirtualMethod(); // Calls Base::nonVirtualMethod (no polymorphism!)
    
    std::cout << "\n💡 KEY INSIGHT:\n";
    std::cout << "   • Virtual functions: ONE entry in vtable per class\n";
    std::cout << "   • Static functions: ONE function for entire class hierarchy\n";
    std::cout << "   • Static means 'belongs to class, not object'\n";
    
    delete ptr;
}

} // namespace static_functions

// ============================================================================
// SECTION 3: CRTP - STATIC POLYMORPHISM (ZERO OVERHEAD)
// ============================================================================

namespace crtp_pattern {

/*
 * CRTP: Curiously Recurring Template Pattern
 * 
 * HOW IT WORKS:
 * • Base class is template parameterized by derived class
 * • Base uses static_cast to call derived methods
 * • Compiler resolves calls at compile-time
 * • NO vtable, NO vptr, NO runtime overhead
 * 
 * CALL MECHANISM:
 * obj.speak() becomes:
 *   1. Compiler knows exact type at compile-time
 *   2. Direct function call (no indirection)
 *   3. Can inline the function
 *   4. Zero runtime overhead
 * 
 * BENEFITS:
 * • Zero runtime overhead (no vtable lookup)
 * • Functions can be inlined
 * • No memory overhead (no vptr)
 * • Compiler can optimize aggressively
 * 
 * DRAWBACKS:
 * • Cannot use heterogeneous containers (no common base type)
 * • Type known at compile-time (no runtime polymorphism)
 * • More complex code (template metaprogramming)
 * • Longer compile times
 */

// CRTP Base class
template <typename Derived>
class Animal {
public:
    void speak() const {
        // Static cast to derived type - resolved at compile-time
        static_cast<const Derived*>(this)->speak_impl();
    }
    
    std::string getName() const {
        return static_cast<const Derived*>(this)->getName_impl();
    }
    
    void describe() const {
        std::cout << "   I am a " << getName() << "\n";
    }
    
    // Optional: Provide default implementation
    void defaultBehavior() const {
        std::cout << "   All animals need food and water\n";
    }
};

class Dog : public Animal<Dog> {
public:
    void speak_impl() const {
        std::cout << "   🐕 Woof! Woof!\n";
    }
    
    std::string getName_impl() const {
        return "Dog";
    }
};

class Cat : public Animal<Cat> {
public:
    void speak_impl() const {
        std::cout << "   🐈 Meow! Meow!\n";
    }
    
    std::string getName_impl() const {
        return "Cat";
    }
};

class Bird : public Animal<Bird> {
public:
    void speak_impl() const {
        std::cout << "   🐦 Tweet! Tweet!\n";
    }
    
    std::string getName_impl() const {
        return "Bird";
    }
};

// Template function that works with any Animal<T>
template <typename T>
void makeAnimalSpeak(const Animal<T>& animal) {
    animal.describe();
    animal.speak();
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 3: CRTP - STATIC POLYMORPHISM (ZERO OVERHEAD)\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "📚 CRTP MECHANISM:\n";
    std::cout << "   • Compile-time polymorphism (no runtime overhead)\n";
    std::cout << "   • No vtable, no vptr (zero memory overhead)\n";
    std::cout << "   • Direct function calls (can be inlined)\n";
    std::cout << "   • Compiler knows exact type at compile-time\n\n";
    
    Dog d;
    Cat c;
    Bird b;
    
    std::cout << "✅ DIRECT CALLS (statically resolved):\n";
    d.speak(); // Compiler knows this is Dog::speak_impl at compile-time
    c.speak(); // Compiler knows this is Cat::speak_impl at compile-time
    b.speak(); // Compiler knows this is Bird::speak_impl at compile-time
    
    std::cout << "\n✅ TEMPLATE FUNCTION (works with any Animal<T>):\n";
    makeAnimalSpeak(d);
    makeAnimalSpeak(c);
    makeAnimalSpeak(b);
    
    std::cout << "\n⚠️  SIZE COMPARISON:\n";
    std::cout << "   sizeof(Dog) = " << sizeof(Dog) << " bytes (NO vptr!)\n";
    std::cout << "   sizeof(Cat) = " << sizeof(Cat) << " bytes (NO vptr!)\n";
    std::cout << "   Compare to virtual_functions::Dog = " 
              << sizeof(virtual_functions::Dog) << " bytes (with vptr)\n";
    
    std::cout << "\n❌ LIMITATION: Cannot create heterogeneous container:\n";
    std::cout << "   // std::vector<Animal<??>> animals; // Won't work!\n";
    std::cout << "   // Dog and Cat are DIFFERENT types (Animal<Dog> vs Animal<Cat>)\n";
}

} // namespace crtp_pattern

// ============================================================================
// SECTION 4: PERFORMANCE COMPARISON
// ============================================================================

namespace performance_comparison {

// Virtual function version
class VirtualAnimal {
public:
    virtual ~VirtualAnimal() = default;
    virtual int compute(int x) const = 0;
};

class VirtualDog : public VirtualAnimal {
public:
    int compute(int x) const override {
        return x * x + x; // Simple computation
    }
};

// CRTP version
template <typename Derived>
class CRTPAnimal {
public:
    int compute(int x) const {
        return static_cast<const Derived*>(this)->compute_impl(x);
    }
};

class CRTPDog : public CRTPAnimal<CRTPDog> {
public:
    int compute_impl(int x) const {
        return x * x + x; // Same computation
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 4: PERFORMANCE BENCHMARKING\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    constexpr int ITERATIONS = 10'000'000;
    
    // Benchmark virtual functions
    auto start = std::chrono::high_resolution_clock::now();
    {
        VirtualDog dog;
        VirtualAnimal* ptr = &dog;
        long long sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += ptr->compute(i % 100); // Virtual call
        }
        // Use sum to prevent optimization
        if (sum > 0) {}
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto virtual_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    // Benchmark CRTP
    start = std::chrono::high_resolution_clock::now();
    {
        CRTPDog dog;
        long long sum = 0;
        for (int i = 0; i < ITERATIONS; ++i) {
            sum += dog.compute(i % 100); // Static call
        }
        // Use sum to prevent optimization
        if (sum > 0) {}
    }
    end = std::chrono::high_resolution_clock::now();
    auto crtp_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "🚀 BENCHMARK: " << ITERATIONS << " function calls\n\n";
    std::cout << "   Virtual functions:  " << std::setw(5) << virtual_time << " ms\n";
    std::cout << "   CRTP (static):      " << std::setw(5) << crtp_time << " ms\n";
    std::cout << "   Speedup:            " << std::setw(5) << std::fixed << std::setprecision(2) 
              << (double)virtual_time / crtp_time << "x\n\n";
    
    std::cout << "💡 WHY CRTP IS FASTER:\n";
    std::cout << "   • No vtable lookup (2-3 memory loads eliminated)\n";
    std::cout << "   • Function can be inlined (compiler sees implementation)\n";
    std::cout << "   • Better cache locality (no vtable indirection)\n";
    std::cout << "   • Better branch prediction (direct call)\n";
    std::cout << "   • Enables other compiler optimizations\n";
}

} // namespace performance_comparison

// ============================================================================
// SECTION 5: WHEN TO USE EACH APPROACH
// ============================================================================

namespace when_to_use {

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 5: WHEN TO USE VIRTUAL FUNCTIONS VS CRTP\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✅ USE VIRTUAL FUNCTIONS WHEN:\n\n";
    std::cout << "   1️⃣ RUNTIME POLYMORPHISM NEEDED:\n";
    std::cout << "      • Heterogeneous containers (vector<Base*>)\n";
    std::cout << "      • Plugin systems (load types at runtime)\n";
    std::cout << "      • Factory patterns\n";
    std::cout << "      • Type not known until runtime\n\n";
    
    std::cout << "   2️⃣ INTERFACE-BASED DESIGN:\n";
    std::cout << "      • Defining abstract interfaces\n";
    std::cout << "      • Separating interface from implementation\n";
    std::cout << "      • Dependency injection\n\n";
    
    std::cout << "   3️⃣ BINARY COMPATIBILITY:\n";
    std::cout << "      • DLLs/shared libraries\n";
    std::cout << "      • ABI stability requirements\n";
    std::cout << "      • Plugin systems across compilation units\n\n";
    
    std::cout << "   4️⃣ CODE SIMPLICITY:\n";
    std::cout << "      • Simpler code (no template metaprogramming)\n";
    std::cout << "      • Faster compile times\n";
    std::cout << "      • More familiar to developers\n\n";
    
    std::cout << "   5️⃣ PERFORMANCE NOT CRITICAL:\n";
    std::cout << "      • UI code, configuration, initialization\n";
    std::cout << "      • Virtual call overhead is negligible\n";
    std::cout << "      • Clarity more important than speed\n\n";
    
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✅ USE CRTP (STATIC POLYMORPHISM) WHEN:\n\n";
    std::cout << "   1️⃣ PERFORMANCE CRITICAL:\n";
    std::cout << "      • Hot loops (inner loops, tight iterations)\n";
    std::cout << "      • Real-time systems (low latency required)\n";
    std::cout << "      • Game engines (per-frame calculations)\n";
    std::cout << "      • High-frequency trading systems\n\n";
    
    std::cout << "   2️⃣ MEMORY CONSTRAINED:\n";
    std::cout << "      • Embedded systems (every byte counts)\n";
    std::cout << "      • Large arrays of objects (vptr overhead * N)\n";
    std::cout << "      • Cache-sensitive code\n\n";
    
    std::cout << "   3️⃣ COMPILE-TIME POLYMORPHISM SUFFICIENT:\n";
    std::cout << "      • Type known at compile-time\n";
    std::cout << "      • Template containers (vector<Dog>, vector<Cat>)\n";
    std::cout << "      • Generic algorithms\n\n";
    
    std::cout << "   4️⃣ INLINING REQUIRED:\n";
    std::cout << "      • Small functions that must be inlined\n";
    std::cout << "      • Zero-overhead abstractions needed\n";
    std::cout << "      • Maximum compiler optimization wanted\n\n";
    
    std::cout << "   5️⃣ TEMPLATE-BASED LIBRARIES:\n";
    std::cout << "      • STL-style libraries (iterators, algorithms)\n";
    std::cout << "      • Expression templates\n";
    std::cout << "      • Policy-based design\n\n";
    
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "⚖️  TRADE-OFFS SUMMARY:\n\n";
    std::cout << "   VIRTUAL FUNCTIONS:\n";
    std::cout << "   ✅ Runtime polymorphism\n";
    std::cout << "   ✅ Heterogeneous containers\n";
    std::cout << "   ✅ Simpler code\n";
    std::cout << "   ✅ Faster compile times\n";
    std::cout << "   ❌ Runtime overhead (vtable lookup)\n";
    std::cout << "   ❌ Memory overhead (vptr per object)\n";
    std::cout << "   ❌ Cannot inline\n\n";
    
    std::cout << "   CRTP (STATIC POLYMORPHISM):\n";
    std::cout << "   ✅ Zero runtime overhead\n";
    std::cout << "   ✅ Zero memory overhead\n";
    std::cout << "   ✅ Can inline functions\n";
    std::cout << "   ✅ Maximum performance\n";
    std::cout << "   ❌ No runtime polymorphism\n";
    std::cout << "   ❌ No heterogeneous containers\n";
    std::cout << "   ❌ More complex code\n";
    std::cout << "   ❌ Longer compile times\n\n";
    
    std::cout << "🎯 GOLDEN RULE:\n";
    std::cout << "   'Use virtual functions by default for flexibility.\n";
    std::cout << "    Use CRTP only when performance profiling shows\n";
    std::cout << "    virtual function overhead is a bottleneck.'\n\n";
    
    std::cout << "⚠️  PREMATURE OPTIMIZATION WARNING:\n";
    std::cout << "   • Don't use CRTP everywhere \"just in case\"\n";
    std::cout << "   • Profile first, optimize later\n";
    std::cout << "   • Virtual function overhead is often negligible\n";
    std::cout << "   • Code clarity often more valuable than tiny speedup\n";
}

} // namespace when_to_use

// ============================================================================
// SECTION 6: HYBRID APPROACH - BEST OF BOTH WORLDS
// ============================================================================

namespace hybrid_approach {

/*
 * HYBRID APPROACH: Combine virtual functions with CRTP
 * 
 * STRATEGY:
 * • Use virtual functions for high-level interfaces
 * • Use CRTP for performance-critical inner operations
 * • Get both flexibility and performance
 */

// Virtual base for runtime polymorphism
class RenderableBase {
public:
    virtual ~RenderableBase() = default;
    virtual void render() const = 0;
};

// CRTP for performance-critical operations
template <typename Derived>
class FastOperations {
public:
    void processPixel(int x, int y) const {
        static_cast<const Derived*>(this)->processPixel_impl(x, y);
    }
};

// Concrete class uses both
class Sprite : public RenderableBase, public FastOperations<Sprite> {
private:
    mutable int pixel_count = 0;
    
public:
    // Virtual function for high-level interface
    void render() const override {
        std::cout << "   [Sprite] Rendering...\n";
        // Performance-critical inner loop uses CRTP
        for (int y = 0; y < 100; ++y) {
            for (int x = 0; x < 100; ++x) {
                processPixel(x, y); // CRTP call (zero overhead)
            }
        }
        std::cout << "   [Sprite] Processed " << pixel_count << " pixels\n";
    }
    
    // CRTP implementation (inlined, zero overhead)
    void processPixel_impl(int x, int y) const {
        // Performance-critical pixel processing
        ++pixel_count;
        // Actual pixel operations would go here
        (void)x; (void)y; // Suppress unused warnings
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "SECTION 6: HYBRID APPROACH - BEST OF BOTH WORLDS\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "💡 STRATEGY:\n";
    std::cout << "   • Virtual functions for high-level interface\n";
    std::cout << "   • CRTP for performance-critical inner loops\n";
    std::cout << "   • Get both flexibility AND performance\n\n";
    
    std::cout << "✅ DEMONSTRATION:\n";
    Sprite sprite;
    RenderableBase* ptr = &sprite;
    
    // Virtual call for high-level operation
    ptr->render(); // Virtual call (small overhead, called once)
                   // Inner loop uses CRTP (zero overhead, called 10000x)
    
    std::cout << "\n🎯 BENEFIT:\n";
    std::cout << "   • Can store in vector<RenderableBase*> (runtime polymorphism)\n";
    std::cout << "   • Inner loop has zero overhead (CRTP inlining)\n";
    std::cout << "   • Best of both worlds!\n";
}

} // namespace hybrid_approach

// ============================================================================
// MAIN
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          CRTP vs VIRTUAL FUNCTIONS IN MODERN C++                  ║\n";
    std::cout << "║         Static vs Dynamic Polymorphism Trade-offs                 ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    
    virtual_functions::demonstrate();
    static_functions::demonstrate();
    crtp_pattern::demonstrate();
    performance_comparison::demonstrate();
    when_to_use::demonstrate();
    hybrid_approach::demonstrate();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "KEY TAKEAWAYS:\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1️⃣ VIRTUAL FUNCTIONS:\n";
    std::cout << "   • Use vtable (per class) and vptr (per object)\n";
    std::cout << "   • Runtime overhead: 2-3 memory loads per call\n";
    std::cout << "   • Memory overhead: 8 bytes per object (vptr)\n";
    std::cout << "   • Cannot inline virtual calls\n";
    std::cout << "   • Enable runtime polymorphism\n\n";
    
    std::cout << "2️⃣ STATIC MEMBER FUNCTIONS:\n";
    std::cout << "   • Cannot be virtual (no 'this' pointer)\n";
    std::cout << "   • Only ONE function per class hierarchy\n";
    std::cout << "   • Zero runtime overhead (direct call)\n";
    std::cout << "   • Not in vtable\n\n";
    
    std::cout << "3️⃣ CRTP (CURIOUSLY RECURRING TEMPLATE PATTERN):\n";
    std::cout << "   • Zero runtime overhead (compile-time resolution)\n";
    std::cout << "   • Zero memory overhead (no vptr)\n";
    std::cout << "   • Functions can be inlined\n";
    std::cout << "   • No heterogeneous containers\n";
    std::cout << "   • More complex code\n\n";
    
    std::cout << "4️⃣ WHEN TO USE WHAT:\n";
    std::cout << "   • Virtual: Flexibility, runtime polymorphism, simplicity\n";
    std::cout << "   • CRTP: Performance-critical code, compile-time known types\n";
    std::cout << "   • Hybrid: High-level virtual + low-level CRTP\n\n";
    
    std::cout << "5️⃣ PERFORMANCE:\n";
    std::cout << "   • Virtual overhead often negligible in real applications\n";
    std::cout << "   • CRTP can be 2-10x faster in tight loops\n";
    std::cout << "   • Profile before optimizing!\n\n";
    
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║           ALL CONCEPTS DEMONSTRATED SUCCESSFULLY!                  ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n\n";
    
    return 0;
}
