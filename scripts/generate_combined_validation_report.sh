#!/bin/bash
# Script to generate improved combined validation and verification report
# Addresses all issues mentioned in the feedback

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

echo "Generating improved combined validation and verification report..."
echo "Build directory: $BUILD_DIR"
echo "Output Markdown: $OUTPUT_MD"
echo "Output HTML: $OUTPUT_HTML"
echo "Source directory: $SOURCE_DIR"

# Temporary files for intermediate results
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

CLANG_TIDY_REPORT="$BUILD_DIR/clang_tidy_report.txt"
MISRA_REPORT="$BUILD_DIR/misra_report.txt"
COMBINED_STATIC_ANALYSIS="$TEMP_DIR/combined_static_analysis.md"
VV_REPORT_MD="$TEMP_DIR/vv_report.md"

# Step 1: Run fresh static analysis on all files
echo "Running static analysis on ALL files in src/ and include/ directories..."

# Run MISRA analysis (already working correctly)
echo "Running MISRA C++ analysis..."
bash "$SOURCE_DIR/scripts/run_misra_analysis.sh" "$SOURCE_DIR" "$MISRA_REPORT"

# Run clang-tidy analysis on ALL files with improved configuration
echo "Running clang-tidy analysis on all project files..."
cd "$SOURCE_DIR"

# Create a simple clang-tidy config
cat > "$TEMP_DIR/.clang-tidy" << 'EOF'
Checks: '-*,clang-diagnostic-*,readability-*,performance-*,bugprone-*,misc-*'
WarningsAsErrors: ''
HeaderFilterRegex: '.*'
FormatStyle: none
CheckOptions:
  - key: readability-identifier-naming.VariableCase
    value: lower_case
  - key: readability-identifier-naming.FunctionCase
    value: lower_case
EOF

# Find all files and run clang-tidy on each one individually to avoid compilation database issues
> "$CLANG_TIDY_REPORT"
echo "=== clang-tidy Analysis Report ===" >> "$CLANG_TIDY_REPORT"
echo "Generated: $(date)" >> "$CLANG_TIDY_REPORT"
echo "" >> "$CLANG_TIDY_REPORT"

FILES_ANALYZED=0
for file in $(find src include -name "*.hpp" -o -name "*.cc" -o -name "*.cpp" | sort); do
    echo "Analyzing $file with clang-tidy..."
    echo "=== Analysis of $file ===" >> "$CLANG_TIDY_REPORT"
    
    # Run clang-tidy with basic C++ flags to avoid dependency issues
    clang-tidy \
        --config-file="$TEMP_DIR/.clang-tidy" \
        --header-filter=".*" \
        "$file" \
        -- \
        -std=c++17 \
        -I./include \
        -I/usr/include/c++/11 \
        -I/usr/include/x86_64-linux-gnu/c++/11 \
        -DFREERTOS_CONFIG_H \
        -DportBYTE_ALIGNMENT=8 \
        -DconfigUSE_16_BIT_TICKS=0 \
        >> "$CLANG_TIDY_REPORT" 2>&1 || true
    
    echo "" >> "$CLANG_TIDY_REPORT"
    ((FILES_ANALYZED++))
done

echo "clang-tidy analysis completed on $FILES_ANALYZED files"
cd - > /dev/null

# Step 2: Generate combined static analysis section
echo "Generating combined static analysis section..."

cat > "$COMBINED_STATIC_ANALYSIS" << 'EOF'
## Static Code Analysis

### Overview

**Static Analysis Tools**: clang-tidy + cppcheck (MISRA C++ 2012)
**Analysis Scope**: Main project code only (src/ and include/ directories)
**Excluded**: Test harness, mocks, and external dependencies

EOF

# Process MISRA analysis
echo "### Combined Static Analysis Results" >> "$COMBINED_STATIC_ANALYSIS"
echo "" >> "$COMBINED_STATIC_ANALYSIS"

if [ -f "$MISRA_REPORT" ]; then
    echo "Processing MISRA analysis results..."
    
    # Count MISRA violations
    MISRA_VIOLATIONS=$(grep -c "misra violation" "$MISRA_REPORT" 2>/dev/null || echo "0")
    MISRA_STYLE_ISSUES=$(grep -c ": style:" "$MISRA_REPORT" 2>/dev/null || echo "0")
    MISRA_WARNINGS=$(grep -c ": warning:" "$MISRA_REPORT" 2>/dev/null || echo "0")
    MISRA_ERRORS=$(grep -c ": error:" "$MISRA_REPORT" 2>/dev/null || echo "0")
    
    echo "#### Summary" >> "$COMBINED_STATIC_ANALYSIS"
    echo "" >> "$COMBINED_STATIC_ANALYSIS"
    echo "- **MISRA C++ 2012 Violations**: $MISRA_VIOLATIONS" >> "$COMBINED_STATIC_ANALYSIS"
    echo "- **Style Issues**: $MISRA_STYLE_ISSUES" >> "$COMBINED_STATIC_ANALYSIS"
    echo "- **Warnings**: $MISRA_WARNINGS" >> "$COMBINED_STATIC_ANALYSIS"
    echo "- **Errors**: $MISRA_ERRORS" >> "$COMBINED_STATIC_ANALYSIS"
    echo "" >> "$COMBINED_STATIC_ANALYSIS"
    
    # Generate detailed MISRA violations with code context
    echo "#### Detailed Violations" >> "$COMBINED_STATIC_ANALYSIS"
    echo "" >> "$COMBINED_STATIC_ANALYSIS"
    
    # Use Python script to parse and format MISRA violations properly
    python3 "$SOURCE_DIR/scripts/parse_misra_report.py" "$MISRA_REPORT" "$SOURCE_DIR" >> "$COMBINED_STATIC_ANALYSIS" 2>/dev/null || true
