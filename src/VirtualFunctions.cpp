#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

// ===================================================================
// VIRTUAL FUNCTIONS AND PURE VIRTUAL FUNCTIONS IN MODERN C++
// ===================================================================
// Demonstrates:
// 1. Virtual functions vs pure virtual functions
// 2. Abstract classes and interfaces
// 3. Runtime polymorphism
// 4. Virtual destructors (critical!)
// 5. override and final keywords (C++11)
// 6. When to use vs alternatives (CRTP, concepts)
// 7. Performance considerations
// 8. Modern C++ best practices
// ===================================================================

// ===================================================================
// 1. PURE VIRTUAL FUNCTIONS - ABSTRACT INTERFACE
// ===================================================================
// Pure virtual function: = 0
// Cannot instantiate class with pure virtual functions
// Derived classes MUST implement all pure virtual functions

class IShape {
public:
    // Pure virtual function - no implementation
    virtual double area() const = 0;
    virtual double perimeter() const = 0;
    virtual void draw() const = 0;
    virtual std::string name() const = 0;
    
    // Virtual destructor is CRITICAL for polymorphic classes
    virtual ~IShape() = default;
    
    // Can have non-virtual functions too
    void info() const {
        std::cout << name() << ": area=" << area() 
                  << ", perimeter=" << perimeter() << std::endl;
    }
};

class Circle : public IShape {
private:
    double radius;
    
public:
    explicit Circle(double r) : radius(r) {}
    
    // Must implement ALL pure virtual functions
    double area() const override {
        return 3.14159 * radius * radius;
    }
    
    double perimeter() const override {
        return 2 * 3.14159 * radius;
    }
    
    void draw() const override {
        std::cout << "Drawing Circle (radius=" << radius << ")" << std::endl;
    }
    
    std::string name() const override {
        return "Circle";
    }
};

class Rectangle : public IShape {
private:
    double width, height;
    
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    double area() const override {
        return width * height;
    }
    
    double perimeter() const override {
        return 2 * (width + height);
    }
    
    void draw() const override {
        std::cout << "Drawing Rectangle (" << width << "x" << height << ")" << std::endl;
    }
    
    std::string name() const override {
        return "Rectangle";
    }
};

void example_pure_virtual() {
    std::cout << "\n=== 1. PURE VIRTUAL FUNCTIONS (ABSTRACT INTERFACE) ===" << std::endl;
    
    // IShape shape;  // ❌ ERROR: Cannot instantiate abstract class
    
    std::vector<std::unique_ptr<IShape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<Circle>(3.0));
    
    std::cout << "\nProcessing shapes polymorphically:" << std::endl;
    for (const auto& shape : shapes) {
        shape->draw();
        shape->info();
        std::cout << std::endl;
    }
    
    std::cout << "✓ PURE VIRTUAL (= 0):" << std::endl;
    std::cout << "   • Forces derived classes to implement" << std::endl;
    std::cout << "   • Creates abstract class (cannot instantiate)" << std::endl;
    std::cout << "   • Defines interface contract" << std::endl;
    std::cout << "   • Use for: Interfaces, abstract base classes" << std::endl;
}

// ===================================================================
// 2. VIRTUAL FUNCTIONS WITH DEFAULT IMPLEMENTATION
// ===================================================================
// Virtual function with implementation - can be overridden but not required

class Animal {
public:
    virtual ~Animal() = default;
    
    // Virtual with default implementation
    virtual void makeSound() const {
        std::cout << "Some generic animal sound" << std::endl;
    }
    
    // Virtual with default implementation
    virtual void eat() const {
        std::cout << "Animal is eating" << std::endl;
    }
    
    // Non-virtual - cannot override
    void breathe() const {
        std::cout << "Animal is breathing" << std::endl;
    }
};

class Dog : public Animal {
public:
    // Override with custom implementation
    void makeSound() const override {
        std::cout << "Woof! Woof!" << std::endl;
    }
    
    // Override eat
    void eat() const override {
        std::cout << "Dog is eating dog food" << std::endl;
    }
    
