#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// ===================================================================
// PRIVATE, PROTECTED, AND PUBLIC INHERITANCE IN C++
// ===================================================================

// ===================================================================
// 1. PUBLIC INHERITANCE - "IS-A" RELATIONSHIP
// ===================================================================

class Animal {
protected:
    std::string name;
    
public:
    Animal(const std::string& n) : name(n) {}
    virtual ~Animal() = default;
    
    virtual void makeSound() const {
        std::cout << name << " makes a sound" << std::endl;
    }
    
    void eat() const {
        std::cout << name << " is eating" << std::endl;
    }
};

// Public inheritance: Dog IS-A Animal
// All public members of Animal remain public in Dog
class Dog : public Animal {
public:
    Dog(const std::string& n) : Animal(n) {}
    
    void makeSound() const override {
        std::cout << name << " barks: Woof!" << std::endl;
    }
};

void example_public_inheritance() {
    std::cout << "\n=== 1. PUBLIC INHERITANCE (IS-A) ===" << std::endl;
    std::cout << "Use case: Dog IS-A Animal (polymorphism, substitutability)\n" << std::endl;
    
    Dog dog("Buddy");
    dog.makeSound();  // ✓ Can call inherited method
    dog.eat();        // ✓ Can call inherited method
    
    // ✓ Polymorphism works with public inheritance
    Animal* animal_ptr = &dog;
    animal_ptr->makeSound();  // Calls Dog::makeSound()
    
    std::cout << "\n✓ PUBLIC INHERITANCE:" << std::endl;
    std::cout << "   • Dog IS-A Animal" << std::endl;
    std::cout << "   • Public members stay public" << std::endl;
    std::cout << "   • Protected members stay protected" << std::endl;
    std::cout << "   • Polymorphism works (can upcast to base)" << std::endl;
    std::cout << "   • Use for: Substitutability, \"is-a\" relationships" << std::endl;
}

// ===================================================================
// 2. PRIVATE INHERITANCE - "IMPLEMENTED-IN-TERMS-OF"
// ===================================================================

// Base class providing timer functionality
class Timer {
private:
    int ticks = 0;
    
public:
    void tick() { ++ticks; }
    void reset() { ticks = 0; }
    int getTicks() const { return ticks; }
    
    void display() const {
        std::cout << "Timer: " << ticks << " ticks" << std::endl;
    }
};

// Private inheritance: Widget is IMPLEMENTED using Timer
// Timer's public interface becomes private in Widget
class Widget : private Timer {  // "implemented-in-terms-of"
private:
    std::string id;
    
public:
    Widget(const std::string& i) : id(i) {}
    
    // Widget controls which Timer methods to expose
    void update() {
        tick();  // ✓ Can call Timer::tick() internally
        std::cout << "Widget " << id << " updated" << std::endl;
    }
    
    void resetState() {
        reset();  // ✓ Can call Timer::reset() internally
    }
    
    // Expose only specific functionality
    void showUpdateCount() const {
        std::cout << "Widget " << id << " has been updated " 
                  << getTicks() << " times" << std::endl;
    }
    
    // Note: Timer::display() is NOT exposed
};

void example_private_inheritance() {
    std::cout << "\n=== 2. PRIVATE INHERITANCE (IMPLEMENTED-IN-TERMS-OF) ===" << std::endl;
    std::cout << "Use case: Widget is IMPLEMENTED using Timer (code reuse, not substitutability)\n" << std::endl;
    
    Widget widget("W1");
    widget.update();
    widget.update();
    widget.update();
    widget.showUpdateCount();
    
    // ❌ Cannot access Timer's public methods from outside
    // widget.tick();     // Compile error! tick() is private in Widget
    // widget.display();  // Compile error! display() is private in Widget
    
    // ❌ Cannot upcast to Timer (no polymorphism)
    // Timer* timer_ptr = &widget;  // Compile error! Private inheritance breaks is-a
    
    std::cout << "\n✓ PRIVATE INHERITANCE:" << std::endl;
    std::cout << "   • Widget is IMPLEMENTED using Timer" << std::endl;
    std::cout << "   • All base members become private" << std::endl;
    std::cout << "   • No polymorphism (cannot upcast)" << std::endl;
    std::cout << "   • Base class is an implementation detail" << std::endl;
    std::cout << "   • Use for: Code reuse without exposing interface" << std::endl;
}

// ===================================================================
// 3. PROTECTED INHERITANCE - "IMPLEMENTED-IN-TERMS-OF" FOR FURTHER DERIVATION
// ===================================================================

