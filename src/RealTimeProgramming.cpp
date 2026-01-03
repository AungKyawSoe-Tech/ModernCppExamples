// RealTimeProgramming.cpp
// Comprehensive guide to Real-Time Programming in Modern C++
//
// REAL-TIME SYSTEM CHARACTERISTICS:
// 1. Deterministic behavior - predictable execution times
// 2. Bounded worst-case execution time (WCET)
// 3. Low and predictable latency
// 4. No dynamic memory allocation in critical paths
// 5. Avoid operations with unbounded execution time
//
// KEY TOPICS COVERED:
// • Big O Notation and Time Complexity
// • STL containers for real-time systems
// • std::bitset vs std::vector<bool>
// • Memory pre-allocation strategies
// • RAII and deterministic resource management
// • Avoiding heap fragmentation
// • Custom allocators and memory pools

#include <iostream>
#include <vector>
#include <array>
#include <list>
#include <bitset>
#include <chrono>
#include <algorithm>
#include <memory>
#include <functional>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <iomanip>
#include <cstdint>

// ============================================================================
// SECTION 1: Big O Notation - Understanding Time Complexity
// ============================================================================

namespace big_o_notation {

void demonstrate_O1() {
    std::cout << "\nO(1) - Constant Time:\n";
    std::cout << "  • Execution time does not depend on input size\n";
    std::cout << "  • Always takes the same amount of time\n\n";
    
    std::array<int, 1000> data;
    data.fill(42);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Array/vector element access - O(1)
    [[maybe_unused]] int value = data[500];
    
    // Hash table lookup - O(1) average case
    // Stack push/pop - O(1)
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    std::cout << "  Example: Array element access data[500]\n";
    std::cout << "  Time: " << duration.count() << " ns (constant, regardless of array size)\n";
    std::cout << "  ✓ BEST for real-time systems - predictable!\n";
}

void demonstrate_Ologn() {
    std::cout << "\nO(log n) - Logarithmic Time:\n";
    std::cout << "  • Execution time grows logarithmically with input size\n";
    std::cout << "  • Doubling input size adds constant time\n\n";
    
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<int>(i);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Binary search - O(log n)
    bool found = std::binary_search(data.begin(), data.end(), 500);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    std::cout << "  Example: Binary search in sorted array of 1000 elements\n";
    std::cout << "  Time: " << duration.count() << " ns\n";
    std::cout << "  Found: " << std::boolalpha << found << "\n";
    std::cout << "  Note: For 1M elements, time increases by only ~10x\n";
    std::cout << "  ✓ ACCEPTABLE for real-time systems with bounded size\n";
}

void demonstrate_On() {
    std::cout << "\nO(n) - Linear Time:\n";
    std::cout << "  • Execution time grows linearly with input size\n";
    std::cout << "  • Doubling input doubles execution time\n\n";
    
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<int>(i);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Linear search - O(n)
    auto it = std::find(data.begin(), data.end(), 500);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    std::cout << "  Example: Linear search through 1000 elements\n";
    std::cout << "  Time: " << duration.count() << " ns\n";
    std::cout << "  Found: " << (it != data.end()) << "\n";
    std::cout << "  ⚠ USE WITH CAUTION - bound the maximum container size!\n";
}

void demonstrate_Onlogn() {
    std::cout << "\nO(n log n) - Linearithmic Time:\n";
    std::cout << "  • Execution time grows at n * log(n)\n";
    std::cout << "  • Common in efficient sorting algorithms\n\n";
    
    std::vector<int> data(1000);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = 1000 - static_cast<int>(i);  // Reverse order
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Sorting - O(n log n) for std::sort
    std::sort(data.begin(), data.end());
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "  Example: std::sort on 1000 elements\n";
    std::cout << "  Time: " << duration.count() << " μs\n";
    std::cout << "  ⚠ AVOID in time-critical paths - do in initialization phase\n";
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 1: Big O Notation - Time Complexity ===\n";
    std::cout << std::string(70, '=') << "\n";
    
    demonstrate_O1();
    demonstrate_Ologn();
    demonstrate_On();
    demonstrate_Onlogn();
    
    std::cout << "\nREAL-TIME SYSTEM PRIORITY:\n\n";
    
    std::cout << "  REAL-TIME THREADS (Hard real-time, strict deadlines):\n";
    std::cout << "    ✓ O(1)     - REQUIRED (array access, bitset ops, hash lookup)\n";
    std::cout << "    ⚠ O(log n) - Only if absolutely necessary AND size bounded\n";
    std::cout << "    ✗ O(n)     - AVOID in time-critical paths\n\n";
    
    std::cout << "  NON-REAL-TIME THREADS (Background tasks, logging, diagnostics):\n";
    std::cout << "    ✓ O(1)     - Ideal (always preferred)\n";
    std::cout << "    ✓ O(log n) - Acceptable (binary search, std::map)\n";
    std::cout << "    ✓ O(n)     - Acceptable with bounded size (linear search, iteration)\n";
    std::cout << "    ⚠ O(n²)    - Avoid unless dataset very small\n\n";
    
    std::cout << "  ✓ GOLDEN RULE: Separate real-time from non-real-time threads!\n";
    std::cout << "  ✓ Use O(1) operations in control loops and sensor processing\n";
    std::cout << "  ✓ Offload O(n) operations to background threads\n";
}

} // namespace big_o_notation

// ============================================================================
// SECTION 1.5: Real-Time Thread Architecture
// ============================================================================

namespace thread_architecture {

// Example: Separating real-time and non-real-time concerns

class SensorSystem {
private:
    // Real-time data - accessed by RT thread
    std::array<double, 10> latest_readings_;  // O(1) access
    std::bitset<10> sensor_status_;            // O(1) operations
    size_t current_index_ = 0;
    
    // Non-real-time data - accessed by background thread
    std::vector<double> historical_data_;      // Can grow, processed in background
    
public:
    SensorSystem() {
        historical_data_.reserve(10000);  // Pre-allocate for background thread
    }
    
    // REAL-TIME THREAD: O(1) operations only!
    void process_sensor_reading_RT(size_t sensor_id, double value) noexcept {
        // O(1) - Store latest reading
        latest_readings_[sensor_id] = value;
        
        // O(1) - Update status flag
        sensor_status_.set(sensor_id);
        
        // O(1) - No complex processing here!
    }
    
