#include <iostream>
#include <array>
#include <cstdint>
#include <algorithm>
#include <type_traits>
#include <optional>
#include <variant>

// ===================================================================
// FUNCTIONAL SAFETY C++ AND STL USAGE ON STM32
// ===================================================================
// This file demonstrates:
// - STL usage on STM32 MCUs vs STM32MPs (with MMU)
// - IEC-61508 SIL-3 compliant C++ patterns
// - ISO 26262 ASIL-D automotive safety requirements
// - MISRA C++ and AUTOSAR C++ compliance
// - Safe alternatives to dynamic features
// ===================================================================

// ===================================================================
// 1. STL USAGE ON STM32 MCUs (Cortex-M, No MMU)
// ===================================================================

namespace stm32_mcu {

std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║       STL USAGE ON STM32 MCUs (Cortex-M, No MMU)             ║
╚═══════════════════════════════════════════════════════════════╝

📌 TARGET: STM32F4/F7/H7 Series
  • ARM Cortex-M4/M7
  • No MMU (Memory Management Unit)
  • RAM: 64KB - 1MB
  • Flash: 256KB - 2MB
  • Real-time constraints

✅ SAFE STL SUBSET (No dynamic allocation):
)" << std::endl;

void demonstrate_safe_stl() {
    std::cout << "\n=== SAFE STL FOR MCUs ===" << std::endl;
    
    // ✅ Fixed-size containers
    std::array<int, 10> sensors{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::cout << "  ✓ std::array<T, N> - Stack allocated, no overhead" << std::endl;
    
    // ✅ Non-allocating algorithms
    auto max_sensor = std::max_element(sensors.begin(), sensors.end());
    std::cout << "  ✓ std::algorithm - Zero overhead abstractions" << std::endl;
    std::cout << "    Max sensor value: " << *max_sensor << std::endl;
    
    // ✅ Optional for error handling (no exceptions)
    std::optional<float> reading = std::nullopt;
    if (sensors[0] > 0) {
        reading = 25.5f;
    }
    std::cout << "  ✓ std::optional<T> - Stack-based error handling" << std::endl;
    
    // ✅ Variant for type-safe unions
    std::variant<int, float, bool> sensor_value = 42;
    std::cout << "  ✓ std::variant<T...> - Type-safe unions (C++17)" << std::endl;
    
    // ✅ String view (no allocation)
    constexpr std::string_view device_name = "STM32H7";
    std::cout << "  ✓ std::string_view - Non-owning string reference" << std::endl;
    std::cout << "    Device: " << device_name << std::endl;
}

std::cout << R"(
⚠️ USE WITH CAUTION (Bounded/Pre-allocated):

  std::vector<T> - IF:
    • reserve() called at startup
    • Never exceeds capacity
    • Not used in ISRs
    
  Example:
    std::vector<float> buffer;
    buffer.reserve(1000);  // One-time allocation
    // Now push_back() won't allocate if size < 1000
)" << std::endl;

void demonstrate_bounded_vector() {
    std::cout << "\n=== BOUNDED VECTOR (Pre-allocated) ===" << std::endl;
    
    std::vector<float> sensor_buffer;
    sensor_buffer.reserve(100);  // Allocate once at startup
    
    std::cout << "  Initial capacity: " << sensor_buffer.capacity() << std::endl;
    std::cout << "  Size: " << sensor_buffer.size() << std::endl;
    
    // Safe if we stay within capacity
    for (int i = 0; i < 50; ++i) {
        if (sensor_buffer.size() < sensor_buffer.capacity()) {
            sensor_buffer.push_back(i * 0.1f);  // No allocation
        }
    }
    
    std::cout << "  After 50 pushes - capacity: " << sensor_buffer.capacity() << std::endl;
    std::cout << "  Size: " << sensor_buffer.size() << " (no reallocation)" << std::endl;
}

std::cout << R"(
❌ AVOID IN MCUs (Dynamic allocation):

  std::vector<T> (unbounded growth)
  std::map<K,V> (tree nodes allocated individually)
  std::list<T> (each element allocated)
  std::string (dynamic resizing)
  std::shared_ptr<T> (atomic overhead + allocation)
  std::function<> (type erasure + allocation)
)" << std::endl;

} // namespace stm32_mcu