class Logger {
public:
    void log(const std::string& msg) const {
        std::cout << "[LOG] " << msg << std::endl;
    }
    
    void debug(const std::string& msg) const {
        std::cout << "[DEBUG] " << msg << std::endl;
    }
};

// Protected inheritance: Component uses Logger, allows derived classes to access it
class Component : protected Logger {
protected:
    std::string name;
    
public:
    Component(const std::string& n) : name(n) {
        log("Component " + name + " created");  // ✓ Can use Logger internally
    }
    
    void doWork() {
        debug("Component " + name + " is working");
    }
};

// Derived class can access Logger through protected inheritance
class AdvancedComponent : public Component {
public:
    AdvancedComponent(const std::string& n) : Component(n) {
        // ✓ Can access Logger methods because of protected inheritance
        log("Advanced features initialized");
    }
    
    void advancedWork() {
        debug("Advanced component processing");  // ✓ Logger accessible here
        doWork();
    }
};

void example_protected_inheritance() {
    std::cout << "\n=== 3. PROTECTED INHERITANCE (IMPLEMENTED-IN-TERMS-OF + DERIVATION) ===" << std::endl;
    std::cout << "Use case: Component uses Logger, derived classes can also use it\n" << std::endl;
    
    AdvancedComponent comp("AC1");
    comp.advancedWork();
    
    // ❌ Cannot access Logger from outside
    // comp.log("test");   // Compile error! log() is protected in Component
    // comp.debug("test"); // Compile error! debug() is protected in Component
    
    // ❌ Cannot upcast to Logger
    // Logger* logger_ptr = &comp;  // Compile error! Protected inheritance
    
    std::cout << "\n✓ PROTECTED INHERITANCE:" << std::endl;
    std::cout << "   • Component is IMPLEMENTED using Logger" << std::endl;
    std::cout << "   • All base public members become protected" << std::endl;
    std::cout << "   • Derived classes can access base interface" << std::endl;
    std::cout << "   • No polymorphism (cannot upcast)" << std::endl;
    std::cout << "   • Use for: Implementation detail shared with derived classes" << std::endl;
}

// ===================================================================
// 4. REAL-WORLD EXAMPLE: STACK IMPLEMENTED USING VECTOR
// ===================================================================

// Private inheritance: Stack is implemented using vector
template<typename T>
class Stack : private std::vector<T> {
    using Base = std::vector<T>;
    
public:
    // Expose only stack operations
    void push(const T& value) {
        Base::push_back(value);
    }
    
    void push(T&& value) {
        Base::push_back(std::move(value));
    }
    
    void pop() {
        if (!Base::empty()) {
            Base::pop_back();
        }
    }
    
    const T& top() const {
        return Base::back();
    }
    
    bool empty() const {
        return Base::empty();
    }
    
    size_t size() const {
        return Base::size();
    }
    
    // Note: We DON'T expose vector's random access, insert, erase, etc.
    // This enforces stack semantics (LIFO)
};

void example_stack_private_inheritance() {
    std::cout << "\n=== 4. REAL-WORLD: STACK USING PRIVATE INHERITANCE ===" << std::endl;
    std::cout << "Use case: Implement Stack using vector, but hide vector's interface\n" << std::endl;
    
    Stack<int> stack;
    stack.push(10);
    stack.push(20);
    stack.push(30);
    
    std::cout << "Stack top: " << stack.top() << std::endl;
    std::cout << "Stack size: " << stack.size() << std::endl;
    
    stack.pop();
    std::cout << "After pop, top: " << stack.top() << std::endl;
    
    // ❌ Vector operations are hidden
    // stack[0] = 100;        // Compile error! operator[] not accessible
    // stack.insert(...);     // Compile error! insert() not accessible
    // stack.at(0);           // Compile error! at() not accessible
    
    std::cout << "\n✓ BENEFIT:" << std::endl;
    std::cout << "   • Reuses vector's implementation" << std::endl;
    std::cout << "   • Enforces stack semantics (LIFO only)" << std::endl;
    std::cout << "   • Users cannot break stack invariants" << std::endl;
}

// ===================================================================
// 5. COMPARISON: PRIVATE INHERITANCE VS COMPOSITION
// ===================================================================

// Option 1: Private inheritance
class WindowPrivate : private Timer {
public:
    void refresh() {
        tick();
        std::cout << "Window refreshed " << getTicks() << " times" << std::endl;
    }
};