    // REAL-TIME THREAD: O(1) read access
    [[nodiscard]] double get_latest_reading_RT(size_t sensor_id) const noexcept {
        return latest_readings_[sensor_id];  // O(1)
    }
    
    // NON-REAL-TIME THREAD: Can use O(n) operations
    void log_historical_data_NonRT() {
        // O(n) - Iterate and log (acceptable in background thread)
        for (size_t i = 0; i < latest_readings_.size(); ++i) {
            if (sensor_status_.test(i)) {
                historical_data_.push_back(latest_readings_[i]);
            }
        }
    }
    
    // NON-REAL-TIME THREAD: Can use O(n) operations
    [[nodiscard]] double calculate_average_NonRT() const noexcept {
        if (historical_data_.empty()) return 0.0;
        
        // O(n) - Acceptable in non-RT thread
        double sum = 0.0;
        for (double val : historical_data_) {
            sum += val;
        }
        return sum / historical_data_.size();
    }
    
    // NON-REAL-TIME THREAD: Can use O(n log n) operations
    [[nodiscard]] double calculate_median_NonRT() {
        if (historical_data_.empty()) return 0.0;
        
        // O(n log n) - Sorting is expensive, do in background!
        auto data_copy = historical_data_;  // Copy to avoid modifying original
        std::sort(data_copy.begin(), data_copy.end());
        
        size_t mid = data_copy.size() / 2;
        return data_copy[mid];
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 1.5: Real-Time Thread Architecture ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "THREAD SEPARATION PRINCIPLE:\n\n";
    
    std::cout << "REAL-TIME THREADS:\n";
    std::cout << "  • Purpose: Control loops, sensor processing, motor control\n";
    std::cout << "  • Requirements: Deterministic, predictable timing\n";
    std::cout << "  • Allowed: O(1) operations ONLY\n";
    std::cout << "  • Examples:\n";
    std::cout << "    - Reading sensor value from array\n";
    std::cout << "    - Updating control output\n";
    std::cout << "    - Checking/setting status flags (bitset)\n";
    std::cout << "    - Simple arithmetic calculations\n\n";
    
    std::cout << "NON-REAL-TIME THREADS:\n";
    std::cout << "  • Purpose: Logging, diagnostics, data aggregation, UI updates\n";
    std::cout << "  • Requirements: Eventually complete, no strict deadlines\n";
    std::cout << "  • Allowed: O(1), O(log n), O(n) operations\n";
    std::cout << "  • Examples:\n";
    std::cout << "    - Writing logs to disk\n";
    std::cout << "    - Calculating statistics (average, median)\n";
    std::cout << "    - Searching historical data\n";
    std::cout << "    - Generating reports\n\n";
    
    std::cout << "EXAMPLE: Sensor System with Thread Separation\n\n";
    
    SensorSystem system;
    
    // Simulate real-time thread
    std::cout << "  REAL-TIME THREAD (1 kHz control loop):\n";
    auto rt_start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < 10; ++i) {
        system.process_sensor_reading_RT(i % 10, i * 0.5);  // O(1) per reading
    }
    
    auto rt_end = std::chrono::high_resolution_clock::now();
    auto rt_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(rt_end - rt_start);
    
    std::cout << "    Processed 10 readings in " << rt_duration.count() << " ns\n";
    std::cout << "    Per-reading time: " << (rt_duration.count() / 10) << " ns (deterministic!)\n\n";
    
    // Simulate non-real-time thread
    std::cout << "  NON-REAL-TIME THREAD (background processing):\n";
    
    system.log_historical_data_NonRT();  // O(n) - acceptable
    
    auto bg_start = std::chrono::high_resolution_clock::now();
    double avg = system.calculate_average_NonRT();  // O(n)
    auto bg_end = std::chrono::high_resolution_clock::now();
    auto bg_duration = std::chrono::duration_cast<std::chrono::microseconds>(bg_end - bg_start);
    
    std::cout << "    Calculated average: " << avg << "\n";
    std::cout << "    Time: " << bg_duration.count() << " μs (non-critical!)\n\n";
    
    std::cout << "  ✓ ARCHITECTURE: Real-time thread stays O(1), background handles O(n)\n";
    std::cout << "  ✓ BENEFIT: Control loop remains deterministic and fast\n";
    std::cout << "  ✓ BENEFIT: Complex processing doesn't block time-critical operations\n";
}

} // namespace thread_architecture

// ============================================================================
// SECTION 2: std::bitset vs std::vector<bool> - The Better Choice
// ============================================================================

namespace bitset_vs_vector_bool {

// std::vector<bool> is a problematic specialization
// - Uses space optimization (1 bit per element)
// - Does NOT behave like a regular container
// - operator[] returns a proxy object, not a reference
// - Cannot get pointer to element
// - NOT suitable for real-time systems due to proxy complexity

// std::bitset is designed for bit manipulation
// - Fixed size at compile time
// - O(1) access and modification
// - Efficient bit operations (AND, OR, XOR, shift)
// - No dynamic memory allocation
// - PERFECT for real-time systems

template<size_t N>
class RealTimeFlagManager {
private:
    std::bitset<N> flags_;
    
public:
    // O(1) - Set a flag
    constexpr void set_flag(size_t index) noexcept {
        if (index < N) {
            flags_.set(index);
        }
    }
    
    // O(1) - Clear a flag
    constexpr void clear_flag(size_t index) noexcept {
        if (index < N) {
            flags_.reset(index);
        }
    }
    
    // O(1) - Toggle a flag
    constexpr void toggle_flag(size_t index) noexcept {
        if (index < N) {
            flags_.flip(index);
        }
    }
    
    // O(1) - Check a flag
    [[nodiscard]] constexpr bool is_set(size_t index) const noexcept {
        return index < N ? flags_.test(index) : false;
    }
    
    // O(1) - Count set bits (hardware instruction on modern CPUs)
    [[nodiscard]] constexpr size_t count() const noexcept {
        return flags_.count();
    }
    
    // O(1) - Check if any flags are set
    [[nodiscard]] constexpr bool any() const noexcept {
        return flags_.any();
    }
    
    // O(1) - Check if all flags are set
    [[nodiscard]] constexpr bool all() const noexcept {
        return flags_.all();
    }
    
    // O(1) - Check if no flags are set
    [[nodiscard]] constexpr bool none() const noexcept {
        return flags_.none();
    }
    
    // Bitwise operations - O(1)
    constexpr RealTimeFlagManager operator&(const RealTimeFlagManager& other) const noexcept {
        RealTimeFlagManager result;
        result.flags_ = flags_ & other.flags_;
        return result;
    }
    
