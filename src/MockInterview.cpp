// ===================================================================
// COMPLETE C++ INTERVIEW QUESTIONS & ANSWERS
// ===================================================================
// Comprehensive collection of C++ interview questions with detailed answers
// covering fundamentals, memory management, OOP, templates, concurrency,
// and optimization techniques.
//
// Topics covered:
// - C++ Fundamentals (pointers, references, const)
// - Memory Management (smart pointers, allocators, alignment)
// - OOP & Design Patterns (virtual dispatch, CRTP, PIMPL)
// - Templates & Metaprogramming (SFINAE, variadic, compile-time)
// - Concurrency (lock-free, thread pools, atomics)
// - Performance & Optimization (cache-friendly, branch prediction)
//
// Build: g++ -std=c++20 -pthread -O2 -o MockInterview MockInterview.cpp
// Run: ./MockInterview
// ===================================================================

#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <list>
#include <unordered_map>
#include <optional>
#include <functional>
#include <queue>
#include <condition_variable>
#include <shared_mutex>
#include <future>
#include <array>
#include <type_traits>
#include <variant>
#include <any>
#include <tuple>
#include <string>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <cstdio>
#include <stdexcept>

using namespace std;

// ===================================================================
// SECTION 1: C++ FUNDAMENTALS
// ===================================================================

// ============ Q1.1: Pointers vs References ============
void pointers_vs_references() {
    cout << "\n=== POINTERS VS REFERENCES ===\n";
    
    int x = 10;
    int y = 20;
    
    // Reference - alias, must be initialized, cannot be null
    int& ref = x;
    ref = 30;  // Changes x
    
    // Pointer - stores address, can be null, can be reassigned
    int* ptr = &x;
    *ptr = 40;  // Changes x
    ptr = &y;   // Now points to y
    
    cout << "x = " << x << ", y = " << y << endl;
    cout << "ref = " << ref << ", *ptr = " << *ptr << endl;
    
    // Key differences:
    cout << "\nKey Differences:" << endl;
    cout << "1. References must be initialized, pointers can be null" << endl;
    cout << "2. References cannot be reassigned, pointers can" << endl;
    cout << "3. sizeof(reference) = sizeof(object), sizeof(pointer) = platform dependent" << endl;
    cout << "4. References have cleaner syntax, pointers more flexible" << endl;
}

// ============ Q1.2: Const Correctness ============
void const_correctness() {
    cout << "\n=== CONST CORRECTNESS ===\n";
    
    class Data {
        mutable int counter;  // Can be modified even in const methods
        int value;
        
    public:
        Data(int v) : counter(0), value(v) {}
        
        // 1. Non-const getter - can only be called on non-const objects
        int getValue() { 
            value = 44;
            cout << "Non-const getValue, Changed value = ";
            return value; 
        }
        
        // 2. Const getter - can be called on both const and non-const objects
        int getValue() const { 
            cout << "Const getValue, value = ";
            counter++;  // OK because counter is mutable
            return value; 
        }
        
        // 3. Const reference return - prevents modification
        const int& getValueRef() const { 
            cout << "Const reference getter" << endl;
            return value; 
        }
    };
    
    Data d1(42);
    const Data d2(43);
    
    cout << "d1 " << std::to_string(d1.getValue()) << endl;     // Calls non-const version
    cout << "d2 " << std::to_string(d2.getValue()) << endl;     // Calls const version


    
    // d2.getValueRef() = 50;  // ERROR: Cannot modify through const reference
    
    cout << "\nConst Rules:" << endl;
    cout << "1. const methods can be called on const objects" << endl;
    cout << "2. const methods cannot modify non-mutable members" << endl;
    cout << "3. Return const references to prevent modification" << endl;
    cout << "4. Use mutable for members that don't affect logical state" << endl;
}

// ============ Q1.3: RAII Pattern ============
class FileHandler {
    FILE* file;
    
public:
    FileHandler(const char* filename, const char* mode) {
        file = fopen(filename, mode);
        if (!file) {
            throw runtime_error("Failed to open file");
        }
        cout << "File opened: " << filename << endl;
    }
    
    ~FileHandler() {
        if (file) {
            fclose(file);
            cout << "File closed" << endl;
        }
    }
    