// ===================================================================
// 2. STL USAGE ON STM32MP (Cortex-A with MMU)
// ===================================================================

namespace stm32_mp {

std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║       STL USAGE ON STM32MP (Cortex-A with MMU)               ║
╚═══════════════════════════════════════════════════════════════╝

📌 TARGET: STM32MP1/MP2 Series
  • ARM Cortex-A7/A35
  • WITH MMU (Memory Management Unit)
  • RAM: 512MB - 4GB (Linux/Embedded Linux)
  • Virtual memory support
  • Less stringent real-time constraints

✅ FULL STL SUPPORT:

  • All STL containers (vector, map, unordered_map, etc.)
  • Dynamic allocation without fragmentation concerns
  • Exception handling enabled
  • RTTI available
  • Standard C++ development practices

🎯 KEY DIFFERENCE: MMU provides:
  • Virtual memory (no fragmentation issues)
  • Memory protection (page faults)
  • Swap space (if configured)
  • Standard malloc/new implementations

💡 RECOMMENDATION:
  Use STL freely as on desktop/server environments.
  Standard Linux C++ development applies.
)" << std::endl;

void demonstrate_full_stl() {
    std::cout << "\n=== FULL STL ON STM32MP (with MMU) ===" << std::endl;
    
    // All features available
    std::vector<int> dynamic_data = {1, 2, 3, 4, 5};
    dynamic_data.push_back(6);  // Safe, MMU handles memory
    
    std::map<std::string, float> sensor_map;
    sensor_map["temperature"] = 25.5f;
    sensor_map["humidity"] = 60.2f;
    
    try {
        dynamic_data.at(100);  // Exceptions work
    } catch (const std::out_of_range& e) {
        std::cout << "  ✓ Exception handling works: " << e.what() << std::endl;
    }
    
    std::cout << "  ✓ Full STL container support" << std::endl;
    std::cout << "  ✓ Dynamic allocation without concerns" << std::endl;
    std::cout << "  ✓ Exception handling enabled" << std::endl;
}

} // namespace stm32_mp

// ===================================================================
// 3. IEC-61508 SIL-3 FUNCTIONAL SAFETY REQUIREMENTS
// ===================================================================

namespace iec61508_sil3 {

std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║           IEC-61508 SIL-3 C++ REQUIREMENTS                    ║
╚═══════════════════════════════════════════════════════════════╝

🔒 FUNCTIONAL SAFETY CODING STANDARD

SIL-3 (Safety Integrity Level 3):
  • Probability of dangerous failure: 10⁻⁷ to 10⁻⁸ per hour
  • Examples: Railway signaling, industrial safety systems
  • Requires: MISRA C++, formal verification, 100% code coverage

╔═══════════════════════════════════════════════════════════════╗
║  FEATURE            │  ALLOWED?  │  REASON                    ║
╠═════════════════════╪════════════╪════════════════════════════╣
║ Dynamic allocation  │     ❌     │ Non-deterministic behavior ║
║ Heap (new/delete)   │     ❌     │ Fragmentation risk         ║
║ Exceptions          │     ❌     │ Hidden control flow        ║
║ RTTI                │     ❌     │ Runtime overhead           ║
║ Virtual functions   │     ⚠️     │ Discouraged (indirect)     ║
║ Templates           │     ✅     │ Compile-time (OK)          ║
║ std::array          │     ✅     │ Fixed-size, stack          ║
║ std::vector         │     ❌     │ Dynamic allocation         ║
║ std::string         │     ❌     │ Dynamic allocation         ║
║ std::optional       │     ✅     │ Stack-based (C++17)        ║
║ Algorithms          │     ✅     │ Deterministic              ║
╚═════════════════════╧════════════╧════════════════════════════╝

🔧 REQUIRED COMPILER FLAGS:
  -fno-exceptions          (Disable exceptions)
  -fno-rtti                (Disable RTTI)
  -fno-use-cxa-atexit      (Disable static destructors)
  -Werror                  (Treat warnings as errors)
  -Wall -Wextra -Wpedantic (Enable all warnings)
)" << std::endl;

