# Event-Driven Programming Examples

This folder contains two contrasting approaches to event-driven programming in C++.

## Files Overview

### 1. EventDrivenProgramming_Lambdas.cpp ✅ **RECOMMENDED**
Modern C++ approach using:
- **`std::function`** - Type-erased callable wrappers
- **Lambdas** - Inline behavior with state capture `[=]`, `[&]`
- **`std::variant`** - Type-safe event unions (C++17)
- **No inheritance** - No observer/listener base classes needed

**Advantages:**
- 75% less boilerplate code
- Inline behavior definitions
- Automatic state capture
- Better readability
- Compile-time type safety

**Examples:**
1. Observer Pattern (lambda-based)
2. Signal/Slot Pattern
3. Event Dispatcher with `std::variant`
4. Callback Composition (Fluent API)
5. Reactive Observable
6. Event Queue
7. Command Pattern

### 2. EventDrivenProgramming_Inheritance.cpp ⚠️ **LEGACY APPROACH**
Traditional OOP approach using:
- **Abstract base classes** - `IObserver`, `IListener`, `ICommand`
- **Virtual functions** - Runtime polymorphism
- **Inheritance hierarchies** - Event classes, observer classes
- **Raw pointers** - Lifetime management issues

**Disadvantages:**
- Excessive boilerplate (interface + concrete classes)
- Behavior scattered across files
- Lifetime management complexity
- Need `dynamic_cast` for event hierarchies
- Harder to maintain and refactor

**Examples:**
1. Observer Pattern (inheritance-based)
2. Event Hierarchy with `dynamic_cast`
3. Listener Pattern (classic GUI style)
4. Command Pattern (class-based)
5. Callback Interface (old-school)

## Quick Comparison

| Feature | Lambdas Approach | Inheritance Approach |
|---------|------------------|---------------------|
| **Lines of code** | ~50 for observer | ~200 for observer |
| **Classes needed** | 0 (just lambdas) | 2+ per pattern |
| **Boilerplate** | Minimal | Extensive |
| **Readability** | Excellent (inline) | Poor (scattered) |
| **Type safety** | Compile-time | Runtime (RTTI) |
| **State capture** | Automatic | Manual |
| **Flexibility** | High | Low |
| **Maintenance** | Easy | Hard |

## When to Use Each Approach

### Use Lambdas When: ✅
- ✅ Simple callback scenarios (95% of cases)
- ✅ Need inline behavior definition
- ✅ Want to capture local state
- ✅ Prefer less boilerplate
- ✅ Building modern C++ codebases

### Use Inheritance When: ⚠️
- ⚠️ Need polymorphic object identity
- ⚠️ Complex state machines with many methods
- ⚠️ Plugin architectures with dynamic loading
- ⚠️ Performance-critical code (avoid `std::function` overhead)
- ⚠️ Multiple related virtual methods needed

## Code Examples

### Lambda Approach (Simple & Clean)
```cpp
Subject subject;

// No class needed! Just pass a lambda
subject.subscribe([](const std::string& event) {
    std::cout << "Event: " << event << std::endl;
});

// Can capture state inline
int count = 0;
subject.subscribe([&count](const std::string& event) {
    std::cout << "Event #" << ++count << ": " << event << std::endl;
});

subject.notify("Something happened");
```

### Inheritance Approach (Verbose & Complex)
```cpp
// Step 1: Define abstract interface
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void on_notify(const std::string& event) = 0;
};

// Step 2: Define concrete observer
class ConsoleObserver : public IObserver {
    void on_notify(const std::string& event) override {
        std::cout << "Event: " << event << std::endl;
    }
};

// Step 3: Create and attach observers
Subject subject;
ConsoleObserver obs;
subject.attach(&obs);  // Pointer lifetime issues!

subject.notify("Something happened");
```

## Key Takeaways

1. **Modern C++ prefers composition over inheritance**
2. **Lambdas eliminate the need for observer/listener classes**
3. **`std::function` provides type-safe callback storage**
4. **`std::variant` replaces event hierarchies**
5. **Code reduction: 75% less boilerplate with lambdas**

## Running the Examples

```bash
# Build both examples
cd build
cmake ..
make EventDrivenProgramming_Lambdas EventDrivenProgramming_Inheritance

# Run modern approach
./bin/EventDrivenProgramming_Lambdas

# Run legacy approach (for comparison)
./bin/EventDrivenProgramming_Inheritance
```

## Related Concepts

- **Observer Pattern**: Subject/Observer decoupling
- **Signal/Slot**: Qt-style event connections
- **Command Pattern**: Encapsulate actions with undo/redo
- **Reactive Programming**: Observable value changes
- **Event Dispatcher**: Central event routing

## Further Reading

- [C++ Core Guidelines: F.50 - Use lambdas when a function won't do](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#f50-use-a-lambda-when-a-function-wont-do-for-capturing-local-variables-or-for-writing-a-local-function)
- [std::function reference](https://en.cppreference.com/w/cpp/utility/functional/function)
- [std::variant reference](https://en.cppreference.com/w/cpp/utility/variant)
- [Lambda expressions in C++](https://en.cppreference.com/w/cpp/language/lambda)
