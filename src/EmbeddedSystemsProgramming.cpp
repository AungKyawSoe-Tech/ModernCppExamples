#include <iostream>
#include <array>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>
#include <optional>
#include <cassert>

// ===================================================================
// EMBEDDED SYSTEMS PROGRAMMING IN MODERN C++
// ===================================================================
// This file demonstrates embedded C++ patterns and best practices:
// - No dynamic allocation (no new/delete)
// - Fixed-size buffers
// - Compile-time computation
// - RAII for hardware resources
// - Static polymorphism (no vtables)
// - Memory pools
// - Interrupt-safe techniques
// ===================================================================

// ===================================================================
// 1. MEMORY POOL (NO HEAP ALLOCATION)
// ===================================================================

template<typename T, size_t PoolSize>
class MemoryPool {
private:
    alignas(T) uint8_t storage[PoolSize * sizeof(T)];
    bool used[PoolSize] = {};
    
public:
    template<typename... Args>
    T* allocate(Args&&... args) {
        for (size_t i = 0; i < PoolSize; ++i) {
            if (!used[i]) {
                used[i] = true;
                T* ptr = reinterpret_cast<T*>(&storage[i * sizeof(T)]);
                new (ptr) T(std::forward<Args>(args)...);  // Placement new
                return ptr;
            }
        }
        return nullptr;  // Pool exhausted
    }
    
    void deallocate(T* ptr) {
        if (!ptr) return;
        
        size_t index = (reinterpret_cast<uint8_t*>(ptr) - storage) / sizeof(T);
        if (index < PoolSize && used[index]) {
            ptr->~T();
            used[index] = false;
        }
    }
    
    size_t available() const {
        size_t count = 0;
        for (size_t i = 0; i < PoolSize; ++i) {
            if (!used[i]) ++count;
        }
        return count;
    }
    
    size_t capacity() const { return PoolSize; }
};

void example_memory_pool() {
    std::cout << "\n=== 1. MEMORY POOL (NO HEAP) ===" << std::endl;
    
    struct Sensor {
        int id;
        float value;
        Sensor(int i, float v) : id(i), value(v) {
            std::cout << "    Sensor " << id << " created" << std::endl;
        }
        ~Sensor() {
            std::cout << "    Sensor " << id << " destroyed" << std::endl;
        }
    };
    
    MemoryPool<Sensor, 5> pool;
    std::cout << "  Pool capacity: " << pool.capacity() << std::endl;
    
    std::cout << "\nAllocating sensors:" << std::endl;
    Sensor* s1 = pool.allocate(1, 25.5f);
    Sensor* s2 = pool.allocate(2, 30.2f);
    Sensor* s3 = pool.allocate(3, 28.7f);
    
    std::cout << "  Available slots: " << pool.available() << std::endl;
    
    std::cout << "\nDeallocating sensor 2:" << std::endl;
    pool.deallocate(s2);
    std::cout << "  Available slots: " << pool.available() << std::endl;
    
    std::cout << "\nAllocating new sensor:" << std::endl;
    Sensor* s4 = pool.allocate(4, 27.1f);
    
    std::cout << "\nCleanup:" << std::endl;
    pool.deallocate(s1);
    pool.deallocate(s3);
    pool.deallocate(s4);
    
    std::cout << "\n✓ EMBEDDED BENEFITS:" << std::endl;
    std::cout << "  • No heap fragmentation" << std::endl;
    std::cout << "  • Deterministic allocation time" << std::endl;
    std::cout << "  • Fixed memory footprint" << std::endl;
    std::cout << "  • Suitable for real-time systems" << std::endl;
}

// ===================================================================
// 2. CIRCULAR BUFFER (RING BUFFER)
// ===================================================================

template<typename T, size_t Size>
class CircularBuffer {
private:
    std::array<T, Size> buffer;
    size_t head = 0;
    size_t tail = 0;
    size_t count = 0;
    
public:
    bool push(const T& item) {
        if (is_full()) return false;
        
        buffer[head] = item;
        head = (head + 1) % Size;
        ++count;
        return true;
    }
    
    bool pop(T& item) {
        if (is_empty()) return false;
        
        item = buffer[tail];
        tail = (tail + 1) % Size;
        --count;
        return true;
    }
    
    bool is_empty() const { return count == 0; }
    bool is_full() const { return count == Size; }
    size_t size() const { return count; }
    size_t capacity() const { return Size; }
};