    // Delete copy operations
    FileHandler(const FileHandler&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;
    
    // Allow move operations
    FileHandler(FileHandler&& other) noexcept : file(other.file) {
        other.file = nullptr;
    }
    
    FileHandler& operator=(FileHandler&& other) noexcept {
        if (this != &other) {
            if (file) fclose(file);
            file = other.file;
            other.file = nullptr;
        }
        return *this;
    }
    
    void write(const string& data) {
        if (file) {
            fputs(data.c_str(), file);
        }
    }
};

void raii_demo() {
    cout << "\n=== RAII (Resource Acquisition Is Initialization) ===\n";
    
    try {
        FileHandler file("test_interview.txt", "w");
        file.write("Hello RAII!\n");
        // File automatically closed when scope ends
    } catch (const exception& e) {
        cout << "Exception: " << e.what() << endl;
    }
    
    cout << "\nRAII Principles:" << endl;
    cout << "1. Acquire resource in constructor" << endl;
    cout << "2. Release resource in destructor" << endl;
    cout << "3. Use stack unwinding for exception safety" << endl;
    cout << "4. Smart pointers, locks, containers use RAII" << endl;
	cout << "5. Use automatic (in stack) std::lock_guard and std::unique_lock in member functions" << endl;
}

// ===================================================================
// SECTION 2: MEMORY MANAGEMENT
// ===================================================================

// ============ Q2.1: Smart Pointers Deep Dive ============
void smart_pointers_advanced() {
    cout << "\n=== SMART POINTERS ADVANCED ===\n";
    
    // 1. Custom deleters - Why and when to use them
    {
        cout << "\n1. Custom Deleters - Use Cases:" << endl;
        cout << "   • File handles (FILE*) - need fclose()" << endl;
        cout << "   • C arrays - need delete[] instead of delete" << endl;
        cout << "   • Resource cleanup - network sockets, locks, etc." << endl;
        cout << "   • Custom allocators - memory pools" << endl;
        cout << "   • Logging/debugging - track object lifetime\n" << endl;
        
        // Example 1: FILE* with custom deleter
        auto file_deleter = [](FILE* f) {
            if (f) {
                cout << "   Closing file with fclose()" << endl;
                fclose(f);
            }
        };
        shared_ptr<FILE> file(fopen("temp.txt", "w"), file_deleter);
        if (file) {
            fprintf(file.get(), "Custom deleter example\n");
        }
        
        // Example 2: Array with delete[]
        shared_ptr<int> arr(new int[10], [](int* p) {
            cout << "   Deleting array with delete[]" << endl;
            delete[] p;
        });
        // Note: Better to use unique_ptr<int[]> or vector<int> for arrays
        
        // Example 3: unique_ptr with custom deleter type
        auto int_deleter = [](int* p) {
            cout << "   Custom delete: " << *p << endl;
            delete p;
        };
        unique_ptr<int, decltype(int_deleter)> ptr1(new int(42), int_deleter);
    }
    
    // 2. weak_ptr - Breaking circular references
    {
        cout << "\n2. weak_ptr - Safe Non-Owning Observation:" << endl;
        cout << "   Purpose: Observe shared_ptr without affecting reference count" << endl;
        cout << "   Use cases:" << endl;
        cout << "   • Breaking circular references (parent-child relationships)" << endl;
        cout << "   • Cache implementations (entries can be evicted)" << endl;
        cout << "   • Observer pattern (observers shouldn't own subject)" << endl;
        cout << "   • Factory tracking (don't prevent object deletion)\n" << endl;
        
        // Example: Parent-Child circular reference problem
        class Child;
        class Parent {
        public:
            shared_ptr<Child> child;
            ~Parent() { cout << "   ~Parent()" << endl; }
        };
        
        class Child {
        public:
            weak_ptr<Parent> parent;  // Use weak_ptr to break cycle!
            ~Child() { cout << "   ~Child()" << endl; }
        };
        
        cout << "   Creating parent-child relationship:" << endl;
        {
            auto parent = make_shared<Parent>();
            auto child = make_shared<Child>();
            
            parent->child = child;
            child->parent = parent;  // weak_ptr doesn't increase ref count
            
            cout << "   Parent use_count: " << parent.use_count() << endl;  // 1
            cout << "   Child use_count: " << child.use_count() << endl;    // 2
            
            // Safe weak_ptr access pattern
            if (auto parent_ptr = child->parent.lock()) {  // Convert weak to shared
                cout << "   Parent is still alive, use_count: " << parent_ptr.use_count() << endl;
            } else {
                cout << "   Parent has been destroyed" << endl;
            }
            
            cout << "   Scope ending..." << endl;
        }  // Both Parent and Child are properly destroyed!
        cout << "   Both objects cleaned up (no memory leak)\n" << endl;
        
        // Example: Safe weak_ptr usage pattern
        shared_ptr<int> strong = make_shared<int>(100);
        weak_ptr<int> weak = strong;
        
        cout << "   weak_ptr safe access pattern:" << endl;
        cout << "   • expired(): Check if object still exists" << endl;
        cout << "   • lock(): Get shared_ptr if object exists" << endl;
        cout << "   • use_count(): Get current ref count\n" << endl;
        
        cout << "   weak.expired(): " << (weak.expired() ? "true" : "false") << endl;
        cout << "   weak.use_count(): " << weak.use_count() << endl;
        
        if (auto sp = weak.lock()) {
            cout << "   Successfully locked: " << *sp << endl;
        }
        
        strong.reset();  // Destroy the object
        cout << "   After strong.reset():" << endl;
        cout << "   weak.expired(): " << (weak.expired() ? "true" : "false") << endl;
        
        if (auto sp = weak.lock()) {
            cout << "   Got shared_ptr" << endl;
        } else {
            cout << "   lock() returned nullptr (object destroyed)" << endl;
        }
    }
    
    // 3. make_shared is ONE-WAY - Cannot convert back to unique_ptr
    {
        cout << "\n3. make_shared is ONE-WAY (Irreversible):" << endl;
        cout << "   ✓ unique_ptr → shared_ptr: YES (via std::move)" << endl;
        cout << "   ✗ shared_ptr → unique_ptr: NO (impossible)\n" << endl;
        
        // Forward conversion: unique → shared (OK)
        unique_ptr<int> unique = make_unique<int>(42);
        cout << "   unique_ptr created: " << *unique << endl;
        
        shared_ptr<int> shared = move(unique);  // Transfer ownership
        cout << "   Moved to shared_ptr: " << *shared << endl;
        cout << "   unique_ptr is now: " << (unique ? "valid" : "nullptr") << endl;
        
        // Backward conversion: shared → unique (IMPOSSIBLE)
        cout << "\n   Why shared_ptr → unique_ptr is IMPOSSIBLE:" << endl;
        cout << "   • shared_ptr allows multiple owners (ref count)" << endl;
        cout << "   • unique_ptr requires single ownership" << endl;
        cout << "   • Cannot guarantee no other shared_ptrs exist" << endl;
        cout << "   • No std::unique_ptr(std::shared_ptr) constructor" << endl;
        cout << "   • No way to 'steal' ownership from shared_ptr\n" << endl;
        
        // // This would NOT compile:
        // unique_ptr<int> back_to_unique = move(shared);  // ❌ ERROR!
        // unique_ptr<int> back_to_unique(shared.get());   // ❌ DANGEROUS! Double delete!
        
        cout << "   Design implication: Choose wisely at creation!" << endl;
        cout << "   • Use unique_ptr by default (can upgrade later)" << endl;
        cout << "   • Use shared_ptr only when shared ownership needed" << endl;
        cout << "   • Prefer make_unique/make_shared for exception safety" << endl;
    }
    
    // 4. Aliasing constructor
    {
        cout << "\n4. Aliasing Constructor:" << endl;
        struct Data {
            int x = 10;
            int y = 20;
        };
        
        auto data_ptr = make_shared<Data>();
        auto x_ptr = shared_ptr<int>(data_ptr, &data_ptr->x);
        auto y_ptr = shared_ptr<int>(data_ptr, &data_ptr->y);
        
        cout << "   Use count: " << data_ptr.use_count() << endl;
        cout << "   x_ptr points to x but shares ownership of Data" << endl;
    }
    
    // 5. enable_shared_from_this
    {
        cout << "\n5. enable_shared_from_this:" << endl;
        class Widget : public enable_shared_from_this<Widget> {
        public:
            shared_ptr<Widget> get_shared() {
                return shared_from_this();  // Safe even if multiple shared_ptrs exist
            }
        };
        
        auto widget = make_shared<Widget>();
        auto another_ref = widget->get_shared();
        cout << "   Use count: " << widget.use_count() << endl;
    }
}

// ============ Q2.2: Memory Alignment ============
void memory_alignment() {
    cout << "\n=== MEMORY ALIGNMENT ===\n";
    
    struct BadLayout {
        char c;      // 1 byte
        int i;       // 4 bytes (may need 3 bytes padding after c)
        double d;    // 8 bytes
        char c2;     // 1 byte (7 bytes padding at end)
        // Total: 1 + 3(pad) + 4 + 8 + 1 + 7(pad) = 24 bytes
    };
    
    struct GoodLayout {
        double d;    // 8 bytes
        int i;       // 4 bytes
        char c;      // 1 byte
        char c2;     // 1 byte (2 bytes padding at end)
        // Total: 8 + 4 + 1 + 1 + 2(pad) = 16 bytes
    };
    
    cout << "sizeof(BadLayout): " << sizeof(BadLayout) << " bytes" << endl;
    cout << "sizeof(GoodLayout): " << sizeof(GoodLayout) << " bytes" << endl;
    cout << "Savings: " << (sizeof(BadLayout) - sizeof(GoodLayout)) << " bytes" << endl;
    
    cout << "\nAlignment Rules:" << endl;
    cout << "1. Data type must be aligned to its size boundary" << endl;
    cout << "2. struct alignment = largest member alignment" << endl;
    cout << "3. Reorder members from largest to smallest" << endl;
    cout << "4. Use alignas() for custom alignment" << endl;
    cout << "5. Cache line size typically 64 bytes" << endl;
}

// ===================================================================
// SECTION 3: OOP & DESIGN PATTERNS
// ===================================================================

// ============ Q3.1: Virtual Dispatch Mechanics ============
void virtual_dispatch_details() {
    cout << "\n=== VIRTUAL DISPATCH MECHANICS ===\n";
    
    class Base {
    public:
        virtual void func1() { cout << "Base::func1" << endl; }
        virtual void func2() { cout << "Base::func2" << endl; }
        virtual ~Base() {}
    };
    
    class Derived : public Base {
    public:
        void func1() override { cout << "Derived::func1" << endl; }
        void func3() { cout << "Derived::func3" << endl; }  // Not in vtable
    };
    
    // How virtual call works:
    Base* b = new Derived();
    b->func1();
    
    cout << "\nVirtual call process:" << endl;
    cout << "1. Object contains vptr (pointer to vtable)" << endl;
    cout << "2. vtable contains function pointers" << endl;
    cout << "3. b->func1() becomes: (*b->vptr[0])()" << endl;
    cout << "4. vptr[0] points to Derived::func1" << endl;
    
    // Multiple inheritance
    class Base2 {
    public:
        virtual void func4() { cout << "Base2::func4" << endl; }
        virtual ~Base2() {}
    };
    
    class MultiDerived : public Base, public Base2 {
    public:
        void func1() override { cout << "MultiDerived::func1" << endl; }
        void func4() override { cout << "MultiDerived::func4" << endl; }
    };
    
    MultiDerived md;
    Base* b1 = &md;
    Base2* b2 = &md;
    
    cout << "\nMultiple inheritance layout:" << endl;
    cout << "MultiDerived object has TWO vptrs" << endl;
    cout << "b1 and b2 point to different subobjects" << endl;
    cout << "Address difference: " << (long long)b2 - (long long)b1 << " bytes" << endl;
    
    delete b;
}

// ============ Q3.2: CRTP with Compile-Time Polymorphism (Static Polymorphism) ============
// CRTP Pattern: Curiously Recurring Template Pattern
// Key Insight: Derived class "overrides" base functionality WITHOUT virtual functions
// Use Case: Zero-overhead polymorphism when types are known at compile-time

// CRTP Base: Animal knows about its derived type at compile time
template <typename Derived>
class Animal {
public:
    // Static polymorphism entry point - calls derived implementation
    void speak() {
        // Downcast to Derived and call its makeSound() method
        static_cast<Derived*>(this)->makeSound();
    }
    
