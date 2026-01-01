#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include <cstdint>
#include <array>
#include <bit>

// ===================================================================
// C++23 FEATURES FOR EMBEDDED SYSTEMS, FIRMWARE, AND RTOS
// ===================================================================
// Focused on features useful for:
// - Firmware development
// - Real-Time Operating Systems (RTOS)
// - Embedded Linux platforms
// - Hardware interfacing
// - Resource-constrained environments
// ===================================================================

// ===================================================================
// 1. std::expected<T, E> - ERROR HANDLING WITHOUT EXCEPTIONS
// ===================================================================
// CRITICAL for embedded: No exception overhead!
// Perfect for firmware where exceptions are disabled

#if __cplusplus >= 202302L && __has_include(<expected>)
#include <expected>

enum class SensorError {
    NotConnected,
    ReadTimeout,
    InvalidData,
    CalibrationFailed
};

const char* to_string(SensorError error) {
    switch (error) {
        case SensorError::NotConnected: return "Sensor not connected";
        case SensorError::ReadTimeout: return "Read timeout";
        case SensorError::InvalidData: return "Invalid data";
        case SensorError::CalibrationFailed: return "Calibration failed";
    }
    return "Unknown error";
}

// Return either temperature value or error - no exceptions!
std::expected<float, SensorError> read_temperature_sensor() {
    // Simulate sensor read
    static int call_count = 0;
    call_count++;
    
    if (call_count == 1) {
        return std::unexpected(SensorError::NotConnected);
    }
    if (call_count == 2) {
        return std::unexpected(SensorError::ReadTimeout);
    }
    
    return 23.5f;  // Success
}

// Chaining operations with monadic interface
std::expected<float, SensorError> read_and_convert_to_fahrenheit() {
    return read_temperature_sensor()
        .and_then([](float celsius) -> std::expected<float, SensorError> {
            if (celsius < -273.15f) {
                return std::unexpected(SensorError::InvalidData);
            }
            return celsius * 9.0f / 5.0f + 32.0f;
        });
}

void demonstrate_expected() {
    std::cout << "\n=== 1. std::expected - EXCEPTION-FREE ERROR HANDLING ===" << std::endl;
    std::cout << "Perfect for firmware with -fno-exceptions" << std::endl;
    
    // First call - error
    auto result1 = read_temperature_sensor();
    if (result1) {
        std::cout << "\n✅ Temperature: " << *result1 << "°C" << std::endl;
    } else {
        std::cout << "\n❌ Error: " << to_string(result1.error()) << std::endl;
    }
    
    // Second call - error
    auto result2 = read_temperature_sensor();
    if (result2) {
        std::cout << "✅ Temperature: " << *result2 << "°C" << std::endl;
    } else {
        std::cout << "❌ Error: " << to_string(result2.error()) << std::endl;
    }
    
    // Third call - success
    auto result3 = read_temperature_sensor();
    if (result3) {
        std::cout << "✅ Temperature: " << *result3 << "°C" << std::endl;
    } else {
        std::cout << "❌ Error: " << to_string(result3.error()) << std::endl;
    }
    
    std::cout << "\n💡 Benefits for Embedded:" << std::endl;
    std::cout << "   • Zero exception overhead" << std::endl;
    std::cout << "   • Works with -fno-exceptions" << std::endl;
    std::cout << "   • Explicit error handling" << std::endl;
    std::cout << "   • Deterministic performance" << std::endl;
}

#else
void demonstrate_expected() {
    std::cout << "\n=== 1. std::expected ===" << std::endl;
    std::cout << "⚠️  std::expected requires C++23 compiler support" << std::endl;
    std::cout << "Alternative: Use std::variant<T, Error> in C++17" << std::endl;
}
#endif

// ===================================================================
// 2. std::byteswap - ENDIANNESS CONVERSION FOR HARDWARE
// ===================================================================
// Essential for: Network protocols, file formats, hardware registers

#if __cplusplus >= 202302L && __has_include(<bit>)

