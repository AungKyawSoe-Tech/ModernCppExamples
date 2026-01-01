# Templated Camera Interface for Multiple Pixel Types

## Overview

This example demonstrates how to use C++ templates to create a flexible camera interface that works with different pixel data types, from 8-bit unsigned integers to 64-bit double-precision floating-point values.

## Motivation

In computer vision and image processing applications, different cameras and sensors produce data in various formats:

- **Consumer webcams**: 8-bit grayscale or RGB (uint8_t)
- **Scientific cameras**: 12-bit or 16-bit (uint16_t)
- **Depth sensors**: 32-bit floating point (float)
- **Astronomy/Research**: 64-bit double precision (double)

Using templates allows us to write generic code that works with all these pixel types without code duplication or runtime overhead.

## Code Structure

### 1. Image Class Template

```cpp
template<typename PixelType>
class Image {
private:
    size_t width, height;
    std::vector<PixelType> pixels;
public:
    // Pixel access
    PixelType& at(size_t x, size_t y);
    const PixelType& at(size_t x, size_t y) const;
    
    // Raw data access for hardware interface
    PixelType* data();
};
```

The `Image` class is templated on `PixelType`, allowing it to store:
- `Image<uint8_t>` - 8-bit grayscale
- `Image<uint16_t>` - 16-bit high dynamic range
- `Image<float>` - normalized floating point
- `Image<double>` - high-precision data

### 2. Camera Interface Template

```cpp
template<typename PixelType>
class Camera {
public:
    virtual Image<PixelType> capture() = 0;
    
    static constexpr size_t bits_per_pixel() {
        return sizeof(PixelType) * 8;
    }
    
    static constexpr bool is_floating_point() {
        return std::is_floating_point_v<PixelType>;
    }
};
```

The abstract `Camera` class provides:
- Pure virtual `capture()` method
- Compile-time pixel type information via `constexpr` functions
- Type traits for checking if pixel type is floating point or integer

### 3. Concrete Camera Implementations

```cpp
class Camera8bit : public Camera<uint8_t> { /* ... */ };
class Camera16bit : public Camera<uint16_t> { /* ... */ };
class CameraFloat : public Camera<float> { /* ... */ };
class CameraDouble : public Camera<double> { /* ... */ };
```

Each concrete camera implementation inherits from the templated base class with a specific pixel type.

### 4. Generic Image Processing Algorithms

```cpp
template<typename PixelType>
class ImageProcessor {
public:
    static double calculate_mean(const Image<PixelType>& img);
    static std::pair<PixelType, PixelType> find_min_max(const Image<PixelType>& img);
    static Image<PixelType> scale(const Image<PixelType>& img, double factor);
    static Image<PixelType> threshold(const Image<PixelType>& img, PixelType threshold);
};
```

Processing algorithms are templated so they work with any pixel type. The same code handles:
- Integer arithmetic for uint8_t/uint16_t
- Floating-point arithmetic for float/double

### 5. Type Conversion Between Pixel Types

```cpp
template<typename DestType, typename SrcType>
Image<DestType> convert_image(const Image<SrcType>& src) {
    // Automatic scaling between different ranges
    // uint8_t [0,255] <-> uint16_t [0,65535] <-> float [0.0,1.0]
}
```

The conversion function uses:
- `if constexpr` for compile-time type checking
- Template parameters for source and destination types
- Automatic range scaling (e.g., 0-255 to 0.0-1.0)

## Template Techniques Demonstrated

### 1. Type Traits
```cpp
std::is_floating_point_v<PixelType>
std::is_integral_v<PixelType>
std::is_same_v<PixelType, uint8_t>
```

### 2. Compile-Time Conditionals
```cpp
if constexpr (std::is_floating_point_v<DestType>) {
    dest_min = 0.0;
    dest_max = 1.0;
} else if constexpr (std::is_same_v<DestType, uint8_t>) {
    dest_min = 0.0;
    dest_max = 255.0;
}
```

### 3. constexpr Functions
```cpp
static constexpr size_t bits_per_pixel() {
    return sizeof(PixelType) * 8;  // Computed at compile time
}
```

### 4. Template Function Overloading
```cpp
template<typename DestType, typename SrcType>
Image<DestType> convert_image(const Image<SrcType>& src);
```

## Memory Usage Comparison

For a 640×480 image (307,200 pixels):

| Pixel Type | Size per Pixel | Total Memory |
|------------|---------------|--------------|
| uint8_t    | 1 byte        | 300 KB       |
| uint16_t   | 2 bytes       | 600 KB       |
| float      | 4 bytes       | 1.2 MB       |
| double     | 8 bytes       | 2.4 MB       |

## Real-World Applications

### uint8_t (8-bit)
- Consumer webcams
- Surveillance cameras
- Industrial inspection
- Video streaming
- Memory-constrained embedded systems

### uint16_t (16-bit)
- Medical X-ray imaging
- Scientific CCD cameras
- Astronomy (star field imaging)
- High dynamic range photography
- Industrial NDT (Non-Destructive Testing)

### float (32-bit)
- Time-of-Flight (ToF) depth sensors
- HDR image processing
- Computational photography
- LiDAR data
- Normalized image pipelines

