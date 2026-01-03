// ===================================================================
// PIMPL IDIOM IN MODERN C++ - STILL RELEVANT?
// ===================================================================
// Comprehensive guide covering:
// 1. What is Pimpl (Pointer to Implementation)?
// 2. Traditional benefits and use cases
// 3. Is it still relevant in Modern C++ (C++11/14/17/20/23)?
// 4. Performance implications (especially for real-time systems)
// 5. Cache locality and memory indirection problems
// 6. Alternatives and mitigations
// 7. When to use and when to avoid Pimpl
//
// KEY QUESTION: Why Pimpl may be BAD for real-time systems?
// ANSWER: Pointer indirection breaks cache locality, causes memory jumps,
//         unpredictable latency, and extra heap allocation overhead
//
// Build: g++ -std=c++20 -Wall -Wextra -O2 -o PimplIdiom PimplIdiom.cpp
// ===================================================================

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>

// ===================================================================
// SECTION 1: WHAT IS THE PIMPL IDIOM?
// ===================================================================

namespace what_is_pimpl {

// ==============================
// WITHOUT PIMPL - Traditional approach
// ==============================

// Widget.h (header file)
class WidgetNoPimpl {
private:
    // ALL implementation details exposed in header
    std::string name_;
    int id_;
    std::vector<double> data_;
    
    // Private helper functions also exposed
    void validateData();
    void processInternal();
    
public:
    WidgetNoPimpl(const std::string& name, int id);
    ~WidgetNoPimpl();
    
    void doSomething();
    void process();
};

// ❌ PROBLEMS WITHOUT PIMPL:
// 1. All private members visible in header (no encapsulation)
// 2. Changing private members requires recompiling ALL clients
// 3. Implementation details leak to users
// 4. Longer compile times (includes propagate)
// 5. Binary compatibility issues (ABI breaks easily)

// ==============================
// WITH PIMPL - Pointer to Implementation
// ==============================

// Widget.h (header file)
class WidgetWithPimpl {
private:
    // ONLY pointer to implementation - opaque pointer
    struct Impl;  // Forward declaration
    std::unique_ptr<Impl> pImpl_;
    
public:
    WidgetWithPimpl(const std::string& name, int id);
    ~WidgetWithPimpl();  // Must be in .cpp (where Impl is complete)
    
    // Move operations (copy disabled by default with unique_ptr)
    WidgetWithPimpl(WidgetWithPimpl&&) noexcept;
    WidgetWithPimpl& operator=(WidgetWithPimpl&&) noexcept;
    
    void doSomething();
    void process();
};

// Widget.cpp (implementation file)
struct WidgetWithPimpl::Impl {
    // ALL implementation details hidden here
    std::string name;
    int id;
    std::vector<double> data;
    
    Impl(const std::string& n, int i) : name(n), id(i) {}
    
    void validateData() {
        std::cout << "      Validating data for " << name << "\n";
    }
    
    void processInternal() {
        std::cout << "      Processing " << data.size() << " elements\n";
    }
};

// Implementation
WidgetWithPimpl::WidgetWithPimpl(const std::string& name, int id)
    : pImpl_(std::make_unique<Impl>(name, id)) {
    std::cout << "    WidgetWithPimpl constructed: " << name << "\n";
}

// Destructor must be in .cpp where Impl is complete type
WidgetWithPimpl::~WidgetWithPimpl() = default;

// Move operations
WidgetWithPimpl::WidgetWithPimpl(WidgetWithPimpl&&) noexcept = default;
WidgetWithPimpl& WidgetWithPimpl::operator=(WidgetWithPimpl&&) noexcept = default;

void WidgetWithPimpl::doSomething() {
    pImpl_->validateData();
    std::cout << "    Doing something with " << pImpl_->name << "\n";
}

void WidgetWithPimpl::process() {
    pImpl_->data.push_back(1.0);
    pImpl_->data.push_back(2.0);
    pImpl_->processInternal();
}

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 1: WHAT IS THE PIMPL IDIOM?\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "PIMPL = Pointer to IMPLementation (also called Opaque Pointer)\n\n";
    
    std::cout << "CONCEPT:\n";
    std::cout << "  • Public class holds ONLY a pointer to implementation\n";
    std::cout << "  • Implementation struct defined in .cpp file\n";
    std::cout << "  • All data members and private functions hidden in Impl\n";
    std::cout << "  • Forwards all operations to pImpl pointer\n\n";
    
    std::cout << "EXAMPLE:\n";
    WidgetWithPimpl widget("Sensor", 42);
    widget.doSomething();
    widget.process();
    
    std::cout << "\n✅ TRADITIONAL BENEFITS:\n";
    std::cout << "   1. COMPILATION FIREWALL\n";
    std::cout << "      • Change private members without recompiling clients\n";
    std::cout << "      • Faster incremental builds\n";
    std::cout << "      • Reduces header dependencies\n\n";
    
    std::cout << "   2. BINARY COMPATIBILITY (ABI STABILITY)\n";
    std::cout << "      • Can modify Impl without breaking ABI\n";
    std::cout << "      • Important for shared libraries\n";
    std::cout << "      • No need to relink client code\n\n";
    
    std::cout << "   3. INFORMATION HIDING\n";
    std::cout << "      • Private members truly hidden\n";
    std::cout << "      • Can use incomplete types in Impl\n";
    std::cout << "      • Better encapsulation\n\n";
    
