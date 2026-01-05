#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <array>
#include <cstring>

// ===================================================================
// WHY AVOID THESE IN EMBEDDED SYSTEMS - DETAILED EXPLANATIONS
// ===================================================================
// Target System Specification (Typical Microcontroller):
// - MCU: ARM Cortex-M4 @ 80MHz
// - Flash: 256KB (code storage)
// - RAM: 64KB (data, stack, heap)
// - Stack: 4KB (limited call depth)
// - Heap: 8KB (if enabled, often disabled)
// - No MMU (no virtual memory, no memory protection)
// - Real-time constraints: 1ms task deadlines
// ===================================================================

// ===================================================================
// 1. PROBLEM: new/delete AND HEAP ALLOCATION
// ===================================================================

void demonstrate_heap_fragmentation() {
    std::cout << "\n=== 1. PROBLEM: new/delete (HEAP FRAGMENTATION) ===" << std::endl;
    
    std::cout << "\n📊 TARGET SYSTEM CONSTRAINTS:" << std::endl;
    std::cout << "  • Heap size: 8KB (8192 bytes)" << std::endl;
    std::cout << "  • No MMU, no memory defragmentation" << std::endl;
    std::cout << "  • Fragmentation = permanent until reboot" << std::endl;
    
    std::cout << "\n❌ BAD CODE (Using new/delete):" << std::endl;
    std::cout << R"(
    // Allocate various sizes over time
    void* ptr1 = new char[100];   // 100 bytes
    void* ptr2 = new char[200];   // 200 bytes
    void* ptr3 = new char[100];   // 100 bytes
    
    delete[] ptr2;  // Free middle block
    
    // Now heap looks like: [100 used][200 FREE][100 used]
    // Try to allocate 250 bytes - FAILS!
    // Even though 200 bytes are free, they're not contiguous
    void* ptr4 = new char[250];  // ❌ Out of memory!
    )" << std::endl;
    
    std::cout << "⚠️ PROBLEMS:" << std::endl;
    std::cout << "  1. Fragmentation: Free memory becomes unusable" << std::endl;
    std::cout << "  2. Non-deterministic: Allocation time varies (0.1µs to 10ms)" << std::endl;
    std::cout << "  3. Leaks: One forgotten delete = permanent memory loss" << std::endl;
    std::cout << "  4. Overhead: 8-16 bytes per allocation for bookkeeping" << std::endl;
    std::cout << "  5. System failure: malloc() returns nullptr, hard to recover" << std::endl;
    
    std::cout << "\n✅ SOLUTION: Memory Pools" << std::endl;
    std::cout << "  • Pre-allocated fixed-size blocks" << std::endl;
    std::cout << "  • O(1) allocation time: always <1µs" << std::endl;
    std::cout << "  • No fragmentation" << std::endl;
    std::cout << "  • Deterministic behavior" << std::endl;
    
    std::cout << "\n💾 MEMORY IMPACT:" << std::endl;
    std::cout << "  • Heap manager overhead: ~1KB" << std::endl;
    std::cout << "  • Per-allocation overhead: 8-16 bytes" << std::endl;
    std::cout << "  • On 64KB RAM: Heap uses 12-15% just for bookkeeping!" << std::endl;
}

// ===================================================================
// 2. PROBLEM: std::vector (DYNAMIC REALLOCATION)
// ===================================================================

