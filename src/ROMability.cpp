// ROMability.cpp
// Demonstrates ROM-ability in C++ - Placing data in Read-Only Memory (.rodata section)
//
// KEY CONCEPTS:
// 1. const - Runtime or compile-time constant, placed in ROM if possible
// 2. constexpr - Compile-time constant, guaranteed ROM placement
// 3. consteval - C++20, forced compile-time evaluation
// 4. constinit - C++20, compile-time initialization, runtime mutable
// 5. ROM vs RAM - .rodata vs .data/.bss sections
//
// EMBEDDED SYSTEMS CONTEXT:
// ROM (Read-Only Memory) - Flash memory, non-volatile, cheaper
// RAM (Random Access Memory) - SRAM, volatile, expensive on MCUs
// Goal: Maximize ROM usage, minimize RAM usage
//
// C++ ADVANTAGES OVER C:
// ✓ constexpr functions - compute at compile-time
// ✓ constexpr constructors - complex objects in ROM
// ✓ std::array - type-safe ROM arrays
// ✓ consteval - guarantee compile-time evaluation
// ✓ Template metaprogramming - generate ROM data

#include <iostream>
#include <array>
#include <string_view>
#include <cmath>
#include <iomanip>
#include <cstdint>

// ============================================================================
// SECTION 1: const vs constexpr - Understanding the Difference
// ============================================================================

namespace const_vs_constexpr {

// C-style const - MAY be in ROM, but not guaranteed
const int c_style_const = 42;

// C++ constexpr - GUARANTEED compile-time constant, always in ROM
constexpr int cpp_constexpr = 42;

// const with runtime initialization - goes to RAM!
int get_runtime_value() { return 42; }
const int runtime_const = get_runtime_value();  // RAM, not ROM!

// constexpr forces compile-time evaluation
constexpr int get_compile_time_value() { return 42; }
constexpr int compile_time_const = get_compile_time_value();  // ROM

// Complex compile-time calculation
constexpr int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

constexpr int fact_5 = factorial(5);  // Computed at compile-time, stored in ROM

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 1: const vs constexpr ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. Traditional const:\n";
    std::cout << "   const int c_style_const = 42;\n";
    std::cout << "   Value: " << c_style_const << "\n";
    std::cout << "   Location: Probably ROM, but not guaranteed\n\n";
    
    std::cout << "2. constexpr (C++11):\n";
    std::cout << "   constexpr int cpp_constexpr = 42;\n";
    std::cout << "   Value: " << cpp_constexpr << "\n";
    std::cout << "   Location: Guaranteed ROM (.rodata section)\n\n";
    
    std::cout << "3. const with runtime initialization:\n";
    std::cout << "   const int runtime_const = get_runtime_value();\n";
    std::cout << "   Value: " << runtime_const << "\n";
    std::cout << "   Location: RAM (.data section) - not ROM!\n\n";
    
    std::cout << "4. constexpr with compile-time function:\n";
    std::cout << "   constexpr int fact_5 = factorial(5);\n";
    std::cout << "   Value: " << fact_5 << " (computed at compile-time!)\n";
    std::cout << "   Location: ROM (.rodata section)\n\n";
    
    std::cout << "✓ RULE: Use constexpr for guaranteed ROM placement\n";
    std::cout << "✓ RULE: const doesn't guarantee ROM (can be initialized at runtime)\n";
}

} // namespace const_vs_constexpr

// ============================================================================
// SECTION 2: ROM-able Arrays - Lookup Tables
// ============================================================================

