#!/bin/bash

echo ""
echo "==========================================="
echo "  Building Modern C++ Examples (WSL)"
echo "==========================================="
echo ""

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

# Navigate to build directory
cd build

# Configure with CMake
echo ""
echo "Configuring with CMake..."
echo ""
cmake ..

if [ $? -ne 0 ]; then
    echo ""
    echo "CMake configuration failed!"
    cd ..
    exit 1
fi

# Build
echo ""
echo "Building all targets..."
echo ""
cmake --build . --config Release -j$(nproc)

if [ $? -ne 0 ]; then
    echo ""
    echo "Build failed!"
    cd ..
    exit 1
fi

# Return to root directory
cd ..

echo ""
echo "==========================================="
echo "  Build completed successfully!"
echo "  Executables are in: build/bin/"
echo "==========================================="
echo ""

# List built executables
echo "Built executables:"
ls -1 build/bin/
echo ""
