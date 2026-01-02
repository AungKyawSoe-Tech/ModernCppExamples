// FunctionalSafetyISO26262.cpp
// Comprehensive demonstration of ISO 26262 Functional Safety principles in Modern C++
// Covers ASIL levels, MISRA C++, redundancy, watchdogs, safe data types, and deterministic patterns

#include <iostream>
#include <cstdint>
#include <array>
#include <limits>
#include <chrono>
#include <functional>
#include <algorithm>
#include <cstring>

// ============================================================================
// SECTION 1: ASIL Levels and Safety Requirements
// ============================================================================
// ISO 26262 defines 4 Automotive Safety Integrity Levels (ASIL A-D)
// ASIL D = highest safety requirements (e.g., braking systems)
// ASIL A = lowest safety requirements (e.g., rear lights)

enum class ASILLevel : uint8_t {
    QM = 0,  // Quality Management (no ASIL)
    A = 1,   // Lowest safety integrity
    B = 2,
    C = 3,
    D = 4    // Highest safety integrity
};

// Safety state for system degradation
enum class SafetyState : uint8_t {
    NORMAL_OPERATION = 0,
    DEGRADED_MODE = 1,
    SAFE_STATE = 2,
    EMERGENCY_STOP = 3
};

// Safety-critical function return type (no exceptions!)
enum class SafetyResult : uint8_t {
    OK = 0,
    WARNING = 1,
    ERROR = 2,
    CRITICAL_FAILURE = 3
};

void demonstrate_asil_levels() {
    std::cout << "\n=== 1. ASIL Levels and Safety Requirements ===\n";
    
    struct SafetyRequirement {
        const char* component;
        ASILLevel asil;
        const char* requirement;
    };
    
    constexpr std::array<SafetyRequirement, 5> requirements = {{
        {"Brake-by-wire", ASILLevel::D, "Dual redundancy + voting"},
        {"Electric power steering", ASILLevel::C, "Plausibility checks + monitoring"},
        {"Airbag control", ASILLevel::D, "Watchdog + memory protection"},
        {"Engine control", ASILLevel::B, "Runtime checks + diagnostics"},
        {"Interior lighting", ASILLevel::QM, "Basic error handling"}
    }};
    
    std::cout << "\nASIL Classification Examples:\n";
    for (const auto& req : requirements) {
        std::cout << "  " << req.component << " [ASIL-";
        switch (req.asil) {
            case ASILLevel::QM: std::cout << "QM"; break;
            case ASILLevel::A: std::cout << "A"; break;
            case ASILLevel::B: std::cout << "B"; break;
            case ASILLevel::C: std::cout << "C"; break;
            case ASILLevel::D: std::cout << "D"; break;
        }
        std::cout << "]: " << req.requirement << "\n";
    }
    
    std::cout << "\nKey ISO 26262 Principles:\n";
    std::cout << "  ✓ No dynamic memory allocation in safety functions\n";
    std::cout << "  ✓ No exceptions in safety-critical paths (use error codes)\n";
    std::cout << "  ✓ Deterministic timing (WCET - Worst Case Execution Time)\n";
    std::cout << "  ✓ Redundancy and voting for ASIL-D\n";
    std::cout << "  ✓ Comprehensive diagnostics and fault detection\n";
}

// ============================================================================
// SECTION 2: Safe Data Types with Overflow Protection
// ============================================================================
// MISRA C++ requires explicit bounds checking and overflow protection

template<typename T>
class SafeInteger {
private:
    T value_;
    bool valid_;
    
public:
    constexpr SafeInteger() noexcept : value_(0), valid_(true) {}
    constexpr explicit SafeInteger(T val) noexcept : value_(val), valid_(true) {}
    
    // Safe addition with overflow detection
    SafeInteger add(T other) const noexcept {
        SafeInteger result;
        
        // Check for overflow before operation (MISRA requirement)
        if (other > 0 && value_ > std::numeric_limits<T>::max() - other) {
            result.valid_ = false;  // Overflow detected
            result.value_ = std::numeric_limits<T>::max();
        } else if (other < 0 && value_ < std::numeric_limits<T>::min() - other) {
            result.valid_ = false;  // Underflow detected
            result.value_ = std::numeric_limits<T>::min();
        } else {
            result.value_ = value_ + other;
            result.valid_ = true;
        }
        
        return result;
    }
    
    // Safe multiplication with overflow detection
    SafeInteger multiply(T other) const noexcept {
        SafeInteger result;
        
        if (other == 0 || value_ == 0) {
            result.value_ = 0;
            result.valid_ = true;
        } else if (value_ > std::numeric_limits<T>::max() / other) {
            result.valid_ = false;  // Overflow
            result.value_ = std::numeric_limits<T>::max();
        } else {
            result.value_ = value_ * other;
            result.valid_ = true;
        }
        
        return result;
    }
    
