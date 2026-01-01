// ===================================================================
// PYBIND11: C++ AND PYTHON INTEROPERABILITY
// ===================================================================
// This example demonstrates bidirectional calling between C++ and Python
// using pybind11 (modern, header-only binding library)
//
// BUILD INSTRUCTIONS:
// ===================
// 1. Install pybind11:
//    pip install pybind11
//
// 2. Compile as Python extension module:
//    c++ -O3 -Wall -shared -std=c++17 -fPIC \
//        $(python3 -m pybind11 --includes) \
//        Pybind11Example.cpp \
//        -o pybind_example$(python3-config --extension-suffix)
//
// 3. Use from Python:
//    import pybind_example
//    result = pybind_example.add(2, 3)
//
// ALTERNATIVE FRAMEWORKS:
// =======================
// - Boost.Python: Older, requires Boost library
// - SWIG: Generates bindings for multiple languages
// - ctypes: Python standard library, simpler but less type-safe
// - cffi: C Foreign Function Interface for Python
// - nanobind: Modern, lightweight alternative to pybind11
// ===================================================================

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>        // STL containers conversion
#include <pybind11/functional.h> // std::function conversion
#include <pybind11/numpy.h>      // NumPy arrays support
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cmath>

namespace py = pybind11;

// ===================================================================
// 1. SIMPLE FUNCTIONS - C++ FUNCTIONS CALLED FROM PYTHON
// ===================================================================

// Simple function
int add(int a, int b) {
    return a + b;
}

// Function with default arguments
double multiply(double a, double b = 2.0) {
    return a * b;
}

// Function with multiple return values (using tuple)
std::tuple<int, int> divide_with_remainder(int dividend, int divisor) {
    int quotient = dividend / divisor;
    int remainder = dividend % divisor;
    return std::make_tuple(quotient, remainder);
}

// Function returning string
std::string greet(const std::string& name) {
    return "Hello, " + name + "!";
}

// ===================================================================
// 2. WORKING WITH STL CONTAINERS
// ===================================================================

// Function accepting and returning std::vector
std::vector<int> square_elements(const std::vector<int>& vec) {
    std::vector<int> result;
    result.reserve(vec.size());
    for (int val : vec) {
        result.push_back(val * val);
    }
    return result;
}

// Function with std::map
std::map<std::string, int> count_words(const std::vector<std::string>& words) {
    std::map<std::string, int> counts;
    for (const auto& word : words) {
        counts[word]++;
    }
    return counts;
}

// ===================================================================
// 3. CLASSES - EXPOSING C++ CLASSES TO PYTHON
// ===================================================================

class Vector2D {
private:
    double x, y;
    
public:
    // Constructors
    Vector2D() : x(0.0), y(0.0) {}
    Vector2D(double x, double y) : x(x), y(y) {}
    
    // Getters/Setters
    double getX() const { return x; }
    double getY() const { return y; }
    void setX(double val) { x = val; }
    void setY(double val) { y = val; }
    
    // Methods
    double length() const {
        return std::sqrt(x * x + y * y);
    }
    
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D operator*(double scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    
    // String representation
    std::string toString() const {
        return "Vector2D(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
    
    // Static method
    static Vector2D zero() {
        return Vector2D(0.0, 0.0);
    }
};

// ===================================================================
// 4. INHERITANCE - BASE AND DERIVED CLASSES
// ===================================================================

class Shape {
protected:
    std::string name;
    
public:
    Shape(const std::string& name) : name(name) {}
    virtual ~Shape() = default;
    
    virtual double area() const = 0;
    virtual std::string getName() const { return name; }
};

class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(const std::string& name, double r) 
        : Shape(name), radius(r) {}
    
    double area() const override {
        return 3.14159 * radius * radius;
    }
    
    double getRadius() const { return radius; }
};

class Rectangle : public Shape {
private:
    double width, height;
    
public:
    Rectangle(const std::string& name, double w, double h)
        : Shape(name), width(w), height(h) {}
    
