// ===================================================================
// RESOURCE MANAGEMENT IN MODERN C++
// ===================================================================
// This example demonstrates how Modern C++ solves resource leak problems
// that plagued older C++ code using manual new/delete.
//
// TOPICS COVERED:
// 1. The Old Problem - Manual memory management with new/delete
// 2. Smart Pointers - Automatic memory management
// 3. RAII Pattern - Resource Acquisition Is Initialization
// 4. Other Resources - Files, mutexes, sockets, database connections
// 5. Custom Deleters - Managing non-memory resources
// 6. Exception Safety - Guarantees and resource cleanup
// 7. Modern Best Practices - When raw pointers are still OK
//
// KEY INSIGHT:
// "Modern C++ doesn't use new/delete directly anymore!"
// - Use smart pointers (unique_ptr, shared_ptr, weak_ptr)
// - Use containers (vector, string, map)
// - Use RAII wrappers for all resources
// - Raw pointers for non-owning references only
//
// EMBEDDED SYSTEMS NOTE:
// Even in resource-constrained embedded systems, smart pointers
// have ZERO runtime overhead compared to manual management!
// ===================================================================

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <exception>
#include <functional>
#include <map>

// ===================================================================
// 1. THE OLD PROBLEM - MANUAL MEMORY MANAGEMENT
// ===================================================================

namespace OldCpp {

// Example 1: Classic memory leak
void memory_leak_example() {
    std::cout << "OLD C++: Memory leak example\n";
    std::cout << "-----------------------------\n";
    
    int* ptr = new int(42);
    std::cout << "Allocated int with value: " << *ptr << "\n";
    
    // Forgot to delete! Memory leak!
    // delete ptr;  // <-- Missing this line
    
    std::cout << "Function returns without delete - MEMORY LEAK!\n\n";
}

// Example 2: Exception causes leak
void exception_causes_leak() {
    std::cout << "OLD C++: Exception causes leak\n";
    std::cout << "-------------------------------\n";
    
    int* data = new int[1000];
    std::cout << "Allocated array of 1000 ints\n";
    
    try {
        // Some code that might throw
        if (data != nullptr) {
            throw std::runtime_error("Simulated error!");
        }
        
        delete[] data;  // <-- Never reached if exception thrown!
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << "\n";
        std::cout << "Array was never deleted - MEMORY LEAK!\n\n";
        // Should have delete[] data here, but easy to forget
    }
}

// Example 3: Multiple exit paths
void multiple_exit_paths(bool condition1, bool condition2) {
    std::cout << "OLD C++: Multiple exit paths\n";
    std::cout << "----------------------------\n";
    
    int* buffer = new int[100];
    std::cout << "Allocated buffer\n";
    
    if (condition1) {
        std::cout << "Early return #1\n";
        // Forgot to delete!
        return;  // LEAK!
    }
    
    // ... do some work ...
    
    if (condition2) {
        std::cout << "Early return #2\n";
        // Forgot to delete!
        return;  // LEAK!
    }
    
    delete[] buffer;  // Only reached if both conditions false
    std::cout << "Normal cleanup\n\n";
}

// Example 4: Wrong delete operator
void wrong_delete_operator() {
    std::cout << "OLD C++: Wrong delete operator\n";
    std::cout << "------------------------------\n";
    
    int* single = new int(42);
    int* array = new int[10];
    
    // WRONG! Undefined behavior!
    delete[] single;  // Should be: delete single;
    delete array;      // Should be: delete[] array;
    
    std::cout << "Used wrong delete operator - UNDEFINED BEHAVIOR!\n\n";
}

// Example 5: Double delete
void double_delete_problem() {
    std::cout << "OLD C++: Double delete\n";
    std::cout << "----------------------\n";
    
    int* ptr = new int(42);
    
    delete ptr;
    std::cout << "Deleted once\n";
    
    // WRONG! Undefined behavior!
    delete ptr;  // Double delete - CRASH or corruption!
    
    std::cout << "Double delete - UNDEFINED BEHAVIOR!\n\n";
}

class ResourceIntensive {
    int* data;
    size_t size;
    
public:
    ResourceIntensive(size_t n) : size(n) {
        data = new int[n];
        std::cout << "  Allocated " << n << " ints\n";
    }
    