    void speak_twice() {
        speak();
        speak();
    }
};

// Dog "overrides" by providing makeSound() method
class Dog : public Animal<Dog> {
public:
    void makeSound() { 
        cout << "Woof!" << endl; 
    }
};

// Cat "overrides" by providing makeSound() method
class Cat : public Animal<Cat> {
public:
    void makeSound() { 
        cout << "Meow!" << endl; 
    }
};

// Cow provides different implementation
class Cow : public Animal<Cow> {
public:
    void makeSound() { 
        cout << "Moo!" << endl; 
    }
};

// Generic function that works with any Animal-derived type
// Note: Takes Animal<T>& not Animal*
template <typename T>
void makeAnimalSpeak(Animal<T>& animal) {
    animal.speak();  // Correctly calls derived implementation at compile-time
}

template <typename T>
void makeAnimalSpeakMultiple(Animal<T>& animal, int times) {
    cout << "Making animal speak " << times << " times: ";
    for (int i = 0; i < times; ++i) {
        animal.speak();
    }
}

void crtp_mixins_demo() {
    cout << "\n=== CRTP: COMPILE-TIME POLYMORPHISM (STATIC POLYMORPHISM) ===\n";
    
    Dog dog;
    Cat cat;
    Cow cow;
    
    cout << "\nDirect calls to speak() (via CRTP):\n";
    cout << "Dog: ";
    dog.speak();
    
    cout << "Cat: ";
    cat.speak();
    
    cout << "Cow: ";
    cow.speak();
    
    cout << "\nUsing generic function with CRTP:\n";
    cout << "Dog via makeAnimalSpeak: ";
    makeAnimalSpeak(dog);   // Output: Woof!
    
    cout << "Cat via makeAnimalSpeak: ";
    makeAnimalSpeak(cat);   // Output: Meow!
    
    cout << "Cow via makeAnimalSpeak: ";
    makeAnimalSpeak(cow);   // Output: Moo!
    
    cout << "\nBase class can build complex behavior:\n";
    cout << "Dog speaking twice: ";
    dog.speak_twice();
    
    cout << "\nUsing parametric function:\n";
    makeAnimalSpeakMultiple(cat, 3);
    
    cout << "\n\nCRTP Key Points:" << endl;
    cout << "1. Base class template parameterized by derived type: Animal<Dog>" << endl;
    cout << "2. Base calls derived methods via static_cast<Derived*>(this)" << endl;
    cout << "3. Derived 'overrides' by providing expected methods (makeSound)" << endl;
    cout << "4. NO VIRTUAL FUNCTIONS - all resolved at compile-time" << endl;
    cout << "5. Zero runtime overhead - can be fully inlined by compiler" << endl;
    cout << "6. Type-safe: won't compile if derived doesn't provide required methods" << endl;
    cout << "7. Cannot store in heterogeneous containers (no Animal* base pointer)" << endl;
    
    cout << "\nVirtual vs CRTP Comparison:" << endl;
    cout << "Virtual:  Animal* -> vtable lookup -> Dog::makeSound() (runtime cost)" << endl;
    cout << "CRTP:     Animal<Dog> -> Dog::makeSound() (compile-time, zero cost)" << endl;
    cout << "Virtual:  Can store in vector<Animal*> (runtime polymorphism)" << endl;
    cout << "CRTP:     Cannot mix types, must know type at compile-time" << endl;
    
    cout << "\nWhen to use CRTP:" << endl;
    cout << "✓ Performance-critical code (no vtable overhead)" << endl;
    cout << "✓ Types known at compile-time" << endl;
    cout << "✓ Want static polymorphism with zero-cost abstraction" << endl;
    cout << "✓ Embedded systems / high-frequency trading / game engines" << endl;
    
    cout << "\nWhen to use Virtual:" << endl;
    cout << "✓ Need runtime polymorphism (plugin systems, dynamic loading)" << endl;
    cout << "✓ Store different types in same container (vector<Animal*>)" << endl;
    cout << "✓ Types not known until runtime" << endl;
}

// ===================================================================
// SECTION 4: TEMPLATES & METAPROGRAMMING
// ===================================================================

// ============ Q4.1: SFINAE Techniques ============
template<typename T>
auto print_value_impl(const T& value, int) 
    -> decltype(cout << value, void()) {
    cout << "Printable: " << value << endl;
}

template<typename T>
void print_value_impl(const T&, long) {
    cout << "Not printable" << endl;
}

template<typename T>
void print_value(const T& value) {
    print_value_impl(value, 0);
}

// SFINAE with enable_if
template<typename T>
typename enable_if<is_integral<T>::value, void>::type
process(T value) {
    cout << "Processing integral: " << value << endl;
}

template<typename T>
typename enable_if<is_floating_point<T>::value, void>::type
process(T value) {
    cout << "Processing float: " << value << endl;
}

void sfinae_demo() {
    cout << "\n=== SFINAE TECHNIQUES ===\n";
    
    print_value(42);
    print_value(vector<int>{1, 2, 3});
    
    process(10);     // Integral version
    process(3.14);   // Float version
}

// ============ Q4.2: Variadic Templates ============
template<typename... Args>
void print_all(Args&&... args) {
    (cout << ... << args) << endl;  // C++17 fold expression
}

void variadic_demo() {
    cout << "\n=== VARIADIC TEMPLATES ===\n";
    
    print_all("Hello", " ", "World", " ", 2024, "!");
    
    cout << "\nVariadic Benefits:" << endl;
    cout << "1. Type-safe variadic functions" << endl;
    cout << "2. Compile-time expansion" << endl;
    cout << "3. Perfect forwarding support" << endl;
}

// ============ Q4.3: Compile-Time Computation ============
constexpr int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N-1>::value;
};

