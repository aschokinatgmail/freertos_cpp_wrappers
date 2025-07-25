#!/bin/bash
#
# Test execution and report generation script
# This script runs all tests and generates the two required reports:
# 1. STATIC_ANALYSIS_REPORT.md
# 2. VALIDATION_VERIFICATION_REPORT.md
#
# Enhanced version: Continues report generation even if tests fail
#

# Don't exit on error - we want to generate reports even if tests fail
set +e

# Function to log messages with timestamps
log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install missing dependencies
install_dependencies() {
    log_message "Checking and installing required dependencies..."
    
    local missing_deps=()
    local missing_python_deps=()
    
    # Check for required tools
    if ! command_exists lcov; then
        missing_deps+=("lcov")
    fi
    
    if ! command_exists doxygen; then
        missing_deps+=("doxygen")
    fi
    
    if ! command_exists cppcheck; then
        missing_deps+=("cppcheck")
    fi
    
    if ! command_exists clang-format; then
        missing_deps+=("clang-format")
    fi
    
    if ! command_exists clang-tidy; then
        missing_deps+=("clang-tidy")
    fi
    
    # Check if GoogleTest/GoogleMock are available
    if ! pkg-config --exists gtest_main 2>/dev/null; then
        missing_deps+=("libgtest-dev")
    fi
    
    if ! pkg-config --exists gmock_main 2>/dev/null; then
        missing_deps+=("libgmock-dev")
    fi
    
    # Check for pip3
    if ! command_exists pip3; then
        missing_deps+=("python3-pip")
    fi
    
    # Install missing system dependencies
    if [ ${#missing_deps[@]} -gt 0 ]; then
        log_message "Installing missing system dependencies: ${missing_deps[*]}"
        sudo apt update
        sudo apt install -y "${missing_deps[@]}"
        
        # Verify installation
        for dep in "${missing_deps[@]}"; do
            case "$dep" in
                "libgtest-dev"|"libgmock-dev")
                    # These don't have command line tools, check via pkg-config later
                    ;;
                "python3-pip")
                    if ! command_exists pip3; then
                        log_message "ERROR: Failed to install pip3"
                        return 1
                    else
                        log_message "SUCCESS: pip3 installed successfully"
                    fi
                    ;;
                *)
                    if ! command_exists "$dep"; then
                        log_message "ERROR: Failed to install $dep"
                        return 1
                    else
                        log_message "SUCCESS: $dep installed successfully"
                    fi
                    ;;
            esac
        done
    else
        log_message "All required system dependencies are already installed"
    fi
    
    # Check for Python dependencies
    if ! python3 -c "import markdown" 2>/dev/null; then
        missing_python_deps+=("markdown")
    fi
    
    # Install missing Python dependencies
    if [ ${#missing_python_deps[@]} -gt 0 ]; then
        log_message "Installing missing Python dependencies: ${missing_python_deps[*]}"
        pip3 install "${missing_python_deps[@]}"
        
        # Verify Python package installation
        for dep in "${missing_python_deps[@]}"; do
            if ! python3 -c "import $dep" 2>/dev/null; then
                log_message "ERROR: Failed to install Python package $dep"
                return 1
            else
                log_message "SUCCESS: Python package $dep installed successfully"
            fi
        done
    else
        log_message "All required Python dependencies are already installed"
    fi
    
    return 0
}

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

# Variables to track execution status
TEST_STATUS=0
BUILD_STATUS=0
COVERAGE_STATUS=0
DOCS_STATUS=0
STATIC_ANALYSIS_STATUS=0

log_message "=== FreeRTOS C++ Wrappers - Test Execution and Report Generation ==="
log_message "Project Root: $PROJECT_ROOT"
log_message "Build Directory: $BUILD_DIR"

# Ensure we're in the right directory
cd "$PROJECT_ROOT"

# Install required dependencies
if ! install_dependencies; then
    log_message "WARNING: Some dependencies may not be available, but continuing with report generation"
fi

# Format code according to coding standard
log_message "Formatting code according to coding standard..."
if [ -f ".clang-format" ] && command_exists clang-format; then
    find include src -name "*.hpp" -o -name "*.cc" -o -name "*.cpp" -o -name "*.h" | xargs clang-format -i
    log_message "Code formatting completed."
else
    log_message "Warning: .clang-format file not found or clang-format not available. Skipping code formatting."
fi

# Check if build directory exists, create if needed
if [ ! -d "$BUILD_DIR" ]; then
    log_message "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Configure with coverage
log_message "Configuring project with coverage enabled..."
cd "$BUILD_DIR"
if cmake -DENABLE_COVERAGE=ON ..; then
    log_message "CMake configuration successful"
else
    log_message "ERROR: CMake configuration failed"
    BUILD_STATUS=1
fi

# Build the project
log_message "Building project..."
if make -j$(nproc); then
    log_message "Build successful"
else
    log_message "ERROR: Build failed"
    BUILD_STATUS=1
fi

# Run tests (continue even if some tests fail)
log_message "Running all tests..."
if ctest --verbose; then
    log_message "All tests passed successfully"
    TEST_STATUS=0
else
    TEST_STATUS=$?
    log_message "Some tests failed (exit code: $TEST_STATUS), but continuing with report generation"
fi

# Generate coverage data (even if tests failed)
log_message "Generating coverage data..."
if command_exists lcov; then
    if lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch,negative,gcov,source; then
        if lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info; then
            log_message "Coverage data generated successfully"
            lcov --summary coverage_filtered.info
            COVERAGE_STATUS=0
        else
            log_message "WARNING: Coverage filtering failed"
            COVERAGE_STATUS=1
        fi
    else
        log_message "WARNING: Coverage capture failed"
        COVERAGE_STATUS=1
    fi
else
    log_message "WARNING: lcov not available, skipping coverage generation"
    COVERAGE_STATUS=1
fi

# Generate static analysis report (even if previous steps failed)
log_message "Generating static analysis report..."
if make static-analysis-report; then
    log_message "Static analysis report generated successfully"
    STATIC_ANALYSIS_STATUS=0
else
    log_message "WARNING: Static analysis report generation failed"
    STATIC_ANALYSIS_STATUS=1
fi

# Generate documentation after tests (even if tests failed)
log_message "Generating project documentation..."
if command_exists doxygen && make docs; then
    log_message "Documentation generated successfully"
    DOCS_STATUS=0
else
    log_message "WARNING: Documentation generation failed or doxygen not available"
    DOCS_STATUS=1
fi

# Generate validation and verification report (always generate, even with failures)
log_message "Generating validation and verification report..."
if python3 "$SCRIPT_DIR/generate_validation_verification_report.py" "$BUILD_DIR" "$PROJECT_ROOT/VALIDATION_VERIFICATION_REPORT.md"; then
    log_message "Validation and verification report generated successfully"
else
    log_message "WARNING: Validation and verification report generation failed, but continuing"
fi

# Generate HTML version of validation and verification report
log_message "Generating validation and verification HTML report..."
if python3 "$SCRIPT_DIR/generate_html_report.py" "Validation and Verification Report" "$PROJECT_ROOT/VALIDATION_VERIFICATION_REPORT.md" "$PROJECT_ROOT/VALIDATION_VERIFICATION_REPORT.html" "$SCRIPT_DIR/report_template.html"; then
    log_message "HTML validation and verification report generated successfully"
else
    log_message "WARNING: HTML validation and verification report generation failed"
fi

log_message ""
log_message "=== Report Generation Complete ==="

# Report generation status
log_message "Generated reports:"
if [ -f "$PROJECT_ROOT/STATIC_ANALYSIS_REPORT.md" ]; then
    log_message "  ✅ STATIC_ANALYSIS_REPORT.md/.html - Static code analysis"
else
    log_message "  ❌ STATIC_ANALYSIS_REPORT.md/.html - Failed to generate"
fi

if [ -f "$PROJECT_ROOT/VALIDATION_VERIFICATION_REPORT.md" ]; then
    log_message "  ✅ VALIDATION_VERIFICATION_REPORT.md/.html - Test execution and coverage analysis"
else
    log_message "  ❌ VALIDATION_VERIFICATION_REPORT.md/.html - Failed to generate"
fi

if [ -d "$PROJECT_ROOT/docs/html" ]; then
    log_message "  ✅ docs/html/index.html - Project documentation"
else
    log_message "  ❌ docs/html/index.html - Failed to generate"
fi

log_message ""
log_message "=== Execution Summary ==="
log_message "Build Status: $([ $BUILD_STATUS -eq 0 ] && echo "✅ SUCCESS" || echo "❌ FAILED")"
log_message "Test Status: $([ $TEST_STATUS -eq 0 ] && echo "✅ ALL PASSED" || echo "❌ SOME FAILED")"
log_message "Coverage Status: $([ $COVERAGE_STATUS -eq 0 ] && echo "✅ SUCCESS" || echo "❌ FAILED")"
log_message "Documentation Status: $([ $DOCS_STATUS -eq 0 ] && echo "✅ SUCCESS" || echo "❌ FAILED")"
log_message "Static Analysis Status: $([ $STATIC_ANALYSIS_STATUS -eq 0 ] && echo "✅ SUCCESS" || echo "❌ FAILED")"

# Show test summary regardless of failures
log_message ""
log_message "Test summary:"
ctest | grep -E "(tests passed|Total Test time|% tests failed)" || true

log_message ""
log_message "Reports are located in: $PROJECT_ROOT"
log_message "To view reports:"
log_message "  Static Analysis: open $PROJECT_ROOT/STATIC_ANALYSIS_REPORT.html"
log_message "  Validation & Verification: open $PROJECT_ROOT/VALIDATION_VERIFICATION_REPORT.html"
log_message "  Documentation: open $PROJECT_ROOT/docs/html/index.html"

# Final status - don't exit with error even if some components failed
# The important thing is that reports are generated
if [ $BUILD_STATUS -ne 0 ]; then
    log_message ""
    log_message "WARNING: Build failed, but reports were generated with available data"
fi

if [ $TEST_STATUS -ne 0 ]; then
    log_message ""
    log_message "WARNING: Some tests failed, but reports include failure details"
fi

log_message ""
log_message "=== Report Generation Script Completed ==="

# Exit successfully since reports were generated (even if tests failed)
exit 0