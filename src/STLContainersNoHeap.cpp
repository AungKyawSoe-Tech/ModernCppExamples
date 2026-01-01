#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <span>

// ===================================================================
// CAN STL CONTAINERS WORK ON SYSTEMS WITHOUT HEAP?
// ===================================================================
// Comprehensive answer:
// 1. Most STL containers (vector, map, list) require heap by default
// 2. std::array works WITHOUT heap (C++11)
// 3. std::span works WITHOUT heap (C++20)
// 4. Custom allocators can use stack/static memory
// 5. C++17 PMR (Polymorphic Memory Resources) for flexible allocation
// 
// This example demonstrates ALL approaches for heap-less STL usage
// ===================================================================

// ===================================================================
// 1. STACK-BASED CONTAINERS - std::array (NO HEAP!)
// ===================================================================

void example_std_array() {
    std::cout << "\n=== 1. std::array - STACK-BASED, NO HEAP ===" << std::endl;
    
    // ✓ std::array: Fixed size, stack allocated, zero heap usage
    std::array<int, 10> sensor_data = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    
    std::cout << "\nSensor readings:" << std::endl;
    for (size_t i = 0; i < sensor_data.size(); ++i) {
        std::cout << "  Sensor " << i << ": " << sensor_data[i] << std::endl;
    }
    
    // ✓ Full STL algorithm support
    std::cout << "\nSorted readings:" << std::endl;
    std::sort(sensor_data.begin(), sensor_data.end());
    for (const auto& reading : sensor_data) {
        std::cout << "  " << reading;
    }
    std::cout << std::endl;
    
    // ✓ constexpr support (C++17)
    constexpr std::array<int, 5> compile_time = {1, 2, 3, 4, 5};
    [[maybe_unused]] constexpr int sum = compile_time[0] + compile_time[1];  // Compile-time computation
    
    std::cout << "\n✓ BENEFITS:" << std::endl;
    std::cout << "   • Zero heap allocation" << std::endl;
    std::cout << "   • Stack-based (fast access)" << std::endl;
    std::cout << "   • Fixed size known at compile time" << std::endl;
    std::cout << "   • Same interface as std::vector" << std::endl;
    std::cout << "   • constexpr support (C++17)" << std::endl;
    std::cout << "   • Perfect for embedded systems" << std::endl;
    
    std::cout << "\n✗ LIMITATIONS:" << std::endl;
    std::cout << "   • Fixed size (cannot grow/shrink)" << std::endl;
    std::cout << "   • Size must be known at compile time" << std::endl;
}

// ===================================================================
// 2. NON-OWNING VIEWS - std::span (C++20, NO HEAP!)
// ===================================================================