    // breathe() cannot be overridden (not virtual)
};

class Cat : public Animal {
public:
    // Only override makeSound
    void makeSound() const override {
        std::cout << "Meow!" << std::endl;
    }
    
    // eat() uses default implementation from Animal
};

void example_virtual_with_default() {
    std::cout << "\n=== 2. VIRTUAL FUNCTIONS WITH DEFAULT IMPLEMENTATION ===" << std::endl;
    
    std::vector<std::unique_ptr<Animal>> animals;
    animals.push_back(std::make_unique<Dog>());
    animals.push_back(std::make_unique<Cat>());
    
    std::cout << "\nDog:" << std::endl;
    animals[0]->makeSound();  // Custom implementation
    animals[0]->eat();        // Custom implementation
    animals[0]->breathe();    // Non-virtual (same for all)
    
    std::cout << "\nCat:" << std::endl;
    animals[1]->makeSound();  // Custom implementation
    animals[1]->eat();        // Uses default from Animal
    animals[1]->breathe();    // Non-virtual (same for all)
    
    std::cout << "\n✓ VIRTUAL WITH DEFAULT:" << std::endl;
    std::cout << "   • Provides base implementation" << std::endl;
    std::cout << "   • Derived classes can override if needed" << std::endl;
    std::cout << "   • Optional customization" << std::endl;
    std::cout << "   • Use for: Template Method pattern" << std::endl;
}

// ===================================================================
// 3. VIRTUAL DESTRUCTOR - CRITICAL FOR POLYMORPHISM!
// ===================================================================

class Base {
public:
    Base() {
        std::cout << "Base constructor" << std::endl;
    }
    
    // ❌ BAD: Non-virtual destructor in polymorphic class!
    ~Base() {
        std::cout << "Base destructor" << std::endl;
    }
};

class DerivedBad : public Base {
private:
    int* data;
    
public:
    DerivedBad() {
        data = new int[100];
        std::cout << "DerivedBad constructor (allocated 100 ints)" << std::endl;
    }
    
    ~DerivedBad() {
        delete[] data;
        std::cout << "DerivedBad destructor (freed memory)" << std::endl;
    }
};

class BaseGood {
public:
    BaseGood() {
        std::cout << "BaseGood constructor" << std::endl;
    }
    
    // ✓ GOOD: Virtual destructor!
    virtual ~BaseGood() {
        std::cout << "BaseGood destructor" << std::endl;
    }
};

class DerivedGood : public BaseGood {
private:
    int* data;
    
public:
    DerivedGood() {
        data = new int[100];
        std::cout << "DerivedGood constructor (allocated 100 ints)" << std::endl;
    }
    
    ~DerivedGood() override {
        delete[] data;
        std::cout << "DerivedGood destructor (freed memory)" << std::endl;
    }
};

void example_virtual_destructor() {
    std::cout << "\n=== 3. VIRTUAL DESTRUCTOR (CRITICAL!) ===" << std::endl;
    
    std::cout << "\n❌ BAD: Non-virtual destructor:" << std::endl;
    {
        Base* ptr = new DerivedBad();
        delete ptr;  // ⚠️ MEMORY LEAK! Only Base destructor called
    }
    
    std::cout << "\n✓ GOOD: Virtual destructor:" << std::endl;
    {
        BaseGood* ptr = new DerivedGood();
        delete ptr;  // ✓ Both destructors called correctly
    }
    
    std::cout << "\n⚠️ RULE: Always make destructor virtual if class has virtual functions!" << std::endl;
    std::cout << "   • Otherwise: Memory leaks, undefined behavior" << std::endl;
    std::cout << "   • Use: virtual ~ClassName() = default;" << std::endl;
}

// ===================================================================
// 4. OVERRIDE AND FINAL KEYWORDS (C++11)
// ===================================================================

class BaseKeywords {
public:
    virtual void foo() const {
        std::cout << "BaseKeywords::foo()" << std::endl;
    }
    
    virtual void bar() const {
        std::cout << "BaseKeywords::bar()" << std::endl;
    }
    
