# C++20 Modules Quick Reference

## Files Created

1. **[CameraModule.cppm](../src/CameraModule.cppm)** - Module interface file
   - Exports camera templates and classes
   - Uses C++20 concepts for type constraints
   
2. **[TemplatedCameraModules.cpp](../src/TemplatedCameraModules.cpp)** - Main program
   - Imports the camera module
   - Demonstrates module usage

3. **[build_modules.sh](../scripts/build_modules.sh)** - Linux/macOS build script
4. **[build_modules.ps1](../build_modules.ps1)** - Windows PowerShell build script

## Quick Start

### Windows (MSVC - Best Support)

```powershell
# Run the PowerShell build script
.\build_modules.ps1
```

Or manually:
```cmd
cd build\modules
cl /std:c++20 /experimental:module /EHsc /c ..\..\src\CameraModule.cppm
cl /std:c++20 /experimental:module /EHsc ..\..\src\TemplatedCameraModules.cpp CameraModule.obj
TemplatedCameraModules.exe
```

### Linux/macOS (GCC)

```bash
# Run the bash build script
chmod +x scripts/build_modules.sh
./scripts/build_modules.sh
```

Or manually:
```bash
mkdir -p build/modules && cd build/modules
g++ -std=c++20 -fmodules-ts -c ../../src/CameraModule.cppm
g++ -std=c++20 -fmodules-ts ../../src/TemplatedCameraModules.cpp CameraModule.o -o TemplatedCameraModules
./TemplatedCameraModules
```

### Clang 16+

```bash
mkdir -p build/modules && cd build/modules
clang++ -std=c++20 -fmodules -c ../../src/CameraModule.cppm
clang++ -std=c++20 -fmodules ../../src/TemplatedCameraModules.cpp CameraModule.o -o TemplatedCameraModules
./TemplatedCameraModules
```

## Module Syntax Quick Reference

### Declaring a Module

```cpp
export module module_name;
```

### Global Module Fragment (for #includes)

```cpp
module;  // Global module fragment

#include <vector>
#include <iostream>

export module module_name;  // Module declaration
```

### Exporting Entities

```cpp
export module camera;

// Export a class
export class Image { };

// Export a template
export template<typename T>
class Camera { };

// Export a function
export void process();

// Export a concept
export template<typename T>
concept PixelType = std::is_arithmetic_v<T>;
```

### Importing a Module

```cpp
#include <iostream>  // Can still use headers

import camera;  // Import module

int main() {
    Image img;  // Use exported types
}
```

## C++20 Features Demonstrated

### 1. Modules (export/import)
```cpp
// Module interface
export module camera;
export class Image { };

// Main program
import camera;
Image img;
```

### 2. Concepts
```cpp
export template<typename T>
concept PixelType = std::is_arithmetic_v<T>;

export template<PixelType T>
class Image { };  // Only accepts PixelType
```

### 3. Requires Clauses
```cpp
export template<typename T>
void threshold(Image<T>& img)
    requires std::is_integral_v<T>  // Only integer types
{
    // Implementation
}
```

### 4. [[nodiscard]]
```cpp
export class Image {
    [[nodiscard]] size_t get_width() const;  // Warn if unused
};
```

### 5. Three-Way Comparison
```cpp
export class Image {
    auto operator<=>(const Image&) const = default;  // Auto-generate all comparisons
};
```

## Compiler Support (January 2026)

| Compiler | Support | Flag |
|----------|---------|------|
| MSVC 2022+ | ✅ Excellent | `/std:c++20 /experimental:module` |
| GCC 11+ | ⚠️ Experimental | `-std=c++20 -fmodules-ts` |
| Clang 16+ | ✅ Good | `-std=c++20 -fmodules` |

## Common Issues

### Issue: "module not found"
**Solution:** Compile the module interface file (`.cppm`) first before the main program.

### Issue: "unknown directive" errors
**Solution:** Use correct compiler flag:
- MSVC: `/experimental:module`
- GCC: `-fmodules-ts`
- Clang: `-fmodules`

### Issue: Standard library headers not found
**Solution:** Use global module fragment:
```cpp
module;  // Global module fragment
#include <vector>  // Include here
export module mymodule;
```

## Benefits Over Traditional Headers

1. **Faster Compilation**: Module compiled once, reused many times
2. **Better Encapsulation**: Only exported entities are visible
3. **No Include Guards**: Modules automatically included once
4. **No Macro Leakage**: Macros don't leak between modules
5. **Order Independent**: Import order doesn't matter

## References

- [Cpp20Modules.md](Cpp20Modules.md) - Complete guide to C++20 modules
- [TemplatedCameraInterface.md](TemplatedCameraInterface.md) - Original template camera interface
- [CPP20.md](CPP20.md) - General C++20 features

## Next Steps

1. Try compiling with your compiler
2. Modify the module to add new camera types
3. Create your own modules for other functionality
4. Explore module partitions for larger projects
