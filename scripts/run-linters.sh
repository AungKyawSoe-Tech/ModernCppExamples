#!/bin/bash
# ===================================================================
# Static Analysis Script for Modern C++ Examples
# ===================================================================
# This script runs multiple static analysis tools to ensure code quality,
# security, and modern C++ best practices.
#
# Tools used:
# 1. clang-tidy - LLVM static analyzer
# 2. cppcheck - Portable static analyzer
# 3. cpplint - Style checker (Google C++ Style Guide)
#
# Usage:
#   ./scripts/run-linters.sh [all|clang-tidy|cppcheck|cpplint]
#   ./scripts/run-linters.sh all          # Run all linters
#   ./scripts/run-linters.sh clang-tidy   # Run only clang-tidy

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
SRC_DIR="src"
COMPILE_COMMANDS="${BUILD_DIR}/compile_commands.json"

# Function to print section headers
print_header() {
    echo -e "\n${GREEN}========================================${NC}"
    echo -e "${GREEN}$1${NC}"
    echo -e "${GREEN}========================================${NC}\n"
}

# Function to print errors
print_error() {
    echo -e "${RED}ERROR: $1${NC}"
}

# Function to print warnings
print_warning() {
    echo -e "${YELLOW}WARNING: $1${NC}"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to run clang-tidy
run_clang_tidy() {
    print_header "Running clang-tidy"
    
    if ! command_exists clang-tidy; then
        print_error "clang-tidy not found. Install with: sudo apt-get install clang-tidy"
        return 1
    fi
    
    if [ ! -f "$COMPILE_COMMANDS" ]; then
        print_warning "compile_commands.json not found. Generating..."
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        cd ..
    fi
    
    echo "Running clang-tidy on C++ source files..."
    
    # Find all .cpp files and run clang-tidy
    find "$SRC_DIR" -name "*.cpp" | while read -r file; do
        echo "Checking: $file"
        clang-tidy "$file" -p="$BUILD_DIR" --config-file=.clang-tidy || true
    done
    
    print_header "clang-tidy completed"
}

# Function to run cppcheck
run_cppcheck() {
    print_header "Running cppcheck"
    
    if ! command_exists cppcheck; then
        print_error "cppcheck not found. Install with: sudo apt-get install cppcheck"
        return 1
    fi
    
    echo "Running cppcheck on source directory..."
    
    cppcheck \
        --enable=warning,style,performance,portability \
        --std=c++20 \
        --language=c++ \
        --suppress=missingIncludeSystem \
        --suppressions-list=.cppcheck-suppressions.txt \
        --inline-suppr \
        --error-exitcode=0 \
        --verbose \
        --template='{file}:{line}: {severity}: {id}: {message}' \
        "$SRC_DIR" 2>&1 | tee cppcheck-report.txt
    
    print_header "cppcheck completed"
    echo "Report saved to: cppcheck-report.txt"
}

# Function to run cpplint
run_cpplint() {
    print_header "Running cpplint"
    
    if ! command_exists cpplint; then
        print_warning "cpplint not found. Install with: pip install cpplint"
        if ! command_exists pip; then
            print_error "pip not found. Cannot install cpplint."
            return 1
        fi
        pip install cpplint
    fi
    
    echo "Running cpplint on C++ source files..."
    
    # Run cpplint with relaxed filters (adjust as needed)
    find "$SRC_DIR" -name "*.cpp" -o -name "*.h" | xargs cpplint \
        --filter=-build/c++11,-readability/todo,-legal/copyright \
        --linelength=120 \
        2>&1 | tee cpplint-report.txt || true
    
    print_header "cpplint completed"
    echo "Report saved to: cpplint-report.txt"
}

# Function to generate summary
generate_summary() {
    print_header "Linting Summary"
    
    echo "Analysis complete!"
    echo ""
    echo "Reports generated:"
    [ -f cppcheck-report.txt ] && echo "  - cppcheck-report.txt"
    [ -f cpplint-report.txt ] && echo "  - cpplint-report.txt"
    echo ""
    echo "Next steps:"
    echo "  1. Review the reports above"
    echo "  2. Fix critical issues (security, bugs)"
    echo "  3. Consider style improvements"
    echo "  4. Update .clang-tidy configuration as needed"
}

# Main script
main() {
    local mode="${1:-all}"
    
    print_header "Modern C++ Static Analysis"
    echo "Mode: $mode"
    
    case "$mode" in
        all)
            run_clang_tidy
            run_cppcheck
            run_cpplint
            generate_summary
            ;;
        clang-tidy)
            run_clang_tidy
            ;;
        cppcheck)
            run_cppcheck
            ;;
        cpplint)
            run_cpplint
            ;;
        *)
            print_error "Unknown mode: $mode"
            echo "Usage: $0 [all|clang-tidy|cppcheck|cpplint]"
            exit 1
            ;;
    esac
}

main "$@"