void demonstrate_byteswap() {
    std::cout << "\n=== 2. std::byteswap - HARDWARE BYTE ORDER ===" << std::endl;
    std::cout << "Critical for network protocols and hardware registers" << std::endl;
    
    // Reading from network (big-endian) to host (possibly little-endian)
    uint32_t network_value = 0x12345678;
    std::cout << "\nNetwork value (big-endian): 0x" << std::hex << network_value << std::dec << std::endl;
    
    uint32_t host_value = std::byteswap(network_value);
    std::cout << "After byteswap: 0x" << std::hex << host_value << std::dec << std::endl;
    
    // 16-bit register value swap
    uint16_t reg16 = 0xABCD;
    std::cout << "\n16-bit register: 0x" << std::hex << reg16 << std::dec << std::endl;
    uint16_t swapped16 = std::byteswap(reg16);
    std::cout << "Swapped: 0x" << std::hex << swapped16 << std::dec << std::endl;
    
    // 64-bit timestamp swap
    uint64_t timestamp = 0x0123456789ABCDEF;
    std::cout << "\n64-bit timestamp: 0x" << std::hex << timestamp << std::dec << std::endl;
    uint64_t swapped64 = std::byteswap(timestamp);
    std::cout << "Swapped: 0x" << std::hex << swapped64 << std::dec << std::endl;
    
    std::cout << "\n💡 Embedded Use Cases:" << std::endl;
    std::cout << "   • Network protocol implementation (TCP/IP)" << std::endl;
    std::cout << "   • File format parsing (headers, metadata)" << std::endl;
    std::cout << "   • Cross-platform binary data exchange" << std::endl;
    std::cout << "   • Hardware register manipulation" << std::endl;
}

#else
void demonstrate_byteswap() {
    std::cout << "\n=== 2. std::byteswap ===" << std::endl;
    std::cout << "⚠️  std::byteswap requires C++23" << std::endl;
    std::cout << "Alternative: Use __builtin_bswap32() or manual bit shifts" << std::endl;
}
#endif

// ===================================================================
// 3. std::to_underlying - ENUM TO INTEGER (HARDWARE REGISTERS)
// ===================================================================

enum class GPIOPin : uint8_t {
    PIN_0 = 0,
    PIN_1 = 1,
    PIN_2 = 2,
    PIN_3 = 3,
    PIN_LED = 13,
    PIN_BUTTON = 7
};

enum class MemoryRegister : uint32_t {
    STATUS_REG = 0x40000000,
    CONTROL_REG = 0x40000004,
    DATA_REG = 0x40000008,
    CONFIG_REG = 0x4000000C
};

void demonstrate_to_underlying() {
    std::cout << "\n=== 3. std::to_underlying - HARDWARE REGISTER ACCESS ===" << std::endl;
    std::cout << "Convert enum class to underlying integer type" << std::endl;
    
    // GPIO pin access
    GPIOPin led_pin = GPIOPin::PIN_LED;
    auto pin_number = std::to_underlying(led_pin);
    std::cout << "\nLED Pin number: " << static_cast<int>(pin_number) << std::endl;
    
    // Memory-mapped register access
    MemoryRegister status = MemoryRegister::STATUS_REG;
    uint32_t* status_ptr = reinterpret_cast<uint32_t*>(std::to_underlying(status));
    std::cout << "Status register address: 0x" << std::hex << std::to_underlying(status) << std::dec << std::endl;
    
    // Array indexing with enum
    std::array<const char*, 4> reg_names = {"STATUS", "CONTROL", "DATA", "CONFIG"};
    MemoryRegister reg = MemoryRegister::CONTROL_REG;
    size_t index = (std::to_underlying(reg) - std::to_underlying(MemoryRegister::STATUS_REG)) / 4;
    std::cout << "Register name: " << reg_names[index] << std::endl;
    
    std::cout << "\n💡 Before C++23:" << std::endl;
    std::cout << "   static_cast<std::underlying_type_t<GPIOPin>>(led_pin)" << std::endl;
    std::cout << "\n💡 C++23:" << std::endl;
    std::cout << "   std::to_underlying(led_pin)  // Much cleaner!" << std::endl;
}