    virtual ~BaseKeywords() = default;
};

class DerivedKeywords : public BaseKeywords {
public:
    // override: Ensures we're actually overriding a base function
    void foo() const override {
        std::cout << "DerivedKeywords::foo()" << std::endl;
    }
    
    // ❌ This would cause compile error with override:
    // void fooo() const override { }  // Typo! No such function in base
    
    // final: Cannot be overridden further
    void bar() const final {
        std::cout << "DerivedKeywords::bar() - cannot override further" << std::endl;
    }
};

// final: Cannot be inherited from
class FinalClass final {
public:
    void method() {
        std::cout << "FinalClass::method()" << std::endl;
    }
};

// ❌ This would cause compile error:
// class CannotDeriveFromFinal : public FinalClass { };

void example_override_final() {
    std::cout << "\n=== 4. OVERRIDE AND FINAL KEYWORDS (C++11) ===" << std::endl;
    
    std::unique_ptr<BaseKeywords> ptr = std::make_unique<DerivedKeywords>();
    ptr->foo();
    ptr->bar();
    
    std::cout << "\n✓ OVERRIDE KEYWORD:" << std::endl;
    std::cout << "   • Catches typos at compile time" << std::endl;
    std::cout << "   • Documents intent clearly" << std::endl;
    std::cout << "   • Prevents accidental shadowing" << std::endl;
    std::cout << "   • ALWAYS use it when overriding!" << std::endl;
    
    std::cout << "\n✓ FINAL KEYWORD:" << std::endl;
    std::cout << "   • Prevents further overriding (function)" << std::endl;
    std::cout << "   • Prevents inheritance (class)" << std::endl;
    std::cout << "   • Enables compiler optimizations" << std::endl;
    std::cout << "   • Use for: Performance, design intent" << std::endl;
}

// ===================================================================
// 5. WHEN TO USE VIRTUAL FUNCTIONS IN MODERN C++
// ===================================================================

void example_when_to_use() {
    std::cout << "\n=== 5. WHEN TO USE VIRTUAL FUNCTIONS ===" << std::endl;
    
    std::cout << "\n✅ USE VIRTUAL FUNCTIONS WHEN:" << std::endl;
    std::cout << "   1. Runtime polymorphism needed" << std::endl;
    std::cout << "      - Heterogeneous containers (vector<Base*>)" << std::endl;
    std::cout << "      - Plugin architectures" << std::endl;
    std::cout << "      - Dynamic type selection" << std::endl;
    
    std::cout << "\n   2. Interface definition" << std::endl;
    std::cout << "      - Abstract base classes" << std::endl;
    std::cout << "      - Pure virtual functions (= 0)" << std::endl;
    std::cout << "      - Contract for derived classes" << std::endl;
    
    std::cout << "\n   3. Framework design" << std::endl;
    std::cout << "      - Template Method pattern" << std::endl;
    std::cout << "      - Strategy pattern" << std::endl;
    std::cout << "      - Factory pattern" << std::endl;
    
    std::cout << "\n   4. Open/Closed principle" << std::endl;
    std::cout << "      - Extend behavior without modifying base" << std::endl;
    std::cout << "      - Add new types without changing existing code" << std::endl;
}

// ===================================================================
// 6. ALTERNATIVES TO VIRTUAL FUNCTIONS (MODERN C++)
// ===================================================================

// CRTP (Curiously Recurring Template Pattern) - Static Polymorphism
template<typename Derived>
class SensorBase {
public:
    float read() {
        // Static cast to derived - no runtime overhead!
        return static_cast<Derived*>(this)->read_impl();
    }
};

class TemperatureSensor : public SensorBase<TemperatureSensor> {
public:
    float read_impl() {
        return 25.5f;  // Temperature reading
    }
};

class PressureSensor : public SensorBase<PressureSensor> {
public:
    float read_impl() {
        return 1013.25f;  // Pressure reading
    }
};

template<typename Sensor>
void process_sensor(Sensor& sensor) {
    // Compile-time polymorphism - fully inlineable!
    std::cout << "Sensor reading: " << sensor.read() << std::endl;
}