// ===================================================================
// SIL-3 COMPLIANT: Error Handling Without Exceptions
// ===================================================================

enum class SafetyResult : uint8_t {
    OK = 0,
    INVALID_PARAMETER = 1,
    BUFFER_OVERFLOW = 2,
    SENSOR_FAULT = 3,
    CHECKSUM_ERROR = 4,
    OUT_OF_RANGE = 5
};

class SafeDataProcessor {
private:
    static constexpr size_t MAX_SAMPLES = 128;
    std::array<int16_t, MAX_SAMPLES> sampleBuffer{};
    size_t sampleCount{0};
    
    static constexpr int16_t MIN_VALID = -1000;
    static constexpr int16_t MAX_VALID = 1000;
    
public:
    SafeDataProcessor() = default;
    
    // Explicit error handling - no exceptions
    SafetyResult addSample(int16_t sample) {
        // Input validation
        if (sample < MIN_VALID || sample > MAX_VALID) {
            return SafetyResult::OUT_OF_RANGE;
        }
        
        // Bounds check
        if (sampleCount >= MAX_SAMPLES) {
            return SafetyResult::BUFFER_OVERFLOW;
        }
        
        // Safe operation
        sampleBuffer[sampleCount++] = sample;
        return SafetyResult::OK;
    }
    
    // Deterministic processing
    int32_t computeAverage() const {
        if (sampleCount == 0) {
            return 0;
        }
        
        int32_t sum = 0;
        for (size_t i = 0; i < sampleCount; ++i) {
            sum += sampleBuffer[i];
        }
        
        return sum / static_cast<int32_t>(sampleCount);
    }
    
    void reset() {
        sampleCount = 0;
        sampleBuffer.fill(0);  // Clear for security
    }
    
    size_t getCount() const { return sampleCount; }
    size_t getCapacity() const { return MAX_SAMPLES; }
};

void demonstrate_sil3_code() {
    std::cout << "\n=== SIL-3 COMPLIANT CODE ===" << std::endl;
    
    SafeDataProcessor processor;
    
    std::cout << "  Buffer capacity: " << processor.getCapacity() << " samples" << std::endl;
    
    // Add valid samples
    for (int16_t i = 0; i < 10; ++i) {
        SafetyResult result = processor.addSample(i * 50);
        if (result != SafetyResult::OK) {
            std::cout << "  ❌ Error adding sample: " 
                      << static_cast<int>(result) << std::endl;
            return;
        }
    }
    
    std::cout << "  ✓ Added 10 samples successfully" << std::endl;
    std::cout << "  Average: " << processor.computeAverage() << std::endl;
    
    // Test error handling
    SafetyResult result = processor.addSample(5000);  // Out of range
    if (result == SafetyResult::OUT_OF_RANGE) {
        std::cout << "  ✓ Out-of-range detection works" << std::endl;
    }
}

// ===================================================================
// SIL-3 COMPLIANT: Static Polymorphism (No Virtual Functions)
// ===================================================================

template<typename Derived>
class SensorBase {
public:
    // CRTP - Compile-time polymorphism
    float readValue() {
        return static_cast<Derived*>(this)->readValueImpl();
    }
    
    const char* getName() const {
        return static_cast<const Derived*>(this)->getNameImpl();
    }
};

class TemperatureSensor : public SensorBase<TemperatureSensor> {
public:
    float readValueImpl() {
        // Simulate ADC read
        return 25.5f;
    }
    
    const char* getNameImpl() const {
        return "Temperature";
    }
};

class PressureSensor : public SensorBase<PressureSensor> {
public:
    float readValueImpl() {
        // Simulate ADC read
        return 101.3f;
    }
    
    const char* getNameImpl() const {
        return "Pressure";
    }
};

void demonstrate_crtp() {
    std::cout << "\n=== CRTP (Static Polymorphism) ===" << std::endl;
    
    TemperatureSensor tempSensor;
    PressureSensor pressSensor;
    
    std::cout << "  " << tempSensor.getName() << ": " 
              << tempSensor.readValue() << "°C" << std::endl;
    std::cout << "  " << pressSensor.getName() << ": " 
              << pressSensor.readValue() << " kPa" << std::endl;
    
    std::cout << "  ✓ Zero runtime overhead" << std::endl;
    std::cout << "  ✓ All calls resolved at compile-time" << std::endl;
    std::cout << "  ✓ No vtable indirection" << std::endl;
}

} // namespace iec61508_sil3