    // PROBLEM: No destructor! Memory leak!
    // ~ResourceIntensive() { delete[] data; }  // <-- Forgot this!
};

void demonstrate_old_problems() {
    std::cout << "\n";
    std::cout << "=========================================================\n";
    std::cout << "1. THE OLD PROBLEM - MANUAL MEMORY MANAGEMENT\n";
    std::cout << "=========================================================\n\n";
    
    memory_leak_example();
    exception_causes_leak();
    multiple_exit_paths(true, false);
    
    // Note: Can't safely run wrong_delete_operator() and double_delete_problem()
    // as they cause undefined behavior
    
    std::cout << "Summary of Problems:\n";
    std::cout << "  ❌ Forgot to delete\n";
    std::cout << "  ❌ Exception before delete\n";
    std::cout << "  ❌ Multiple exit paths\n";
    std::cout << "  ❌ Wrong delete operator (delete vs delete[])\n";
    std::cout << "  ❌ Double delete\n";
    std::cout << "  ❌ Forgot destructor in class\n";
    std::cout << "\n";
}

} // namespace OldCpp

// ===================================================================
// 2. MODERN C++ SOLUTION - SMART POINTERS
// ===================================================================

namespace ModernCpp {

// Example 1: unique_ptr - Exclusive ownership
void unique_ptr_example() {
    std::cout << "MODERN C++: unique_ptr (exclusive ownership)\n";
    std::cout << "--------------------------------------------\n";
    
    // No manual new!
    std::unique_ptr<int> ptr = std::make_unique<int>(42);
    std::cout << "Created unique_ptr with value: " << *ptr << "\n";
    
    // Automatically deleted when scope ends!
    std::cout << "Leaving scope - automatic cleanup!\n\n";
    
    // NO delete needed!
}

// Example 2: Exception-safe with unique_ptr
void exception_safe_unique_ptr() {
    std::cout << "MODERN C++: Exception-safe with unique_ptr\n";
    std::cout << "------------------------------------------\n";
    
    auto data = std::make_unique<int[]>(1000);
    std::cout << "Allocated array of 1000 ints\n";
    
    try {
        throw std::runtime_error("Simulated error!");
        
        // Never reached, but doesn't matter!
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << "\n";
        std::cout << "unique_ptr automatically cleaned up - NO LEAK!\n\n";
    }
    
    // Cleanup happens automatically in catch block
}

// Example 3: Multiple exit paths - no problem!
void multiple_exit_paths_safe(bool condition1, bool condition2) {
    std::cout << "MODERN C++: Multiple exit paths (safe)\n";
    std::cout << "--------------------------------------\n";
    
    auto buffer = std::make_unique<int[]>(100);
    std::cout << "Allocated buffer\n";
    
    if (condition1) {
        std::cout << "Early return #1 - buffer auto-deleted!\n\n";
        return;  // NO LEAK! unique_ptr cleans up
    }
    
    if (condition2) {
        std::cout << "Early return #2 - buffer auto-deleted!\n\n";
        return;  // NO LEAK! unique_ptr cleans up
    }
    
    std::cout << "Normal return - buffer auto-deleted!\n\n";
    // Automatic cleanup regardless of path!
}

// Example 4: shared_ptr - Shared ownership
void shared_ptr_example() {
    std::cout << "MODERN C++: shared_ptr (shared ownership)\n";
    std::cout << "-----------------------------------------\n";
    
    std::shared_ptr<int> ptr1 = std::make_shared<int>(42);
    std::cout << "Created shared_ptr, ref count: " << ptr1.use_count() << "\n";
    
    {
        std::shared_ptr<int> ptr2 = ptr1;  // Share ownership
        std::cout << "Shared ownership, ref count: " << ptr1.use_count() << "\n";
        
        std::shared_ptr<int> ptr3 = ptr1;  // Another share
        std::cout << "More sharing, ref count: " << ptr1.use_count() << "\n";
        
    } // ptr2 and ptr3 destroyed
    
    std::cout << "After scope, ref count: " << ptr1.use_count() << "\n";
    std::cout << "Memory deleted when last shared_ptr destroyed!\n\n";
}

// Example 5: weak_ptr - Non-owning observer (doesn't keep object alive)
void weak_ptr_example() {
    std::cout << "MODERN C++: weak_ptr (non-owning observer)\n";
    std::cout << "-------------------------------------------\n";
    
    std::weak_ptr<int> weak;
    
    {
        std::shared_ptr<int> shared = std::make_shared<int>(42);
        weak = shared;  // weak_ptr observes, but doesn't own
        
        std::cout << "shared_ptr alive, ref count: " << shared.use_count() << "\n";
        std::cout << "weak_ptr expired? " << (weak.expired() ? "YES" : "NO") << "\n";
        
        // ALWAYS check if weak_ptr is still valid before using!
        if (auto locked = weak.lock()) {  // lock() returns shared_ptr
            std::cout << "Locked weak_ptr, value: " << *locked << "\n";
            std::cout << "Ref count during lock: " << locked.use_count() << "\n";
        } else {
            std::cout << "Object was deleted!\n";
        }
        
    } // shared destroyed here
    
    std::cout << "\nAfter shared_ptr destroyed:\n";
    std::cout << "weak_ptr expired? " << (weak.expired() ? "YES" : "NO") << "\n";
    
    if (auto locked = weak.lock()) {
        std::cout << "Locked successfully, value: " << *locked << "\n";
    } else {
        std::cout << "Cannot lock - object was deleted!\n";
    }
    std::cout << "\n";
}

// Example 6: weak_ptr - Breaking circular references
class Node {
public:
    std::string name;
    std::shared_ptr<Node> next;    // Strong reference (owns next)
    std::weak_ptr<Node> prev;      // Weak reference (doesn't own prev)
    