namespace rom_arrays {

// C-style array - ROM placement
constexpr int sine_table_c[16] = {
    0, 707, 1000, 707, 0, -707, -1000, -707,
    0, 707, 1000, 707, 0, -707, -1000, -707
};

// C++ std::array - ROM placement with type safety
constexpr std::array<int, 16> sine_table_cpp = {
    0, 707, 1000, 707, 0, -707, -1000, -707,
    0, 707, 1000, 707, 0, -707, -1000, -707
};

// Generate lookup table at compile-time
constexpr auto generate_sine_table() {
    std::array<int, 360> table{};
    for (int i = 0; i < 360; ++i) {
        // Note: std::sin is not constexpr before C++26
        // Using approximation for demonstration
        table[i] = static_cast<int>(1000 * std::sin(i * 3.14159 / 180.0));
    }
    return table;
}

// Entire lookup table generated at compile-time, stored in ROM
constexpr auto sine_lookup = generate_sine_table();

// PWM duty cycle table for LED brightness (gamma correction)
constexpr auto generate_gamma_table() {
    std::array<uint8_t, 256> table{};
    for (int i = 0; i < 256; ++i) {
        // Gamma correction: output = input^2.2
        double normalized = i / 255.0;
        double corrected = std::pow(normalized, 2.2);
        table[i] = static_cast<uint8_t>(corrected * 255.0);
    }
    return table;
}

constexpr auto gamma_table = generate_gamma_table();

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 2: ROM-able Arrays - Lookup Tables ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. C-style const array in ROM:\n";
    std::cout << "   constexpr int sine_table_c[16] = {...};\n";
    std::cout << "   Sample values: " << sine_table_c[0] << ", " 
              << sine_table_c[2] << ", " << sine_table_c[6] << "\n";
    std::cout << "   Size: " << sizeof(sine_table_c) << " bytes\n\n";
    
    std::cout << "2. C++ std::array in ROM (type-safe):\n";
    std::cout << "   constexpr std::array<int, 16> sine_table_cpp = {...};\n";
    std::cout << "   Sample values: " << sine_table_cpp[0] << ", " 
              << sine_table_cpp[2] << ", " << sine_table_cpp[6] << "\n";
    std::cout << "   Size: " << sine_table_cpp.size() << " elements\n\n";
    
    std::cout << "3. Generated lookup table (360 entries, compile-time):\n";
    std::cout << "   Sine values (degrees):\n";
    std::cout << "     0°: " << sine_lookup[0] << "\n";
    std::cout << "    30°: " << sine_lookup[30] << "\n";
    std::cout << "    60°: " << sine_lookup[60] << "\n";
    std::cout << "    90°: " << sine_lookup[90] << "\n";
    std::cout << "   Total size: " << sizeof(sine_lookup) << " bytes in ROM\n\n";
    
    std::cout << "4. Gamma correction table (256 entries):\n";
    std::cout << "   PWM duty cycles for linear brightness:\n";
    std::cout << "     Input 0: " << static_cast<int>(gamma_table[0]) << "\n";
    std::cout << "    Input 64: " << static_cast<int>(gamma_table[64]) << "\n";
    std::cout << "   Input 128: " << static_cast<int>(gamma_table[128]) << "\n";
    std::cout << "   Input 255: " << static_cast<int>(gamma_table[255]) << "\n";
    std::cout << "   Total size: " << sizeof(gamma_table) << " bytes in ROM\n\n";
    
    std::cout << "✓ Embedded benefit: Lookup tables computed at compile-time\n";
    std::cout << "✓ No runtime computation needed (saves CPU cycles)\n";
    std::cout << "✓ All stored in cheap ROM, not expensive RAM\n";
}

} // namespace rom_arrays

// ============================================================================
// SECTION 3: constexpr Objects - Complex Data in ROM
// ============================================================================

namespace constexpr_objects {

// Configuration stored entirely in ROM
struct DeviceConfig {
    const char* device_name;
    uint32_t serial_number;
    uint16_t max_voltage_mv;
    uint8_t num_channels;
    
    constexpr DeviceConfig(const char* name, uint32_t serial, 
                          uint16_t voltage, uint8_t channels)
        : device_name(name), serial_number(serial), 
          max_voltage_mv(voltage), num_channels(channels) {}
};

// Entire config in ROM
constexpr DeviceConfig device_config{
    "STM32F407",
    0x12345678,
    3300,  // 3.3V
    16
};

// Sensor calibration data in ROM
struct SensorCalibration {
    float offset;
    float gain;
    int16_t temp_coefficient;
    
    constexpr SensorCalibration(float o, float g, int16_t tc)
        : offset(o), gain(g), temp_coefficient(tc) {}
    
