#!/bin/bash
# Script to generate static analysis report with statistics

CLANG_TIDY_REPORT_TXT="$1"
CLANG_TIDY_STATS_MD="$2"
CLANG_TIDY_REPORT_MD="$3"
SOURCE_DIR="$4"
MISRA_REPORT_TXT="$5"
ENHANCED_CPPCHECK_REPORT_TXT="$6"

# Generate statistics for clang-tidy
python3 "${SOURCE_DIR}/scripts/parse_clang_tidy_stats.py" "$CLANG_TIDY_REPORT_TXT" "$CLANG_TIDY_STATS_MD"

# Create the report
cat > "$CLANG_TIDY_REPORT_MD" << EOF
# Static Analysis Report

## Overview

**Static Analysis Tools**: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)
**Analysis Scope**: Library modules only - src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-* + All cppcheck rules (style, performance, portability, security, etc.) + MISRA C 2012 (applicable to C++)

EOF

# Add clang-tidy statistics
echo "## clang-tidy Analysis" >> "$CLANG_TIDY_REPORT_MD"
echo "" >> "$CLANG_TIDY_REPORT_MD"
cat "$CLANG_TIDY_STATS_MD" >> "$CLANG_TIDY_REPORT_MD" 2>/dev/null || echo "### Statistics

*Statistics not available - parsing failed.*

" >> "$CLANG_TIDY_REPORT_MD"

# Add MISRA analysis if available
if [ -n "$MISRA_REPORT_TXT" ] && [ -f "$MISRA_REPORT_TXT" ]; then
    echo "## MISRA C++ Analysis" >> "$CLANG_TIDY_REPORT_MD"
    echo "" >> "$CLANG_TIDY_REPORT_MD"
    
    # Generate MISRA statistics and code context
    python3 "${SOURCE_DIR}/scripts/parse_misra_report.py" "$MISRA_REPORT_TXT" "$SOURCE_DIR" >> "$CLANG_TIDY_REPORT_MD" 2>/dev/null || echo "### MISRA Analysis

*MISRA analysis not available.*

" >> "$CLANG_TIDY_REPORT_MD"
fi

# Add Enhanced cppcheck analysis if available
if [ -n "$ENHANCED_CPPCHECK_REPORT_TXT" ] && [ -f "$ENHANCED_CPPCHECK_REPORT_TXT" ]; then
    echo "## Enhanced cppcheck Analysis (All Rules)" >> "$CLANG_TIDY_REPORT_MD"
    echo "" >> "$CLANG_TIDY_REPORT_MD"
    
    # Generate enhanced cppcheck statistics and code context
    python3 "${SOURCE_DIR}/scripts/parse_enhanced_cppcheck_report.py" "$ENHANCED_CPPCHECK_REPORT_TXT" "$SOURCE_DIR" >> "$CLANG_TIDY_REPORT_MD" 2>/dev/null || echo "### Enhanced cppcheck Analysis

*Enhanced cppcheck analysis not available.*

" >> "$CLANG_TIDY_REPORT_MD"
fi

# Add detailed clang-tidy analysis with violations and code snippets
if [ -f "$CLANG_TIDY_REPORT_TXT" ]; then
    echo "### Detailed clang-tidy Analysis" >> "$CLANG_TIDY_REPORT_MD"
    echo "" >> "$CLANG_TIDY_REPORT_MD"
    
    # Generate detailed clang-tidy violations with code context
    python3 "${SOURCE_DIR}/scripts/parse_clang_tidy_violations.py" "$CLANG_TIDY_REPORT_TXT" "$SOURCE_DIR" >> "$CLANG_TIDY_REPORT_MD" 2>/dev/null || echo "### Detailed clang-tidy Analysis

*Detailed clang-tidy analysis not available.*

" >> "$CLANG_TIDY_REPORT_MD"
else
    echo "### Detailed clang-tidy Analysis" >> "$CLANG_TIDY_REPORT_MD"
    echo "" >> "$CLANG_TIDY_REPORT_MD"
    echo "*clang-tidy report not available.*" >> "$CLANG_TIDY_REPORT_MD"
    echo "" >> "$CLANG_TIDY_REPORT_MD"
fi

echo "" >> "$CLANG_TIDY_REPORT_MD"
echo "---" >> "$CLANG_TIDY_REPORT_MD"

# Add footer with date
date +"*Generated: %B %d, %Y*" >> "$CLANG_TIDY_REPORT_MD"
echo "*Tools: clang-tidy + Enhanced cppcheck (all rules) + MISRA C++ (cppcheck)*" >> "$CLANG_TIDY_REPORT_MD"
echo "*Scope: Library modules only*" >> "$CLANG_TIDY_REPORT_MD"

# Also generate an HTML version with Apple Developer styling
CLANG_TIDY_REPORT_HTML="${CLANG_TIDY_REPORT_MD%.md}.html"
python3 "${SOURCE_DIR}/scripts/generate_html_report.py" "Static Analysis Report" "$CLANG_TIDY_REPORT_MD" "$CLANG_TIDY_REPORT_HTML" "${SOURCE_DIR}/scripts/report_template.html"

echo "Static analysis report generated: $CLANG_TIDY_REPORT_MD"
echo "Static analysis HTML report generated: $CLANG_TIDY_REPORT_HTML"
echo "Statistics generated: $CLANG_TIDY_STATS_MD"