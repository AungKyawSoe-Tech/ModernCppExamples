# PYBIND11 AND C++/PYTHON INTEROPERABILITY

## Overview

Pybind11 is a lightweight header-only library that exposes C++ types in Python and vice versa, mainly to create Python bindings of existing C++ code. It enables seamless interoperability between C++ and Python, allowing you to:

1. **Call C++ functions from Python** - Expose C++ libraries to Python
2. **Call Python functions from C++** - Use Python callbacks in C++ code
3. **Share data efficiently** - Automatic conversion of STL containers, NumPy arrays
4. **Leverage performance** - Write performance-critical code in C++, use from Python

## Why Use Pybind11?

### Advantages
- ✅ **Modern C++ support** (C++11/14/17/20/23)
- ✅ **Header-only** - No external dependencies
- ✅ **Automatic type conversion** - STL containers, smart pointers
- ✅ **NumPy integration** - Zero-copy access to NumPy arrays
- ✅ **Exception mapping** - C++ exceptions become Python exceptions
- ✅ **Easy to learn** - Intuitive API similar to Boost.Python
- ✅ **Small binary size** - Much smaller than Boost.Python
- ✅ **Cross-platform** - Linux, Windows, macOS

### When to Use
- You have existing C++ code to expose to Python
- Performance-critical sections (100-1000x faster than pure Python)
- Numerical computing with NumPy arrays
- Embedded systems integration (sensor data, hardware interfaces)
- Machine learning inference (C++ models called from Python)

### When NOT to Use
- Pure Python is fast enough for your use case
- Simple C functions (use `ctypes` instead)
- Multi-language support needed (consider SWIG)
- Deployment complexity is a concern

## Installation

```bash
# Install pybind11
pip install pybind11

# Or with conda
conda install -c conda-forge pybind11
```

## Building the Example

### Linux/macOS

```bash
# Make the build script executable
chmod +x scripts/build_pybind.sh

# Build and test
./scripts/build_pybind.sh
```

### Manual Build

```bash
c++ -O3 -Wall -shared -std=c++17 -fPIC \
    $(python3 -m pybind11 --includes) \
    src/Pybind11Example.cpp \
    -o pybind_example$(python3-config --extension-suffix)
```

### Windows (MSVC)

```cmd
cl /O2 /W4 /EHsc /LD /std:c++17 ^
   /I"%PYTHON_INCLUDE%" ^
   /I"%PYBIND11_INCLUDE%" ^
   src\Pybind11Example.cpp ^
   /link /OUT:pybind_example.pyd
```

## Usage Examples

### 1. Simple Functions (C++ → Python)

```python
import pybind_example as pe

# Call C++ function
result = pe.add(5, 3)  # Returns 8

# Default arguments work
value = pe.multiply(4.5)  # Uses default: 4.5 * 2.0 = 9.0

# Multiple return values (tuple)
quotient, remainder = pe.divide_with_remainder(17, 5)
```

### 2. STL Containers (Automatic Conversion)

```python
# Python list → C++ std::vector → Python list
numbers = [1, 2, 3, 4, 5]
squared = pe.square_elements(numbers)  # [1, 4, 9, 16, 25]

# Python list → C++ std::vector<string> → Python dict
words = ["hello", "world", "hello"]
counts = pe.count_words(words)  # {'hello': 2, 'world': 1}
```

### 3. Classes (C++ Objects in Python)

```python
# Create C++ object
v1 = pe.Vector2D(3.0, 4.0)
v2 = pe.Vector2D(1.0, 2.0)

# Call methods
length = v1.length()  # Returns 5.0

# Operator overloading
v3 = v1 + v2  # Vector addition
v4 = v1 * 2.0  # Scalar multiplication

# Property access
v1.x = 10.0  # Calls setX(10.0)
x = v1.x     # Calls getX()

# Static methods
v_zero = pe.Vector2D.zero()
```

### 4. Inheritance (Polymorphism)

```python
# Base and derived classes
circle = pe.Circle("MyCircle", 5.0)
rect = pe.Rectangle("MyRect", 4.0, 6.0)

# Polymorphic calls
shapes = [circle, rect]
for shape in shapes:
    print(f"{shape.get_name()}: area = {shape.area()}")
```

### 5. Callbacks (Python → C++)

**This is the most powerful feature!** C++ code can call Python functions.