    constexpr float calibrate(int16_t raw_value, int16_t temperature) const {
        float temp_correction = temp_coefficient * (temperature - 25) / 1000.0f;
        return (raw_value + offset) * gain * (1.0f + temp_correction);
    }
};

constexpr SensorCalibration pressure_sensor{
    -50.0f,   // offset
    0.125f,   // gain
    -25       // temp coefficient (ppm/°C)
};

// Compile-time test of calibration
static_assert(pressure_sensor.calibrate(1000, 25) > 0, "Calibration sanity check");

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 3: constexpr Objects in ROM ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. Device configuration (ROM):\n";
    std::cout << "   Device: " << device_config.device_name << "\n";
    std::cout << "   Serial: 0x" << std::hex << device_config.serial_number 
              << std::dec << "\n";
    std::cout << "   Max Voltage: " << device_config.max_voltage_mv << " mV\n";
    std::cout << "   Channels: " << static_cast<int>(device_config.num_channels) << "\n";
    std::cout << "   Location: ROM (.rodata section)\n\n";
    
    std::cout << "2. Sensor calibration with compile-time function:\n";
    std::cout << "   Offset: " << pressure_sensor.offset << "\n";
    std::cout << "   Gain: " << pressure_sensor.gain << "\n";
    std::cout << "   Temp Coeff: " << pressure_sensor.temp_coefficient << " ppm/°C\n\n";
    
    std::cout << "   Runtime calibration:\n";
    int16_t raw = 1000;
    int16_t temp = 25;
    float calibrated = pressure_sensor.calibrate(raw, temp);
    std::cout << "     Raw value: " << raw << " at " << temp << "°C\n";
    std::cout << "     Calibrated: " << calibrated << " Pa\n";
    std::cout << "   Location: ROM (.rodata section)\n\n";
    
    std::cout << "✓ Complex objects can live entirely in ROM\n";
    std::cout << "✓ constexpr methods allow compile-time testing\n";
    std::cout << "✓ Perfect for embedded: config, calibration, constants\n";
}

} // namespace constexpr_objects

// ============================================================================
// SECTION 4: C++20 Features - consteval and constinit
// ============================================================================

namespace cpp20_features {

// consteval - MUST be evaluated at compile-time
consteval int must_be_compile_time(int n) {
    return n * n;
}

// This works - compile-time constant
constexpr int square_5 = must_be_compile_time(5);

// This would NOT compile:
// int x = 5;
// int result = must_be_compile_time(x);  // ERROR: x is not constexpr

// constinit - Compile-time initialization, but runtime mutable
constinit int initialized_at_compile_time = 42;

// Compare with const/constexpr
const int const_value = 42;              // Can't modify
constexpr int constexpr_value = 42;      // Can't modify, must be compile-time
// constinit allows modification but guarantees compile-time init

// Use case: Global variables that need compile-time init but runtime modification
constinit int error_count = 0;  // Initialized at compile-time to 0

// String literals in ROM
constexpr std::string_view device_name = "STM32F407";
constexpr std::string_view error_messages[] = {
    "No error",
    "Sensor timeout",
    "Invalid data",
    "Communication error"
};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 4: C++20 - consteval and constinit ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. consteval - Forced compile-time evaluation:\n";
    std::cout << "   consteval int must_be_compile_time(int n) { return n * n; }\n";
    std::cout << "   square_5 = " << square_5 << " (computed at compile-time)\n";
    std::cout << "   ✓ Guarantees no runtime computation\n";
    std::cout << "   ✓ Perfect for embedded: zero runtime cost\n\n";
    
    std::cout << "2. constinit - Compile-time init, runtime mutable:\n";
    std::cout << "   constinit int error_count = 0;\n";
    std::cout << "   Initial value: " << error_count << "\n";
    
    // Can modify at runtime (unlike const/constexpr)
    ++error_count;
    std::cout << "   After increment: " << error_count << "\n";
    std::cout << "   ✓ Initialized at compile-time (no static initialization order issues)\n";
    std::cout << "   ✓ Can be modified at runtime\n\n";
    
    std::cout << "3. String literals in ROM:\n";
    std::cout << "   constexpr std::string_view device_name = \"STM32F407\";\n";
    std::cout << "   Device: " << device_name << "\n";
    std::cout << "   String data in ROM, no heap allocation\n\n";
    