    constexpr RealTimeFlagManager operator|(const RealTimeFlagManager& other) const noexcept {
        RealTimeFlagManager result;
        result.flags_ = flags_ | other.flags_;
        return result;
    }
};

// Example: Sensor status flags for embedded system
constexpr size_t NUM_SENSORS = 32;

enum class SensorStatus : size_t {
    TEMPERATURE_OK = 0,
    PRESSURE_OK = 1,
    HUMIDITY_OK = 2,
    VOLTAGE_OK = 3,
    CURRENT_OK = 4,
    GPS_LOCK = 5,
    IMU_CALIBRATED = 6,
    COMMUNICATION_OK = 7,
    // ... up to 32 sensors
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 2: std::bitset vs std::vector<bool> ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "WHY std::vector<bool> IS PROBLEMATIC:\n";
    std::cout << "  ✗ Space-optimized specialization (1 bit per element)\n";
    std::cout << "  ✗ operator[] returns proxy object, not bool&\n";
    std::cout << "  ✗ Cannot take address of elements\n";
    std::cout << "  ✗ Non-standard container behavior\n";
    std::cout << "  ✗ Dynamic memory allocation\n";
    std::cout << "  ✗ Unpredictable performance\n\n";
    
    std::cout << "WHY std::bitset IS BETTER FOR REAL-TIME:\n";
    std::cout << "  ✓ Fixed size at compile time (no heap allocation)\n";
    std::cout << "  ✓ O(1) access and modification\n";
    std::cout << "  ✓ Efficient bitwise operations (AND, OR, XOR)\n";
    std::cout << "  ✓ Hardware-optimized bit counting\n";
    std::cout << "  ✓ Standard, predictable behavior\n";
    std::cout << "  ✓ Deterministic performance\n\n";
    
    // Demonstrate real-time flag manager
    RealTimeFlagManager<NUM_SENSORS> sensor_status;
    
    std::cout << "EXAMPLE: Real-Time Sensor Status Manager\n";
    std::cout << "  Using std::bitset<32> for 32 sensor flags\n\n";
    
    // Set some flags
    sensor_status.set_flag(static_cast<size_t>(SensorStatus::TEMPERATURE_OK));
    sensor_status.set_flag(static_cast<size_t>(SensorStatus::PRESSURE_OK));
    sensor_status.set_flag(static_cast<size_t>(SensorStatus::GPS_LOCK));
    
    std::cout << "  Set flags: TEMPERATURE_OK, PRESSURE_OK, GPS_LOCK\n";
    std::cout << "  Active sensors: " << sensor_status.count() << " / " << NUM_SENSORS << "\n";
    std::cout << "  All sensors OK? " << std::boolalpha << sensor_status.all() << "\n";
    std::cout << "  Any sensor OK? " << sensor_status.any() << "\n";
    
    // Check specific flag
    bool temp_ok = sensor_status.is_set(static_cast<size_t>(SensorStatus::TEMPERATURE_OK));
    std::cout << "  Temperature sensor OK? " << temp_ok << "\n\n";
    
    // Timing comparison
    constexpr size_t ITERATIONS = 1000000;
    
    std::bitset<64> bitset_flags;
    auto start_bitset = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERATIONS; ++i) {
        bitset_flags.set(i % 64);
        [[maybe_unused]] bool val = bitset_flags.test(i % 64);
    }
    auto end_bitset = std::chrono::high_resolution_clock::now();
    auto bitset_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_bitset - start_bitset);
    
    std::vector<bool> vector_bool(64, false);
    auto start_vector = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERATIONS; ++i) {
        vector_bool[i % 64] = true;
        [[maybe_unused]] bool val = vector_bool[i % 64];
    }
    auto end_vector = std::chrono::high_resolution_clock::now();
    auto vector_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_vector - start_vector);
    
    std::cout << "PERFORMANCE COMPARISON (" << ITERATIONS << " operations):\n";
    std::cout << "  std::bitset<64>:     " << std::setw(8) << bitset_duration.count() << " μs\n";
    std::cout << "  std::vector<bool>:   " << std::setw(8) << vector_duration.count() << " μs\n";
    std::cout << "  Speedup:             " << std::fixed << std::setprecision(2) 
              << (static_cast<double>(vector_duration.count()) / bitset_duration.count()) << "x\n\n";
    
    std::cout << "  ✓ RECOMMENDATION: Use std::bitset for real-time flag management!\n";
}

} // namespace bitset_vs_vector_bool

// ============================================================================
// SECTION 3: STL Containers - Real-Time Best Practices
// ============================================================================

namespace stl_containers_realtime {

// Container Complexity Reference:
//
// std::vector:
//   - Access:       O(1)
//   - Insert front: O(n)
//   - Insert back:  O(1) amortized, O(n) worst case (reallocation!)
//   - Insert mid:   O(n)
//   - Real-time:    Use reserve() to pre-allocate!
//
// std::array:
//   - Access:       O(1)
//   - Size:         Fixed at compile time
//   - Real-time:    PERFECT - no dynamic allocation, stack-based
//
// std::deque:
//   - Access:       O(1)
//   - Insert front: O(1)
//   - Insert back:  O(1)
//   - Real-time:    Acceptable if size bounded
//
// std::list:
//   - Access:       O(n) - must traverse!
//   - Insert:       O(1) if you have iterator
//   - Real-time:    AVOID - dynamic allocation per element, poor cache
//
// std::map/std::set:
//   - Access:       O(log n)
//   - Insert:       O(log n)
//   - Real-time:    Use with custom allocator or bounded size

// Example 1: Pre-allocated std::vector - The Right Way
class RealTimeDataBuffer {
private:
    static constexpr size_t MAX_SAMPLES = 1000;
    std::vector<double> samples_;
    
public:
    RealTimeDataBuffer() {
        // ✓ CRITICAL: Pre-allocate during initialization
        // This prevents reallocation during real-time operation
        samples_.reserve(MAX_SAMPLES);
        
        std::cout << "  Buffer created with capacity: " << samples_.capacity() << "\n";
        std::cout << "  No reallocation will occur until this limit\n";
    }
    
    // O(1) - Add sample (no reallocation if within capacity)
    bool add_sample(double value) noexcept {
        if (samples_.size() < MAX_SAMPLES) {
            samples_.push_back(value);  // O(1) - deterministic!
            return true;
        }
        return false;  // Buffer full
    }
    