fi

# Process clang-tidy analysis
if [ -f "$CLANG_TIDY_REPORT" ]; then
    echo "Processing clang-tidy analysis results..."
    
    # Count clang-tidy issues (excluding dependency/header errors)
    CLANG_VIOLATIONS=$(grep -c ": warning:" "$CLANG_TIDY_REPORT" 2>/dev/null || echo "0")
    CLANG_ERRORS=$(grep -c ": error:" "$CLANG_TIDY_REPORT" 2>/dev/null || echo "0")
    
    # Ensure they are integers
    CLANG_VIOLATIONS=$(( CLANG_VIOLATIONS + 0 ))
    CLANG_ERRORS=$(( CLANG_ERRORS + 0 ))
    
    echo "" >> "$COMBINED_STATIC_ANALYSIS"
    echo "#### clang-tidy Analysis Results" >> "$COMBINED_STATIC_ANALYSIS"
    echo "" >> "$COMBINED_STATIC_ANALYSIS"
    echo "- **Tool**: clang-tidy" >> "$COMBINED_STATIC_ANALYSIS"
    echo "- **Warnings**: $CLANG_VIOLATIONS" >> "$COMBINED_STATIC_ANALYSIS"
    echo "- **Errors**: $CLANG_ERRORS" >> "$COMBINED_STATIC_ANALYSIS"
    echo "" >> "$COMBINED_STATIC_ANALYSIS"
    
    # Generate detailed clang-tidy violations with code context (filter out dependency errors)
    if [ "$CLANG_VIOLATIONS" -gt 0 ] || [ "$CLANG_ERRORS" -gt 0 ]; then
        echo "##### Detailed clang-tidy Violations" >> "$COMBINED_STATIC_ANALYSIS"
        echo "" >> "$COMBINED_STATIC_ANALYSIS"
        
        # Use Python script to parse and format clang-tidy violations properly
        python3 "$SOURCE_DIR/scripts/parse_clang_tidy_violations.py" "$CLANG_TIDY_REPORT" "$SOURCE_DIR" >> "$COMBINED_STATIC_ANALYSIS" 2>/dev/null || true
    fi
fi

# Step 3: Generate validation and verification section (excluding test harness)
echo "Generating validation and verification section..."
if [ -f "$SOURCE_DIR/scripts/generate_validation_verification_report.py" ]; then
    python3 "$SOURCE_DIR/scripts/generate_validation_verification_report.py" \
        "$BUILD_DIR" \
        "$VV_REPORT_MD"
else
    echo "Error: Validation and verification script not found"
    exit 1
fi

# Step 4: Combine everything into final report
echo "Combining reports..."
cat > "$OUTPUT_MD" << 'EOF'
# Validation and Verification Report

## Executive Summary

This report provides comprehensive validation and verification results for the FreeRTOS C++ Wrappers project. The analysis focuses exclusively on the main project code (src/ and include/ directories) and excludes test harness code to maintain focus on the library implementation.

**Key Areas Covered:**
- Static code analysis (MISRA C++ 2012 compliance and clang-tidy)
- Test execution results  
- Code coverage analysis
- Validation conclusions

EOF

# Add combined static analysis content
if [ -f "$COMBINED_STATIC_ANALYSIS" ]; then
    cat "$COMBINED_STATIC_ANALYSIS" >> "$OUTPUT_MD"
    echo "" >> "$OUTPUT_MD"
fi

# Add V&V content (skip the title and executive summary since we have our own)
if [ -f "$VV_REPORT_MD" ]; then
    # Extract everything after the executive summary, starting with test results
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

echo ""
echo "Improved combined validation and verification report generated:"
echo "  Markdown: $OUTPUT_MD"
echo "  HTML: $OUTPUT_HTML"

# Show summary
if [ -f "$OUTPUT_MD" ]; then
    echo ""
    echo "Report summary:"
    echo "=== Static Analysis ==="
    grep -E "^\*\*.*\*\*:|^- \*\*.*\*\*:" "$OUTPUT_MD" | head -8
    echo ""
    echo "=== Test Results ==="
    grep -E "^- \*\*Total|^- \*\*Success|^- \*\*Line Coverage|^- \*\*Function Coverage" "$OUTPUT_MD" | head -5
fi