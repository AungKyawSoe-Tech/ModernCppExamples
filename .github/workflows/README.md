# GitHub Actions Workflows

This directory contains GitHub Actions workflows for building and testing the Modern C++ Examples project.

## Workflows

### 1. Build and Test (`build-and-test.yml`)
**Trigger:** Push/PR to main/master/develop branches, manual dispatch

**Purpose:** Comprehensive build and test of all examples

**Matrix Strategy:**
- Compilers: GCC 13, Clang 17
- Build Types: Release, Debug

**Steps:**
1. Install dependencies and compilers
2. Configure CMake with specified compiler
3. Build all 20 examples
4. Run comprehensive C++ standard examples (C++11, C++14, C++17, C++20)
5. Run algorithm examples
6. Run feature-specific examples
7. Upload executables as artifacts (retained for 7 days)

**Usage:**
- Automatically runs on push/PR
- Manual trigger: Go to Actions → Build and Test → Run workflow

---

### 2. Comprehensive Examples (`comprehensive-examples.yml`)
**Trigger:** Changes to Cpp{11,14,17,20}Examples.cpp or CMakeLists.txt, manual dispatch

**Purpose:** Test each C++ standard independently

**Jobs:**
- `test-cpp11` - Build and run C++11 examples
- `test-cpp14` - Build and run C++14 examples  
- `test-cpp17` - Build and run C++17 examples
- `test-cpp20` - Build and run C++20 examples
- `verify-all-standards` - Confirmation job when all pass

**Usage:**
- Automatically runs when comprehensive example files are modified
- Manual trigger: Go to Actions → Test Comprehensive C++ Standard Examples → Run workflow

---

### 3. Quick Test (`quick-test.yml`)
**Trigger:** Push to main/master, manual dispatch

**Purpose:** Fast smoke test of comprehensive examples

**Features:**
- Single job, fast execution
- 30-second timeout per example
- Release build only
- Displays build summary

**Usage:**
- Automatically runs on push to main/master
- Manual trigger: Go to Actions → Quick Test → Run workflow

---

## Status Badges

Add these to your README.md:

```markdown
![Build and Test](https://github.com/YOUR_USERNAME/ModernCppExamples/workflows/Build%20and%20Test/badge.svg)
![Comprehensive Examples](https://github.com/YOUR_USERNAME/ModernCppExamples/workflows/Test%20Comprehensive%20C++%20Standard%20Examples/badge.svg)
![Quick Test](https://github.com/YOUR_USERNAME/ModernCppExamples/workflows/Quick%20Test/badge.svg)
```

Replace `YOUR_USERNAME` with your GitHub username.

---

## Compiler Support

### GCC 13
- Full C++11/14/17/20 support
- Default compiler in workflows
- Available on Ubuntu 22.04+

### Clang 17
- Full C++11/14/17/20 support
- Provides additional validation
- Installed via LLVM script

---

## Build Matrix

| Compiler | C++11 | C++14 | C++17 | C++20 |
|----------|-------|-------|-------|-------|
| GCC 13   | ✅    | ✅    | ✅    | ✅    |
| Clang 17 | ✅    | ✅    | ✅    | ✅    |

---

## Artifacts

Build artifacts (executables) are uploaded and retained for 7 days:
- `executables-g++-13-Release`
- `executables-g++-13-Debug`
- `executables-clang++-17-Release`
- `executables-clang++-17-Debug`

Download from: Actions → Workflow Run → Artifacts section

---

## Troubleshooting

### Workflow fails on specific example
1. Check the workflow logs for the specific step
2. Look for compilation errors or runtime issues
3. Test locally with the same compiler version

### Timeout errors
The quick-test workflow uses 30s timeouts. If an example legitimately needs more time:
```yaml
timeout 60s ./build/bin/ExampleName  # Increase timeout
```

### Compiler not found
Ensure the compiler installation step matches the Ubuntu version:
```yaml
sudo apt-get install -y g++-13  # For GCC
```

---

## Local Testing

To test what the workflows do locally:

```bash
# Install same dependencies
sudo apt-get update
sudo apt-get install -y cmake g++-13

# Build
mkdir build && cd build
CXX=g++-13 cmake ..
make -j$(nproc)

# Run comprehensive examples
./bin/Cpp11Examples
./bin/Cpp14Examples
./bin/Cpp17Examples
./bin/Cpp20Examples
```

---

## Future Enhancements

Potential improvements:
- Add code coverage reporting
- Add static analysis (clang-tidy, cppcheck)
- Add performance benchmarking
- Add memory leak detection (valgrind)
- Add sanitizers (address, thread, undefined behavior)
- Add Windows/macOS builds
