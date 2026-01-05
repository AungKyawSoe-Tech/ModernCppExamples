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
#include <cstring>
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
#include <iostream>

template <typename Derived>
class Animal {
public:
    void speak() {
        static_cast<Derived*>(this)->makeSound();
    }
};

class Dog : public Animal<Dog> {
public:  // Changed to public
    void makeSound() { std::cout << "Woof!\n"; }
};

class Cat : public Animal<Cat> {
public:  // Changed to public
    void makeSound() { std::cout << "Meow!\n"; }
};

template <typename T>
void makeAnimalSpeak(Animal<T>& animal) {
    animal.speak();
}

int crtp_mixins_demo() {
    Dog dog;
    Cat cat;
    
    makeAnimalSpeak(dog);  // Output: Woof!
    makeAnimalSpeak(cat);  // Output: Meow!
    
    // Can also call directly
    dog.speak();  // Woof!
    cat.speak();  // Meow!
    
    return 0;
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

/*
SFINAE, or Substitution Failure Is Not An Error, is a core C++ compilation rule 
where a compiler ignores an invalid function template specialization during 
overload resolution instead of throwing an error, allowing it to try other valid 
overloads. This powerful technique enables advanced template metaprogramming, type 
introspection, and conditional compilation, letting developers write generic code 
that adapts based on type properties, like checking if a type has a specific member,
using tools like std::enable_if or if constexpr. 
*/
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

// ============ Q6.1: Move Semantics - Complete Educational Guide ============

// ===== PART 1: Basic String Class with Move Semantics =====
class MyString {
private:
    char* data;
    size_t length;
    
public:
    // Constructor
    MyString(const char* str = "") {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
        cout << "  [Constructor] Created string: \"" << data << "\" at " << (void*)data << endl;
    }
    
    // Destructor
    ~MyString() {
        cout << "  [Destructor] Deleting " << (void*)data;
        if (data && length > 0) {
            cout << " (\"" << data << "\")";
        }
        cout << endl;
        delete[] data;
    }
    
    // Copy constructor (EXPENSIVE - allocates new memory and copies)
    MyString(const MyString& other) {
        length = other.length;
        data = new char[length + 1];
        strcpy(data, other.data);
        cout << "  [Copy Constructor] Copied \"" << data << "\" to new memory at " << (void*)data << endl;
    }
    
    // Copy assignment operator
    MyString& operator=(const MyString& other) {
        cout << "  [Copy Assignment] ";
        if (this != &other) {
            delete[] data;  // Free old memory
            length = other.length;
            data = new char[length + 1];
            strcpy(data, other.data);
            cout << "Copied \"" << data << "\" to " << (void*)data << endl;
        } else {
            cout << "Self-assignment detected, no-op" << endl;
        }
        return *this;
    }
    
    // Move constructor (CHEAP - steals resources, no allocation)
    MyString(MyString&& other) noexcept {
        cout << "  [Move Constructor] Stealing resources from " << (void*)other.data << endl;
        // Steal the data
        data = other.data;
        length = other.length;
        
        // Leave other in valid but empty state
        other.data = nullptr;
        other.length = 0;
        cout << "  [Move Constructor] Moved \"" << data << "\" to " << (void*)data 
             << ", source now empty" << endl;
    }
    
    // Move assignment operator
    MyString& operator=(MyString&& other) noexcept {
        cout << "  [Move Assignment] ";
        if (this != &other) {
            // Free our current resource
            delete[] data;
            
            // Steal other's resources
            data = other.data;
            length = other.length;
            
            // Leave other in valid state
            other.data = nullptr;
            other.length = 0;
            
            cout << "Moved \"" << data << "\" to " << (void*)data << endl;
        } else {
            cout << "Self-assignment detected, no-op" << endl;
        }
        return *this;
    }
    
    // Utility
    const char* c_str() const { return data ? data : ""; }
    size_t size() const { return length; }
};

void demonstrate_basic_move_semantics() {
    cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
    cout << "║         MOVE SEMANTICS - EDUCATIONAL DEMONSTRATION            ║\n";
    cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    
    cout << "\n1️⃣  COPY CONSTRUCTOR vs MOVE CONSTRUCTOR:\n";
    cout << string(60, '-') << "\n";
    
    {
        cout << "\n📋 Copy Constructor (Expensive - allocates new memory):\n";
        MyString str1("Hello");
        MyString str2 = str1;  // Copy constructor - str1 is lvalue
        cout << "  Result: str1=\"" << str1.c_str() << "\", str2=\"" << str2.c_str() << "\"\n";
    }
    
    {
        cout << "\n🚀 Move Constructor (Cheap - transfers ownership):\n";
        MyString str1("World");
        MyString str2 = move(str1);  // Move constructor - explicitly cast to rvalue
        cout << "  Result: str1=\"" << str1.c_str() << "\" (moved-from), str2=\"" << str2.c_str() << "\"\n";
        cout << "  ⚠️  str1 is now in valid but unspecified state (empty)\n";
    }
    
    cout << "\n2️⃣  COPY ASSIGNMENT vs MOVE ASSIGNMENT:\n";
    cout << string(60, '-') << "\n";
    
    {
        cout << "\n📋 Copy Assignment:\n";
        MyString str1("Original");
        MyString str2("Other");
        cout << "Before assignment:\n";
        str2 = str1;  // Copy assignment
        cout << "  Result: str1=\"" << str1.c_str() << "\", str2=\"" << str2.c_str() << "\"\n";
    }
    
    {
        cout << "\n🚀 Move Assignment:\n";
        MyString str1("Moving");
        MyString str2("Target");
        cout << "Before assignment:\n";
        str2 = move(str1);  // Move assignment
        cout << "  Result: str1=\"" << str1.c_str() << "\" (moved-from), str2=\"" << str2.c_str() << "\"\n";
    }
}

// ===== PART 2: Understanding Rvalues and Lvalues =====
void demonstrate_value_categories() {
    cout << "\n3️⃣  VALUE CATEGORIES (Lvalue vs Rvalue):\n";
    cout << string(60, '-') << "\n";
    
    cout << "\n📚 DEFINITIONS:\n";
    cout << "  • Lvalue: Has a name, has an address, persists beyond expression\n";
    cout << "            Examples: variables, dereferenced pointers, array elements\n";
    cout << "  • Rvalue: Temporary, no name (or about to die), can be moved from\n";
    cout << "            Examples: literals, temporary objects, function returns\n\n";
    
    MyString str1("Lvalue");
    cout << "  MyString str1(\"Lvalue\");  // str1 is an LVALUE\n\n";
    
    cout << "  MyString str2 = str1;      // Copy (str1 is lvalue)\n";
    MyString str2 = str1;
    
    cout << "\n  MyString str3 = MyString(\"Rvalue\");  // Move (temporary is rvalue)\n";
    MyString str3 = MyString("Rvalue");
    
    cout << "\n  MyString str4 = move(str1);  // Move (std::move casts to rvalue)\n";
    MyString str4 = move(str1);
    
    cout << "\n💡 KEY INSIGHT: std::move() doesn't move anything!\n";
    cout << "   It just casts an lvalue to an rvalue reference (xvalue)\n";
    cout << "   The actual move happens in the move constructor/assignment\n";
}

// ===== PART 3: Rule of Five =====
class ResourceManager {
private:
    int* resource;
    string name;
    
public:
    // 1. Constructor
    explicit ResourceManager(const string& n) : resource(new int(42)), name(n) {
        cout << "  ✓ Constructor: " << name << " created resource at " << (void*)resource << endl;
    }
    
    // 2. Destructor
    ~ResourceManager() {
        cout << "  ✗ Destructor: " << name << " destroying resource at " << (void*)resource << endl;
        delete resource;
    }
    
    // 3. Copy Constructor
    ResourceManager(const ResourceManager& other) 
        : resource(new int(*other.resource)), name(other.name + "_copy") {
        cout << "  📋 Copy Constructor: Created " << name << " from " << other.name << endl;
    }
    
    // 4. Copy Assignment
    ResourceManager& operator=(const ResourceManager& other) {
        if (this != &other) {
            delete resource;
            resource = new int(*other.resource);
            name = other.name + "_assigned";
            cout << "  📋 Copy Assignment: " << name << " from " << other.name << endl;
        }
        return *this;
    }
    
    // 5. Move Constructor
    ResourceManager(ResourceManager&& other) noexcept
        : resource(other.resource), name(move(other.name) + "_moved") {
        other.resource = nullptr;  // Nullify source
        cout << "  🚀 Move Constructor: " << name << " stole resource" << endl;
    }
    
    // 6. Move Assignment
    ResourceManager& operator=(ResourceManager&& other) noexcept {
        if (this != &other) {
            delete resource;
            resource = other.resource;
            name = move(other.name) + "_move_assigned";
            other.resource = nullptr;
            cout << "  🚀 Move Assignment: " << name << " stole resource" << endl;
        }
        return *this;
    }
    
    const string& getName() const { return name; }
};

void demonstrate_rule_of_five() {
    cout << "\n4️⃣  RULE OF FIVE:\n";
    cout << string(60, '-') << "\n";
    cout << "If you define ANY of: destructor, copy constructor, copy assignment,\n";
    cout << "                      move constructor, or move assignment\n";
    cout << "You should probably define ALL FIVE (or explicitly delete them)\n\n";
    
    {
        cout << "Creating original:\n";
        ResourceManager rm1("Original");
        
        cout << "\nCopy constructor:\n";
        ResourceManager rm2 = rm1;
        
        cout << "\nMove constructor:\n";
        ResourceManager rm3 = move(rm1);
        
        cout << "\nCopy assignment:\n";
        ResourceManager rm4("Target");
        rm4 = rm2;
        
        cout << "\nMove assignment:\n";
        ResourceManager rm5("AnotherTarget");
        rm5 = move(rm3);
        
        cout << "\nAll objects going out of scope...\n";
    }
    cout << "Cleanup complete!\n";
}

// ===== PART 4: Performance Comparison =====
class LargeObject {
    vector<int> data;
    string name;
    
public:
    LargeObject(const string& n, size_t size) : data(size, 42), name(n) {
        cout << "  Constructor: " << name << " with " << size << " elements" << endl;
    }
    
    // Copy constructor (expensive)
    LargeObject(const LargeObject& other) : data(other.data), name(other.name + "_copy") {
        cout << "  📋 Copy Constructor: " << name << " copied " << data.size() << " elements" << endl;
    }
    
    // Move constructor (cheap)
    LargeObject(LargeObject&& other) noexcept 
        : data(move(other.data)), name(move(other.name) + "_moved") {
        cout << "  🚀 Move Constructor: " << name << " (just pointer swap, O(1))" << endl;
    }
    
    // Copy assignment
    LargeObject& operator=(const LargeObject& other) {
        if (this != &other) {
            data = other.data;
            name = other.name + "_assigned";
            cout << "  📋 Copy Assignment: " << name << " copied " << data.size() << " elements" << endl;
        }
        return *this;
    }
    
    // Move assignment
    LargeObject& operator=(LargeObject&& other) noexcept {
        if (this != &other) {
            data = move(other.data);
            name = move(other.name) + "_move_assigned";
            cout << "  🚀 Move Assignment: " << name << " (just pointer swap, O(1))" << endl;
        }
        return *this;
    }
    
    const string& getName() const { return name; }
    size_t size() const { return data.size(); }
};

void demonstrate_performance() {
    cout << "\n5️⃣  PERFORMANCE COMPARISON:\n";
    cout << string(60, '-') << "\n";
    
    cout << "\n⏱️  Copying large object:\n";
    {
        LargeObject obj1("Large1", 10000000);
        auto start = chrono::high_resolution_clock::now();
        LargeObject obj2 = obj1;  // Copy - allocates and copies 10M integers
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "  Time taken: " << duration.count() << " µs\n";
    }
    
    cout << "\n⚡ Moving large object:\n";
    {
        LargeObject obj1("Large2", 10000000);
        auto start = chrono::high_resolution_clock::now();
        LargeObject obj2 = move(obj1);  // Move - just swaps pointers
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "  Time taken: " << duration.count() << " µs\n";
    }
    
    cout << "\n💡 Move is typically 100-1000x faster for large objects!\n";
}

// ===== PART 5: Common Pitfalls =====
void demonstrate_pitfalls() {
    cout << "\n6️⃣  COMMON MOVE SEMANTICS PITFALLS:\n";
    cout << string(60, '-') << "\n";
    
    cout << "\n❌ PITFALL 1: Moving from const objects\n";
    cout << "  const MyString str1(\"Const\");\n";
    cout << "  MyString str2 = move(str1);  // ❌ Calls COPY, not move!\n";
    cout << "  Reason: Can't bind rvalue reference (T&&) to const T\n";
    
    cout << "\n❌ PITFALL 2: Using moved-from object\n";
    cout << R"(
  MyString str1("Data");
  MyString str2 = move(str1);
  cout << str1.c_str();  // ❌ Undefined behavior! str1 was moved from
  
  ✓ CORRECT: After move, only assign to or destroy the moved-from object
)";
    
    cout << "\n❌ PITFALL 3: Returning local by std::move\n";
    cout << R"(
  MyString createString() {
      MyString local("Value");
      return move(local);  // ❌ BAD! Prevents RVO
  }
  
  ✓ CORRECT:
  MyString createString() {
      MyString local("Value");
      return local;  // Compiler uses RVO or implicit move
  }
)";
    
    cout << "\n❌ PITFALL 4: Forgetting noexcept on move operations\n";
    cout << R"(
  MyString(MyString&& other) noexcept { ... }  // ✓ GOOD
  MyString(MyString&& other) { ... }           // ❌ BAD - may throw
  
  Reason: STL containers won't use move if it can throw
          (they fall back to copy for strong exception guarantee)
)";
}

