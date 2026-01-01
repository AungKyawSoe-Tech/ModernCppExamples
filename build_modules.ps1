# ===================================================================
# Build Script for C++20 Modules Example (Windows PowerShell)
# ===================================================================
# C++20 modules require special compilation steps that aren't yet
# fully supported by standard CMake workflows.
# ===================================================================

$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "================================================================"
Write-Host "  Building C++20 Modules Example: TemplatedCameraModules"
Write-Host "================================================================"

# Detect compiler
$compiler = $null
$compilerType = $null

# Check for MSVC (cl.exe)
if (Get-Command cl.exe -ErrorAction SilentlyContinue) {
    $compiler = "cl.exe"
    $compilerType = "MSVC"
    Write-Host "Using: MSVC (cl.exe)"
}
# Check for g++
elseif (Get-Command g++.exe -ErrorAction SilentlyContinue) {
    $compiler = "g++.exe"
    $compilerType = "GCC"
    Write-Host "Using: g++"
}
# Check for clang++
elseif (Get-Command clang++.exe -ErrorAction SilentlyContinue) {
    $compiler = "clang++.exe"
    $compilerType = "Clang"
    Write-Host "Using: clang++"
}
else {
    Write-Host "Error: No C++ compiler found!"
    Write-Host "Please install MSVC, GCC, or Clang"
    exit 1
}

Write-Host ""
Write-Host "Compiler type: $compilerType"

# Create build directory
$buildDir = "build\modules"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
}

Set-Location $buildDir

Write-Host ""
Write-Host "================================================================"
Write-Host "  Step 1: Compiling module interface (CameraModule.cppm)"
Write-Host "================================================================"

try {
    if ($compilerType -eq "MSVC") {
        # MSVC has the best C++20 modules support
        Write-Host "Command: cl /std:c++20 /experimental:module /EHsc /c ..\..\src\CameraModule.cppm"
        cl /std:c++20 /experimental:module /EHsc /c ..\..\src\CameraModule.cppm
        
        if ($LASTEXITCODE -ne 0) {
            throw "Module compilation failed!"
        }
        Write-Host "✅ Module compiled successfully!"
        
        Write-Host ""
        Write-Host "================================================================"
        Write-Host "  Step 2: Compiling main program (TemplatedCameraModules.cpp)"
        Write-Host "================================================================"
        
        Write-Host "Command: cl /std:c++20 /experimental:module /EHsc ..\..\src\TemplatedCameraModules.cpp CameraModule.obj"
        cl /std:c++20 /experimental:module /EHsc ..\..\src\TemplatedCameraModules.cpp CameraModule.obj
        
        if ($LASTEXITCODE -ne 0) {
            throw "Program compilation failed!"
        }
        Write-Host "✅ Program compiled successfully!"
        
    } elseif ($compilerType -eq "GCC") {
        # GCC experimental modules support
        Write-Host "Command: g++ -std=c++20 -fmodules-ts -c ..\..\src\CameraModule.cppm"
        & g++ -std=c++20 -fmodules-ts -c ..\..\src\CameraModule.cppm
        
        if ($LASTEXITCODE -ne 0) {
            throw "Module compilation failed!"
        }
        Write-Host "✅ Module compiled successfully!"
        
        Write-Host ""
        Write-Host "================================================================"
        Write-Host "  Step 2: Compiling main program (TemplatedCameraModules.cpp)"
        Write-Host "================================================================"
        
        Write-Host "Command: g++ -std=c++20 -fmodules-ts ..\..\src\TemplatedCameraModules.cpp CameraModule.o -o TemplatedCameraModules.exe"
        & g++ -std=c++20 -fmodules-ts ..\..\src\TemplatedCameraModules.cpp CameraModule.o -o TemplatedCameraModules.exe
        
        if ($LASTEXITCODE -ne 0) {
            throw "Program compilation failed!"
        }
        Write-Host "✅ Program compiled successfully!"
        
    } elseif ($compilerType -eq "Clang") {
        # Clang modules support
        Write-Host "Command: clang++ -std=c++20 -fmodules -c ..\..\src\CameraModule.cppm"
        & clang++ -std=c++20 -fmodules -c ..\..\src\CameraModule.cppm
        
        if ($LASTEXITCODE -ne 0) {
            throw "Module compilation failed!"
        }
        Write-Host "✅ Module compiled successfully!"
        
        Write-Host ""
        Write-Host "================================================================"
        Write-Host "  Step 2: Compiling main program (TemplatedCameraModules.cpp)"
        Write-Host "================================================================"
        
        Write-Host "Command: clang++ -std=c++20 -fmodules ..\..\src\TemplatedCameraModules.cpp CameraModule.o -o TemplatedCameraModules.exe"
        & clang++ -std=c++20 -fmodules ..\..\src\TemplatedCameraModules.cpp CameraModule.o -o TemplatedCameraModules.exe
        
        if ($LASTEXITCODE -ne 0) {
            throw "Program compilation failed!"
        }
        Write-Host "✅ Program compiled successfully!"
    }
    
    Write-Host ""
    Write-Host "================================================================"
    Write-Host "  Step 3: Running program"
    Write-Host "================================================================"
    
    if ($compilerType -eq "MSVC") {
        .\TemplatedCameraModules.exe
    } else {
        .\TemplatedCameraModules.exe
    }
    
    Write-Host ""
    Write-Host "================================================================"
    Write-Host "  ✅ Build and execution completed successfully!"
    Write-Host "================================================================"
    Write-Host ""
    Write-Host "Executable location: $buildDir\TemplatedCameraModules.exe"
    Write-Host "Module object: $buildDir\CameraModule.obj (or .o)"
    Write-Host ""
    
} catch {
    Write-Host ""
    Write-Host "❌ Build failed: $_"
    Write-Host ""
    Write-Host "Note: C++20 modules support varies by compiler:"
    Write-Host "  - MSVC 2022+: Best support (use /std:c++20 /experimental:module)"
    Write-Host "  - GCC: Experimental support (use -fmodules-ts)"
    Write-Host "  - Clang 16+: Use -fmodules"
    Write-Host ""
    Write-Host "Make sure you have a recent compiler version!"
    exit 1
}
