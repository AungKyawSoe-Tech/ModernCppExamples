// ===================================================================
// STL CONTAINERS FOR SAFETY-CRITICAL SYSTEMS
// ===================================================================
// Comprehensive guide for using STL containers in:
// - ISO 26262 (Automotive functional safety)
// - DO-178C (Avionics software)
// - IEC 61508 (Industrial safety)
// - Medical device software (IEC 62304)
//
// Based on:
// - MISRA C++:2008 and MISRA C++:2023
// - AUTOSAR C++14 Coding Guidelines
// - High Integrity C++ (HICPP)
// - JSF AV C++ Coding Standards
//
// Key concerns in safety-critical systems:
// 1. Deterministic behavior (no unpredictable timing)
// 2. Bounded memory usage (no dynamic allocation at runtime)
// 3. No hidden control flow (exceptions, virtual functions)
// 4. Verifiable and testable code
// 5. No undefined behavior
//
// Build: g++ -std=c++20 -Wall -Wextra -Wpedantic -O2 -o SafetyCriticalSTLContainers SafetyCriticalSTLContainers.cpp
// ===================================================================

#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <forward_list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <string>
#include <algorithm>

// ===================================================================
// SECTION 1: CONTAINER CLASSIFICATION FOR SAFETY-CRITICAL SYSTEMS
// ===================================================================

namespace container_classification {

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 1: STL CONTAINERS - SAFETY-CRITICAL CLASSIFICATION\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "┌────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ CONTAINER          │ HEAP? │ NODE-BASED? │ SAFE? │ NOTES              │\n";
    std::cout << "├────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ std::array         │  NO   │     NO      │  ✅   │ BEST for safety    │\n";
    std::cout << "│ std::vector        │  YES  │     NO      │  ⚠️   │ OK with allocator  │\n";
    std::cout << "│ std::list          │  YES  │     YES     │  ❌   │ AVOID per-node     │\n";
    std::cout << "│ std::forward_list  │  YES  │     YES     │  ❌   │ AVOID per-node     │\n";
    std::cout << "│ std::deque         │  YES  │     NO      │  ❌   │ AVOID complex      │\n";
    std::cout << "│ std::set           │  YES  │     YES     │  ❌   │ AVOID per-node     │\n";
    std::cout << "│ std::map           │  YES  │     YES     │  ❌   │ AVOID per-node     │\n";
    std::cout << "│ std::multiset      │  YES  │     YES     │  ❌   │ AVOID per-node     │\n";
    std::cout << "│ std::multimap      │  YES  │     YES     │  ❌   │ AVOID per-node     │\n";
    std::cout << "│ std::unordered_set │  YES  │     YES     │  ❌   │ AVOID per-bucket   │\n";
    std::cout << "│ std::unordered_map │  YES  │     YES     │  ❌   │ AVOID per-bucket   │\n";
    std::cout << "│ std::unordered_*   │  YES  │     YES     │  ❌   │ AVOID per-bucket   │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "KEY ISSUES WITH HEAP ALLOCATION IN SAFETY-CRITICAL SYSTEMS:\n\n";
    
    std::cout << "1. NON-DETERMINISTIC TIMING:\n";
    std::cout << "   • malloc/new can take variable time depending on:\n";
    std::cout << "     - Current heap fragmentation\n";
    std::cout << "     - Size of allocation request\n";
    std::cout << "     - Operating system state\n";
    std::cout << "   • Worst-case execution time (WCET) becomes unpredictable\n";
    std::cout << "   • Unacceptable for hard real-time systems\n\n";
    
    std::cout << "2. MEMORY FRAGMENTATION:\n";
    std::cout << "   • Repeated allocations/deallocations cause fragmentation\n";
    std::cout << "   • Can lead to allocation failures even with available memory\n";
    std::cout << "   • Node-based containers (list, map, set) are worst offenders\n";
    std::cout << "   • Each element requires separate heap allocation\n\n";
    
    std::cout << "3. ALLOCATION FAILURES:\n";
    std::cout << "   • new can throw std::bad_alloc (exception handling required)\n";
    std::cout << "   • malloc can return nullptr (error handling required)\n";
    std::cout << "   • Difficult to recover gracefully in critical systems\n";
    std::cout << "   • May violate MISRA/AUTOSAR guidelines on exceptions\n\n";
    