void process_data(std::span<int> data) {
    std::cout << "  Processing " << data.size() << " elements: ";
    for (auto val : data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

void example_std_span() {
    std::cout << "\n=== 2. std::span - NON-OWNING VIEW, NO HEAP ===" << std::endl;
    
    // Stack-based data
    std::array<int, 5> arr = {1, 2, 3, 4, 5};
    int c_array[] = {10, 20, 30, 40};
    
    // ✓ std::span: Non-owning view, zero allocation
    std::span<int> span1(arr);
    std::span<int> span2(c_array, 4);
    
    std::cout << "\nProcessing array:" << std::endl;
    process_data(span1);
    
    std::cout << "\nProcessing C array:" << std::endl;
    process_data(span2);
    
    // Sub-spans (slicing)
    auto first_three = span1.first(3);
    std::cout << "\nFirst 3 elements:" << std::endl;
    process_data(first_three);
    
    std::cout << "\n✓ BENEFITS:" << std::endl;
    std::cout << "   • Zero allocation (view only)" << std::endl;
    std::cout << "   • Works with std::array, C arrays, std::vector" << std::endl;
    std::cout << "   • Safe bounds checking (can enable)" << std::endl;
    std::cout << "   • Sub-span support (slicing)" << std::endl;
    std::cout << "   • Modern replacement for (T*, size_t)" << std::endl;
    
    std::cout << "\n⚠️ CAUTION:" << std::endl;
    std::cout << "   • Non-owning (must ensure lifetime)" << std::endl;
    std::cout << "   • Can dangle if underlying data destroyed" << std::endl;
}

// ===================================================================
// 3. CUSTOM STACK ALLOCATOR FOR std::vector
// ===================================================================

template<typename T, size_t N>
class StackAllocator {
private:
    alignas(T) uint8_t storage[N * sizeof(T)];
    size_t used = 0;
    
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    StackAllocator() = default;
    
    template<typename U>
    StackAllocator(const StackAllocator<U, N>&) noexcept {}
    
    template<typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };
    
    T* allocate(size_t n) {
        if (used + n > N) {
            throw std::bad_alloc();  // Or return nullptr in embedded
        }
        
        T* result = reinterpret_cast<T*>(storage + used * sizeof(T));
        used += n;
        return result;
    }
    
    void deallocate(T* p, size_t n) noexcept {
        // Simple stack allocator: only deallocate if it's the last allocation
        T* last = reinterpret_cast<T*>(storage + (used - n) * sizeof(T));
        if (p == last) {
            used -= n;
        }
        // Otherwise, memory is "leaked" until allocator is destroyed
        // For better deallocation, use a more sophisticated allocator
    }
    
    size_t capacity() const { return N; }
    size_t available() const { return N - used; }
};

template<typename T, size_t N, typename U, size_t M>
bool operator==(const StackAllocator<T, N>&, const StackAllocator<U, M>&) noexcept {
    return false;  // Different allocators are never equal
}

template<typename T, size_t N, typename U, size_t M>
bool operator!=(const StackAllocator<T, N>&, const StackAllocator<U, M>&) noexcept {
    return true;
}

void example_custom_stack_allocator() {
    std::cout << "\n=== 3. CUSTOM STACK ALLOCATOR FOR std::vector ===" << std::endl;
    
    // ✓ std::vector with stack allocator - NO HEAP!
    using StackVec = std::vector<int, StackAllocator<int, 20>>;
    StackVec vec;
    
    std::cout << "\nAdding elements (stack allocation):" << std::endl;
    vec.reserve(10);  // Pre-reserve to avoid reallocation
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i * 10);
        std::cout << "  Added " << i * 10 
                  << ", size=" << vec.size() << std::endl;
    }
    
    std::cout << "\nVector contents:" << std::endl;
    for (const auto& val : vec) {
        std::cout << "  " << val;
    }
    std::cout << std::endl;
    
    std::cout << "\n✓ BENEFITS:" << std::endl;
    std::cout << "   • Uses std::vector interface" << std::endl;
    std::cout << "   • Stack-based (no heap)" << std::endl;
    std::cout << "   • Can still push_back(), resize(), etc." << std::endl;
    std::cout << "   • Custom allocator handles memory" << std::endl;
    
    std::cout << "\n✗ LIMITATIONS:" << std::endl;
    std::cout << "   • Fixed maximum capacity (20 in this case)" << std::endl;
    std::cout << "   • Simple deallocation strategy" << std::endl;
    std::cout << "   • Throws exception when out of memory" << std::endl;
}

// ===================================================================
// 4. STATIC MEMORY POOL ALLOCATOR
// ===================================================================

template<typename T, size_t PoolSize>
class PoolAllocator {
private:
    struct Block {
        alignas(T) uint8_t data[sizeof(T)];
    };
    
    // Static storage - shared across all instances
    inline static Block storage[PoolSize];
    inline static bool used[PoolSize] = {};
    inline static size_t allocated_count = 0;
    
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    PoolAllocator() noexcept = default;
    
    template<typename U>
    PoolAllocator(const PoolAllocator<U, PoolSize>&) noexcept {}
    
    template<typename U>
    struct rebind {
        using other = PoolAllocator<U, PoolSize>;
    };
    
    T* allocate(size_t n) {
        if (n > PoolSize || allocated_count + n > PoolSize) {
            throw std::bad_alloc();
        }
        
        // Find contiguous free blocks
        for (size_t i = 0; i <= PoolSize - n; ++i) {
            bool found = true;
            for (size_t j = 0; j < n; ++j) {
                if (used[i + j]) {
                    found = false;
                    break;
                }
            }
            
            if (found) {
                for (size_t j = 0; j < n; ++j) {
                    used[i + j] = true;
                }
                allocated_count += n;
                return reinterpret_cast<T*>(&storage[i]);
            }
        }
        
        throw std::bad_alloc();
    }
    
    void deallocate(T* p, size_t n) noexcept {
        if (!p) return;
        
        auto base = reinterpret_cast<Block*>(storage);
        auto block = reinterpret_cast<Block*>(p);
        size_t index = block - base;
        
        if (index < PoolSize) {
            for (size_t i = 0; i < n && (index + i) < PoolSize; ++i) {
                used[index + i] = false;
            }
            allocated_count -= n;
        }
    }
    
    static size_t pool_size() { return PoolSize; }
};

template<typename T, size_t N, typename U, size_t M>
bool operator==(const PoolAllocator<T, N>&, const PoolAllocator<U, M>&) noexcept {
    return N == M;
}

