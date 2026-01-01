# C++20 Modules: Import and Export

## Overview

C++20 introduces **modules**, a modern alternative to the traditional `#include` header system. Modules provide better encapsulation, faster compilation, and eliminate many preprocessor-related issues.

## Traditional Headers vs Modules

### Traditional Headers (Pre-C++20)
```cpp
// camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <memory>
// ... more includes

template<typename T>
class Camera {
    // ...
};

#endif // CAMERA_H
```

```cpp
// main.cpp
#include "camera.h"  // Entire header parsed
#include "camera.h"  // Potentially included again!
```

**Problems:**
- Header parsed every time it's included
- Include guards needed to prevent multiple inclusion
- Macro leakage between headers
- Compile-time dependencies on include order
- Long compilation times for large projects

### C++20 Modules

**Module Interface File (CameraModule.cppm):**
```cpp
export module camera;

#include <vector>
#include <memory>

export template<typename T>
class Camera {
    // ...
};
```

**Main Program:**
```cpp
import camera;  // Import module (compiled once)

int main() {
    Camera<uint8_t> cam;
}
```

**Benefits:**
- Module compiled once, reused many times
- No include guards needed
- No macro leakage
- Explicit export control
- Faster compilation
- Better encapsulation

## Module Syntax

### 1. Module Declaration

```cpp
export module module_name;
```

Declares a module interface unit. Everything after this belongs to the module.

### 2. Global Module Fragment

```cpp
module;  // Global module fragment

#include <iostream>  // Legacy headers
#include <vector>

export module camera;  // Module declaration
```

The global module fragment allows `#include` directives before the module declaration.

### 3. Exporting Entities

```cpp
export module camera;

// Export a class
export class Image { /* ... */ };

// Export a template class
export template<typename T>
class Camera { /* ... */ };

// Export a function
export void process_image();

// Export a template function
export template<typename T>
void convert(const Image<T>& img);

// Export a concept
export template<typename T>
concept PixelType = std::is_arithmetic_v<T>;

// Export multiple entities
export {
    class Image;
    class Camera;
    void process();
}
```

### 4. Importing Modules

```cpp
import camera;  // Import entire module

int main() {
    Image img;  // Use exported class
}
```

### 5. Private (Non-exported) Entities

```cpp
export module camera;

// Private helper (not exported)
namespace detail {
    void internal_helper() { /* ... */ }
}

// Exported public API
export class Camera {
    void capture() {
        detail::internal_helper();  // Can use private entities
    }
};
```

## Example: Camera Module

### Module Interface (CameraModule.cppm)

```cpp
module;

#include <vector>
#include <memory>
#include <concepts>

export module camera;

// Export concept
export template<typename T>
concept PixelType = std::is_arithmetic_v<T>;

// Export template class
export template<PixelType T>
class Image {
private:
    size_t width, height;
    std::vector<T> pixels;
public:
    Image(size_t w, size_t h) : width(w), height(h), pixels(w*h) {}
    
    T& at(size_t x, size_t y) { return pixels[y * width + x]; }
    const T& at(size_t x, size_t y) const { return pixels[y * width + x]; }
};

// Export camera interface
export template<PixelType T>
class Camera {
public:
    virtual ~Camera() = default;
    virtual Image<T> capture() = 0;
};

// Export concrete implementation
export class Camera8bit : public Camera<uint8_t> {
public:
    Image<uint8_t> capture() override;
};
```

### Main Program (TemplatedCameraModules.cpp)

```cpp
#include <iostream>

import camera;  // Import the module

int main() {
    // Use exported types
    Camera8bit cam;
    Image<uint8_t> img = cam.capture();
    
    std::cout << "Captured " << img.get_width() << "x" << img.get_height() << " image\n";
}
```

## C++20 Concepts with Modules

Modules work seamlessly with C++20 concepts:

```cpp
export module camera;

// Export concept
export template<typename T>
concept PixelType = std::is_arithmetic_v<T> && (
    std::is_same_v<T, uint8_t> ||
    std::is_same_v<T, uint16_t> ||
    std::is_same_v<T, float> ||
    std::is_same_v<T, double>
);

// Constrain template with concept
export template<PixelType T>
class Image { /* ... */ };

// Function with requires clause
export template<PixelType T>
void process(const Image<T>& img)
    requires std::is_integral_v<T>
{
    // Only works with integer pixel types
}
```

## Compilation

### GCC 11+ (Experimental)

```bash
# Compile module
g++ -std=c++20 -fmodules-ts -c CameraModule.cppm

# Compile main program
g++ -std=c++20 -fmodules-ts TemplatedCameraModules.cpp CameraModule.o -o program

# Run
./program
```

### MSVC 2022+ (Best Support)