    double area() const override {
        return width * height;
    }
};

// ===================================================================
// 5. CALLBACKS - CALLING PYTHON FUNCTIONS FROM C++
// ===================================================================

// Process data with Python callback
std::vector<int> process_with_callback(
    const std::vector<int>& data,
    std::function<int(int)> callback) {
    
    std::vector<int> result;
    result.reserve(data.size());
    
    for (int val : data) {
        result.push_back(callback(val));  // Call Python function!
    }
    
    return result;
}

// Numerical integration using Python callback
double integrate(std::function<double(double)> func, 
                double start, double end, int steps = 1000) {
    double dx = (end - start) / steps;
    double sum = 0.0;
    
    for (int i = 0; i < steps; ++i) {
        double x = start + i * dx;
        sum += func(x) * dx;  // Call Python function!
    }
    
    return sum;
}

// ===================================================================
// 6. SENSOR DATA PROCESSING (EMBEDDED SYSTEMS EXAMPLE)
// ===================================================================

struct SensorReading {
    double timestamp;
    double value;
    std::string sensor_id;
    
    SensorReading(double t, double v, const std::string& id)
        : timestamp(t), value(v), sensor_id(id) {}
};

class SensorProcessor {
private:
    std::vector<SensorReading> readings;
    
public:
    void addReading(double timestamp, double value, const std::string& id) {
        readings.emplace_back(timestamp, value, id);
    }
    
    size_t getReadingCount() const {
        return readings.size();
    }
    
    std::vector<SensorReading> getReadings() const {
        return readings;
    }
    
    // Process readings with Python callback
    std::vector<double> processReadings(
        std::function<double(const SensorReading&)> processor) {
        
        std::vector<double> results;
        results.reserve(readings.size());
        
        for (const auto& reading : readings) {
            results.push_back(processor(reading));  // Call Python!
        }
        
        return results;
    }
    
    // Filter readings with Python predicate
    std::vector<SensorReading> filterReadings(
        std::function<bool(const SensorReading&)> predicate) {
        
        std::vector<SensorReading> filtered;
        
        for (const auto& reading : readings) {
            if (predicate(reading)) {  // Call Python!
                filtered.push_back(reading);
            }
        }
        
        return filtered;
    }
};

// ===================================================================
// 7. NUMPY ARRAY PROCESSING (HIGH PERFORMANCE)
// ===================================================================

// Process NumPy array in C++ for performance
py::array_t<double> numpy_square(py::array_t<double> input) {
    // Get buffer info
    py::buffer_info buf = input.request();
    
    if (buf.ndim != 1) {
        throw std::runtime_error("Number of dimensions must be 1");
    }
    
    // Create output array
    auto result = py::array_t<double>(buf.size);
    py::buffer_info result_buf = result.request();
    
    // Get pointers
    double* input_ptr = static_cast<double*>(buf.ptr);
    double* result_ptr = static_cast<double*>(result_buf.ptr);
    
    // Process in C++ (fast!)
    for (size_t i = 0; i < buf.shape[0]; i++) {
        result_ptr[i] = input_ptr[i] * input_ptr[i];
    }
    
    return result;
}

// Matrix multiplication (2D NumPy arrays)
py::array_t<double> matrix_multiply(
    py::array_t<double> a,
    py::array_t<double> b) {
    
    py::buffer_info buf_a = a.request();
    py::buffer_info buf_b = b.request();
    
    if (buf_a.ndim != 2 || buf_b.ndim != 2) {
        throw std::runtime_error("Inputs must be 2D arrays");
    }
    
    size_t rows_a = buf_a.shape[0];
    size_t cols_a = buf_a.shape[1];
    size_t rows_b = buf_b.shape[0];
    size_t cols_b = buf_b.shape[1];
    
    if (cols_a != rows_b) {
        throw std::runtime_error("Matrix dimensions don't match");
    }
    
    // Create result matrix
    auto result = py::array_t<double>({rows_a, cols_b});
    py::buffer_info buf_result = result.request();
    
    double* ptr_a = static_cast<double*>(buf_a.ptr);
    double* ptr_b = static_cast<double*>(buf_b.ptr);
    double* ptr_result = static_cast<double*>(buf_result.ptr);
    
    // Initialize result to zero
    for (size_t i = 0; i < rows_a * cols_b; i++) {
        ptr_result[i] = 0.0;
    }
    
    // Matrix multiplication
    for (size_t i = 0; i < rows_a; i++) {
        for (size_t j = 0; j < cols_b; j++) {
            for (size_t k = 0; k < cols_a; k++) {
                ptr_result[i * cols_b + j] += 
                    ptr_a[i * cols_a + k] * ptr_b[k * cols_b + j];
            }
        }
    }
    
    return result;
}

// ===================================================================
// 8. SMART POINTERS - MANAGING OBJECT LIFETIME
// ===================================================================

class Resource {
private:
    std::string name;
    int id;
    
public:
    Resource(const std::string& n, int i) : name(n), id(i) {
        std::cout << "Resource created: " << name << " (ID: " << id << ")" << std::endl;
    }
    