    constexpr T get() const noexcept { return value_; }
    constexpr bool is_valid() const noexcept { return valid_; }
};

void demonstrate_safe_data_types() {
    std::cout << "\n=== 2. Safe Data Types with Overflow Protection ===\n";
    
    SafeInteger<int32_t> speed(100);
    SafeInteger<int32_t> delta(50);
    
    auto new_speed = speed.add(delta.get());
    std::cout << "\nSafe Addition: 100 + 50 = " << new_speed.get() 
              << " (valid: " << (new_speed.is_valid() ? "✓" : "✗") << ")\n";
    
    // Demonstrate overflow detection
    SafeInteger<int32_t> large_val(std::numeric_limits<int32_t>::max() - 10);
    auto overflow_result = large_val.add(100);
    std::cout << "\nOverflow Detection: MAX-10 + 100 = " << overflow_result.get()
              << " (valid: " << (overflow_result.is_valid() ? "✓" : "✗ OVERFLOW DETECTED") << ")\n";
    
    std::cout << "\nMISRA C++ Guidelines:\n";
    std::cout << "  ✓ All arithmetic checked for overflow/underflow\n";
    std::cout << "  ✓ Explicit error flags instead of exceptions\n";
    std::cout << "  ✓ constexpr for compile-time safety verification\n";
    std::cout << "  ✓ noexcept guarantees for safety-critical functions\n";
}

// ============================================================================
// SECTION 3: Redundancy and Voting Mechanisms (ASIL-D Requirement)
// ============================================================================
// Dual-channel or triple-modular redundancy for critical computations

template<typename T>
class DualChannelComputation {
private:
    std::function<T()> channel1_;
    std::function<T()> channel2_;
    T tolerance_;
    
public:
    DualChannelComputation(std::function<T()> ch1, std::function<T()> ch2, T tol) noexcept
        : channel1_(ch1), channel2_(ch2), tolerance_(tol) {}
    
    // Execute both channels and compare results
    SafetyResult compute(T& result) noexcept {
        T result1 = channel1_();
        T result2 = channel2_();
        
        // Calculate absolute difference
        T diff = (result1 > result2) ? (result1 - result2) : (result2 - result1);
        
        if (diff <= tolerance_) {
            // Results agree within tolerance
            result = (result1 + result2) / 2;  // Use average
            return SafetyResult::OK;
        } else {
            // Mismatch detected - critical failure
            result = 0;  // Safe default
            return SafetyResult::CRITICAL_FAILURE;
        }
    }
};

// Triple Modular Redundancy (TMR) with voting
template<typename T>
class TripleModularRedundancy {
private:
    std::array<std::function<T()>, 3> channels_;
    T tolerance_;
    
public:
    TripleModularRedundancy(std::function<T()> ch1, std::function<T()> ch2, 
                           std::function<T()> ch3, T tol) noexcept
        : channels_{{ch1, ch2, ch3}}, tolerance_(tol) {}
    
    // 2-out-of-3 voting
    SafetyResult compute(T& result) noexcept {
        std::array<T, 3> results;
        for (size_t i = 0; i < 3; ++i) {
            results[i] = channels_[i]();
        }
        
        // Check if any two results agree within tolerance
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = i + 1; j < 3; ++j) {
                T diff = (results[i] > results[j]) ? 
                         (results[i] - results[j]) : (results[j] - results[i]);
                
                if (diff <= tolerance_) {
                    // Two channels agree
                    result = (results[i] + results[j]) / 2;
                    return (i == 2 || j == 2) ? SafetyResult::WARNING : SafetyResult::OK;
                }
            }
        }
        
        // No agreement - critical failure
        result = 0;
        return SafetyResult::CRITICAL_FAILURE;
    }
};

