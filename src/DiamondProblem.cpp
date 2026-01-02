// DiamondProblem.cpp
// Demonstrates the Diamond Problem (Diamond of Death) in C++ and its solution
//
// The Diamond Problem occurs when a class inherits from two classes that both
// inherit from the same base class, creating a diamond-shaped inheritance hierarchy.
//
// PROBLEM:
// Without virtual inheritance, the derived class contains TWO copies of the
// base class, leading to ambiguity and wasted memory.
//
// SOLUTION:
// Use 'virtual' keyword in inheritance to ensure only ONE copy of the base
// class exists in the final derived class.
//
//     Base
//    /    \
//   A      B
//    \    /
//   Derived
//
// KEY CONCEPTS:
// 1. Multiple Inheritance creates duplicate base class instances
// 2. Virtual Inheritance solves the duplication
// 3. Constructor initialization order with virtual bases
// 4. When to use and when to avoid multiple inheritance

#include <iostream>
#include <string>
#include <memory>

// ============================================================================
// SECTION 1: THE PROBLEM - Diamond Without Virtual Inheritance
// ============================================================================

namespace diamond_problem {

class Device {
protected:
    std::string name_;
    int id_;
    
public:
    Device(const std::string& name, int id) 
        : name_(name), id_(id) {
        std::cout << "    [Device] Constructed: " << name_ << " (ID: " << id_ << ")\n";
    }
    
    void displayInfo() const {
        std::cout << "    Device: " << name_ << ", ID: " << id_ << "\n";
    }
    
    int getId() const { return id_; }
};

class InputDevice : public Device {
protected:
    std::string inputType_;
    
public:
    InputDevice(const std::string& name, int id, const std::string& inputType)
        : Device(name, id), inputType_(inputType) {
        std::cout << "    [InputDevice] Constructed: " << inputType_ << "\n";
    }
    
    void showInput() const {
        std::cout << "    Input Type: " << inputType_ << "\n";
    }
};

class OutputDevice : public Device {
protected:
    std::string outputType_;
    
public:
    OutputDevice(const std::string& name, int id, const std::string& outputType)
        : Device(name, id), outputType_(outputType) {
        std::cout << "    [OutputDevice] Constructed: " << outputType_ << "\n";
    }
    
    void showOutput() const {
        std::cout << "    Output Type: " << outputType_ << "\n";
    }
};

// PROBLEM: IODevice inherits Device TWICE (through InputDevice and OutputDevice)
class IODevice : public InputDevice, public OutputDevice {
public:
    IODevice(const std::string& name, int id)
        : InputDevice(name, id, "Keyboard"),
          OutputDevice(name, id, "Display") {
        std::cout << "    [IODevice] Constructed\n";
    }
    
    void showInfo() const {
        // AMBIGUITY: Which Device::displayInfo() to call?
        // AMBIGUITY: Which Device::getId() to call?
        // Compiler error without explicit qualification
        
        // Must specify which base class path to use:
        std::cout << "  \n  Input Device Info:\n";
        InputDevice::displayInfo();   // Calls Device from InputDevice path
        showInput();
        
        std::cout << "\n  Output Device Info:\n";
        OutputDevice::displayInfo();  // Calls Device from OutputDevice path
        showOutput();
        
        std::cout << "\n  ⚠️  Problem: Two copies of Device exist!\n";
        std::cout << "      InputDevice::getId() = " << InputDevice::getId() << "\n";
        std::cout << "      OutputDevice::getId() = " << OutputDevice::getId() << "\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 1: Diamond Problem (Without Virtual Inheritance) ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Creating IODevice...\n";
    std::cout << "Notice: Device constructor called TWICE!\n\n";
    
    IODevice device("MyIODevice", 42);
    
    std::cout << "\nShowing device information:\n";
    device.showInfo();
    
    std::cout << "\nMemory Analysis:\n";
    std::cout << "  sizeof(Device) = " << sizeof(Device) << " bytes\n";
    std::cout << "  sizeof(InputDevice) = " << sizeof(InputDevice) << " bytes\n";
    std::cout << "  sizeof(OutputDevice) = " << sizeof(OutputDevice) << " bytes\n";
    std::cout << "  sizeof(IODevice) = " << sizeof(IODevice) << " bytes\n";
    std::cout << "  ⚠️  IODevice contains TWO Device instances (memory waste)\n";
}

} // namespace diamond_problem

// ============================================================================
// SECTION 2: THE SOLUTION - Virtual Inheritance
// ============================================================================

namespace virtual_inheritance_solution {

class Device {
protected:
    std::string name_;
    int id_;
    
public:
    Device(const std::string& name, int id) 
        : name_(name), id_(id) {
        std::cout << "    [Device] Constructed: " << name_ << " (ID: " << id_ << ")\n";
    }
    