    ~Resource() {
        std::cout << "Resource destroyed: " << name << " (ID: " << id << ")" << std::endl;
    }
    
    std::string getName() const { return name; }
    int getId() const { return id; }
};

// Factory function returning unique_ptr
std::unique_ptr<Resource> create_resource(const std::string& name, int id) {
    return std::make_unique<Resource>(name, id);
}

// Factory returning shared_ptr
std::shared_ptr<Resource> create_shared_resource(const std::string& name, int id) {
    return std::make_shared<Resource>(name, id);
}

// ===================================================================
// PYBIND11 MODULE DEFINITION
// ===================================================================

PYBIND11_MODULE(pybind_example, m) {
    m.doc() = "pybind11 example module - C++ and Python interoperability";
    
    // ===============================================================
    // 1. SIMPLE FUNCTIONS
    // ===============================================================
    m.def("add", &add, "Add two integers",
          py::arg("a"), py::arg("b"));
    
    m.def("multiply", &multiply, "Multiply two numbers",
          py::arg("a"), py::arg("b") = 2.0);
    
    m.def("divide_with_remainder", &divide_with_remainder,
          "Divide and return (quotient, remainder)");
    
    m.def("greet", &greet, "Greet someone");
    
    // ===============================================================
    // 2. STL CONTAINERS
    // ===============================================================
    m.def("square_elements", &square_elements,
          "Square all elements in a list");
    
    m.def("count_words", &count_words,
          "Count occurrences of each word");
    
    // ===============================================================
    // 3. VECTOR2D CLASS
    // ===============================================================
    py::class_<Vector2D>(m, "Vector2D")
        .def(py::init<>())
        .def(py::init<double, double>())
        .def_property("x", &Vector2D::getX, &Vector2D::setX)
        .def_property("y", &Vector2D::getY, &Vector2D::setY)
        .def("length", &Vector2D::length)
        .def("__add__", &Vector2D::operator+)
        .def("__mul__", &Vector2D::operator*)
        .def("__repr__", &Vector2D::toString)
        .def_static("zero", &Vector2D::zero);
    
    // ===============================================================
    // 4. INHERITANCE - SHAPES
    // ===============================================================
    py::class_<Shape>(m, "Shape")
        .def("area", &Shape::area)
        .def("get_name", &Shape::getName);
    
    py::class_<Circle, Shape>(m, "Circle")
        .def(py::init<const std::string&, double>())
        .def("get_radius", &Circle::getRadius);
    
    py::class_<Rectangle, Shape>(m, "Rectangle")
        .def(py::init<const std::string&, double, double>());
    
    // ===============================================================
    // 5. CALLBACKS
    // ===============================================================
    m.def("process_with_callback", &process_with_callback,
          "Process data with Python callback function");
    
    m.def("integrate", &integrate,
          "Numerical integration using Python callback",
          py::arg("func"), py::arg("start"), py::arg("end"),
          py::arg("steps") = 1000);
    
    // ===============================================================
    // 6. SENSOR DATA PROCESSING
    // ===============================================================
    py::class_<SensorReading>(m, "SensorReading")
        .def(py::init<double, double, const std::string&>())
        .def_readonly("timestamp", &SensorReading::timestamp)
        .def_readonly("value", &SensorReading::value)
        .def_readonly("sensor_id", &SensorReading::sensor_id);
    
    py::class_<SensorProcessor>(m, "SensorProcessor")
        .def(py::init<>())
        .def("add_reading", &SensorProcessor::addReading)
        .def("get_reading_count", &SensorProcessor::getReadingCount)
        .def("get_readings", &SensorProcessor::getReadings)
        .def("process_readings", &SensorProcessor::processReadings)
        .def("filter_readings", &SensorProcessor::filterReadings);
    
    // ===============================================================
    // 7. NUMPY ARRAYS
    // ===============================================================
    m.def("numpy_square", &numpy_square,
          "Square all elements in NumPy array (C++ speed!)");
    
    m.def("matrix_multiply", &matrix_multiply,
          "Matrix multiplication using NumPy arrays");
    
    // ===============================================================
    // 8. SMART POINTERS
    // ===============================================================
    py::class_<Resource, std::shared_ptr<Resource>>(m, "Resource")
        .def("get_name", &Resource::getName)
        .def("get_id", &Resource::getId);
    
    m.def("create_resource", &create_resource,
          "Create a resource (returns unique_ptr)");
    
    m.def("create_shared_resource", &create_shared_resource,
          "Create a shared resource (returns shared_ptr)");
}

// ===================================================================
// STANDALONE EXAMPLE (FOR DOCUMENTATION)
// ===================================================================

/*
PYTHON USAGE EXAMPLES:
======================

# 1. Simple functions
import pybind_example as pe

result = pe.add(5, 3)
print(f"5 + 3 = {result}")  # Output: 5 + 3 = 8

value = pe.multiply(4.5)
print(f"4.5 * 2.0 = {value}")  # Output: 4.5 * 2.0 = 9.0

quotient, remainder = pe.divide_with_remainder(17, 5)
print(f"17 / 5 = {quotient} remainder {remainder}")  # Output: 17 / 5 = 3 remainder 2

message = pe.greet("World")
print(message)  # Output: Hello, World!

# 2. STL containers
numbers = [1, 2, 3, 4, 5]
squared = pe.square_elements(numbers)
print(f"Squared: {squared}")  # Output: Squared: [1, 4, 9, 16, 25]

words = ["hello", "world", "hello", "python"]
counts = pe.count_words(words)
print(f"Word counts: {counts}")  # Output: Word counts: {'hello': 2, 'world': 1, 'python': 1}

# 3. Classes
v1 = pe.Vector2D(3.0, 4.0)
v2 = pe.Vector2D(1.0, 2.0)
print(f"v1 = {v1}")  # Output: v1 = Vector2D(3.000000, 4.000000)
print(f"v1.length() = {v1.length()}")  # Output: v1.length() = 5.0

v3 = v1 + v2
print(f"v1 + v2 = {v3}")  # Output: v1 + v2 = Vector2D(4.000000, 6.000000)

v4 = v1 * 2.0
print(f"v1 * 2.0 = {v4}")  # Output: v1 * 2.0 = Vector2D(6.000000, 8.000000)

# 4. Inheritance
circle = pe.Circle("MyCircle", 5.0)
rect = pe.Rectangle("MyRect", 4.0, 6.0)
print(f"{circle.get_name()} area: {circle.area()}")  # Output: MyCircle area: 78.53975
print(f"{rect.get_name()} area: {rect.area()}")  # Output: MyRect area: 24.0

# 5. Callbacks - CALLING PYTHON FROM C++!
def my_python_function(x):
    return x * 2 + 1

data = [1, 2, 3, 4, 5]
result = pe.process_with_callback(data, my_python_function)
print(f"Processed: {result}")  # Output: Processed: [3, 5, 7, 9, 11]

# Integrate a Python function using C++
import math
result = pe.integrate(lambda x: x**2, 0, 1, steps=10000)
print(f"Integral of x^2 from 0 to 1: {result}")  # Output: ~0.333333

result = pe.integrate(math.sin, 0, math.pi, steps=10000)
print(f"Integral of sin(x) from 0 to π: {result}")  # Output: ~2.0

# 6. Sensor processing with callbacks
processor = pe.SensorProcessor()
processor.add_reading(0.0, 25.5, "TEMP_01")
processor.add_reading(1.0, 26.2, "TEMP_01")
processor.add_reading(2.0, 24.8, "TEMP_02")

# Process with Python function
def extract_value(reading):
    return reading.value

values = processor.process_readings(extract_value)
print(f"Values: {values}")  # Output: Values: [25.5, 26.2, 24.8]

# Filter with Python predicate
def high_temp(reading):
    return reading.value > 25.0

high_temps = processor.filter_readings(high_temp)
print(f"High temps: {len(high_temps)}")  # Output: High temps: 2

# 7. NumPy arrays (high performance!)
import numpy as np

arr = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
squared = pe.numpy_square(arr)  # Fast C++ processing!
print(f"NumPy squared: {squared}")  # Output: NumPy squared: [1. 4. 9. 16. 25.]

A = np.array([[1.0, 2.0], [3.0, 4.0]])
B = np.array([[5.0, 6.0], [7.0, 8.0]])
C = pe.matrix_multiply(A, B)
print(f"Matrix multiply:\n{C}")
# Output:
# [[19. 22.]
#  [43. 50.]]

# 8. Smart pointers
resource = pe.create_shared_resource("MyResource", 42)
print(f"Resource: {resource.get_name()}, ID: {resource.get_id()}")
# Output: Resource: MyResource, ID: 42
# When resource goes out of scope, C++ destructor is automatically called

BUILD SCRIPT (build.sh):
========================
#!/bin/bash

# Compile the pybind11 extension
c++ -O3 -Wall -shared -std=c++17 -fPIC \
    $(python3 -m pybind11 --includes) \
    Pybind11Example.cpp \
    -o pybind_example$(python3-config --extension-suffix)

# Run Python tests
python3 test_pybind.py

ALTERNATIVE FRAMEWORKS:
=======================

1. BOOST.PYTHON (older, requires Boost):
   #include <boost/python.hpp>
   BOOST_PYTHON_MODULE(example) {
       boost::python::def("add", add);
   }

2. SWIG (multi-language support):
   // example.i
   %module example
   %{
   #include "example.h"
   %}
   %include "example.h"
   
   Build: swig -python -c++ example.i

3. CTYPES (Python standard library):
   // Compile: g++ -shared -fPIC example.cpp -o example.so
   
   # Python:
   import ctypes
   lib = ctypes.CDLL('./example.so')
   lib.add.argtypes = [ctypes.c_int, ctypes.c_int]
   lib.add.restype = ctypes.c_int
   result = lib.add(5, 3)

4. CFFI (C Foreign Function Interface):
   from cffi import FFI
   ffi = FFI()
   ffi.cdef("int add(int, int);")
   lib = ffi.dlopen('./example.so')
   result = lib.add(5, 3)

5. NANOBIND (modern, lightweight):
   #include <nanobind/nanobind.h>
   NB_MODULE(example, m) {
       m.def("add", &add);
   }

PERFORMANCE COMPARISON:
=======================
For numerical computations:
- Pure Python: 1x (baseline)
- NumPy: 10-100x faster
- C++ via pybind11: 100-1000x faster
- Direct C++ (no Python): 1000x+ faster

WHEN TO USE:
============
✓ Use pybind11 when:
  • Need performance-critical code in C++
  • Have existing C++ libraries to expose
  • Want type safety and automatic conversions
  • Need to process NumPy arrays efficiently
  • Want modern C++ features (C++11-20)

✗ Avoid when:
  • Pure Python is fast enough
  • No C++ expertise available
  • Deployment complexity is a concern
  • Only need simple C functions (use ctypes)
*/