void demonstrate_vector_problems() {
    std::cout << "\n=== 2. PROBLEM: std::vector (DYNAMIC REALLOCATION) ===" << std::endl;
    
    std::cout << "\n📊 SCENARIO: Sensor data collection" << std::endl;
    std::cout << "  • Need to store 100 sensor readings" << std::endl;
    std::cout << "  • Each reading: 4 bytes (float)" << std::endl;
    std::cout << "  • Expected memory: 400 bytes" << std::endl;
    
    std::cout << "\n❌ BAD CODE (Using std::vector):" << std::endl;
    std::cout << R"(
    std::vector<float> sensor_data;
    
    for (int i = 0; i < 100; ++i) {
        sensor_data.push_back(read_sensor());  // ❌ Unpredictable!
    }
    )" << std::endl;
    
    std::cout << "⚠️ WHAT ACTUALLY HAPPENS:" << std::endl;
    std::cout << "  Iteration 1: Allocate 4 bytes (capacity=1)" << std::endl;
    std::cout << "  Iteration 2: Reallocate to 8 bytes, copy 1 element" << std::endl;
    std::cout << "  Iteration 3: Reallocate to 16 bytes, copy 2 elements" << std::endl;
    std::cout << "  Iteration 5: Reallocate to 32 bytes, copy 4 elements" << std::endl;
    std::cout << "  Iteration 9: Reallocate to 64 bytes, copy 8 elements" << std::endl;
    std::cout << "  ... (continues with exponential growth)" << std::endl;
    
    std::cout << "\n💣 PROBLEMS:" << std::endl;
    std::cout << "  1. Multiple allocations: ~7 allocations for 100 elements" << std::endl;
    std::cout << "  2. Memory copies: ~100 element copies total" << std::endl;
    std::cout << "  3. Peak memory: Needs 2x during reallocation" << std::endl;
    std::cout << "     (800 bytes instead of 400 bytes!)" << std::endl;
    std::cout << "  4. Heap fragmentation: Old blocks left behind" << std::endl;
    std::cout << "  5. Timing: Each push_back() takes different time" << std::endl;
    std::cout << "     - Normal: 0.1µs" << std::endl;
    std::cout << "     - Reallocation: 50µs (500x slower!)" << std::endl;
    
    std::cout << "\n✅ SOLUTION: std::array or fixed buffer" << std::endl;
    std::cout << R"(
    std::array<float, 100> sensor_data;  // Fixed size, stack allocated
    size_t count = 0;
    
    for (int i = 0; i < 100; ++i) {
        sensor_data[count++] = read_sensor();  // Constant time!
    }
    )" << std::endl;
    
    std::cout << "\n💾 MEMORY COMPARISON:" << std::endl;
    std::cout << "  std::vector: 400 bytes + 24 bytes overhead + heap fragmentation" << std::endl;
    std::cout << "  std::array:  400 bytes (stack) + 0 bytes overhead" << std::endl;
}

// ===================================================================
// 3. PROBLEM: std::string (DYNAMIC ALLOCATION)
// ===================================================================

void demonstrate_string_problems() {
    std::cout << "\n=== 3. PROBLEM: std::string (DYNAMIC ALLOCATION) ===" << std::endl;
    
    std::cout << "\n📊 SCENARIO: UART message building" << std::endl;
    std::cout << "  • Need message: \"TEMP:25.5C\\r\\n\"" << std::endl;
    std::cout << "  • Length: 13 characters" << std::endl;
    
    std::cout << "\n❌ BAD CODE (Using std::string):" << std::endl;
    std::cout << R"(
    std::string msg = "TEMP:";      // Allocation 1
    msg += std::to_string(25.5);    // Allocation 2 + reallocation
    msg += "C\r\n";                 // Possible reallocation 3
    
    send_uart(msg.c_str());
    )" << std::endl;
    
    std::cout << "⚠️ PROBLEMS:" << std::endl;
    std::cout << "  1. Multiple allocations: 2-3 heap allocations" << std::endl;
    std::cout << "  2. SSO ambiguity: Small String Optimization" << std::endl;
    std::cout << "     - Strings <16 chars: stored inline (good)" << std::endl;
    std::cout << "     - Strings >=16 chars: heap allocation (bad)" << std::endl;
    std::cout << "     - Behavior changes at runtime!" << std::endl;
    std::cout << "  3. Hidden cost: std::to_string() always allocates" << std::endl;
    std::cout << "  4. Exception throwing: Can throw std::bad_alloc" << std::endl;
    
    std::cout << "\n💾 MEMORY COST:" << std::endl;
    std::cout << "  • std::string object: 24-32 bytes" << std::endl;
    std::cout << "  • String data: 13 bytes" << std::endl;
    std::cout << "  • Heap overhead: 8-16 bytes" << std::endl;
    std::cout << "  • Total: ~45-60 bytes for 13-char string!" << std::endl;
    
    std::cout << "\n✅ SOLUTION: Fixed-size buffer" << std::endl;
    std::cout << R"(
    char msg[32];
    snprintf(msg, sizeof(msg), "TEMP:%.1fC\r\n", 25.5f);
    send_uart(msg);
    
    // Total stack usage: 32 bytes, zero heap allocations
    )" << std::endl;
    
    std::cout << "\n⏱️ TIMING COMPARISON:" << std::endl;
    std::cout << "  std::string version: 15-50µs (unpredictable)" << std::endl;
    std::cout << "  snprintf version:    2-5µs (deterministic)" << std::endl;
}