void example_circular_buffer() {
    std::cout << "\n=== 2. CIRCULAR BUFFER (INTERRUPT-SAFE) ===" << std::endl;
    
    CircularBuffer<uint8_t, 8> uart_rx_buffer;
    
    std::cout << "\nSimulating UART receive (ISR):" << std::endl;
    for (uint8_t i = 0; i < 5; ++i) {
        if (uart_rx_buffer.push(0x30 + i)) {
            std::cout << "  [ISR] Received: 0x" << std::hex 
                      << (int)(0x30 + i) << std::dec << std::endl;
        }
    }
    
    std::cout << "\nBuffer status: " << uart_rx_buffer.size() 
              << "/" << uart_rx_buffer.capacity() << std::endl;
    
    std::cout << "\nMain loop processing:" << std::endl;
    uint8_t data;
    while (uart_rx_buffer.pop(data)) {
        std::cout << "  [Main] Processing: 0x" << std::hex 
                  << (int)data << std::dec << std::endl;
    }
    
    std::cout << "\n✓ EMBEDDED BENEFITS:" << std::endl;
    std::cout << "  • Lock-free for single producer/consumer" << std::endl;
    std::cout << "  • Constant-time operations" << std::endl;
    std::cout << "  • ISR-safe data transfer" << std::endl;
    std::cout << "  • No dynamic allocation" << std::endl;
}

// ===================================================================
// 3. HARDWARE REGISTER ABSTRACTION (RAII)
// ===================================================================

// Simulate hardware registers
struct HardwareRegisters {
    static inline uint32_t GPIO_CONFIG = 0;
    static inline uint32_t GPIO_OUTPUT = 0;
    static inline uint32_t ADC_CONTROL = 0;
};

enum class PinMode : uint8_t {
    Input = 0,
    Output = 1,
    Analog = 2
};

class GpioPin {
private:
    uint8_t pin_number;
    PinMode original_mode;
    
    static constexpr uint32_t get_pin_mask(uint8_t pin) {
        return 1u << pin;
    }
    
public:
    GpioPin(uint8_t pin, PinMode mode) 
        : pin_number(pin)
        , original_mode(static_cast<PinMode>(
            (HardwareRegisters::GPIO_CONFIG >> (pin * 2)) & 0x3)) {
        
        // Configure pin mode
        uint32_t mask = 0x3u << (pin * 2);
        HardwareRegisters::GPIO_CONFIG &= ~mask;
        HardwareRegisters::GPIO_CONFIG |= (static_cast<uint32_t>(mode) << (pin * 2));
        
        std::cout << "    GPIO Pin " << (int)pin_number 
                  << " configured as " << (mode == PinMode::Output ? "Output" : "Input")
                  << std::endl;
    }
    
    ~GpioPin() {
        // Restore original mode (RAII cleanup)
        uint32_t mask = 0x3u << (pin_number * 2);
        HardwareRegisters::GPIO_CONFIG &= ~mask;
        HardwareRegisters::GPIO_CONFIG |= (static_cast<uint32_t>(original_mode) << (pin_number * 2));
        
        std::cout << "    GPIO Pin " << (int)pin_number 
                  << " restored to original mode" << std::endl;
    }
    
    // Prevent copying (hardware resources are unique)
    GpioPin(const GpioPin&) = delete;
    GpioPin& operator=(const GpioPin&) = delete;
    
    // Allow moving
    GpioPin(GpioPin&& other) noexcept 
        : pin_number(other.pin_number)
        , original_mode(other.original_mode) {
        other.pin_number = 0xFF;  // Mark as moved
    }
    
    void write(bool value) {
        if (value) {
            HardwareRegisters::GPIO_OUTPUT |= get_pin_mask(pin_number);
        } else {
            HardwareRegisters::GPIO_OUTPUT &= ~get_pin_mask(pin_number);
        }
    }
    
    bool read() const {
        return (HardwareRegisters::GPIO_OUTPUT & get_pin_mask(pin_number)) != 0;
    }
};

void example_hardware_raii() {
    std::cout << "\n=== 3. HARDWARE REGISTER ABSTRACTION (RAII) ===" << std::endl;
    
    std::cout << "\nConfiguring LED pin:" << std::endl;
    {
        GpioPin led(5, PinMode::Output);
        
        std::cout << "  Setting LED ON" << std::endl;
        led.write(true);
        
        std::cout << "  LED state: " << (led.read() ? "ON" : "OFF") << std::endl;
        
        // RAII: Pin automatically restored when going out of scope
    }
    std::cout << "  (Pin automatically restored)\n" << std::endl;
    
    std::cout << "✓ EMBEDDED BENEFITS:" << std::endl;
    std::cout << "  • Automatic resource cleanup" << std::endl;
    std::cout << "  • Exception-safe (if exceptions enabled)" << std::endl;
    std::cout << "  • Can't forget to restore state" << std::endl;
    std::cout << "  • Move semantics for ownership transfer" << std::endl;
}

