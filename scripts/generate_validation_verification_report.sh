#!/bin/bash
# =============================================================================
# Unified Validation and Verification Report Generator
# =============================================================================
#
# This script generates a single comprehensive validation and verification
# report that combines:
#   1. Static code analysis results (clang-tidy + MISRA C++ / cppcheck)
#   2. Google Test based test harness execution results
#   3. Code coverage analysis
#
# The report is generated with a timestamp in the filename and placed
# in the VnV/ directory.
#
# Usage:
#   ./scripts/generate_validation_verification_report.sh [build_dir]
#
# Arguments:
#   build_dir  - Path to the build directory (default: ./build)
#
# Output:
#   VnV/VALIDATION_VERIFICATION_REPORT_<YYYYMMDD_HHMMSS>.md
#   VnV/VALIDATION_VERIFICATION_REPORT_<YYYYMMDD_HHMMSS>.html
#
# =============================================================================

set +e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${1:-$PROJECT_ROOT/build}"
TIMESTAMP="$(date '+%Y%m%d_%H%M%S')"
VNV_DIR="$PROJECT_ROOT/VnV"
REPORT_MD="$VNV_DIR/VALIDATION_VERIFICATION_REPORT_${TIMESTAMP}.md"
REPORT_HTML="$VNV_DIR/VALIDATION_VERIFICATION_REPORT_${TIMESTAMP}.html"

# Tracking variables
BUILD_STATUS=0
TEST_STATUS=0
COVERAGE_STATUS=0
STATIC_ANALYSIS_STATUS=0

log_message() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# =============================================================================
# Setup
# =============================================================================

log_message "=== FreeRTOS C++ Wrappers - Validation and Verification Report Generator ==="
log_message "Project Root: $PROJECT_ROOT"
log_message "Build Directory: $BUILD_DIR"
log_message "Output Directory: $VNV_DIR"
log_message "Timestamp: $TIMESTAMP"

cd "$PROJECT_ROOT"
mkdir -p "$VNV_DIR"
mkdir -p "$BUILD_DIR"

# =============================================================================
# Stage 1: Build
# =============================================================================

log_message "--- Stage 1: Building project with coverage enabled ---"
cd "$BUILD_DIR"
if cmake -DENABLE_COVERAGE=ON -DCLANG_TIDY_WARNINGS_AS_ERRORS=OFF "$PROJECT_ROOT" > cmake_output.txt 2>&1; then
    log_message "CMake configuration successful"
else
    log_message "ERROR: CMake configuration failed"
    BUILD_STATUS=1
fi

if cmake --build "$BUILD_DIR" --parallel > build_output.txt 2>&1; then
    log_message "Build successful"
else
    log_message "ERROR: Build failed"
    BUILD_STATUS=1
fi

# =============================================================================
# Stage 2: Test Execution
# =============================================================================

log_message "--- Stage 2: Running Google Test based test harness ---"
# Clean previous coverage data
find . -name "*.gcda" -delete 2>/dev/null || true

# Run tests and capture verbose output
ctest --verbose --output-on-failure > ctest_output.txt 2>&1
TEST_STATUS=$?

if [ $TEST_STATUS -eq 0 ]; then
    log_message "All tests passed"
else
    log_message "Some tests failed (exit code: $TEST_STATUS)"
fi

# =============================================================================
# Stage 3: Coverage Analysis
# =============================================================================

log_message "--- Stage 3: Generating code coverage data ---"
if command -v lcov > /dev/null 2>&1; then
    if lcov --capture --directory . --output-file coverage.info \
         --ignore-errors mismatch,negative,gcov,source > /dev/null 2>&1; then
        if lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info > /dev/null 2>&1; then
            lcov --summary coverage_filtered.info > coverage_summary.txt 2>&1
            log_message "Coverage data generated successfully"
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
    log_message "WARNING: lcov not available"
    COVERAGE_STATUS=1
fi

# =============================================================================
# Stage 4: Static Code Analysis
# =============================================================================

log_message "--- Stage 4: Running static code analysis tools ---"

# 4a: clang-tidy analysis
CLANG_TIDY_EXE="$(command -v clang-tidy 2>/dev/null)"
CLANG_TIDY_REPORT="$BUILD_DIR/clang-tidy-report.txt"
CLANG_TIDY_CHECKS="cppcoreguidelines-*,cert-*,google-*,hicpp-*"

