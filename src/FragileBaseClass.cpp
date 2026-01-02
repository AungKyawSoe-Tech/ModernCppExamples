// FragileBaseClass.cpp
// Demonstrates the Fragile Base Class Problem in C++ and its solutions
//
// The Fragile Base Class problem occurs when changes to a base class,
// which seem safe in isolation, inadvertently break the functionality
// of its derived (child) classes.
//
// KEY CONCEPTS:
// 1. Changes in Method Calls - base class refactoring breaks derived classes
// 2. Altering Data Layout - ABI compatibility issues
// 3. Virtual Function Overriding - unexpected behavior changes
//
// SOLUTIONS:
// 1. Composition over Inheritance
// 2. Hide Implementation Details (Encapsulation)
// 3. Use 'final' keyword
// 4. Design for Inheritance Explicitly
// 5. Prefer Interfaces/Abstract Classes

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// ============================================================================
// SECTION 1: THE PROBLEM - Fragile Base Class
// ============================================================================

namespace fragile_example {

// Version 1: Original base class
class Counter_V1 {
protected:
    int count_ = 0;
    
public:
    virtual void add(int value) {
        count_ += value;
        std::cout << "    [Base] Added " << value << ", count = " << count_ << "\n";
    }
    
    void addMultiple(const std::vector<int>& values) {
        for (int val : values) {
            count_ += val;  // Direct implementation
        }
        std::cout << "    [Base] Added multiple values, count = " << count_ << "\n";
    }
    
    int getCount() const { return count_; }
};

// Derived class works fine with V1
class LoggingCounter_V1 : public Counter_V1 {
private:
    std::vector<std::string> log_;
    
public:
    void add(int value) override {
        log_.push_back("Adding " + std::to_string(value));
        Counter_V1::add(value);
    }
    
    void printLog() const {
        std::cout << "    Log entries: " << log_.size() << "\n";
        for (const auto& entry : log_) {
            std::cout << "      - " << entry << "\n";
        }
    }
};

// ============================================================================
// Now developer "optimizes" the base class by refactoring
// ============================================================================

class Counter_V2 {
protected:
    int count_ = 0;
    
public:
    virtual void add(int value) {
        count_ += value;
        std::cout << "    [Base] Added " << value << ", count = " << count_ << "\n";
    }
    
    // FRAGILE CHANGE: Refactored to use add() for "code reuse"
    void addMultiple(const std::vector<int>& values) {
        for (int val : values) {
            add(val);  // Now calls virtual add()!
        }
        std::cout << "    [Base] Added multiple values, count = " << count_ << "\n";
    }
    
    int getCount() const { return count_; }
};

// Same derived class, now BROKEN!
class LoggingCounter_V2 : public Counter_V2 {
private:
    std::vector<std::string> log_;
    
public:
    void add(int value) override {
        log_.push_back("Adding " + std::to_string(value));
        Counter_V2::add(value);
    }
    
    void printLog() const {
        std::cout << "    Log entries: " << log_.size() << "\n";
        for (const auto& entry : log_) {
            std::cout << "      - " << entry << "\n";
        }
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 1: The Fragile Base Class Problem ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Scenario: Developer refactors base class to call virtual method\n\n";
    
    std::cout << "1. Original Version (V1) - Works as expected:\n";
    LoggingCounter_V1 counter1;
    counter1.addMultiple({1, 2, 3});
    counter1.printLog();
    std::cout << "   Expected: No log entries (addMultiple bypasses add())\n";
    std::cout << "   Actual: " << (counter1.printLog(), "✓ Works!\n");
    
    std::cout << "\n2. After Refactoring (V2) - BROKEN:\n";
    LoggingCounter_V2 counter2;
    counter2.addMultiple({1, 2, 3});
    counter2.printLog();
    std::cout << "   Problem: Now creates log entries unexpectedly!\n";
    std::cout << "   The derived class behavior changed without any modification!\n";
}

} // namespace fragile_example

// ============================================================================
// SECTION 2: SOLUTION 1 - Composition Over Inheritance
// ============================================================================

namespace composition_solution {

// Internal implementation (not exposed for inheritance)
class CounterImpl {
private:
    int count_ = 0;
    
public:
    void add(int value) {
        count_ += value;
    }
    
    void addMultiple(const std::vector<int>& values) {
        for (int val : values) {
            add(val);  // Safe - not virtual
        }
    }
    