// ===================================================================
// 4. STATIC POLYMORPHISM (NO VTABLES)
// ===================================================================

// CRTP (Curiously Recurring Template Pattern)
template<typename Derived>
class SensorBase {
public:
    float read() {
        return static_cast<Derived*>(this)->read_impl();
    }
    
    const char* name() {
        return static_cast<Derived*>(this)->name_impl();
    }
};

class TemperatureSensor : public SensorBase<TemperatureSensor> {
public:
    float read_impl() { return 25.5f; }
    const char* name_impl() { return "Temperature"; }
};

class HumiditySensor : public SensorBase<HumiditySensor> {
public:
    float read_impl() { return 65.2f; }
    const char* name_impl() { return "Humidity"; }
};

template<typename Sensor>
void process_sensor(Sensor& sensor) {
    std::cout << "  Reading " << sensor.name() << " sensor: " 
              << sensor.read() << std::endl;
}

void example_static_polymorphism() {
    std::cout << "\n=== 4. STATIC POLYMORPHISM (NO VTABLES) ===" << std::endl;
    
    TemperatureSensor temp;
    HumiditySensor humid;
    
    std::cout << "\nReading sensors (compile-time dispatch):" << std::endl;
    process_sensor(temp);
    process_sensor(humid);
    
    std::cout << "\n✓ EMBEDDED BENEFITS:" << std::endl;
    std::cout << "  • No vtable overhead (saves RAM)" << std::endl;
    std::cout << "  • No runtime indirection (faster)" << std::endl;
    std::cout << "  • Fully inlineable" << std::endl;
    std::cout << "  • Smaller code size" << std::endl;
}

// ===================================================================
// 5. COMPILE-TIME COMPUTATION (constexpr)
// ===================================================================

constexpr uint32_t calculate_baud_rate_register(uint32_t system_clock, uint32_t baud_rate) {
    return (system_clock / (16 * baud_rate)) - 1;
}

constexpr uint32_t calculate_pwm_period(uint32_t timer_clock, uint32_t frequency) {
    return timer_clock / frequency;
}

template<size_t N>
constexpr uint32_t fibonacci() {
    if constexpr (N <= 1) {
        return N;
    } else {
        return fibonacci<N-1>() + fibonacci<N-2>();
    }
}

void example_compile_time() {
    std::cout << "\n=== 5. COMPILE-TIME COMPUTATION (constexpr) ===" << std::endl;
    
    // Computed at compile time!
    constexpr uint32_t BAUD_115200 = calculate_baud_rate_register(16000000, 115200);
    constexpr uint32_t PWM_1KHZ = calculate_pwm_period(1000000, 1000);
    constexpr uint32_t FIB_10 = fibonacci<10>();
    
    std::cout << "\nCompile-time computed values:" << std::endl;
    std::cout << "  UART baud rate register: 0x" << std::hex << BAUD_115200 << std::dec << std::endl;
    std::cout << "  PWM period (1kHz): " << PWM_1KHZ << std::endl;
    std::cout << "  Fibonacci(10): " << FIB_10 << std::endl;
    
    std::cout << "\n✓ EMBEDDED BENEFITS:" << std::endl;
    std::cout << "  • Zero runtime overhead" << std::endl;
    std::cout << "  • Compile-time error checking" << std::endl;
    std::cout << "  • Values in ROM, not computed" << std::endl;
    std::cout << "  • No initialization code needed" << std::endl;
}

// ===================================================================
// 6. FIXED-SIZE STRING (NO STD::STRING)
// ===================================================================

template<size_t MaxSize>
class FixedString {
private:
    char data[MaxSize + 1] = {};  // +1 for null terminator
    size_t length = 0;
    
public:
    FixedString() = default;
    
    FixedString(const char* str) {
        if (str) {
            length = 0;
            while (str[length] && length < MaxSize) {
                data[length] = str[length];
                ++length;
            }
            data[length] = '\0';
        }
    }
    
    bool append(char c) {
        if (length < MaxSize) {
            data[length++] = c;
            data[length] = '\0';
            return true;
        }
        return false;
    }
    