    std::cout << "   4. HEADER FILE SIMPLICITY\n";
    std::cout << "      • Minimal includes in header\n";
    std::cout << "      • Cleaner public interface\n";
    std::cout << "      • Easier to read and maintain\n";
}

} // namespace what_is_pimpl

// ===================================================================
// SECTION 2: MODERN C++ IMPROVEMENTS - STILL RELEVANT?
// ===================================================================

namespace modern_cpp_pimpl {

// C++98: Raw pointer (manual memory management)
class WidgetCpp98 {
private:
    struct Impl;
    Impl* pImpl_;  // Raw pointer
    
public:
    WidgetCpp98();
    ~WidgetCpp98();
    
    // Rule of Three needed
    WidgetCpp98(const WidgetCpp98&);
    WidgetCpp98& operator=(const WidgetCpp98&);
};

// C++11: unique_ptr (automatic memory management)
class WidgetCpp11 {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;  // Smart pointer
    
public:
    WidgetCpp11();
    ~WidgetCpp11();  // Still needed for incomplete type
    
    // Rule of Five: Move-only by default (unique_ptr)
    WidgetCpp11(WidgetCpp11&&) noexcept;
    WidgetCpp11& operator=(WidgetCpp11&&) noexcept;
    
    // Copy operations require custom implementation if needed
    WidgetCpp11(const WidgetCpp11&) = delete;
    WidgetCpp11& operator=(const WidgetCpp11&) = delete;
};

// C++11: shared_ptr (for copyable Pimpl)
class WidgetCpp11Shared {
private:
    struct Impl;
    std::shared_ptr<Impl> pImpl_;  // Shared ownership
    
public:
    WidgetCpp11Shared();
    ~WidgetCpp11Shared() = default;  // No explicit destructor needed!
    
    // Copyable and movable by default
    WidgetCpp11Shared(const WidgetCpp11Shared&) = default;
    WidgetCpp11Shared& operator=(const WidgetCpp11Shared&) = default;
    WidgetCpp11Shared(WidgetCpp11Shared&&) noexcept = default;
    WidgetCpp11Shared& operator=(WidgetCpp11Shared&&) noexcept = default;
};

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 2: MODERN C++ IMPROVEMENTS - STILL RELEVANT?\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "EVOLUTION OF PIMPL:\n\n";
    
    std::cout << "C++98: Raw pointer (manual memory management)\n";
    std::cout << "  ❌ Manual new/delete\n";
    std::cout << "  ❌ Must implement Rule of Three\n";
    std::cout << "  ❌ Risk of memory leaks\n";
    std::cout << "  ❌ Exception safety issues\n\n";
    
    std::cout << "C++11: std::unique_ptr\n";
    std::cout << "  ✅ Automatic memory management\n";
    std::cout << "  ✅ Move-only by default\n";
    std::cout << "  ✅ Exception safe\n";
    std::cout << "  ⚠️  Still need destructor in .cpp (incomplete type)\n\n";
    
    std::cout << "C++11: std::shared_ptr\n";
    std::cout << "  ✅ Automatic memory management\n";
    std::cout << "  ✅ Copyable by default\n";
    std::cout << "  ✅ No destructor needed in .cpp!\n";
    std::cout << "  ❌ Reference counting overhead\n";
    std::cout << "  ❌ Two heap allocations (control block + Impl)\n\n";
    
    std::cout << "IS PIMPL STILL RELEVANT IN MODERN C++?\n\n";
    
    std::cout << "✅ YES, when you need:\n";
    std::cout << "   1. BINARY STABILITY (shared libraries, plugins)\n";
    std::cout << "      • Qt framework uses Pimpl extensively\n";
    std::cout << "      • Windows API uses opaque handles (HWND, HANDLE)\n";
    std::cout << "      • Linux kernel uses opaque pointers\n\n";
    
    std::cout << "   2. COMPILATION FIREWALL (large projects)\n";
    std::cout << "      • Reduce compilation dependencies\n";
    std::cout << "      • Faster incremental builds\n";
    std::cout << "      • Important for multi-million line codebases\n\n";
    
    std::cout << "   3. INTERFACE STABILITY (APIs)\n";
    std::cout << "      • Public interface never changes\n";
    std::cout << "      • Implementation can evolve freely\n";
    std::cout << "      • Versioning becomes easier\n\n";
    
    std::cout << "❌ LESS RELEVANT when:\n";
    std::cout << "   1. Using header-only libraries (templates)\n";
    std::cout << "   2. Internal classes (not exposed in API)\n";
    std::cout << "   3. Performance-critical code (real-time systems)\n";
    std::cout << "   4. Small classes with simple members\n";
    std::cout << "   5. Using modules (C++20) - reduces header dependencies\n";
}

} // namespace modern_cpp_pimpl

// ===================================================================
// SECTION 3: PERFORMANCE PROBLEMS - THE REAL-TIME KILLER
// ===================================================================

namespace performance_problems {

// Simple data structure for testing
struct SensorData {
    int id;
    double value;
    int64_t timestamp;
    