// ===================================================================
// 4. PROBLEM: VIRTUAL FUNCTIONS (VTABLE OVERHEAD)
// ===================================================================

class BaseSensor {
public:
    virtual ~BaseSensor() = default;
    virtual float read() = 0;
    virtual const char* name() = 0;
};

class TempSensor : public BaseSensor {
public:
    float read() override { return 25.5f; }
    const char* name() override { return "Temperature"; }
};

void demonstrate_virtual_function_overhead() {
    std::cout << "\n=== 4. PROBLEM: VIRTUAL FUNCTIONS (VTABLE) ===" << std::endl;
    
    std::cout << "\n📊 MEMORY OVERHEAD PER OBJECT:" << std::endl;
    std::cout << "  • vtable pointer (vptr): 4 bytes (32-bit) or 8 bytes (64-bit)" << std::endl;
    std::cout << "  • vtable itself: 8 bytes per virtual function" << std::endl;
    std::cout << "  • Small objects become bloated!" << std::endl;
    
    std::cout << "\nExample object sizes:" << std::endl;
    std::cout << "  sizeof(BaseSensor) = " << sizeof(BaseSensor) << " bytes" << std::endl;
    std::cout << "  sizeof(TempSensor) = " << sizeof(TempSensor) << " bytes" << std::endl;
    
    std::cout << "\n⚠️ PROBLEMS:" << std::endl;
    std::cout << "  1. RAM overhead: Every object has vptr (4-8 bytes)" << std::endl;
    std::cout << "     - With 100 sensors: 400-800 bytes wasted!" << std::endl;
    std::cout << "  2. Flash overhead: vtable stored in Flash" << std::endl;
    std::cout << "     - 3 virtual functions = 24 bytes Flash per class" << std::endl;
    std::cout << "  3. Indirection: Function call requires:" << std::endl;
    std::cout << "     a. Load vptr from object (1 memory access)" << std::endl;
    std::cout << "     b. Load function pointer from vtable (2nd memory access)" << std::endl;
    std::cout << "     c. Indirect jump (breaks CPU pipeline)" << std::endl;
    std::cout << "  4. Not inlineable: Compiler can't optimize" << std::endl;
    std::cout << "  5. Cache misses: Vtable and code in different locations" << std::endl;
    
    std::cout << "\n⏱️ PERFORMANCE IMPACT:" << std::endl;
    std::cout << "  Direct call:   1-2 CPU cycles" << std::endl;
    std::cout << "  Virtual call:  10-20 CPU cycles (10x slower!)" << std::endl;
    std::cout << "  • At 80MHz: Virtual call = 125-250ns overhead" << std::endl;
    std::cout << "  • In 1ms task: Can fit 4000 direct calls or 400 virtual calls" << std::endl;
    
    std::cout << "\n✅ SOLUTION: Static polymorphism (CRTP or templates)" << std::endl;
    std::cout << R"(
    template<typename Derived>
    class SensorBase {
        float read() { return static_cast<Derived*>(this)->read_impl(); }
    };
    
    // Zero overhead, fully inlineable, no vtable!
    )" << std::endl;
}

// ===================================================================
// 5. PROBLEM: RTTI (Runtime Type Information)
// ===================================================================