### double (64-bit)
- Radio telescope data
- Spectroscopy
- High-precision scientific analysis
- Satellite imagery processing
- Research applications requiring maximum precision

## Advantages of Templates

### 1. **Type Safety**
- Compile-time type checking
- No runtime casting needed
- Prevents mixing incompatible pixel types
- Compiler catches type errors early

### 2. **Zero Overhead**
- No virtual function calls for pixel access
- All types resolved at compile time
- Fully optimizable by compiler
- Same performance as hand-written code

### 3. **Code Reusability**
- Write algorithms once, work with all types
- Easy to add new pixel types
- Generic code is more maintainable
- Reduces code duplication

### 4. **Compile-Time Information**
- `sizeof(PixelType)` known at compile time
- Type traits available
- `if constexpr` for type-specific optimizations
- Memory layout predictable

### 5. **Flexibility**
- Same interface for all pixel types
- Easy conversion between types
- Algorithms adapt to data type
- Support for custom pixel types

## Compilation

### Linux/macOS
```bash
g++ -std=c++20 -Wall -Wextra -O2 -o TemplatedCameraInterface src/TemplatedCameraInterface.cpp
./TemplatedCameraInterface
```

### Windows (MSVC)
```cmd
cl /std:c++20 /EHsc /O2 src\TemplatedCameraInterface.cpp
TemplatedCameraInterface.exe
```

### CMake
```bash
mkdir build && cd build
cmake ..
make TemplatedCameraInterface
./bin/TemplatedCameraInterface
```

## Output Example

```
================================================================
  TEMPLATED CAMERA INTERFACE FOR MULTIPLE PIXEL TYPES
================================================================

=== 1. 8-BIT GRAYSCALE CAMERA (uint8_t) ===
Common in: Webcams, industrial cameras, surveillance

  Camera: Webcam HD
  Resolution: 640x480
  Pixel type: uint8_t
  Bits per pixel: 8
  Floating point: No
  Memory per frame: 307200 bytes (300 KB)

  Capturing image...
  Capture time: 1523 µs

  Processing image...
  Mean value: 127.5
  Min value: 0
  Max value: 254

=== 2. 16-BIT CAMERA (uint16_t) ===
Common in: Scientific imaging, medical X-ray, astronomy

  Camera: Scientific CCD
  Resolution: 1024x1024
  Pixel type: uint16_t
  Bits per pixel: 16
  Floating point: No
  Memory per frame: 2097152 bytes (2 MB)

  [... more output ...]
```

## Performance Considerations

### Embedded Systems
- Use `uint8_t` for memory-constrained systems
- Avoid `double` unless precision is critical
- Consider fixed-size buffers instead of `std::vector`
- Template specialization for optimized code paths

### Real-Time Processing
- Pre-allocate image buffers
- Use SIMD intrinsics with templates for vectorization
- Consider memory alignment for DMA transfers
- Cache locality matters - templates don't affect this

### Cross-Platform
- Templates are header-only friendly
- Consistent behavior across compilers
- No ABI issues (unlike virtual functions)
- Easy to optimize per-platform

## Extending the Example

### Adding New Pixel Types

```cpp
// Complex numbers for FFT
class CameraComplex : public Camera<std::complex<double>> { /* ... */ };

// Custom packed RGB
struct RGB888 { uint8_t r, g, b; };
class CameraRGB : public Camera<RGB888> { /* ... */ };

// Fixed-point for embedded
using fixed_point_t = int32_t;
class CameraFixedPoint : public Camera<fixed_point_t> { /* ... */ };
```

### Adding Processing Algorithms

```cpp
template<typename PixelType>
class ImageProcessor {
    // Add histogram calculation
    static std::array<size_t, 256> histogram(const Image<PixelType>& img);
    
    // Add convolution filter
    static Image<PixelType> convolve(const Image<PixelType>& img, 
                                     const std::vector<double>& kernel);
    
    // Add edge detection
    static Image<PixelType> sobel_edge_detection(const Image<PixelType>& img);
};
```

## Related Examples

- [CPP17.md](CPP17.md) - `std::variant` and `std::any` for runtime pixel type handling
- [CPP20.md](CPP20.md) - Concepts for constraining pixel types
- [EmbeddedSystemsProgramming.cpp](../src/EmbeddedSystemsProgramming.cpp) - Fixed-size buffers for embedded cameras
- [ConceptsExamples.cpp](../src/ConceptsExamples.cpp) - Using concepts with templates

## Best Practices

1. **Use Type Traits**: Check type properties at compile time
2. **constexpr for Compile-Time Data**: Calculate constants during compilation
3. **if constexpr**: Avoid runtime branching for type-specific code
4. **Template Specialization**: Optimize critical paths for specific types
5. **Forward Declarations**: Keep compilation times reasonable
6. **Concept Constraints** (C++20): Document template requirements explicitly

## Further Reading

- C++20 Concepts for template constraints
- SFINAE (Substitution Failure Is Not An Error) for advanced template metaprogramming
- Template specialization for optimization
- Expression templates for lazy evaluation in image processing
