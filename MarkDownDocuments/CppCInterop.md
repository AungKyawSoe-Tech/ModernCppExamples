# C and C++ Interoperability Examples

This document describes the two comprehensive examples demonstrating bidirectional C/C++ interoperability.

## Overview

The workspace contains two complementary examples that demonstrate both directions of C/C++ interop:

1. **CppWrappingCLibrary.cpp** - Wrapping C libraries in C++ (C → C++)
2. **CreatingCApiFromCpp.cpp** - Exposing C++ code through C API (C++ → C)

## Example 1: Wrapping C Libraries in C++ (CppWrappingCLibrary.cpp)

### Purpose
Shows how to wrap C libraries (like POSIX sockets) in modern C++ classes using RAII, noexcept, and [[nodiscard]].

### Key Techniques

#### 1. RAII Resource Management
```cpp
class Socket {
    socket_t fd_;
public:
    Socket(int domain, int type, int protocol);  // Acquire
    ~Socket() noexcept;                          // Release
};
```

#### 2. Tag Dispatch for Constructor Disambiguation
```cpp
struct from_fd_t {};
static constexpr from_fd_t from_fd{};

Socket(from_fd_t, socket_t fd);  // Unambiguous from Socket(int, int, int)
```

#### 3. noexcept Specifications
- **Safe uses**: Destructors, move operations, getters
- **Unsafe uses**: Operations that can fail but return error codes
```cpp
~Socket() noexcept;                    // Safe: must not throw
[[nodiscard]] SocketError bind(...);   // Safe: returns error code
```

#### 4. [[nodiscard]] Attribute
Forces error checking at call sites:
```cpp
[[nodiscard]] SocketError send(const void* data, size_t len);
// Usage:
auto err = socket.send(data, len);  // Must check error
```

#### 5. extern "C" for C Library Integration
```cpp
extern "C" {
    bool c_validate_port(int port) {
        return port > 0 && port <= 65535;
    }
}
```

### Use Cases
- Using C libraries in modern C++ codebases
- Legacy system integration
- Cross-platform abstractions (POSIX/Winsock)
- Adding type safety to C APIs

### Build and Run
```bash
# From WSL/Linux
cd /mnt/c/CoPilot_Cli/ModernCppExamples
cmake --build build --target CppWrappingCLibrary
./build/bin/CppWrappingCLibrary
```

## Example 2: Creating a C API from C++ (CreatingCApiFromCpp.cpp)

### Purpose
Shows how to expose C++ functionality through a C-compatible API for use from C code or other languages.

### Key Techniques

#### 1. Opaque Handles (Pimpl Idiom for C)
```cpp
// C API header (what C code sees)
typedef struct ImageHandle* ImageHandle_t;

// Implementation (C++ side)
extern "C" ImageError image_create(size_t width, size_t height, 
                                   ImageHandle_t* out_handle) {
    auto* img = new Image(width, height);
    *out_handle = reinterpret_cast<ImageHandle_t>(img);
    return IMAGE_SUCCESS;
}
```

#### 2. Exception Safety Across C Boundary
```cpp
template<typename Func>
ImageError safe_call(Func&& func) noexcept {
    try {
        func();
        return IMAGE_SUCCESS;
    } catch (const std::out_of_range& e) {
        return IMAGE_ERROR_OUT_OF_RANGE;
    } catch (...) {
        return IMAGE_ERROR_UNKNOWN;
    }
}
```

#### 3. Error Code-Based Error Handling
```cpp
typedef enum {
    IMAGE_SUCCESS = 0,
    IMAGE_ERROR_INVALID_HANDLE = -1,
    IMAGE_ERROR_OUT_OF_RANGE = -3,
    IMAGE_ERROR_OUT_OF_MEMORY = -4
} ImageError;

const char* image_error_string(ImageError error);
```

#### 4. Output Parameters Instead of Return Values
```cpp
// Returns error code, data via output parameter
ImageError image_get_pixel(ImageHandle_t handle, 
                          size_t x, size_t y, 
                          uint8_t* out_value);
```

