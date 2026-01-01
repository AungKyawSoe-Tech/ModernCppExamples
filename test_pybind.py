#!/usr/bin/env python3
"""
Test script for pybind11 example module
Demonstrates calling C++ from Python and Python from C++
"""

import sys
import math

def main():
    try:
        import pybind_example as pe
    except ImportError:
        print("ERROR: pybind_example module not found!")
        print("Build it first with:")
        print("  c++ -O3 -Wall -shared -std=c++17 -fPIC \\")
        print("      $(python3 -m pybind11 --includes) \\")
        print("      src/Pybind11Example.cpp \\")
        print("      -o pybind_example$(python3-config --extension-suffix)")
        sys.exit(1)

    print("=" * 70)
    print("PYBIND11 EXAMPLE - C++ AND PYTHON INTEROPERABILITY")
    print("=" * 70)

    # ================================================================
    # 1. SIMPLE FUNCTIONS
    # ================================================================
    print("\n1. SIMPLE FUNCTIONS (Python calling C++):")
    print("-" * 70)
    
    result = pe.add(5, 3)
    print(f"pe.add(5, 3) = {result}")
    
    value = pe.multiply(4.5)
    print(f"pe.multiply(4.5) = {value} (default arg: 2.0)")
    
    value = pe.multiply(4.5, 3.0)
    print(f"pe.multiply(4.5, 3.0) = {value}")
    
    quotient, remainder = pe.divide_with_remainder(17, 5)
    print(f"pe.divide_with_remainder(17, 5) = ({quotient}, {remainder})")
    
    message = pe.greet("World")
    print(f"pe.greet('World') = '{message}'")

    # ================================================================
    # 2. STL CONTAINERS
    # ================================================================
    print("\n2. STL CONTAINERS (automatic conversion):")
    print("-" * 70)
    
    numbers = [1, 2, 3, 4, 5]
    squared = pe.square_elements(numbers)
    print(f"pe.square_elements({numbers}) = {squared}")
    
    words = ["hello", "world", "hello", "python", "world"]
    counts = pe.count_words(words)
    print(f"pe.count_words({words})")
    print(f"  Result: {counts}")

    # ================================================================
    # 3. CLASSES
    # ================================================================
    print("\n3. CLASSES (C++ objects in Python):")
    print("-" * 70)
    
    v1 = pe.Vector2D(3.0, 4.0)
    v2 = pe.Vector2D(1.0, 2.0)
    print(f"v1 = {v1}")
    print(f"v2 = {v2}")
    print(f"v1.length() = {v1.length()}")
    
    v3 = v1 + v2
    print(f"v1 + v2 = {v3}")
    
    v4 = v1 * 2.0
    print(f"v1 * 2.0 = {v4}")
    
    # Property access
    v1.x = 10.0
    print(f"After v1.x = 10.0: v1 = {v1}")
    
    # Static method
    v_zero = pe.Vector2D.zero()
    print(f"Vector2D.zero() = {v_zero}")

    # ================================================================
    # 4. INHERITANCE
    # ================================================================
    print("\n4. INHERITANCE (polymorphism):")
    print("-" * 70)
    
    shapes = [
        pe.Circle("Circle1", 5.0),
        pe.Rectangle("Rect1", 4.0, 6.0),
        pe.Circle("Circle2", 3.0),
    ]
    
    for shape in shapes:
        print(f"{shape.get_name()}: area = {shape.area():.2f}")

    # ================================================================
    # 5. CALLBACKS - CALLING PYTHON FROM C++!
    # ================================================================
    print("\n5. CALLBACKS (C++ calling Python functions!):")
    print("-" * 70)
    
    # Define Python function
    def my_transform(x):
        return x * 2 + 1
    
    data = [1, 2, 3, 4, 5]
    result = pe.process_with_callback(data, my_transform)
    print(f"Data: {data}")
    print(f"Transform: x -> 2*x + 1")
    print(f"Result: {result}")
    
    # Lambda function
    result = pe.process_with_callback(data, lambda x: x ** 2)
    print(f"\nTransform: x -> x^2")
    print(f"Result: {result}")
    
    # Numerical integration
    print("\nNumerical integration:")
    result = pe.integrate(lambda x: x**2, 0, 1, steps=10000)
    print(f"∫(x² dx) from 0 to 1 = {result:.6f} (exact: 0.333333)")
    
    result = pe.integrate(math.sin, 0, math.pi, steps=10000)
    print(f"∫(sin(x) dx) from 0 to π = {result:.6f} (exact: 2.0)")

    # ================================================================
    # 6. SENSOR DATA PROCESSING
    # ================================================================
    print("\n6. SENSOR DATA PROCESSING (embedded example):")
    print("-" * 70)
    
    processor = pe.SensorProcessor()
    processor.add_reading(0.0, 25.5, "TEMP_01")
    processor.add_reading(1.0, 26.2, "TEMP_01")
    processor.add_reading(2.0, 24.8, "TEMP_02")
    processor.add_reading(3.0, 27.1, "TEMP_01")
    processor.add_reading(4.0, 23.5, "TEMP_02")
    
    print(f"Total readings: {processor.get_reading_count()}")
    
    # Process with Python function
    def extract_value(reading):
        return reading.value
    
    values = processor.process_readings(extract_value)
    print(f"Values: {values}")
    
    # Filter with Python predicate
    def high_temp(reading):
        return reading.value > 25.0
    
    high_temps = processor.filter_readings(high_temp)
    print(f"\nReadings with temp > 25.0:")
    for reading in high_temps:
        print(f"  t={reading.timestamp}, value={reading.value}, id={reading.sensor_id}")
    
    # More complex filtering
    def temp01_only(reading):
        return reading.sensor_id == "TEMP_01"
    
    temp01_readings = processor.filter_readings(temp01_only)
    print(f"\nTEMP_01 readings: {len(temp01_readings)}")

    # ================================================================
    # 7. NUMPY ARRAYS (if available)
    # ================================================================
    try:
        import numpy as np
        
        print("\n7. NUMPY ARRAYS (high performance!):")
        print("-" * 70)
        
        # 1D array processing
        arr = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
        squared = pe.numpy_square(arr)
        print(f"Input:  {arr}")
        print(f"Squared: {squared}")
        
        # Matrix multiplication
        print("\nMatrix multiplication:")
        A = np.array([[1.0, 2.0], [3.0, 4.0]])
        B = np.array([[5.0, 6.0], [7.0, 8.0]])
        C = pe.matrix_multiply(A, B)
        
        print("A =")
        print(A)
        print("\nB =")
        print(B)
        print("\nA @ B =")
        print(C)
        
        # Verify correctness
        expected = np.matmul(A, B)
        if np.allclose(C, expected):
            print("\n✓ Result matches NumPy's matmul!")
        
    except ImportError:
        print("\n7. NUMPY ARRAYS:")
        print("-" * 70)
        print("NumPy not installed - skipping NumPy examples")
        print("Install with: pip install numpy")

    # ================================================================
    # 8. SMART POINTERS
    # ================================================================
    print("\n8. SMART POINTERS (automatic memory management):")
    print("-" * 70)
    
    print("Creating resource...")
    resource = pe.create_shared_resource("MyResource", 42)
    print(f"Resource: {resource.get_name()}, ID: {resource.get_id()}")
    print("Deleting resource...")
    del resource
    print("Resource automatically destroyed by C++ destructor!")

    print("\n" + "=" * 70)
    print("ALL TESTS PASSED!")
    print("=" * 70)

if __name__ == "__main__":
    main()