    virtual ~Device() = default;
    
    void displayInfo() const {
        std::cout << "    Device: " << name_ << ", ID: " << id_ << "\n";
    }
    
    int getId() const { return id_; }
    std::string getName() const { return name_; }
};

// SOLUTION: Use 'virtual' keyword in inheritance
class InputDevice : virtual public Device {
protected:
    std::string inputType_;
    
public:
    InputDevice(const std::string& name, int id, const std::string& inputType)
        : Device(name, id), inputType_(inputType) {
        std::cout << "    [InputDevice] Constructed: " << inputType_ << "\n";
    }
    
    void showInput() const {
        std::cout << "    Input Type: " << inputType_ << "\n";
    }
};

// SOLUTION: Use 'virtual' keyword in inheritance
class OutputDevice : virtual public Device {
protected:
    std::string outputType_;
    
public:
    OutputDevice(const std::string& name, int id, const std::string& outputType)
        : Device(name, id), outputType_(outputType) {
        std::cout << "    [OutputDevice] Constructed: " << outputType_ << "\n";
    }
    
    void showOutput() const {
        std::cout << "    Output Type: " << outputType_ << "\n";
    }
};

// Now IODevice has only ONE copy of Device
class IODevice : public InputDevice, public OutputDevice {
public:
    // IMPORTANT: With virtual inheritance, the most derived class
    // is responsible for initializing the virtual base class
    IODevice(const std::string& name, int id)
        : Device(name, id),  // Must initialize Device directly!
          InputDevice(name, id, "Keyboard"),
          OutputDevice(name, id, "Display") {
        std::cout << "    [IODevice] Constructed\n";
    }
    
    void showInfo() const {
        // No ambiguity now - only one Device exists
        std::cout << "\n  Device Info:\n";
        displayInfo();  // Unambiguous!
        
        std::cout << "\n  Input/Output Details:\n";
        showInput();
        showOutput();
        
        std::cout << "\n  ✓ Solution: Only ONE copy of Device exists!\n";
        std::cout << "    getId() = " << getId() << " (no ambiguity)\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 2: Solution - Virtual Inheritance ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Creating IODevice with virtual inheritance...\n";
    std::cout << "Notice: Device constructor called ONCE!\n\n";
    
    IODevice device("MyIODevice", 42);
    
    std::cout << "\nShowing device information:\n";
    device.showInfo();
    
    std::cout << "\nMemory Analysis:\n";
    std::cout << "  sizeof(Device) = " << sizeof(Device) << " bytes\n";
    std::cout << "  sizeof(InputDevice) = " << sizeof(InputDevice) << " bytes\n";
    std::cout << "  sizeof(OutputDevice) = " << sizeof(OutputDevice) << " bytes\n";
    std::cout << "  sizeof(IODevice) = " << sizeof(IODevice) << " bytes\n";
    std::cout << "  ✓ IODevice contains only ONE Device instance (no duplication)\n";
}

} // namespace virtual_inheritance_solution

// ============================================================================
// SECTION 3: Real-World Example - File System
// ============================================================================

namespace filesystem_example {

class File {
protected:
    std::string path_;
    size_t size_;
    
public:
    File(const std::string& path, size_t size)
        : path_(path), size_(size) {
        std::cout << "    [File] Created: " << path_ << " (" << size_ << " bytes)\n";
    }
    
