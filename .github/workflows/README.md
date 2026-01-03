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

### 4. Pybind11 Examples (`pybind11-examples.yml`)
**Trigger:** Changes to Pybind11Example.cpp, test_pybind.py, or build script, manual dispatch

**Purpose:** Build and test C++/Python interoperability examples

**Matrix Strategy:**
- Operating Systems: Ubuntu, macOS, Windows
- Python Versions: 3.8, 3.9, 3.10, 3.11, 3.12

**Jobs:**
- `test-pybind11` - Build extension module and run tests on all platforms/versions
- `test-pybind11-minimal` - Test basic functionality without NumPy dependency
- `verify-build-script` - Test the scripts/build_pybind.sh script
- `documentation-check` - Verify documentation completeness
- `summary` - Display test summary

**Features:**
- Cross-platform testing (Linux, macOS, Windows)
- Multiple Python version support (3.8-3.12)
- Tests with and without NumPy
- Builds Python extension modules
- Upload artifacts for each platform/version
- Comprehensive documentation verification

**Usage:**
- Automatically runs when pybind11 files are modified
- Manual trigger: Go to Actions → Build and Test Pybind11 Examples → Run workflow

**Build Instructions Included:**
- Linux/macOS: Uses GCC/Clang with pybind11 includes
- Windows: Uses MSVC with proper Python library linking
- Build script verification for reproducible builds

---

### 5. Eigen Examples (`eigen-examples.yml`)
**Trigger:** Changes to EigenSensorFusion.cpp or CMakeLists.txt, manual dispatch

**Purpose:** Build and test Eigen-based sensor fusion and particle filter examples

**Matrix Strategy:**
- Operating Systems: Ubuntu, macOS
- Build Types: Release, Debug
- Eigen Versions: 3.3.9, 3.4.0

**Jobs:**
- `test-eigen-sensor-fusion` - Build and run on Linux/macOS with different build types
- `test-eigen-versions` - Test compatibility with multiple Eigen versions
- `test-manual-compilation` - Test direct g++ compilation without CMake
- `performance-benchmark` - Measure execution time and memory usage
- `documentation-check` - Verify code documentation and structure
- `cross-compiler-test` - Cross-compile for ARM embedded systems
- `summary` - Display comprehensive test summary

**Features:**
- Multi-platform testing (Linux, macOS)
- Eigen version compatibility testing
- Multiple optimization levels (-O0, -O2, -O3 -march=native)
- Performance profiling with `/usr/bin/time`
- ARM cross-compilation for embedded systems
- Automatic Eigen3 installation verification

**Algorithms Tested:**
- Kalman Filter - GPS/IMU sensor fusion
- Complementary Filter - Accelerometer/Gyroscope fusion
- Particle Filter - Monte Carlo localization (500 particles)
- Extended Kalman Filter - Nonlinear sensor fusion
- Complete Sensor Fusion Pipeline - Multi-rate sensor integration

**Build Instructions Included:**
```bash
# CMake method
mkdir build && cd build
cmake ..
make EigenSensorFusion

# Direct compilation
g++ -std=c++17 -O3 -march=native -I/usr/include/eigen3 \
    src/EigenSensorFusion.cpp -o EigenSensorFusion

# ARM cross-compilation
arm-linux-gnueabihf-g++ -std=c++17 -O2 -I/usr/include/eigen3 \
    src/EigenSensorFusion.cpp -o EigenSensorFusion_arm
```

**Usage:**
- Automatically runs when Eigen files are modified
- Manual trigger: Go to Actions → Build and Test Eigen Examples → Run workflow

---

## Status Badges

Add these to your README.md:

```markdown
![Build and Test](https://github.com/YOUR_USERNAME/ModernCppExamples/workflows/Build%20and%20Test/badge.svg)
![Comprehensive Examples](https://github.com/YOUR_USERNAME/ModernCppExamples/workflows/Test%20Comprehensive%20C++%20Standard%20Examples/badge.svg)
![Quick Test](https://github.com/YOUR_USERNAME/ModernCppExamples/workflows/Quick%20Test/badge.svg)
![Pybind11 Examples](https://github.com/YOUR_USERNAME/ModernCppExamples/workflows/Build%20and%20Test%20Pybind11%20Examples/badge.svg)
![Eigen Examples](https://github.com/YOUR_USERNAME/ModernCppExamples/workflows/Build%20and%20Test%20Eigen%20Examples/badge.svg)
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

Build artifacts are uploaded and retained for 7 days:

**C++ Executables:**
- `executables-g++-13-Release`
- `executables-g++-13-Debug`
- `executables-clang++-17-Release`
- `executables-clang++-17-Debug`

**Pybind11 Modules:**
- `pybind11-module-ubuntu-latest-py3.X`
- `pybind11-module-macos-latest-py3.X`
- `pybind11-module-windows-latest-py3.X`

**Eigen Examples:**
- `EigenSensorFusion-ubuntu-latest-Release`
- `EigenSensorFusion-ubuntu-latest-Debug`
- `EigenSensorFusion-macos-latest-Release`
- `EigenSensorFusion-macos-latest-Debug`
- `EigenSensorFusion-arm` (ARM cross-compiled)
- `performance-log` (Performance benchmark results)

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

### C++ Examples
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

### Pybind11 Examples
```bash
# Install Python dependencies
pip install pybind11 numpy

# Build (Linux/macOS)
c++ -O3 -Wall -shared -std=c++17 -fPIC \
    $(python3 -m pybind11 --includes) \
    src/Pybind11Example.cpp \
    -o pybind_example$(python3-config --extension-suffix)

# Or use the build script
chmod +x scripts/build_pybind.sh
./scripts/build_pybind.sh

# Run tests
python test_pybind.py

# Test import
python -c "import pybind_example as pe; print(pe.add(5, 3))"
```

### Eigen Examples
```bash
# Install Eigen3
sudo apt-get install -y libeigen3-dev  # Ubuntu/Debian
brew install eigen                      # macOS

# Build with CMake
mkdir build && cd build
cmake ..
make EigenSensorFusion

# Run
./bin/EigenSensorFusion

# Or compile directly
g++ -std=c++17 -O3 -march=native -I/usr/include/eigen3 \
    src/EigenSensorFusion.cpp -o EigenSensorFusion
./EigenSensorFusion

# Performance benchmark
/usr/bin/time -v ./bin/EigenSensorFusion
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

---

## 📄 PDF Generation (NEW!)

### Automatic PDF Generation Job

**Added to:** `build-and-test.yml`

**Job Name:** `generate-pdf`

**Trigger:** Runs automatically after `build-and-test` completes successfully

**What it does:**
1. Installs pandoc and LaTeX (XeLaTeX engine)
2. Combines README.md + all source files (alphabetically) + INDEX.md
3. Generates professional PDF: `ModernCppRefresherCourse.pdf`
4. Uploads PDF as artifact (90-day retention)
5. Uploads combined markdown (30-day retention)

**PDF Contents:**
- Title page with authors, version, repository link
- Table of contents (3 levels deep, auto-generated)
- Complete README with all examples
- All 72+ source code files with syntax highlighting
- Comprehensive INDEX as appendix
- Clickable GitHub repository links

---

## 📥 How to Download the PDF

### Option 1: From GitHub Actions Artifacts

1. Go to **Actions** tab on GitHub
2. Click on latest **"Build and Test C++ Examples"** run
3. Scroll to **Artifacts** section
4. Click **"ModernCppRefresherCourse-PDF"** to download

**Retention:** 90 days

### Option 2: From GitHub Releases (Recommended)

#### Create a Release with PDF:

```bash
# Tag your commit
git tag -a v1.0.0 -m "Release version 1.0.0"
git push origin v1.0.0
```

**Automatic actions:**
- Workflow generates PDF
- Creates GitHub Release
- Attaches PDF to release
- Adds release notes

**Download:**
- Go to repository → **Releases**
- Click on latest release
- Download `ModernCppRefresherCourse.pdf`

**Retention:** Permanent (attached to release)

---

## 🚀 Release Job (NEW!)

**Job Name:** `release-pdf`

**Trigger:** Only when you push a version tag (e.g., `v1.0.0`, `v2.1.3`)

**Dependencies:** Requires `generate-pdf` to complete first