    int getCount() const { return count_; }
};

// Use composition instead of inheritance
class LoggingCounter {
private:
    CounterImpl counter_;  // Composition
    std::vector<std::string> log_;
    
public:
    void add(int value) {
        log_.push_back("Adding " + std::to_string(value));
        counter_.add(value);
    }
    
    void addMultiple(const std::vector<int>& values) {
        for (int val : values) {
            counter_.add(val);  // Direct delegation, no logging
        }
    }
    
    int getCount() const { return counter_.getCount(); }
    
    void printLog() const {
        std::cout << "    Log entries: " << log_.size() << "\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 2: Solution 1 - Composition Over Inheritance ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✓ Using composition - behavior is explicit and controlled\n";
    LoggingCounter counter;
    counter.add(5);
    counter.addMultiple({1, 2, 3});
    std::cout << "  Count: " << counter.getCount() << "\n";
    counter.printLog();
    std::cout << "  Benefit: No fragility - implementation changes don't break us\n";
}

} // namespace composition_solution

// ============================================================================
// SECTION 3: SOLUTION 2 - Hide Implementation Details
// ============================================================================

namespace encapsulation_solution {

class Counter {
private:
    int count_ = 0;
    
    // Private helper - derived classes cannot rely on this
    void incrementInternal(int value) {
        count_ += value;
    }
    
protected:
    // Protected interface for derived classes
    void setCount(int value) { count_ = value; }
    int getCountInternal() const { return count_; }
    
public:
    virtual void add(int value) {
        incrementInternal(value);
    }
    
    // Non-virtual - cannot be overridden
    void addMultiple(const std::vector<int>& values) {
        for (int val : values) {
            incrementInternal(val);  // Calls private method
        }
    }
    
    int getCount() const { return count_; }
};

class LoggingCounter : public Counter {
private:
    std::vector<std::string> log_;
    
public:
    void add(int value) override {
        log_.push_back("Adding " + std::to_string(value));
        Counter::add(value);
    }
    
    void printLog() const {
        std::cout << "    Log entries: " << log_.size() << "\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 3: Solution 2 - Hide Implementation Details ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✓ Private implementation, public/protected interface only\n";
    LoggingCounter counter;
    counter.add(5);
    counter.addMultiple({1, 2, 3});
    std::cout << "  Count: " << counter.getCount() << "\n";
    counter.printLog();
    std::cout << "  Benefit: addMultiple() is non-virtual and uses private method\n";
}

} // namespace encapsulation_solution

// ============================================================================
// SECTION 4: SOLUTION 3 - Use 'final' Keyword
// ============================================================================

namespace final_solution {

// Concrete class marked as final - cannot be inherited
class Counter final {
private:
    int count_ = 0;
    
public:
    void add(int value) {
        count_ += value;
    }
    
    void addMultiple(const std::vector<int>& values) {
        for (int val : values) {
            add(val);
        }
    }
    
    int getCount() const { return count_; }
};

// Base class with final methods
class ExtensibleCounter {
protected:
    int count_ = 0;
    
public:
    virtual ~ExtensibleCounter() = default;
    
    // This method can be overridden
    virtual void add(int value) {
        count_ += value;
    }
    
    // This method CANNOT be overridden - marked final
    virtual void addMultiple(const std::vector<int>& values) final {
        for (int val : values) {
            add(val);
        }
    }
    
    int getCount() const { return count_; }
};

class LoggingCounter : public ExtensibleCounter {
private:
    std::vector<std::string> log_;
    
public:
    void add(int value) override {
        log_.push_back("Adding " + std::to_string(value));
        ExtensibleCounter::add(value);
    }
    
    // Cannot override addMultiple() - it's final
    // void addMultiple(...) { } // ERROR!
    
    void printLog() const {
        std::cout << "    Log entries: " << log_.size() << "\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 4: Solution 3 - Use 'final' Keyword ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✓ Using 'final' to prevent inheritance or method override\n\n";
    
    std::cout << "1. Final class (Counter) - cannot be inherited\n";
    Counter counter1;
    counter1.add(5);
    std::cout << "  Count: " << counter1.getCount() << "\n";
    // class Derived : public Counter { }; // ERROR: cannot inherit from final class
    
    std::cout << "\n2. Final method (addMultiple) - cannot be overridden\n";
    LoggingCounter counter2;
    counter2.add(5);
    counter2.addMultiple({1, 2, 3});
    std::cout << "  Count: " << counter2.getCount() << "\n";
    counter2.printLog();
    std::cout << "  Benefit: Base class controls critical behavior\n";
}

} // namespace final_solution

// ============================================================================
// SECTION 5: SOLUTION 4 - Template Method Pattern
// ============================================================================

namespace template_method_solution {

// Base class designed for inheritance with clear extension points
class Counter {
protected:
    int count_ = 0;
    