    virtual ~File() = default;
    
    std::string getPath() const { return path_; }
    size_t getSize() const { return size_; }
    
    void displayInfo() const {
        std::cout << "    File: " << path_ << " (" << size_ << " bytes)\n";
    }
};

// Readable file
class ReadableFile : virtual public File {
public:
    ReadableFile(const std::string& path, size_t size)
        : File(path, size) {
        std::cout << "    [ReadableFile] Readable permissions added\n";
    }
    
    void read() const {
        std::cout << "    Reading from: " << path_ << "\n";
    }
};

// Writable file
class WritableFile : virtual public File {
public:
    WritableFile(const std::string& path, size_t size)
        : File(path, size) {
        std::cout << "    [WritableFile] Writable permissions added\n";
    }
    
    void write(const std::string& data) {
        std::cout << "    Writing to: " << path_ << " - Data: \"" << data << "\"\n";
        size_ += data.length();
    }
};

// Read-Write file - inherits from both
class ReadWriteFile : public ReadableFile, public WritableFile {
public:
    ReadWriteFile(const std::string& path, size_t size)
        : File(path, size),  // Initialize virtual base
          ReadableFile(path, size),
          WritableFile(path, size) {
        std::cout << "    [ReadWriteFile] Read-Write file created\n";
    }
    
    void showInfo() const {
        displayInfo();  // No ambiguity
        std::cout << "    Permissions: Read + Write\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 3: Real-World Example - File System ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Creating ReadWriteFile with virtual inheritance:\n\n";
    ReadWriteFile file("document.txt", 1024);
    
    std::cout << "\nUsing the file:\n";
    file.showInfo();
    file.read();
    file.write("Hello, World!");
    
    std::cout << "\nFinal state:\n";
    file.displayInfo();
}

} // namespace filesystem_example

// ============================================================================
// SECTION 4: Constructor Initialization Order
// ============================================================================

namespace initialization_order {

class Base {
public:
    Base(int value) {
        std::cout << "    [Base] Constructed with value: " << value << "\n";
    }
};

class A : virtual public Base {
public:
    A(int value) : Base(value) {
        std::cout << "    [A] Constructed\n";
    }
};

class B : virtual public Base {
public:
    B(int value) : Base(value) {
        std::cout << "    [B] Constructed\n";
    }
};

class Derived : public A, public B {
public:
    // CRITICAL: Derived class must initialize virtual base class directly!
    // The Base(value) calls in A and B are IGNORED
    Derived(int value) 
        : Base(value),  // This is the one that matters!
          A(value),
          B(value) {
        std::cout << "    [Derived] Constructed\n";
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 4: Constructor Initialization Order ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Important: Virtual base classes are initialized first,\n";
    std::cout << "           by the MOST DERIVED class.\n\n";
    
    std::cout << "Creating Derived object:\n";
    Derived d(42);
    
    std::cout << "\nInitialization order:\n";
    std::cout << "  1. Base (initialized by Derived, not A or B)\n";
    std::cout << "  2. A\n";
    std::cout << "  3. B\n";
    std::cout << "  4. Derived\n";
}

} // namespace initialization_order

// ============================================================================
// SECTION 5: When to Avoid Multiple Inheritance
// ============================================================================

namespace alternative_designs {

// Instead of multiple inheritance, use composition
class Device {
protected:
    std::string name_;
    int id_;
    
public:
    Device(const std::string& name, int id) 
        : name_(name), id_(id) {}
    
    std::string getName() const { return name_; }
    int getId() const { return id_; }
};

class InputCapability {
    std::string inputType_;
    
public:
    explicit InputCapability(const std::string& type) : inputType_(type) {}
    
    void handleInput() const {
        std::cout << "    Handling input: " << inputType_ << "\n";
    }
};

class OutputCapability {
    std::string outputType_;
    
public:
    explicit OutputCapability(const std::string& type) : outputType_(type) {}
    
