#!/bin/bash
#
# Test execution and report generation script
# This script runs all tests and generates the two required reports:
# 1. STATIC_ANALYSIS_REPORT.md
# 2. VALIDATION_VERIFICATION_REPORT.md
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

echo "=== FreeRTOS C++ Wrappers - Test Execution and Report Generation ==="
echo "Project Root: $PROJECT_ROOT"
echo "Build Directory: $BUILD_DIR"
echo

# Ensure we're in the right directory
cd "$PROJECT_ROOT"

# Check if build directory exists, create if needed
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Configure with coverage
echo "Configuring project with coverage enabled..."
cd "$BUILD_DIR"
cmake -DENABLE_COVERAGE=ON ..

# Build the project
echo "Building project..."
make -j$(nproc)

# Run tests
echo "Running all tests..."
ctest --verbose

# Generate coverage data
echo "Generating coverage data..."
lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch,negative,gcov
lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info

echo "Coverage summary:"
lcov --summary coverage_filtered.info

# Generate static analysis report
echo "Generating static analysis report..."
make static-analysis-report

# Generate documentation after successful tests
echo "Generating project documentation..."
make docs

# Generate validation and verification report
echo "Generating validation and verification report..."
python3 "$SCRIPT_DIR/generate_validation_verification_report.py" "$BUILD_DIR" "$PROJECT_ROOT/VALIDATION_VERIFICATION_REPORT.md"

echo
echo "=== Report Generation Complete ==="
echo "Generated reports:"
echo "  1. STATIC_ANALYSIS_REPORT.md - Static code analysis for main modules"
echo "  2. VALIDATION_VERIFICATION_REPORT.md - Test execution and coverage analysis"
echo "  3. docs/html/index.html - Updated project documentation"
echo
echo "Test summary:"
ctest | grep -E "(tests passed|Total Test time)" || true

echo
echo "Reports are located in: $PROJECT_ROOT"
echo "To view reports:"
echo "  Static Analysis: cat $PROJECT_ROOT/STATIC_ANALYSIS_REPORT.md"
echo "  Validation & Verification: cat $PROJECT_ROOT/VALIDATION_VERIFICATION_REPORT.md"
echo "  Documentation: open $PROJECT_ROOT/docs/html/index.html"