    Node(const std::string& n) : name(n) {
        std::cout << "  Node '" << name << "' created\n";
    }
    
    ~Node() {
        std::cout << "  Node '" << name << "' destroyed\n";
    }
};

void circular_reference_example() {
    std::cout << "MODERN C++: weak_ptr breaks circular references\n";
    std::cout << "------------------------------------------------\n";
    
    auto node1 = std::make_shared<Node>("First");
    auto node2 = std::make_shared<Node>("Second");
    auto node3 = std::make_shared<Node>("Third");
    
    // Build doubly-linked list
    node1->next = node2;
    node2->prev = node1;  // weak_ptr - doesn't create cycle!
    
    node2->next = node3;
    node3->prev = node2;  // weak_ptr - doesn't create cycle!
    
    std::cout << "\nNavigate forward (using shared_ptr):\n";
    auto current = node1;
    while (current) {
        std::cout << "  At node: " << current->name << "\n";
        current = current->next;
    }
    
    std::cout << "\nNavigate backward (using weak_ptr):\n";
    current = node3;
    while (current) {
        std::cout << "  At node: " << current->name << "\n";
        
        // Must lock() weak_ptr to use it!
        if (auto prev = current->prev.lock()) {
            current = prev;
        } else {
            break;  // No previous node
        }
    }
    
    std::cout << "\nLeaving scope - all nodes will be destroyed:\n";
}

// Example 7: weak_ptr - Cache implementation
class ExpensiveResource {
    std::string data;
public:
    ExpensiveResource(const std::string& d) : data(d) {
        std::cout << "  Expensive resource created: " << data << "\n";
    }
    ~ExpensiveResource() {
        std::cout << "  Expensive resource destroyed: " << data << "\n";
    }
    const std::string& get_data() const { return data; }
};

class ResourceCache {
    std::map<std::string, std::weak_ptr<ExpensiveResource>> cache;
    
public:
    std::shared_ptr<ExpensiveResource> get_resource(const std::string& key) {
        // Try to get from cache
        auto it = cache.find(key);
        if (it != cache.end()) {
            // Check if cached resource is still alive
            if (auto resource = it->second.lock()) {
                std::cout << "  Cache HIT: " << key << "\n";
                return resource;
            } else {
                std::cout << "  Cache EXPIRED: " << key << "\n";
                cache.erase(it);  // Clean up expired entry
            }
        }
        
        // Create new resource
        std::cout << "  Cache MISS: Creating " << key << "\n";
        auto resource = std::make_shared<ExpensiveResource>(key);
        cache[key] = resource;  // Store weak_ptr in cache
        return resource;
    }
};

void cache_example() {
    std::cout << "MODERN C++: weak_ptr for caching\n";
    std::cout << "---------------------------------\n";
    
    ResourceCache cache;
    
    {
        auto res1 = cache.get_resource("data1");
        auto res2 = cache.get_resource("data1");  // Should be cache hit
        
        std::cout << "Both pointing to same resource: " 
                  << (res1 == res2 ? "YES" : "NO") << "\n";
    }
    
    std::cout << "\nResources destroyed, trying to access again:\n";
    auto res3 = cache.get_resource("data1");  // Should be cache miss
    
    std::cout << "\n";
}

// Example 8: Modern class with RAII
class ResourceIntensiveModern {
    std::unique_ptr<int[]> data;
    size_t size;
    
public:
    ResourceIntensiveModern(size_t n) : data(std::make_unique<int[]>(n)), size(n) {
        std::cout << "  Allocated " << n << " ints (RAII)\n";
    }
    