void demonstrate_rtti_overhead() {
    std::cout << "\n=== 5. PROBLEM: RTTI (typeid, dynamic_cast) ===" << std::endl;
    
    std::cout << "\n📊 OVERHEAD:" << std::endl;
    std::cout << "  • Type info in Flash: 20-50 bytes per polymorphic class" << std::endl;
    std::cout << "  • Name strings: Variable length (stored in Flash)" << std::endl;
    std::cout << "  • Dynamic_cast: String comparison overhead" << std::endl;
    
    std::cout << "\n❌ BAD CODE:" << std::endl;
    std::cout << R"(
    BaseSensor* sensor = get_sensor();
    
    // dynamic_cast requires RTTI
    if (TempSensor* temp = dynamic_cast<TempSensor*>(sensor)) {
        // Process temperature sensor
    }
    )" << std::endl;
    
    std::cout << "\n⚠️ PROBLEMS:" << std::endl;
    std::cout << "  1. Flash overhead: RTTI adds 5-10KB to binary size" << std::endl;
    std::cout << "     - On 256KB Flash: 2-4% wasted!" << std::endl;
    std::cout << "  2. RAM overhead: Type info structures in RAM" << std::endl;
    std::cout << "  3. CPU overhead: String comparisons for casts" << std::endl;
    std::cout << "  4. Complexity: Name mangling, RTTI tables" << std::endl;
    
    std::cout << "\n⏱️ TIMING:" << std::endl;
    std::cout << "  static_cast<>:   0 cycles (compile-time)" << std::endl;
    std::cout << "  dynamic_cast<>:  100-500 cycles (runtime check!)" << std::endl;
    
    std::cout << "\n✅ SOLUTION: std::variant or manual type tags" << std::endl;
    std::cout << R"(
    enum class SensorType { Temperature, Humidity };
    
    struct Sensor {
        SensorType type;
        float value;
    };
    
    // Check type with simple enum comparison (1 cycle)
    if (sensor.type == SensorType::Temperature) { ... }
    )" << std::endl;
    
    std::cout << "\n🔧 GCC FLAG:" << std::endl;
    std::cout << "  -fno-rtti  (disables RTTI, saves 5-10KB)" << std::endl;
}

// ===================================================================
// 6. PROBLEM: EXCEPTIONS
// ===================================================================

void demonstrate_exception_overhead() {
    std::cout << "\n=== 6. PROBLEM: EXCEPTIONS ===" << std::endl;
    
    std::cout << "\n📊 OVERHEAD:" << std::endl;
    std::cout << "  • Exception tables in Flash: 10-30KB" << std::endl;
    std::cout << "  • Unwinding code: 5-10KB" << std::endl;
    std::cout << "  • Per-function overhead: Extra instructions for cleanup" << std::endl;
    std::cout << "  • Total: 15-40KB on 256KB Flash (6-15%!)" << std::endl;
    
    std::cout << "\n❌ BAD CODE:" << std::endl;
    std::cout << R"(
    try {
        sensor_data.push_back(value);  // May throw std::bad_alloc
        config.parse(json_string);      // May throw parse_error
        send_network(data);             // May throw network_error
    } catch (const std::exception& e) {
        log_error(e.what());
    }
    )" << std::endl;
    
    std::cout << "\n⚠️ PROBLEMS:" << std::endl;
    std::cout << "  1. Code bloat: Exception tables for every function" << std::endl;
    std::cout << "  2. Unpredictable: Stack unwinding takes unknown time" << std::endl;
    std::cout << "     - Normal: 0ns (no exception)" << std::endl;
    std::cout << "     - Exception: 10-1000µs (milliseconds!)" << std::endl;
    std::cout << "  3. RAM for stack: Need extra stack for unwinding" << std::endl;
    std::cout << "  4. Real-time unsafe: Can't use in ISRs" << std::endl;
    std::cout << "  5. Hidden control flow: Function can exit anywhere" << std::endl;
    
    std::cout << "\n💣 REAL-TIME VIOLATION:" << std::endl;
    std::cout << "  Task deadline: 1ms" << std::endl;
    std::cout << "  Exception thrown: 500µs to unwind" << std::endl;
    std::cout << "  Result: DEADLINE MISSED! System fails!" << std::endl;
    
    std::cout << "\n✅ SOLUTION: Error codes or std::optional" << std::endl;
    std::cout << R"(
    enum class Error { None, OutOfMemory, ParseError, NetworkError };
    
    Error result = sensor_data.add(value);
    if (result != Error::None) {
        handle_error(result);  // Explicit, predictable
    }
    
    // Or use std::optional (C++17)
    std::optional<float> maybe_value = read_sensor();
    if (maybe_value) {
        process(*maybe_value);
    }
    )" << std::endl;
    
    std::cout << "\n🔧 GCC FLAG:" << std::endl;
    std::cout << "  -fno-exceptions  (disables exceptions, saves 15-40KB)" << std::endl;
}

// ===================================================================
// 7. PROBLEM: iostream (CODE BLOAT)
// ===================================================================

