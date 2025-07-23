#!/bin/bash
#
# Generate Combined Validation and Verification Report
#
# This script creates a comprehensive validation report by:
# 1. Running the existing test suite to generate VALIDATION_VERIFICATION_REPORT
# 2. Running static analysis to generate STATIC_ANALYSIS_REPORT  
# 3. Combining both reports into a single comprehensive document
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "=== Generating Combined Validation and Verification Report ==="
echo "Project Root: $PROJECT_ROOT"
echo

# Ensure we're in the right directory
cd "$PROJECT_ROOT"

# Step 1: Run the full test and analysis suite to generate the base reports
echo "Step 1: Running full test suite and static analysis..."
if [ -f "scripts/run_tests_and_generate_reports.sh" ]; then
    bash scripts/run_tests_and_generate_reports.sh
else
    echo "Warning: run_tests_and_generate_reports.sh not found. Assuming reports already exist."
fi

# Step 2: Check that both base reports exist
if [ ! -f "STATIC_ANALYSIS_REPORT.md" ] || [ ! -f "VALIDATION_VERIFICATION_REPORT.md" ]; then
    echo "Error: Base reports not found. Please ensure static analysis and validation reports exist."
    echo "Expected files:"
    echo "  - STATIC_ANALYSIS_REPORT.md"
    echo "  - VALIDATION_VERIFICATION_REPORT.md"
    exit 1
fi

# Step 3: Combine the reports
echo "Step 2: Combining static analysis and validation reports..."
python3 "$SCRIPT_DIR/combine_reports.py" "STATIC_ANALYSIS_REPORT.md" "VALIDATION_VERIFICATION_REPORT.md" "COMBINED_VALIDATION_VERIFICATION_REPORT.md"

# Step 4: Generate HTML version
echo "Step 3: Generating HTML version..."
python3 "$SCRIPT_DIR/create_html_report.py" "COMBINED_VALIDATION_VERIFICATION_REPORT.md" "COMBINED_VALIDATION_VERIFICATION_REPORT.html"

# Step 5: Replace the original validation report with the combined version
echo "Step 4: Updating validation report with combined content..."
cp "COMBINED_VALIDATION_VERIFICATION_REPORT.md" "VALIDATION_VERIFICATION_REPORT.md"
cp "COMBINED_VALIDATION_VERIFICATION_REPORT.html" "VALIDATION_VERIFICATION_REPORT.html"

# Step 6: Clean up temporary files but keep separate static analysis report for reference
echo "Step 5: Cleaning up temporary files..."
rm "COMBINED_VALIDATION_VERIFICATION_REPORT.md" "COMBINED_VALIDATION_VERIFICATION_REPORT.html"

echo
echo "=== Combined Report Generation Complete ==="
echo "Generated reports:"
echo "  - VALIDATION_VERIFICATION_REPORT.md/.html - Combined comprehensive validation report"
echo "  - STATIC_ANALYSIS_REPORT.md/.html - Static analysis report (kept for reference)"
echo
echo "The VALIDATION_VERIFICATION_REPORT now contains:"
echo "  ✓ Executive Summary"
echo "  ✓ Static Code Analysis (clang-tidy + MISRA C++)" 
echo "  ✓ Test Execution Results (Google Test suite)"
echo "  ✓ Code Coverage Analysis"
echo "  ✓ Validation Conclusions and Recommendations"
echo
echo "View the combined report: open $PROJECT_ROOT/VALIDATION_VERIFICATION_REPORT.html"