    // Hook for derived classes - well-documented extension point
    virtual void onBeforeAdd([[maybe_unused]] int value) {
        // Default: do nothing
    }
    
    virtual void onAfterAdd([[maybe_unused]] int value) {
        // Default: do nothing
    }
    
private:
    // Template method - defines the algorithm structure
    void addImpl(int value) {
        onBeforeAdd(value);
        count_ += value;
        onAfterAdd(value);
    }
    
public:
    virtual ~Counter() = default;
    
    // Public interface - calls template method
    void add(int value) {
        addImpl(value);
    }
    
    // Non-virtual - uses template method internally
    virtual void addMultiple(const std::vector<int>& values) final {
        for (int val : values) {
            addImpl(val);  // Each add goes through hooks
        }
    }
    
    int getCount() const { return count_; }
};

class LoggingCounter : public Counter {
private:
    std::vector<std::string> log_;
    
protected:
    void onBeforeAdd(int value) override {
        log_.push_back("Adding " + std::to_string(value));
    }
    
public:
    void printLog() const {
        std::cout << "    Log entries: " << log_.size() << "\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 5: Solution 4 - Template Method Pattern ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✓ Clear extension points with template method pattern\n";
    LoggingCounter counter;
    counter.add(5);
    counter.addMultiple({1, 2, 3});
    std::cout << "  Count: " << counter.getCount() << "\n";
    counter.printLog();
    std::cout << "  Benefit: Hooks are called for both add() and addMultiple()\n";
    std::cout << "  Base class controls algorithm, derived class controls hooks\n";
}

} // namespace template_method_solution

// ============================================================================
// SECTION 6: SOLUTION 5 - Prefer Interfaces (Pure Abstract Classes)
// ============================================================================

namespace interface_solution {

// Pure interface - no implementation details to break
class ICounter {
public:
    virtual ~ICounter() = default;
    virtual void add(int value) = 0;
    virtual void addMultiple(const std::vector<int>& values) = 0;
    virtual int getCount() const = 0;
};

// Concrete implementation
class BasicCounter : public ICounter {
private:
    int count_ = 0;
    
public:
    void add(int value) override {
        count_ += value;
    }
    
    void addMultiple(const std::vector<int>& values) override {
        for (int val : values) {
            count_ += val;  // Direct implementation
        }
    }
    
    int getCount() const override { return count_; }
};

// Logging decorator using composition
class LoggingCounter : public ICounter {
private:
    std::unique_ptr<ICounter> wrapped_;
    std::vector<std::string> log_;
    
public:
    explicit LoggingCounter(std::unique_ptr<ICounter> counter)
        : wrapped_(std::move(counter)) {}
    
    void add(int value) override {
        log_.push_back("Adding " + std::to_string(value));
        wrapped_->add(value);
    }
    
    void addMultiple(const std::vector<int>& values) override {
        // No logging for batch operations
        wrapped_->addMultiple(values);
    }
    
    int getCount() const override { return wrapped_->getCount(); }
    
    void printLog() const {
        std::cout << "    Log entries: " << log_.size() << "\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 6: Solution 5 - Prefer Interfaces ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✓ Pure interface + composition (Decorator pattern)\n";
    auto counter = std::make_unique<LoggingCounter>(
        std::make_unique<BasicCounter>()
    );
    counter->add(5);
    counter->addMultiple({1, 2, 3});
    std::cout << "  Count: " << counter->getCount() << "\n";
    counter->printLog();
    std::cout << "  Benefit: No implementation inheritance - no fragility\n";
}

} // namespace interface_solution

// ============================================================================
// SECTION 7: ABI Stability Example
// ============================================================================

namespace abi_example {

// Version 1: Library shipped to customers
struct Device_V1 {
    int id;
    std::string name;
    
    virtual ~Device_V1() = default;
    virtual void process() { std::cout << "    Processing " << name << "\n"; }
};

struct Sensor_V1 : public Device_V1 {
    double reading;
    
    void process() override {
        std::cout << "    Sensor " << name << " reading: " << reading << "\n";
    }
};

// Version 2: Developer adds a field (BREAKS ABI)
struct Device_V2 {
    int id;
    std::string name;
    bool enabled;  // NEW FIELD - changes memory layout!
    