// Option 2: Composition (often preferred)
class WindowComposition {
private:
    Timer timer;  // Has-a Timer
    
public:
    void refresh() {
        timer.tick();
        std::cout << "Window refreshed " << timer.getTicks() << " times" << std::endl;
    }
};

void example_private_vs_composition() {
    std::cout << "\n=== 5. PRIVATE INHERITANCE VS COMPOSITION ===" << std::endl;
    
    std::cout << "\nPrivate inheritance:" << std::endl;
    WindowPrivate win1;
    win1.refresh();
    win1.refresh();
    
    std::cout << "\nComposition:" << std::endl;
    WindowComposition win2;
    win2.refresh();
    win2.refresh();
    
    std::cout << "\n📋 WHEN TO USE PRIVATE INHERITANCE:" << std::endl;
    std::cout << "   ✓ Need to override virtual functions from base" << std::endl;
    std::cout << "   ✓ Need access to protected members of base" << std::endl;
    std::cout << "   ✓ Want Empty Base Optimization (EBO) for zero-size bases" << std::endl;
    
    std::cout << "\n📋 WHEN TO USE COMPOSITION (USUALLY PREFERRED):" << std::endl;
    std::cout << "   ✓ Don't need to override virtual functions" << std::endl;
    std::cout << "   ✓ Don't need access to protected members" << std::endl;
    std::cout << "   ✓ Want to hold multiple instances" << std::endl;
    std::cout << "   ✓ Want more explicit \"has-a\" relationship" << std::endl;
    std::cout << "   ✓ Better encapsulation and flexibility" << std::endl;
}

// ===================================================================
// 6. OVERRIDING VIRTUAL FUNCTIONS WITH PRIVATE INHERITANCE
// ===================================================================

class Observable {
public:
    virtual ~Observable() = default;
    
    virtual void onEvent() {
        std::cout << "Observable: Event occurred" << std::endl;
    }
    
    void triggerEvent() {
        std::cout << "Triggering event..." << std::endl;
        onEvent();
    }
};

// Private inheritance to customize behavior
class Sensor : private Observable {
private:
    std::string sensorId;
    int reading = 0;
    
    // Override virtual function from Observable
    void onEvent() override {
        reading++;
        std::cout << "Sensor " << sensorId << " received event, reading: " << reading << std::endl;
    }
    
public:
    Sensor(const std::string& id) : sensorId(id) {}
    
    void simulate() {
        // Call base class method that will call our overridden onEvent()
        triggerEvent();
    }
};

void example_override_with_private() {
    std::cout << "\n=== 6. OVERRIDING VIRTUAL FUNCTIONS (PRIVATE INHERITANCE) ===" << std::endl;
    std::cout << "Use case: Customize base class behavior without exposing base interface\n" << std::endl;
    
    Sensor sensor("TEMP-01");
    sensor.simulate();
    sensor.simulate();
    sensor.simulate();
    
    std::cout << "\n✓ KEY POINT:" << std::endl;
    std::cout << "   • Can override virtual functions even with private inheritance" << std::endl;
    std::cout << "   • Base class doesn't need to know about derived class" << std::endl;
    std::cout << "   • This is the main reason to use private inheritance over composition" << std::endl;
}

// ===================================================================
// 7. USING DECLARATION TO SELECTIVELY EXPOSE MEMBERS
// ===================================================================

class Engine {
public:
    void start() { std::cout << "Engine started" << std::endl; }
    void stop() { std::cout << "Engine stopped" << std::endl; }
    void diagnose() { std::cout << "Running diagnostics..." << std::endl; }
    void internalMaintenance() { std::cout << "Internal maintenance" << std::endl; }
};

class Car : private Engine {
public:
    // Selectively expose only specific Engine methods
    using Engine::start;  // Make start() public
    using Engine::stop;   // Make stop() public
    
    // Note: diagnose() and internalMaintenance() remain private
    
    void drive() {
        start();
        std::cout << "Car is driving" << std::endl;
    }
};

void example_using_declaration() {
    std::cout << "\n=== 7. USING DECLARATION - SELECTIVE EXPOSURE ===" << std::endl;
    std::cout << "Use case: Expose only specific base class methods\n" << std::endl;
    
    Car car;
    car.start();     // ✓ Works - exposed via using declaration
    car.drive();
    car.stop();      // ✓ Works - exposed via using declaration
    
    // ❌ These are not exposed
    // car.diagnose();            // Compile error!
    // car.internalMaintenance(); // Compile error!
    
    std::cout << "\n✓ BENEFIT:" << std::endl;
    std::cout << "   • Fine-grained control over interface" << std::endl;
    std::cout << "   • Can expose some methods while hiding others" << std::endl;
    std::cout << "   • More maintainable than forwarding each method manually" << std::endl;
}