// ===================================================================
// 4. constexpr FOR <cmath> - COMPILE-TIME CALCULATIONS
// ===================================================================

// Lookup table generation at compile time
constexpr std::array<float, 256> generate_sine_lookup_table() {
    std::array<float, 256> table{};
    for (size_t i = 0; i < 256; ++i) {
        // C++23: std::sin is now constexpr!
        table[i] = std::sin(2.0 * 3.14159265358979323846 * i / 256.0);
    }
    return table;
}

// PWM duty cycle calculation at compile time
constexpr uint16_t calculate_pwm_value(float duty_percent) {
    return static_cast<uint16_t>(duty_percent * 65535.0f / 100.0f);
}

void demonstrate_constexpr_cmath() {
    std::cout << "\n=== 4. constexpr <cmath> - COMPILE-TIME CALCULATIONS ===" << std::endl;
    std::cout << "Generate lookup tables at compile time" << std::endl;
    
    // Sine lookup table generated at compile time
    constexpr auto sine_table = generate_sine_lookup_table();
    std::cout << "\nSine lookup table (256 entries) generated at compile time" << std::endl;
    std::cout << "Sample values:" << std::endl;
    std::cout << "  sin(0°)   ≈ " << sine_table[0] << std::endl;
    std::cout << "  sin(90°)  ≈ " << sine_table[64] << std::endl;
    std::cout << "  sin(180°) ≈ " << sine_table[128] << std::endl;
    std::cout << "  sin(270°) ≈ " << sine_table[192] << std::endl;
    
    // PWM values calculated at compile time
    constexpr uint16_t pwm_25 = calculate_pwm_value(25.0f);
    constexpr uint16_t pwm_50 = calculate_pwm_value(50.0f);
    constexpr uint16_t pwm_75 = calculate_pwm_value(75.0f);
    
    std::cout << "\nPWM duty cycle values (calculated at compile time):" << std::endl;
    std::cout << "  25%: " << pwm_25 << std::endl;
    std::cout << "  50%: " << pwm_50 << std::endl;
    std::cout << "  75%: " << pwm_75 << std::endl;
    
    std::cout << "\n💡 Embedded Benefits:" << std::endl;
    std::cout << "   • Zero runtime cost for lookup tables" << std::endl;
    std::cout << "   • Stored in ROM/Flash, not RAM" << std::endl;
    std::cout << "   • Fast table lookups vs real-time calculations" << std::endl;
    std::cout << "   • Deterministic execution time" << std::endl;
}

// ===================================================================
// 5. if consteval - COMPILE-TIME VS RUNTIME PATHS
// ===================================================================

constexpr uint32_t crc32_compute(const char* data, size_t len) {
    if consteval {
        // Compile-time: Simple algorithm
        uint32_t crc = 0xFFFFFFFF;
        for (size_t i = 0; i < len; ++i) {
            crc ^= static_cast<uint32_t>(data[i]);
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
            }
        }
        return ~crc;
    } else {
        // Runtime: Could use hardware CRC or optimized table lookup
        std::cout << "[Using runtime CRC algorithm]" << std::endl;
        uint32_t crc = 0xFFFFFFFF;
        for (size_t i = 0; i < len; ++i) {
            crc ^= static_cast<uint32_t>(data[i]);
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
            }
        }
        return ~crc;
    }
}