    std::cout << "   Error message table in ROM:\n";
    for (size_t i = 0; i < 4; ++i) {
        std::cout << "     Error " << i << ": " << error_messages[i] << "\n";
    }
    std::cout << "   All strings in ROM (.rodata section)\n\n";
    
    std::cout << "✓ consteval: Forces compile-time (zero runtime cost)\n";
    std::cout << "✓ constinit: Safe global init, runtime mutable\n";
    std::cout << "✓ string_view: String literals without heap\n";
}

} // namespace cpp20_features

// ============================================================================
// SECTION 5: Practical Embedded Example - Entire System Config in ROM
// ============================================================================

namespace embedded_example {

// Pin configuration
struct PinConfig {
    uint8_t port;
    uint8_t pin;
    const char* function;
    
    constexpr PinConfig(uint8_t p, uint8_t pn, const char* func)
        : port(p), pin(pn), function(func) {}
};

// All pin configs in ROM
constexpr std::array<PinConfig, 8> pin_configs = {{
    {0, 0, "UART_TX"},
    {0, 1, "UART_RX"},
    {0, 5, "SPI_SCK"},
    {0, 6, "SPI_MISO"},
    {0, 7, "SPI_MOSI"},
    {1, 0, "I2C_SCL"},
    {1, 1, "I2C_SDA"},
    {2, 13, "LED"}
}};

// Timer configuration
struct TimerConfig {
    uint32_t frequency_hz;
    uint16_t prescaler;
    uint16_t period;
    
    constexpr TimerConfig(uint32_t freq) 
        : frequency_hz(freq),
          prescaler(calculate_prescaler(freq)),
          period(calculate_period(freq, calculate_prescaler(freq))) {}
    
private:
    static constexpr uint16_t calculate_prescaler(uint32_t freq) {
        uint32_t divisor = freq * 65536;
        if (divisor == 0) return 1;
        return static_cast<uint16_t>(84000000 / divisor + 1);
    }
    
    static constexpr uint16_t calculate_period(uint32_t freq, uint16_t prescaler) {
        if (freq == 0 || prescaler == 0) return 1000;
        return static_cast<uint16_t>(84000000 / (freq * prescaler));
    }
};

// Timer configs computed at compile-time, stored in ROM
constexpr TimerConfig pwm_timer{1000};      // 1 kHz PWM
constexpr TimerConfig adc_timer{10000};     // 10 kHz ADC sampling
constexpr TimerConfig led_timer{2};         // 2 Hz LED blink

// Memory map - register addresses
namespace registers {
    constexpr uint32_t GPIO_BASE = 0x40020000;
    constexpr uint32_t UART_BASE = 0x40011000;
    constexpr uint32_t SPI_BASE = 0x40013000;
    constexpr uint32_t I2C_BASE = 0x40005400;
    constexpr uint32_t TIM_BASE = 0x40000000;
}

// Protocol constants
namespace can_protocol {
    constexpr uint32_t CAN_ID_STATUS = 0x100;
    constexpr uint32_t CAN_ID_SENSOR1 = 0x201;
    constexpr uint32_t CAN_ID_SENSOR2 = 0x202;
    constexpr uint32_t CAN_ID_COMMAND = 0x300;
    
    constexpr std::array<uint32_t, 4> filter_ids = {
        CAN_ID_STATUS, CAN_ID_SENSOR1, CAN_ID_SENSOR2, CAN_ID_COMMAND
    };
}

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 5: Embedded System - All Config in ROM ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "1. Pin Configuration Table (ROM):\n";
    for (const auto& pin : pin_configs) {
        std::cout << "   Port " << static_cast<int>(pin.port) 
                  << ", Pin " << static_cast<int>(pin.pin) 
                  << ": " << pin.function << "\n";
    }
    std::cout << "   Size: " << sizeof(pin_configs) << " bytes in ROM\n\n";
    
    std::cout << "2. Timer Configurations (computed at compile-time):\n";
    std::cout << "   PWM Timer:\n";
    std::cout << "     Frequency: " << pwm_timer.frequency_hz << " Hz\n";
    std::cout << "     Prescaler: " << pwm_timer.prescaler << "\n";
    std::cout << "     Period: " << pwm_timer.period << "\n\n";
    