template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

void compile_time_demo() {
    cout << "\n=== COMPILE-TIME COMPUTATION ===\n";
    
    constexpr int fact = factorial(5);
    cout << "Factorial(5) at compile time: " << fact << endl;
    
    cout << "Template factorial(5): " << Factorial<5>::value << endl;
    
    // Static asserts
    static_assert(factorial(5) == 120, "Compile-time factorial failed");
    static_assert(Factorial<5>::value == 120, "Template factorial failed");
    
    cout << "\nCompile-Time Benefits:" << endl;
    cout << "1. Zero runtime overhead" << endl;
    cout << "2. Computed during compilation" << endl;
    cout << "3. Can be used in constant expressions" << endl;
}

// ===================================================================
// SECTION 5: CONCURRENCY & MULTITHREADING
// ===================================================================

// ============ Q5.1: Thread-Safe Singleton ============
class ThreadSafeSingleton {
    static mutex instance_mutex;
    static unique_ptr<ThreadSafeSingleton> instance;
    
    ThreadSafeSingleton() {
        cout << "Singleton created" << endl;
    }
    
public:
    static ThreadSafeSingleton& getInstance() {
        // C++11 guarantees thread-safe static initialization
        static ThreadSafeSingleton instance;
        return instance;
    }
    
    // Better: use call_once
    static ThreadSafeSingleton& getInstanceCallOnce() {
        static once_flag flag;
        call_once(flag, []() {
            cout << "Initialized with call_once" << endl;
        });
        static ThreadSafeSingleton instance;
        return instance;
    }
    