    SensorData(int i = 0, double v = 0.0) 
        : id(i), value(v), timestamp(0) {}
};

// ==============================
// VERSION 1: NO PIMPL - Direct data access
// ==============================

class SensorNoPimpl {
private:
    int id_;
    double value_;
    int64_t timestamp_;
    
public:
    SensorNoPimpl(int id, double value) 
        : id_(id), value_(value), timestamp_(0) {}
    
    void update(double new_value) {
        value_ = new_value;
        timestamp_++;
    }
    
    double getValue() const { return value_; }
    int getId() const { return id_; }
};

// ==============================
// VERSION 2: WITH PIMPL - Pointer indirection
// ==============================

class SensorWithPimpl {
private:
    struct Impl {
        int id;
        double value;
        int64_t timestamp;
        
        Impl(int i, double v) : id(i), value(v), timestamp(0) {}
    };
    
    std::unique_ptr<Impl> pImpl_;
    
public:
    SensorWithPimpl(int id, double value) 
        : pImpl_(std::make_unique<Impl>(id, value)) {}
    
    ~SensorWithPimpl() = default;
    
    void update(double new_value) {
        pImpl_->value = new_value;
        pImpl_->timestamp++;
    }
    
    double getValue() const { return pImpl_->value; }
    int getId() const { return pImpl_->id; }
};

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 3: PERFORMANCE PROBLEMS - THE REAL-TIME KILLER\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "PROBLEM 1: POINTER INDIRECTION OVERHEAD\n\n";
    
    std::cout << "Without Pimpl:\n";
    std::cout << "  sensor.getValue()  →  Direct access: sensor.value_\n";
    std::cout << "  • ONE memory access\n";
    std::cout << "  • CPU loads value directly from sensor object\n";
    std::cout << "  • Predictable: O(1) memory access\n\n";
    
    std::cout << "With Pimpl:\n";
    std::cout << "  sensor.getValue()  →  sensor.pImpl_->value\n";
    std::cout << "  • TWO memory accesses:\n";
    std::cout << "    1. Load pImpl_ pointer from sensor object\n";
    std::cout << "    2. Follow pointer to load value from Impl\n";
    std::cout << "  • Unpredictable: pointer could point anywhere in heap\n\n";
    
    std::cout << "PROBLEM 2: CACHE LOCALITY DESTROYED\n\n";
    
    std::cout << "Without Pimpl (Array of sensors):\n";
    std::cout << "  Memory layout:  [Sensor1][Sensor2][Sensor3][Sensor4]\n";
    std::cout << "  • Contiguous memory (cache-friendly)\n";
    std::cout << "  • CPU prefetcher loads next sensors automatically\n";
    std::cout << "  • All data in same cache line (64 bytes)\n\n";
    
    std::cout << "With Pimpl (Array of sensors):\n";
    std::cout << "  Memory layout:  [Sensor1*]→Impl1  [Sensor2*]→Impl2\n";
    std::cout << "                  scattered          scattered\n";
    std::cout << "                  in heap            in heap\n";
    std::cout << "  • Impl objects scattered across heap (cache-hostile)\n";
    std::cout << "  • Each access causes cache miss\n";
    std::cout << "  • CPU prefetcher cannot predict pattern\n";
    std::cout << "  • Every pointer dereference = potential cache miss\n\n";
    
    std::cout << "PROBLEM 3: HEAP ALLOCATION OVERHEAD\n\n";
    
    std::cout << "Without Pimpl:\n";
    std::cout << "  SensorNoPimpl sensor(1, 3.14);  // Stack allocation\n";
    std::cout << "  • O(1) construction time\n";
    std::cout << "  • No malloc/free overhead\n";
    std::cout << "  • Deterministic\n\n";
    
    std::cout << "With Pimpl:\n";
    std::cout << "  SensorWithPimpl sensor(1, 3.14);  // Heap allocation!\n";
    std::cout << "  • Calls malloc() for Impl\n";
    std::cout << "  • O(?) allocation time (non-deterministic)\n";
    std::cout << "  • Heap fragmentation over time\n";
    std::cout << "  • Destructor calls free() (non-deterministic)\n\n";
    
    // Benchmark
    constexpr int ITERATIONS = 1000000;
    
    std::cout << "BENCHMARK: " << ITERATIONS << " operations\n\n";
    
    // Test 1: No Pimpl
    {
        SensorNoPimpl sensor(1, 100.0);
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < ITERATIONS; ++i) {
            sensor.update(i * 0.1);
            [[maybe_unused]] volatile double val = sensor.getValue();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "  WITHOUT Pimpl: " << std::setw(8) << duration.count() << " μs\n";
    }
    
    // Test 2: With Pimpl
    {
        SensorWithPimpl sensor(1, 100.0);
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < ITERATIONS; ++i) {
            sensor.update(i * 0.1);
            [[maybe_unused]] volatile double val = sensor.getValue();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "  WITH Pimpl:    " << std::setw(8) << duration.count() << " μs\n";
    }
    
    std::cout << "\n❌ PIMPL OVERHEAD:\n";
    std::cout << "   • 50-200% slower (depends on cache behavior)\n";
    std::cout << "   • Non-deterministic timing variations\n";
    std::cout << "   • Extra heap allocation per object\n";
    std::cout << "   • Destroys cache locality\n";
}

} // namespace performance_problems

// ===================================================================
// SECTION 4: WHY PIMPL IS BAD FOR REAL-TIME SYSTEMS
// ===================================================================