    std::cout << "   ADC Timer:\n";
    std::cout << "     Frequency: " << adc_timer.frequency_hz << " Hz\n";
    std::cout << "     Prescaler: " << adc_timer.prescaler << "\n";
    std::cout << "     Period: " << adc_timer.period << "\n\n";
    
    std::cout << "3. Register Addresses (ROM):\n";
    std::cout << "   GPIO_BASE: 0x" << std::hex << registers::GPIO_BASE << "\n";
    std::cout << "   UART_BASE: 0x" << registers::UART_BASE << "\n";
    std::cout << "   SPI_BASE:  0x" << registers::SPI_BASE << "\n";
    std::cout << "   I2C_BASE:  0x" << registers::I2C_BASE << std::dec << "\n\n";
    
    std::cout << "4. CAN Protocol IDs (ROM):\n";
    for (size_t i = 0; i < can_protocol::filter_ids.size(); ++i) {
        std::cout << "   Filter " << i << ": 0x" << std::hex 
                  << can_protocol::filter_ids[i] << std::dec << "\n";
    }
    std::cout << "\n✓ EMBEDDED BENEFITS:\n";
    std::cout << "  • Zero RAM usage for configuration\n";
    std::cout << "  • All configs in cheap Flash ROM\n";
    std::cout << "  • Compile-time validation with static_assert\n";
    std::cout << "  • No runtime initialization overhead\n";
    std::cout << "  • Type-safe, no magic numbers\n";
}

} // namespace embedded_example

// ============================================================================
// SECTION 6: Verifying ROM Placement - Compiler Output
// ============================================================================

namespace verifying_placement {

// These should go to ROM (.rodata)
constexpr int rom_value = 42;
constexpr std::array<int, 10> rom_array = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const char* const rom_string = "This is in ROM";

// These go to RAM (.data or .bss)
int ram_value = 42;
int ram_array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

void demonstrate() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== SECTION 6: Verifying ROM Placement ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "To verify ROM vs RAM placement, use these tools:\n\n";
    
    std::cout << "1. Using 'nm' command:\n";
    std::cout << "   $ nm ROMability | grep rom_value\n";
    std::cout << "   Output shows section:\n";
    std::cout << "     'r' or 'R' = .rodata (ROM)\n";
    std::cout << "     'd' or 'D' = .data (RAM)\n";
    std::cout << "     'b' or 'B' = .bss (uninitialized RAM)\n\n";
    
    std::cout << "2. Using 'objdump' command:\n";
    std::cout << "   $ objdump -h ROMability\n";
    std::cout << "   Shows sections:\n";
    std::cout << "     .rodata - Read-only data (ROM/Flash)\n";
    std::cout << "     .data   - Initialized data (RAM)\n";
    std::cout << "     .bss    - Uninitialized data (RAM)\n\n";
    
    std::cout << "3. Using 'size' command:\n";
    std::cout << "   $ size ROMability\n";
    std::cout << "   Shows memory usage:\n";
    std::cout << "     text   - Code + .rodata (ROM/Flash)\n";
    std::cout << "     data   - Initialized data (RAM)\n";
    std::cout << "     bss    - Uninitialized data (RAM)\n\n";
    
    std::cout << "4. GCC-specific: __attribute__((section))\n";
    std::cout << "   Can explicitly place data in specific section:\n";
    std::cout << "   __attribute__((section(\".rodata\"))) const int x = 42;\n\n";
    
    std::cout << "5. Embedded: Check linker map file\n";
    std::cout << "   Linker script (*.ld) and map file show exact placement\n";
    std::cout << "   Critical for embedded systems with limited RAM\n\n";
    
    std::cout << "✓ Best Practices:\n";
    std::cout << "  • Use constexpr for guaranteed ROM placement\n";
    std::cout << "  • Verify with nm/objdump during development\n";
    std::cout << "  • Check map file for embedded targets\n";
    std::cout << "  • Profile RAM usage on resource-constrained systems\n";
}

} // namespace verifying_placement

// ============================================================================
// SECTION 7: Summary and Best Practices
// ============================================================================