// ===================================================================
// 8. ACCESS LEVEL SUMMARY TABLE
// ===================================================================

void example_access_summary() {
    std::cout << "\n=== 8. ACCESS LEVEL SUMMARY ===" << std::endl;
    
    std::cout << "\n┌───────────────────────┬─────────────┬─────────────┬─────────────┐" << std::endl;
    std::cout << "│ Base Class Member     │   Public    │  Protected  │   Private   │" << std::endl;
    std::cout << "│ Access Level          │ Inheritance │ Inheritance │ Inheritance │" << std::endl;
    std::cout << "├───────────────────────┼─────────────┼─────────────┼─────────────┤" << std::endl;
    std::cout << "│ public                │   public    │  protected  │   private   │" << std::endl;
    std::cout << "│ protected             │  protected  │  protected  │   private   │" << std::endl;
    std::cout << "│ private               │   (hidden)  │   (hidden)  │   (hidden)  │" << std::endl;
    std::cout << "└───────────────────────┴─────────────┴─────────────┴─────────────┘" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  PRIVATE, PROTECTED, AND PUBLIC INHERITANCE IN C++" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    example_public_inheritance();
    example_private_inheritance();
    example_protected_inheritance();
    example_stack_private_inheritance();
    example_private_vs_composition();
    example_override_with_private();
    example_using_declaration();
    example_access_summary();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  DECISION GUIDE: WHICH INHERITANCE TYPE TO USE?" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n🎯 USE PUBLIC INHERITANCE WHEN:" << std::endl;
    std::cout << "   • Derived class IS-A base class" << std::endl;
    std::cout << "   • You need polymorphism and substitutability" << std::endl;
    std::cout << "   • Liskov Substitution Principle applies" << std::endl;
    std::cout << "   • Example: Dog is-a Animal, Circle is-a Shape" << std::endl;
    std::cout << "   • Usage: ~95% of all inheritance cases" << std::endl;
    
    std::cout << "\n🔧 USE PRIVATE INHERITANCE WHEN:" << std::endl;
    std::cout << "   • Derived class is IMPLEMENTED using base class" << std::endl;
    std::cout << "   • Need to override virtual functions from base" << std::endl;
    std::cout << "   • Need access to protected members of base" << std::endl;
    std::cout << "   • Want Empty Base Optimization (EBO)" << std::endl;
    std::cout << "   • Example: Stack implemented using vector" << std::endl;
    std::cout << "   • Usage: <5% of inheritance cases" << std::endl;
    std::cout << "   • Alternative: Prefer composition if possible" << std::endl;
    
    std::cout << "\n🛡️ USE PROTECTED INHERITANCE WHEN:" << std::endl;
    std::cout << "   • Same as private inheritance, but..." << std::endl;
    std::cout << "   • Further derived classes need access to base interface" << std::endl;
    std::cout << "   • Creating a hierarchy of implementation details" << std::endl;
    std::cout << "   • Example: Component hierarchy sharing Logger" << std::endl;
    std::cout << "   • Usage: <1% of inheritance cases (very rare)" << std::endl;
    std::cout << "   • Alternative: Usually better to use composition" << std::endl;
    
    std::cout << "\n📦 COMPOSITION VS PRIVATE INHERITANCE:" << std::endl;
    std::cout << "   • Default to COMPOSITION (has-a relationship)" << std::endl;
    std::cout << "   • Use private inheritance ONLY if:" << std::endl;
    std::cout << "     - Need to override virtual functions, OR" << std::endl;
    std::cout << "     - Need access to protected members, OR" << std::endl;
    std::cout << "     - Need Empty Base Optimization" << std::endl;
    std::cout << "   • Composition is more explicit and flexible" << std::endl;
    
    std::cout << "\n💡 MODERN C++ BEST PRACTICES:" << std::endl;
    std::cout << "   • Public inheritance: Use for polymorphism" << std::endl;
    std::cout << "   • Private inheritance: Rare, prefer composition" << std::endl;
    std::cout << "   • Protected inheritance: Almost never use" << std::endl;
    std::cout << "   • If unsure: Choose composition over private inheritance" << std::endl;
    std::cout << "   • Scott Meyers: \"Prefer composition to private inheritance\"" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