// ===================================================================
// 4. ISO 26262 ASIL-D (AUTOMOTIVE SAFETY)
// ===================================================================

namespace iso26262_asil_d {

std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║           ISO 26262 ASIL-D (AUTOMOTIVE SAFETY)                ║
╚═══════════════════════════════════════════════════════════════╝

🚗 AUTOMOTIVE FUNCTIONAL SAFETY

ASIL-D (Automotive Safety Integrity Level D):
  • Highest automotive safety level
  • Examples: Airbag control, brake-by-wire, steering
  • Requires: AUTOSAR C++, MISRA C++, formal methods

📋 AUTOSAR C++14 CODING STANDARD

Allowed STL Subset:
  ✅ std::array
  ✅ std::numeric_limits
  ✅ std::move, std::forward
  ✅ std::initializer_list
  ✅ Algorithms (<algorithm>)
  ✅ Type traits (<type_traits>)
  ✅ std::unique_ptr (with custom allocator)

NOT Allowed:
  ❌ std::vector, std::list, std::deque
  ❌ std::map, std::set, std::unordered_map
  ❌ std::string
  ❌ std::shared_ptr
  ❌ std::function
  ❌ Exceptions (try/catch/throw)
  ❌ RTTI (dynamic_cast, typeid)
  ❌ Multiple inheritance (except interfaces)

🔍 ADDITIONAL REQUIREMENTS:
  • MC/DC code coverage (Modified Condition/Decision Coverage)
  • Static analysis (Polyspace, LDRA, Coverity)
  • Formal verification for critical functions
  • Traceability: Requirements → Code → Tests
  • Certified toolchain (TÜV-certified compiler)
)" << std::endl;

// ===================================================================
// ASIL-D COMPLIANT: Safety-Critical Control System
// ===================================================================

enum class BrakeCommand : uint8_t {
    RELEASE = 0,
    APPLY_LIGHT = 25,
    APPLY_MEDIUM = 50,
    APPLY_FULL = 100
};

enum class BrakeStatus : uint8_t {
    OK = 0,
    SENSOR_FAULT = 1,
    ACTUATOR_FAULT = 2,
    INVALID_COMMAND = 3
};

class SafetyBrakeController {
private:
    std::array<uint16_t, 4> wheelSpeeds{};  // Fixed-size
    BrakeCommand lastCommand{BrakeCommand::RELEASE};
    uint32_t faultCounter{0};
    
    static constexpr uint16_t MAX_SPEED_DIFF = 100;  // RPM
    static constexpr uint32_t MAX_FAULTS = 3;
    
    // Validate wheel speeds are consistent
    bool validateWheelSpeeds() const {
        uint16_t minSpeed = wheelSpeeds[0];
        uint16_t maxSpeed = wheelSpeeds[0];
        
        for (size_t i = 1; i < wheelSpeeds.size(); ++i) {
            if (wheelSpeeds[i] < minSpeed) minSpeed = wheelSpeeds[i];
            if (wheelSpeeds[i] > maxSpeed) maxSpeed = wheelSpeeds[i];
        }
        
        return (maxSpeed - minSpeed) <= MAX_SPEED_DIFF;
    }
    
public:
    SafetyBrakeController() = default;
    
    // Update wheel speed sensor data
    void updateWheelSpeed(size_t wheel, uint16_t speed) {
        if (wheel < wheelSpeeds.size()) {
            wheelSpeeds[wheel] = speed;
        }
    }
    
    // Apply brake command with safety checks
    BrakeStatus applyBrake(BrakeCommand command) {
        // Validate wheel speeds
        if (!validateWheelSpeeds()) {
            ++faultCounter;
            if (faultCounter >= MAX_FAULTS) {
                return BrakeStatus::SENSOR_FAULT;
            }
        } else {
            faultCounter = 0;  // Reset on valid reading
        }
        
        // Validate command
        if (static_cast<uint8_t>(command) > 100) {
            return BrakeStatus::INVALID_COMMAND;
        }
        
        // Apply brake (would interface with hardware)
        lastCommand = command;
        
        return BrakeStatus::OK;
    }
    