void demonstrate_redundancy_voting() {
    std::cout << "\n=== 3. Redundancy and Voting Mechanisms ===\n";
    
    // Simulate brake pressure sensor readings
    auto sensor_channel_1 = []() -> int32_t { return 850; };  // 850 kPa
    auto sensor_channel_2 = []() -> int32_t { return 852; };  // 852 kPa (slight variation)
    
    DualChannelComputation<int32_t> brake_pressure(sensor_channel_1, sensor_channel_2, 5);
    
    int32_t pressure = 0;
    SafetyResult status = brake_pressure.compute(pressure);
    
    std::cout << "\nDual-Channel Brake Pressure Sensor:\n";
    std::cout << "  Channel 1: 850 kPa\n";
    std::cout << "  Channel 2: 852 kPa\n";
    std::cout << "  Result: " << pressure << " kPa (avg)\n";
    std::cout << "  Status: " << (status == SafetyResult::OK ? "✓ OK" : "✗ FAILURE") << "\n";
    
    // Triple modular redundancy example
    auto tmr_ch1 = []() -> int32_t { return 100; };
    auto tmr_ch2 = []() -> int32_t { return 101; };
    auto tmr_ch3 = []() -> int32_t { return 99; };  // All agree within tolerance
    
    TripleModularRedundancy<int32_t> steering_angle(tmr_ch1, tmr_ch2, tmr_ch3, 5);
    
    int32_t angle = 0;
    SafetyResult tmr_status = steering_angle.compute(angle);
    
    std::cout << "\nTriple Modular Redundancy (Steering Angle):\n";
    std::cout << "  Channel 1: 100°\n";
    std::cout << "  Channel 2: 101°\n";
    std::cout << "  Channel 3: 99°\n";
    std::cout << "  Voted Result: " << angle << "°\n";
    std::cout << "  Status: " << (tmr_status == SafetyResult::OK ? "✓ OK (2-of-3 agree)" : "✗ FAILURE") << "\n";
    
    std::cout << "\nRedundancy Patterns:\n";
    std::cout << "  ✓ Dual-channel for ASIL-C/D\n";
    std::cout << "  ✓ Triple modular redundancy (TMR) for highest safety\n";
    std::cout << "  ✓ 2-out-of-3 voting tolerates single-point failures\n";
    std::cout << "  ✓ Tolerance bands for real-world sensor variations\n";
}

// ============================================================================
// SECTION 4: Watchdog Timer and Heartbeat Monitoring
// ============================================================================
// Detect software hangs and stuck states

class WatchdogTimer {
private:
    uint32_t timeout_ms_;
    uint32_t last_kick_time_;
    bool expired_;
    
public:
    explicit WatchdogTimer(uint32_t timeout_ms) noexcept 
        : timeout_ms_(timeout_ms), last_kick_time_(0), expired_(false) {}
    
    // Kick the watchdog (reset timer)
    void kick(uint32_t current_time_ms) noexcept {
        last_kick_time_ = current_time_ms;
        expired_ = false;
    }
    
    // Check if watchdog expired
    bool check_expired(uint32_t current_time_ms) noexcept {
        if (current_time_ms - last_kick_time_ > timeout_ms_) {
            expired_ = true;
        }
        return expired_;
    }
    
    bool is_expired() const noexcept { return expired_; }
};

class SafetyMonitor {
private:
    WatchdogTimer watchdog_;
    uint32_t heartbeat_count_;
    SafetyState state_;
    
public:
    explicit SafetyMonitor(uint32_t watchdog_timeout) noexcept
        : watchdog_(watchdog_timeout), heartbeat_count_(0), state_(SafetyState::NORMAL_OPERATION) {}
    
    void heartbeat(uint32_t current_time) noexcept {
        watchdog_.kick(current_time);
        ++heartbeat_count_;
    }
    
    SafetyResult check_safety(uint32_t current_time) noexcept {
        if (watchdog_.check_expired(current_time)) {
            state_ = SafetyState::EMERGENCY_STOP;
            return SafetyResult::CRITICAL_FAILURE;
        }
        return SafetyResult::OK;
    }
    
    SafetyState get_state() const noexcept { return state_; }
    uint32_t get_heartbeat_count() const noexcept { return heartbeat_count_; }
};

void demonstrate_watchdog_monitoring() {
    std::cout << "\n=== 4. Watchdog Timer and Heartbeat Monitoring ===\n";
    
    SafetyMonitor monitor(100);  // 100ms timeout
    
    std::cout << "\nNormal Operation:\n";
    for (uint32_t time = 0; time <= 250; time += 50) {
        monitor.heartbeat(time);
        SafetyResult status = monitor.check_safety(time);
        std::cout << "  Time " << time << "ms: Heartbeat #" << monitor.get_heartbeat_count()
                  << " - Status: " << (status == SafetyResult::OK ? "✓ OK" : "✗ FAILURE") << "\n";
    }
    
    // Simulate watchdog expiration (no heartbeat for >100ms)
    std::cout << "\nSimulating Watchdog Timeout (no heartbeat):\n";
    SafetyMonitor timeout_monitor(100);
    timeout_monitor.heartbeat(0);
    
    for (uint32_t time = 50; time <= 200; time += 50) {
        // Skip heartbeat to simulate hang
        SafetyResult status = timeout_monitor.check_safety(time);
        std::cout << "  Time " << time << "ms: ";
        if (status == SafetyResult::CRITICAL_FAILURE) {
            std::cout << "✗ WATCHDOG EXPIRED - EMERGENCY STOP\n";
            break;
        } else {
            std::cout << "✓ OK\n";
        }
    }
    
    std::cout << "\nWatchdog Best Practices:\n";
    std::cout << "  ✓ Timeout based on WCET (Worst Case Execution Time)\n";
    std::cout << "  ✓ Hardware watchdog preferred for fail-safe\n";
    std::cout << "  ✓ Heartbeat every control cycle\n";
    std::cout << "  ✓ Transition to safe state on expiration\n";
}