    // No destructor needed! unique_ptr handles cleanup!
    // Compiler-generated destructor is perfect!
    
    // Safe to copy if we want (Rule of Zero)
    // Or delete copy if we want move-only:
    ResourceIntensiveModern(const ResourceIntensiveModern&) = delete;
    ResourceIntensiveModern& operator=(const ResourceIntensiveModern&) = delete;
    
    // Move operations provided automatically!
    ResourceIntensiveModern(ResourceIntensiveModern&&) = default;
    ResourceIntensiveModern& operator=(ResourceIntensiveModern&&) = default;
    
    int& operator[](size_t i) { return data[i]; }
};

void demonstrate_modern_solutions() {
    std::cout << "\n";
    std::cout << "=========================================================\n";
    std::cout << "2. MODERN C++ SOLUTION - SMART POINTERS\n";
    std::cout << "=========================================================\n\n";
    
    unique_ptr_example();
    exception_safe_unique_ptr();
    multiple_exit_paths_safe(true, false);
    shared_ptr_example();
    weak_ptr_example();
    circular_reference_example();
    cache_example();
    
    std::cout << "Modern class with RAII:\n";
    {
        ResourceIntensiveModern obj(100);
        // Automatic cleanup when obj goes out of scope!
    }
    std::cout << "  Automatically cleaned up!\n\n";
    
    std::cout << "Summary of Solutions:\n";
    std::cout << "  ✅ No manual new/delete\n";
    std::cout << "  ✅ Exception-safe automatically\n";
    std::cout << "  ✅ Multiple exit paths safe\n";
    std::cout << "  ✅ Can't use wrong delete operator\n";
    std::cout << "  ✅ Can't double-delete\n";
    std::cout << "  ✅ Compiler-generated destructor works\n";
    std::cout << "  ✅ weak_ptr breaks circular references\n";
    std::cout << "  ✅ weak_ptr for cache/observer patterns\n";
    std::cout << "\n";
}

} // namespace ModernCpp

// ===================================================================
// 3. RAII PATTERN - BEYOND MEMORY
// ===================================================================

namespace RAII {

// File handle RAII
class FileRAII {
    std::ofstream file;
    std::string filename;
    
public:
    FileRAII(const std::string& name) : filename(name) {
        file.open(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file");
        }
        std::cout << "  File opened: " << filename << "\n";
    }
    
    ~FileRAII() {
        if (file.is_open()) {
            file.close();
            std::cout << "  File closed: " << filename << "\n";
        }
    }
    
    // Delete copy, allow move
    FileRAII(const FileRAII&) = delete;
    FileRAII& operator=(const FileRAII&) = delete;
    FileRAII(FileRAII&&) = default;
    FileRAII& operator=(FileRAII&&) = default;
    
    void write(const std::string& data) {
        file << data;
    }
};

// Mutex lock RAII (std::lock_guard does this!)
class MutexLockRAII {
    std::mutex& mtx;
    
public:
    explicit MutexLockRAII(std::mutex& m) : mtx(m) {
        mtx.lock();
        std::cout << "  Mutex locked\n";
    }
    
    ~MutexLockRAII() {
        mtx.unlock();
        std::cout << "  Mutex unlocked\n";
    }
    
    // Non-copyable, non-movable
    MutexLockRAII(const MutexLockRAII&) = delete;
    MutexLockRAII& operator=(const MutexLockRAII&) = delete;
};

// Timer RAII (measures scope duration)
class ScopeTimer {
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
    
public:
    explicit ScopeTimer(const std::string& n) 
        : name(n), start(std::chrono::high_resolution_clock::now()) {
        std::cout << "  [" << name << "] Started\n";
    }
    
    ~ScopeTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "  [" << name << "] Duration: " << duration.count() << " μs\n";
    }
};

// Generic RAII wrapper
template<typename ResourceType, typename AcquireFunc, typename ReleaseFunc>
class GenericRAII {
    ResourceType resource;
    ReleaseFunc release;
    bool active;
    
public:
    GenericRAII(AcquireFunc acquire, ReleaseFunc rel) 
        : resource(acquire()), release(rel), active(true) {}
    
    ~GenericRAII() {
        if (active) {
            release(resource);
        }
    }
    
    ResourceType& get() { return resource; }
    
    // Delete copy
    GenericRAII(const GenericRAII&) = delete;
    GenericRAII& operator=(const GenericRAII&) = delete;
    