**Actions:**
1. Downloads the generated PDF artifact
2. Creates a GitHub Release with the tag name
3. Attaches `ModernCppRefresherCourse.pdf` to the release
4. Includes comprehensive release notes with:
   - Version information
   - Contents summary
   - Authors
   - Download instructions

**Example:**
```bash
# Create and push a tag
git tag -a v1.0.0 -m "Initial release with all examples"
git push origin v1.0.0

# GitHub Actions will automatically:
# 1. Build and test code
# 2. Generate PDF
# 3. Create release with PDF attached
```

---

## 🔧 Customizing PDF Generation

Edit `.github/workflows/build-and-test.yml` to customize:

### Change Retention Period:
```yaml
retention-days: 90  # PDF artifacts kept for 90 days
retention-days: 30  # Markdown kept for 30 days
```

### Change PDF Engine:
```yaml
--pdf-engine=xelatex   # Current (best Unicode support)
--pdf-engine=pdflatex  # Faster, less Unicode support
--pdf-engine=lualatex  # More features, slower
```

### Change Syntax Highlighting:
```yaml
--highlight-style=tango        # Current
--highlight-style=pygments     # Default
--highlight-style=kate         # Alternative
--highlight-style=monochrome   # Black & white
--highlight-style=espresso     # Dark theme
```

### Change PDF Formatting:
```yaml
-V geometry:margin=1in    # Margins
-V fontsize=11pt          # Font size (10pt, 11pt, 12pt)
-V linkcolor=blue         # Link color
-V papersize=a4           # Paper size (letter, a4)
```

---

## 🧪 Test PDF Generation Locally

Before pushing, test locally:

**Linux/Mac:**
```bash
sudo apt-get install pandoc texlive-xetex texlive-fonts-recommended
chmod +x generate_pdf.sh
./generate_pdf.sh
```

**Windows:**
```powershell
choco install pandoc miktex
.\generate_pdf.ps1
```

**Output:** `pdf_output/ModernCppRefresherCourse.pdf`

---

## 📊 Workflow Status Badges

Add to your README.md:

```markdown
[![Build and Test](https://github.com/yourusername/ModernCppExamples/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/yourusername/ModernCppExamples/actions/workflows/build-and-test.yml)

[![Latest Release](https://img.shields.io/github/v/release/yourusername/ModernCppExamples)](https://github.com/yourusername/ModernCppExamples/releases/latest)

[![PDF Available](https://img.shields.io/badge/PDF-Download-blue)](https://github.com/yourusername/ModernCppExamples/releases/latest)
```

---

## 🐛 Troubleshooting PDF Generation

### Issue: "pandoc: xelatex not found"
**Solution:** Workflow installs XeLaTeX automatically. Check install logs.

### Issue: "LaTeX Error: File ... not found"
**Solution:** Missing LaTeX package. Add to workflow:
```yaml
sudo apt-get install -y texlive-latex-extra texlive-fonts-extra
```

### Issue: PDF generation timeout
**Solution:** Large PDF (72+ files). Increase timeout:
```yaml
timeout-minutes: 30  # Default is 360 (6 hours)
```

### Issue: PDF too large
**Solutions:**
- Split into multiple PDFs
- Use `pdflatex` instead of `xelatex`
- Reduce code samples or comments

---

## 📋 Complete Workflow Overview

```
Push to main/master/develop
  ↓
build-and-test job (Matrix: GCC/Clang, Release/Debug)
  ├─ Build all examples
  ├─ Run tests
  └─ Upload executables (artifacts, 7 days)
  ↓
build-pybind11 job (Matrix: Python 3.8-3.12)
  ├─ Build pybind11 extension
  ├─ Run Python tests
  └─ Upload Python modules (artifacts, 7 days)
  ↓
generate-pdf job
  ├─ Install pandoc + LaTeX
  ├─ Combine README + sources + INDEX
  ├─ Generate PDF with XeLaTeX
  └─ Upload PDF (artifact, 90 days)
  
If tag pushed (e.g., v1.0.0):
  ↓
release-pdf job
  ├─ Download PDF artifact
  ├─ Create GitHub Release
  └─ Attach PDF to release (permanent)
```

---

**Last Updated:** January 3, 2026  
**Version:** 1.0.0 (with PDF generation)