```python
# Define Python function
def transform(x):
    return x * 2 + 1

# Pass to C++, which will call it for each element
data = [1, 2, 3, 4, 5]
result = pe.process_with_callback(data, transform)
# C++ calls transform() for each element
# Result: [3, 5, 7, 9, 11]

# Works with lambdas too
result = pe.process_with_callback(data, lambda x: x**2)

# Numerical integration
import math
area = pe.integrate(math.sin, 0, math.pi)  # ≈ 2.0
```

### 6. Sensor Data Processing (Embedded Example)

```python
processor = pe.SensorProcessor()

# Add readings from C++ embedded device
processor.add_reading(0.0, 25.5, "TEMP_01")
processor.add_reading(1.0, 26.2, "TEMP_01")
processor.add_reading(2.0, 24.8, "TEMP_02")

# Process with Python function (called from C++)
def extract_value(reading):
    return reading.value

values = processor.process_readings(extract_value)
# C++ iterates readings and calls extract_value()

# Filter with Python predicate
high_temps = processor.filter_readings(
    lambda r: r.value > 25.0
)
```

### 7. NumPy Arrays (High Performance)

```python
import numpy as np

# Process NumPy array in C++ (zero-copy!)
arr = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
squared = pe.numpy_square(arr)  # C++ speed!

# Matrix multiplication
A = np.array([[1.0, 2.0], [3.0, 4.0]])
B = np.array([[5.0, 6.0], [7.0, 8.0]])
C = pe.matrix_multiply(A, B)  # Fast C++ implementation
```

## Performance Comparison

| Implementation | Relative Speed | Use Case |
|---------------|---------------|----------|
| Pure Python | 1x (baseline) | Prototyping, I/O-bound |
| NumPy | 10-100x | Vectorized operations |
| C++ via pybind11 | 100-1000x | Custom algorithms |
| Pure C++ (no Python) | 1000x+ | Maximum performance |

### When Performance Matters

```python
# Example: Process 1 million sensor readings

# Pure Python: ~5 seconds
for reading in readings:
    result = process_pure_python(reading)

# C++ via pybind11: ~0.01 seconds (500x faster!)
results = pe.process_batch(readings)
```

## Alternative Frameworks

### 1. Boost.Python (Older, Heavyweight)

```cpp
#include <boost/python.hpp>

BOOST_PYTHON_MODULE(example) {
    boost::python::def("add", add);
}
```

**Pros:** Mature, feature-rich  
**Cons:** Large dependency (all of Boost), slower compile times

### 2. SWIG (Multi-Language)

```swig
// example.i
%module example
%{
#include "example.h"
%}
%include "example.h"
```

**Build:** `swig -python -c++ example.i`

**Pros:** Supports Java, C#, Ruby, etc.  
**Cons:** Generated code is verbose, harder to debug

### 3. ctypes (Python Standard Library)

```python
import ctypes

lib = ctypes.CDLL('./example.so')
lib.add.argtypes = [ctypes.c_int, ctypes.c_int]
lib.add.restype = ctypes.c_int
result = lib.add(5, 3)
```

**Pros:** No compilation, works with pure C  
**Cons:** Manual type annotations, no C++ classes, error-prone

### 4. CFFI (C Foreign Function Interface)

```python
from cffi import FFI

ffi = FFI()
ffi.cdef("int add(int, int);")
lib = ffi.dlopen('./example.so')
result = lib.add(5, 3)
```

**Pros:** Faster than ctypes, better error messages  
**Cons:** Limited C++ support, no automatic conversions

### 5. nanobind (Modern, Lightweight)

```cpp
#include <nanobind/nanobind.h>

NB_MODULE(example, m) {
    m.def("add", &add);
}
```

**Pros:** Smaller binaries, faster compile times than pybind11  
**Cons:** Newer, less mature, fewer features

## Framework Comparison

| Feature | pybind11 | Boost.Python | SWIG | ctypes | nanobind |
|---------|----------|--------------|------|--------|----------|
| C++ Support | ✅ Full | ✅ Full | ⚠️ Limited | ❌ No | ✅ Full |
| Modern C++ | ✅ C++11-23 | ✅ C++11-17 | ⚠️ Basic | N/A | ✅ C++17+ |
| Header-only | ✅ Yes | ❌ No | ❌ No | ✅ Yes | ✅ Yes |
| NumPy | ✅ Excellent | ⚠️ Manual | ⚠️ Manual | ⚠️ Manual | ✅ Excellent |
| Auto Convert | ✅ Yes | ✅ Yes | ⚠️ Limited | ❌ No | ✅ Yes |
| Binary Size | Good | Large | Medium | N/A | Small |
| Compile Time | Medium | Slow | Fast | N/A | Fast |
| Multi-Lang | ❌ Python only | ❌ Python only | ✅ Many | ❌ Python only | ❌ Python only |
| Maturity | Mature | Very Mature | Very Mature | Mature | New |