void demonstrate_if_consteval() {
    std::cout << "\n=== 5. if consteval - COMPILE-TIME VS RUNTIME ===" << std::endl;
    std::cout << "Different code paths for compile-time and runtime" << std::endl;
    
    // Compile-time CRC calculation
    constexpr const char* firmware_id = "FIRMWARE_V1.2.3";
    constexpr uint32_t compile_time_crc = crc32_compute(firmware_id, 15);
    std::cout << "\nFirmware ID: " << firmware_id << std::endl;
    std::cout << "CRC32 (compile-time): 0x" << std::hex << compile_time_crc << std::dec << std::endl;
    
    // Runtime CRC calculation
    const char* runtime_data = "RUNTIME_DATA_PACKET";
    uint32_t runtime_crc = crc32_compute(runtime_data, 19);
    std::cout << "\nRuntime data: " << runtime_data << std::endl;
    std::cout << "CRC32 (runtime): 0x" << std::hex << runtime_crc << std::dec << std::endl;
    
    std::cout << "\n💡 Embedded Applications:" << std::endl;
    std::cout << "   • Compile-time: Simple, portable algorithm" << std::endl;
    std::cout << "   • Runtime: Hardware CRC accelerator" << std::endl;
    std::cout << "   • Firmware verification checksums" << std::endl;
    std::cout << "   • Protocol integrity checks" << std::endl;
}

// ===================================================================
// 6. std::unreachable - OPTIMIZATION HINTS FOR COMPILER
// ===================================================================

enum class DeviceState : uint8_t {
    IDLE = 0,
    RUNNING = 1,
    STOPPED = 2,
    ERROR = 3
};

[[nodiscard]] constexpr const char* device_state_name(DeviceState state) {
    switch (state) {
        case DeviceState::IDLE:    return "IDLE";
        case DeviceState::RUNNING: return "RUNNING";
        case DeviceState::STOPPED: return "STOPPED";
        case DeviceState::ERROR:   return "ERROR";
    }
    std::unreachable();  // Tell compiler all cases covered
}

uint32_t process_command(uint8_t cmd) {
    if (cmd >= 0 && cmd <= 3) {
        return cmd * 100;  // Valid command processing
    }
    
    // Invalid command - should never happen with validated input
    std::unreachable();  // Optimization hint
}

void demonstrate_unreachable() {
    std::cout << "\n=== 6. std::unreachable - COMPILER OPTIMIZATION ===" << std::endl;
    std::cout << "Tell compiler certain code paths are impossible" << std::endl;
    
    std::cout << "\nDevice states:" << std::endl;
    std::cout << "  " << device_state_name(DeviceState::IDLE) << std::endl;
    std::cout << "  " << device_state_name(DeviceState::RUNNING) << std::endl;
    std::cout << "  " << device_state_name(DeviceState::STOPPED) << std::endl;
    std::cout << "  " << device_state_name(DeviceState::ERROR) << std::endl;
    
    std::cout << "\nCommand processing:" << std::endl;
    std::cout << "  Command 0: " << process_command(0) << std::endl;
    std::cout << "  Command 2: " << process_command(2) << std::endl;
    
    std::cout << "\n💡 Optimization Benefits:" << std::endl;
    std::cout << "   • Eliminates dead code paths" << std::endl;
    std::cout << "   • Better branch prediction" << std::endl;
    std::cout << "   • Smaller code size" << std::endl;
    std::cout << "   • Faster execution" << std::endl;
    
    std::cout << "\n⚠️  WARNING: Reaching std::unreachable() is UB!" << std::endl;
}

// ===================================================================
// 7. SIZE_T LITERAL SUFFIX (uz/z) - TYPE-SAFE SIZES
// ===================================================================