    // Allow move
    GenericRAII(GenericRAII&& other) noexcept 
        : resource(std::move(other.resource)), 
          release(std::move(other.release)),
          active(other.active) {
        other.active = false;
    }
};

void demonstrate_raii() {
    std::cout << "\n";
    std::cout << "=========================================================\n";
    std::cout << "3. RAII PATTERN - BEYOND MEMORY\n";
    std::cout << "=========================================================\n\n";
    
    std::cout << "File RAII example:\n";
    {
        FileRAII file("test.txt");
        file.write("Hello, RAII!\n");
        // File automatically closed when scope ends
    }
    std::cout << "\n";
    
    std::cout << "Mutex RAII example:\n";
    std::mutex mtx;
    {
        MutexLockRAII lock(mtx);
        // Critical section
        std::cout << "  In critical section\n";
        // Mutex automatically unlocked when scope ends
    }
    std::cout << "\n";
    
    std::cout << "Scope timer example:\n";
    {
        ScopeTimer timer("MyOperation");
        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // Timer automatically reports duration
    }
    std::cout << "\n";
    
    std::cout << "RAII Resources:\n";
    std::cout << "  ✅ Memory (unique_ptr, shared_ptr)\n";
    std::cout << "  ✅ Files (fstream, custom wrappers)\n";
    std::cout << "  ✅ Locks (lock_guard, unique_lock)\n";
    std::cout << "  ✅ Timers (scope profiling)\n";
    std::cout << "  ✅ Network sockets\n";
    std::cout << "  ✅ Database connections\n";
    std::cout << "  ✅ OpenGL contexts\n";
    std::cout << "  ✅ Hardware registers (embedded)\n";
    std::cout << "\n";
}

} // namespace RAII

// ===================================================================
// 4. CUSTOM DELETERS
// ===================================================================

namespace CustomDeleters {

// Example: C-style FILE*
void file_deleter(FILE* fp) {
    if (fp) {
        std::cout << "  Custom deleter: closing FILE*\n";
        fclose(fp);
    }
}

void custom_deleter_file_example() {
    std::cout << "Custom deleter with FILE*:\n";
    
    std::unique_ptr<FILE, decltype(&file_deleter)> file(
        fopen("test.txt", "w"),
        file_deleter
    );
    
    if (file) {
        fprintf(file.get(), "Hello from C-style FILE!\n");
    }
    
    // FILE* automatically closed by custom deleter!
}

// Example: Hardware register (embedded systems)
struct HardwareRegister {
    volatile uint32_t* address;
    uint32_t original_value;
    
    HardwareRegister(volatile uint32_t* addr) 
        : address(addr), original_value(*addr) {
        std::cout << "  Saving register value: 0x" << std::hex << original_value << std::dec << "\n";
    }
    
    ~HardwareRegister() {
        *address = original_value;
        std::cout << "  Restored register value: 0x" << std::hex << original_value << std::dec << "\n";
    }
};

void embedded_register_example() {
    std::cout << "\nEmbedded hardware register RAII:\n";
    
    uint32_t simulated_register = 0x12345678;
    
    {
        HardwareRegister reg(&simulated_register);
        
        // Modify register
        simulated_register = 0xABCDEF00;
        std::cout << "  Modified register: 0x" << std::hex << simulated_register << std::dec << "\n";
        
        // Automatically restored on scope exit!
    }
    
    std::cout << "  Register after scope: 0x" << std::hex << simulated_register << std::dec << "\n";
}

void demonstrate_custom_deleters() {
    std::cout << "\n";
    std::cout << "=========================================================\n";
    std::cout << "4. CUSTOM DELETERS\n";
    std::cout << "=========================================================\n\n";
    
    custom_deleter_file_example();
    embedded_register_example();
    
    std::cout << "\nCustom deleters for:\n";
    std::cout << "  ✅ C API resources (FILE*, malloc, etc.)\n";
    std::cout << "  ✅ Operating system handles\n";
    std::cout << "  ✅ Hardware registers (embedded)\n";
    std::cout << "  ✅ Third-party library resources\n";
    std::cout << "\n";
}

} // namespace CustomDeleters

// ===================================================================
// 5. WHEN RAW POINTERS ARE OK IN MODERN C++
// ===================================================================

namespace RawPointersOK {

class Node {
public:
    int value;
    std::unique_ptr<Node> next;  // Owns the next node
    Node* prev;                   // Non-owning pointer to previous
    