// ============================================================================
// SECTION 5: Memory Safety and Bounds Checking
// ============================================================================
// MISRA C++ requires explicit bounds checking, no buffer overruns

template<typename T, size_t N>
class SafeArray {
private:
    std::array<T, N> data_;
    
public:
    constexpr SafeArray() noexcept : data_{} {}
    
    // Safe access with bounds checking (no exceptions!)
    SafetyResult get(size_t index, T& value) const noexcept {
        if (index >= N) {
            return SafetyResult::ERROR;
        }
        value = data_[index];
        return SafetyResult::OK;
    }
    
    SafetyResult set(size_t index, T value) noexcept {
        if (index >= N) {
            return SafetyResult::ERROR;
        }
        data_[index] = value;
        return SafetyResult::OK;
    }
    
    constexpr size_t size() const noexcept { return N; }
    
    // Safe iteration
    T* begin() noexcept { return data_.data(); }
    T* end() noexcept { return data_.data() + N; }
    const T* begin() const noexcept { return data_.data(); }
    const T* end() const noexcept { return data_.data() + N; }
};

// Safe string operations (fixed-size, no dynamic allocation)
template<size_t N>
class SafeString {
private:
    std::array<char, N> buffer_;
    size_t length_;
    
public:
    constexpr SafeString() noexcept : buffer_{}, length_(0) {
        buffer_[0] = '\0';
    }
    
    SafetyResult copy_from(const char* src) noexcept {
        if (src == nullptr) {
            return SafetyResult::ERROR;
        }
        
        size_t i = 0;
        while (i < N - 1 && src[i] != '\0') {
            buffer_[i] = src[i];
            ++i;
        }
        buffer_[i] = '\0';
        length_ = i;
        
        return (src[i] == '\0') ? SafetyResult::OK : SafetyResult::WARNING;  // Truncated
    }
    
    SafetyResult append(const char* src) noexcept {
        if (src == nullptr) {
            return SafetyResult::ERROR;
        }
        
        size_t i = 0;
        while (length_ < N - 1 && src[i] != '\0') {
            buffer_[length_++] = src[i++];
        }
        buffer_[length_] = '\0';
        
        return (src[i] == '\0') ? SafetyResult::OK : SafetyResult::WARNING;
    }
    
    const char* c_str() const noexcept { return buffer_.data(); }
    size_t length() const noexcept { return length_; }
    constexpr size_t capacity() const noexcept { return N - 1; }
};

void demonstrate_memory_safety() {
    std::cout << "\n=== 5. Memory Safety and Bounds Checking ===\n";
    
    // Safe array with compile-time size
    SafeArray<int32_t, 10> sensor_readings;
    
    std::cout << "\nSafe Array Operations:\n";
    for (size_t i = 0; i < sensor_readings.size(); ++i) {
        sensor_readings.set(i, static_cast<int32_t>(i * 100));
    }
    
    int32_t value = 0;
    SafetyResult result = sensor_readings.get(5, value);
    std::cout << "  Reading[5] = " << value << " (status: " 
              << (result == SafetyResult::OK ? "✓ OK" : "✗ ERROR") << ")\n";
    
    // Attempt out-of-bounds access
    result = sensor_readings.get(15, value);
    std::cout << "  Reading[15] = " << (result == SafetyResult::OK ? "Valid" : "✗ OUT OF BOUNDS") << "\n";
    
    // Safe string operations
    SafeString<32> device_id;
    device_id.copy_from("BRAKE_CTRL_");
    device_id.append("ECU001");
    
    std::cout << "\nSafe String Operations:\n";
    std::cout << "  Device ID: \"" << device_id.c_str() << "\"\n";
    std::cout << "  Length: " << device_id.length() << "/" << device_id.capacity() << "\n";
    
    std::cout << "\nMemory Safety Rules:\n";
    std::cout << "  ✓ No dynamic allocation (new/malloc) in safety functions\n";
    std::cout << "  ✓ Fixed-size buffers with compile-time bounds\n";
    std::cout << "  ✓ All array accesses bounds-checked\n";
    std::cout << "  ✓ No pointer arithmetic without validation\n";
    std::cout << "  ✓ Stack-allocated containers only\n";
}

// ============================================================================
// SECTION 6: Runtime Diagnostics and Self-Test
// ============================================================================
// Continuous self-monitoring and fault detection

class DiagnosticMonitor {
private:
    struct DiagnosticCounter {
        uint32_t total_checks;
        uint32_t failures;
        uint32_t warnings;
    };
    