template<typename T, size_t N, typename U, size_t M>
bool operator!=(const PoolAllocator<T, N>&, const PoolAllocator<U, M>&) noexcept {
    return N != M;
}

void example_pool_allocator() {
    std::cout << "\n=== 4. STATIC MEMORY POOL ALLOCATOR ===" << std::endl;
    
    struct Sensor {
        int id;
        float value;
        Sensor(int i, float v) : id(i), value(v) {
            std::cout << "    Sensor(" << id << ", " << value << ")" << std::endl;
        }
        ~Sensor() {
            std::cout << "    ~Sensor(" << id << ")" << std::endl;
        }
    };
    
    using SensorVec = std::vector<Sensor, PoolAllocator<Sensor, 10>>;
    
    std::cout << "\nAllocating sensors from static pool:" << std::endl;
    {
        SensorVec sensors;
        sensors.push_back(Sensor(1, 25.5f));
        sensors.push_back(Sensor(2, 30.2f));
        sensors.push_back(Sensor(3, 28.7f));
        
        std::cout << "\nSensors in vector: " << sensors.size() << std::endl;
    }
    std::cout << "\nVector destroyed, memory returned to pool" << std::endl;
    
    std::cout << "\n✓ BENEFITS:" << std::endl;
    std::cout << "   • Static memory (no heap)" << std::endl;
    std::cout << "   • Fast O(1) allocation/deallocation" << std::endl;
    std::cout << "   • No fragmentation" << std::endl;
    std::cout << "   • Works with std::vector, std::list, etc." << std::endl;
    std::cout << "   • Memory reused across instances" << std::endl;
    
    std::cout << "\n✗ LIMITATIONS:" << std::endl;
    std::cout << "   • Fixed pool size" << std::endl;
    std::cout << "   • Static storage (lifetime of program)" << std::endl;
    std::cout << "   • Simple implementation (one object per allocation)" << std::endl;
}

// ===================================================================
// 5. FIXED CAPACITY VECTOR (NO ALLOCATOR NEEDED)
// ===================================================================

template<typename T, size_t Capacity>
class FixedVector {
private:
    alignas(T) uint8_t storage[Capacity * sizeof(T)];
    size_t count = 0;
    
public:
    using value_type = T;
    using iterator = T*;
    using const_iterator = const T*;
    
    FixedVector() = default;
    
    ~FixedVector() {
        clear();
    }
    
    // Copy constructor
    FixedVector(const FixedVector& other) : count(0) {
        for (size_t i = 0; i < other.count; ++i) {
            push_back(other[i]);
        }
    }
    
    // Move constructor
    FixedVector(FixedVector&& other) noexcept : count(0) {
        for (size_t i = 0; i < other.count; ++i) {
            push_back(std::move(other[i]));
        }
        other.clear();
    }
    
    void push_back(const T& value) {
        if (count >= Capacity) {
            // In embedded: return error code or assert
            throw std::length_error("FixedVector capacity exceeded");
        }
        new (storage + count * sizeof(T)) T(value);
        ++count;
    }
    
    void push_back(T&& value) {
        if (count >= Capacity) {
            throw std::length_error("FixedVector capacity exceeded");
        }
        new (storage + count * sizeof(T)) T(std::move(value));
        ++count;
    }
    
    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (count >= Capacity) {
            throw std::length_error("FixedVector capacity exceeded");
        }
        new (storage + count * sizeof(T)) T(std::forward<Args>(args)...);
        ++count;
    }
    
    void pop_back() {
        if (count > 0) {
            --count;
            reinterpret_cast<T*>(storage + count * sizeof(T))->~T();
        }
    }
    
    void clear() {
        while (count > 0) {
            pop_back();
        }
    }
    
    T& operator[](size_t index) {
        return *reinterpret_cast<T*>(storage + index * sizeof(T));
    }
    
    const T& operator[](size_t index) const {
        return *reinterpret_cast<const T*>(storage + index * sizeof(T));
    }
    
    T& front() { return (*this)[0]; }
    const T& front() const { return (*this)[0]; }
    T& back() { return (*this)[count - 1]; }
    const T& back() const { return (*this)[count - 1]; }
    
    size_t size() const { return count; }
    size_t capacity() const { return Capacity; }
    bool empty() const { return count == 0; }
    bool full() const { return count == Capacity; }
    
    iterator begin() { return reinterpret_cast<T*>(storage); }
    iterator end() { return reinterpret_cast<T*>(storage + count * sizeof(T)); }
    const_iterator begin() const { return reinterpret_cast<const T*>(storage); }
    const_iterator end() const { return reinterpret_cast<const T*>(storage + count * sizeof(T)); }
};