#### 5. Create/Destroy Pattern
```cpp
ImageHandle_t image = NULL;
image_create(10, 10, &image);
// ... use image ...
image_destroy(image);
```

### Use Cases
- Creating libraries for C codebases
- Stable ABI across compiler versions
- FFI for other languages (Python, Rust, Go, etc.)
- Plugin systems with C interfaces
- Game engine APIs

### Build and Run
```bash
# From WSL/Linux
cd /mnt/c/CoPilot_Cli/ModernCppExamples
cmake --build build --target CreatingCApiFromCpp
./build/bin/CreatingCApiFromCpp
```

## Comparison: When to Use Each Approach

### Wrapping C in C++ (Example 1)
**Use when:**
- You have existing C libraries to integrate
- You want modern C++ safety features (RAII, smart pointers)
- You want automatic resource management
- Internal project use within C++ codebase
- You need type safety and compile-time checks

**Benefits:**
- Modern C++ idioms
- Automatic cleanup via destructors
- Type-safe wrappers
- Exception-based error handling (optional)
- Better testability

### Exposing C++ as C (Example 2)
**Use when:**
- You want to create a C-compatible library
- You need stable ABI across versions/compilers
- You want FFI compatibility with other languages
- You need to support legacy C code
- You're building plugin systems
- You need name mangling avoidance

**Benefits:**
- C compatibility
- Stable binary interface
- Usable from ANY language with C FFI
- Complete implementation hiding
- Version-independent binary compatibility
- No C++ runtime dependencies for users

## Best Practices Summary

### Common to Both Approaches

1. **Clear Ownership Semantics**
   - Document who owns resources
   - Match create/destroy pairs
   - Avoid dangling handles/pointers

2. **Thread Safety**
   - Document thread-safety guarantees
   - Use appropriate synchronization
   - Consider const-correctness

3. **Error Handling**
   - Always check return values
   - Provide meaningful error messages
   - Never let exceptions cross C boundary

4. **Documentation**
   - Clear API documentation
   - Usage examples
   - Platform-specific notes

### Wrapping C in C++ Specific

- Use RAII for automatic cleanup
- Mark noexcept appropriately
- Use [[nodiscard]] for error-returning functions
- Consider smart pointers for ownership
- Prefer strong typing over raw pointers

### Creating C API from C++ Specific

- Use opaque handles to hide implementation
- All extern "C" functions must be noexcept
- Return error codes, use output parameters
- Avoid C++ features: templates, overloading, references, bool
- Use #ifdef __cplusplus for C/C++ compatibility
- Test with actual C compiler

## Real-World Examples

### Libraries Using C API from C++

1. **SQLite** - Database engine with C API, C++ implementation
2. **zlib** - Compression library with C interface
3. **libpng** - Image library with C API
4. **SDL2** - Game library with C interface
5. **OpenSSL** - Cryptography library with C API

### Libraries Wrapping C in C++

1. **Boost.Asio** - Wraps BSD sockets
2. **JUCE** - Wraps various OS APIs
3. **Qt** - Wraps platform-specific C APIs
4. **SFML** - Wraps OpenGL and system libraries

## Additional Resources

- [C++ Core Guidelines: Interfacing with C](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-cpl)
- [extern "C" Explained](https://en.cppreference.com/w/cpp/language/language_linkage)
- [RAII in Modern C++](https://en.cppreference.com/w/cpp/language/raii)
- [FFI Best Practices](https://www.lurklurk.org/effective-rust/ffi.html)

## Testing

Both examples include comprehensive demonstrations:

- **CppWrappingCLibrary**: Shows RAII lifecycle, noexcept usage, [[nodiscard]] enforcement
- **CreatingCApiFromCpp**: Shows exception handling, error codes, opaque handles

Run both examples to see best practices in action!

## Platform Support

- **Linux/WSL**: Full support with POSIX sockets
- **Windows**: Full support with Winsock2 (requires linking ws2_32)
- **macOS**: Full support with POSIX sockets

Both examples are cross-platform with appropriate conditional compilation.