// ===== PART 6: Perfect Forwarding =====
template<typename T>
void processValue(T&& value) {
    cout << "  Processing: " << value.c_str() << " (type: ";
    if (is_lvalue_reference_v<T>) {
        cout << "lvalue reference)\n";
    } else {
        cout << "rvalue reference)\n";
    }
}

template<typename T>
void forwardingWrapper(T&& value) {
    cout << "  Wrapper called, forwarding to processValue...\n";
    processValue(forward<T>(value));  // Perfect forwarding
}

void demonstrate_perfect_forwarding() {
    cout << "\n7️⃣  PERFECT FORWARDING:\n";
    cout << string(60, '-') << "\n";
    
    cout << "\nUniversal references (T&&) in templates:\n";
    cout << "  • T&& + lvalue → collapses to T&\n";
    cout << "  • T&& + rvalue → stays T&&\n\n";
    
    MyString str("Value");
    
    cout << "Forwarding lvalue:\n";
    forwardingWrapper(str);
    
    cout << "\nForwarding rvalue:\n";
    forwardingWrapper(MyString("Temporary"));
    
    cout << "\n💡 std::forward preserves value category through templates\n";
}

// ===== MAIN MOVE SEMANTICS DEMO =====
void move_semantics_performance() {
    demonstrate_basic_move_semantics();
    demonstrate_value_categories();
    demonstrate_rule_of_five();
    demonstrate_performance();
    demonstrate_pitfalls();
    demonstrate_perfect_forwarding();
    
    cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
    cout << "║                    MOVE SEMANTICS SUMMARY                     ║\n";
    cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    
    cout << "\n📝 KEY TAKEAWAYS:\n";
    cout << "  1. Move semantics transfers ownership instead of copying\n";
    cout << "  2. Use std::move() to cast lvalues to rvalues\n";
    cout << "  3. Implement Rule of Five for resource-owning classes\n";
    cout << "  4. Always mark move operations noexcept\n";
    cout << "  5. Don't use std::move on return statements (prevents RVO)\n";
    cout << "  6. Moved-from objects must be in valid but unspecified state\n";
    cout << "  7. Use std::forward for perfect forwarding in templates\n";
    cout << "  8. Performance gain: O(n) copy → O(1) move for large objects\n\n";
    
    cout << "🎯 WHEN TO USE MOVE SEMANTICS:\n";
    cout << "  ✓ Returning expensive objects from functions\n";
    cout << "  ✓ Inserting into STL containers\n";
    cout << "  ✓ Implementing resource-managing classes\n";
    cout << "  ✓ Transferring unique ownership (e.g., unique_ptr)\n\n";
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
