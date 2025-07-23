#!/bin/bash
# Script to generate combined validation and verification report with static analysis details

BUILD_DIR="$1"
OUTPUT_MD="$2"
OUTPUT_HTML="$3"
SOURCE_DIR="$4"

if [ $# -ne 4 ]; then
    echo "Usage: $0 <build_dir> <output_md> <output_html> <source_dir>"
    echo "Example: $0 build VALIDATION_VERIFICATION_REPORT.md VALIDATION_VERIFICATION_REPORT.html ."
    exit 1
fi

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory '$BUILD_DIR' does not exist"
    exit 1
fi

echo "Generating combined validation and verification report..."
echo "Build directory: $BUILD_DIR"
echo "Output Markdown: $OUTPUT_MD"
echo "Output HTML: $OUTPUT_HTML"
echo "Source directory: $SOURCE_DIR"

# Temporary files for intermediate results
TEMP_DIR=$(mktemp -d)
STATIC_ANALYSIS_MD="$TEMP_DIR/static_analysis.md"
VV_REPORT_MD="$TEMP_DIR/vv_report.md"
CLANG_TIDY_REPORT="$BUILD_DIR/clang_tidy_report.txt"
CLANG_TIDY_STATS="$TEMP_DIR/clang_tidy_stats.md"
MISRA_REPORT="$BUILD_DIR/misra_report.txt"
ENHANCED_CPPCHECK_REPORT="$BUILD_DIR/enhanced_cppcheck_report.txt"

# Step 1: Run static analysis tools to get fresh results
echo "Running static analysis tools to ensure fresh results..."

# Run MISRA analysis
if [ -f "$SOURCE_DIR/scripts/run_misra_analysis.sh" ]; then
    echo "Running MISRA C++ analysis..."
    bash "$SOURCE_DIR/scripts/run_misra_analysis.sh" "$SOURCE_DIR" "$MISRA_REPORT"
else
    echo "Warning: MISRA analysis script not found"
fi

# Run clang-tidy analysis (if not already available)
if [ ! -f "$CLANG_TIDY_REPORT" ] && [ -f "$SOURCE_DIR/compile_commands.json" ]; then
    echo "Running clang-tidy analysis..."
    cd "$SOURCE_DIR"
    clang-tidy \
        --config-file=.clang-tidy \
        --header-filter=".*include.*" \
        $(find src include -name "*.hpp" -o -name "*.cc" -o -name "*.cpp") \
        > "$CLANG_TIDY_REPORT" 2>&1 || true
    cd - > /dev/null
fi

# Step 2: Generate static analysis report if raw data exists
if [ -f "$CLANG_TIDY_REPORT" ] || [ -f "$MISRA_REPORT" ]; then
    echo "Generating static analysis section..."
    
    # Generate the static analysis report (without detailed clang-tidy output to save space)
    if [ -f "$SOURCE_DIR/scripts/generate_static_analysis_report.sh" ]; then
        bash "$SOURCE_DIR/scripts/generate_static_analysis_report.sh" \
            "$CLANG_TIDY_REPORT" \
            "$CLANG_TIDY_STATS" \
            "$STATIC_ANALYSIS_MD" \
            "$SOURCE_DIR" \
            "$MISRA_REPORT" \
            "$ENHANCED_CPPCHECK_REPORT"
    else
        echo "Warning: Static analysis script not found, creating minimal report"
        cat > "$STATIC_ANALYSIS_MD" << EOF
# Static Analysis Report

## Overview

**Static Analysis Tools**: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)
**Analysis Scope**: Library modules only - src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-* + All cppcheck rules

## Analysis Results

*Static analysis data not available in build directory*
EOF
    fi
else
    echo "Warning: No static analysis raw data found in build directory"
    echo "Expected files: $CLANG_TIDY_REPORT or $MISRA_REPORT"
    cat > "$STATIC_ANALYSIS_MD" << EOF
# Static Analysis Report

## Overview

**Static Analysis Tools**: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)
**Analysis Scope**: Library modules only - src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-* + All cppcheck rules

## Analysis Results

*Static analysis reports not available - run static analysis first*
EOF
fi

# Step 3: Generate validation and verification report
echo "Generating validation and verification section..."
if [ -f "$SOURCE_DIR/scripts/generate_validation_verification_report.py" ]; then
    python3 "$SOURCE_DIR/scripts/generate_validation_verification_report.py" \
        "$BUILD_DIR" \
        "$VV_REPORT_MD"
else
    echo "Error: Validation and verification script not found"
    exit 1
fi

# Step 4: Combine reports
echo "Combining reports..."
cat > "$OUTPUT_MD" << 'EOF'
# Validation and Verification Report

## Executive Summary

This report provides comprehensive validation and verification results for the FreeRTOS C++ Wrappers project, including static code analysis, detailed test execution outcomes, and code coverage analysis.

EOF

# Extract static analysis content and add it to combined report
if [ -f "$STATIC_ANALYSIS_MD" ]; then
    echo "## Static Code Analysis" >> "$OUTPUT_MD"
    echo "" >> "$OUTPUT_MD"
    
    # Extract content after the first heading
    sed -n '/^## Overview/,$p' "$STATIC_ANALYSIS_MD" >> "$OUTPUT_MD"
    echo "" >> "$OUTPUT_MD"
fi

# Extract V&V content (skip the title and executive summary since we have our own)
if [ -f "$VV_REPORT_MD" ]; then
    # Extract everything after the executive summary
    sed -n '/^## Test Execution Results/,$p' "$VV_REPORT_MD" >> "$OUTPUT_MD" || \
    sed -n '/^## Detailed Test Results/,$p' "$VV_REPORT_MD" >> "$OUTPUT_MD" || \
    sed -n '/^### Test Execution Summary/,$p' "$VV_REPORT_MD" >> "$OUTPUT_MD"
fi

# Step 5: Generate HTML version
echo "Generating HTML report..."
if [ -f "$SOURCE_DIR/scripts/generate_html_report.py" ]; then
    python3 "$SOURCE_DIR/scripts/generate_html_report.py" \
        "Validation and Verification Report" \
        "$OUTPUT_MD" \
        "$OUTPUT_HTML" \
        "$SOURCE_DIR/scripts/report_template.html"
else
    echo "Warning: HTML generation script not found, skipping HTML output"
fi

# Cleanup
rm -rf "$TEMP_DIR"

echo "Combined validation and verification report generated:"
echo "  Markdown: $OUTPUT_MD"
echo "  HTML: $OUTPUT_HTML"

# Show summary
if [ -f "$OUTPUT_MD" ]; then
    echo ""
    echo "Report summary:"
    grep -E "^- \*\*Total|^- \*\*Line Coverage|^- \*\*Function Coverage|Success Rate" "$OUTPUT_MD" | head -10
fi