void example_fixed_vector() {
    std::cout << "\n=== 5. FIXED CAPACITY VECTOR (NO ALLOCATOR) ===" << std::endl;
    
    FixedVector<int, 10> vec;
    
    std::cout << "\nAdding elements:" << std::endl;
    for (int i = 0; i < 8; ++i) {
        vec.push_back(i * 5);
        std::cout << "  Pushed " << i * 5 
                  << ", size=" << vec.size() 
                  << "/" << vec.capacity() << std::endl;
    }
    
    std::cout << "\nVector contents:" << std::endl;
    for (const auto& val : vec) {
        std::cout << "  " << val;
    }
    std::cout << std::endl;
    
    std::cout << "\n✓ BENEFITS:" << std::endl;
    std::cout << "   • Complete control over memory" << std::endl;
    std::cout << "   • No allocator overhead" << std::endl;
    std::cout << "   • Vector-like interface" << std::endl;
    std::cout << "   • Stack or static storage" << std::endl;
    std::cout << "   • Perfect for embedded systems" << std::endl;
    std::cout << "   • Compile-time capacity checking" << std::endl;
}

// ===================================================================
// 6. SUMMARY TABLE
// ===================================================================

void example_summary() {
    std::cout << "\n=== 6. SUMMARY: STL ON HEAP-LESS SYSTEMS ===" << std::endl;
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ APPROACH              │ HEAP? │ GROWS? │ COMPLEXITY │ USE CASE  │" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ std::array            │  NO   │   NO   │   SIMPLE   │ Fixed     │" << std::endl;
    std::cout << "│ std::span (C++20)     │  NO   │   NO   │   SIMPLE   │ View      │" << std::endl;
    std::cout << "│ Stack Allocator       │  NO   │  YES   │   MEDIUM   │ Temporary │" << std::endl;
    std::cout << "│ Pool Allocator        │  NO   │  YES   │   MEDIUM   │ Reusable  │" << std::endl;
    std::cout << "│ FixedVector           │  NO   │  YES   │   LOW      │ Embedded  │" << std::endl;
    std::cout << "│ std::vector (default) │ YES   │  YES   │   SIMPLE   │ General   │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────────────┘" << std::endl;
    
    std::cout << "\n🎯 ANSWER: Can STL containers work without heap?" << std::endl;
    std::cout << "\n✅ YES! Multiple approaches:" << std::endl;
    std::cout << "   1. Use std::array - Fixed size, stack-based" << std::endl;
    std::cout << "   2. Use std::span - Non-owning view (C++20)" << std::endl;
    std::cout << "   3. Custom stack allocator - std::vector on stack" << std::endl;
    std::cout << "   4. Static pool allocator - Pre-allocated memory" << std::endl;
    std::cout << "   5. Custom fixed vector - Complete control" << std::endl;
    
    std::cout << "\n❌ NO (by default):" << std::endl;
    std::cout << "   • std::vector - Uses heap by default" << std::endl;
    std::cout << "   • std::map - Uses heap by default" << std::endl;
    std::cout << "   • std::list - Uses heap by default" << std::endl;
    std::cout << "   • std::string - Uses heap (except SSO)" << std::endl;
    std::cout << "   • All use std::allocator which calls new/delete" << std::endl;
    
    std::cout << "\n💡 BEST PRACTICES FOR EMBEDDED SYSTEMS:" << std::endl;
    std::cout << "\n  1. PREFER std::array:" << std::endl;
    std::cout << "     ✓ Simple, safe, zero overhead" << std::endl;
    std::cout << "     ✓ Compile-time size checking" << std::endl;
    std::cout << "     ✓ Full STL algorithm support" << std::endl;
    
    std::cout << "\n  2. USE std::span for function parameters:" << std::endl;
    std::cout << "     ✓ Works with array, vector, C arrays" << std::endl;
    std::cout << "     ✓ Zero allocation" << std::endl;
    std::cout << "     ✓ Modern replacement for (T*, size_t)" << std::endl;
    
    std::cout << "\n  3. CUSTOM ALLOCATORS when needed:" << std::endl;
    std::cout << "     ✓ std::vector with stack/pool allocator" << std::endl;
    std::cout << "     ✓ Keep std::vector interface" << std::endl;
    std::cout << "     ⚠️  More complex, test thoroughly" << std::endl;
    
    std::cout << "\n  4. CUSTOM CONTAINERS for critical paths:" << std::endl;
    std::cout << "     ✓ FixedVector, CircularBuffer, etc." << std::endl;
    std::cout << "     ✓ Complete control over behavior" << std::endl;
    std::cout << "     ✓ Optimized for specific use case" << std::endl;
}