void demonstrate_iostream_bloat() {
    std::cout << "\n=== 7. PROBLEM: iostream (CODE BLOAT) ===" << std::endl;
    
    std::cout << "\n📊 BINARY SIZE IMPACT:" << std::endl;
    
    std::cout << "\n❌ BAD CODE:" << std::endl;
    std::cout << R"(
    #include <iostream>
    
    int main() {
        std::cout << "Temperature: " << 25.5 << "C" << std::endl;
        return 0;
    }
    )" << std::endl;
    
    std::cout << "\n⚠️ BINARY SIZE:" << std::endl;
    std::cout << "  Without iostream:  2-5KB" << std::endl;
    std::cout << "  With iostream:     60-150KB (!)" << std::endl;
    std::cout << "  Increase: 30-60x larger!" << std::endl;
    
    std::cout << "\n💣 WHAT'S INCLUDED:" << std::endl;
    std::cout << "  • Locale support: 20-40KB" << std::endl;
    std::cout << "  • Formatting code: 15-30KB" << std::endl;
    std::cout << "  • Stream state management: 10-20KB" << std::endl;
    std::cout << "  • Virtual functions (std::ios_base): vtables" << std::endl;
    std::cout << "  • Static initialization: Global constructors" << std::endl;
    
    std::cout << "\n✅ SOLUTION: printf/snprintf" << std::endl;
    std::cout << R"(
    #include <stdio.h>
    
    int main() {
        printf("Temperature: %.1fC\n", 25.5);
        return 0;
    }
    
    // Binary size: 5-10KB (10x smaller!)
    )" << std::endl;
    
    std::cout << "\n📊 COMPARISON (ARM Cortex-M4):" << std::endl;
    std::cout << "  iostream:  80KB Flash + 2KB RAM" << std::endl;
    std::cout << "  printf:    8KB Flash + 512 bytes RAM" << std::endl;
    std::cout << "  snprintf:  6KB Flash + 0 bytes heap" << std::endl;
    
    std::cout << "\n💾 ON 256KB FLASH:" << std::endl;
    std::cout << "  • iostream uses 31% of Flash!" << std::endl;
    std::cout << "  • printf uses 3% of Flash" << std::endl;
}

// ===================================================================
// 8. PROBLEM: RECURSION (STACK OVERFLOW)
// ===================================================================

uint32_t factorial_recursive(uint32_t n) {
    if (n <= 1) return 1;
    return n * factorial_recursive(n - 1);  // ❌ Stack grows
}

