#!/bin/bash
# ===================================================================
# Build Script for C++20 Modules Example
# ===================================================================
# C++20 modules require special compilation steps that aren't yet
# fully supported by standard CMake workflows.
# ===================================================================

set -e  # Exit on error

echo "================================================================"
echo "  Building C++20 Modules Example: TemplatedCameraModules"
echo "================================================================"

# Detect compiler
if command -v g++-13 &> /dev/null; then
    CXX=g++-13
    echo "Using: g++-13"
elif command -v g++ &> /dev/null; then
    CXX=g++
    echo "Using: g++"
else
    echo "Error: No C++ compiler found!"
    exit 1
fi

# Check C++20 support
echo ""
echo "Checking C++20 support..."
$CXX --version

# Create build directory
mkdir -p build/modules
cd build/modules

echo ""
echo "================================================================"
echo "  Step 1: Compiling module interface (CameraModule.cppm)"
echo "================================================================"

# GCC requires -fmodules-ts for C++20 modules
$CXX -std=c++20 -fmodules-ts -c ../../src/CameraModule.cppm -o CameraModule.o

if [ $? -eq 0 ]; then
    echo "✅ Module compiled successfully!"
else
    echo "❌ Module compilation failed!"
    echo ""
    echo "Note: C++20 modules support varies by compiler:"
    echo "  - GCC: Experimental support (use -fmodules-ts)"
    echo "  - Clang 16+: Use -fmodules"
    echo "  - MSVC 2022+: Best support (use /std:c++20 /experimental:module)"
    exit 1
fi

echo ""
echo "================================================================"
echo "  Step 2: Compiling main program (TemplatedCameraModules.cpp)"
echo "================================================================"

$CXX -std=c++20 -fmodules-ts ../../src/TemplatedCameraModules.cpp CameraModule.o -o TemplatedCameraModules

if [ $? -eq 0 ]; then
    echo "✅ Program compiled successfully!"
else
    echo "❌ Program compilation failed!"
    exit 1
fi

echo ""
echo "================================================================"
echo "  Step 3: Running program"
echo "================================================================"

./TemplatedCameraModules

echo ""
echo "================================================================"
echo "  ✅ Build and execution completed successfully!"
echo "================================================================"
echo ""
echo "Executable location: build/modules/TemplatedCameraModules"
echo "Module object: build/modules/CameraModule.o"
echo ""