    BrakeCommand getLastCommand() const { return lastCommand; }
    uint32_t getFaultCounter() const { return faultCounter; }
};

void demonstrate_asil_d_code() {
    std::cout << "\n=== ASIL-D COMPLIANT BRAKE CONTROLLER ===" << std::endl;
    
    SafetyBrakeController controller;
    
    // Update wheel speeds
    controller.updateWheelSpeed(0, 1000);
    controller.updateWheelSpeed(1, 1005);
    controller.updateWheelSpeed(2, 998);
    controller.updateWheelSpeed(3, 1002);
    
    std::cout << "  Wheel speeds updated (all consistent)" << std::endl;
    
    // Apply brake
    BrakeStatus status = controller.applyBrake(BrakeCommand::APPLY_MEDIUM);
    if (status == BrakeStatus::OK) {
        std::cout << "  ✓ Brake applied successfully" << std::endl;
    } else {
        std::cout << "  ❌ Brake fault: " << static_cast<int>(status) << std::endl;
    }
    
    std::cout << "  Fault counter: " << controller.getFaultCounter() << std::endl;
}

} // namespace iso26262_asil_d

// ===================================================================
// 5. COMPARISON TABLE: WHEN TO USE WHAT
// ===================================================================

void print_comparison_table() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════════════════════╗
║                    C++ FEATURE USAGE BY PLATFORM/STANDARD                     ║
╠═══════════════════════╦═══════════╦══════════╦═══════════╦════════════════════╣
║ FEATURE              ║ STM32 MCU ║ STM32 MP ║ SIL-3     ║ ASIL-D            ║
║                      ║ (No MMU)  ║ (+ MMU)  ║ (IEC-61508)║ (ISO 26262)      ║
╠═══════════════════════╬═══════════╬══════════╬═══════════╬════════════════════╣
║ Heap allocation      ║     ⚠️    ║    ✅    ║     ❌    ║     ❌            ║
║ new/delete           ║     ⚠️    ║    ✅    ║     ❌    ║     ❌            ║
║ Exceptions           ║     ❌    ║    ✅    ║     ❌    ║     ❌            ║
║ RTTI                 ║     ❌    ║    ✅    ║     ❌    ║     ❌            ║
║ Virtual functions    ║     ⚠️    ║    ✅    ║     ⚠️    ║     ⚠️            ║
║ std::array           ║     ✅    ║    ✅    ║     ✅    ║     ✅            ║
║ std::vector          ║     ⚠️    ║    ✅    ║     ❌    ║     ❌            ║
║ std::string          ║     ❌    ║    ✅    ║     ❌    ║     ❌            ║
║ std::map             ║     ❌    ║    ✅    ║     ❌    ║     ❌            ║
║ std::optional        ║     ✅    ║    ✅    ║     ✅    ║     ✅            ║
║ std::variant         ║     ✅    ║    ✅    ║     ✅    ║     ✅            ║
║ std::unique_ptr      ║     ⚠️    ║    ✅    ║     ⚠️    ║     ✅*           ║
║ std::shared_ptr      ║     ❌    ║    ✅    ║     ❌    ║     ❌            ║
║ Algorithms           ║     ✅    ║    ✅    ║     ✅    ║     ✅            ║
║ Templates            ║     ✅    ║    ✅    ║     ✅    ║     ✅            ║
║ constexpr            ║     ✅    ║    ✅    ║     ✅    ║     ✅            ║
╚═══════════════════════╩═══════════╩══════════╩═══════════╩════════════════════╝

Legend:
  ✅ = Fully supported and recommended
  ⚠️ = Allowed with restrictions/caution
  ❌ = Not allowed or strongly discouraged
  * = With custom allocator only

)" << std::endl;
}

// ===================================================================
// 6. PRACTICAL RECOMMENDATIONS
// ===================================================================

void print_recommendations() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                  PRACTICAL RECOMMENDATIONS                     ║
╚═══════════════════════════════════════════════════════════════╝

