#!/bin/bash

# ===================================================================
# BUILD SCRIPT FOR PYBIND11 EXAMPLE
# ===================================================================
# This script compiles the C++ code into a Python extension module
# and runs the test suite.
# ===================================================================

set -e  # Exit on error

echo "======================================================================="
echo "BUILDING PYBIND11 EXAMPLE"
echo "======================================================================="

# Check if pybind11 is installed
if ! python3 -c "import pybind11" 2>/dev/null; then
    echo "ERROR: pybind11 not found!"
    echo "Install with: pip install pybind11"
    exit 1
fi

# Get Python configuration
PYTHON_INCLUDES=$(python3 -m pybind11 --includes)
EXTENSION_SUFFIX=$(python3-config --extension-suffix)

echo ""
echo "Python configuration:"
echo "  Python includes: $PYTHON_INCLUDES"
echo "  Extension suffix: $EXTENSION_SUFFIX"
echo ""

# Compile the extension
echo "Compiling C++ extension module..."
c++ -O3 -Wall -shared -std=c++17 -fPIC \
    $PYTHON_INCLUDES \
    src/Pybind11Example.cpp \
    -o pybind_example$EXTENSION_SUFFIX

if [ $? -eq 0 ]; then
    echo "✓ Compilation successful!"
    echo "  Output: pybind_example$EXTENSION_SUFFIX"
else
    echo "✗ Compilation failed!"
    exit 1
fi

# Run tests
echo ""
echo "======================================================================="
echo "RUNNING TESTS"
echo "======================================================================="
echo ""

python3 test_pybind.py

echo ""
echo "======================================================================="
echo "BUILD AND TEST COMPLETE"
echo "======================================================================="
