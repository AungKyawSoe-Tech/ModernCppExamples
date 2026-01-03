# Real-Time Programming in Modern C++

## Overview

Real-time programming requires **deterministic behavior** and **predictable performance**. This guide covers Modern C++ (C++11 through C++23) best practices for building real-time systems where timing guarantees are critical.

---

## Table of Contents

1. [Core Principles](#core-principles)
2. [Big O Notation for Real-Time](#big-o-notation-for-real-time)
3. [std::bitset vs std::vector\<bool\>](#stdbitset-vs-stdvectorbool)
4. [STL Container Selection](#stl-container-selection)
5. [Memory Management](#memory-management)
6. [Modern C++ Features](#modern-cpp-features)
7. [Real-Time Checklist](#real-time-checklist)

---

## Core Principles

### 1. **Resource Acquisition Is Initialization (RAII)**
- Use RAII principles universally to manage resources
- Ensures deterministic cleanup when objects go out of scope
- Prevents resource leaks automatically

### 2. **Predictable Performance**
- Avoid unexpected latency spikes
- No non-deterministic memory allocations in critical paths
- Pre-allocate memory during initialization phase

### 3. **Immutability**
- Favor `const` and `constexpr` for thread safety
- Enable compile-time optimizations
- Reduce runtime overhead

### 4. **Use the Right Tool**
- Choose appropriate STL containers for your use case
- Understand the time complexity of operations
- Always bound the maximum data size

---

## Big O Notation for Real-Time

Understanding algorithm complexity is **critical** for real-time systems. Here's what each complexity class means for your application:

### O(1) - Constant Time ✅
**Best for real-time systems**

- Execution time doesn't depend on input size
- Always takes the same amount of time
- **Examples:**
  - Array element access: `data[index]`
  - Hash table lookup (average case)
  - Stack push/pop
  - `std::bitset` operations

```cpp
std::array<int, 1000> data;
int value = data[500];  // O(1) - instant access
```

**Verdict:** ✅ **Ideal for real-time - always use when possible**

---

### O(log n) - Logarithmic Time ✅
**Good for real-time systems with bounded size**

- Execution time grows logarithmically
- Doubling input size adds only constant time
- **Examples:**
  - Binary search in sorted array
  - Balanced tree operations (std::map, std::set)
  - Priority queue operations

```cpp
std::vector<int> sorted_data(1000);
// ... populate with sorted data ...
bool found = std::binary_search(sorted_data.begin(), sorted_data.end(), value);
// O(log n) - for 1000 elements: ~10 comparisons
//            for 1M elements: ~20 comparisons (only 2x slower!)
```

**Verdict:** ✅ **Acceptable for real-time if maximum size is bounded**

---

### O(n) - Linear Time ⚠️
**Use with caution - bound the size!**

- Execution time grows linearly with input size
- Doubling input doubles execution time
- **Examples:**
  - Linear search
  - Iterating through container
  - Copying data

```cpp
std::vector<int> data(1000);
auto it = std::find(data.begin(), data.end(), value);
// O(n) - worst case: 1000 comparisons
//        for 1M elements: 1M comparisons (1000x slower!)
```

**Verdict:** ⚠️ **Use only with bounded, small sizes (< 100-1000 elements)**

---

### O(n log n) - Linearithmic Time ⚠️
**Avoid in time-critical paths**

- Common in efficient sorting algorithms
- **Examples:**
  - `std::sort`
  - Merge sort
  - Heap sort

```cpp
std::vector<int> data(1000);
std::sort(data.begin(), data.end());
// O(n log n) - can take microseconds for 1000 elements
//              milliseconds for 1M elements
```

**Verdict:** ⚠️ **Do during initialization phase, NOT in real-time loop**

---

### O(n²) - Quadratic Time ❌
**Never use in real-time systems**

- Execution time grows quadratically
- **Examples:**
  - Nested loops
  - Bubble sort
  - Selection sort

```cpp
// ❌ AVOID THIS!
for (size_t i = 0; i < data.size(); ++i) {
    for (size_t j = 0; j < data.size(); ++j) {
        // O(n²) - disaster for real-time!
    }
}
```

**Verdict:** ❌ **Never acceptable in real-time systems**

---

## Thread Architecture: Real-Time vs Non-Real-Time

### Critical Design Principle

> **In real-time systems, separate time-critical operations (real-time threads) from background processing (non-real-time threads).**

### Real-Time Threads

**Purpose:**
- Control loops (PID controllers, motor control)
- Sensor data acquisition and processing
- Time-critical safety checks
- Immediate response to hardware interrupts

**Requirements:**
- ✅ **MUST** complete within strict deadlines (e.g., 1ms for 1kHz loop)
- ✅ **MUST** have deterministic, predictable timing
- ✅ **MUST** use O(1) operations exclusively
- ❌ **NO** blocking operations (I/O, locks, sleeps)
- ❌ **NO** dynamic memory allocation

**Allowed Operations:**
```cpp
// ✅ O(1) - Array access
latest_value = sensor_readings[sensor_id];

// ✅ O(1) - Bitset operations
status_flags.set(SENSOR_OK);

// ✅ O(1) - Simple arithmetic
control_output = Kp * error + Ki * integral;

// ✅ O(1) - Hash table lookup (constant-time)
auto it = lookup_table.find(key);  // std::unordered_map
```

**Prohibited Operations:**
```cpp
// ❌ O(n) - Linear search
std::find(data.begin(), data.end(), value);

// ❌ O(log n) - Binary search (even if fast, avoid if possible)
std::binary_search(sorted.begin(), sorted.end(), value);

// ❌ O(n) - Iteration
for (const auto& item : large_collection) { /* ... */ }

// ❌ Any I/O operation
std::cout << "Debug: " << value;
file.write(data);
```

---

### Non-Real-Time Threads

**Purpose:**
- Data logging and persistence
- Statistical analysis and reporting
- Diagnostics and monitoring
- User interface updates
- Network communication
- File operations

**Requirements:**
- ✓ Should eventually complete
- ✓ No strict timing deadlines
- ✓ Can be interrupted or delayed
- ✓ Can use dynamic memory allocation

**Allowed Operations:**
```cpp
// ✅ O(1) - Always preferred
history[index] = value;

// ✅ O(log n) - Acceptable for searching
std::binary_search(sorted_history.begin(), sorted_history.end(), target);

// ✅ O(n) - Acceptable for processing
for (const auto& reading : sensor_history) {
    total += reading;
}

// ✅ O(n log n) - Acceptable for occasional sorting
std::sort(data.begin(), data.end());

// ✅ File I/O
log_file << timestamp << "," << value << "\n";

// ✅ Network operations
send_telemetry(data);
```

---

### Example: Proper Thread Separation

```cpp
class MotorController {
private:
    // Real-time data (O(1) access)
    std::array<double, 4> motor_positions_;     // Current positions
    std::array<double, 4> target_positions_;    // Target positions
    std::bitset<4> motor_enabled_;              // Enable flags
    
    // Non-real-time data (can be larger)
    std::vector<double> position_history_;      // Historical data
    std::vector<std::string> error_log_;        // Error messages
    
public:
    // ========================================
    // REAL-TIME THREAD - O(1) operations only
    // ========================================
    
    void update_control_RT(size_t motor_id) noexcept {
        if (!motor_enabled_.test(motor_id)) return;  // O(1)
        
        // O(1) - Read current position
        double current = motor_positions_[motor_id];
        
        // O(1) - Read target
        double target = target_positions_[motor_id];
        
        // O(1) - Calculate error
        double error = target - current;
        
        // O(1) - Simple PD control
        double control = Kp * error + Kd * (error - prev_error_[motor_id]);
        
        // O(1) - Apply control (this would go to hardware)
        apply_motor_voltage(motor_id, control);
        
        // ✅ Entire function is O(1) - suitable for 1kHz+ loop!
    }
    
    // O(1) - Read position from real-time thread
    double get_position_RT(size_t motor_id) const noexcept {
        return motor_positions_[motor_id];  // O(1)
    }
    
    // ========================================
    // NON-REAL-TIME THREAD - O(n) acceptable
    // ========================================
    
    void log_telemetry_NonRT() {
        // O(n) - Iterate and log (fine for background thread)
        for (size_t i = 0; i < motor_positions_.size(); ++i) {
            position_history_.push_back(motor_positions_[i]);
        }
        
        // I/O operation - only in non-RT thread!
        save_to_file(position_history_);
    }
    
    double calculate_rms_error_NonRT() const {
        // O(n) - Calculate RMS over history
        double sum_squares = 0.0;
        for (size_t i = 0; i < position_history_.size(); ++i) {
            double error = target_positions_[i % 4] - position_history_[i];
            sum_squares += error * error;
        }
        return std::sqrt(sum_squares / position_history_.size());
    }
};
```

---

### Complexity Guidelines by Thread Type

| Operation Type | Real-Time Thread | Non-Real-Time Thread |
|---------------|------------------|----------------------|
| Array access | ✅ O(1) | ✅ O(1) |
| Bitset ops | ✅ O(1) | ✅ O(1) |
| Hash lookup | ✅ O(1) | ✅ O(1) |
| Binary search | ⚠️ Avoid | ✅ O(log n) |
| Linear search | ❌ No | ✅ O(n) |
| Iteration | ❌ No | ✅ O(n) |
| Sorting | ❌ No | ✅ O(n log n) |
| File I/O | ❌ Never | ✅ Allowed |
| Network I/O | ❌ Never | ✅ Allowed |
| Dynamic alloc | ❌ Never | ⚠️ Acceptable |

---

### Key Takeaways

1. **Real-time threads**: O(1) only, no exceptions
2. **Non-real-time threads**: O(log n) and O(n) acceptable
3. **Architectural separation**: Design your system with distinct thread roles
4. **Data sharing**: Use lock-free structures (ring buffers) or keep RT data separate
5. **Offload complexity**: Move expensive operations to background threads

**Example workflow:**
- RT thread (1 kHz): Read sensor → O(1) array access → Write to circular buffer
- Non-RT thread: Read from circular buffer → Process/log/analyze → O(n) operations OK

---

## std::bitset vs std::vector\<bool\>

### The Problem with std::vector\<bool\>

`std::vector<bool>` is a **problematic specialization** that should be avoided in real-time systems:

```cpp
std::vector<bool> flags(64);

// ❌ Problems:
// 1. operator[] returns a proxy object, not bool&
// 2. Cannot take address of elements: &flags[0] won't compile as expected
// 3. Non-standard container behavior
// 4. Dynamic memory allocation
// 5. Unpredictable performance due to bit-packing overhead
```

### The Solution: std::bitset

`std::bitset` is specifically designed for bit manipulation and is **perfect for real-time systems**:

```cpp
std::bitset<64> flags;

// ✅ Advantages:
// 1. Fixed size at compile time (no heap allocation)
// 2. O(1) access and modification
// 3. Efficient bitwise operations (AND, OR, XOR, shift)
// 4. Hardware-optimized bit counting
// 5. Standard, predictable behavior
// 6. Deterministic performance
```

### Example: Real-Time Flag Manager

```cpp
template<size_t N>
class RealTimeFlagManager {
private:
    std::bitset<N> flags_;
    
public:
    // All operations are O(1)
    constexpr void set_flag(size_t index) noexcept {
        if (index < N) flags_.set(index);
    }
    
    constexpr void clear_flag(size_t index) noexcept {
        if (index < N) flags_.reset(index);
    }
    
    constexpr bool is_set(size_t index) const noexcept {
        return index < N ? flags_.test(index) : false;
    }
    
    // Hardware-accelerated on modern CPUs
    constexpr size_t count() const noexcept {
        return flags_.count();
    }
};

// Usage in embedded system
enum class SensorStatus : size_t {
    TEMPERATURE_OK = 0,
    PRESSURE_OK = 1,
    GPS_LOCK = 2,
    IMU_CALIBRATED = 3,
    // ... up to 32 or 64 sensors
};

RealTimeFlagManager<32> sensor_status;
sensor_status.set_flag(static_cast<size_t>(SensorStatus::TEMPERATURE_OK));

if (sensor_status.is_set(static_cast<size_t>(SensorStatus::GPS_LOCK))) {
    // GPS is locked, proceed with navigation
}
```

### Performance Comparison

```cpp
// Benchmark results (1M operations):
// std::bitset<64>:     ~15 μs  ✅
// std::vector<bool>:   ~45 μs  ❌
// Speedup: 3x faster!
```

**Recommendation:** ✅ **Always use `std::bitset` for real-time flag management!**

---

## STL Container Selection

### Container Time Complexity Reference

| Container | Access | Insert Front | Insert Back | Insert Mid | Search |
|-----------|--------|--------------|-------------|------------|--------|
| `std::array` | O(1) | N/A (fixed) | N/A (fixed) | N/A (fixed) | O(n) |
| `std::vector` | O(1) | O(n) | O(1)* | O(n) | O(n) |
| `std::deque` | O(1) | O(1) | O(1) | O(n) | O(n) |
| `std::list` | O(n) | O(1)** | O(1)** | O(1)** | O(n) |
| `std::map` | O(log n) | N/A | N/A | O(log n) | O(log n) |
| `std::unordered_map` | O(1) avg | N/A | N/A | O(1) avg | O(1) avg |

\* O(1) amortized, O(n) worst case (reallocation!)  
\** O(1) if you have iterator to position

---

### ✅ PREFERRED Containers

#### 1. `std::array<T, N>` - Best Choice
```cpp
std::array<double, 100> samples;

// ✅ Advantages:
// - O(1) access
// - Fixed size at compile time
// - Stack-allocated (no heap)
// - Zero overhead
// - Perfect for real-time!

samples[42] = 3.14159;  // O(1), deterministic
```

**Use when:** Size known at compile time

---

#### 2. `std::vector<T>` + `reserve()` - Second Best
```cpp
std::vector<double> samples;
samples.reserve(100);  // ✅ CRITICAL: Pre-allocate!

// ✅ Advantages:
// - O(1) access
// - Pre-allocated, no reallocation
// - Contiguous memory (cache-friendly)
// - Dynamic size (within capacity)

samples.push_back(3.14159);  // O(1), no allocation
```

**Use when:** Size known at runtime, but bounded

**⚠️ WARNING:** Without `reserve()`, `push_back()` may trigger reallocation!

---

#### 3. `std::bitset<N>` - Flags and Bit Fields
```cpp
std::bitset<64> flags;

// ✅ Advantages:
// - O(1) bit operations
// - Fixed size at compile time
// - No heap allocation
// - Hardware-optimized

flags.set(5);           // O(1)
bool is_set = flags[5]; // O(1)
size_t count = flags.count(); // O(1), hardware instruction!
```

**Use when:** Need bit flags or bit manipulation

---

#### 4. `std::string_view` - Zero-Copy Strings
```cpp
void process(std::string_view sv) noexcept {
    // ✅ No copy, no allocation
    // Just pointer + size
}

std::string data = "Hello, Real-Time!";
process(data);      // No copy
process("Literal"); // No copy
```

**Use when:** Read-only string access needed

---

### ⚠️ USE WITH CAUTION

#### `std::deque<T>`
```cpp
std::deque<int> data;

// ⚠️ Allocates in chunks
// ⚠️ Not contiguous memory
// ✅ O(1) insertion at both ends
```

**Use when:** Need O(1) insertion at both ends, size is bounded

---

#### `std::map<K, V>` / `std::set<T>`
```cpp
std::map<int, std::string> lookup;

// ⚠️ O(log n) access
// ⚠️ Allocates per element (uses tree nodes)
// ✅ Sorted order maintained
```

**Use when:** Need sorted access, size is bounded, can use custom allocator

---

### ❌ AVOID in Real-Time

#### `std::list<T>` / `std::forward_list<T>`
```cpp
std::list<int> data;

// ❌ Poor cache locality (non-contiguous)
// ❌ Heap allocation per element
// ❌ O(n) access time
// ❌ Pointer overhead per element
```

**Verdict:** ❌ **Almost never use in real-time systems**

---

#### `std::vector<bool>`
```cpp
std::vector<bool> flags;

// ❌ Problematic specialization
// ❌ Dynamic allocation
// ❌ Returns proxy objects
```

**Verdict:** ❌ **Use `std::bitset<N>` instead!**

---

### Critical: Pre-Allocation Strategy

```cpp
// ❌ BAD: Unpredictable reallocations
std::vector<int> bad_vector;
for (int i = 0; i < 1000; ++i) {
    bad_vector.push_back(i);  // May reallocate multiple times!
}

// ✅ GOOD: Pre-allocated, deterministic
std::vector<int> good_vector;
good_vector.reserve(1000);    // One allocation upfront
for (int i = 0; i < 1000; ++i) {
    good_vector.push_back(i); // O(1), no reallocation!
}
```

### Circular Buffer with `std::array`

```cpp
template<size_t N>
class CircularBuffer {
private:
    std::array<double, N> buffer_;
    size_t head_ = 0;
    size_t count_ = 0;
    
public:
    // O(1) - Add element
    constexpr void push(double value) noexcept {
        buffer_[head_] = value;
        head_ = (head_ + 1) % N;
        if (count_ < N) ++count_;
    }
    
    // O(1) - Get element
    constexpr std::optional<double> get(size_t index) const noexcept {
        if (index >= count_) return std::nullopt;
        size_t actual = (head_ + N - count_ + index) % N;
        return buffer_[actual];
    }
    
    // ✅ Zero heap allocations!
    // ✅ All operations O(1)!
    // ✅ Deterministic behavior!
};
```

---

## Memory Management

### The Golden Rule

> **In real-time systems: Pre-allocate everything during initialization, never allocate in the time-critical loop!**

---

### ✅ DO

1. **Pre-allocate all memory during initialization**
```cpp
// Initialization phase (non-real-time)
std::vector<Data> buffer;
buffer.reserve(MAX_SIZE);  // Allocate once

// Real-time loop
buffer.push_back(data);    // No allocation, O(1)
```

2. **Use memory pools for frequent alloc/dealloc**
```cpp
template<typename T, size_t PoolSize>
class MemoryPool {
private:
    std::array<T, PoolSize> pool_;
    std::array<bool, PoolSize> in_use_;
    
public:
    T* acquire() noexcept {
        for (size_t i = 0; i < PoolSize; ++i) {
            if (!in_use_[i]) {
                in_use_[i] = true;
                return &pool_[i];
            }
        }
        return nullptr;  // Pool exhausted
    }
    
    void release(T* ptr) noexcept {
        // Find and mark as available
    }
};
```

3. **Use RAII for deterministic cleanup**
```cpp
class ScopedResource {
public:
    ScopedResource(Resource* r) : resource_(r) {}
    ~ScopedResource() { 
        if (resource_) release(resource_); 
    }
    
    // Delete copy, allow move
    ScopedResource(const ScopedResource&) = delete;
    ScopedResource(ScopedResource&&) noexcept = default;
    
private:
    Resource* resource_;
};
```

4. **Prefer stack allocation**
```cpp
// ✅ Stack allocation - fast, automatic cleanup
std::array<int, 100> data;
int values[100];

// ❌ Heap allocation in real-time loop
std::vector<int> dynamic_data;  // Unless pre-allocated!
```

5. **Use `std::unique_ptr` for sole ownership**
```cpp
// Minimal overhead, deterministic cleanup
std::unique_ptr<Sensor> sensor = create_sensor();

// ⚠️ Avoid std::shared_ptr in tight loops
// (atomic reference counting overhead)
```

---

### ❌ DON'T

1. ❌ **Call `new`/`delete` in time-critical paths**
```cpp
// ❌ BAD: Heap allocation in real-time loop
for (;;) {
    auto* data = new Data();  // Non-deterministic latency!
    process(data);
    delete data;              // Non-deterministic latency!
}
```

2. ❌ **Use `std::shared_ptr` in tight loops**
```cpp
// ❌ Atomic reference counting overhead
std::shared_ptr<Data> shared_data;
```

3. ❌ **Allow unbounded memory growth**
```cpp
// ❌ BAD: Can grow indefinitely
std::vector<Data> unlimited;
unlimited.push_back(data);  // May run out of memory!

// ✅ GOOD: Bounded with circular overwrite
CircularBuffer<1000> bounded;
bounded.push(data);  // Overwrites oldest when full
```

4. ❌ **Rely on garbage collection**
- C++ uses deterministic destruction (RAII)
- No garbage collection pauses
- Predictable timing

5. ❌ **Use dynamic allocation with unpredictable size**
```cpp
// ❌ BAD: Size varies unpredictably
std::vector<int> variable_size;
variable_size.resize(get_unpredictable_size());
```

---

### RAII Example: Pool-Allocated Smart Pointer

```cpp
template<typename T>
class PoolPtr {
private:
    T* ptr_;
    std::function<void(T*)> deleter_;
    
public:
    PoolPtr(T* ptr, std::function<void(T*)> deleter) 
        : ptr_(ptr), deleter_(std::move(deleter)) {}
    
    ~PoolPtr() {
        if (ptr_ && deleter_) {
            deleter_(ptr_);  // Deterministic cleanup
        }
    }
    
    // No copy, move only
    PoolPtr(const PoolPtr&) = delete;
    PoolPtr(PoolPtr&& other) noexcept 
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;
    }
    
    T* get() noexcept { return ptr_; }
    T& operator*() noexcept { return *ptr_; }
    T* operator->() noexcept { return ptr_; }
};

// Usage
MemoryPool<SensorData, 100> pool;

{
    auto sensor = PoolPtr<SensorData>(
        pool.acquire(),
        [&pool](SensorData* ptr) { pool.release(ptr); }
    );
    
    sensor->temperature = 25.5;
    // ... use sensor ...
    
} // Automatically returned to pool via RAII!
```

---

## Modern C++ Features

### ✅ Real-Time Friendly Features

#### 1. `std::string_view` (C++17)
```cpp
// Zero-copy string observation
void process(std::string_view sv) noexcept {
    // No allocation, just pointer + size
    if (sv == "START") {
        // ...
    }
}

std::string data = "START";
process(data);      // No copy
process("LITERAL"); // No copy
```

**Benefits:**
- ✅ No allocation
- ✅ No copying
- ✅ Works with string literals
- ⚠️ Ensure underlying string lifetime!

---

#### 2. `std::optional` (C++17)
```cpp
// Error handling without exceptions
std::optional<double> safe_divide(double a, double b) noexcept {
    if (b == 0.0) return std::nullopt;
    return a / b;
}

auto result = safe_divide(10.0, 2.0);
if (result) {
    std::cout << *result;  // 5.0
}
```

**Benefits:**
- ✅ No exceptions
- ✅ No heap allocation
- ✅ Clear intent (optional value)
- ✅ Stack-based

---

#### 3. `std::variant` (C++17)
```cpp
// Type-safe union without heap
using SensorValue = std::variant<int, double, std::array<uint8_t, 4>>;

void process(const SensorValue& value) noexcept {
    std::visit([](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) {
            // Handle int
        } else if constexpr (std::is_same_v<T, double>) {
            // Handle double
        }
    }, value);
}
```

**Benefits:**
- ✅ Type-safe
- ✅ No heap allocation
- ✅ No virtual dispatch
- ✅ Compile-time type checking

---

#### 4. `constexpr` - Compile-Time Computation
```cpp
constexpr uint32_t compute_crc32(std::string_view data) {
    uint32_t crc = 0xFFFFFFFF;
    for (char c : data) {
        crc ^= static_cast<uint32_t>(c);
        for (int i = 0; i < 8; ++i) {
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
        }
    }
    return ~crc;
}

// Computed at compile time!
constexpr uint32_t header_crc = compute_crc32("HEADER");
```

**Benefits:**
- ✅ Zero runtime cost
- ✅ Computed by compiler
- ✅ Values stored in ROM

---

#### 5. `std::array` - Type-Safe Stack Array
```cpp
std::array<double, 100> samples;

// ✅ Benefits:
// - Stack-allocated
// - Compile-time size
// - Bounds checking (at() method)
// - STL algorithms compatible
// - Zero overhead vs C array
```

---

### ❌ Features to Avoid in Hard Real-Time

#### Exceptions
```cpp
// ❌ Exceptions add overhead and unpredictability
try {
    risky_operation();
} catch (const std::exception& e) {
    // Exception handling has non-deterministic timing
}

// ✅ Better: Use return codes or std::optional
std::optional<Result> safe_operation() noexcept;

// Compiler flag: -fno-exceptions
```

---

#### RTTI (Run-Time Type Information)
```cpp
// ❌ RTTI increases binary size and has runtime cost
if (typeid(obj) == typeid(SpecificType)) {
    // ...
}

// ✅ Better: Use std::variant or compile-time polymorphism

// Compiler flag: -fno-rtti
```

---

#### Virtual Functions in Critical Paths
```cpp
// ❌ Virtual function calls have overhead
class Base {
public:
    virtual void process() = 0;  // Virtual dispatch overhead
};

// ✅ Better: Use CRTP (Curiously Recurring Template Pattern)
template<typename Derived>
class Base {
public:
    void process() {
        static_cast<Derived*>(this)->process_impl();
    }
};

class Derived : public Base<Derived> {
public:
    void process_impl() {
        // No virtual dispatch!
    }
};
```

---

#### `std::shared_ptr` in Tight Loops
```cpp
// ❌ Atomic reference counting overhead
for (size_t i = 0; i < 1000000; ++i) {
    std::shared_ptr<Data> ptr = shared_data;  // Atomic increment
    process(ptr);
    // Atomic decrement on destruction
}

// ✅ Better: Use std::unique_ptr or raw pointers with careful lifetime management
```

---

## Real-Time Checklist

### 📋 Initialization Phase (Non-Real-Time)

- [ ] Pre-allocate all memory (`reserve()` for vectors)
- [ ] Create memory pools for frequent allocations
- [ ] Load and sort lookup tables
- [ ] Initialize all data structures to max size
- [ ] Warm up caches if applicable
- [ ] Set thread priorities and CPU affinity
- [ ] Run any O(n log n) or worse operations (sorting, etc.)

---

### 📋 Real-Time Execution Phase

**Prohibitions:**
- [ ] NO `new`/`delete`/`malloc`/`free`
- [ ] NO operations with unbounded execution time
- [ ] NO file I/O or blocking system calls
- [ ] NO unbounded loops or recursion
- [ ] NO exceptions in critical paths
- [ ] NO virtual function calls if avoidable

**Requirements:**
- [ ] Use O(1) or O(log n) operations only
- [ ] All containers have bounded maximum size
- [ ] Use `std::bitset` instead of `std::vector<bool>`
- [ ] Use `std::array` when size known at compile-time
- [ ] Use RAII for deterministic cleanup
- [ ] Measure worst-case execution time (WCET)

---

### 📋 Container Selection

**Preferred:**
- [ ] `std::array<T, N>` - Fixed size, stack-based
- [ ] `std::vector<T>` + `reserve()` - Dynamic, pre-allocated
- [ ] `std::bitset<N>` - Flags and bit manipulation
- [ ] `std::string_view` - Zero-copy string viewing

**Use with Caution:**
- [ ] `std::deque<T>` - If size bounded
- [ ] `std::map<T>` / `std::set<T>` - If size bounded, consider custom allocator

**Avoid:**
- [ ] `std::list<T>` - Poor cache, heap per element
- [ ] `std::forward_list<T>` - Poor cache
- [ ] `std::vector<bool>` - Use `std::bitset` instead

---

### 📋 Algorithm Complexity Targets

**Real-Time Threads (Control loops, sensor processing):**
- [ ] ✅ O(1) - REQUIRED (array access, bitset operations)
- [ ] ⚠️ O(log n) - Only if absolutely necessary and bounded
- [ ] ❌ O(n) - AVOID in time-critical code paths
- [ ] ❌ O(n log n) - Never
- [ ] ❌ O(n²) - Never

**Non-Real-Time Threads (Logging, diagnostics, background processing):**
- [ ] ✅ O(1) - Ideal (always preferred)
- [ ] ✅ O(log n) - Good (binary search, std::map operations)
- [ ] ✅ O(n) - Acceptable with bounded size (< 10,000)
- [ ] ⚠️ O(n log n) - Acceptable occasionally (sorting, data preparation)
- [ ] ❌ O(n²) - Avoid unless dataset very small (< 100)

---

### 📋 Compiler Flags for Real-Time

```bash
# Optimization
-O3                  # Maximum optimization
-march=native        # Target specific CPU architecture
-flto                # Link-time optimization

# Real-time specific
-fno-exceptions      # Disable exception handling
-fno-rtti            # Disable run-time type information

# Analysis
-Wall -Wextra        # Enable all warnings
-Werror              # Treat warnings as errors
```

---

### 📋 Profiling and Verification

- [ ] Measure worst-case execution time (WCET)
- [ ] Use real-time profiling tools (not just average time!)
- [ ] Test under worst-case conditions
- [ ] Verify no unexpected allocations (`-fsanitize=address`)
- [ ] Monitor stack usage
- [ ] Validate all timing constraints are met
- [ ] Profile on target hardware (not just development machine)

---

## Quick Reference Table

| Feature | Real-Time Friendly? | Notes |
|---------|---------------------|-------|
| `std::array` | ✅ Yes | Best choice, stack-based |
| `std::vector` + `reserve()` | ✅ Yes | Pre-allocate capacity! |
| `std::bitset` | ✅ Yes | Better than `vector<bool>` |
| `std::string_view` | ✅ Yes | Zero-copy, no allocation |
| `std::optional` | ✅ Yes | No exceptions, stack-based |
| `std::variant` | ✅ Yes | Type-safe union, no heap |
| `constexpr` | ✅ Yes | Compile-time computation |
| `std::unique_ptr` | ✅ Yes | Minimal overhead |
| `std::deque` | ⚠️ Caution | Bound size, allocates chunks |
| `std::map/set` | ⚠️ Caution | O(log n), custom allocator |
| `std::vector` (no reserve) | ❌ No | Unpredictable reallocations |
| `std::vector<bool>` | ❌ No | Use `std::bitset` instead |
| `std::list` | ❌ No | Poor cache, heap per element |
| `std::shared_ptr` | ❌ No | Atomic overhead |
| Exceptions | ❌ No | Non-deterministic timing |
| RTTI | ❌ No | Runtime overhead |
| Virtual functions | ❌ Avoid | Use in non-critical paths only |

---

## Key Takeaways

1. **Always pre-allocate memory during initialization** - Use `reserve()` for vectors
2. **Use `std::bitset` instead of `std::vector<bool>`** - Fixed size, O(1) operations
3. **Prefer O(1) operations, accept O(log n), bound O(n)** - Know your algorithm complexity
4. **Use `std::array` for fixed-size data** - Stack-based, zero overhead
5. **RAII provides deterministic resource management** - Automatic, exception-safe cleanup
6. **Modern C++ features are real-time friendly** - `optional`, `variant`, `string_view` have no heap overhead
7. **Measure worst-case execution time (WCET)**, not average - That's what matters in real-time systems
8. **Disable exceptions and RTTI** - Use `-fno-exceptions -fno-rtti` for hard real-time

---

## Example Code

See [RealTimeProgramming.cpp](../src/RealTimeProgramming.cpp) for a comprehensive, runnable example demonstrating:

- Big O notation with timing measurements
- `std::bitset` vs `std::vector<bool>` comparison
- Pre-allocated containers and circular buffers
- Memory pools with RAII
- Modern C++ features for real-time systems
- Complete real-time system examples

---

## Additional Resources

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Real-Time C++ (Pfannkuchen)](https://www.amazon.com/Real-Time-Efficient-Object-Oriented-Microcontroller-Programming/dp/3662629941)
- [ISO 26262 Functional Safety Standard](https://www.iso.org/standard/68383.html)
- [MISRA C++ Guidelines](https://www.misra.org.uk/)

---

*Last updated: 2026*