🎯 FOR HIGH-END STM32 MCUs (H7/F7 - Non-Safety):

  Strategy: Bounded allocation at startup
  
  ✅ DO:
    • Reserve vector capacity during initialization
    • Use std::array for fixed-size data
    • Pre-allocate all resources at startup
    • Profile memory usage
    • Monitor heap fragmentation
    
  📝 Example:
    void system_init() {
        dataLogger.reserve(10000);  // Once at startup
        eventQueue.reserve(1000);
        // Never allocate in runtime loops
    }

🔒 FOR FUNCTIONAL SAFETY (SIL-3/ASIL-D):

  Strategy: Zero dynamic allocation
  
  ✅ DO:
    • Use std::array exclusively
    • Error codes instead of exceptions
    • CRTP instead of virtual functions
    • Static analysis tools (mandatory)
    • MC/DC code coverage
    • Formal verification
    
  📋 Required Standards:
    • MISRA C++ 2023
    • AUTOSAR C++14 (automotive)
    • IEC-61508 guidelines
    • DO-178C (aerospace)

🚀 FOR STM32MP (with MMU/Linux):

  Strategy: Standard C++ development
  
  ✅ DO:
    • Use full STL as needed
    • Exception handling for errors
    • Standard memory management
    • Focus on software quality over constraints
    
  💡 Treat like desktop/server development

📚 RECOMMENDED LIBRARIES FOR EMBEDDED:

  • ETL (Embedded Template Library)
    - Fixed-capacity containers
    - No dynamic allocation
    - Safety-critical compatible
    - https://github.com/ETLCPP/etl
    
  • Abseil (Google)
    - Flat containers (better cache locality)
    - Swiss tables (fast hash maps)
    - Selected components work on embedded
    
  • {fmt} library
    - Faster than iostream
    - Type-safe formatting
    - Compile-time format checking

🛠️ COMPILER CONFIGURATION:

  For MCU (non-safety):
    -Os -fno-rtti -fno-exceptions
    -ffunction-sections -fdata-sections
    -Wl,--gc-sections
    
  For SIL-3/ASIL-D:
    -Os -fno-rtti -fno-exceptions -fno-use-cxa-atexit
    -Werror -Wall -Wextra -Wpedantic
    -fno-threadsafe-statics
    [+ static analysis tools]

)" << std::endl;
}

// ===================================================================
// MAIN DEMONSTRATION
// ===================================================================

int main() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║     FUNCTIONAL SAFETY C++ & STL USAGE ON STM32 PLATFORMS      ║
║                                                               ║
║  Topics Covered:                                              ║
║  1. STL on STM32 MCUs (Cortex-M, no MMU)                     ║
║  2. STL on STM32MP (Cortex-A, with MMU)                      ║
║  3. IEC-61508 SIL-3 functional safety requirements           ║
║  4. ISO 26262 ASIL-D automotive safety                       ║
║  5. Comparison and recommendations                            ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;

    // Part 1: STM32 MCU examples
    stm32_mcu::demonstrate_safe_stl();
    stm32_mcu::demonstrate_bounded_vector();
    
    // Part 2: STM32MP examples
    stm32_mp::demonstrate_full_stl();
    
    // Part 3: IEC-61508 SIL-3 examples
    iec61508_sil3::demonstrate_sil3_code();
    iec61508_sil3::demonstrate_crtp();
    
    // Part 4: ISO 26262 ASIL-D examples
    iso26262_asil_d::demonstrate_asil_d_code();
    
    // Part 5: Comparison and recommendations
    print_comparison_table();
    print_recommendations();
    
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                          KEY TAKEAWAYS                         ║
╚═══════════════════════════════════════════════════════════════╝

1. STM32 MCUs (no MMU): 
   → Use STL carefully, prefer fixed-size containers
   
2. STM32MP (with MMU): 
   → Full STL support, standard C++ development
   
3. Functional Safety (SIL-3/ASIL-D): 
   → Minimal C++, no dynamic features, formal verification
   
4. Rule of Thumb:
   → The stricter the safety requirement, the simpler the C++ subset

5. Always consider:
   → Memory constraints
   → Real-time requirements
   → Safety certification needs
   → Tool qualification requirements

)" << std::endl;

    return 0;
}