    // O(1) - Get sample
    [[nodiscard]] std::optional<double> get_sample(size_t index) const noexcept {
        if (index < samples_.size()) {
            return samples_[index];
        }
        return std::nullopt;
    }
    
    // O(n) - Clear buffer (but no deallocation!)
    void clear() noexcept {
        samples_.clear();
        // Capacity remains unchanged - memory is retained!
    }
    
    [[nodiscard]] size_t size() const noexcept { return samples_.size(); }
    [[nodiscard]] size_t capacity() const noexcept { return samples_.capacity(); }
};

// Example 2: Fixed-size std::array - Even Better
template<size_t N>
class CircularBuffer {
private:
    std::array<double, N> buffer_;
    size_t head_ = 0;
    size_t count_ = 0;
    
public:
    // O(1) - Add element (overwrites oldest if full)
    constexpr void push(double value) noexcept {
        buffer_[head_] = value;
        head_ = (head_ + 1) % N;
        if (count_ < N) {
            ++count_;
        }
    }
    
    // O(1) - Get element (0 = most recent)
    [[nodiscard]] constexpr std::optional<double> get(size_t index) const noexcept {
        if (index >= count_) {
            return std::nullopt;
        }
        size_t actual_index = (head_ + N - count_ + index) % N;
        return buffer_[actual_index];
    }
    
    [[nodiscard]] constexpr size_t size() const noexcept { return count_; }
    [[nodiscard]] constexpr bool is_full() const noexcept { return count_ == N; }
    
    // Statistics - O(n) but bounded by N
    [[nodiscard]] double average() const noexcept {
        if (count_ == 0) return 0.0;
        double sum = 0.0;
        for (size_t i = 0; i < count_; ++i) {
            size_t idx = (head_ + N - count_ + i) % N;
            sum += buffer_[idx];
        }
        return sum / count_;
    }
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 3: STL Containers - Real-Time Best Practices ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "CONTAINER SELECTION FOR REAL-TIME SYSTEMS:\n\n";
    
    std::cout << "✓ PREFERRED:\n";
    std::cout << "  1. std::array<T, N>           - O(1) access, no heap, compile-time size\n";
    std::cout << "  2. std::vector<T> + reserve() - O(1) access, pre-allocated\n";
    std::cout << "  3. std::bitset<N>             - O(1) bit ops, no heap\n";
    std::cout << "  4. std::string_view           - O(1) string viewing, no allocation\n\n";
    
    std::cout << "⚠ USE WITH CAUTION (bound size!):\n";
    std::cout << "  • std::deque<T>               - O(1) both ends, but allocates chunks\n";
    std::cout << "  • std::map/set<T>             - O(log n), allocates per element\n\n";
    
    std::cout << "✗ AVOID:\n";
    std::cout << "  • std::list<T>                - Poor cache locality, alloc per element\n";
    std::cout << "  • std::forward_list<T>        - Poor cache locality\n";
    std::cout << "  • std::vector<bool>           - Use std::bitset instead!\n";
    std::cout << "  • Unbounded growth containers - Always set max size!\n\n";
    
    // Demonstrate pre-allocated vector
    std::cout << "EXAMPLE 1: Pre-allocated std::vector\n";
    RealTimeDataBuffer buffer;
    
    for (int i = 0; i < 100; ++i) {
        buffer.add_sample(i * 0.1);
    }
    
    std::cout << "  Added 100 samples\n";
    std::cout << "  Current size: " << buffer.size() << "\n";
    std::cout << "  Capacity: " << buffer.capacity() << "\n";
    std::cout << "  ✓ No reallocation occurred - deterministic O(1) insertion!\n\n";
    
    // Demonstrate fixed-size array
    std::cout << "EXAMPLE 2: Fixed-size circular buffer (std::array<T, N>)\n";
    CircularBuffer<10> circ_buffer;
    
    for (int i = 0; i < 15; ++i) {
        circ_buffer.push(i * 1.5);
    }
    
    std::cout << "  Pushed 15 values into buffer of size 10\n";
    std::cout << "  Current size: " << circ_buffer.size() << "\n";
    std::cout << "  Average: " << circ_buffer.average() << "\n";
    std::cout << "  ✓ No heap allocation - pure stack-based, deterministic!\n\n";
    
    // Demonstrate reserve() importance
    std::cout << "CRITICAL: Always use reserve() for std::vector in real-time code!\n\n";
    
    std::vector<int> bad_vector;  // No reserve
    auto start_bad = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        bad_vector.push_back(i);  // May trigger reallocations!
    }
    auto end_bad = std::chrono::high_resolution_clock::now();
    auto bad_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_bad - start_bad);
    
    std::vector<int> good_vector;
    good_vector.reserve(1000);  // Pre-allocate
    auto start_good = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        good_vector.push_back(i);  // No reallocations!
    }
    auto end_good = std::chrono::high_resolution_clock::now();
    auto good_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_good - start_good);
    
    std::cout << "  Without reserve(): " << bad_duration.count() << " μs (unpredictable!)\n";
    std::cout << "  With reserve():    " << good_duration.count() << " μs (deterministic!)\n";
    std::cout << "  Speedup:           " << std::fixed << std::setprecision(2)
              << (static_cast<double>(bad_duration.count()) / good_duration.count()) << "x\n\n";
    
    std::cout << "  ✓ GOLDEN RULE: reserve() prevents non-deterministic reallocations!\n";
}

} // namespace stl_containers_realtime

// ============================================================================
// SECTION 3.5: std::list::splice() - O(1) Element Movement
// ============================================================================

namespace list_splice_realtime {

// std::list::splice() is PERFECT for real-time systems because:
// 1. O(1) complexity for moving elements (constant time!)
// 2. NO MEMORY ALLOCATION - just pointer manipulation
// 3. NO COPYING/MOVING - elements stay in place
// 4. Deterministic and predictable performance
// 5. Iterators remain valid after splice
//
// Use cases in real-time systems:
// - Free lists / object pools
// - Task queue management
// - Event scheduling
// - Moving data between priority queues

struct Task {
    int id;
    std::string name;
    int priority;
    