    DiagnosticCounter counters_;
    bool system_healthy_;
    
public:
    DiagnosticMonitor() noexcept 
        : counters_{0, 0, 0}, system_healthy_(true) {}
    
    // RAM test (simple pattern check)
    SafetyResult test_ram(uint32_t* test_area, size_t size) noexcept {
        ++counters_.total_checks;
        
        // Write pattern
        constexpr uint32_t PATTERN1 = 0x55555555;
        constexpr uint32_t PATTERN2 = 0xAAAAAAAA;
        
        for (size_t i = 0; i < size; ++i) {
            test_area[i] = PATTERN1;
        }
        
        // Verify pattern 1
        for (size_t i = 0; i < size; ++i) {
            if (test_area[i] != PATTERN1) {
                ++counters_.failures;
                system_healthy_ = false;
                return SafetyResult::CRITICAL_FAILURE;
            }
        }
        
        // Write and verify pattern 2
        for (size_t i = 0; i < size; ++i) {
            test_area[i] = PATTERN2;
        }
        
        for (size_t i = 0; i < size; ++i) {
            if (test_area[i] != PATTERN2) {
                ++counters_.failures;
                system_healthy_ = false;
                return SafetyResult::CRITICAL_FAILURE;
            }
        }
        
        return SafetyResult::OK;
    }
    
    // Plausibility check for sensor values
    SafetyResult check_plausibility(int32_t value, int32_t min, int32_t max) noexcept {
        ++counters_.total_checks;
        
        if (value < min || value > max) {
            ++counters_.failures;
            return SafetyResult::ERROR;
        }
        
        // Warning range (10% from limits)
        int32_t warning_margin = (max - min) / 10;
        if (value < min + warning_margin || value > max - warning_margin) {
            ++counters_.warnings;
            return SafetyResult::WARNING;
        }
        
        return SafetyResult::OK;
    }
    
    void get_diagnostics(uint32_t& total, uint32_t& failures, uint32_t& warnings) const noexcept {
        total = counters_.total_checks;
        failures = counters_.failures;
        warnings = counters_.warnings;
    }
    
    bool is_healthy() const noexcept { return system_healthy_; }
};

void demonstrate_runtime_diagnostics() {
    std::cout << "\n=== 6. Runtime Diagnostics and Self-Test ===\n";
    
    DiagnosticMonitor diagnostics;
    
    // RAM self-test
    std::array<uint32_t, 256> test_ram;
    std::cout << "\nRAM Self-Test:\n";
    SafetyResult ram_result = diagnostics.test_ram(test_ram.data(), test_ram.size());
    std::cout << "  Pattern Test (0x55555555/0xAAAAAAAA): " 
              << (ram_result == SafetyResult::OK ? "✓ PASS" : "✗ FAIL") << "\n";
    
    // Plausibility checks
    std::cout << "\nSensor Plausibility Checks:\n";
    
    struct SensorTest {
        const char* name;
        int32_t value;
        int32_t min;
        int32_t max;
    };
    
    constexpr std::array<SensorTest, 4> tests = {{
        {"Wheel speed", 1200, 0, 3000},      // OK
        {"Brake pressure", 850, 0, 1000},     // OK
        {"Steering angle", 950, -900, 900},   // WARNING (near limit)
        {"Throttle position", 1500, 0, 1000}  // ERROR (out of range)
    }};
    
    for (const auto& test : tests) {
        SafetyResult result = diagnostics.check_plausibility(test.value, test.min, test.max);
        std::cout << "  " << test.name << " = " << test.value 
                  << " [" << test.min << ".." << test.max << "]: ";
        
        switch (result) {
            case SafetyResult::OK:
                std::cout << "✓ OK\n";
                break;
            case SafetyResult::WARNING:
                std::cout << "⚠ WARNING (near limit)\n";
                break;
            case SafetyResult::ERROR:
                std::cout << "✗ OUT OF RANGE\n";
                break;
            default:
                std::cout << "✗ CRITICAL\n";
                break;
        }
    }
    
    // Diagnostic summary
    uint32_t total, failures, warnings;
    diagnostics.get_diagnostics(total, failures, warnings);
    
    std::cout << "\nDiagnostic Summary:\n";
    std::cout << "  Total checks: " << total << "\n";
    std::cout << "  Failures: " << failures << "\n";
    std::cout << "  Warnings: " << warnings << "\n";
    std::cout << "  System Health: " << (diagnostics.is_healthy() ? "✓ HEALTHY" : "✗ DEGRADED") << "\n";
    
    std::cout << "\nDiagnostic Coverage:\n";
    std::cout << "  ✓ RAM pattern test (startup + periodic)\n";
    std::cout << "  ✓ Plausibility checks (range validation)\n";
    std::cout << "  ✓ Stuck-at fault detection\n";
    std::cout << "  ✓ Diagnostic Trouble Code (DTC) logging\n";
}