    bool append(const char* str) {
        if (!str) return false;
        
        while (*str && length < MaxSize) {
            data[length++] = *str++;
        }
        data[length] = '\0';
        return *str == '\0';  // True if entire string was appended
    }
    
    void clear() {
        length = 0;
        data[0] = '\0';
    }
    
    const char* c_str() const { return data; }
    size_t size() const { return length; }
    size_t capacity() const { return MaxSize; }
    bool is_full() const { return length >= MaxSize; }
};

void example_fixed_string() {
    std::cout << "\n=== 6. FIXED-SIZE STRING (NO HEAP) ===" << std::endl;
    
    FixedString<32> msg("Sensor: ");
    
    std::cout << "\nBuilding message:" << std::endl;
    std::cout << "  Initial: \"" << msg.c_str() << "\"" << std::endl;
    
    msg.append("Temp=");
    msg.append("25.5");
    msg.append("C");
    
    std::cout << "  Final: \"" << msg.c_str() << "\"" << std::endl;
    std::cout << "  Size: " << msg.size() << "/" << msg.capacity() << std::endl;
    
    std::cout << "\n✓ EMBEDDED BENEFITS:" << std::endl;
    std::cout << "  • No heap allocation" << std::endl;
    std::cout << "  • Fixed memory footprint" << std::endl;
    std::cout << "  • Predictable behavior" << std::endl;
    std::cout << "  • Stack-based storage" << std::endl;
}

// ===================================================================
// 7. STATE MACHINE (EMBEDDED PATTERN)
// ===================================================================

enum class State {
    Idle,
    Reading,
    Processing,
    Sending,
    Error
};

enum class Event {
    StartRead,
    DataReady,
    ProcessComplete,
    SendComplete,
    ErrorOccurred,
    Reset
};

class StateMachine {
private:
    State current_state = State::Idle;
    
public:
    void process_event(Event event) {
        std::cout << "  [" << state_name(current_state) << "] -> Event: " 
                  << event_name(event);
        
        State new_state = current_state;
        
        switch (current_state) {
            case State::Idle:
                if (event == Event::StartRead) {
                    new_state = State::Reading;
                }
                break;
                
            case State::Reading:
                if (event == Event::DataReady) {
                    new_state = State::Processing;
                } else if (event == Event::ErrorOccurred) {
                    new_state = State::Error;
                }
                break;
                
            case State::Processing:
                if (event == Event::ProcessComplete) {
                    new_state = State::Sending;
                } else if (event == Event::ErrorOccurred) {
                    new_state = State::Error;
                }
                break;
                
            case State::Sending:
                if (event == Event::SendComplete) {
                    new_state = State::Idle;
                } else if (event == Event::ErrorOccurred) {
                    new_state = State::Error;
                }
                break;
                
            case State::Error:
                if (event == Event::Reset) {
                    new_state = State::Idle;
                }
                break;
        }
        
        if (new_state != current_state) {
            current_state = new_state;
            std::cout << " -> [" << state_name(current_state) << "]" << std::endl;
        } else {
            std::cout << " (no transition)" << std::endl;
        }
    }
    
    State get_state() const { return current_state; }
    
private:
    static const char* state_name(State s) {
        switch (s) {
            case State::Idle: return "Idle";
            case State::Reading: return "Reading";
            case State::Processing: return "Processing";
            case State::Sending: return "Sending";
            case State::Error: return "Error";
            default: return "Unknown";
        }
    }
    
    static const char* event_name(Event e) {
        switch (e) {
            case Event::StartRead: return "StartRead";
            case Event::DataReady: return "DataReady";
            case Event::ProcessComplete: return "ProcessComplete";
            case Event::SendComplete: return "SendComplete";
            case Event::ErrorOccurred: return "ErrorOccurred";
            case Event::Reset: return "Reset";
            default: return "Unknown";
        }
    }
};

void example_state_machine() {
    std::cout << "\n=== 7. STATE MACHINE (EMBEDDED PATTERN) ===" << std::endl;
    
    StateMachine sm;
    
    std::cout << "\nNormal operation sequence:" << std::endl;
    sm.process_event(Event::StartRead);
    sm.process_event(Event::DataReady);
    sm.process_event(Event::ProcessComplete);
    sm.process_event(Event::SendComplete);
    
    std::cout << "\nError handling:" << std::endl;
    sm.process_event(Event::StartRead);
    sm.process_event(Event::ErrorOccurred);
    sm.process_event(Event::Reset);
    
    std::cout << "\n✓ EMBEDDED BENEFITS:" << std::endl;
    std::cout << "  • Clear, predictable behavior" << std::endl;
    std::cout << "  • Easy to validate and test" << std::endl;
    std::cout << "  • Common in embedded protocols" << std::endl;
    std::cout << "  • No dynamic dispatch overhead" << std::endl;
}

