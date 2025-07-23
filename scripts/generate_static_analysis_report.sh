#!/bin/bash
# Script to generate static analysis report with statistics

CLANG_TIDY_REPORT_TXT="$1"
CLANG_TIDY_STATS_MD="$2"
CLANG_TIDY_REPORT_MD="$3"
SOURCE_DIR="$4"

# Generate statistics
python3 "${SOURCE_DIR}/scripts/parse_clang_tidy_stats.py" "$CLANG_TIDY_REPORT_TXT" "$CLANG_TIDY_STATS_MD"

# Create the report
cat > "$CLANG_TIDY_REPORT_MD" << EOF
# Static Analysis Report

## Overview

**Static Analysis Tool**: clang-tidy
**Analysis Scope**: Library modules only - src/ include/
**Check Sets**: cppcoreguidelines-*, cert-*, google-*, hicpp-*

EOF

# Add statistics
cat "$CLANG_TIDY_STATS_MD" >> "$CLANG_TIDY_REPORT_MD" 2>/dev/null || echo "## Statistics

*Statistics not available - parsing failed.*

" >> "$CLANG_TIDY_REPORT_MD"

# Add detailed analysis
cat >> "$CLANG_TIDY_REPORT_MD" << EOF
## Detailed Analysis

\`\`\`
EOF

cat "$CLANG_TIDY_REPORT_TXT" >> "$CLANG_TIDY_REPORT_MD" 2>/dev/null || echo "No detailed output available" >> "$CLANG_TIDY_REPORT_MD"

cat >> "$CLANG_TIDY_REPORT_MD" << EOF
\`\`\`

---
EOF

# Add footer with date
date +"*Generated: %B %d, %Y*" >> "$CLANG_TIDY_REPORT_MD"
echo "*Tool: clang-tidy*" >> "$CLANG_TIDY_REPORT_MD"
echo "*Scope: Library modules only*" >> "$CLANG_TIDY_REPORT_MD"

# Also generate an HTML version with Apple Developer styling
CLANG_TIDY_REPORT_HTML="${CLANG_TIDY_REPORT_MD%.md}.html"
python3 "${SOURCE_DIR}/scripts/generate_html_report.py" "Static Analysis Report" "$CLANG_TIDY_REPORT_MD" "$CLANG_TIDY_REPORT_HTML" "${SOURCE_DIR}/scripts/report_template.html"

echo "Static analysis report generated: $CLANG_TIDY_REPORT_MD"
echo "Static analysis HTML report generated: $CLANG_TIDY_REPORT_HTML"
echo "Statistics generated: $CLANG_TIDY_STATS_MD"