uint32_t factorial_iterative(uint32_t n) {
    uint32_t result = 1;
    for (uint32_t i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;  // ✅ Constant stack
}

void demonstrate_recursion_danger() {
    std::cout << "\n=== 8. PROBLEM: RECURSION (STACK OVERFLOW) ===" << std::endl;
    
    std::cout << "\n📊 STACK CONSTRAINTS:" << std::endl;
    std::cout << "  • Total stack: 4KB (4096 bytes)" << std::endl;
    std::cout << "  • Stack frame: ~32 bytes per function call" << std::endl;
    std::cout << "  • Maximum depth: ~128 calls" << std::endl;
    std::cout << "  • NO PROTECTION: Stack overflow = silent corruption!" << std::endl;
    
    std::cout << "\n❌ BAD CODE (Recursive factorial):" << std::endl;
    std::cout << R"(
    uint32_t factorial(uint32_t n) {
        if (n <= 1) return 1;
        return n * factorial(n - 1);  // Each call uses stack
    }
    
    // factorial(10)  = 10 stack frames = 320 bytes ✓ OK
    // factorial(100) = 100 stack frames = 3200 bytes ✓ Close!
    // factorial(150) = 150 stack frames = 4800 bytes ❌ OVERFLOW!
    )" << std::endl;
    
    std::cout << "\n💣 WHAT HAPPENS ON OVERFLOW:" << std::endl;
    std::cout << "  1. Stack grows downward into heap" << std::endl;
    std::cout << "  2. Corrupts heap data structures" << std::endl;
    std::cout << "  3. Corrupts global variables" << std::endl;
    std::cout << "  4. System crashes (hard fault)" << std::endl;
    std::cout << "  5. NO WARNING! Silent death!" << std::endl;
    
    std::cout << "\n⚠️ REAL EXAMPLE: JSON Parser" << std::endl;
    std::cout << R"(
    void parse_json(JsonNode* node) {
        if (node->type == Object) {
            for (auto& child : node->children) {
                parse_json(child);  // ❌ Recursion!
            }
        }
    }
    
    // Deeply nested JSON (100 levels) = STACK OVERFLOW!
    )" << std::endl;
    
    std::cout << "\n✅ SOLUTION: Iterative with explicit stack" << std::endl;
    std::cout << R"(
    uint32_t factorial(uint32_t n) {
        uint32_t result = 1;
        for (uint32_t i = 2; i <= n; ++i) {
            result *= i;
        }
        return result;  // Constant 8 bytes stack
    }
    )" << std::endl;
    
    std::cout << "\n📊 STACK USAGE COMPARISON:" << std::endl;
    std::cout << "  Recursive:  32 bytes × depth (unbounded)" << std::endl;
    std::cout << "  Iterative:  8-16 bytes (constant)" << std::endl;
    
    std::cout << "\nTest with n=10:" << std::endl;
    std::cout << "  factorial_recursive(10) = " << factorial_recursive(10) << std::endl;
    std::cout << "  factorial_iterative(10) = " << factorial_iterative(10) << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  WHY AVOID THESE IN EMBEDDED SYSTEMS" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n🎯 TARGET SYSTEM SPECIFICATION:" << std::endl;
    std::cout << "  MCU: ARM Cortex-M4F @ 80MHz" << std::endl;
    std::cout << "  Flash: 256KB (code + constants)" << std::endl;
    std::cout << "  RAM: 64KB (data + stack + heap)" << std::endl;
    std::cout << "    - Stack: 4KB (main task)" << std::endl;
    std::cout << "    - Heap: 8KB (if enabled)" << std::endl;
    std::cout << "    - .bss/.data: ~52KB available" << std::endl;
    std::cout << "  No MMU (no virtual memory)" << std::endl;
    std::cout << "  No OS (bare metal or lightweight RTOS)" << std::endl;
    std::cout << "  Real-time: 1ms task deadlines" << std::endl;
    std::cout << "  Power: Battery powered (sleep modes important)" << std::endl;
    
    demonstrate_heap_fragmentation();
    demonstrate_vector_problems();
    demonstrate_string_problems();
    demonstrate_virtual_function_overhead();
    demonstrate_rtti_overhead();
    demonstrate_exception_overhead();
    demonstrate_iostream_bloat();
    demonstrate_recursion_danger();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  SUMMARY: RESOURCE IMPACT" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n📊 FLASH USAGE (256KB total):" << std::endl;
    std::cout << "  ┌──────────────────────────────────────┐" << std::endl;
    std::cout << "  │ iostream:       80KB (31%)           │ ❌" << std::endl;
    std::cout << "  │ Exceptions:     30KB (12%)           │ ❌" << std::endl;
    std::cout << "  │ RTTI:           8KB  (3%)            │ ❌" << std::endl;
    std::cout << "  │ Virtual tables: 5KB  (2%)            │ ⚠️" << std::endl;
    std::cout << "  │ TOTAL WASTE:    123KB (48%)          │" << std::endl;
    std::cout << "  ├──────────────────────────────────────┤" << std::endl;
    std::cout << "  │ Your code:      133KB (52%)          │ ✓" << std::endl;
    std::cout << "  └──────────────────────────────────────┘" << std::endl;
    
    std::cout << "\n📊 RAM USAGE (64KB total):" << std::endl;
    std::cout << "  ┌──────────────────────────────────────┐" << std::endl;
    std::cout << "  │ Heap:           8KB  (12.5%)         │ ⚠️" << std::endl;
    std::cout << "  │ Stack:          4KB  (6.25%)         │" << std::endl;
    std::cout << "  │ iostream:       2KB  (3.12%)         │ ❌" << std::endl;
    std::cout << "  │ vtable ptrs:    1KB  (1.56%)         │ ⚠️" << std::endl;
    std::cout << "  ├──────────────────────────────────────┤" << std::endl;
    std::cout << "  │ Your data:      49KB (76.6%)         │ ✓" << std::endl;
    std::cout << "  └──────────────────────────────────────┘" << std::endl;
    
    std::cout << "\n⏱️ REAL-TIME IMPACT:" << std::endl;
    std::cout << "  Task deadline: 1ms = 80,000 CPU cycles" << std::endl;
    std::cout << "\n  Direct function call:  2 cycles   (40,000 calls per ms)" << std::endl;
    std::cout << "  Virtual call:          20 cycles  (4,000 calls per ms)" << std::endl;
    std::cout << "  std::vector::push:     50 cycles  (1,600 calls per ms)" << std::endl;
    std::cout << "  std::vector realloc:   4,000 cycles (20 calls per ms)" << std::endl;
    std::cout << "  Exception throw:       40,000 cycles (2 per ms = DEADLINE MISS!)" << std::endl;
    
    std::cout << "\n💡 BEST PRACTICES FOR EMBEDDED:" << std::endl;
    std::cout << "\n  ✅ USE:" << std::endl;
    std::cout << "    • std::array (fixed-size, stack)" << std::endl;
    std::cout << "    • Memory pools (controlled allocation)" << std::endl;
    std::cout << "    • Static polymorphism (CRTP, templates)" << std::endl;
    std::cout << "    • Error codes, std::optional (no exceptions)" << std::endl;
    std::cout << "    • printf/snprintf (smaller than iostream)" << std::endl;
    std::cout << "    • Iterative algorithms (constant stack)" << std::endl;
    std::cout << "    • constexpr (compile-time computation)" << std::endl;
    std::cout << "    • std::variant (type-safe unions)" << std::endl;
    
    std::cout << "\n  ❌ AVOID:" << std::endl;
    std::cout << "    • new/delete, malloc/free" << std::endl;
    std::cout << "    • std::vector, std::string, std::map" << std::endl;
    std::cout << "    • Virtual functions (use sparingly)" << std::endl;
    std::cout << "    • RTTI (typeid, dynamic_cast)" << std::endl;
    std::cout << "    • Exceptions (try/catch/throw)" << std::endl;
    std::cout << "    • iostream (cout, cin, etc.)" << std::endl;
    std::cout << "    • Recursion (unbounded depth)" << std::endl;
    
    std::cout << "\n🔧 COMPILER FLAGS FOR EMBEDDED:" << std::endl;
    std::cout << "  -fno-exceptions      (saves 15-40KB Flash)" << std::endl;
    std::cout << "  -fno-rtti            (saves 5-10KB Flash)" << std::endl;
    std::cout << "  -fno-threadsafe-statics (saves 2-5KB Flash)" << std::endl;
    std::cout << "  -ffunction-sections  (enables dead code removal)" << std::endl;
    std::cout << "  -fdata-sections      (enables dead data removal)" << std::endl;
    std::cout << "  -Wl,--gc-sections    (linker removes unused code)" << std::endl;
    std::cout << "  -Os                  (optimize for size)" << std::endl;
    
    std::cout << "\n💾 MEMORY SAVINGS SUMMARY:" << std::endl;
    std::cout << "  Disabling features above: ~130KB Flash savings!" << std::endl;
    std::cout << "  From 256KB Flash → 126KB available (49% → 98% usable!)" << std::endl;
    
    std::cout << "\n🔗 RELATED TOPICS:" << std::endl;
    std::cout << "  • See EmbeddedSystemsProgramming.cpp for best practices" << std::endl;
    std::cout << "  • See FunctionalSafetyCppSTL.cpp for:" << std::endl;
    std::cout << "    - STL usage guidelines for STM32 MCUs vs STM32MPs" << std::endl;
    std::cout << "    - Functional safety requirements (IEC-61508 SIL-3)" << std::endl;
    std::cout << "    - Automotive safety standards (ISO 26262 ASIL-D)" << std::endl;
    std::cout << "    - When heap/STL is allowed vs prohibited" << std::endl;
    
    std::cout << "\n⚠️ IMPORTANT NOTES:" << std::endl;
    std::cout << "  • STM32 MCUs (Cortex-M): Very limited resources, avoid dynamic allocation" << std::endl;
    std::cout << "  • STM32MP (Cortex-A with MMU): Full STL support, Linux-like development" << std::endl;
    std::cout << "  • Safety-critical (SIL-3/ASIL-D): Zero dynamic allocation, MISRA/AUTOSAR" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