    Task(int i, std::string n, int p) : id(i), name(std::move(n)), priority(p) {}
};

void demonstrate() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "SECTION 3.5: std::list::splice() - O(1) Element Movement\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "WHY splice() IS CRITICAL FOR REAL-TIME SYSTEMS:\n";
    std::cout << "  • O(1) constant time - no iteration required\n";
    std::cout << "  • NO memory allocation - just rewires pointers\n";
    std::cout << "  • NO copying/moving - elements stay in original memory\n";
    std::cout << "  • Iterators remain valid - predictable behavior\n";
    std::cout << "  • Deterministic performance - perfect for hard real-time\n\n";
    
    // =======================================================================
    // EXAMPLE 1: splice() entire list - O(1)
    // =======================================================================
    std::cout << std::string(80, '-') << "\n";
    std::cout << "EXAMPLE 1: Splice Entire List - O(1) Operation\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::list<Task> active_tasks;
    active_tasks.emplace_back(1, "Process Sensor Data", 10);
    active_tasks.emplace_back(2, "Update Control Loop", 20);
    active_tasks.emplace_back(3, "Send Telemetry", 5);
    
    std::list<Task> pending_tasks;
    pending_tasks.emplace_back(4, "Log Event", 1);
    pending_tasks.emplace_back(5, "Check Diagnostics", 3);
    
    std::cout << "BEFORE splice():\n";
    std::cout << "  Active tasks: " << active_tasks.size() << "\n";
    for (const auto& t : active_tasks) {
        std::cout << "    Task " << t.id << ": " << t.name << " (priority: " << t.priority << ")\n";
    }
    std::cout << "  Pending tasks: " << pending_tasks.size() << "\n";
    for (const auto& t : pending_tasks) {
        std::cout << "    Task " << t.id << ": " << t.name << " (priority: " << t.priority << ")\n";
    }
    
    // Splice entire pending_tasks list into active_tasks at the end
    // This is O(1) - just rewires a few pointers!
    auto start = std::chrono::high_resolution_clock::now();
    active_tasks.splice(active_tasks.end(), pending_tasks);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    std::cout << "\nAFTER splice(active.end(), pending):\n";
    std::cout << "  Active tasks: " << active_tasks.size() << "\n";
    for (const auto& t : active_tasks) {
        std::cout << "    Task " << t.id << ": " << t.name << " (priority: " << t.priority << ")\n";
    }
    std::cout << "  Pending tasks: " << pending_tasks.size() << " (empty!)\n";
    std::cout << "  ⏱ Time taken: " << duration.count() << " ns (O(1) - constant time!)\n";
    std::cout << "  ✓ NO allocation, NO copying, just pointer rewiring!\n\n";
    
    // =======================================================================
    // EXAMPLE 2: splice() single element - O(1)
    // =======================================================================
    std::cout << std::string(80, '-') << "\n";
    std::cout << "EXAMPLE 2: Splice Single Element - O(1) Operation\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::list<Task> high_priority;
    high_priority.emplace_back(10, "Critical Shutdown", 100);
    high_priority.emplace_back(11, "Emergency Stop", 99);
    
    std::list<Task> normal_priority;
    normal_priority.emplace_back(20, "Routine Check", 10);
    normal_priority.emplace_back(21, "Update Display", 8);
    normal_priority.emplace_back(22, "Log Status", 5);
    
    std::cout << "SCENARIO: Emergency task detected in normal queue!\n\n";
    
    std::cout << "BEFORE splice():\n";
    std::cout << "  High priority: " << high_priority.size() << " tasks\n";
    std::cout << "  Normal priority: " << normal_priority.size() << " tasks\n";
    for (const auto& t : normal_priority) {
        std::cout << "    Task " << t.id << ": " << t.name << " (priority: " << t.priority << ")\n";
    }
    
    // Find the "Update Display" task and move it to high priority
    auto it = std::find_if(normal_priority.begin(), normal_priority.end(),
                          [](const Task& t) { return t.id == 21; });
    
    if (it != normal_priority.end()) {
        std::cout << "\n  Found Task 21: " << it->name << " - moving to high priority!\n";
        
        // Splice single element - O(1)!
        start = std::chrono::high_resolution_clock::now();
        high_priority.splice(high_priority.end(), normal_priority, it);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }
    
    std::cout << "\nAFTER splice(high.end(), normal, iterator):\n";
    std::cout << "  High priority: " << high_priority.size() << " tasks\n";
    for (const auto& t : high_priority) {
        std::cout << "    Task " << t.id << ": " << t.name << " (priority: " << t.priority << ")\n";
    }
    std::cout << "  Normal priority: " << normal_priority.size() << " tasks\n";
    for (const auto& t : normal_priority) {
        std::cout << "    Task " << t.id << ": " << t.name << " (priority: " << t.priority << ")\n";
    }
    std::cout << "  ⏱ Time taken: " << duration.count() << " ns (O(1) - constant time!)\n";
    std::cout << "  ✓ Task moved instantly without copying!\n\n";
    
    // =======================================================================
    // EXAMPLE 3: splice() range of elements - O(n) where n = range size
    // =======================================================================
    std::cout << std::string(80, '-') << "\n";
    std::cout << "EXAMPLE 3: Splice Range of Elements - O(n) but NO Allocation\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::list<int> source = {10, 20, 30, 40, 50, 60, 70, 80};
    std::list<int> dest = {1, 2, 3};
    
    std::cout << "BEFORE splice():\n";
    std::cout << "  Source: ";
    for (int v : source) std::cout << v << " ";
    std::cout << "\n  Dest: ";
    for (int v : dest) std::cout << v << " ";
    std::cout << "\n";
    
    // Move elements 30, 40, 50 from source to dest
    auto range_start = std::find(source.begin(), source.end(), 30);
    auto range_end = std::find(source.begin(), source.end(), 60);  // One past last element
    
    std::cout << "\n  Moving range [30, 40, 50] from source to dest...\n";
    
    // Splice range - O(n) where n = distance(range_start, range_end)
    // But still NO memory allocation!
    start = std::chrono::high_resolution_clock::now();
    dest.splice(dest.end(), source, range_start, range_end);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    std::cout << "\nAFTER splice(dest.end(), source, first, last):\n";
    std::cout << "  Source: ";
    for (int v : source) std::cout << v << " ";
    std::cout << "\n  Dest: ";
    for (int v : dest) std::cout << v << " ";
    std::cout << "\n";
    std::cout << "  ⏱ Time taken: " << duration.count() << " ns (O(n) but predictable!)\n";
    std::cout << "  ✓ NO allocation - just pointer updates for range!\n\n";
    