    std::cout << "4. NODE-BASED CONTAINERS (list, map, set, unordered_*):\n";
    std::cout << "   • EACH ELEMENT requires separate heap allocation\n";
    std::cout << "   • 1000 elements = 1000 malloc/free calls\n";
    std::cout << "   • Massive fragmentation over time\n";
    std::cout << "   • Iterator/pointer invalidation issues\n";
    std::cout << "   • Hidden per-node overhead (8-24 bytes per element)\n\n";
    
    std::cout << "5. STANDARDS GUIDANCE:\n";
    std::cout << "   • MISRA C++ Rule 18-4-1: Dynamic heap allocation shall not be used\n";
    std::cout << "   • AUTOSAR A18-5-1: Functions malloc, calloc, realloc, free not used\n";
    std::cout << "   • ISO 26262: Recommends static memory allocation for ASIL-D\n";
    std::cout << "   • DO-178C: Dynamic allocation discouraged for Level A software\n";
}

} // namespace container_classification

// ===================================================================
// SECTION 2: CONTAINERS TO AVOID IN SAFETY-CRITICAL SYSTEMS
// ===================================================================

namespace containers_to_avoid {

void demonstrate_list_issues() {
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "WHY AVOID std::list, std::forward_list?\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::cout << "PROBLEM: Each element requires separate heap allocation\n\n";
    
    // Demonstrate list overhead
    std::list<int32_t> my_list;
    
    std::cout << "Adding 5 elements to std::list:\n";
    for (int32_t i = 0; i < 5; ++i) {
        my_list.push_back(i * 10);
        std::cout << "  push_back(" << i * 10 << ") - HEAP ALLOCATION #" << (i + 1) << "\n";
    }
    
    std::cout << "\n❌ ISSUES:\n";
    std::cout << "   • 5 separate malloc() calls - non-deterministic timing\n";
    std::cout << "   • Each node has pointer overhead (16-24 bytes per element)\n";
    std::cout << "   • Fragmentation increases with each allocation\n";
    std::cout << "   • Cannot use custom allocator to pre-allocate (node-by-node)\n";
    std::cout << "   • Cache-unfriendly (nodes scattered in memory)\n\n";
    
    std::cout << "✅ ALTERNATIVE: std::array or std::vector with reserve()\n";
}

void demonstrate_map_set_issues() {
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "WHY AVOID std::map, std::set, std::multimap, std::multiset?\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::cout << "PROBLEM: Red-black tree with per-node heap allocation\n\n";
    
    std::map<int32_t, std::string> my_map;
    
    std::cout << "Adding 3 entries to std::map:\n";
    my_map[1] = "Critical";
    std::cout << "  map[1] = \"Critical\" - HEAP ALLOCATION for node\n";
    my_map[2] = "Warning";
    std::cout << "  map[2] = \"Warning\" - HEAP ALLOCATION for node\n";
    my_map[3] = "Info";
    std::cout << "  map[3] = \"Info\" - HEAP ALLOCATION for node\n";
    
    std::cout << "\n❌ ISSUES:\n";
    std::cout << "   • Each insertion allocates a tree node (40-64 bytes overhead!)\n";
    std::cout << "   • Rebalancing operations (rotations) at runtime\n";
    std::cout << "   • Non-constant insertion time: O(log n) with allocation\n";
    std::cout << "   • Cannot pre-allocate all nodes\n";
    std::cout << "   • Iterators invalidated on modification\n\n";
    
    std::cout << "✅ ALTERNATIVES:\n";
    std::cout << "   • std::array of key-value pairs + binary search\n";
    std::cout << "   • std::vector of pairs + sort + binary search\n";
    std::cout << "   • Fixed-size hash table (compile-time allocation)\n";
}

void demonstrate_unordered_issues() {
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "WHY AVOID std::unordered_map, std::unordered_set, std::unordered_multimap?\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::cout << "PROBLEM: Hash table with bucket allocation + chaining\n\n";
    
    std::unordered_map<int32_t, std::string> my_unordered_map;
    
    std::cout << "Adding 3 entries to std::unordered_map:\n";
    my_unordered_map[100] = "Sensor1";
    std::cout << "  map[100] = \"Sensor1\" - HEAP: buckets + node\n";
    my_unordered_map[200] = "Sensor2";
    std::cout << "  map[200] = \"Sensor2\" - HEAP: buckets + node\n";
    my_unordered_map[300] = "Sensor3";
    std::cout << "  map[300] = \"Sensor3\" - HEAP: buckets + node\n";
    
    std::cout << "\n❌ ISSUES:\n";
    std::cout << "   • Initial bucket array allocation\n";
    std::cout << "   • Each element allocated separately (chaining)\n";
    std::cout << "   • REHASHING at runtime (load factor > threshold)\n";
    std::cout << "     - Allocates NEW larger bucket array\n";
    std::cout << "     - Rehashes ALL elements (moves to new buckets)\n";
    std::cout << "     - Deallocates old bucket array\n";
    std::cout << "     - MASSIVE non-determinism!\n";
    std::cout << "   • Cannot predict WCET (worst-case execution time)\n";
    std::cout << "   • Hash collisions cause unpredictable performance\n\n";
    
    std::cout << "✅ ALTERNATIVES:\n";
    std::cout << "   • Fixed-size hash table (no rehashing)\n";
    std::cout << "   • Perfect hashing (compile-time)\n";
    std::cout << "   • Sorted std::array + binary search\n";
}

void demonstrate_deque_issues() {
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "WHY AVOID std::deque?\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::cout << "PROBLEM: Complex internal structure with multiple allocations\n\n";
    
    std::cout << "std::deque internal structure:\n";
    std::cout << "  • Array of pointers to fixed-size chunks\n";
    std::cout << "  • Each chunk allocated separately\n";
    std::cout << "  • Grows by allocating new chunks + updating pointer array\n";
    std::cout << "  • Pointer array itself may need reallocation\n\n";
    
    std::cout << "❌ ISSUES:\n";
    std::cout << "   • Multiple heap allocations (chunks + pointer array)\n";
    std::cout << "   • Complex iterator invalidation rules\n";
    std::cout << "   • Non-contiguous memory (cache-unfriendly)\n";
    std::cout << "   • Unpredictable memory usage pattern\n";
    std::cout << "   • Difficult to analyze for WCET\n\n";
    
    std::cout << "✅ ALTERNATIVES:\n";
    std::cout << "   • std::array (fixed size)\n";
    std::cout << "   • std::vector with reserve() (growable)\n";
    std::cout << "   • Circular buffer (custom implementation)\n";
}

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 2: CONTAINERS TO AVOID IN SAFETY-CRITICAL SYSTEMS\n";
    std::cout << std::string(80, '=') << "\n";
    