void demonstrate_size_literals() {
    std::cout << "\n=== 7. SIZE_T LITERAL SUFFIX - TYPE SAFETY ===" << std::endl;
    std::cout << "Avoid unsigned vs signed comparison warnings" << std::endl;
    
    std::vector<uint32_t> buffer = {1, 2, 3, 4, 5};
    
    // Old way - warning: comparison between signed and unsigned
    // for (int i = 0; i < buffer.size(); ++i) { }  // Warning!
    
    // C++23 way - type-safe
    constexpr size_t BUFFER_SIZE = 256uz;  // uz suffix = size_t
    std::array<uint8_t, BUFFER_SIZE> uart_buffer{};
    
    std::cout << "\nBuffer allocations:" << std::endl;
    std::cout << "  UART buffer: " << BUFFER_SIZE << " bytes" << std::endl;
    std::cout << "  Vector size: " << buffer.size() << " elements" << std::endl;
    
    // No warning with uz suffix
    if (buffer.size() > 3uz) {
        std::cout << "  Buffer has more than 3 elements" << std::endl;
    }
    
    std::cout << "\n💡 Before C++23:" << std::endl;
    std::cout << "   if (buffer.size() > 3)     // May warn" << std::endl;
    std::cout << "   if (buffer.size() > 3u)    // Wrong type (unsigned)" << std::endl;
    std::cout << "   if (buffer.size() > 3UL)   // Platform dependent" << std::endl;
    std::cout << "\n💡 C++23:" << std::endl;
    std::cout << "   if (buffer.size() > 3uz)   // Correct type!" << std::endl;
}

// ===================================================================
// 8. MULTIDIMENSIONAL SUBSCRIPT OPERATOR
// ===================================================================

template<typename T, size_t Rows, size_t Cols>
class Matrix {
private:
    std::array<T, Rows * Cols> data;
    
public:
    // C++23: Multi-dimensional operator[]
    constexpr T& operator[](size_t row, size_t col) {
        return data[row * Cols + col];
    }
    
    constexpr const T& operator[](size_t row, size_t col) const {
        return data[row * Cols + col];
    }
    
    constexpr void fill(T value) {
        data.fill(value);
    }
};