    // =======================================================================
    // EXAMPLE 4: Real-time pool management with splice()
    // =======================================================================
    std::cout << std::string(80, '-') << "\n";
    std::cout << "EXAMPLE 4: Free List Pool Management - Real-Time Pattern\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    std::cout << "SCENARIO: Object pool using splice() for O(1) allocation/deallocation\n\n";
    
    struct Buffer {
        int id;
        std::array<char, 256> data;
        explicit Buffer(int i) : id(i), data{} {}
    };
    
    // Free pool (available buffers)
    std::list<Buffer> free_pool;
    for (int i = 0; i < 5; ++i) {
        free_pool.emplace_back(i);
    }
    
    // Active pool (in-use buffers)
    std::list<Buffer> active_pool;
    
    std::cout << "Initial state:\n";
    std::cout << "  Free pool: " << free_pool.size() << " buffers\n";
    std::cout << "  Active pool: " << active_pool.size() << " buffers\n\n";
    
    // Allocate buffer from free pool - O(1)
    std::cout << "Allocating 3 buffers from free pool...\n";
    for (int i = 0; i < 3; ++i) {
        auto it = free_pool.begin();
        std::cout << "  Allocating buffer " << it->id << "\n";
        active_pool.splice(active_pool.end(), free_pool, it);  // O(1)!
    }
    
    std::cout << "\nAfter allocation:\n";
    std::cout << "  Free pool: " << free_pool.size() << " buffers\n";
    std::cout << "  Active pool: " << active_pool.size() << " buffers (";
    for (const auto& b : active_pool) std::cout << b.id << " ";
    std::cout << ")\n\n";
    
    // Deallocate buffer back to free pool - O(1)
    std::cout << "Deallocating buffer 1 back to free pool...\n";
    auto to_free = std::find_if(active_pool.begin(), active_pool.end(),
                                [](const Buffer& b) { return b.id == 1; });
    if (to_free != active_pool.end()) {
        free_pool.splice(free_pool.end(), active_pool, to_free);  // O(1)!
        std::cout << "  Buffer 1 returned to free pool\n";
    }
    
    std::cout << "\nFinal state:\n";
    std::cout << "  Free pool: " << free_pool.size() << " buffers\n";
    std::cout << "  Active pool: " << active_pool.size() << " buffers (";
    for (const auto& b : active_pool) std::cout << b.id << " ";
    std::cout << ")\n\n";
    
    std::cout << "  ✓ Pool allocation/deallocation in O(1) with splice()!\n";
    std::cout << "  ✓ NO dynamic allocation - perfect for real-time systems!\n\n";
    
    // =======================================================================
    // KEY TAKEAWAYS
    // =======================================================================
    std::cout << std::string(80, '=') << "\n";
    std::cout << "KEY TAKEAWAYS: Why splice() is Essential for Real-Time C++\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    std::cout << "1. PERFORMANCE:\n";
    std::cout << "   • splice() entire list: O(1) - constant time\n";
    std::cout << "   • splice() single element: O(1) - constant time\n";
    std::cout << "   • splice() range: O(n) where n = range size (but predictable!)\n\n";
    
    std::cout << "2. MEMORY BEHAVIOR:\n";
    std::cout << "   • NO memory allocation - just pointer manipulation\n";
    std::cout << "   • NO copying or moving - elements stay in original memory\n";
    std::cout << "   • Deterministic and bounded - perfect for hard real-time\n\n";
    
    std::cout << "3. ITERATOR VALIDITY:\n";
    std::cout << "   • All iterators, pointers, and references remain valid\n";
    std::cout << "   • Safe to cache iterators across splice operations\n\n";
    
    std::cout << "4. REAL-TIME USE CASES:\n";
    std::cout << "   • Free list / object pool management\n";
    std::cout << "   • Task queue reordering (priority changes)\n";
    std::cout << "   • Event scheduling and rescheduling\n";
    std::cout << "   • Moving data between priority queues\n";
    std::cout << "   • Load balancing across worker queues\n\n";
    
    std::cout << "5. ALTERNATIVES ARE WORSE:\n";
    std::cout << "   • std::copy() + erase(): Requires copying, O(n) erase\n";
    std::cout << "   • std::move() + erase(): Still requires move, O(n) erase\n";
    std::cout << "   • Manual reallocation: Unpredictable timing, allocation overhead\n";
    std::cout << "   • splice() is THE optimal solution for list element movement!\n\n";
    
    std::cout << "⚡ GOLDEN RULE: Use splice() when you need to move elements\n";
    std::cout << "   between std::list containers in real-time critical code!\n";
}

} // namespace list_splice_realtime

// ============================================================================
// SECTION 4: Memory Management - RAII and Custom Allocators
// ============================================================================

namespace memory_management {

// RAII (Resource Acquisition Is Initialization)
// - Resources acquired in constructor
// - Resources released in destructor
// - Deterministic, exception-safe cleanup
// - No manual memory management needed

// Example: Real-time resource manager using RAII
template<typename T, size_t PoolSize>
class MemoryPool {
private:
    std::array<T, PoolSize> pool_;
    std::array<bool, PoolSize> in_use_;
    
public:
    MemoryPool() {
        in_use_.fill(false);
        std::cout << "  Memory pool created: " << PoolSize << " objects of size " 
                  << sizeof(T) << " bytes\n";
        std::cout << "  Total pool size: " << (PoolSize * sizeof(T)) << " bytes\n";
    }
    
    // Acquire object from pool - O(n) but bounded by PoolSize
    [[nodiscard]] T* acquire() noexcept {
        for (size_t i = 0; i < PoolSize; ++i) {
            if (!in_use_[i]) {
                in_use_[i] = true;
                return &pool_[i];
            }
        }
        return nullptr;  // Pool exhausted
    }
    
    // Release object back to pool - O(1)
    void release(T* ptr) noexcept {
        if (!ptr) return;
        
        // Find the object in pool
        for (size_t i = 0; i < PoolSize; ++i) {
            if (&pool_[i] == ptr) {
                in_use_[i] = false;
                return;
            }
        }
    }
    