// ===================================================================
// 8. BIT MANIPULATION UTILITIES
// ===================================================================

namespace BitOps {
    template<typename T>
    constexpr void set_bit(T& reg, uint8_t bit) {
        reg |= (static_cast<T>(1) << bit);
    }
    
    template<typename T>
    constexpr void clear_bit(T& reg, uint8_t bit) {
        reg &= ~(static_cast<T>(1) << bit);
    }
    
    template<typename T>
    constexpr void toggle_bit(T& reg, uint8_t bit) {
        reg ^= (static_cast<T>(1) << bit);
    }
    
    template<typename T>
    constexpr bool test_bit(T reg, uint8_t bit) {
        return (reg & (static_cast<T>(1) << bit)) != 0;
    }
    
    template<typename T>
    constexpr void write_bits(T& reg, uint8_t start_bit, uint8_t num_bits, T value) {
        T mask = ((static_cast<T>(1) << num_bits) - 1) << start_bit;
        reg = (reg & ~mask) | ((value << start_bit) & mask);
    }
}

void example_bit_manipulation() {
    std::cout << "\n=== 8. BIT MANIPULATION UTILITIES ===" << std::endl;
    
    uint32_t control_reg = 0;
    
    std::cout << "\nManipulating control register:" << std::endl;
    std::cout << "  Initial value: 0x" << std::hex << control_reg << std::dec << std::endl;
    
    BitOps::set_bit(control_reg, 0);  // Enable bit 0
    std::cout << "  After set bit 0: 0x" << std::hex << control_reg << std::dec << std::endl;
    
    BitOps::set_bit(control_reg, 5);  // Enable bit 5
    std::cout << "  After set bit 5: 0x" << std::hex << control_reg << std::dec << std::endl;
    
    BitOps::write_bits(control_reg, 2, 3, 0b101u);  // Write 3 bits starting at bit 2
    std::cout << "  After write bits 2-4: 0x" << std::hex << control_reg << std::dec << std::endl;
    
    std::cout << "\nTesting bits:" << std::endl;
    std::cout << "  Bit 0 is " << (BitOps::test_bit(control_reg, 0) ? "SET" : "CLEAR") << std::endl;
    std::cout << "  Bit 1 is " << (BitOps::test_bit(control_reg, 1) ? "SET" : "CLEAR") << std::endl;
    
    std::cout << "\n✓ EMBEDDED BENEFITS:" << std::endl;
    std::cout << "  • Type-safe bit operations" << std::endl;
    std::cout << "  • Constexpr for compile-time use" << std::endl;
    std::cout << "  • Clear, readable code" << std::endl;
    std::cout << "  • No magic numbers" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  EMBEDDED SYSTEMS PROGRAMMING IN MODERN C++" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    example_memory_pool();
    example_circular_buffer();
    example_hardware_raii();
    example_static_polymorphism();
    example_compile_time();
    example_fixed_string();
    example_state_machine();
    example_bit_manipulation();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  EMBEDDED C++ BEST PRACTICES SUMMARY" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n🎯 KEY PRINCIPLES FOR EMBEDDED C++:" << std::endl;
    
    std::cout << "\n1. NO DYNAMIC ALLOCATION" << std::endl;
    std::cout << "   • Use memory pools instead of new/delete" << std::endl;
    std::cout << "   • Fixed-size containers (std::array)" << std::endl;
    std::cout << "   • Stack allocation preferred" << std::endl;
    std::cout << "   • Predictable memory usage" << std::endl;
    
    std::cout << "\n2. COMPILE-TIME COMPUTATION" << std::endl;
    std::cout << "   • constexpr functions for configuration" << std::endl;
    std::cout << "   • Template metaprogramming" << std::endl;
    std::cout << "   • if constexpr for conditional compilation" << std::endl;
    std::cout << "   • Zero runtime overhead" << std::endl;
    
    std::cout << "\n3. STATIC POLYMORPHISM" << std::endl;
    std::cout << "   • CRTP instead of virtual functions" << std::endl;
    std::cout << "   • No vtable overhead" << std::endl;
    std::cout << "   • Fully inlineable" << std::endl;
    std::cout << "   • Saves RAM and Flash" << std::endl;
    
    std::cout << "\n4. RAII FOR HARDWARE" << std::endl;
    std::cout << "   • Automatic resource management" << std::endl;
    std::cout << "   • Scope-based cleanup" << std::endl;
    std::cout << "   • Move semantics for ownership" << std::endl;
    std::cout << "   • Exception-safe (if enabled)" << std::endl;
    
    std::cout << "\n5. INTERRUPT-SAFE PATTERNS" << std::endl;
    std::cout << "   • Circular buffers for ISR->Main communication" << std::endl;
    std::cout << "   • Atomic operations" << std::endl;
    std::cout << "   • Lock-free data structures" << std::endl;
    std::cout << "   • Minimal ISR processing" << std::endl;
    
    std::cout << "\n6. DETERMINISTIC BEHAVIOR" << std::endl;
    std::cout << "   • No recursion (or limited)" << std::endl;
    std::cout << "   • Bounded loops" << std::endl;
    std::cout << "   • Fixed execution time" << std::endl;
    std::cout << "   • Real-time constraints" << std::endl;
    
    std::cout << "\n📋 MODERN C++ FEATURES FOR EMBEDDED:" << std::endl;
    std::cout << "   ✓ constexpr (C++11/14/17) - Compile-time computation" << std::endl;
    std::cout << "   ✓ std::array (C++11) - Fixed-size arrays" << std::endl;
    std::cout << "   ✓ std::optional (C++17) - Error handling without exceptions" << std::endl;
    std::cout << "   ✓ if constexpr (C++17) - Conditional compilation" << std::endl;
    std::cout << "   ✓ Templates - Static polymorphism" << std::endl;
    std::cout << "   ✓ Move semantics (C++11) - Efficient ownership transfer" << std::endl;
    std::cout << "   ✓ RAII - Automatic resource management" << std::endl;
    std::cout << "   ✓ Type traits - Compile-time type checking" << std::endl;
    
    std::cout << "\n⚠️ AVOID IN EMBEDDED:" << std::endl;
    std::cout << "   ❌ new/delete (use memory pools)" << std::endl;
    std::cout << "   ❌ std::vector, std::string (dynamic allocation)" << std::endl;
    std::cout << "   ❌ Virtual functions (unless justified)" << std::endl;
    std::cout << "   ❌ RTTI (enable only if needed)" << std::endl;
    std::cout << "   ❌ Exceptions (often disabled in embedded)" << std::endl;
    std::cout << "   ❌ iostream (large code size)" << std::endl;
    std::cout << "   ❌ Recursion (stack overflow risk)" << std::endl;
    
    std::cout << "\n💡 EMBEDDED C++ IDIOMS:" << std::endl;
    std::cout << "   • Singleton pattern for hardware peripherals" << std::endl;
    std::cout << "   • State machines for protocol handling" << std::endl;
    std::cout << "   • Circular buffers for data streaming" << std::endl;
    std::cout << "   • Memory pools for dynamic-like allocation" << std::endl;
    std::cout << "   • CRTP for zero-cost abstraction" << std::endl;
    std::cout << "   • Register classes with bit fields" << std::endl;
    
    std::cout << "\n🔧 TYPICAL MEMORY CONSTRAINTS:" << std::endl;
    std::cout << "   • Flash: 32KB - 2MB (code storage)" << std::endl;
    std::cout << "   • RAM: 4KB - 256KB (runtime data)" << std::endl;
    std::cout << "   • Stack: 1KB - 8KB (limited depth)" << std::endl;
    std::cout << "   • No heap or tiny heap (fragmentation risk)" << std::endl;
    
    std::cout << "\n🚀 REAL-TIME CONSIDERATIONS:" << std::endl;
    std::cout << "   • Interrupt latency: <10µs typical" << std::endl;
    std::cout << "   • Task scheduling: RTOS or bare-metal" << std::endl;
    std::cout << "   • Timing critical sections" << std::endl;
    std::cout << "   • Watchdog timer management" << std::endl;
    
    std::cout << "\n📚 RECOMMENDED READING:" << std::endl;
    std::cout << "   • \"Embedded C++ Coding Standard\" by BARR Group" << std::endl;
    std::cout << "   • \"Real-Time C++\" by Christopher Kormanyos" << std::endl;
    std::cout << "   • \"Effective Modern C++\" (embedded-applicable patterns)" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