namespace realtime_problems {

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 4: WHY PIMPL IS BAD FOR REAL-TIME SYSTEMS\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "REAL-TIME SYSTEM REQUIREMENTS:\n";
    std::cout << "  1. DETERMINISTIC TIMING - Predictable worst-case execution time (WCET)\n";
    std::cout << "  2. BOUNDED LATENCY - Maximum response time guaranteed\n";
    std::cout << "  3. NO HEAP ALLOCATION - At runtime (malloc/free non-deterministic)\n";
    std::cout << "  4. CACHE PREDICTABILITY - Minimize cache misses\n";
    std::cout << "  5. NO MEMORY JUMPS - Sequential memory access for prefetcher\n\n";
    
    std::cout << "PIMPL VIOLATIONS:\n\n";
    
    std::cout << "❌ VIOLATION 1: NON-DETERMINISTIC HEAP ALLOCATION\n";
    std::cout << "   Problem:\n";
    std::cout << "     • Each Pimpl object requires malloc() for Impl\n";
    std::cout << "     • malloc() time depends on heap state (fragmentation)\n";
    std::cout << "     • WCET becomes unpredictable\n";
    std::cout << "   Impact on Real-Time:\n";
    std::cout << "     • Cannot prove deadline guarantees\n";
    std::cout << "     • Violates MISRA C++ Rule 18-4-1 (no dynamic allocation)\n";
    std::cout << "     • Violates ISO 26262 (automotive safety) guidelines\n";
    std::cout << "     • Unacceptable for hard real-time (ASIL-D)\n\n";
    
    std::cout << "❌ VIOLATION 2: POINTER INDIRECTION = CACHE MISSES\n";
    std::cout << "   Problem:\n";
    std::cout << "     • Every operation requires following pImpl_ pointer\n";
    std::cout << "     • Impl object allocated somewhere in heap\n";
    std::cout << "     • High probability of cache miss on every access\n";
    std::cout << "   Impact on Real-Time:\n";
    std::cout << "     • Cache miss = 100+ cycles (vs 1 cycle for cache hit)\n";
    std::cout << "     • Unpredictable latency spikes\n";
    std::cout << "     • Worst-case timing analysis becomes pessimistic\n";
    std::cout << "     • May cause deadline misses in tight loops\n\n";
    
    std::cout << "❌ VIOLATION 3: DESTROYED CACHE LOCALITY\n";
    std::cout << "   Problem:\n";
    std::cout << "     • Array of Pimpl objects:\n";
    std::cout << "       [Obj1 ptr]→Impl1 (heap addr 0x1000)\n";
    std::cout << "       [Obj2 ptr]→Impl2 (heap addr 0x5000) ← 16KB away!\n";
    std::cout << "       [Obj3 ptr]→Impl3 (heap addr 0x2000)\n";
    std::cout << "     • Each Impl scattered randomly in heap\n";
    std::cout << "     • CPU prefetcher cannot help\n";
    std::cout << "   Impact on Real-Time:\n";
    std::cout << "     • Sequential access becomes random access\n";
    std::cout << "     • 10-100x slower than contiguous data\n";
    std::cout << "     • Unacceptable for sensor processing, control loops\n\n";
    
    std::cout << "❌ VIOLATION 4: HEAP FRAGMENTATION OVER TIME\n";
    std::cout << "   Problem:\n";
    std::cout << "     • Repeated allocation/deallocation of Impl objects\n";
    std::cout << "     • Heap becomes fragmented (Swiss cheese memory)\n";
    std::cout << "     • Allocation time increases over time\n";
    std::cout << "   Impact on Real-Time:\n";
    std::cout << "     • System degrades over runtime\n";
    std::cout << "     • May fail after hours/days of operation\n";
    std::cout << "     • Cannot certify for long-running systems\n";
    std::cout << "     • Requires periodic restart (unacceptable)\n\n";
    
    std::cout << "❌ VIOLATION 5: DOUBLE DEALLOCATION (shared_ptr Pimpl)\n";
    std::cout << "   Problem:\n";
    std::cout << "     • shared_ptr has control block (heap allocation)\n";
    std::cout << "     • Plus Impl object (another heap allocation)\n";
    std::cout << "     • TWO heap operations per object\n";
    std::cout << "     • Reference counting overhead on every copy\n";
    std::cout << "   Impact on Real-Time:\n";
    std::cout << "     • Doubles the allocation overhead\n";
    std::cout << "     • Atomic operations for ref counting (slow)\n";
    std::cout << "     • Even worse for real-time systems\n\n";
    
    std::cout << "REAL-WORLD EXAMPLE:\n\n";
    std::cout << "Scenario: Automotive ECU processing 100 sensor values at 1kHz\n\n";
    
    std::cout << "WITHOUT Pimpl:\n";
    std::cout << "  • struct Sensor { int id; double value; };\n";
    std::cout << "  • std::array<Sensor, 100> sensors;  // Contiguous, no heap\n";
    std::cout << "  • for (auto& s : sensors) process(s.value);  // Cache-friendly\n";
    std::cout << "  • Total time: ~500 cycles (all in cache)\n";
    std::cout << "  • Deterministic: Always 500 ± 10 cycles\n";
    std::cout << "  ✅ PASSES WCET analysis\n\n";
    