    [[nodiscard]] size_t available() const noexcept {
        return std::count(in_use_.begin(), in_use_.end(), false);
    }
};

// RAII wrapper for pool-allocated object
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
            deleter_(ptr_);
        }
    }
    
    // Delete copy operations
    PoolPtr(const PoolPtr&) = delete;
    PoolPtr& operator=(const PoolPtr&) = delete;
    
    // Move operations
    PoolPtr(PoolPtr&& other) noexcept 
        : ptr_(other.ptr_), deleter_(std::move(other.deleter_)) {
        other.ptr_ = nullptr;
    }
    
    PoolPtr& operator=(PoolPtr&& other) noexcept {
        if (this != &other) {
            if (ptr_ && deleter_) {
                deleter_(ptr_);
            }
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    T* get() noexcept { return ptr_; }
    const T* get() const noexcept { return ptr_; }
    
    T& operator*() noexcept { return *ptr_; }
    const T& operator*() const noexcept { return *ptr_; }
    
    T* operator->() noexcept { return ptr_; }
    const T* operator->() const noexcept { return ptr_; }
    
    explicit operator bool() const noexcept { return ptr_ != nullptr; }
};

struct SensorData {
    double temperature;
    double pressure;
    uint64_t timestamp;
    
    SensorData() : temperature(0.0), pressure(0.0), timestamp(0) {}
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 4: Memory Management - RAII and Memory Pools ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "REAL-TIME MEMORY MANAGEMENT PRINCIPLES:\n\n";
    
    std::cout << "✓ DO:\n";
    std::cout << "  1. Pre-allocate all memory during initialization\n";
    std::cout << "  2. Use memory pools for frequent alloc/dealloc\n";
    std::cout << "  3. Use RAII for deterministic resource cleanup\n";
    std::cout << "  4. Prefer stack allocation over heap\n";
    std::cout << "  5. Use std::unique_ptr for ownership (minimal overhead)\n\n";
    
    std::cout << "✗ DON'T:\n";
    std::cout << "  1. Call new/delete in time-critical paths\n";
    std::cout << "  2. Use std::shared_ptr in tight loops (atomic overhead)\n";
    std::cout << "  3. Allow unbounded memory growth\n";
    std::cout << "  4. Rely on garbage collection\n";
    std::cout << "  5. Use dynamic allocation with unpredictable size\n\n";
    
    std::cout << "EXAMPLE: Memory Pool with RAII\n";
    
    // Create memory pool (during initialization phase)
    MemoryPool<SensorData, 100> sensor_pool;
    
    std::cout << "  Available slots: " << sensor_pool.available() << "\n\n";
    
    // Acquire objects from pool (during real-time operation)
    {
        auto sensor1 = PoolPtr<SensorData>(
            sensor_pool.acquire(),
            [&sensor_pool](SensorData* ptr) { sensor_pool.release(ptr); }
        );
        
        auto sensor2 = PoolPtr<SensorData>(
            sensor_pool.acquire(),
            [&sensor_pool](SensorData* ptr) { sensor_pool.release(ptr); }
        );
        
        if (sensor1 && sensor2) {
            sensor1->temperature = 25.5;
            sensor1->pressure = 1013.25;
            
            std::cout << "  Acquired 2 objects from pool\n";
            std::cout << "  Available slots: " << sensor_pool.available() << "\n";
            std::cout << "  Sensor 1 temp: " << sensor1->temperature << "°C\n";
        }
        
        // Objects automatically released when PoolPtr goes out of scope (RAII!)
        std::cout << "  Exiting scope...\n";
    }
    
    std::cout << "  Objects released automatically via RAII\n";
    std::cout << "  Available slots: " << sensor_pool.available() << "\n\n";
    
    std::cout << "  ✓ DETERMINISTIC: No heap fragmentation, predictable timing!\n";
    std::cout << "  ✓ SAFE: RAII ensures resources are always released!\n";
}

} // namespace memory_management

// ============================================================================
// SECTION 5: Modern C++ Features for Real-Time Systems
// ============================================================================

namespace modern_cpp_features {

// C++17 std::string_view - Zero-copy string viewing
void process_command(std::string_view cmd) noexcept {
    // No string copy - just a view into existing data
    if (cmd == "START") {
        std::cout << "    Starting system...\n";
    } else if (cmd == "STOP") {
        std::cout << "    Stopping system...\n";
    }
}

// C++17 std::optional - Express optional values without exceptions
[[nodiscard]] std::optional<double> safe_divide(double a, double b) noexcept {
    if (b == 0.0) {
        return std::nullopt;  // No exception thrown!
    }
    return a / b;
}

// C++17 std::variant - Type-safe union without heap allocation
using SensorValue = std::variant<int, double, std::array<uint8_t, 4>>;

void process_sensor_value(const SensorValue& value) noexcept {
    std::visit([](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            std::cout << "    Integer sensor: " << val << "\n";
        } else if constexpr (std::is_same_v<T, double>) {
            std::cout << "    Double sensor: " << val << "\n";
        } else {
            std::cout << "    Array sensor\n";
        }
    }, value);
}

// constexpr functions - Compile-time computation
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

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 5: Modern C++ Features for Real-Time ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "MODERN C++ FEATURES SUITABLE FOR REAL-TIME:\n\n";
    
    // std::string_view
    std::cout << "1. std::string_view (C++17) - Zero-copy string handling:\n";
    std::string command = "START";
    process_command(command);  // No copy, just a view
    process_command("STOP");   // Works with string literals too
    std::cout << "    ✓ No allocation, no copy, just a pointer + size\n\n";
    
    // std::optional
    std::cout << "2. std::optional (C++17) - Safe error handling without exceptions:\n";
    auto result1 = safe_divide(10.0, 2.0);
    auto result2 = safe_divide(10.0, 0.0);
    
    std::cout << "    10.0 / 2.0 = ";
    if (result1) {
        std::cout << *result1 << " (valid)\n";
    }
    
    std::cout << "    10.0 / 0.0 = ";
    if (result2) {
        std::cout << *result2 << "\n";
    } else {
        std::cout << "error (nullopt)\n";
    }
    std::cout << "    ✓ No exceptions, no heap allocation\n\n";
    
    // std::variant
    std::cout << "3. std::variant (C++17) - Type-safe union without heap:\n";
    SensorValue val1 = 42;
    SensorValue val2 = 3.14159;
    process_sensor_value(val1);
    process_sensor_value(val2);
    std::cout << "    ✓ Type-safe, no heap allocation, no virtual dispatch\n\n";
    
    // constexpr
    std::cout << "4. constexpr - Compile-time computation:\n";
    std::cout << "    Header CRC32 (computed at compile-time): 0x" 
              << std::hex << std::uppercase << header_crc << std::dec << "\n";
    std::cout << "    ✓ Zero runtime cost - computed by compiler!\n\n";
    
    // std::array
    std::cout << "5. std::array - Type-safe stack array:\n";
    std::array<double, 10> samples{1.0, 2.0, 3.0, 4.0, 5.0};
    std::cout << "    Array size: " << samples.size() << "\n";
    std::cout << "    ✓ Stack-allocated, compile-time size, bounds checking\n\n";
    
    std::cout << "FEATURES TO AVOID IN HARD REAL-TIME:\n";
    std::cout << "  ✗ Exceptions (use -fno-exceptions)\n";
    std::cout << "  ✗ RTTI (use -fno-rtti)\n";
    std::cout << "  ✗ Virtual functions in time-critical paths\n";
    std::cout << "  ✗ std::shared_ptr (atomic ref counting overhead)\n";
    std::cout << "  ✗ Dynamic polymorphism (prefer compile-time polymorphism)\n";
}

} // namespace modern_cpp_features