    demonstrate_list_issues();
    demonstrate_map_set_issues();
    demonstrate_unordered_issues();
    demonstrate_deque_issues();
}

} // namespace containers_to_avoid

// ===================================================================
// SECTION 3: SAFE ALTERNATIVES - USING CUSTOM ALLOCATORS
// ===================================================================

namespace safe_alternatives {

// Pre-allocated memory pool allocator for safety-critical systems
template<typename T, size_t PoolSize>
class SafetyPoolAllocator {
private:
    struct alignas(T) Block {
        uint8_t data[sizeof(T)];
    };
    
    // Static storage - allocated at compile time
    inline static Block storage[PoolSize];
    inline static bool used[PoolSize] = {};
    inline static size_t allocated_count = 0;
    
public:
    using value_type = T;
    using size_type = std::size_t;
    
    SafetyPoolAllocator() noexcept = default;
    
    template<typename U>
    SafetyPoolAllocator(const SafetyPoolAllocator<U, PoolSize>&) noexcept {}
    
    template<typename U>
    struct rebind {
        using other = SafetyPoolAllocator<U, PoolSize>;
    };
    
    // Allocate from pool - bounded, deterministic
    [[nodiscard]] T* allocate(size_t n) {
        if (n > PoolSize || allocated_count + n > PoolSize) {
            // In safety-critical systems, handle gracefully without exception
            return nullptr;  // Or use error code
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
        
        return nullptr;  // Pool exhausted
    }
    
    void deallocate(T* p, size_t n) noexcept {
        if (!p) return;
        
        // Find and mark blocks as free
        for (size_t i = 0; i < PoolSize; ++i) {
            if (reinterpret_cast<T*>(&storage[i]) == p) {
                for (size_t j = 0; j < n && (i + j) < PoolSize; ++j) {
                    used[i + j] = false;
                }
                allocated_count -= n;
                return;
            }
        }
    }
    
    static size_t capacity() { return PoolSize; }
    static size_t available() { return PoolSize - allocated_count; }
};

template<typename T, size_t N, typename U, size_t M>
bool operator==(const SafetyPoolAllocator<T, N>&, const SafetyPoolAllocator<U, M>&) noexcept {
    return N == M;
}

template<typename T, size_t N, typename U, size_t M>
bool operator!=(const SafetyPoolAllocator<T, N>&, const SafetyPoolAllocator<U, M>&) noexcept {
    return N != M;
}

void demonstrate_vector_with_allocator() {
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "SAFE: std::vector WITH CUSTOM ALLOCATOR\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::cout << "✅ Solution: Pre-allocated pool at initialization\n\n";
    
    using SafeVector = std::vector<int32_t, SafetyPoolAllocator<int32_t, 100>>;
    
    std::cout << "Creating std::vector with SafetyPoolAllocator<int32_t, 100>:\n";
    std::cout << "  • Pool allocated at compile-time (static storage)\n";
    std::cout << "  • Max 100 elements (bounded memory)\n";
    std::cout << "  • No heap allocation during runtime\n";
    std::cout << "  • Deterministic allocation from pool\n\n";
    
    SafeVector safe_vec;
    safe_vec.reserve(10);  // Reserve from pool
    
    std::cout << "Adding 10 elements:\n";
    for (int32_t i = 0; i < 10; ++i) {
        safe_vec.push_back(i * 10);
        std::cout << "  push_back(" << i * 10 << ") - from POOL, not heap\n";
    }
    
    std::cout << "\n✅ BENEFITS:\n";
    std::cout << "   • NO heap allocation (malloc/new)\n";
    std::cout << "   • Deterministic timing (pool allocation is O(n) bounded)\n";
    std::cout << "   • No fragmentation\n";
    std::cout << "   • Bounded memory usage (max 100 elements)\n";
    std::cout << "   • Keeps std::vector interface\n";
    std::cout << "   • Compatible with algorithms (sort, find, etc.)\n\n";
    
    std::cout << "⚠️  LIMITATIONS:\n";
    std::cout << "   • Fixed maximum capacity (100 in this example)\n";
    std::cout << "   • Must handle allocation failures gracefully\n";
    std::cout << "   • Pool exhaustion returns nullptr (not exception)\n";
    std::cout << "   • Requires careful sizing during design phase\n";
}

void demonstrate_array_alternative() {
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "SAFEST: std::array - NO ALLOCATOR NEEDED\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::cout << "✅ BEST SOLUTION for fixed-size data: std::array\n\n";
    
    std::array<int32_t, 10> safe_array = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
    
    std::cout << "std::array<int32_t, 10> properties:\n";
    std::cout << "  • Zero heap allocation (stack or static storage)\n";
    std::cout << "  • Size known at compile time\n";
    std::cout << "  • Bounds checking with .at()\n";
    std::cout << "  • Compatible with STL algorithms\n";
    std::cout << "  • constexpr support (C++17)\n";
    std::cout << "  • No overhead vs C array\n\n";
    
    std::cout << "Array contents:\n  ";
    for (const auto& val : safe_array) {
        std::cout << val << " ";
    }
    std::cout << "\n\n";
    
    std::cout << "✅ PERFECT FOR SAFETY-CRITICAL:\n";
    std::cout << "   • ISO 26262 ASIL-D compliant\n";
    std::cout << "   • MISRA C++ compliant (prefer over C arrays)\n";
    std::cout << "   • AUTOSAR C++14 recommended\n";
    std::cout << "   • DO-178C Level A acceptable\n";
    std::cout << "   • Deterministic behavior (O(1) access)\n";
    std::cout << "   • Predictable memory footprint\n";
    std::cout << "   • No hidden control flow\n";
    std::cout << "   • Verifiable and testable\n";
}

void demonstrate_initialization_phase_pattern() {
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "PATTERN: Initialization Phase vs Runtime Phase\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::cout << "GOLDEN RULE for safety-critical systems:\n";
    std::cout << "  'Allocate during INITIALIZATION, operate during RUNTIME'\n\n";
    
    std::cout << "┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ PHASE           │ ALLOWED                │ FORBIDDEN       │\n";
    std::cout << "├─────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ INITIALIZATION  │ • Dynamic allocation   │ • Nothing       │\n";
    std::cout << "│ (startup)       │ • reserve()            │                 │\n";
    std::cout << "│                 │ • Pool setup           │                 │\n";
    std::cout << "│                 │ • Resource acquisition │                 │\n";
    std::cout << "├─────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ RUNTIME         │ • Read/write data      │ • new/malloc    │\n";
    std::cout << "│ (operational)   │ • Fixed operations     │ • delete/free   │\n";
    std::cout << "│                 │ • Pool allocation      │ • realloc       │\n";
    std::cout << "│                 │ • Bounded algorithms   │ • resize/grow   │\n";
    std::cout << "│                 │                        │ • Exceptions    │\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "EXAMPLE: Two-phase lifecycle\n\n";
    
    std::cout << "// ===== INITIALIZATION PHASE (startup, non-critical) =====\n";
    std::cout << "void initialize() {\n";
    std::cout << "    static std::vector<SensorData> sensor_buffer;\n";
    std::cout << "    sensor_buffer.reserve(1000);  // ONE-TIME allocation\n";
    std::cout << "    // Pre-allocate ALL buffers, pools, resources\n";
    std::cout << "}\n\n";
    
    std::cout << "// ===== RUNTIME PHASE (operational, time-critical) =====\n";
    std::cout << "void process_sensor_data(SensorData data) {\n";
    std::cout << "    // NO allocation here! Only use pre-allocated memory\n";
    std::cout << "    sensor_buffer.push_back(data);  // No realloc (reserved)\n";
    std::cout << "    if (sensor_buffer.size() > 1000) {\n";
    std::cout << "        sensor_buffer.erase(sensor_buffer.begin());  // Shift\n";
    std::cout << "    }\n";
    std::cout << "}\n\n";
    
    std::cout << "✅ KEY PRINCIPLES:\n";
    std::cout << "   1. Allocate ONCE during initialization\n";
    std::cout << "   2. Use reserve() to prevent reallocation\n";
    std::cout << "   3. Never exceed reserved capacity at runtime\n";
    std::cout << "   4. Use static or global storage for long-lived data\n";
    std::cout << "   5. Monitor memory usage during testing\n";
    std::cout << "   6. Assert/verify no runtime allocation in production\n";
}

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 3: SAFE ALTERNATIVES WITH CUSTOM ALLOCATORS\n";
    std::cout << std::string(80, '=') << "\n";
    