    void handleOutput() const {
        std::cout << "    Handling output: " << outputType_ << "\n";
    }
};

// Composition-based design - cleaner and more flexible
class IODevice {
    Device device_;
    InputCapability input_;
    OutputCapability output_;
    
public:
    IODevice(const std::string& name, int id)
        : device_(name, id),
          input_("Keyboard"),
          output_("Display") {
        std::cout << "    [IODevice] Constructed using composition\n";
    }
    
    void showInfo() const {
        std::cout << "    Device: " << device_.getName() 
                  << " (ID: " << device_.getId() << ")\n";
        input_.handleInput();
        output_.handleOutput();
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 5: Alternative - Composition Over Inheritance ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Creating IODevice with composition:\n";
    IODevice device("MyIODevice", 42);
    
    std::cout << "\nUsing the device:\n";
    device.showInfo();
    
    std::cout << "\nBenefits:\n";
    std::cout << "  ✓ No diamond problem\n";
    std::cout << "  ✓ More flexible\n";
    std::cout << "  ✓ Easier to understand\n";
    std::cout << "  ✓ Simpler construction\n";
}

} // namespace alternative_designs

// ============================================================================
// SECTION 6: Best Practices
// ============================================================================

void show_best_practices() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== Best Practices for Diamond Problem ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. UNDERSTAND THE PROBLEM\n";
    std::cout << "   • Multiple inheritance creates duplicate base class instances\n";
    std::cout << "   • Causes ambiguity and memory waste\n";
    std::cout << "   • Creates the \"diamond\" inheritance shape\n\n";
    
    std::cout << "2. USE VIRTUAL INHERITANCE WHEN NEEDED\n";
    std::cout << "   • Add 'virtual' keyword: class Derived : virtual public Base\n";
    std::cout << "   • Ensures only ONE base class instance exists\n";
    std::cout << "   • Most derived class initializes the virtual base\n\n";
    
    std::cout << "3. INITIALIZATION ORDER RULES\n";
    std::cout << "   • Virtual base classes initialized FIRST\n";
    std::cout << "   • Most derived class is responsible for initialization\n";
    std::cout << "   • Base class constructors in intermediate classes are ignored\n\n";
    
    std::cout << "4. PERFORMANCE CONSIDERATIONS\n";
    std::cout << "   • Virtual inheritance has small runtime overhead (vtable pointer)\n";
    std::cout << "   • Slightly larger object size\n";
    std::cout << "   • Usually worth it to avoid duplication\n\n";
    
    std::cout << "5. PREFER COMPOSITION OVER MULTIPLE INHERITANCE\n";
    std::cout << "   • Composition is often clearer and more flexible\n";
    std::cout << "   • No diamond problem at all\n";
    std::cout << "   • Easier to test and maintain\n\n";
    
    std::cout << "6. WHEN TO USE MULTIPLE INHERITANCE\n";
    std::cout << "   ✓ Inheriting multiple pure interfaces (abstract classes)\n";
    std::cout << "   ✓ Mixins (adding orthogonal functionality)\n";
    std::cout << "   ✗ Inheriting implementation from multiple classes\n\n";
    
    std::cout << "7. INTERFACE SEGREGATION\n";
    std::cout << "   • Use multiple pure abstract classes (interfaces)\n";
    std::cout << "   • No diamond problem with pure interfaces\n";
    std::cout << "   • Clear contracts without implementation conflicts\n\n";
    
    std::cout << "REMEMBER:\n";
    std::cout << "  \"Prefer composition over inheritance\" - Gang of Four\n";
    std::cout << "  \"Virtual inheritance solves the diamond problem\" - Bjarne Stroustrup\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║         Diamond Problem (Diamond of Death) in C++              ║\n";
    std::cout << "║              Solution: Virtual Inheritance                     ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    // Demonstrate the problem
    diamond_problem::demonstrate();
    
    // Show the solution
    virtual_inheritance_solution::demonstrate();
    
    // Real-world example
    filesystem_example::demonstrate();
    
    // Initialization order
    initialization_order::demonstrate();
    
    // Alternative designs
    alternative_designs::demonstrate();
    
    // Best practices
    show_best_practices();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All demonstrations completed!\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    return 0;
}