// ============================================================================
// SECTION 6: Real-Time System Checklist
// ============================================================================

void print_realtime_checklist() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 6: Real-Time Programming Checklist ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "INITIALIZATION PHASE (Non-real-time):\n";
    std::cout << "  ☐ Pre-allocate all memory (reserve() for vectors)\n";
    std::cout << "  ☐ Create memory pools for frequent allocations\n";
    std::cout << "  ☐ Load and sort lookup tables\n";
    std::cout << "  ☐ Initialize all data structures to max size\n";
    std::cout << "  ☐ Warm up caches if applicable\n";
    std::cout << "  ☐ Set thread priorities and CPU affinity\n\n";
    
    std::cout << "REAL-TIME EXECUTION PHASE:\n";
    std::cout << "  ☐ NO dynamic memory allocation (no new/delete/malloc/free)\n";
    std::cout << "  ☐ NO operations with unbounded execution time\n";
    std::cout << "  ☐ NO file I/O or blocking system calls\n";
    std::cout << "  ☐ NO unbounded loops or recursion\n";
    std::cout << "  ☐ NO exceptions in critical paths\n";
    std::cout << "  ☐ Use O(1) or O(log n) operations only\n";
    std::cout << "  ☐ All containers have bounded maximum size\n";
    std::cout << "  ☐ Use std::bitset instead of std::vector<bool>\n";
    std::cout << "  ☐ Use std::array when size known at compile-time\n";
    std::cout << "  ☐ Use RAII for deterministic cleanup\n\n";
    
    std::cout << "CONTAINER SELECTION:\n";
    std::cout << "  ✓ std::array<T, N>           - Fixed size, stack-based\n";
    std::cout << "  ✓ std::vector<T> + reserve() - Dynamic, pre-allocated\n";
    std::cout << "  ✓ std::bitset<N>             - Flags and bit manipulation\n";
    std::cout << "  ✓ std::string_view           - Zero-copy string viewing\n";
    std::cout << "  ⚠ std::deque<T>              - If size bounded\n";
    std::cout << "  ✗ std::list<T>               - Poor cache, heap per element\n";
    std::cout << "  ✗ std::vector<bool>          - Use std::bitset instead\n\n";
    
    std::cout << "ALGORITHM COMPLEXITY TARGETS:\n";
    std::cout << "  ✓ O(1)       - Ideal (array access, bitset operations)\n";
    std::cout << "  ✓ O(log n)   - Good (binary search, balanced trees)\n";
    std::cout << "  ⚠ O(n)       - Acceptable if n is bounded and small\n";
    std::cout << "  ✗ O(n log n) - Avoid in critical paths (sort)\n";
    std::cout << "  ✗ O(n²)      - Never (nested loops, bubble sort)\n\n";
    
    std::cout << "COMPILER FLAGS FOR REAL-TIME:\n";
    std::cout << "  -fno-exceptions    Disable exception handling\n";
    std::cout << "  -fno-rtti          Disable run-time type information\n";
    std::cout << "  -O3                Maximum optimization\n";
    std::cout << "  -march=native      Target specific CPU architecture\n";
    std::cout << "  -flto              Link-time optimization\n\n";
    
    std::cout << "PROFILING AND VERIFICATION:\n";
    std::cout << "  ☐ Measure worst-case execution time (WCET)\n";
    std::cout << "  ☐ Use real-time profiling tools\n";
    std::cout << "  ☐ Test under worst-case conditions\n";
    std::cout << "  ☐ Verify no unexpected allocations\n";
    std::cout << "  ☐ Monitor stack usage\n";
    std::cout << "  ☐ Validate timing constraints are met\n";
}

// ============================================================================
// Main Function
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                 REAL-TIME PROGRAMMING IN MODERN C++                  ║\n";
    std::cout << "║                                                                      ║\n";
    std::cout << "║  Comprehensive guide to deterministic, high-performance C++ code    ║\n";
    std::cout << "║  for embedded systems, robotics, and time-critical applications     ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════════╝\n";
    
    try {
        // Section 1: Big O Notation
        big_o_notation::demonstrate();
        
        // Section 1.5: Thread Architecture
        thread_architecture::demonstrate();
        
        // Section 2: std::bitset vs std::vector<bool>
        bitset_vs_vector_bool::demonstrate();
        
        // Section 3: STL Containers
        stl_containers_realtime::demonstrate();
        
        // Section 3.5: std::list::splice() - O(1) Element Movement
        list_splice_realtime::demonstrate();
        
        // Section 4: Memory Management
        memory_management::demonstrate();
        
        // Section 5: Modern C++ Features
        modern_cpp_features::demonstrate();
        
        // Section 6: Checklist
        print_realtime_checklist();
        
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "=== END OF REAL-TIME PROGRAMMING GUIDE ===\n";
        std::cout << std::string(70, '=') << "\n\n";
        
        std::cout << "KEY TAKEAWAYS:\n";
        std::cout << "  1. Always pre-allocate memory during initialization\n";
        std::cout << "  2. Use std::bitset instead of std::vector<bool>\n";
        std::cout << "  3. Prefer O(1) operations, accept O(log n), bound O(n)\n";
        std::cout << "  4. Use reserve() for std::vector to prevent reallocations\n";
        std::cout << "  5. std::array is perfect for fixed-size data\n";
        std::cout << "  6. RAII provides deterministic resource management\n";
        std::cout << "  7. Modern C++ features like std::optional and std::variant\n";
        std::cout << "     are real-time friendly (no exceptions, no heap)\n";
        std::cout << "  8. Measure WCET, not average performance!\n\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
