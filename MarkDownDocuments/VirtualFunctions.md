# Virtual Functions and Pure Virtual Functions in Modern C++

## Overview

Virtual functions and pure virtual functions are **core C++ features** that remain **highly relevant** in modern C++ (C++11 through C++23). They enable **runtime polymorphism**, which is essential for many design patterns, frameworks, and architectural patterns.

## Table of Contents
1. [Pure Virtual Functions](#pure-virtual-functions)
2. [Virtual Functions with Default Implementation](#virtual-functions-with-default-implementation)
3. [Virtual Destructors (Critical!)](#virtual-destructors)
4. [Override and Final Keywords](#override-and-final-keywords)
5. [When to Use Virtual Functions](#when-to-use-virtual-functions)
6. [Alternatives in Modern C++](#alternatives-in-modern-cpp)
7. [Performance Considerations](#performance-considerations)
8. [Best Practices](#best-practices)
9. [Still Relevant?](#are-virtual-functions-still-relevant)

---

## Pure Virtual Functions

### Definition
A **pure virtual function** is declared with `= 0` and has no implementation in the base class.

```cpp
class IShape {
public:
    // Pure virtual function - no implementation
    virtual double area() const = 0;
    virtual void draw() const = 0;
    
    // Virtual destructor is CRITICAL!
    virtual ~IShape() = default;
};
```

### Key Characteristics
- **Cannot instantiate** a class with pure virtual functions (abstract class)
- **Derived classes MUST implement** all pure virtual functions
- Creates an **interface contract** that derived classes must follow
- Used for **interface definition** and **abstract base classes**

### Example

```cpp
class Circle : public IShape {
private:
    double radius;
    
public:
    explicit Circle(double r) : radius(r) {}
    
    // Must implement all pure virtual functions
    double area() const override {
        return 3.14159 * radius * radius;
    }
    
    void draw() const override {
        std::cout << "Drawing Circle" << std::endl;
    }
};

// Usage
std::vector<std::unique_ptr<IShape>> shapes;
shapes.push_back(std::make_unique<Circle>(5.0));
shapes[0]->draw();  // Polymorphic call
```

---

## Virtual Functions with Default Implementation

### Definition
A **virtual function** with a default implementation can be overridden but doesn't have to be.

```cpp
class Animal {
public:
    virtual ~Animal() = default;
    
    // Virtual with default implementation
    virtual void makeSound() const {
        std::cout << "Some generic animal sound" << std::endl;
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
};

class Cat : public Animal {
public:
    void makeSound() const override {
        std::cout << "Meow!" << std::endl;
    }
    // eat() uses default implementation from Animal
};
```

### Use Cases
- **Template Method pattern** - Base class defines algorithm structure
- **Optional customization** - Derived classes override only what they need
- **Sensible defaults** - Base class provides reasonable behavior

---

## Virtual Destructors

### ⚠️ CRITICAL RULE
**Always make the destructor virtual if your class has virtual functions!**

### The Problem

```cpp
class Base {
public:
    // ❌ BAD: Non-virtual destructor!
    ~Base() {
        std::cout << "Base destructor" << std::endl;
    }
};

class Derived : public Base {
private:
    int* data;
    
public:
    Derived() {
        data = new int[100];
    }
    
    ~Derived() {
        delete[] data;  // This won't be called!
        std::cout << "Derived destructor" << std::endl;
    }
};

// Memory leak!
Base* ptr = new Derived();
delete ptr;  // Only Base destructor called - MEMORY LEAK!
```

### The Solution

```cpp
class BaseGood {
public:
    // ✓ GOOD: Virtual destructor!
    virtual ~BaseGood() = default;
};

class DerivedGood : public BaseGood {
private:
    int* data;
    
public:
    DerivedGood() {
        data = new int[100];
    }
    
    ~DerivedGood() override {
        delete[] data;  // Correctly called
    }
};

// Correct cleanup
BaseGood* ptr = new DerivedGood();
delete ptr;  // Both destructors called correctly
```

### Rule of Thumb
```cpp
// If your class has virtual functions:
class Polymorphic {
public:
    virtual void someFunction() = 0;
    virtual ~Polymorphic() = default;  // ✓ ALWAYS!
};
```

---

## Override and Final Keywords

### The `override` Keyword (C++11)

**Always use `override` when overriding virtual functions!**

```cpp
class Base {
public:
    virtual void foo() const {
        std::cout << "Base::foo()" << std::endl;
    }
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    // ✓ GOOD: Catches typos at compile time
    void foo() const override {
        std::cout << "Derived::foo()" << std::endl;
    }
    
    // ❌ Compile error - no such function in base
    // void fooo() const override { }  // Typo caught!
};
```

### Benefits of `override`
- **Catches typos** at compile time
- **Documents intent** clearly
- **Prevents accidental shadowing** (function hiding)
- **Compiler error** if signature doesn't match base

### The `final` Keyword (C++11)

Prevents further overriding or inheritance.

```cpp
class Base {
public:
    virtual void foo() { }
    virtual void bar() { }
};

class Derived : public Base {
public:
    void foo() override { }
    
    // Cannot be overridden further
    void bar() final { }
};

// final class - cannot inherit from it
class FinalClass final {
public:
    void method() { }
};

// ❌ Compile error
// class CannotDeriveFromFinal : public FinalClass { };
```

---

## When to Use Virtual Functions

### ✅ Use Virtual Functions When:

#### 1. **Runtime Polymorphism Needed**
```cpp
// Heterogeneous container
std::vector<std::unique_ptr<IShape>> shapes;
shapes.push_back(std::make_unique<Circle>(5.0));
shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));

for (const auto& shape : shapes) {
    shape->draw();  // Runtime polymorphism
}
```

#### 2. **Interface Definition**
```cpp
// Abstract interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const std::string& message) = 0;
};

// Multiple implementations
class ConsoleLogger : public ILogger { /*...*/ };
class FileLogger : public ILogger { /*...*/ };
```

#### 3. **Design Patterns**
- **Strategy Pattern** - Encapsulate algorithms
- **Observer Pattern** - Event notification
- **Template Method** - Algorithm skeleton
- **Factory Method** - Object creation
- **Visitor Pattern** - Double dispatch

#### 4. **Plugin Architectures**
```cpp
// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual void initialize() = 0;
    virtual void execute() = 0;
};

// Load plugins at runtime
void loadPlugin(const std::string& pluginPath) {
    IPlugin* plugin = dlopen(pluginPath);
    plugin->initialize();
}
```

---

## Alternatives in Modern C++

### 1. CRTP (Curiously Recurring Template Pattern)

**Static polymorphism** - compile-time binding, zero runtime overhead.

```cpp
template<typename Derived>
class SensorBase {
public:
    float read() {
        // Static cast - no vtable!
        return static_cast<Derived*>(this)->read_impl();
    }
};

class TemperatureSensor : public SensorBase<TemperatureSensor> {
public:
    float read_impl() {
        return 25.5f;
    }
};

template<typename Sensor>
void process_sensor(Sensor& sensor) {
    // Compile-time polymorphism - fully inlineable!
    std::cout << sensor.read() << std::endl;
}
```

**Advantages:**
- ✓ Zero runtime overhead (no vtable)
- ✓ Fully inlineable
- ✓ Faster than virtual functions
- ✓ Good for embedded systems

**Disadvantages:**
- ✗ No heterogeneous containers
- ✗ Compile-time binding only
- ✗ More complex syntax
- ✗ Longer compile times

### 2. Concepts (C++20)

**Compile-time constraints** with duck typing.

```cpp
template<typename T>
concept Drawable = requires(T obj) {
    { obj.draw() } -> std::same_as<void>;
    { obj.area() } -> std::convertible_to<double>;
};

void render(Drawable auto& shape) {
    shape.draw();  // No inheritance needed!
}
```

**Advantages:**
- ✓ No inheritance needed
- ✓ Compile-time checking
- ✓ Better error messages
- ✓ Duck typing with type safety

### 3. std::variant + std::visit (C++17)

**Type-safe union** with compile-time polymorphism.

```cpp
struct Circle { double radius; };
struct Rectangle { double width, height; };

using Shape = std::variant<Circle, Rectangle>;

double calculateArea(const Shape& shape) {
    return std::visit([](auto&& s) {
        using T = std::decay_t<decltype(s)>;
        if constexpr (std::is_same_v<T, Circle>) {
            return 3.14159 * s.radius * s.radius;
        } else {
            return s.width * s.height;
        }
    }, shape);
}
```

**Advantages:**
- ✓ Value semantics (no pointers)
- ✓ No vtable overhead
- ✓ Compile-time dispatch
- ✓ Type-safe

**Disadvantages:**
- ✗ Closed set of types (must know all types upfront)
- ✗ More verbose
- ✗ Cannot add types at runtime

### 4. std::function (C++11)

**Type-erased callable** - works with lambdas, functors.

```cpp
class Service {
private:
    std::function<void(const std::string&)> logger;
    
public:
    void setLogger(std::function<void(const std::string&)> log) {
        logger = std::move(log);
    }
    
    void doWork() {
        logger("Working...");
    }
};

// Usage
Service service;
service.setLogger([](const std::string& msg) {
    std::cout << msg << std::endl;
});
```

---

## Performance Considerations

### Virtual Function Overhead

**Sources of overhead:**
1. **Vtable lookup** - 8 bytes per object (pointer to vtable)
2. **Indirect function call** - Pointer dereference + jump
3. **Not inlineable** - Compiler cannot inline virtual calls
4. **Cache misses** - Vtable and function code may not be in cache
5. **Branch prediction** - Harder for CPU to predict

### Typical Performance Impact

```cpp
// Benchmark: 10 million calls
Virtual function:  13 ms
Direct call:       10 ms
Overhead:          ~3 ms (30% slower)
```

**Per-call overhead:** 1-5 nanoseconds on modern hardware

### ⚖️ Trade-off

```
Virtual Functions:
  • Small overhead (~30% per call)
  • Big flexibility (runtime polymorphism)
  • Usually worth it for abstraction benefits
  
CRTP/Concepts:
  • Zero overhead
  • Less flexibility (compile-time only)
  • Good for performance-critical code
```

### When to Optimize

**Don't prematurely optimize!** Profile first:
1. **Measure** - Use profiler to find bottlenecks
2. **Optimize** - Only if virtual calls are in hot path
3. **Consider alternatives** - CRTP, concepts, or `final` keyword

**The `final` keyword enables optimization:**
```cpp
class Derived final : public Base {
    void foo() override { }
};

// Compiler can devirtualize calls to foo()
Derived d;
d.foo();  // Can be inlined!
```

---

## Best Practices

### ✅ DO

#### 1. **Always use `override` keyword**
```cpp
class Derived : public Base {
public:
    // ✓ Catches mistakes
    void foo() override { }
    
    // ✗ Easy to make errors
    // void foo() { }
};
```

#### 2. **Always virtual destructor for polymorphic classes**
```cpp
class Base {
public:
    virtual void foo() { }
    virtual ~Base() = default;  // ✓ CRITICAL!
};
```

#### 3. **Use `final` to prevent overriding**
```cpp
class Derived : public Base {
public:
    void foo() final { }  // Cannot override further
};
```

#### 4. **Use pure virtual (= 0) for interfaces**
```cpp
class IInterface {
public:
    virtual void method() = 0;  // Must implement
    virtual ~IInterface() = default;
};
```

#### 5. **Prefer smart pointers**
```cpp
// ✓ GOOD
std::unique_ptr<Base> ptr = std::make_unique<Derived>();

// ✗ BAD
Base* ptr = new Derived();
delete ptr;
```

### ❌ DON'T

#### 1. **Don't use virtual for non-polymorphic classes**
```cpp
// ✗ BAD: Unnecessary overhead
class NotPolymorphic {
    virtual void method() { }  // Why virtual?
};
```

#### 2. **Don't forget virtual destructor**
```cpp
// ✗ MEMORY LEAK!
class Base {
    ~Base() { }  // Not virtual!
};
```

#### 3. **Don't call virtual functions in constructor/destructor**
```cpp
class Base {
public:
    Base() {
        init();  // ❌ BAD! Calls Base::init(), not Derived::init()
    }
    
    virtual void init() { }
};

class Derived : public Base {
public:
    void init() override {
        // This is NOT called from Base constructor!
    }
};
```

**Why?** During construction:
- Derived class not fully constructed yet
- Virtual call to derived class would be unsafe
- C++ calls base class version

#### 4. **Don't use virtual if CRTP/concepts work**
```cpp
// If you only need compile-time polymorphism:
// ✓ Better: CRTP (zero overhead)
template<typename Derived>
class Base { /*...*/ };

// ✗ Unnecessary: Virtual (runtime overhead)
class Base {
    virtual void method() = 0;
};
```

---

## Are Virtual Functions Still Relevant?

### 🎯 YES! Highly Relevant in Modern C++

Virtual functions are **NOT outdated**. They remain essential for:

### 1. **Runtime Polymorphism**
- Heterogeneous containers (`std::vector<std::unique_ptr<Base>>`)
- Plugin systems and dynamic loading
- GUI frameworks (Qt, wxWidgets, Dear ImGui)
- Game engines (entity component systems)

### 2. **Design Patterns**
- Strategy, Observer, Command patterns
- Factory Method, Template Method
- Visitor pattern (double dispatch)
- Decorator, Proxy patterns

### 3. **API Design**
- Stable binary interfaces (ABIs)
- Dynamic libraries (.dll, .so)
- Cross-module boundaries
- Versioned APIs

### 4. **Simplicity and Maintainability**
- Easier to understand than CRTP
- More maintainable for most teams
- Better error messages
- Standard OOP approach

### 🆕 Modern C++ Alternatives (Not Replacements!)

C++11-C++20 introduced **alternatives**, not **replacements**:

| Feature | C++ Standard | Use Case |
|---------|--------------|----------|
| **Virtual Functions** | C++98 | Runtime polymorphism |
| **CRTP** | C++98 | Static polymorphism |
| **`override`/`final`** | C++11 | Safety + optimization |
| **`std::function`** | C++11 | Type-erased callables |
| **`std::variant` + `std::visit`** | C++17 | Closed set of types |
| **Concepts** | C++20 | Compile-time constraints |
| **`constexpr virtual`** | C++20 | Compile-time polymorphism |

### ⚖️ When to Choose

#### Choose **Virtual Functions** When:
- ✓ Need runtime polymorphism
- ✓ Heterogeneous containers
- ✓ Plugin architectures
- ✓ Simplicity and maintainability
- ✓ Open set of types (extensible)

#### Choose **CRTP/Concepts** When:
- ✓ Performance critical code
- ✓ Compile-time polymorphism sufficient
- ✓ Embedded systems (no vtable overhead)
- ✓ Header-only libraries
- ✓ Closed set of types

### 💡 Bottom Line

```
✅ Virtual functions: Core C++ feature, still essential
✅ Modern C++ adds alternatives, not replacements
✅ Choose based on requirements, not trends
✅ Most codebases use BOTH virtual and static polymorphism
```

**Real-world observation:**
- Large codebases (LLVM, Chromium, Qt): **Heavy use of virtual functions**
- Game engines (Unreal, Unity): **Virtual functions for interfaces**
- Embedded systems: **Mix of virtual and CRTP**
- Financial systems: **Virtual for plugins, CRTP for hot paths**

---

## Summary

### Key Concepts
1. **Pure virtual (= 0)** - Abstract interface, must implement
2. **Virtual** - Can override, optional
3. **Virtual destructor** - ALWAYS for polymorphic classes
4. **`override` keyword** - ALWAYS use when overriding
5. **`final` keyword** - Prevent further overriding

### Best Practices Checklist
- ✅ Use `override` on all overrides
- ✅ Always virtual destructor for polymorphic classes
- ✅ Prefer smart pointers over raw pointers
- ✅ Consider CRTP/concepts for performance-critical code
- ❌ Don't call virtual functions in constructors/destructors

### Modern C++ (C++11-C++23)
- **C++11:** `override`, `final`, `= default`
- **C++17:** `std::variant`, `std::visit`
- **C++20:** `constexpr virtual`, concepts
- **C++23:** No major changes (virtual functions mature)

### Final Verdict

> **Virtual functions are NOT outdated!**  
> They remain essential for runtime polymorphism.  
> Modern C++ enhances them with `override`, `final`, and provides alternatives like CRTP and concepts.  
> Use the right tool for the job: virtual for runtime polymorphism, CRTP/concepts for compile-time polymorphism.

---

## References

- **C++ Core Guidelines:** [C.128-C.139 (Hierarchy Class Rules)](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-class)
- **Effective C++** by Scott Meyers - Items 7, 34, 36, 37
- **More Effective C++** by Scott Meyers - Item 24
- **A Tour of C++** by Bjarne Stroustrup - Chapter 4 (Classes)

## See Also

- [InheritanceTypes.cpp](../src/InheritanceTypes.cpp) - Public, private, protected inheritance
- [ObjectSlicingSmartPtr.cpp](../src/ObjectSlicingSmartPtr.cpp) - Object slicing with smart pointers
- [DependencyInjection.cpp](../src/DependencyInjection.cpp) - Using interfaces for DI
- [EmbeddedSystemsProgramming.cpp](../src/EmbeddedSystemsProgramming.cpp) - CRTP alternative