    Node(int v) : value(v), prev(nullptr) {}
};

void raw_pointer_examples() {
    std::cout << "\n";
    std::cout << "=========================================================\n";
    std::cout << "5. WHEN RAW POINTERS ARE OK IN MODERN C++\n";
    std::cout << "=========================================================\n\n";
    
    std::cout << "Raw pointers are OK for:\n\n";
    
    std::cout << "1. NON-OWNING REFERENCES:\n";
    std::cout << "   auto owner = std::make_unique<int>(42);\n";
    std::cout << "   int* observer = owner.get();  // OK! Not responsible for deletion\n\n";
    
    std::cout << "2. FUNCTION PARAMETERS (non-owning):\n";
    std::cout << "   void process(const Widget* widget);  // OK! Doesn't own\n\n";
    
    std::cout << "3. OPTIONAL PARAMETERS:\n";
    std::cout << "   void render(Texture* texture = nullptr);  // OK! May be null\n\n";
    
    std::cout << "4. POINTERS TO STACK OBJECTS:\n";
    std::cout << "   int value = 42;\n";
    std::cout << "   int* ptr = &value;  // OK! No ownership\n\n";
    
    std::cout << "5. BACK POINTERS IN DATA STRUCTURES:\n";
    std::cout << "   struct Node {\n";
    std::cout << "       std::unique_ptr<Node> next;  // Owns next\n";
    std::cout << "       Node* prev;                   // Non-owning back pointer\n";
    std::cout << "   };\n\n";
    
    std::cout << "6. POINTERS TO GLOBAL/STATIC OBJECTS:\n";
    std::cout << "   static Logger logger;\n";
    std::cout << "   Logger* getLogger() { return &logger; }  // OK!\n\n";
    
    std::cout << "NEVER use raw pointers for:\n";
    std::cout << "  ❌ Ownership (use unique_ptr or shared_ptr)\n";
    std::cout << "  ❌ Arrays (use std::vector or std::array)\n";
    std::cout << "  ❌ Manual new/delete\n";
    std::cout << "\n";
}

} // namespace RawPointersOK

// ===================================================================
// 6. CONTAINERS INSTEAD OF MANUAL ARRAYS
// ===================================================================

namespace Containers {

void demonstrate_containers() {
    std::cout << "\n";
    std::cout << "=========================================================\n";
    std::cout << "6. CONTAINERS INSTEAD OF MANUAL ARRAYS\n";
    std::cout << "=========================================================\n\n";
    
    std::cout << "OLD C++: Manual array management\n";
    std::cout << "  int* arr = new int[100];  // Manual allocation\n";
    std::cout << "  // ... use array ...\n";
    std::cout << "  delete[] arr;  // Manual cleanup (easy to forget!)\n\n";
    
    std::cout << "MODERN C++: Use std::vector\n";
    std::vector<int> vec(100);
    std::cout << "  std::vector<int> vec(100);  // Automatic!\n";
    std::cout << "  // Automatic cleanup, resizing, bounds checking\n";
    std::cout << "  Size: " << vec.size() << " elements\n\n";
    
    std::cout << "Other containers (all automatic!):\n";
    std::cout << "  ✅ std::vector - Dynamic array\n";
    std::cout << "  ✅ std::array - Fixed-size array\n";
    std::cout << "  ✅ std::string - String (char array)\n";
    std::cout << "  ✅ std::map - Key-value pairs\n";
    std::cout << "  ✅ std::unordered_map - Hash table\n";
    std::cout << "  ✅ std::set - Unique elements\n";
    std::cout << "  ✅ std::list - Doubly-linked list\n";
    std::cout << "  ✅ std::deque - Double-ended queue\n";
    std::cout << "\n";
}

} // namespace Containers

// ===================================================================
// 7. PERFORMANCE - ZERO OVERHEAD
// ===================================================================

