#!/bin/bash

echo ""
echo "==========================================="
echo "  Running All Modern C++ Examples"
echo "==========================================="
echo ""

binPath="build/bin"

if [ ! -d "$binPath" ]; then
    echo "Error: Build directory not found. Please run build.sh first."
    exit 1
fi

executables=$(find "$binPath" -maxdepth 1 -type f -executable | sort)

if [ -z "$executables" ]; then
    echo "Error: No executables found in $binPath"
    exit 1
fi

for exe in $executables; do
    exeName=$(basename "$exe")
    echo ""
    echo "==========================================="
    echo "  Running: $exeName"
    echo "==========================================="
    echo ""
    
    "$exe"
    
    exitCode=$?
    if [ $exitCode -ne 0 ]; then
        echo ""
        echo "Warning: $exeName exited with code $exitCode"
    fi
    
    echo ""
    echo "Press Enter to continue to next example..."
    read -r
done

echo ""
echo "==========================================="
echo "  All examples completed!"
echo "==========================================="
echo ""