    std::cout << "WITH Pimpl:\n";
    std::cout << "  • class Sensor { unique_ptr<Impl> pImpl_; };\n";
    std::cout << "  • std::array<Sensor, 100> sensors;  // 100 heap allocations!\n";
    std::cout << "  • for (auto& s : sensors) process(s->value);  // Cache misses\n";
    std::cout << "  • Total time: ~5000-50000 cycles (cache misses)\n";
    std::cout << "  • Non-deterministic: 5000-50000 cycles (10x variation!)\n";
    std::cout << "  ❌ FAILS WCET analysis (unpredictable)\n\n";
    
    std::cout << "STANDARDS VIOLATIONS:\n\n";
    std::cout << "  • MISRA C++ Rule 18-4-1: Dynamic heap allocation shall not be used\n";
    std::cout << "  • AUTOSAR A18-5-1: malloc/calloc/realloc/free shall not be used\n";
    std::cout << "  • ISO 26262 (ASIL-D): Static allocation required\n";
    std::cout << "  • DO-178C Level A: Dynamic allocation discouraged\n";
    std::cout << "  • JSF AV C++ Rule 207: new/delete not used after initialization\n\n";
    
    std::cout << "VERDICT: ❌ PIMPL IS FORBIDDEN IN REAL-TIME SYSTEMS\n";
}

} // namespace realtime_problems

// ===================================================================
// SECTION 5: ALTERNATIVES AND MITIGATIONS
// ===================================================================

namespace alternatives {

// ==============================
// ALTERNATIVE 1: NO PIMPL - Expose implementation (if acceptable)
// ==============================

class DirectImplementation {
private:
    int id_;
    double value_;
    
public:
    DirectImplementation(int id, double value) : id_(id), value_(value) {}
    
    void process() { value_ *= 2.0; }
    double getValue() const { return value_; }
};

// ✅ Pros: Fast, cache-friendly, deterministic
// ❌ Cons: Breaks ABI on changes, longer compile times

// ==============================
// ALTERNATIVE 2: FAST PIMPL - Pre-allocated storage
// ==============================

template<typename T, size_t Size, size_t Alignment = alignof(std::max_align_t)>
class FastPimpl {
private:
    alignas(Alignment) std::byte storage_[Size];
    
    T* ptr() { return reinterpret_cast<T*>(storage_); }
    const T* ptr() const { return reinterpret_cast<const T*>(storage_); }
    
public:
    template<typename... Args>
    FastPimpl(Args&&... args) {
        static_assert(sizeof(T) <= Size, "Storage too small");
        static_assert(alignof(T) <= Alignment, "Alignment too small");
        new (storage_) T(std::forward<Args>(args)...);
    }
    
    ~FastPimpl() { ptr()->~T(); }
    
    // Delete copy/move (or implement if needed)
    FastPimpl(const FastPimpl&) = delete;
    FastPimpl& operator=(const FastPimpl&) = delete;
    
    T* operator->() { return ptr(); }
    const T* operator->() const { return ptr(); }
    
    T& operator*() { return *ptr(); }
    const T& operator*() const { return *ptr(); }
};

class SensorFastPimpl {
private:
    struct Impl {
        int id;
        double value;
        Impl(int i, double v) : id(i), value(v) {}
        void process() { value *= 2.0; }
    };
    
    FastPimpl<Impl, 32> pImpl_;  // Storage in object itself!
    
public:
    SensorFastPimpl(int id, double value) : pImpl_(id, value) {}
    
    void process() { pImpl_->process(); }
    double getValue() const { return pImpl_->value; }
};

// ✅ Pros: NO heap allocation, cache-friendly, ABI stable (if size doesn't change)
// ⚠️  Cons: Must know maximum Impl size at compile time

// ==============================
// ALTERNATIVE 3: Type Erasure (std::function style)
// ==============================

class TypeErasedSensor {
private:
    struct ConceptBase {
        virtual ~ConceptBase() = default;
        virtual void process() = 0;
        virtual double getValue() const = 0;
        virtual std::unique_ptr<ConceptBase> clone() const = 0;
    };
    
    template<typename T>
    struct ConceptModel : ConceptBase {
        T data;
        
        ConceptModel(T d) : data(std::move(d)) {}
        
        void process() override { data.process(); }
        double getValue() const override { return data.getValue(); }
        std::unique_ptr<ConceptBase> clone() const override {
            return std::make_unique<ConceptModel<T>>(data);
        }
    };
    
    std::unique_ptr<ConceptBase> pImpl_;
    
public:
    template<typename T>
    TypeErasedSensor(T sensor) 
        : pImpl_(std::make_unique<ConceptModel<T>>(std::move(sensor))) {}
    
    void process() { pImpl_->process(); }
    double getValue() const { return pImpl_->getValue(); }
};

// ✅ Pros: Flexible, any type works, no template in interface
// ❌ Cons: Still uses heap, virtual function overhead

// ==============================
// ALTERNATIVE 4: SBO (Small Buffer Optimization) Pimpl
// ==============================

template<size_t BufferSize = 64>
class SBOPimpl {
private:
    alignas(std::max_align_t) std::byte buffer_[BufferSize];
    void* heap_ptr_ = nullptr;
    