    virtual ~Device_V2() = default;
    virtual void process() { 
        if (enabled) std::cout << "    Processing " << name << "\n"; 
    }
};

// Customer code (compiled against V1) now has wrong offsets!

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 7: ABI Stability and Memory Layout ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Problem: Adding fields to base class changes memory layout\n\n";
    
    std::cout << "Device_V1 layout:\n";
    Device_V1 d1;
    d1.id = 1;
    d1.name = "Device1";
    std::cout << "  sizeof(Device_V1) = " << sizeof(Device_V1) << " bytes\n";
    std::cout << "  &id offset   = " << (void*)&d1.id << "\n";
    std::cout << "  &name offset = " << (void*)&d1.name << "\n";
    
    std::cout << "\nSensor_V1 (derived) layout:\n";
    Sensor_V1 s1;
    s1.id = 2;
    s1.name = "Sensor1";
    s1.reading = 42.5;
    std::cout << "  sizeof(Sensor_V1) = " << sizeof(Sensor_V1) << " bytes\n";
    std::cout << "  &reading offset = " << (void*)&s1.reading << "\n";
    
    std::cout << "\nDevice_V2 layout (after adding 'enabled' field):\n";
    Device_V2 d2;
    std::cout << "  sizeof(Device_V2) = " << sizeof(Device_V2) << " bytes\n";
    std::cout << "  Problem: All derived class member offsets have shifted!\n";
    std::cout << "  Solution: Never add non-static data members to base classes\n";
    std::cout << "           Use Pimpl idiom or version interfaces\n";
}

} // namespace abi_example

// ============================================================================
// SECTION 8: Best Practices Summary
// ============================================================================

void show_best_practices() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Best Practices to Avoid Fragile Base Class ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. PREFER COMPOSITION OVER INHERITANCE\n";
    std::cout << "   ✓ Use 'has-a' instead of 'is-a' when possible\n";
    std::cout << "   ✓ Decouple implementation details\n";
    std::cout << "   ✓ More flexible and testable\n\n";
    
    std::cout << "2. DESIGN FOR INHERITANCE OR PROHIBIT IT\n";
    std::cout << "   ✓ Make classes 'final' by default\n";
    std::cout << "   ✓ Only allow inheritance for classes explicitly designed for it\n";
    std::cout << "   ✓ Document extension points clearly\n\n";
    
    std::cout << "3. USE THE 'final' KEYWORD\n";
    std::cout << "   ✓ Mark concrete classes as 'final'\n";
    std::cout << "   ✓ Mark critical methods as 'final' to prevent override\n";
    std::cout << "   ✓ Prevents accidental misuse\n\n";
    
    std::cout << "4. HIDE IMPLEMENTATION DETAILS\n";
    std::cout << "   ✓ Make data members private\n";
    std::cout << "   ✓ Use private helper methods\n";
    std::cout << "   ✓ Expose minimal protected interface\n\n";
    
    std::cout << "5. PREFER INTERFACES (PURE ABSTRACT CLASSES)\n";
    std::cout << "   ✓ Define contracts, not implementations\n";
    std::cout << "   ✓ Use composition + decorator pattern\n";
    std::cout << "   ✓ Eliminates implementation inheritance fragility\n\n";
    
    std::cout << "6. USE TEMPLATE METHOD PATTERN\n";
    std::cout << "   ✓ Base class controls algorithm structure\n";
    std::cout << "   ✓ Derived classes override hooks only\n";
    std::cout << "   ✓ Clear extension points\n\n";
    
    std::cout << "7. FOR LIBRARY DEVELOPERS - MAINTAIN ABI STABILITY\n";
    std::cout << "   ✓ Never add non-static data members to base classes\n";
    std::cout << "   ✓ Use Pimpl idiom for implementation hiding\n";
    std::cout << "   ✓ Version your interfaces\n\n";
    
    std::cout << "REMEMBER: \"Prefer composition over inheritance\" - Gang of Four\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     Fragile Base Class Problem in C++ and Solutions           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    // Demonstrate the problem
    fragile_example::demonstrate();
    
    // Show solutions
    composition_solution::demonstrate();
    encapsulation_solution::demonstrate();
    final_solution::demonstrate();
    template_method_solution::demonstrate();
    interface_solution::demonstrate();
    
    // ABI considerations
    abi_example::demonstrate();
    
    // Best practices
    show_best_practices();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All demonstrations completed!\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    return 0;
}