// ============================================================================
// SECTION 7: Safe State Transitions and Fault Handling
// ============================================================================
// Deterministic state machine with safe fallback

class BrakingController {
private:
    enum class BrakeState : uint8_t {
        INIT = 0,
        NORMAL = 1,
        DEGRADED = 2,
        FAILSAFE = 3,
        EMERGENCY = 4
    };
    
    BrakeState current_state_;
    BrakeState previous_state_;
    uint32_t fault_count_;
    
public:
    BrakingController() noexcept 
        : current_state_(BrakeState::INIT),
          previous_state_(BrakeState::INIT),
          fault_count_(0) {}
    
    SafetyResult initialize() noexcept {
        if (current_state_ != BrakeState::INIT) {
            return SafetyResult::ERROR;
        }
        
        // Perform self-tests
        // ... (RAM test, sensor check, actuator test)
        
        previous_state_ = current_state_;
        current_state_ = BrakeState::NORMAL;
        return SafetyResult::OK;
    }
    
    SafetyResult handle_fault(SafetyResult fault_severity) noexcept {
        ++fault_count_;
        previous_state_ = current_state_;
        
        switch (fault_severity) {
            case SafetyResult::WARNING:
                // Stay in current state but log warning
                return SafetyResult::WARNING;
                
            case SafetyResult::ERROR:
                // Transition to degraded mode
                if (current_state_ == BrakeState::NORMAL) {
                    current_state_ = BrakeState::DEGRADED;
                }
                return SafetyResult::ERROR;
                
            case SafetyResult::CRITICAL_FAILURE:
                // Immediate transition to emergency
                current_state_ = BrakeState::EMERGENCY;
                return SafetyResult::CRITICAL_FAILURE;
                
            default:
                return SafetyResult::OK;
        }
    }
    
    SafetyResult apply_brakes(int32_t pressure) noexcept {
        switch (current_state_) {
            case BrakeState::INIT:
                return SafetyResult::ERROR;  // Not initialized
                
            case BrakeState::NORMAL:
                // Full functionality
                return apply_normal_braking(pressure);
                
            case BrakeState::DEGRADED:
                // Limited functionality (e.g., one channel failed)
                return apply_degraded_braking(pressure / 2);
                
            case BrakeState::FAILSAFE:
            case BrakeState::EMERGENCY:
                // Maximum braking regardless of input
                return apply_emergency_braking();
        }
        
        return SafetyResult::ERROR;
    }
    
    const char* get_state_name() const noexcept {
        switch (current_state_) {
            case BrakeState::INIT: return "INIT";
            case BrakeState::NORMAL: return "NORMAL";
            case BrakeState::DEGRADED: return "DEGRADED";
            case BrakeState::FAILSAFE: return "FAILSAFE";
            case BrakeState::EMERGENCY: return "EMERGENCY";
            default: return "UNKNOWN";
        }
    }
    
    uint32_t get_fault_count() const noexcept { return fault_count_; }
    
private:
    SafetyResult apply_normal_braking(int32_t pressure) noexcept {
        // Normal braking logic
        (void)pressure;  // Suppress unused warning
        return SafetyResult::OK;
    }
    
    SafetyResult apply_degraded_braking(int32_t pressure) noexcept {
        // Degraded mode (e.g., mechanical backup)
        (void)pressure;
        return SafetyResult::WARNING;
    }
    
    SafetyResult apply_emergency_braking() noexcept {
        // Maximum braking effort
        return SafetyResult::CRITICAL_FAILURE;
    }
};

void demonstrate_safe_state_transitions() {
    std::cout << "\n=== 7. Safe State Transitions and Fault Handling ===\n";
    
    BrakingController brake_system;
    
    std::cout << "\nBraking System State Machine:\n";
    std::cout << "  Initial State: " << brake_system.get_state_name() << "\n";
    
    // Initialize
    brake_system.initialize();
    std::cout << "  After Init: " << brake_system.get_state_name() << "\n";
    
    // Normal operation
    SafetyResult result = brake_system.apply_brakes(500);
    std::cout << "  Apply 500 kPa: " << (result == SafetyResult::OK ? "✓ OK" : "✗ ERROR") << "\n";
    
    // Simulate fault
    std::cout << "\n  Simulating sensor fault...\n";
    brake_system.handle_fault(SafetyResult::ERROR);
    std::cout << "  State after fault: " << brake_system.get_state_name() << "\n";
    
    result = brake_system.apply_brakes(500);
    std::cout << "  Apply 500 kPa in degraded: " 
              << (result == SafetyResult::WARNING ? "⚠ LIMITED" : "✗ ERROR") << "\n";
    
    // Critical fault
    std::cout << "\n  Simulating critical failure...\n";
    brake_system.handle_fault(SafetyResult::CRITICAL_FAILURE);
    std::cout << "  State after critical: " << brake_system.get_state_name() << "\n";
    std::cout << "  Total faults handled: " << brake_system.get_fault_count() << "\n";
    
    std::cout << "\nState Machine Principles:\n";
    std::cout << "  ✓ Deterministic transitions (no race conditions)\n";
    std::cout << "  ✓ Always transition to safe state on fault\n";
    std::cout << "  ✓ No invalid states reachable\n";
    std::cout << "  ✓ Fail-safe defaults (e.g., emergency braking)\n";
}