void example_alternatives() {
    std::cout << "\n=== 6. ALTERNATIVES TO VIRTUAL FUNCTIONS ===" << std::endl;
    
    std::cout << "\n1. CRTP (STATIC POLYMORPHISM):" << std::endl;
    TemperatureSensor temp;
    PressureSensor pressure;
    
    process_sensor(temp);
    process_sensor(pressure);
    
    std::cout << "\n   ✓ Advantages:" << std::endl;
    std::cout << "      • Zero runtime overhead (no vtable)" << std::endl;
    std::cout << "      • Fully inlineable" << std::endl;
    std::cout << "      • Faster than virtual functions" << std::endl;
    std::cout << "      • Good for embedded systems" << std::endl;
    
    std::cout << "\n   ✗ Disadvantages:" << std::endl;
    std::cout << "      • No heterogeneous containers" << std::endl;
    std::cout << "      • Compile-time binding only" << std::endl;
    std::cout << "      • More complex syntax" << std::endl;
    
    std::cout << "\n2. CONCEPTS (C++20):" << std::endl;
    std::cout << "   • Compile-time constraints" << std::endl;
    std::cout << "   • Duck typing with type safety" << std::endl;
    std::cout << "   • No inheritance needed" << std::endl;
    std::cout << "   • Example: template<Drawable T> void draw(T& obj)" << std::endl;
    
    std::cout << "\n3. std::variant (C++17):" << std::endl;
    std::cout << "   • Type-safe union" << std::endl;
    std::cout << "   • std::visit for polymorphic behavior" << std::endl;
    std::cout << "   • Value semantics (no pointers)" << std::endl;
    std::cout << "   • Good for small, closed set of types" << std::endl;
    
    std::cout << "\n4. std::function (C++11):" << std::endl;
    std::cout << "   • Type-erased callable" << std::endl;
    std::cout << "   • Works with lambdas, functors" << std::endl;
    std::cout << "   • Runtime overhead (like virtual)" << std::endl;
    std::cout << "   • Good for callbacks, strategies" << std::endl;
}

// ===================================================================
// 7. PERFORMANCE COMPARISON
// ===================================================================

class VirtualBase {
public:
    virtual int compute(int x) { return x * 2; }
    virtual ~VirtualBase() = default;
};

class VirtualDerived : public VirtualBase {
public:
    int compute(int x) override { return x * 3; }
};

void example_performance() {
    std::cout << "\n=== 7. PERFORMANCE CONSIDERATIONS ===" << std::endl;
    
    constexpr int iterations = 10'000'000;
    
    // Virtual function call
    VirtualBase* vptr = new VirtualDerived();
    auto start_virtual = std::chrono::high_resolution_clock::now();
    volatile int result_virtual = 0;
    for (int i = 0; i < iterations; ++i) {
        result_virtual += vptr->compute(i);
    }
    auto end_virtual = std::chrono::high_resolution_clock::now();
    auto duration_virtual = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_virtual - start_virtual).count();
    
    // Direct function call (non-virtual)
    VirtualDerived direct;
    auto start_direct = std::chrono::high_resolution_clock::now();
    volatile int result_direct = 0;
    for (int i = 0; i < iterations; ++i) {
        result_direct += direct.compute(i);
    }
    auto end_direct = std::chrono::high_resolution_clock::now();
    auto duration_direct = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_direct - start_direct).count();
    
    delete vptr;
    
    std::cout << "\nPerformance (10 million calls):" << std::endl;
    std::cout << "   Virtual function:  " << duration_virtual << " ms" << std::endl;
    std::cout << "   Direct call:       " << duration_direct << " ms" << std::endl;
    std::cout << "   Overhead:          ~" 
              << (duration_virtual - duration_direct) << " ms" << std::endl;
    
    std::cout << "\n💡 OVERHEAD SOURCES:" << std::endl;
    std::cout << "   1. Vtable lookup (8 bytes per object)" << std::endl;
    std::cout << "   2. Indirect function call (cache miss)" << std::endl;
    std::cout << "   3. Cannot be inlined by compiler" << std::endl;
    std::cout << "   4. Branch prediction harder" << std::endl;
    
    std::cout << "\n⚖️ TRADE-OFF:" << std::endl;
    std::cout << "   • Virtual functions: Small overhead, big flexibility" << std::endl;
    std::cout << "   • Usually worth it for abstraction benefits" << std::endl;
    std::cout << "   • Only optimize if profiling shows bottleneck" << std::endl;
}