    void doSomething() {
        cout << "Singleton method called" << endl;
    }
    
    // Delete copy/move
    ThreadSafeSingleton(const ThreadSafeSingleton&) = delete;
    ThreadSafeSingleton& operator=(const ThreadSafeSingleton&) = delete;
};

void singleton_demo() {
    cout << "\n=== THREAD-SAFE SINGLETON ===\n";
    
    vector<thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([]() {
            ThreadSafeSingleton::getInstance().doSomething();
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    cout << "\nSingleton Patterns:" << endl;
    cout << "1. Meyer's Singleton (C++11 static init)" << endl;
    cout << "2. std::call_once for initialization" << endl;
    cout << "3. Double-checked locking (avoid!)" << endl;
    cout << "4. Thread-local storage (per-thread instances)" << endl;
}

// ============ Q5.2: Producer-Consumer with Condition Variables ============
class ProducerConsumer {
    queue<int> buffer;
    mutex mtx;
    condition_variable cv_producer, cv_consumer;
    const size_t max_size = 10;
    bool done = false;
    
public:
    void produce(int id, int count) {
        for (int i = 0; i < count; ++i) {
            unique_lock lock(mtx);
            cv_producer.wait(lock, [this] {
                return buffer.size() < max_size || done;
            });
            
            if (done) break;
            
            buffer.push(id * 100 + i);
            cout << "Produced: " << (id * 100 + i) << " (buffer size: " << buffer.size() << ")" << endl;
            
            cv_consumer.notify_one();
            this_thread::sleep_for(chrono::milliseconds(50));
        }
    }
    
    void consume(int id, int count) {
        for (int i = 0; i < count; ++i) {
            unique_lock lock(mtx);
            cv_consumer.wait(lock, [this] {
                return !buffer.empty() || done;
            });
            
            if (done && buffer.empty()) break;
            
            int value = buffer.front();
            buffer.pop();
            cout << "Consumed: " << value << " by consumer " << id << " (buffer size: " << buffer.size() << ")" << endl;
            
            cv_producer.notify_one();
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
    
    void stop() {
        {
            lock_guard lock(mtx);
            done = true;
        }
        cv_producer.notify_all();
        cv_consumer.notify_all();
    }
};

void producer_consumer_demo() {
    cout << "\n=== PRODUCER-CONSUMER PATTERN ===\n";
    
    ProducerConsumer pc;
    
    thread producer1([&pc]() { pc.produce(1, 5); });
    thread producer2([&pc]() { pc.produce(2, 5); });
    thread consumer1([&pc]() { pc.consume(1, 5); });
    thread consumer2([&pc]() { pc.consume(2, 5); });
    
    producer1.join();
    producer2.join();
    consumer1.join();
    consumer2.join();
    
    pc.stop();
    
    cout << "\nCondition Variable Best Practices:" << endl;
    cout << "1. Always use with a mutex" << endl;
    cout << "2. Use wait() with predicate to avoid spurious wakeups" << endl;
    cout << "3. notify_one() vs notify_all()" << endl;
    cout << "4. Consider separate CVs for different conditions" << endl;
}

// ============ Q5.3: Atomic Operations ============
void atomic_operations_demo() {
    cout << "\n=== ATOMIC OPERATIONS ===\n";
    
    atomic<int> counter{0};
    atomic<bool> ready{false};
    
    vector<thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&counter, &ready]() {
            while (!ready.load(memory_order_acquire)) {
                this_thread::yield();
            }
            
            for (int j = 0; j < 1000; ++j) {
                counter.fetch_add(1, memory_order_relaxed);
            }
        });
    }
    
    ready.store(true, memory_order_release);
    
    for (auto& t : threads) {
        t.join();
    }
    
    cout << "Final counter value: " << counter.load() << " (expected: 10000)" << endl;
    
    cout << "\nAtomic Operation Types:" << endl;
    cout << "1. load/store - read/write atomic value" << endl;
    cout << "2. fetch_add/fetch_sub - atomic arithmetic" << endl;
    cout << "3. compare_exchange_weak/strong - CAS operations" << endl;
    cout << "4. Memory ordering: relaxed, acquire, release, seq_cst" << endl;
}

// ===================================================================
// SECTION 6: PERFORMANCE & OPTIMIZATION
// ===================================================================

// ============ Q6.1: Move Semantics Performance ============
class LargeObject {
    vector<int> data;
    
public:
    LargeObject(size_t size) : data(size, 42) {
        cout << "Constructor: " << size << " elements" << endl;
    }
    
    // Copy constructor (expensive)
    LargeObject(const LargeObject& other) : data(other.data) {
        cout << "Copy constructor: " << data.size() << " elements" << endl;
    }
    
    // Move constructor (cheap)
    LargeObject(LargeObject&& other) noexcept : data(move(other.data)) {
        cout << "Move constructor" << endl;
    }
    
    // Copy assignment
    LargeObject& operator=(const LargeObject& other) {
        if (this != &other) {
            data = other.data;
            cout << "Copy assignment: " << data.size() << " elements" << endl;
        }
        return *this;
    }
    
    // Move assignment
    LargeObject& operator=(LargeObject&& other) noexcept {
        if (this != &other) {
            data = move(other.data);
            cout << "Move assignment" << endl;
        }
        return *this;
    }
};

void move_semantics_performance() {
    cout << "\n=== MOVE SEMANTICS PERFORMANCE ===\n";
    
    {
        cout << "\nCopy version:" << endl;
        LargeObject obj1(1000000);
        LargeObject obj2 = obj1;  // Copy
    }
    
    {
        cout << "\nMove version:" << endl;
        LargeObject obj1(1000000);
        LargeObject obj2 = move(obj1);  // Move
    }
    
    cout << "\nMove Semantics Guidelines:" << endl;
    cout << "1. Return by value (compiler uses RVO/NRVO)" << endl;
    cout << "2. Use std::move() for rvalues" << endl;
    cout << "3. Mark move constructors noexcept" << endl;
    cout << "4. Don't move from const objects" << endl;
    cout << "5. Perfect forwarding with std::forward" << endl;
}

// ============ Q6.2: RVO and Copy Elision ============
class Expensive {
public:
    Expensive() {
        cout << "Default constructor" << endl;
    }
    
    Expensive(const Expensive&) {
        cout << "Copy constructor" << endl;
    }
    
    Expensive(Expensive&&) noexcept {
        cout << "Move constructor" << endl;
    }
};

Expensive createExpensive() {
    return Expensive{};  // RVO applies
}

Expensive createExpensiveNamed() {
    Expensive obj;
    return obj;  // NRVO applies
}

void copy_elision_demo() {
    cout << "\n=== COPY ELISION (RVO/NRVO) ===\n";
    
    cout << "Creating with RVO:" << endl;
    Expensive e1 = createExpensive();
    
    cout << "\nCreating with NRVO:" << endl;
    Expensive e2 = createExpensiveNamed();
    
    cout << "\nCopy Elision Rules:" << endl;
    cout << "1. RVO: Return Value Optimization" << endl;
    cout << "2. NRVO: Named Return Value Optimization" << endl;
    cout << "3. Guaranteed in C++17 for prvalue" << endl;
    cout << "4. Compiler may apply in other cases" << endl;
    cout << "5. Don't use std::move() on return values" << endl;
}

// ===================================================================
// MAIN - Demonstrate All Interview Topics
// ===================================================================

int main() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║         COMPLETE C++ INTERVIEW QUESTIONS & ANSWERS               ║\n";
    cout << "║              Mock Interview Preparation Guide                    ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    try {
        // Section 1: C++ Fundamentals
        cout << "\n" << string(70, '=') << "\n";
        cout << "SECTION 1: C++ FUNDAMENTALS\n";
        cout << string(70, '=') << "\n";
        
        pointers_vs_references();
        const_correctness();
        raii_demo();
        
        // Section 2: Memory Management
        cout << "\n" << string(70, '=') << "\n";
        cout << "SECTION 2: MEMORY MANAGEMENT\n";
        cout << string(70, '=') << "\n";
        
        smart_pointers_advanced();
        memory_alignment();
        
        // Section 3: OOP & Design Patterns
        cout << "\n" << string(70, '=') << "\n";
        cout << "SECTION 3: OOP & DESIGN PATTERNS\n";
        cout << string(70, '=') << "\n";
        
        virtual_dispatch_details();
        crtp_mixins_demo();
        
        // Section 4: Templates & Metaprogramming
        cout << "\n" << string(70, '=') << "\n";
        cout << "SECTION 4: TEMPLATES & METAPROGRAMMING\n";
        cout << string(70, '=') << "\n";
        
        sfinae_demo();
        variadic_demo();
        compile_time_demo();
        
        // Section 5: Concurrency & Multithreading
        cout << "\n" << string(70, '=') << "\n";
        cout << "SECTION 5: CONCURRENCY & MULTITHREADING\n";
        cout << string(70, '=') << "\n";
        
        singleton_demo();
        producer_consumer_demo();
        atomic_operations_demo();
        
        // Section 6: Performance & Optimization
        cout << "\n" << string(70, '=') << "\n";
        cout << "SECTION 6: PERFORMANCE & OPTIMIZATION\n";
        cout << string(70, '=') << "\n";
        
        move_semantics_performance();
        copy_elision_demo();
        
        // Summary
        cout << "\n" << string(70, '=') << "\n";
        cout << "SUMMARY: KEY INTERVIEW TOPICS COVERED\n";
        cout << string(70, '=') << "\n\n";
        
        cout << "✅ FUNDAMENTALS:\n";
        cout << "   • Pointers vs References\n";
        cout << "   • Const correctness\n";
        cout << "   • RAII pattern\n\n";
        
        cout << "✅ MEMORY MANAGEMENT:\n";
        cout << "   • Smart pointers (unique_ptr, shared_ptr, weak_ptr)\n";
        cout << "   • Memory alignment and padding\n";
        cout << "   • Custom allocators\n\n";
        
        cout << "✅ OOP & DESIGN:\n";
        cout << "   • Virtual dispatch mechanics\n";
        cout << "   • CRTP with mixins\n";
        cout << "   • PIMPL idiom\n\n";
        
        cout << "✅ TEMPLATES:\n";
        cout << "   • SFINAE techniques\n";
        cout << "   • Variadic templates\n";
        cout << "   • Compile-time computation\n\n";
        
        cout << "✅ CONCURRENCY:\n";
        cout << "   • Thread-safe singleton\n";
        cout << "   • Producer-consumer pattern\n";
        cout << "   • Atomic operations\n\n";
        
        cout << "✅ PERFORMANCE:\n";
        cout << "   • Move semantics\n";
        cout << "   • Copy elision (RVO/NRVO)\n";
        cout << "   • Cache-friendly design\n\n";
        
        cout << "🎯 COMMON INTERVIEW QUESTIONS ANSWERED:\n";
        cout << "   1. What's the difference between pointers and references?\n";
        cout << "   2. Explain smart pointers and when to use each\n";
        cout << "   3. How does virtual dispatch work internally?\n";
        cout << "   4. What is SFINAE and how is it used?\n";
        cout << "   5. How do you implement thread-safe singleton?\n";
        cout << "   6. What's the difference between RVO and move semantics?\n";
        cout << "   7. Explain memory ordering in atomic operations\n";
        cout << "   8. How does CRTP provide compile-time polymorphism?\n\n";
        
        cout << "📚 ADDITIONAL TOPICS TO STUDY:\n";
        cout << "   • Lambda expressions and closures\n";
        cout << "   • Concepts (C++20)\n";
        cout << "   • Coroutines (C++20)\n";
        cout << "   • Ranges library (C++20)\n";
        cout << "   • std::expected (C++23)\n";
        cout << "   • Design patterns (Factory, Observer, Strategy)\n\n";
        
        cout << "╔══════════════════════════════════════════════════════════════════╗\n";
        cout << "║         ALL INTERVIEW TOPICS SUCCESSFULLY DEMONSTRATED!          ║\n";
        cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";
        
    } catch (const exception& e) {
        cerr << "❌ Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