```cmd
# Compile module
cl /std:c++20 /experimental:module /c CameraModule.cppm

# Compile main program
cl /std:c++20 /experimental:module TemplatedCameraModules.cpp CameraModule.obj

# Run
TemplatedCameraModules.exe
```

### Clang 16+

```bash
# Compile module
clang++ -std=c++20 -fmodules -c CameraModule.cppm

# Compile main program
clang++ -std=c++20 -fmodules TemplatedCameraModules.cpp CameraModule.o -o program

# Run
./program
```

## Module File Extensions

Different conventions exist:

- `.cppm` - C++ Module (GCC, Clang)
- `.ixx` - Interface (MSVC)
- `.mpp` - Module++ (some IDEs)
- `.mxx` - Module Extension (alternative)

All are valid; choose based on your toolchain.

## Module Partitions

Large modules can be split into partitions:

```cpp
// camera:image partition
export module camera:image;

export template<typename T>
class Image { /* ... */ };
```

```cpp
// camera:processor partition
export module camera:processor;

export template<typename T>
class ImageProcessor { /* ... */ };
```

```cpp
// Main camera module
export module camera;

export import :image;      // Re-export image partition
export import :processor;  // Re-export processor partition
```

Usage:
```cpp
import camera;  // Gets everything
// OR
import camera:image;  // Just the image partition
```

## Advantages Over Headers

### 1. Faster Compilation

**Headers:** Parsed every time included
```cpp
// main.cpp
#include "big_header.h"  // Parse 10,000 lines

// other.cpp
#include "big_header.h"  // Parse 10,000 lines again!
```

**Modules:** Compiled once, reused
```cpp
// main.cpp
import big_module;  // Use precompiled module

// other.cpp
import big_module;  // Reuse same compilation
```

### 2. Better Encapsulation

**Headers:** Everything is public
```cpp
// header.h
void internal_helper();  // Visible to all includers!
void public_api();       // Actually public
```

**Modules:** Explicit export control
```cpp
export module api;

void internal_helper();   // Private to module
export void public_api(); // Explicitly exported
```

### 3. No Macro Leakage

**Headers:**
```cpp
// lib.h
#define MAX 100

// main.cpp
#include "lib.h"
#define MAX 200  // Conflict!
```

**Modules:**
```cpp
// lib module
export module lib;
const int MAX = 100;  // No macro pollution

// main.cpp
import lib;
const int MAX = 200;  // No conflict
```

### 4. Order Independent

**Headers:** Order matters
```cpp
#include "b.h"  // Depends on a.h
#include "a.h"  // ERROR: b.h needs a.h first!
```

**Modules:** Order doesn't matter
```cpp
import b;
import a;  // Works fine!
```

### 5. One Definition Rule (ODR)

**Headers:** ODR violations possible
```cpp
// header.h
inline int value = 42;  // Need inline!

// If not inline, multiple includes = ODR violation
```

**Modules:** ODR handled automatically
```cpp
export module lib;
int value = 42;  // Just works, no inline needed
```

## Best Practices

### 1. Use `.cppm` Extension

Standard emerging convention for module interface files.

### 2. Global Module Fragment for Legacy Headers

```cpp
module;  // Start global module fragment

#include <vector>     // Legacy standard headers
#include <iostream>

export module mymodule;  // Begin module
```

### 3. Export Only Public API

```cpp
export module api;

// Private
namespace detail {
    void helper() { }
}

// Public
export class PublicAPI {
    void method() { detail::helper(); }
};
```

### 4. Use Concepts with Modules

```cpp
export module math;

export template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

export template<Numeric T>
T add(T a, T b) { return a + b; }
```

### 5. Module Partitions for Large Modules

Split large modules into logical partitions for maintainability.

## Limitations and Considerations

### Current Compiler Support (2026)

- **MSVC:** Excellent support ✅
- **GCC:** Experimental (use `-fmodules-ts`) ⚠️
- **Clang:** Good support in 16+ ✅

### Build System Support

- **CMake 3.28+:** Good module support
- **Older build systems:** May need manual configuration

### Migration

- Mix headers and modules during transition
- Gradually convert headers to modules
- Use `import std;` for standard library (C++23)

## Future: C++23 Standard Library Modules

C++23 introduces:
```cpp
import std;  // Entire standard library as module!

int main() {
    std::vector<int> v{1, 2, 3};  // Works without #include <vector>
}
```

## Summary

| Feature | Headers | Modules |
|---------|---------|---------|
| Compilation | Parsed every time | Compiled once |
| Include guards | Required | Not needed |
| Macro isolation | No | Yes |
| Encapsulation | Weak | Strong |
| Order dependency | Yes | No |
| ODR violations | Possible | Prevented |
| Compilation speed | Slow | Fast |

**Modules are the future of C++!** They solve decades-old problems with the preprocessor and header system.