    void* storage() { return heap_ptr_ ? heap_ptr_ : buffer_; }
    const void* storage() const { return heap_ptr_ ? heap_ptr_ : buffer_; }
    
public:
    template<typename T, typename... Args>
    void emplace(Args&&... args) {
        if (sizeof(T) <= BufferSize && alignof(T) <= alignof(std::max_align_t)) {
            // Small: use buffer (no heap)
            new (buffer_) T(std::forward<Args>(args)...);
            heap_ptr_ = nullptr;
        } else {
            // Large: use heap
            heap_ptr_ = new T(std::forward<Args>(args)...);
        }
    }
    
    template<typename T>
    T* get() { return static_cast<T*>(storage()); }
    
    template<typename T>
    const T* get() const { return static_cast<const T*>(storage()); }
    
    ~SBOPimpl() {
        if (heap_ptr_) {
            // Must know type to delete - requires type erasure
        }
    }
};

// ✅ Pros: Avoids heap for small objects
// ⚠️  Cons: Complex, requires type erasure for destruction

// ==============================
// ALTERNATIVE 5: Interface Segregation + Direct Storage
// ==============================

// Pure interface (header file)
class ISensor {
public:
    virtual ~ISensor() = default;
    virtual void process() = 0;
    virtual double getValue() const = 0;
};

// Concrete implementation (cpp file)
class ConcreteSensor : public ISensor {
private:
    int id_;
    double value_;
    
public:
    ConcreteSensor(int id, double value) : id_(id), value_(value) {}
    
    void process() override { value_ *= 2.0; }
    double getValue() const override { return value_; }
};

// Factory function (cpp file)
std::unique_ptr<ISensor> createSensor(int id, double value) {
    return std::make_unique<ConcreteSensor>(id, value);
}

// ✅ Pros: Clean interface, implementation hidden
// ❌ Cons: Still uses heap, virtual function overhead

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 5: ALTERNATIVES AND MITIGATIONS\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "ALTERNATIVE 1: NO PIMPL (Direct Implementation)\n";
    std::cout << "  ✅ BEST for performance and real-time systems\n";
    std::cout << "  ✅ Zero overhead, cache-friendly\n";
    std::cout << "  ❌ Breaks ABI on changes\n";
    std::cout << "  ❌ Longer compile times\n";
    std::cout << "  USE WHEN: Performance critical, internal classes\n\n";
    
    DirectImplementation direct(1, 100.0);
    direct.process();
    std::cout << "  Example: Direct value = " << direct.getValue() << "\n\n";
    
    std::cout << "ALTERNATIVE 2: FAST PIMPL (Pre-allocated storage)\n";
    std::cout << "  ✅ NO heap allocation (storage in object)\n";
    std::cout << "  ✅ Cache-friendly (contiguous memory)\n";
    std::cout << "  ✅ ABI stable (if size doesn't change)\n";
    std::cout << "  ⚠️  Must know maximum Impl size\n";
    std::cout << "  USE WHEN: Need ABI stability + performance\n\n";
    
    SensorFastPimpl fast(1, 100.0);
    fast.process();
    std::cout << "  Example: FastPimpl value = " << fast.getValue() << "\n\n";
    
    std::cout << "ALTERNATIVE 3: TYPE ERASURE\n";
    std::cout << "  ✅ Flexible (any type works)\n";
    std::cout << "  ✅ Clean generic interface\n";
    std::cout << "  ❌ Still uses heap\n";
    std::cout << "  ❌ Virtual function overhead\n";
    std::cout << "  USE WHEN: Need runtime polymorphism\n\n";
    
    std::cout << "ALTERNATIVE 4: SBO (Small Buffer Optimization)\n";
    std::cout << "  ✅ Avoids heap for small objects\n";
    std::cout << "  ✅ Fallback to heap for large objects\n";
    std::cout << "  ❌ Complex implementation\n";
    std::cout << "  ❌ Similar to std::string SSO\n";
    std::cout << "  USE WHEN: Mix of small and large objects\n\n";
    
    std::cout << "ALTERNATIVE 5: INTERFACE + FACTORY\n";
    std::cout << "  ✅ Clean separation of interface/implementation\n";
    std::cout << "  ✅ Implementation hidden in .cpp\n";
    std::cout << "  ❌ Still uses heap\n";
    std::cout << "  ❌ Virtual function overhead\n";
    std::cout << "  USE WHEN: Need polymorphism + information hiding\n\n";
    
    std::cout << "MITIGATION STRATEGIES FOR REAL-TIME:\n\n";
    
    std::cout << "1. INITIALIZATION PHASE PATTERN\n";
    std::cout << "   • Allocate ALL Pimpl objects during initialization\n";
    std::cout << "   • Use object pools (pre-allocated)\n";
    std::cout << "   • Never allocate/deallocate during runtime\n";
    std::cout << "   • Acceptable: init phase not time-critical\n\n";
    
    std::cout << "2. CUSTOM POOL ALLOCATOR\n";
    std::cout << "   • Pre-allocate pool of Impl objects\n";
    std::cout << "   • Custom allocator for unique_ptr<Impl>\n";
    std::cout << "   • Bounded, deterministic allocation\n";
    std::cout << "   • Improves cache locality (pool in contiguous memory)\n\n";
    