// ===================================================================
// 8. MODERN C++ BEST PRACTICES
// ===================================================================

void example_best_practices() {
    std::cout << "\n=== 8. MODERN C++ BEST PRACTICES ===" << std::endl;
    
    std::cout << "\n✅ DO:" << std::endl;
    std::cout << "   1. ALWAYS use 'override' keyword" << std::endl;
    std::cout << "      ✓ void foo() override { }" << std::endl;
    std::cout << "      ✗ void foo() { }  // Easy to make mistakes" << std::endl;
    
    std::cout << "\n   2. ALWAYS make destructor virtual in polymorphic classes" << std::endl;
    std::cout << "      ✓ virtual ~Base() = default;" << std::endl;
    std::cout << "      ✗ ~Base() { }  // Memory leaks!" << std::endl;
    
    std::cout << "\n   3. Use 'final' to prevent further overriding" << std::endl;
    std::cout << "      ✓ void foo() final { }  // Design intent + optimization" << std::endl;
    
    std::cout << "\n   4. Use pure virtual (= 0) for interfaces" << std::endl;
    std::cout << "      ✓ virtual void draw() const = 0;  // Must implement" << std::endl;
    
    std::cout << "\n   5. Prefer smart pointers for polymorphic objects" << std::endl;
    std::cout << "      ✓ std::unique_ptr<Base> ptr = std::make_unique<Derived>();" << std::endl;
    std::cout << "      ✗ Base* ptr = new Derived();  // Manual delete needed" << std::endl;
    
    std::cout << "\n❌ DON'T:" << std::endl;
    std::cout << "   1. Don't use virtual for non-polymorphic classes" << std::endl;
    std::cout << "      - Unnecessary overhead" << std::endl;
    
    std::cout << "\n   2. Don't forget virtual destructor" << std::endl;
    std::cout << "      - Causes memory leaks and undefined behavior" << std::endl;
    
    std::cout << "\n   3. Don't call virtual functions in constructor/destructor" << std::endl;
    std::cout << "      - Derived class not fully constructed yet" << std::endl;
    std::cout << "      - Will call base version, not derived!" << std::endl;
    
    std::cout << "\n   4. Don't use virtual if CRTP/concepts work" << std::endl;
    std::cout << "      - Static polymorphism is faster" << std::endl;
    std::cout << "      - Good for performance-critical code" << std::endl;
}

// ===================================================================
// 9. ARE VIRTUAL FUNCTIONS STILL RELEVANT?
// ===================================================================