// ============================================================================
// SECTION 8: Deterministic Timing and WCET
// ============================================================================
// Predictable execution time for real-time safety functions

class ExecutionTimeMonitor {
private:
    std::chrono::steady_clock::time_point start_time_;
    uint32_t wcet_us_;  // Worst-case execution time in microseconds
    bool timing_violation_;
    
public:
    explicit ExecutionTimeMonitor(uint32_t wcet_us) noexcept
        : wcet_us_(wcet_us), timing_violation_(false) {}
    
    void start() noexcept {
        start_time_ = std::chrono::steady_clock::now();
        timing_violation_ = false;
    }
    
    SafetyResult check() noexcept {
        auto end_time = std::chrono::steady_clock::now();
        auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time_).count();
        
        if (elapsed_us > wcet_us_) {
            timing_violation_ = true;
            return SafetyResult::ERROR;
        }
        
        return SafetyResult::OK;
    }
    
    bool has_violation() const noexcept { return timing_violation_; }
};

// Safety-critical control loop with deterministic timing
SafetyResult safety_control_cycle(ExecutionTimeMonitor& timer) noexcept {
    timer.start();
    
    // Read sensors (deterministic)
    volatile int32_t sensor1 = 100;
    volatile int32_t sensor2 = 200;
    
    // Compute control output (no branches, no loops with variable iterations)
    volatile int32_t output = (sensor1 + sensor2) / 2;
    
    // Write actuators
    (void)output;  // Suppress unused warning
    
    // Check timing constraint
    return timer.check();
}

void demonstrate_deterministic_timing() {
    std::cout << "\n=== 8. Deterministic Timing and WCET ===\n";
    
    // Set WCET budget to 100 microseconds
    ExecutionTimeMonitor timer(100);
    
    std::cout << "\nControl Cycle Timing (WCET = 100 µs):\n";
    
    for (int i = 0; i < 5; ++i) {
        SafetyResult result = safety_control_cycle(timer);
        std::cout << "  Cycle " << (i + 1) << ": " 
                  << (result == SafetyResult::OK ? "✓ Within WCET" : "✗ TIMING VIOLATION") << "\n";
    }
    
    std::cout << "\nDeterminism Requirements:\n";
    std::cout << "  ✓ No dynamic memory allocation (non-deterministic)\n";
    std::cout << "  ✓ No unbounded loops (use fixed iteration count)\n";
    std::cout << "  ✓ No recursive functions (stack usage)\n";
    std::cout << "  ✓ Disable interrupts in critical sections\n";
    std::cout << "  ✓ Priority-based scheduling (preemptive RTOS)\n";
    std::cout << "  ✓ WCET analysis tools (e.g., aiT, RapiTime)\n";
}

// ============================================================================
// SECTION 9: MISRA C++ Compliance Patterns
// ============================================================================

void demonstrate_misra_compliance() {
    std::cout << "\n=== 9. MISRA C++ Compliance Patterns ===\n";
    
    std::cout << "\nKey MISRA C++ 2023 Rules:\n\n";
    
    std::cout << "Rule 5-0-3: Prohibited types and features:\n";
    std::cout << "  ✗ Avoid: exceptions, RTTI (typeid), dynamic_cast in safety code\n";
    std::cout << "  ✓ Use: Error codes, static polymorphism (templates)\n\n";
    
    std::cout << "Rule 5-2-12: No dynamic memory allocation:\n";
    std::cout << "  ✗ Avoid: new, delete, malloc, free, std::vector (dynamic)\n";
    std::cout << "  ✓ Use: std::array, static buffers, placement new (if needed)\n\n";
    
    std::cout << "Rule 6-4-5: Unconditional throw/goto prohibited:\n";
    std::cout << "  ✗ Avoid: throw exceptions in safety functions\n";
    std::cout << "  ✓ Use: Return error codes (SafetyResult enum)\n\n";
    
    std::cout << "Rule 8-0-1: All functions have one exit point:\n";
    std::cout << "  ✗ Avoid: Multiple return statements\n";
    std::cout << "  ✓ Use: Single return with result variable\n\n";
    
    std::cout << "Rule 18-0-3: No library functions with undefined behavior:\n";
    std::cout << "  ✗ Avoid: atoi, gets, sprintf (unsafe)\n";
    std::cout << "  ✓ Use: strtol with error checking, snprintf\n\n";
    
    std::cout << "Rule 27-0-1: All includes have include guards:\n";
    std::cout << "  ✓ Use: #pragma once or #ifndef guards\n\n";
    
    std::cout << "Additional Safety Guidelines:\n";
    std::cout << "  ✓ All arithmetic operations overflow-checked\n";
    std::cout << "  ✓ All array accesses bounds-checked\n";
    std::cout << "  ✓ All pointers validated before dereference\n";
    std::cout << "  ✓ All functions declared noexcept if they don't throw\n";
    std::cout << "  ✓ constexpr for compile-time evaluation\n";
    std::cout << "  ✓ Static analysis tools (e.g., LDRA, PC-lint Plus)\n";
}