void demonstrate_multidim_subscript() {
    std::cout << "\n=== 8. MULTIDIMENSIONAL SUBSCRIPT OPERATOR ===" << std::endl;
    std::cout << "Direct matrix[row, col] syntax" << std::endl;
    
    Matrix<uint16_t, 3, 3> sensor_data;
    
    // C++23: Clean syntax for 2D array access
    sensor_data[0, 0] = 100;
    sensor_data[0, 1] = 150;
    sensor_data[0, 2] = 200;
    sensor_data[1, 0] = 250;
    sensor_data[1, 1] = 300;
    sensor_data[1, 2] = 350;
    sensor_data[2, 0] = 400;
    sensor_data[2, 1] = 450;
    sensor_data[2, 2] = 500;
    
    std::cout << "\n3x3 Sensor data matrix:" << std::endl;
    for (size_t row = 0; row < 3; ++row) {
        std::cout << "  ";
        for (size_t col = 0; col < 3; ++col) {
            std::cout << sensor_data[row, col] << "\t";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\n💡 Before C++23:" << std::endl;
    std::cout << "   matrix[row][col]      // Traditional" << std::endl;
    std::cout << "   matrix.at(row, col)   // Member function" << std::endl;
    std::cout << "\n💡 C++23:" << std::endl;
    std::cout << "   matrix[row, col]      // Direct syntax!" << std::endl;
}

// ===================================================================
// 9. PRACTICAL EMBEDDED EXAMPLE: FIRMWARE STATUS SYSTEM
// ===================================================================

#if __cplusplus >= 202302L && __has_include(<expected>)

enum class FirmwareError {
    ConfigInvalid,
    MemoryInsufficient,
    HardwareNotResponding,
    ChecksumMismatch
};

const char* firmware_error_name(FirmwareError err) {
    switch (err) {
        case FirmwareError::ConfigInvalid: return "Configuration invalid";
        case FirmwareError::MemoryInsufficient: return "Insufficient memory";
        case FirmwareError::HardwareNotResponding: return "Hardware not responding";
        case FirmwareError::ChecksumMismatch: return "Checksum mismatch";
    }
    std::unreachable();
}

class FirmwareStatus {
public:
    std::expected<bool, FirmwareError> initialize() {
        std::cout << "  [1] Checking configuration..." << std::endl;
        // Simulate config check
        
        std::cout << "  [2] Allocating memory..." << std::endl;
        // Simulate memory allocation
        
        std::cout << "  [3] Initializing hardware..." << std::endl;
        // Simulate hardware init
        
        std::cout << "  [4] Verifying checksums..." << std::endl;
        constexpr uint32_t expected_crc = 0x12345678;
        constexpr uint32_t actual_crc = 0x12345678;
        
        if (expected_crc != actual_crc) {
            return std::unexpected(FirmwareError::ChecksumMismatch);
        }
        
        return true;
    }
    
    std::expected<uint32_t, FirmwareError> get_hardware_id() {
        // Simulate hardware ID read
        return 0xABCD1234;
    }
};

void demonstrate_practical_firmware() {
    std::cout << "\n=== 9. PRACTICAL EXAMPLE: FIRMWARE STATUS ===" << std::endl;
    std::cout << "C++23 features in embedded firmware" << std::endl;
    
    FirmwareStatus firmware;
    
    std::cout << "\nInitializing firmware..." << std::endl;
    auto init_result = firmware.initialize();
    
    if (init_result) {
        std::cout << "✅ Firmware initialized successfully" << std::endl;
        
        auto hw_id = firmware.get_hardware_id();
        if (hw_id) {
            std::cout << "✅ Hardware ID: 0x" << std::hex << *hw_id << std::dec << std::endl;
        }
    } else {
        std::cout << "❌ Initialization failed: " 
                  << firmware_error_name(init_result.error()) << std::endl;
    }
    
    std::cout << "\n💡 This firmware code:" << std::endl;
    std::cout << "   • Uses std::expected (no exceptions)" << std::endl;
    std::cout << "   • Works with -fno-exceptions" << std::endl;
    std::cout << "   • Deterministic error handling" << std::endl;
    std::cout << "   • Suitable for safety-critical systems" << std::endl;
}

#else
void demonstrate_practical_firmware() {
    std::cout << "\n=== 9. PRACTICAL EXAMPLE: FIRMWARE STATUS ===" << std::endl;
    std::cout << "⚠️  Requires C++23 std::expected" << std::endl;
}
#endif

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  C++23 FEATURES FOR EMBEDDED SYSTEMS & FIRMWARE" << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "Focused on: Firmware, RTOS, Embedded Linux" << std::endl;
    
    demonstrate_expected();
    demonstrate_byteswap();
    demonstrate_to_underlying();
    demonstrate_constexpr_cmath();
    demonstrate_if_consteval();
    demonstrate_unreachable();
    demonstrate_size_literals();
    demonstrate_multidim_subscript();
    demonstrate_practical_firmware();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  C++23 FEATURES SUMMARY FOR EMBEDDED" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n🔥 MUST-HAVE FOR FIRMWARE:" << std::endl;
    std::cout << "\n1️⃣  std::expected<T, E>" << std::endl;
    std::cout << "   • No exception overhead" << std::endl;
    std::cout << "   • Works with -fno-exceptions" << std::endl;
    std::cout << "   • Explicit error handling" << std::endl;
    std::cout << "   • Perfect for RTOS and firmware" << std::endl;
    std::cout << "   Use case: Sensor reads, hardware init, protocol parsing" << std::endl;
    
    std::cout << "\n2️⃣  std::byteswap" << std::endl;
    std::cout << "   • Endianness conversion" << std::endl;
    std::cout << "   • Network protocol implementation" << std::endl;
    std::cout << "   • Binary file format parsing" << std::endl;
    std::cout << "   • Hardware register access" << std::endl;
    std::cout << "   Use case: TCP/IP stack, file systems, cross-platform data" << std::endl;
    
    std::cout << "\n3️⃣  std::to_underlying" << std::endl;
    std::cout << "   • Cleaner enum to integer conversion" << std::endl;
    std::cout << "   • Memory-mapped register access" << std::endl;
    std::cout << "   • GPIO pin manipulation" << std::endl;
    std::cout << "   • Hardware abstraction layers" << std::endl;
    std::cout << "   Use case: Register maps, pin assignments, state machines" << std::endl;
    
    std::cout << "\n4️⃣  constexpr <cmath>" << std::endl;
    std::cout << "   • Compile-time lookup table generation" << std::endl;
    std::cout << "   • Stored in ROM, not RAM" << std::endl;
    std::cout << "   • Zero runtime cost" << std::endl;
    std::cout << "   • Fast deterministic execution" << std::endl;
    std::cout << "   Use case: Sine/cosine tables, PWM values, calibration data" << std::endl;
    
    std::cout << "\n5️⃣  if consteval" << std::endl;
    std::cout << "   • Different paths for compile-time vs runtime" << std::endl;
    std::cout << "   • Compile-time: simple algorithm" << std::endl;
    std::cout << "   • Runtime: hardware accelerator" << std::endl;
    std::cout << "   Use case: CRC calculation, crypto, compression" << std::endl;
    
    std::cout << "\n6️⃣  std::unreachable" << std::endl;
    std::cout << "   • Compiler optimization hints" << std::endl;
    std::cout << "   • Smaller code size" << std::endl;
    std::cout << "   • Better branch prediction" << std::endl;
    std::cout << "   Use case: State machines, validated inputs, switch statements" << std::endl;
    
    std::cout << "\n7️⃣  Size literal suffix (uz)" << std::endl;
    std::cout << "   • Type-safe size comparisons" << std::endl;
    std::cout << "   • No signed/unsigned warnings" << std::endl;
    std::cout << "   • Cleaner buffer management code" << std::endl;
    std::cout << "   Use case: Buffer sizes, array indexing, memory allocation" << std::endl;
    
    std::cout << "\n8️⃣  Multidimensional operator[]" << std::endl;
    std::cout << "   • Direct matrix[row, col] syntax" << std::endl;
    std::cout << "   • Cleaner multi-dimensional arrays" << std::endl;
    std::cout << "   • Natural matrix/tensor access" << std::endl;
    std::cout << "   Use case: Image buffers, sensor arrays, DSP data" << std::endl;
    
    std::cout << "\n📊 COMPILER SUPPORT (January 2026):" << std::endl;
    std::cout << "\n  GCC 12+:" << std::endl;
    std::cout << "    • std::expected ❌ (use GCC 13+)" << std::endl;
    std::cout << "    • std::byteswap ✅" << std::endl;
    std::cout << "    • std::to_underlying ✅" << std::endl;
    std::cout << "    • if consteval ✅" << std::endl;
    
    std::cout << "\n  Clang 16+:" << std::endl;
    std::cout << "    • std::expected ✅" << std::endl;
    std::cout << "    • Most C++23 features ✅" << std::endl;
    
    std::cout << "\n  MSVC 2022 (17.6+):" << std::endl;
    std::cout << "    • Excellent C++23 support ✅" << std::endl;
    
    std::cout << "\n  ARM Compiler 6:" << std::endl;
    std::cout << "    • Based on Clang, good C++23 support ✅" << std::endl;
    
    std::cout << "\n🎯 RECOMMENDED EMBEDDED COMPILER FLAGS:" << std::endl;
    std::cout << "  -std=c++23              # Enable C++23" << std::endl;
    std::cout << "  -fno-exceptions         # Disable exceptions" << std::endl;
    std::cout << "  -fno-rtti              # Disable RTTI" << std::endl;
    std::cout << "  -Os or -O2             # Optimize for size/speed" << std::endl;
    std::cout << "  -flto                  # Link-time optimization" << std::endl;
    std::cout << "  -ffunction-sections    # Dead code elimination" << std::endl;
    
    std::cout << "\n💡 MIGRATION STRATEGY:" << std::endl;
    std::cout << "  1. Start with std::to_underlying (easy win)" << std::endl;
    std::cout << "  2. Add constexpr lookup tables (ROM savings)" << std::endl;
    std::cout << "  3. Replace error codes with std::expected (safer)" << std::endl;
    std::cout << "  4. Use std::byteswap for protocols (cleaner)" << std::endl;
    std::cout << "  5. Add std::unreachable for optimizations" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