namespace Performance {

void demonstrate_zero_overhead() {
    std::cout << "\n";
    std::cout << "=========================================================\n";
    std::cout << "7. PERFORMANCE - ZERO OVERHEAD\n";
    std::cout << "=========================================================\n\n";
    
    std::cout << "Smart pointers have ZERO runtime overhead!\n\n";
    
    std::cout << "unique_ptr:\n";
    std::cout << "  • Same size as raw pointer\n";
    std::cout << "  • No runtime cost\n";
    std::cout << "  • Optimized to same assembly as manual delete\n";
    std::cout << "  • sizeof(unique_ptr<int>) = " << sizeof(std::unique_ptr<int>) << " bytes\n";
    std::cout << "  • sizeof(int*) = " << sizeof(int*) << " bytes\n\n";
    
    std::cout << "shared_ptr:\n";
    std::cout << "  • Small overhead for reference counting\n";
    std::cout << "  • sizeof(shared_ptr<int>) = " << sizeof(std::shared_ptr<int>) << " bytes\n";
    std::cout << "  • Only use when you need shared ownership\n\n";
    
    std::cout << "Embedded systems:\n";
    std::cout << "  ✅ unique_ptr: Perfect! Zero overhead\n";
    std::cout << "  ✅ std::vector: Excellent! Deterministic\n";
    std::cout << "  ⚠️ shared_ptr: Use sparingly (ref counting cost)\n";
    std::cout << "  ✅ RAII wrappers: Zero cost abstraction\n\n";
    
    std::cout << "Compiler optimizations:\n";
    std::cout << "  • unique_ptr: Completely optimized away\n";
    std::cout << "  • Move semantics: No copies, just pointer transfer\n";
    std::cout << "  • RVO/NRVO: Return value optimization\n";
    std::cout << "  • Inline expansion: Zero function call overhead\n";
    std::cout << "\n";
}

} // namespace Performance

// ===================================================================
// MAIN DEMONSTRATION
// ===================================================================

int main() {
    std::cout << "\n";
    std::cout << "===================================================================\n";
    std::cout << "RESOURCE MANAGEMENT IN MODERN C++\n";
    std::cout << "===================================================================\n";
    std::cout << "\"Modern C++ doesn't use new/delete directly anymore!\"\n";
    std::cout << "===================================================================\n";
    
    OldCpp::demonstrate_old_problems();
    ModernCpp::demonstrate_modern_solutions();
    RAII::demonstrate_raii();
    CustomDeleters::demonstrate_custom_deleters();
    RawPointersOK::raw_pointer_examples();
    Containers::demonstrate_containers();
    Performance::demonstrate_zero_overhead();
    
    std::cout << "\n";
    std::cout << "===================================================================\n";
    std::cout << "SUMMARY - MODERN C++ RESOURCE MANAGEMENT\n";
    std::cout << "===================================================================\n";
    std::cout << "\n";
    std::cout << "THE GOLDEN RULES:\n";
    std::cout << "-----------------\n";
    std::cout << "1. NEVER use 'new' or 'delete' directly\n";
    std::cout << "   → Use std::make_unique<T>() instead\n";
    std::cout << "\n";
    std::cout << "2. NEVER use 'new[]' or 'delete[]'\n";
    std::cout << "   → Use std::vector<T> or std::array<T, N> instead\n";
    std::cout << "\n";
    std::cout << "3. Prefer unique_ptr over shared_ptr\n";
    std::cout << "   → Exclusive ownership is clearer and faster\n";
    std::cout << "\n";
    std::cout << "4. Use RAII for ALL resources\n";
    std::cout << "   → Memory, files, locks, sockets, everything!\n";
    std::cout << "\n";
    std::cout << "5. Raw pointers are OK for non-owning references\n";
    std::cout << "   → Never responsible for deletion\n";
    std::cout << "\n";
    std::cout << "6. Follow the Rule of Zero\n";
    std::cout << "   → Let compiler generate special members\n";
    std::cout << "   → Use standard library types as members\n";
    std::cout << "\n";
    std::cout << "BENEFITS:\n";
    std::cout << "---------\n";
    std::cout << "✅ No memory leaks (automatic cleanup)\n";
    std::cout << "✅ Exception-safe (automatic cleanup on unwind)\n";
    std::cout << "✅ No double-delete bugs\n";
    std::cout << "✅ No use-after-free bugs\n";
    std::cout << "✅ Clear ownership semantics\n";
    std::cout << "✅ Zero runtime overhead (unique_ptr)\n";
    std::cout << "✅ Easier to reason about code\n";
    std::cout << "✅ Suitable for embedded systems\n";
    std::cout << "\n";
    std::cout << "WHEN YOU STILL SEE new/delete:\n";
    std::cout << "------------------------------\n";
    std::cout << "• Legacy code (pre-C++11)\n";
    std::cout << "• Educational purposes (showing the old way)\n";
    std::cout << "• Custom memory allocators (advanced)\n";
    std::cout << "• Framework internals (Qt, COM, etc.)\n";
    std::cout << "\n";
    std::cout << "IN MODERN C++ (C++11 and later):\n";
    std::cout << "--------------------------------\n";
    std::cout << "new/delete are considered OBSOLETE for application code!\n";
    std::cout << "\n";
    std::cout << "MIGRATION PATH:\n";
    std::cout << "---------------\n";
    std::cout << "T* ptr = new T;           → auto ptr = std::make_unique<T>();\n";
    std::cout << "T* arr = new T[N];        → std::vector<T> arr(N);\n";
    std::cout << "delete ptr;               → (automatic)\n";
    std::cout << "delete[] arr;             → (automatic)\n";
    std::cout << "T* shared = new T;        → auto shared = std::make_shared<T>();\n";
    std::cout << "\n";
    std::cout << "===================================================================\n";
    std::cout << "ALL EXAMPLES COMPLETED SUCCESSFULLY!\n";
    std::cout << "===================================================================\n\n";
    
    return 0;
}

