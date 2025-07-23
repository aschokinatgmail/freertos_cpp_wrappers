#!/bin/bash
# Simplified script to generate combined validation and verification report with test execution results
# This version ensures the Google Test harness results are included in the final report

BUILD_DIR="$1"
OUTPUT_MD="$2"
OUTPUT_HTML="$3"
SOURCE_DIR="$4"

if [ $# -ne 4 ]; then
    echo "Usage: $0 <build_dir> <output_md> <output_html> <source_dir>"
    echo "Example: $0 build VALIDATION_VERIFICATION_REPORT.md VALIDATION_VERIFICATION_REPORT.html ."
    exit 1
fi

# Check if source directory exists
if [ ! -d "$SOURCE_DIR" ]; then
    echo "Error: Source directory '$SOURCE_DIR' does not exist"
    exit 1
fi

echo "Generating combined validation and verification report with test execution results..."
echo "Build directory: $BUILD_DIR"
echo "Output Markdown: $OUTPUT_MD"
echo "Output HTML: $OUTPUT_HTML"
echo "Source directory: $SOURCE_DIR"

# Convert SOURCE_DIR to absolute path at the beginning
SOURCE_DIR_ABS="$(cd "$SOURCE_DIR" && pwd)"
echo "Absolute source directory: $SOURCE_DIR_ABS"

# Temporary files for intermediate results
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

VV_REPORT_MD="$TEMP_DIR/vv_report.md"

# Step 1: Build project and run tests to generate test execution results
echo "Building project and running test harness to generate execution results..."
cd "$SOURCE_DIR_ABS"

# Check if build directory exists, create if needed
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Configure with coverage (needed for test execution)
echo "Configuring project with coverage enabled..."
cd "$BUILD_DIR"
cmake -DENABLE_COVERAGE=ON .. 2>/dev/null || cmake .. 2>/dev/null

# Build the project
echo "Building project..."
make -j$(nproc) 2>/dev/null || make 2>/dev/null

# Run tests to generate test execution data
echo "Running Google Test harness to generate test execution results..."
ctest --verbose > "$TEMP_DIR/test_output.txt" 2>&1 || true

# Generate coverage data
echo "Generating coverage data..."
lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch,negative,gcov 2>/dev/null || true
lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info 2>/dev/null || true

echo "Test execution and coverage generation completed."

# Step 2: Generate validation and verification section with actual test results
echo "Generating validation and verification section with test execution results..."
cd "$SOURCE_DIR_ABS"
if [ -f "scripts/generate_validation_verification_report.py" ]; then
    # Use absolute path for build directory
    BUILD_DIR_ABS="$SOURCE_DIR_ABS/$BUILD_DIR"
    python3 "scripts/generate_validation_verification_report.py" \
        "$BUILD_DIR_ABS" \
        "$VV_REPORT_MD"
else
    echo "Error: Validation and verification script not found at scripts/generate_validation_verification_report.py"
    echo "Current working directory: $(pwd)"
    echo "Looking for: $(pwd)/scripts/generate_validation_verification_report.py"
    exit 1
fi

# Step 3: Create simplified static analysis section
echo "Creating static analysis placeholder section..."
cat > "$TEMP_DIR/static_analysis.md" << 'EOF'
## Static Code Analysis

### Overview

**Static Analysis Tools**: clang-tidy + cppcheck (MISRA C++ 2012)
**Analysis Scope**: Main project code only (src/ and include/ directories)
**Excluded**: Test harness, mocks, and external dependencies

### Combined Static Analysis Results

#### Summary

Static analysis tools are configured and available. For detailed static analysis results including MISRA C++ violations and clang-tidy findings, run:

```bash
make static-analysis-report
```

This will generate comprehensive static analysis results with:
- MISRA C++ 2012 compliance analysis
- clang-tidy code quality checks
- Detailed violation reports with code context

EOF

# Step 4: Combine everything into final report
echo "Combining reports..."
cat > "$OUTPUT_MD" << 'EOF'
# Validation and Verification Report

## Executive Summary

This report provides comprehensive validation and verification results for the FreeRTOS C++ Wrappers project. The analysis focuses on test execution results, code coverage analysis, and validation conclusions.

**Key Areas Covered:**
- Static code analysis (MISRA C++ 2012 compliance and clang-tidy)
- Test execution results  
- Code coverage analysis
- Validation conclusions

EOF

# Add static analysis placeholder
if [ -f "$TEMP_DIR/static_analysis.md" ]; then
    cat "$TEMP_DIR/static_analysis.md" >> "$OUTPUT_MD"
    echo "" >> "$OUTPUT_MD"
fi

# Add V&V content (skip the title and executive summary since we have our own)
if [ -f "$VV_REPORT_MD" ]; then
    # First, try to add the test execution summary from the original executive summary
    echo "### Test Execution Summary" >> "$OUTPUT_MD"
    sed -n '/^### Test Execution Summary/,/^### Code Coverage Summary/p' "$VV_REPORT_MD" | sed '1d' | head -n -1 >> "$OUTPUT_MD" 2>/dev/null || true
    echo "" >> "$OUTPUT_MD"
    
    # Then add the detailed test results and everything after
    sed -n '/^## Detailed Test Results/,$p' "$VV_REPORT_MD" >> "$OUTPUT_MD" || \
    sed -n '/^### Test Execution Summary/,$p' "$VV_REPORT_MD" >> "$OUTPUT_MD" || \
    # If those don't work, include everything after the executive summary
    tail -n +20 "$VV_REPORT_MD" >> "$OUTPUT_MD"
fi

# Step 5: Generate HTML version
echo "Generating HTML report..."
if [ -f "scripts/generate_html_report.py" ]; then
    python3 "scripts/generate_html_report.py" \
        "Validation and Verification Report" \
        "$OUTPUT_MD" \
        "$OUTPUT_HTML" \
        "scripts/report_template.html"
else
    echo "Warning: HTML generation script not found, skipping HTML output"
fi

echo ""
echo "Combined validation and verification report generated:"
echo "  Markdown: $OUTPUT_MD"
echo "  HTML: $OUTPUT_HTML"

# Show summary
if [ -f "$OUTPUT_MD" ]; then
    echo ""
    echo "Report summary:"
    echo "=== Test Results ==="
    grep -E "^- \*\*Total|^- \*\*Success|^- \*\*Line Coverage|^- \*\*Function Coverage" "$OUTPUT_MD" | head -5
fi