void show_summary() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== ROM-ability in C++ - Complete Summary ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "┌────────────────┬─────────────────┬──────────┬─────────────┐\n";
    std::cout << "│  KEYWORD       │  COMPILE-TIME   │ LOCATION │  MUTABLE    │\n";
    std::cout << "├────────────────┼─────────────────┼──────────┼─────────────┤\n";
    std::cout << "│  const         │  Maybe          │ ROM/RAM  │  No         │\n";
    std::cout << "│  constexpr     │  Yes            │ ROM      │  No         │\n";
    std::cout << "│  consteval     │  Must be        │ ROM      │  No         │\n";
    std::cout << "│  constinit     │  Yes (init)     │ RAM      │  Yes        │\n";
    std::cout << "└────────────────┴─────────────────┴──────────┴─────────────┘\n\n";
    
    std::cout << "WHEN TO USE WHAT:\n\n";
    
    std::cout << "1. constexpr - Most common for ROM data\n";
    std::cout << "   • Lookup tables (sine, gamma, CRC, etc.)\n";
    std::cout << "   • Configuration structs\n";
    std::cout << "   • Constants and arrays\n";
    std::cout << "   • Compile-time computed values\n\n";
    
    std::cout << "2. consteval (C++20) - Force compile-time\n";
    std::cout << "   • Functions that MUST run at compile-time\n";
    std::cout << "   • Zero runtime cost guarantee\n";
    std::cout << "   • Configuration validation\n\n";
    
    std::cout << "3. constinit (C++20) - Global variables\n";
    std::cout << "   • Globals that need compile-time init\n";
    std::cout << "   • Avoids static initialization order fiasco\n";
    std::cout << "   • Runtime mutable (unlike constexpr)\n\n";
    
    std::cout << "4. const - Use sparingly\n";
    std::cout << "   • May or may not be ROM\n";
    std::cout << "   • Prefer constexpr for ROM guarantee\n\n";
    
    std::cout << "C++ ADVANTAGES OVER C:\n";
    std::cout << "  ✓ constexpr functions - compute at compile-time\n";
    std::cout << "  ✓ constexpr constructors - complex objects in ROM\n";
    std::cout << "  ✓ std::array - type-safe ROM arrays\n";
    std::cout << "  ✓ consteval - guarantee compile-time\n";
    std::cout << "  ✓ Templates - generate ROM data\n";
    std::cout << "  ✓ static_assert - compile-time validation\n\n";
    
    std::cout << "EMBEDDED SYSTEMS BENEFITS:\n";
    std::cout << "  ✓ Maximize ROM usage (cheap Flash)\n";
    std::cout << "  ✓ Minimize RAM usage (expensive SRAM)\n";
    std::cout << "  ✓ Zero runtime initialization overhead\n";
    std::cout << "  ✓ Compile-time validation\n";
    std::cout << "  ✓ No dynamic memory allocation\n";
    std::cout << "  ✓ Deterministic memory layout\n\n";
    
    std::cout << "BEST PRACTICES:\n";
    std::cout << "  1. Use constexpr by default for constants\n";
    std::cout << "  2. Generate lookup tables at compile-time\n";
    std::cout << "  3. Store configuration in ROM\n";
    std::cout << "  4. Use std::array instead of C arrays\n";
    std::cout << "  5. Use string_view for ROM strings\n";
    std::cout << "  6. Verify placement with nm/objdump\n";
    std::cout << "  7. Profile RAM usage on embedded targets\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              ROM-ability in C++ (constexpr & More)             ║\n";
    std::cout << "║         Placing Data in Read-Only Memory (.rodata)             ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    // Section 1: const vs constexpr
    const_vs_constexpr::demonstrate();
    
    // Section 2: ROM arrays
    rom_arrays::demonstrate();
    
    // Section 3: constexpr objects
    constexpr_objects::demonstrate();
    
    // Section 4: C++20 features
    cpp20_features::demonstrate();
    
    // Section 5: Embedded example
    embedded_example::demonstrate();
    
    // Section 6: Verification
    verifying_placement::demonstrate();
    
    // Section 7: Summary
    show_summary();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All demonstrations completed!\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "KEY TAKEAWAY: Use constexpr for guaranteed ROM placement!\n";
    std::cout << "Perfect for embedded systems: maximize ROM, minimize RAM.\n\n";
    
    return 0;
}
