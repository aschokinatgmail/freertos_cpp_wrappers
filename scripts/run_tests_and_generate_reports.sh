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
DOCS_STATUS=0

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

# Generate combined validation and verification report
# (handles build, test execution, coverage, static analysis, and report generation)
log_message "Generating combined validation and verification report..."
"$SCRIPT_DIR/generate_validation_verification_report.sh" "$BUILD_DIR"
REPORT_STATUS=$?

if [ $REPORT_STATUS -eq 0 ]; then
    log_message "Combined validation and verification report generated successfully"
else
    log_message "WARNING: Combined validation and verification report generation encountered failures"
fi

# Generate documentation (optional, does not affect the V&V report)
cd "$BUILD_DIR"
log_message "Generating project documentation..."
if command_exists doxygen && cmake --build "$BUILD_DIR" --target docs 2>/dev/null; then
    log_message "Documentation generated successfully"
    DOCS_STATUS=0
else
    log_message "WARNING: Documentation generation failed or doxygen not available"
    DOCS_STATUS=1
fi

log_message ""
log_message "=== Report Generation Complete ==="

# Report generation status
log_message "Generated reports:"
VNV_DIR="$PROJECT_ROOT/VnV"
if ls "$VNV_DIR"/VALIDATION_VERIFICATION_REPORT_*.md 1>/dev/null 2>&1; then
    LATEST_REPORT=$(ls -t "$VNV_DIR"/VALIDATION_VERIFICATION_REPORT_*.md | head -1)
    log_message "  ✅ $(basename "$LATEST_REPORT") - Combined V&V report (static analysis + test results)"
else
    log_message "  ❌ Combined V&V report - Failed to generate"
fi

if [ -d "$PROJECT_ROOT/docs/html" ]; then
    log_message "  ✅ docs/html/index.html - Project documentation"
else
    log_message "  ❌ docs/html/index.html - Failed to generate"
fi

log_message ""
log_message "Reports are located in: $VNV_DIR"
log_message "To view reports:"
if ls "$VNV_DIR"/VALIDATION_VERIFICATION_REPORT_*.html 1>/dev/null 2>&1; then
    LATEST_HTML=$(ls -t "$VNV_DIR"/VALIDATION_VERIFICATION_REPORT_*.html | head -1)
    log_message "  Combined V&V Report: open $LATEST_HTML"
fi
log_message "  Documentation: open $PROJECT_ROOT/docs/html/index.html"

log_message ""
log_message "=== Report Generation Script Completed ==="

# Propagate exit status from the unified report script
exit $REPORT_STATUS