    std::cout << "3. PLACEMENT NEW IN FIXED BUFFER\n";
    std::cout << "   • Static buffer for all Impl objects\n";
    std::cout << "   • Placement new to construct in buffer\n";
    std::cout << "   • Zero heap allocation\n";
    std::cout << "   • Requires careful lifetime management\n\n";
    
    std::cout << "4. C++20 MODULES\n";
    std::cout << "   • Reduce need for Pimpl (compilation firewall)\n";
    std::cout << "   • Faster builds without Pimpl overhead\n";
    std::cout << "   • Still evolving, limited tooling support\n\n";
    
    std::cout << "RECOMMENDATION FOR REAL-TIME SYSTEMS:\n\n";
    std::cout << "  ✅ FIRST CHOICE: No Pimpl (Direct implementation)\n";
    std::cout << "     • Best performance\n";
    std::cout << "     • Deterministic\n";
    std::cout << "     • Accept longer compile times as trade-off\n\n";
    
    std::cout << "  ✅ SECOND CHOICE: FastPimpl (in-object storage)\n";
    std::cout << "     • Good compromise\n";
    std::cout << "     • No heap, ABI stable\n";
    std::cout << "     • Requires careful sizing\n\n";
    
    std::cout << "  ⚠️  THIRD CHOICE: Pimpl with pool allocator\n";
    std::cout << "     • Acceptable with pre-allocation\n";
    std::cout << "     • Only during initialization phase\n";
    std::cout << "     • Requires justification for certification\n\n";
    
    std::cout << "  ❌ NEVER: Traditional heap-based Pimpl at runtime\n";
    std::cout << "     • Non-deterministic\n";
    std::cout << "     • Violates real-time standards\n";
    std::cout << "     • Unacceptable for safety-critical systems\n";
}

} // namespace alternatives

// ===================================================================
// SECTION 6: DECISION GUIDE
// ===================================================================

namespace decision_guide {

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 6: WHEN TO USE PIMPL - DECISION GUIDE\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "DECISION TREE:\n\n";
    
    std::cout << "┌─ Q1: Is this a real-time or safety-critical system?\n";
    std::cout << "│\n";
    std::cout << "├─ YES → ❌ AVOID PIMPL (use direct implementation or FastPimpl)\n";
    std::cout << "│         Reason: Non-deterministic, cache-hostile, heap allocation\n";
    std::cout << "│\n";
    std::cout << "└─ NO → Continue to Q2\n\n";
    
    std::cout << "┌─ Q2: Is this a public API/library interface?\n";
    std::cout << "│\n";
    std::cout << "├─ YES → Continue to Q3\n";
    std::cout << "│\n";
    std::cout << "└─ NO → ❌ DON'T USE PIMPL (internal class - direct impl)\n\n";
    
    std::cout << "┌─ Q3: Do you need ABI (binary) stability?\n";
    std::cout << "│\n";
    std::cout << "├─ YES → Continue to Q4\n";
    std::cout << "│\n";
    std::cout << "└─ NO → ❌ DON'T USE PIMPL (recompilation acceptable)\n\n";
    
    std::cout << "┌─ Q4: Is the class performance-critical (hot path)?\n";
    std::cout << "│\n";
    std::cout << "├─ YES → ⚠️  RECONSIDER Pimpl (profile first, consider FastPimpl)\n";
    std::cout << "│\n";
    std::cout << "└─ NO → Continue to Q5\n\n";
    
    std::cout << "┌─ Q5: Is the implementation complex/large?\n";
    std::cout << "│\n";
    std::cout << "├─ YES → ✅ USE PIMPL (good candidate)\n";
    std::cout << "│\n";
    std::cout << "└─ NO → ⚠️  Probably not worth it (overhead > benefit)\n\n";
    
    std::cout << std::string(80, '-') << "\n\n";
    
    std::cout << "COMPARISON TABLE:\n\n";
    std::cout << "┌────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ CRITERION          │ NO PIMPL │ PIMPL  │ FAST PIMPL │ REAL-TIME OK? │\n";
    std::cout << "├────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Performance        │ BEST     │ POOR   │ GOOD       │ ✅ / ✅ / ❌    │\n";
    std::cout << "│ Cache Locality     │ BEST     │ WORST  │ BEST       │ ✅ / ✅ / ❌    │\n";
    std::cout << "│ Heap Allocation    │ NO       │ YES    │ NO         │ ✅ / ✅ / ❌    │\n";
    std::cout << "│ ABI Stability      │ NO       │ YES    │ PARTIAL    │ N/A           │\n";
    std::cout << "│ Compile Time       │ SLOW     │ FAST   │ FAST       │ N/A           │\n";
    std::cout << "│ Information Hiding │ NO       │ YES    │ YES        │ N/A           │\n";
    std::cout << "│ Complexity         │ LOW      │ MEDIUM │ HIGH       │ N/A           │\n";
    std::cout << "│ Memory Overhead    │ 0 bytes  │ 8 bytes│ Impl size  │ N/A           │\n";
    std::cout << "└────────────────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "USE PIMPL WHEN:\n\n";
    std::cout << "  ✅ Shipping shared libraries (ABI stability critical)\n";
    std::cout << "     Examples: Qt, wxWidgets, system libraries\n\n";
    
    std::cout << "  ✅ Plugin architectures (binary compatibility)\n";
    std::cout << "     Examples: Audio plugins, game engines\n\n";
    