/*
EXPECTED OUTPUT HIGHLIGHTS:
===========================

=========================================================
1. THE OLD PROBLEM - MANUAL MEMORY MANAGEMENT
=========================================================

OLD C++: Memory leak example
-----------------------------
Allocated int with value: 42
Function returns without delete - MEMORY LEAK!

OLD C++: Exception causes leak
-------------------------------
Allocated array of 1000 ints
Caught exception: Simulated error!
Array was never deleted - MEMORY LEAK!

Summary of Problems:
  ❌ Forgot to delete
  ❌ Exception before delete
  ❌ Multiple exit paths
  ❌ Wrong delete operator (delete vs delete[])
  ❌ Double delete
  ❌ Forgot destructor in class

=========================================================
2. MODERN C++ SOLUTION - SMART POINTERS
=========================================================

MODERN C++: unique_ptr (exclusive ownership)
--------------------------------------------
Created unique_ptr with value: 42
Leaving scope - automatic cleanup!

MODERN C++: Exception-safe with unique_ptr
------------------------------------------
Allocated array of 1000 ints
Caught exception: Simulated error!
unique_ptr automatically cleaned up - NO LEAK!

Summary of Solutions:
  ✅ No manual new/delete
  ✅ Exception-safe automatically
  ✅ Multiple exit paths safe
  ✅ Can't use wrong delete operator
  ✅ Can't double-delete
  ✅ Compiler-generated destructor works

=========================================================
3. RAII PATTERN - BEYOND MEMORY
=========================================================

File RAII example:
  File opened: test.txt
  File closed: test.txt

Mutex RAII example:
  Mutex locked
  In critical section
  Mutex unlocked

RAII Resources:
  ✅ Memory (unique_ptr, shared_ptr)
  ✅ Files (fstream, custom wrappers)
  ✅ Locks (lock_guard, unique_lock)
  ✅ Timers (scope profiling)
  ✅ Network sockets
  ✅ Database connections
  ✅ Hardware registers (embedded)

=========================================================
7. PERFORMANCE - ZERO OVERHEAD
=========================================================

Smart pointers have ZERO runtime overhead!

unique_ptr:
  • Same size as raw pointer
  • No runtime cost
  • Optimized to same assembly as manual delete
  • sizeof(unique_ptr<int>) = 8 bytes
  • sizeof(int*) = 8 bytes

Embedded systems:
  ✅ unique_ptr: Perfect! Zero overhead
  ✅ std::vector: Excellent! Deterministic
  ⚠️ shared_ptr: Use sparingly (ref counting cost)
  ✅ RAII wrappers: Zero cost abstraction

===================================================================
SUMMARY - MODERN C++ RESOURCE MANAGEMENT
===================================================================

THE GOLDEN RULES:
-----------------
1. NEVER use 'new' or 'delete' directly
   → Use std::make_unique<T>() instead

2. NEVER use 'new[]' or 'delete[]'
   → Use std::vector<T> or std::array<T, N> instead

3. Prefer unique_ptr over shared_ptr
   → Exclusive ownership is clearer and faster

4. Use RAII for ALL resources
   → Memory, files, locks, sockets, everything!

5. Raw pointers are OK for non-owning references
   → Never responsible for deletion

BENEFITS:
---------
✅ No memory leaks (automatic cleanup)
✅ Exception-safe (automatic cleanup on unwind)
✅ No double-delete bugs
✅ No use-after-free bugs
✅ Clear ownership semantics
✅ Zero runtime overhead (unique_ptr)
✅ Suitable for embedded systems

IN MODERN C++ (C++11 and later):
--------------------------------
new/delete are considered OBSOLETE for application code!

*/