    demonstrate_vector_with_allocator();
    demonstrate_array_alternative();
    demonstrate_initialization_phase_pattern();
}

} // namespace safe_alternatives

// ===================================================================
// SECTION 4: STANDARDS AND GUIDELINES SUMMARY
// ===================================================================

namespace standards_summary {

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 4: STANDARDS AND GUIDELINES SUMMARY\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "┌────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ STANDARD / GUIDELINE │ KEY REQUIREMENTS                                │\n";
    std::cout << "├────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ MISRA C++:2008        │ Rule 18-4-1: No dynamic heap allocation        │\n";
    std::cout << "│                       │ Rule 5-2-4: C-style casts forbidden            │\n";
    std::cout << "│                       │ Rule 27-0-1: <cstdio> functions discouraged    │\n";
    std::cout << "├────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ MISRA C++:2023        │ Allows std::array, std::span                   │\n";
    std::cout << "│                       │ Allows std::vector with reserve()              │\n";
    std::cout << "│                       │ Custom allocators acceptable                   │\n";
    std::cout << "├────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ AUTOSAR C++14         │ A18-5-1: malloc/calloc/realloc/free forbidden  │\n";
    std::cout << "│                       │ A18-5-2: new/delete in non-throwing form only  │\n";
    std::cout << "│                       │ A18-1-1: Prefer std::array over C arrays       │\n";
    std::cout << "├────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ ISO 26262 (ASIL-D)    │ Recommends static allocation                   │\n";
    std::cout << "│                       │ Dynamic allocation requires justification      │\n";
    std::cout << "│                       │ WCET analysis mandatory                        │\n";
    std::cout << "├────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ DO-178C Level A       │ Dynamic memory discouraged                     │\n";
    std::cout << "│ (Avionics)            │ Must prove bounded memory usage                │\n";
    std::cout << "│                       │ Full MC/DC coverage required                   │\n";
    std::cout << "├────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ IEC 61508 (SIL 3/4)   │ Deterministic behavior required                │\n";
    std::cout << "│                       │ Static analysis mandatory                      │\n";
    std::cout << "│                       │ Runtime monitoring recommended                 │\n";
    std::cout << "├────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ JSF AV C++            │ Rule 206: malloc/free not used                 │\n";
    std::cout << "│ (F-35 Fighter)        │ Rule 207: new/delete not used after init       │\n";
    std::cout << "│                       │ Rule 208: Prefer automatic storage             │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "CONTAINER RECOMMENDATIONS BY STANDARD:\n\n";
    