    std::cout << "  ✅ Large complex implementations (compilation firewall)\n";
    std::cout << "     Examples: GUI frameworks, network libraries\n\n";
    
    std::cout << "  ✅ Platform-specific code (hide platform details)\n";
    std::cout << "     Examples: Cross-platform libraries\n\n";
    
    std::cout << "AVOID PIMPL WHEN:\n\n";
    std::cout << "  ❌ Real-time systems (non-deterministic)\n";
    std::cout << "     Examples: Control systems, DSP, robotics\n\n";
    
    std::cout << "  ❌ Performance-critical code (cache-hostile)\n";
    std::cout << "     Examples: Game engines, HFT, rendering\n\n";
    
    std::cout << "  ❌ Safety-critical systems (heap allocation)\n";
    std::cout << "     Examples: Automotive (ISO 26262), avionics (DO-178C)\n\n";
    
    std::cout << "  ❌ Small simple classes (overhead > benefit)\n";
    std::cout << "     Examples: POD types, value types\n\n";
    
    std::cout << "  ❌ Internal implementation classes (not exposed)\n";
    std::cout << "     Examples: Private helpers, implementation details\n\n";
    
    std::cout << "  ❌ Header-only libraries (templates)\n";
    std::cout << "     Examples: Template libraries, generic code\n\n";
    
    std::cout << "  ❌ Using C++20 modules (reduces need)\n";
    std::cout << "     Examples: Modern codebases with module support\n\n";
    
    std::cout << "FAMOUS LIBRARIES USING PIMPL:\n\n";
    std::cout << "  • Qt Framework - Extensive use for ABI stability\n";
    std::cout << "  • Boost.Asio - Some classes use Pimpl\n";
    std::cout << "  • Windows API - HWND, HANDLE (opaque pointers)\n";
    std::cout << "  • OpenSSL - Internal structures hidden\n";
    std::cout << "  • wxWidgets - Cross-platform abstraction\n\n";
    
    std::cout << "GOLDEN RULES:\n\n";
    std::cout << "  1. Pimpl is a DESIGN PATTERN, not a default choice\n";
    std::cout << "  2. Only use when benefits outweigh performance cost\n";
    std::cout << "  3. Profile before and after - measure the impact\n";
    std::cout << "  4. Consider FastPimpl for real-time constraints\n";
    std::cout << "  5. Never use traditional Pimpl in real-time code\n";
    std::cout << "  6. C++20 modules may reduce need for Pimpl\n";
}

} // namespace decision_guide

// ===================================================================
// MAIN
// ===================================================================

int main() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "PIMPL IDIOM IN MODERN C++ - STILL RELEVANT?\n";
    std::cout << "Performance Impact on Real-Time Systems\n";
    std::cout << std::string(80, '=') << "\n";
    
    what_is_pimpl::demonstrate();
    modern_cpp_pimpl::demonstrate();
    performance_problems::demonstrate();
    realtime_problems::demonstrate();
    alternatives::demonstrate();
    decision_guide::demonstrate();
    
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "FINAL VERDICT\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "IS PIMPL STILL RELEVANT IN MODERN C++?\n\n";
    
    std::cout << "✅ YES, for:\n";
    std::cout << "   • Shared libraries requiring ABI stability\n";
    std::cout << "   • Large projects needing compilation firewalls\n";
    std::cout << "   • Public APIs with evolving implementation\n";
    std::cout << "   • Modern C++ makes it easier (unique_ptr, shared_ptr)\n\n";
    
    std::cout << "❌ NO (AVOID), for:\n";
    std::cout << "   • Real-time systems - NON-DETERMINISTIC\n";
    std::cout << "   • Safety-critical systems - HEAP ALLOCATION\n";
    std::cout << "   • Performance-critical code - CACHE-HOSTILE\n";
    std::cout << "   • Simple classes - OVERHEAD > BENEFIT\n\n";
    
    std::cout << "WHY PIMPL IS BAD FOR REAL-TIME:\n\n";
    std::cout << "  1. POINTER INDIRECTION - Extra memory access (cache miss)\n";
    std::cout << "  2. DESTROYED CACHE LOCALITY - Scattered Impl objects in heap\n";
    std::cout << "  3. HEAP ALLOCATION - Non-deterministic malloc/free\n";
    std::cout << "  4. MEMORY JUMPS - CPU prefetcher cannot help\n";
    std::cout << "  5. FRAGMENTATION - System degrades over time\n\n";
    
    std::cout << "MITIGATION OPTIONS:\n\n";
    std::cout << "  ✅ FastPimpl (in-object storage) - Zero heap, cache-friendly\n";
    std::cout << "  ✅ Pre-allocated pools - Bounded allocation during init\n";
    std::cout << "  ✅ No Pimpl - Accept longer compile times for performance\n\n";
    
    std::cout << "BOTTOM LINE:\n\n";
    std::cout << "  \"Pimpl is still relevant for APIs and libraries,\n";
    std::cout << "   but should be AVOIDED in real-time and performance-critical code.\n";
    std::cout << "   The pointer indirection and heap allocation make it\n";
    std::cout << "   fundamentally incompatible with deterministic real-time systems.\"\n\n";
    
    std::cout << std::string(80, '=') << "\n\n";
    
    return 0;
}