## Best Practices

### 1. Exception Handling

```cpp
// C++ exceptions become Python exceptions
void risky_function() {
    throw std::runtime_error("Something went wrong!");
}

// In Python:
try:
    pe.risky_function()
except RuntimeError as e:
    print(f"Caught: {e}")
```

### 2. Memory Management

```cpp
// Use smart pointers - automatic cleanup
py::class_<Resource, std::shared_ptr<Resource>>(m, "Resource");

// Pybind11 handles reference counting correctly
```

### 3. GIL (Global Interpreter Lock)

```cpp
// Release GIL for long-running C++ code
m.def("long_computation", []() {
    py::gil_scoped_release release;  // Release GIL
    // C++ code runs in parallel
    compute_for_a_long_time();
    py::gil_scoped_acquire acquire;  // Reacquire GIL
    return result;
});
```

### 4. Documentation

```cpp
// Add docstrings
m.def("add", &add, 
      "Add two integers\n\n"
      "Args:\n"
      "    a (int): First integer\n"
      "    b (int): Second integer\n\n"
      "Returns:\n"
      "    int: Sum of a and b",
      py::arg("a"), py::arg("b"));
```

### 5. Default Arguments

```cpp
// Named and default arguments
m.def("integrate", &integrate,
      py::arg("func"),
      py::arg("start"),
      py::arg("end"),
      py::arg("steps") = 1000);

// Python: pe.integrate(f, 0, 1)  # Uses steps=1000
```

## Debugging Tips

### 1. Check Module Import

```python
import pybind_example as pe
print(dir(pe))  # List all exported functions/classes
help(pe.add)    # Show docstring
```

### 2. Type Errors

```python
# Wrong type
pe.add("5", "3")  # TypeError: incompatible function arguments

# Check signature
print(pe.add.__doc__)
```

### 3. Compilation Errors

```bash
# Verbose compilation
c++ -v -O3 -Wall -shared -std=c++17 -fPIC \
    $(python3 -m pybind11 --includes) \
    src/Pybind11Example.cpp \
    -o pybind_example$(python3-config --extension-suffix)
```

## Real-World Applications

### 1. Machine Learning
- TensorFlow, PyTorch use similar binding techniques
- C++ inference engines called from Python
- NumPy arrays for zero-copy data transfer

### 2. Embedded Systems
- Hardware interface libraries (I2C, SPI, UART)
- Sensor data processing
- Real-time control systems
- Firmware testing frameworks

### 3. Scientific Computing
- Fast numerical algorithms
- Physics simulations
- Signal processing
- Computer vision

### 4. Game Development
- Game engine core in C++
- Python scripting for game logic
- Level editors and tools

## CMake Integration

```cmake
cmake_minimum_required(VERSION 3.15)
project(pybind_example)

# Find Python and pybind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# Create Python module
pybind11_add_module(pybind_example src/Pybind11Example.cpp)

# Set C++ standard
target_compile_features(pybind_example PRIVATE cxx_std_17)
```

## Further Reading

- **Pybind11 Documentation:** https://pybind11.readthedocs.io/
- **GitHub Repository:** https://github.com/pybind/pybind11
- **Tutorial:** https://pybind11.readthedocs.io/en/stable/basics.html
- **Advanced Topics:** https://pybind11.readthedocs.io/en/stable/advanced/

## Summary

| Direction | Description | Example |
|-----------|-------------|---------|
| **C++ → Python** | Expose C++ functions/classes to Python | `pe.add(5, 3)` |
| **Python → C++** | Pass Python callbacks to C++ | `pe.process_with_callback(data, lambda x: x**2)` |
| **Data Sharing** | Automatic STL/NumPy conversion | `pe.square_elements([1,2,3])` |
| **Performance** | 100-1000x speedup for algorithms | NumPy array processing |

**Pybind11 is the modern standard for C++/Python interoperability!**