if [ -n "$CLANG_TIDY_EXE" ]; then
    log_message "Running clang-tidy analysis on all library modules..."
    $CLANG_TIDY_EXE \
        --checks="$CLANG_TIDY_CHECKS" \
        --format-style=file \
        --system-headers=false \
        --header-filter="^${PROJECT_ROOT}/include/.*" \
        "$PROJECT_ROOT"/src/*.cc \
        "$PROJECT_ROOT"/include/freertos.hpp \
        "$PROJECT_ROOT"/include/freertos_event_group.hpp \
        "$PROJECT_ROOT"/include/freertos_message_buffer.hpp \
        "$PROJECT_ROOT"/include/freertos_queue.hpp \
        "$PROJECT_ROOT"/include/freertos_semaphore.hpp \
        "$PROJECT_ROOT"/include/freertos_stream_buffer.hpp \
        "$PROJECT_ROOT"/include/freertos_sw_timer.hpp \
        "$PROJECT_ROOT"/include/freertos_task.hpp \
        -- \
        -I"$PROJECT_ROOT"/include \
        -isystem"$PROJECT_ROOT"/tests/mocks \
        -std=c++17 \
        -DUNIT_TESTING=1 \
        > "$CLANG_TIDY_REPORT" 2>&1 || true
    log_message "clang-tidy analysis complete"
else
    log_message "WARNING: clang-tidy not found"
    STATIC_ANALYSIS_STATUS=1
fi

# 4b: MISRA C++ analysis (cppcheck)
MISRA_REPORT="$BUILD_DIR/misra_analysis_output.txt"
if [ -x "$SCRIPT_DIR/run_misra_analysis.sh" ]; then
    log_message "Running MISRA C++ analysis..."
    if "$SCRIPT_DIR/run_misra_analysis.sh" "$PROJECT_ROOT" "$MISRA_REPORT" > /dev/null 2>&1; then
        log_message "MISRA analysis complete"
    else
        log_message "WARNING: MISRA analysis returned non-zero exit code"
        STATIC_ANALYSIS_STATUS=1
    fi
else
    log_message "WARNING: run_misra_analysis.sh not found"
    STATIC_ANALYSIS_STATUS=1
fi

# 4c: Enhanced cppcheck analysis
ENHANCED_CPPCHECK_REPORT="$BUILD_DIR/enhanced_cppcheck_output.txt"
if [ -x "$SCRIPT_DIR/run_enhanced_cppcheck_analysis.sh" ]; then
    log_message "Running enhanced cppcheck analysis..."
    if "$SCRIPT_DIR/run_enhanced_cppcheck_analysis.sh" "$PROJECT_ROOT" "$ENHANCED_CPPCHECK_REPORT" > /dev/null 2>&1; then
        log_message "Enhanced cppcheck analysis complete"
    else
        log_message "WARNING: Enhanced cppcheck analysis returned non-zero exit code"
        STATIC_ANALYSIS_STATUS=1
    fi
else
    log_message "WARNING: run_enhanced_cppcheck_analysis.sh not found"
    STATIC_ANALYSIS_STATUS=1
fi

log_message "--- Stage 4 complete ---"

# =============================================================================
# Stage 5: Generate Combined Report
# =============================================================================

log_message "--- Stage 5: Generating combined validation and verification report ---"

# 5a: Generate V&V report (test results + coverage) using existing Python script
VV_TEMP="$BUILD_DIR/vv_temp.md"
python3 "$SCRIPT_DIR/generate_validation_verification_report.py" "$BUILD_DIR" "$VV_TEMP" 2>/dev/null || true

# 5b: Generate static analysis sections using existing parsers
SA_TEMP="$BUILD_DIR/sa_temp.md"

# Generate clang-tidy stats
CLANG_TIDY_STATS="$BUILD_DIR/clang-tidy-stats.md"
if [ -f "$CLANG_TIDY_REPORT" ]; then
    python3 "$SCRIPT_DIR/parse_clang_tidy_stats.py" "$CLANG_TIDY_REPORT" "$CLANG_TIDY_STATS" 2>/dev/null || true
fi

# Build static analysis section
{
    echo "## clang-tidy Analysis"
    echo ""
    if [ -f "$CLANG_TIDY_STATS" ]; then
        cat "$CLANG_TIDY_STATS"
    else
        echo "*clang-tidy statistics not available.*"
        echo ""
    fi

    echo "## MISRA C++ Analysis"
    echo ""
    if [ -f "$MISRA_REPORT" ]; then
        python3 "$SCRIPT_DIR/parse_misra_report.py" "$MISRA_REPORT" "$PROJECT_ROOT" 2>/dev/null || echo "*MISRA analysis not available.*"
    else
        echo "*MISRA analysis not available.*"
    fi
    echo ""

    echo "## Enhanced cppcheck Analysis (All Rules)"
    echo ""
    if [ -f "$ENHANCED_CPPCHECK_REPORT" ]; then
        python3 "$SCRIPT_DIR/parse_enhanced_cppcheck_report.py" "$ENHANCED_CPPCHECK_REPORT" "$PROJECT_ROOT" 2>/dev/null || echo "*Enhanced cppcheck analysis not available.*"
    else
        echo "*Enhanced cppcheck analysis not available.*"
    fi
    echo ""

    echo "## Detailed clang-tidy Analysis"
    echo ""
    if [ -f "$CLANG_TIDY_REPORT" ]; then
        python3 "$SCRIPT_DIR/parse_clang_tidy_detailed.py" "$CLANG_TIDY_REPORT" "$PROJECT_ROOT" 2>/dev/null || echo "*Detailed clang-tidy analysis not available.*"
    else
        echo "*Detailed clang-tidy analysis not available.*"
    fi
    echo ""
} > "$SA_TEMP"

# 5c: Combine into final report
{
    # If V&V temp report exists, inject static analysis after the executive summary
    if [ -f "$VV_TEMP" ]; then
        # Extract the executive summary (everything up to "## Complete Test Results")
        # and inject static analysis between the summary and test results
        awk '
        /^## Complete Test Results/ {
            found_test_results = 1
            # Print static analysis section marker
            print "---"
            print ""
            print "## Static Code Analysis"
            print ""
            print "**Static Analysis Tools**: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)"
            print "**Analysis Scope**: Library modules only - src/ include/"
            print "**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-* + All cppcheck rules (style, performance, portability, security, etc.) + MISRA C 2012 (applicable to C++)"
            print ""
            # Signal to insert static analysis content
            print "%%STATIC_ANALYSIS_PLACEHOLDER%%"
            print ""
            print "---"
            print ""
        }
        { print }
        ' "$VV_TEMP" | while IFS= read -r line; do
            if [ "$line" = "%%STATIC_ANALYSIS_PLACEHOLDER%%" ]; then
                cat "$SA_TEMP"
            else
                echo "$line"
            fi
        done
    else
        # No V&V report available, just create a minimal one with static analysis
        echo "# Validation and Verification Report"
        echo ""
        echo "## Executive Summary"
        echo ""
        echo "This report provides comprehensive validation and verification results for the FreeRTOS C++ Wrappers project."
        echo ""
        echo "*Generated: $(date '+%B %d, %Y at %I:%M %p')*"
        echo ""
        echo "---"
        echo ""
        echo "## Static Code Analysis"
        echo ""
        echo "**Static Analysis Tools**: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)"
        echo "**Analysis Scope**: Library modules only - src/ include/"
        echo ""
        cat "$SA_TEMP"
    fi

    # Add report generation footer
    echo ""
    echo "---"
    echo ""
    echo "*Report generated: $(date '+%B %d, %Y at %I:%M %p')*"
    echo "*Tools: Google Test + lcov + clang-tidy + cppcheck (MISRA + all rules)*"
    echo "*Scope: Library modules only (src/, include/)*"
} > "$REPORT_MD"

log_message "Markdown report generated: $REPORT_MD"

# 5d: Generate HTML version
if [ -f "$SCRIPT_DIR/generate_html_report.py" ] && [ -f "$SCRIPT_DIR/report_template.html" ]; then
    python3 "$SCRIPT_DIR/generate_html_report.py" \
        "Validation and Verification Report" \
        "$REPORT_MD" \
        "$REPORT_HTML" \
        "$SCRIPT_DIR/report_template.html" 2>/dev/null

    if [ -f "$REPORT_HTML" ]; then
        log_message "HTML report generated: $REPORT_HTML"
    else
        log_message "WARNING: HTML report generation failed"
    fi
else
    log_message "WARNING: HTML report generator not available"
fi

# Clean up temp files
rm -f "$VV_TEMP" "$SA_TEMP" "$CLANG_TIDY_STATS" 2>/dev/null || true

# =============================================================================
# Summary
# =============================================================================

log_message ""
log_message "=== Report Generation Complete ==="
log_message ""
log_message "Build Status:           $([ $BUILD_STATUS -eq 0 ] && echo '✅ SUCCESS' || echo '❌ FAILED')"
log_message "Test Status:            $([ $TEST_STATUS -eq 0 ] && echo '✅ ALL PASSED' || echo '❌ SOME FAILED')"
log_message "Coverage Status:        $([ $COVERAGE_STATUS -eq 0 ] && echo '✅ SUCCESS' || echo '❌ FAILED')"
log_message "Static Analysis Status: $([ $STATIC_ANALYSIS_STATUS -eq 0 ] && echo '✅ SUCCESS' || echo '❌ FAILED')"
log_message ""
log_message "Reports:"
[ -f "$REPORT_MD" ] && log_message "  ✅ $REPORT_MD"
[ -f "$REPORT_HTML" ] && log_message "  ✅ $REPORT_HTML"
log_message ""
log_message "=== Done ==="

# Exit with non-zero status if any step failed
EXIT_CODE=0
if [ "$BUILD_STATUS" -ne 0 ] || [ "$TEST_STATUS" -ne 0 ] || [ "$COVERAGE_STATUS" -ne 0 ] || [ "$STATIC_ANALYSIS_STATUS" -ne 0 ]; then
    EXIT_CODE=1
fi

exit "$EXIT_CODE"