    std::cout << "✅ UNIVERSALLY ACCEPTED (all standards):\n";
    std::cout << "   • std::array - Fixed size, no allocation\n";
    std::cout << "   • std::span (C++20) - Non-owning view\n";
    std::cout << "   • C arrays with size (if bounds-checked)\n\n";
    
    std::cout << "⚠️  CONDITIONALLY ACCEPTED (with restrictions):\n";
    std::cout << "   • std::vector - ONLY with:\n";
    std::cout << "     - reserve() called during initialization\n";
    std::cout << "     - Custom allocator (pool-based)\n";
    std::cout << "     - Never exceeds reserved capacity at runtime\n";
    std::cout << "     - Documented maximum size\n\n";
    
    std::cout << "❌ UNIVERSALLY FORBIDDEN (all standards):\n";
    std::cout << "   • std::list - Per-node heap allocation\n";
    std::cout << "   • std::forward_list - Per-node heap allocation\n";
    std::cout << "   • std::map / std::set - Per-node heap allocation\n";
    std::cout << "   • std::multimap / std::multiset - Per-node heap allocation\n";
    std::cout << "   • std::unordered_map / std::unordered_set - Rehashing\n";
    std::cout << "   • std::unordered_multimap / std::unordered_multiset - Rehashing\n";
    std::cout << "   • std::deque - Complex internal structure\n";
    std::cout << "   • std::string - Dynamic (unless with custom allocator)\n\n";
    