void example_still_relevant() {
    std::cout << "\n=== 9. ARE VIRTUAL FUNCTIONS STILL RELEVANT? ===" << std::endl;
    
    std::cout << "\n🎯 YES! Virtual functions are STILL HIGHLY RELEVANT:" << std::endl;
    
    std::cout << "\n1. RUNTIME POLYMORPHISM:" << std::endl;
    std::cout << "   • Containers of heterogeneous objects" << std::endl;
    std::cout << "   • Plugin systems and dynamic loading" << std::endl;
    std::cout << "   • GUI frameworks (Qt, wxWidgets)" << std::endl;
    std::cout << "   • Game engines (entity systems)" << std::endl;
    
    std::cout << "\n2. DESIGN PATTERNS:" << std::endl;
    std::cout << "   • Strategy, Observer, Command patterns" << std::endl;
    std::cout << "   • Factory method, Template method" << std::endl;
    std::cout << "   • Visitor pattern (double dispatch)" << std::endl;
    
    std::cout << "\n3. API DESIGN:" << std::endl;
    std::cout << "   • Stable binary interfaces (ABIs)" << std::endl;
    std::cout << "   • Dynamic libraries (.dll, .so)" << std::endl;
    std::cout << "   • Cross-module boundaries" << std::endl;
    
    std::cout << "\n4. SIMPLICITY:" << std::endl;
    std::cout << "   • Easier to understand than CRTP" << std::endl;
    std::cout << "   • More maintainable for most teams" << std::endl;
    std::cout << "   • Better error messages" << std::endl;
    
    std::cout << "\n🆕 MODERN ALTERNATIVES (C++11-20):" << std::endl;
    std::cout << "   • CRTP - Compile-time polymorphism" << std::endl;
    std::cout << "   • Concepts (C++20) - Duck typing with constraints" << std::endl;
    std::cout << "   • std::variant + std::visit (C++17)" << std::endl;
    std::cout << "   • std::function - Type-erased callables" << std::endl;
    
    std::cout << "\n⚖️ WHEN TO CHOOSE:" << std::endl;
    std::cout << "   Virtual Functions:" << std::endl;
    std::cout << "      ✓ Need runtime polymorphism" << std::endl;
    std::cout << "      ✓ Heterogeneous containers" << std::endl;
    std::cout << "      ✓ Plugin architectures" << std::endl;
    std::cout << "      ✓ Simplicity and maintainability" << std::endl;
    
    std::cout << "\n   CRTP/Concepts:" << std::endl;
    std::cout << "      ✓ Performance critical code" << std::endl;
    std::cout << "      ✓ Compile-time polymorphism sufficient" << std::endl;
    std::cout << "      ✓ Embedded systems (no vtable)" << std::endl;
    std::cout << "      ✓ Header-only libraries" << std::endl;
    
    std::cout << "\n💡 BOTTOM LINE:" << std::endl;
    std::cout << "   • Virtual functions: Core C++ feature, still essential" << std::endl;
    std::cout << "   • Modern C++ adds alternatives, not replacements" << std::endl;
    std::cout << "   • Choose based on requirements, not trends" << std::endl;
    std::cout << "   • Most codebases use BOTH virtual and static polymorphism" << std::endl;
}

// ===================================================================
// MAIN
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  VIRTUAL FUNCTIONS AND PURE VIRTUAL FUNCTIONS IN MODERN C++" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    example_pure_virtual();
    example_virtual_with_default();
    example_virtual_destructor();
    example_override_final();
    example_when_to_use();
    example_alternatives();
    example_performance();
    example_best_practices();
    example_still_relevant();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  SUMMARY: VIRTUAL FUNCTIONS IN MODERN C++" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n📚 KEY CONCEPTS:" << std::endl;
    std::cout << "   1. Pure virtual (= 0) - Abstract interface, must implement" << std::endl;
    std::cout << "   2. Virtual - Can override, optional" << std::endl;
    std::cout << "   3. Virtual destructor - ALWAYS for polymorphic classes" << std::endl;
    std::cout << "   4. override keyword - ALWAYS use when overriding" << std::endl;
    std::cout << "   5. final keyword - Prevent further overriding" << std::endl;
    
    std::cout << "\n✅ BEST PRACTICES:" << std::endl;
    std::cout << "   • Use 'override' on all virtual function overrides" << std::endl;
    std::cout << "   • Always virtual destructor for polymorphic classes" << std::endl;
    std::cout << "   • Prefer std::unique_ptr/shared_ptr over raw pointers" << std::endl;
    std::cout << "   • Consider CRTP/concepts for performance-critical code" << std::endl;
    std::cout << "   • Don't call virtual functions in constructors/destructors" << std::endl;
    
    std::cout << "\n🎯 STILL RELEVANT IN MODERN C++:" << std::endl;
    std::cout << "   • YES! Essential for runtime polymorphism" << std::endl;
    std::cout << "   • Plugin systems, frameworks, design patterns" << std::endl;
    std::cout << "   • Modern C++ adds alternatives, not replacements" << std::endl;
    std::cout << "   • Choose based on requirements, not trends" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
