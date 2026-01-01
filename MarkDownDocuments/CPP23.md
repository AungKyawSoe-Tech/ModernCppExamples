# C++23 Features for Embedded Systems, Firmware, and RTOS

## Table of Contents
1. [Overview](#overview)
2. [std::expected - Exception-Free Error Handling](#stdexpected---exception-free-error-handling)
3. [std::byteswap - Endianness Conversion](#stdbyteswap---endianness-conversion)
4. [std::to_underlying - Enum to Integer](#stdto_underlying---enum-to-integer)
5. [constexpr for <cmath> - Compile-Time Math](#constexpr-for-cmath---compile-time-math)
6. [if consteval - Compile-Time vs Runtime Paths](#if-consteval---compile-time-vs-runtime-paths)
7. [std::unreachable - Optimization Hints](#stdunreachable---optimization-hints)
8. [Size Literal Suffix (uz/z) - Type-Safe Sizes](#size-literal-suffix-uzz---type-safe-sizes)
9. [Multidimensional Subscript Operator](#multidimensional-subscript-operator)
10. [Other C++23 Features](#other-c23-features)
11. [Compiler Support](#compiler-support)
12. [Migration Strategy](#migration-strategy)

---

## Overview

C++23 introduces features particularly valuable for embedded systems, firmware development, and RTOS environments:

- **Zero-overhead abstractions** for resource-constrained systems
- **Exception-free error handling** with `std::expected`
- **Hardware interfacing** improvements with `std::byteswap` and `std::to_underlying`
- **Compile-time optimizations** with `constexpr` math and `if consteval`
- **Code size reductions** with `std::unreachable`
- **Type safety** improvements for buffer management

### Target Platforms
- ARM Cortex-M microcontrollers
- RTOS (FreeRTOS, Zephyr, ThreadX)
- Embedded Linux (Yocto, Buildroot)
- Bare-metal firmware

---

## std::expected - Exception-Free Error Handling

### Problem
Traditional exception handling:
- Requires RTTI and exception tables → Code bloat
- Non-deterministic execution time → RTOS issues
- Stack unwinding overhead → Performance penalty
- Often disabled in embedded: `-fno-exceptions`

### Solution: std::expected<T, E>

```cpp
#include <expected>

enum class SensorError {
    NotConnected,
    ReadTimeout,
    InvalidData,
    CalibrationFailed
};

// Return either value or error - no exceptions!
std::expected<float, SensorError> read_temperature_sensor() {
    // Check sensor connection
    if (!sensor_connected()) {
        return std::unexpected(SensorError::NotConnected);
    }
    
    // Read sensor with timeout
    auto data = sensor_read();
    if (!data) {
        return std::unexpected(SensorError::ReadTimeout);
    }
    
    // Validate data
    if (!is_valid(*data)) {
        return std::unexpected(SensorError::InvalidData);
    }
    
    return *data;  // Success!
}

// Usage
void process_temperature() {
    auto result = read_temperature_sensor();
    
    if (result) {
        // Success path
        float temp = *result;
        display_temperature(temp);
    } else {
        // Error path
        handle_error(result.error());
    }
}
```

### Monadic Operations

```cpp
// Chain operations elegantly
std::expected<float, SensorError> read_and_convert() {
    return read_temperature_sensor()
        .and_then([](float celsius) -> std::expected<float, SensorError> {
            if (celsius < -273.15f) {
                return std::unexpected(SensorError::InvalidData);
            }
            return celsius * 9.0f / 5.0f + 32.0f;  // Convert to Fahrenheit
        })
        .or_else([](SensorError err) -> std::expected<float, SensorError> {
            log_error(err);
            return use_cached_value();  // Fallback
        });
}
```

### Embedded Benefits
✅ **Zero exception overhead** - Works with `-fno-exceptions`  
✅ **Explicit error handling** - No hidden control flow  
✅ **Deterministic** - Predictable execution time for RTOS  
✅ **Type-safe** - Compiler enforces error checking  
✅ **Composable** - Monadic interface for chaining operations  

### Use Cases
- Sensor data acquisition
- Hardware initialization
- Protocol parsing (UART, SPI, I2C)
- File system operations
- Network communication
- Flash memory operations

---

## std::byteswap - Endianness Conversion

### Problem
Embedded systems frequently need endianness conversion:
- Network protocols (TCP/IP) use big-endian
- Most CPUs (ARM, x86) are little-endian
- File formats have defined byte order
- Hardware registers may use different endianness

### Solution: std::byteswap

```cpp
#include <bit>

// Network byte order (big-endian) to host byte order
uint32_t network_to_host(uint32_t network_value) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return std::byteswap(network_value);
    #else
        return network_value;
    #endif
}

// Parse TCP/IP packet header
struct TCPHeader {
    uint16_t source_port;
    uint16_t dest_port;
    uint32_t sequence_num;
    uint32_t ack_num;
    // ...
};

void parse_tcp_header(const uint8_t* data, TCPHeader& header) {
    uint16_t src_port_raw;
    std::memcpy(&src_port_raw, data, 2);
    header.source_port = std::byteswap(src_port_raw);  // Network to host
    
    uint16_t dst_port_raw;
    std::memcpy(&dst_port_raw, data + 2, 2);
    header.dest_port = std::byteswap(dst_port_raw);
    
    // ...
}
```

### Hardware Register Access

```cpp
// Memory-mapped 32-bit register with swapped byte order
volatile uint32_t* const DEVICE_REG = reinterpret_cast<uint32_t*>(0x40000000);

void write_device_register(uint32_t value) {
    *DEVICE_REG = std::byteswap(value);  // Convert to device byte order
}

uint32_t read_device_register() {
    return std::byteswap(*DEVICE_REG);  // Convert to host byte order
}
```

### Before C++23

```cpp
// Manual bit shifting
uint32_t byteswap_manual(uint32_t x) {
    return ((x & 0xFF000000) >> 24) |
           ((x & 0x00FF0000) >> 8)  |
           ((x & 0x0000FF00) << 8)  |
           ((x & 0x000000FF) << 24);
}

// Compiler intrinsics (non-portable)
uint32_t value = __builtin_bswap32(data);  // GCC/Clang
uint32_t value = _byteswap_ulong(data);    // MSVC
```

### Embedded Benefits
✅ **Portable** - Works across all platforms  
✅ **Optimized** - Compilers use single instruction (REV on ARM)  
✅ **Type-safe** - Works with uint16_t, uint32_t, uint64_t  
✅ **Constexpr** - Can be used at compile time  

### Use Cases
- TCP/IP stack implementation
- USB, Bluetooth, CAN protocols
- Binary file format parsing
- Flash memory data structures
- Cross-platform data exchange
- Hardware abstraction layers

---

## std::to_underlying - Enum to Integer

### Problem
Converting enum class to underlying integer type:

```cpp
// Before C++23 - verbose and error-prone
enum class GPIOPin : uint8_t { PIN_0 = 0, PIN_1 = 1, PIN_LED = 13 };

auto pin_num = static_cast<std::underlying_type_t<GPIOPin>>(GPIOPin::PIN_LED);
//             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Too verbose!
```

### Solution: std::to_underlying

```cpp
#include <utility>

enum class GPIOPin : uint8_t {
    PIN_0 = 0,
    PIN_1 = 1,
    PIN_2 = 2,
    PIN_LED = 13,
    PIN_BUTTON = 7
};

// Clean and concise
auto pin_number = std::to_underlying(GPIOPin::PIN_LED);  // Returns uint8_t
```

### Memory-Mapped Registers

```cpp
enum class MemoryRegister : uint32_t {
    STATUS_REG  = 0x40000000,
    CONTROL_REG = 0x40000004,
    DATA_REG    = 0x40000008,
    CONFIG_REG  = 0x4000000C
};

void write_register(MemoryRegister reg, uint32_t value) {
    volatile uint32_t* reg_ptr = 
        reinterpret_cast<volatile uint32_t*>(std::to_underlying(reg));
    *reg_ptr = value;
}

uint32_t read_register(MemoryRegister reg) {
    volatile uint32_t* reg_ptr = 
        reinterpret_cast<volatile uint32_t*>(std::to_underlying(reg));
    return *reg_ptr;
}
```

### GPIO Abstraction

```cpp
class GPIO {
    std::array<bool, 32> pin_states{};
    
public:
    void set_pin(GPIOPin pin, bool state) {
        auto index = std::to_underlying(pin);
        pin_states[index] = state;
        
        // Write to hardware register
        hardware_write_pin(index, state);
    }
    
    bool get_pin(GPIOPin pin) const {
        return pin_states[std::to_underlying(pin)];
    }
};
```

### Embedded Benefits
✅ **Cleaner syntax** - Less boilerplate  
✅ **Type-safe** - Preserves underlying type  
✅ **constexpr** - Compile-time evaluation  
✅ **Readable** - Intent is clear  

### Use Cases
- GPIO pin manipulation
- Hardware register access
- Interrupt vector indexing
- State machine states
- Protocol command codes
- Device addressing

---

## constexpr for <cmath> - Compile-Time Math

### Problem
Runtime math calculations:
- Consume CPU cycles at runtime
- Require floating-point unit (FPU)
- May have non-deterministic execution time
- Waste power on battery-powered devices

### Solution: constexpr <cmath>

C++23 makes `<cmath>` functions `constexpr`:
- `std::sin`, `std::cos`, `std::tan`
- `std::sqrt`, `std::pow`, `std::exp`
- `std::log`, `std::log10`, `std::log2`
- Many more...

```cpp
#include <cmath>
#include <array>

// Generate sine lookup table at compile time
constexpr std::array<float, 256> generate_sine_table() {
    std::array<float, 256> table{};
    constexpr float PI = 3.14159265358979323846f;
    
    for (size_t i = 0; i < 256; ++i) {
        table[i] = std::sin(2.0f * PI * i / 256.0f);
    }
    
    return table;
}

// Table stored in ROM/Flash, not RAM!
constexpr auto SINE_LOOKUP = generate_sine_table();

// Fast runtime lookup
float fast_sin(uint8_t angle) {
    return SINE_LOOKUP[angle];
}
```

### PWM Duty Cycle Calculations

```cpp
// Calculate PWM values at compile time
constexpr uint16_t pwm_duty_cycle(float percent) {
    return static_cast<uint16_t>(percent * 65535.0f / 100.0f);
}

// All calculated at compile time
constexpr uint16_t PWM_0   = pwm_duty_cycle(0.0f);     // 0
constexpr uint16_t PWM_25  = pwm_duty_cycle(25.0f);    // 16383
constexpr uint16_t PWM_50  = pwm_duty_cycle(50.0f);    // 32767
constexpr uint16_t PWM_75  = pwm_duty_cycle(75.0f);    // 49151
constexpr uint16_t PWM_100 = pwm_duty_cycle(100.0f);   // 65535

void set_led_brightness(uint8_t brightness_percent) {
    if (brightness_percent == 0)   set_pwm(PWM_0);
    else if (brightness_percent <= 25)  set_pwm(PWM_25);
    else if (brightness_percent <= 50)  set_pwm(PWM_50);
    else if (brightness_percent <= 75)  set_pwm(PWM_75);
    else set_pwm(PWM_100);
}
```

### Calibration Data

```cpp
// Temperature sensor calibration curve at compile time
constexpr std::array<float, 101> generate_calibration_curve() {
    std::array<float, 101> curve{};
    
    for (size_t i = 0; i <= 100; ++i) {
        float raw = static_cast<float>(i);
        // Polynomial calibration: temp = a + b*x + c*x^2
        curve[i] = 25.0f + 0.5f * raw + 0.001f * std::pow(raw, 2.0f);
    }
    
    return curve;
}

constexpr auto TEMP_CALIBRATION = generate_calibration_curve();

float calibrated_temperature(uint8_t raw_adc_value) {
    return TEMP_CALIBRATION[raw_adc_value];
}
```

### Embedded Benefits
✅ **Zero runtime cost** - All calculations at compile time  
✅ **ROM/Flash storage** - Not consuming RAM  
✅ **Deterministic** - Instant lookup, no calculations  
✅ **Power efficient** - No FPU usage at runtime  
✅ **Fast** - Array lookup vs transcendental functions  

### Use Cases
- Sine/cosine lookup tables (motor control, DSP)
- PWM duty cycle values
- ADC calibration curves
- Filter coefficients (IIR, FIR)
- PID controller constants
- Gamma correction tables (displays)

---

## if consteval - Compile-Time vs Runtime Paths

### Problem
Different implementations needed for:
- Compile-time: Simple, portable algorithm
- Runtime: Hardware accelerator or optimized ASM

### Solution: if consteval

```cpp
constexpr uint32_t crc32_compute(const char* data, size_t len) {
    if consteval {
        // Compile-time: Simple bit-by-bit algorithm
        uint32_t crc = 0xFFFFFFFF;
        for (size_t i = 0; i < len; ++i) {
            crc ^= static_cast<uint32_t>(data[i]);
            for (int bit = 0; bit < 8; ++bit) {
                crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
            }
        }
        return ~crc;
    } else {
        // Runtime: Use hardware CRC peripheral
        return hardware_crc32(data, len);  // Fast hardware accelerator
    }
}

// Compile-time CRC for firmware version string
constexpr uint32_t FIRMWARE_CRC = crc32_compute("FW_V1.0.0", 9);

// Runtime CRC for received data
void process_packet(const uint8_t* data, size_t len) {
    uint32_t packet_crc = crc32_compute(reinterpret_cast<const char*>(data), len);
    // Uses hardware accelerator at runtime
}
```

### AES Encryption

```cpp
constexpr std::array<uint8_t, 16> aes_encrypt(
    const std::array<uint8_t, 16>& plaintext,
    const std::array<uint8_t, 16>& key
) {
    if consteval {
        // Compile-time: Reference implementation
        return aes_encrypt_reference(plaintext, key);
    } else {
        // Runtime: Hardware AES accelerator
        return aes_encrypt_hardware(plaintext, key);
    }
}

// Encrypt firmware key at compile time
constexpr auto ENCRYPTED_KEY = aes_encrypt(SECRET_KEY, MASTER_KEY);

// Encrypt data at runtime using hardware
auto encrypted_data = aes_encrypt(sensor_data, session_key);
```

### Memory Allocation Strategy

```cpp
template<typename T, size_t N>
constexpr auto allocate_buffer() {
    if consteval {
        // Compile-time: Use std::array
        return std::array<T, N>{};
    } else {
        // Runtime: Could use malloc or custom allocator
        // (this is conceptual - real implementation differs)
        return std::array<T, N>{};
    }
}
```

### Embedded Benefits
✅ **Hardware acceleration** - Use peripherals at runtime  
✅ **Portable** - Simple algorithm for compile-time  
✅ **Efficient** - Best of both worlds  
✅ **Maintainable** - Single function, two implementations  

### Use Cases
- CRC/checksum calculations
- Cryptographic operations (AES, SHA)
- Data compression
- String hashing
- Mathematical operations
- Memory management

---

## std::unreachable - Optimization Hints

### Problem
Compiler cannot prove certain code paths are impossible:
- Extra bounds checking
- Dead code paths
- Suboptimal branch prediction

### Solution: std::unreachable

```cpp
#include <utility>

enum class DeviceState : uint8_t {
    IDLE    = 0,
    RUNNING = 1,
    STOPPED = 2,
    ERROR   = 3
};

// Compiler knows all cases are covered
[[nodiscard]] constexpr const char* state_name(DeviceState state) {
    switch (state) {
        case DeviceState::IDLE:    return "IDLE";
        case DeviceState::RUNNING: return "RUNNING";
        case DeviceState::STOPPED: return "STOPPED";
        case DeviceState::ERROR:   return "ERROR";
    }
    std::unreachable();  // All cases handled, this is impossible
}
```

### Validated Input Processing

```cpp
// Input is pre-validated: 0 <= cmd <= 3
uint32_t process_command(uint8_t cmd) {
    // Validation done by caller
    assert(cmd <= 3);
    
    if (cmd == 0) return handle_reset();
    if (cmd == 1) return handle_start();
    if (cmd == 2) return handle_stop();
    if (cmd == 3) return handle_status();
    
    // Compiler: "cmd must be 0-3, no need for bounds check"
    std::unreachable();
}
```

### State Machine Optimization

```cpp
class StateMachine {
    enum class State { INIT, READY, BUSY, DONE };
    State current_state = State::INIT;
    
    void transition(State new_state) {
        // State transitions are controlled and validated
        switch (new_state) {
            case State::INIT:  handle_init();  break;
            case State::READY: handle_ready(); break;
            case State::BUSY:  handle_busy();  break;
            case State::DONE:  handle_done();  break;
        }
        std::unreachable();  // All states handled
    }
};
```

### Assembly Comparison

```cpp
// Without std::unreachable - compiler generates bounds checking
const char* get_name_slow(uint8_t id) {
    if (id == 0) return "ZERO";
    if (id == 1) return "ONE";
    if (id == 2) return "TWO";
    // Compiler adds: "what if id > 2?"
    return "UNKNOWN";  // Dead code, but compiler doesn't know
}

// With std::unreachable - no bounds check needed
const char* get_name_fast(uint8_t id) {
    if (id == 0) return "ZERO";
    if (id == 1) return "ONE";
    if (id == 2) return "TWO";
    std::unreachable();  // Compiler: "id must be 0-2"
}
```

### Embedded Benefits
✅ **Smaller code size** - Eliminates dead code  
✅ **Faster execution** - Better branch prediction  
✅ **Optimization** - Compiler can make assumptions  
✅ **Documentation** - Expresses intent clearly  

### ⚠️ WARNING
Reaching `std::unreachable()` is **undefined behavior**! Only use when you can guarantee the code path is truly impossible.

### Use Cases
- Exhaustive switch statements on enum class
- Validated input processing
- State machines
- Protocol parsers with known formats
- Array indexing with validated bounds

---

## Size Literal Suffix (uz/z) - Type-Safe Sizes

### Problem
Comparing `size_t` with integer literals causes warnings:

```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};

// Warning: comparison between signed and unsigned
if (vec.size() > 3) { }   // 3 is int, vec.size() is size_t

// Wrong type - unsigned int, not size_t
if (vec.size() > 3u) { }  // May work on 32-bit, fails on 64-bit

// Verbose
if (vec.size() > static_cast<size_t>(3)) { }  // Too much typing
```

### Solution: uz suffix

```cpp
#include <vector>

std::vector<uint8_t> buffer = {1, 2, 3, 4, 5};

// C++23: Type-safe size literal
if (buffer.size() > 3uz) {  // uz = size_t literal
    // Process buffer
}

// Array indexing
constexpr size_t MAX_BUFFER_SIZE = 256uz;
std::array<uint8_t, MAX_BUFFER_SIZE> uart_buffer{};

// No warning
if (uart_buffer.size() == MAX_BUFFER_SIZE) { }
```

### Embedded Buffer Management

```cpp
// DMA buffer sizes - type-safe
constexpr size_t DMA_BUFFER_SIZE  = 512uz;
constexpr size_t UART_BUFFER_SIZE = 256uz;
constexpr size_t SPI_BUFFER_SIZE  = 128uz;
constexpr size_t I2C_BUFFER_SIZE  = 64uz;

class SerialPort {
    std::array<uint8_t, UART_BUFFER_SIZE> rx_buffer{};
    size_t rx_count = 0;
    
public:
    bool write_data(const uint8_t* data, size_t len) {
        // Type-safe comparison
        if (rx_count + len > UART_BUFFER_SIZE) {
            return false;  // Buffer overflow
        }
        
        std::copy_n(data, len, rx_buffer.begin() + rx_count);
        rx_count += len;
        return true;
    }
};
```

### Loop Iteration

```cpp
void process_sensor_array() {
    constexpr size_t NUM_SENSORS = 16uz;
    std::array<float, NUM_SENSORS> sensor_readings{};
    
    // Type-safe loop
    for (size_t i = 0uz; i < NUM_SENSORS; ++i) {
        sensor_readings[i] = read_sensor(i);
    }
    
    // Range-based for is still preferred when possible
    for (auto& reading : sensor_readings) {
        process(reading);
    }
}
```

### Embedded Benefits
✅ **No warnings** - Correct type from the start  
✅ **Portable** - Works on 32-bit and 64-bit  
✅ **Readable** - Clear intent (size, not count)  
✅ **Safe** - No implicit conversions  

### Use Cases
- Buffer size definitions
- Array indexing
- Container size comparisons
- DMA transfer sizes
- Memory allocation sizes

---

## Multidimensional Subscript Operator

### Problem
Accessing 2D/3D arrays was awkward:

```cpp
// C++20 and before
matrix[row][col]        // Requires double indirection
matrix.at(row, col)     // Member function, not operator
matrix.get(row, col)    // Custom function
```

### Solution: Multi-dimensional operator[]

```cpp
template<typename T, size_t Rows, size_t Cols>
class Matrix {
    std::array<T, Rows * Cols> data{};
    
public:
    // C++23: Multi-dimensional subscript operator
    constexpr T& operator[](size_t row, size_t col) {
        return data[row * Cols + col];
    }
    
    constexpr const T& operator[](size_t row, size_t col) const {
        return data[row * Cols + col];
    }
};

// Natural syntax
Matrix<float, 3, 3> sensor_data;
sensor_data[0, 0] = 1.5f;  // Clean and intuitive!
sensor_data[2, 1] = 3.7f;
```

### Image Buffer

```cpp
template<typename PixelType>
class ImageBuffer {
    size_t width, height;
    std::vector<PixelType> pixels;
    
public:
    ImageBuffer(size_t w, size_t h) 
        : width(w), height(h), pixels(w * h) {}
    
    // Access pixel at (x, y)
    PixelType& operator[](size_t x, size_t y) {
        return pixels[y * width + x];
    }
    
    const PixelType& operator[](size_t x, size_t y) const {
        return pixels[y * width + x];
    }
};

// Usage
ImageBuffer<uint8_t> camera_frame(640, 480);
camera_frame[320, 240] = 255;  // Set center pixel to white
```

### 3D Voxel Grid

```cpp
template<typename T, size_t X, size_t Y, size_t Z>
class VoxelGrid {
    std::array<T, X * Y * Z> data{};
    
public:
    constexpr T& operator[](size_t x, size_t y, size_t z) {
        return data[x + y * X + z * X * Y];
    }
};

// 3D temperature sensor array
VoxelGrid<float, 10, 10, 10> temp_grid;
temp_grid[5, 5, 5] = 25.5f;  // Center of cube
```

### Embedded Benefits
✅ **Intuitive syntax** - Natural array access  
✅ **Type-safe** - Compiler checks dimensions  
✅ **Zero overhead** - Inlined to direct access  
✅ **Flexible** - Works with any number of dimensions  

### Use Cases
- Image/video frame buffers
- Sensor arrays (2D/3D)
- Matrix operations
- Game grids/maps
- Voxel data structures
- Multi-dimensional LUTs

---

## Other C++23 Features

### 1. Deducing `this`
Simplifies CRTP and recursive lambdas:

```cpp
struct Device {
    void configure(this auto&& self) {
        // self deduced to Device& or const Device&
        self.setup_hardware();
    }
};
```

### 2. Literal suffix for `size_t`
Already covered: `uz` and `z`

### 3. `std::print` and `std::println`
Type-safe formatted output (minimal use in embedded due to no `<iostream>`):

```cpp
#include <print>
std::print("Temperature: {}°C\n", temperature);
```

### 4. `#warning` directive
Portable warning messages:

```cpp
#warning "This code path is deprecated"
```

### 5. Preprocessor `#elifdef` and `#elifndef`

```cpp
#ifdef CONFIG_SENSOR_A
    // Use sensor A
#elifdef CONFIG_SENSOR_B
    // Use sensor B
#else
    #error "No sensor configured"
#endif
```

### 6. Multidimensional array `operator[]`
Already covered above.

### 7. `constexpr` `<cstdlib>` functions
- `std::abs`, `std::div`, etc. are now `constexpr`

### 8. Extended floating-point types
- `std::float16_t`, `std::float32_t`, `std::float64_t`, `std::bfloat16_t`

---

## Compiler Support

### GCC
| Feature | GCC 12 | GCC 13 | GCC 14+ |
|---------|--------|--------|---------|
| `std::expected` | ❌ | ✅ | ✅ |
| `std::byteswap` | ✅ | ✅ | ✅ |
| `std::to_underlying` | ✅ | ✅ | ✅ |
| `if consteval` | ✅ | ✅ | ✅ |
| `std::unreachable` | ✅ | ✅ | ✅ |
| Size suffix `uz` | ✅ | ✅ | ✅ |
| Multi-dim `operator[]` | ❌ | ✅ | ✅ |

**Recommended**: GCC 13+ for full C++23 support

### Clang
| Feature | Clang 15 | Clang 16+ |
|---------|----------|-----------|
| `std::expected` | ❌ | ✅ |
| `std::byteswap` | ✅ | ✅ |
| `std::to_underlying` | ✅ | ✅ |
| `if consteval` | ✅ | ✅ |
| `std::unreachable` | ✅ | ✅ |
| Size suffix `uz` | ✅ | ✅ |
| Multi-dim `operator[]` | ✅ | ✅ |

**Recommended**: Clang 16+ for full C++23 support

### MSVC
| Feature | MSVC 2022 (17.6+) |
|---------|-------------------|
| All C++23 features | ✅ Excellent support |

### ARM Compiler 6
Based on Clang/LLVM:
- Version 6.18+ has good C++23 support
- Update regularly for latest features

### Embedded Toolchains
- **GNU ARM Embedded**: Use GCC 13.2+ arm-none-eabi
- **Zephyr SDK**: GCC 12.2+ (partial C++23)
- **ESP-IDF**: GCC 12.2+ (configure with `-std=c++23`)
- **PlatformIO**: Set `platform = espressif32@6.0.0` or newer

---

## Migration Strategy

### Phase 1: Low-Hanging Fruit (Week 1)
1. **Replace enum casts with `std::to_underlying`**
   ```cpp
   // Before
   auto pin = static_cast<std::underlying_type_t<GPIOPin>>(GPIOPin::LED);
   
   // After
   auto pin = std::to_underlying(GPIOPin::LED);
   ```

2. **Add size literal suffix `uz`**
   ```cpp
   // Before
   if (buffer.size() > 256) { }
   
   // After
   if (buffer.size() > 256uz) { }
   ```

3. **Use `std::unreachable` in switches**
   ```cpp
   switch (state) {
       case State::A: return "A";
       case State::B: return "B";
   }
   std::unreachable();  // Add this
   ```

### Phase 2: Compile-Time Optimizations (Week 2-3)
1. **Generate lookup tables with constexpr**
   ```cpp
   constexpr auto SINE_TABLE = generate_sine_lookup();
   constexpr auto GAMMA_TABLE = generate_gamma_correction();
   ```

2. **Add `if consteval` for hardware paths**
   ```cpp
   if consteval {
       return software_crc(data);
   } else {
       return hardware_crc(data);
   }
   ```

### Phase 3: Replace Error Codes (Week 4-6)
1. **Start with leaf functions**
   ```cpp
   // Before
   int read_sensor(float* out_value) {
       if (!sensor_ready()) return -1;
       *out_value = sensor_read();
       return 0;
   }
   
   // After
   std::expected<float, SensorError> read_sensor() {
       if (!sensor_ready()) {
           return std::unexpected(SensorError::NotReady);
       }
       return sensor_read();
   }
   ```

2. **Propagate upward through call stack**

3. **Remove exception handling (if using)**
   - Enable `-fno-exceptions` compiler flag
   - Verify binary size reduction

### Phase 4: Hardware Interfacing (Ongoing)
1. **Use `std::byteswap` for protocols**
   - Network stacks (TCP/IP)
   - File formats
   - Hardware registers

2. **Apply multi-dimensional operator[]**
   - Image buffers
   - Sensor arrays
   - Matrix operations

### Compiler Flag Recommendations
```makefile
# C++23 compilation flags for embedded
CXXFLAGS = -std=c++23         # Enable C++23
CXXFLAGS += -fno-exceptions   # Disable exceptions (use std::expected)
CXXFLAGS += -fno-rtti         # Disable RTTI
CXXFLAGS += -Os               # Optimize for size
CXXFLAGS += -flto             # Link-time optimization
CXXFLAGS += -ffunction-sections -fdata-sections  # Dead code elimination
LDFLAGS = -Wl,--gc-sections   # Garbage collect unused sections
```

### Testing Strategy
1. **Unit tests** with GoogleTest/Catch2
2. **Static analysis** with clang-tidy
3. **Binary size comparison** before/after
4. **Runtime benchmarks** for critical paths
5. **Code coverage** analysis

---

## Summary

### Top 3 Features for Embedded Systems

**🥇 `std::expected<T, E>`**
- Exception-free error handling
- Works with `-fno-exceptions`
- Deterministic for RTOS

**🥈 `std::byteswap`**
- Portable endianness conversion
- Single instruction on most CPUs
- Essential for protocols

**🥉 `constexpr <cmath>`**
- Compile-time lookup tables
- Zero runtime cost
- Stored in ROM/Flash

### Binary Size Impact
- `std::expected` vs exceptions: **-15% to -30%** code size
- `constexpr` tables: **+ROM**, **-RAM**, **-CPU cycles**
- `std::unreachable`: **-5% to -10%** code size in state machines

### When to Use C++23
✅ **New firmware projects** - Start with C++23  
✅ **Refactoring** - Migrate incrementally  
✅ **Safety-critical** - `std::expected` improves error handling  
✅ **Resource-constrained** - Compile-time optimizations save resources  

### When to Stay with C++17/20
❌ **Legacy toolchain** - Compiler doesn't support C++23  
❌ **Third-party dependencies** - Libraries require older standard  
❌ **Team unfamiliar** - Training needed first  

---

## References

- [C++23 Standard (N4950)](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf)
- [cppreference.com - C++23](https://en.cppreference.com/w/cpp/23)
- [GCC C++23 Status](https://gcc.gnu.org/projects/cxx-status.html#cxx23)
- [Clang C++23 Status](https://clang.llvm.org/cxx_status.html#cxx23)
- [MSVC C++23 Status](https://docs.microsoft.com/en-us/cpp/overview/visual-cpp-language-conformance)

---

## Example Code

Complete runnable examples: [Cpp23Examples.cpp](../src/Cpp23Examples.cpp)

Compilation:
```bash
# Linux/WSL
g++ -std=c++23 -Wall -O2 -o Cpp23Examples src/Cpp23Examples.cpp
./Cpp23Examples

# Windows (MSVC)
cl /std:c++latest /O2 /W4 src\Cpp23Examples.cpp
Cpp23Examples.exe

# ARM Embedded
arm-none-eabi-g++ -std=c++23 -mcpu=cortex-m4 -mthumb -O2 \
    -fno-exceptions -fno-rtti -o firmware.elf src/Cpp23Examples.cpp
```

---

**Last Updated**: January 1, 2026  
**Tested With**: GCC 13.2, Clang 17, MSVC 17.8