    std::cout << "RATIONALE FOR PROHIBITIONS:\n\n";
    
    std::cout << "1. NODE-BASED CONTAINERS (list, map, set):\n";
    std::cout << "   Problem: Each element → separate allocation\n";
    std::cout << "   Impact: 1000 elements = 1000 heap operations\n";
    std::cout << "   Result: Massive fragmentation + non-deterministic timing\n\n";
    
    std::cout << "2. HASH CONTAINERS (unordered_map, unordered_set):\n";
    std::cout << "   Problem: Rehashing at unpredictable times\n";
    std::cout << "   Impact: Can reallocate entire bucket array at runtime\n";
    std::cout << "   Result: Unpredictable latency spikes (WCET violation)\n\n";
    
    std::cout << "3. DEQUE:\n";
    std::cout << "   Problem: Complex multi-level allocation\n";
    std::cout << "   Impact: Chunks + pointer array allocations\n";
    std::cout << "   Result: Difficult to analyze and verify\n\n";
}

} // namespace standards_summary

// ===================================================================
// SECTION 5: PRACTICAL DECISION GUIDE
// ===================================================================

namespace decision_guide {

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 5: PRACTICAL DECISION GUIDE\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "DECISION TREE: Which Container Should I Use?\n\n";
    
    std::cout << "┌─ Q: Is the size KNOWN at compile time?\n";
    std::cout << "│\n";
    std::cout << "├─ YES → Use std::array<T, N>\n";
    std::cout << "│         ✅ BEST choice for safety-critical\n";
    std::cout << "│         • Zero overhead\n";
    std::cout << "│         • No allocation\n";
    std::cout << "│         • Compile-time size checking\n";
    std::cout << "│\n";
    std::cout << "└─ NO → Continue to Q2\n\n";
    