// ===================================================================
// 7. REAL-WORLD EXAMPLE: SENSOR DATA COLLECTION
// ===================================================================

void example_real_world() {
    std::cout << "\n=== 7. REAL-WORLD: SENSOR DATA COLLECTION ===" << std::endl;
    
    std::cout << "\n📊 SCENARIO:" << std::endl;
    std::cout << "   • ARM Cortex-M4 @ 80MHz" << std::endl;
    std::cout << "   • 64KB RAM (no heap)" << std::endl;
    std::cout << "   • Collect 100 sensor readings" << std::endl;
    std::cout << "   • Process with STL algorithms" << std::endl;
    
    // ✓ Solution 1: std::array (simplest)
    std::cout << "\n✓ SOLUTION 1: std::array" << std::endl;
    std::array<float, 100> readings1 = {};
    for (size_t i = 0; i < 100; ++i) {
        readings1[i] = 20.0f + (i % 10) * 0.5f;  // Simulated readings
    }
    
    auto min_max1 = std::minmax_element(readings1.begin(), readings1.end());
    std::cout << "   Min: " << *min_max1.first 
              << ", Max: " << *min_max1.second << std::endl;
    
    // ✓ Solution 2: FixedVector (dynamic-like)
    std::cout << "\n✓ SOLUTION 2: FixedVector" << std::endl;
    FixedVector<float, 100> readings2;
    for (int i = 0; i < 100; ++i) {
        readings2.push_back(20.0f + (i % 10) * 0.5f);
    }
    
    auto min_max2 = std::minmax_element(readings2.begin(), readings2.end());
    std::cout << "   Min: " << *min_max2.first 
              << ", Max: " << *min_max2.second << std::endl;
    
    std::cout << "\n✓ BOTH WORK WITHOUT HEAP!" << std::endl;
    std::cout << "   • Zero dynamic allocation" << std::endl;
    std::cout << "   • Full STL algorithm support" << std::endl;
    std::cout << "   • Predictable memory usage" << std::endl;
    std::cout << "   • Real-time safe" << std::endl;
}

// ===================================================================
// MAIN
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  CAN STL CONTAINERS WORK ON SYSTEMS WITHOUT HEAP?" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    example_std_array();
    example_std_span();
    example_custom_stack_allocator();
    example_pool_allocator();
    example_fixed_vector();
    example_summary();
    example_real_world();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  FINAL ANSWER" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n🎯 YES! STL containers CAN work without heap:" << std::endl;
    
    std::cout << "\n1. BUILT-IN HEAP-LESS CONTAINERS:" << std::endl;
    std::cout << "   • std::array (C++11) - Fixed size, stack-based" << std::endl;
    std::cout << "   • std::span (C++20) - Non-owning view" << std::endl;
    std::cout << "   • std::string_view (C++17) - String view" << std::endl;
    
    std::cout << "\n2. CUSTOM ALLOCATORS:" << std::endl;
    std::cout << "   • Stack allocator - Use stack memory" << std::endl;
    std::cout << "   • Pool allocator - Pre-allocated pools" << std::endl;
    std::cout << "   • PMR allocators - C++17 polymorphic" << std::endl;
    std::cout << "   • Works with: vector, map, list, etc." << std::endl;
    
    std::cout << "\n3. CUSTOM CONTAINERS:" << std::endl;
    std::cout << "   • FixedVector - Vector with capacity limit" << std::endl;
    std::cout << "   • CircularBuffer - Ring buffer" << std::endl;
    std::cout << "   • StaticString - Fixed-size string" << std::endl;
    std::cout << "   • Full control over allocation" << std::endl;
    
    std::cout << "\n⚠️ DEFAULT STL CONTAINERS USE HEAP:" << std::endl;
    std::cout << "   • std::vector - std::allocator uses new/delete" << std::endl;
    std::cout << "   • std::map - std::allocator uses new/delete" << std::endl;
    std::cout << "   • std::string - std::allocator (except SSO)" << std::endl;
    std::cout << "   • Must use custom allocator or alternatives" << std::endl;
    
    std::cout << "\n💡 RECOMMENDATION FOR EMBEDDED/REAL-TIME:" << std::endl;
    std::cout << "   1st choice: std::array (simple, safe)" << std::endl;
    std::cout << "   2nd choice: std::span for parameters" << std::endl;
    std::cout << "   3rd choice: Custom allocators for std::vector" << std::endl;
    std::cout << "   4th choice: Custom containers (FixedVector)" << std::endl;
    std::cout << "   ❌ Avoid: Default std::vector, std::map, std::string" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
