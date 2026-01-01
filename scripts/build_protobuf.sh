#!/bin/bash
# ===================================================================
# Build Script for Protocol Buffers Example
# ===================================================================
# This script generates C++ code from .proto files and compiles the example.

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Building Protocol Buffers Example${NC}"
echo -e "${GREEN}========================================${NC}\n"

# Check if protoc is installed
if ! command -v protoc &> /dev/null; then
    echo -e "${RED}Error: protoc not found!${NC}"
    echo "Install Protocol Buffers compiler:"
    echo "  Ubuntu/Debian: sudo apt-get install protobuf-compiler libprotobuf-dev"
    echo "  macOS: brew install protobuf"
    exit 1
fi

echo -e "${GREEN}✓${NC} protoc found: $(protoc --version)"

# Check if libprotobuf is available
if ! pkg-config --exists protobuf; then
    echo -e "${YELLOW}Warning: libprotobuf not found via pkg-config${NC}"
    echo "Trying to continue anyway..."
fi

# Create directories
mkdir -p generated
mkdir -p build

# Generate C++ code from .proto files
echo -e "\n${GREEN}Generating C++ code from .proto files...${NC}"
protoc --cpp_out=generated proto/sensor_data.proto

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} Code generation successful"
    echo "  Generated files:"
    ls -lh generated/sensor_data.pb.*
else
    echo -e "${RED}✗${NC} Code generation failed!"
    exit 1
fi

# Compile the example
echo -e "\n${GREEN}Compiling ProtobufExample...${NC}"

${CXX:-g++} -std=c++17 -Wall -Wextra \
    -I generated \
    src/ProtobufExample.cpp \
    generated/sensor_data.pb.cc \
    $(pkg-config --cflags --libs protobuf 2>/dev/null || echo "-lprotobuf") \
    -o build/ProtobufExample

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} Compilation successful"
    echo "  Output: build/ProtobufExample"
    
    # Show binary size
    size=$(stat -f%z build/ProtobufExample 2>/dev/null || stat -c%s build/ProtobufExample)
    echo -e "  Size: $((size / 1024)) KB"
else
    echo -e "${RED}✗${NC} Compilation failed!"
    exit 1
fi

# Run the example
echo -e "\n${GREEN}Running example:${NC}"
echo -e "${GREEN}========================================${NC}\n"
./build/ProtobufExample

echo -e "\n${GREEN}========================================${NC}"
echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "${GREEN}========================================${NC}"