    std::cout << "┌─ Q: Can I determine MAXIMUM size at design time?\n";
    std::cout << "│\n";
    std::cout << "├─ YES → Use std::vector with reserve(MAX_SIZE)\n";
    std::cout << "│         ⚠️  ACCEPTABLE with these conditions:\n";
    std::cout << "│         1. Call reserve() during initialization\n";
    std::cout << "│         2. Never exceed reserved capacity at runtime\n";
    std::cout << "│         3. Monitor usage during testing\n";
    std::cout << "│         4. Document maximum size\n";
    std::cout << "│         5. Consider custom allocator for extra safety\n";
    std::cout << "│\n";
    std::cout << "├─ NO (truly unbounded) → DESIGN PROBLEM!\n";
    std::cout << "│         ❌ Safety-critical systems MUST have bounded resources\n";
    std::cout << "│         • Revisit requirements\n";
    std::cout << "│         • Determine worst-case maximum\n";
    std::cout << "│         • Add resource limits\n";
    std::cout << "│         • Consider circular buffer pattern\n";
    std::cout << "│\n";
    std::cout << "└─ ALTERNATIVE → Fixed-capacity container (custom)\n\n";
    
    std::cout << "┌─ Q: Do I need to insert/remove in the MIDDLE frequently?\n";
    std::cout << "│\n";
    std::cout << "├─ YES (was thinking std::list) → RECONSIDER!\n";
    std::cout << "│         • std::list forbidden in safety-critical systems\n";
    std::cout << "│         • Alternatives:\n";
    std::cout << "│           1. std::vector + erase/insert (usually fast enough)\n";
    std::cout << "│           2. Circular buffer (if queue-like)\n";
    std::cout << "│           3. Fixed-size priority queue\n";
    std::cout << "│           4. Index-based linked list in std::array\n";
    std::cout << "│\n";
    std::cout << "└─ NO → std::vector or std::array is fine\n\n";
    
    std::cout << "┌─ Q: Do I need key-value mapping (was thinking std::map)?\n";
    std::cout << "│\n";
    std::cout << "├─ YES → ALTERNATIVES (no std::map!):\n";
    std::cout << "│         1. std::array of std::pair<Key, Value> + binary_search\n";
    std::cout << "│            • Sort during initialization\n";
    std::cout << "│            • Use binary_search at runtime (O(log n))\n";
    std::cout << "│            • No allocation after init\n";
    std::cout << "│\n";
    std::cout << "│         2. Fixed-size hash table (compile-time)\n";
    std::cout << "│            • Perfect hashing (no collisions)\n";
    std::cout << "│            • O(1) lookup\n";
    std::cout << "│            • Requires design-time hash analysis\n";
    std::cout << "│\n";
    std::cout << "│         3. std::vector<pair> + sort + binary_search\n";
    std::cout << "│            • Sorted during initialization\n";
    std::cout << "│            • Read-only at runtime\n";
    std::cout << "│            • Good for configuration tables\n";
    std::cout << "│\n";
    std::cout << "└─ NO → Continue\n\n";
    