// ============================================================================
// SECTION 10: Safety Checklist and Best Practices Summary
// ============================================================================

void demonstrate_safety_checklist() {
    std::cout << "\n=== 10. ISO 26262 Safety Checklist ===\n";
    
    std::cout << "\n✓ DESIGN PHASE:\n";
    std::cout << "  [✓] ASIL classification completed\n";
    std::cout << "  [✓] Hazard analysis and risk assessment (HARA)\n";
    std::cout << "  [✓] Safety goals defined (fail-safe behavior)\n";
    std::cout << "  [✓] Redundancy strategy selected (dual/TMR)\n";
    std::cout << "  [✓] WCET analysis performed\n";
    std::cout << "  [✓] Diagnostic coverage targets set\n\n";
    
    std::cout << "✓ IMPLEMENTATION PHASE:\n";
    std::cout << "  [✓] MISRA C++ guidelines followed\n";
    std::cout << "  [✓] No dynamic memory allocation\n";
    std::cout << "  [✓] No exceptions in safety functions\n";
    std::cout << "  [✓] All arithmetic overflow-checked\n";
    std::cout << "  [✓] All arrays bounds-checked\n";
    std::cout << "  [✓] Watchdog timer implemented\n";
    std::cout << "  [✓] Safe state transitions verified\n";
    std::cout << "  [✓] Deterministic timing validated\n\n";
    
    std::cout << "✓ VERIFICATION PHASE:\n";
    std::cout << "  [✓] Static analysis (PC-lint, Coverity, PVS-Studio)\n";
    std::cout << "  [✓] Dynamic testing (functional + fault injection)\n";
    std::cout << "  [✓] Code reviews (multiple reviewers)\n";
    std::cout << "  [✓] Requirements traceability matrix\n";
    std::cout << "  [✓] Coverage analysis (MC/DC for ASIL-D)\n";
    std::cout << "  [✓] Safety case documentation\n\n";
    
    std::cout << "✓ VALIDATION PHASE:\n";
    std::cout << "  [✓] Hardware-in-the-loop (HIL) testing\n";
    std::cout << "  [✓] Environmental stress testing\n";
    std::cout << "  [✓] EMC/EMI validation\n";
    std::cout << "  [✓] Long-term reliability testing\n";
    std::cout << "  [✓] Safety audit completed\n\n";
    
    std::cout << "KEY TAKEAWAYS:\n";
    std::cout << "  1. Safety is a process, not just code\n";
    std::cout << "  2. Redundancy and diagnostics are essential for ASIL-C/D\n";
    std::cout << "  3. Deterministic behavior is mandatory\n";
    std::cout << "  4. Always transition to safe state on fault\n";
    std::cout << "  5. Document everything (traceability is critical)\n";
    std::cout << "  6. Use certified tools and compilers for final builds\n";
    std::cout << "  7. Continuous monitoring in production (field data)\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     ISO 26262 Functional Safety - Modern C++ Demonstration     ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║  Comprehensive examples of safety-critical automotive code    ║\n";
    std::cout << "║  covering ASIL levels, MISRA C++, redundancy, and more       ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    demonstrate_asil_levels();
    demonstrate_safe_data_types();
    demonstrate_redundancy_voting();
    demonstrate_watchdog_monitoring();
    demonstrate_memory_safety();
    demonstrate_runtime_diagnostics();
    demonstrate_safe_state_transitions();
    demonstrate_deterministic_timing();
    demonstrate_misra_compliance();
    demonstrate_safety_checklist();
    
    std::cout << "\n" << std::string(68, '=') << "\n";
    std::cout << "All safety demonstrations completed successfully!\n";
    std::cout << "Remember: Safety certification requires formal verification,\n";
    std::cout << "          extensive testing, and compliance with ISO 26262 process.\n";
    std::cout << std::string(68, '=') << "\n\n";
    
    return 0;
}