    std::cout << "SUMMARY TABLE:\n\n";
    std::cout << "┌────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ NEED                  │ SAFE SOLUTION           │ AVOID                │\n";
    std::cout << "├────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Fixed-size sequence   │ std::array              │ C array (no bounds)  │\n";
    std::cout << "│ Bounded sequence      │ std::vector + reserve() │ std::vector (default)│\n";
    std::cout << "│ FIFO queue            │ Circular buffer         │ std::queue/std::list │\n";
    std::cout << "│ LIFO stack            │ std::array + index      │ std::stack (default) │\n";
    std::cout << "│ Priority queue        │ std::array + heap alg   │ std::priority_queue  │\n";
    std::cout << "│ Key-value map         │ sorted array + search   │ std::map             │\n";
    std::cout << "│ Hash table            │ Fixed hash table        │ std::unordered_map   │\n";
    std::cout << "│ Sorted set            │ sorted array + search   │ std::set             │\n";
    std::cout << "│ Linked list           │ Index-based in array    │ std::list            │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "GOLDEN RULES:\n\n";
    std::cout << "1. ALWAYS prefer std::array when size is known\n";
    std::cout << "2. NEVER use node-based containers (list, map, set)\n";
    std::cout << "3. NEVER use std::unordered_* (rehashing issue)\n";
    std::cout << "4. IF using std::vector:\n";
    std::cout << "   • Call reserve() during initialization\n";
    std::cout << "   • Never exceed reserved capacity at runtime\n";
    std::cout << "   • Consider custom allocator for extra safety\n";
    std::cout << "5. DOCUMENT maximum sizes for all containers\n";
    std::cout << "6. VERIFY no runtime allocation (static analysis)\n";
    std::cout << "7. TEST with memory allocation monitoring enabled\n";
}

} // namespace decision_guide

// ===================================================================
// MAIN
// ===================================================================

int main() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "STL CONTAINERS FOR SAFETY-CRITICAL SYSTEMS\n";
    std::cout << "ISO 26262 | DO-178C | MISRA C++ | AUTOSAR C++14\n";
    std::cout << std::string(80, '=') << "\n";
    
    container_classification::demonstrate();
    containers_to_avoid::demonstrate();
    safe_alternatives::demonstrate();
    standards_summary::demonstrate();
    decision_guide::demonstrate();
    
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "FINAL ANSWER TO: Which STL containers to avoid?\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "❌ ABSOLUTELY FORBIDDEN in safety-critical systems:\n\n";
    std::cout << "   1. std::list - Per-element heap allocation\n";
    std::cout << "   2. std::forward_list - Per-element heap allocation\n";
    std::cout << "   3. std::map - Per-node allocation (red-black tree)\n";
    std::cout << "   4. std::multimap - Per-node allocation\n";
    std::cout << "   5. std::set - Per-node allocation\n";
    std::cout << "   6. std::multiset - Per-node allocation\n";
    std::cout << "   7. std::unordered_map - Rehashing + per-bucket allocation\n";
    std::cout << "   8. std::unordered_multimap - Rehashing + per-bucket allocation\n";
    std::cout << "   9. std::unordered_set - Rehashing + per-bucket allocation\n";
    std::cout << "   10. std::unordered_multiset - Rehashing + per-bucket allocation\n";
    std::cout << "   11. std::deque - Complex multi-level allocation\n";
    std::cout << "   12. std::string - Dynamic (use with custom allocator only)\n\n";
    
    std::cout << "✅ YES, you CAN use containers with CUSTOM ALLOCATORS:\n\n";
    std::cout << "   • std::vector<T, CustomAllocator>\n";
    std::cout << "   • Pre-allocated pool-based allocator\n";
    std::cout << "   • Static memory pool (no heap)\n";
    std::cout << "   • Bounded capacity enforced\n";
    std::cout << "   • Allocation during initialization phase only\n\n";
    
    std::cout << "✅ SAFEST CHOICE (no allocator needed):\n\n";
    std::cout << "   • std::array<T, N> - Stack or static storage\n";
    std::cout << "   • std::span<T> (C++20) - Non-owning view\n";
    std::cout << "   • C++17 std::string_view - String view\n\n";
    
    std::cout << "💡 REMEMBER:\n";
    std::cout << "   'If heap is the issue, custom allocators can help,\n";
    std::cout << "    but std::array is ALWAYS the safest choice when size is known!'\n\n";
    
    std::cout << std::string(80, '=') << "\n\n";
    
    return 0;
}
