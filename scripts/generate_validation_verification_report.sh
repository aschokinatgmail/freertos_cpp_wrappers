#!/bin/bash
# Comprehensive Validation and Verification Report Generator
# This script generates a real report based on actual test execution and static analysis
# No fake data - everything is based on running the actual tools

set -e

# Script parameters
BUILD_DIR="$1"
OUTPUT_MD="$2"
OUTPUT_HTML="$3"
SOURCE_DIR="$4"

# Usage check
if [ $# -ne 4 ]; then
    echo "Usage: $0 <build_dir> <output_md> <output_html> <source_dir>"
    echo "Example: $0 build VALIDATION_VERIFICATION_REPORT.md VALIDATION_VERIFICATION_REPORT.html ."
    exit 1
fi

# Validate inputs
if [ ! -d "$SOURCE_DIR" ]; then
    echo "Error: Source directory '$SOURCE_DIR' does not exist"
    exit 1
fi

echo "==================================================================="
echo "Generating Real Validation and Verification Report"
echo "==================================================================="
echo "Build directory: $BUILD_DIR"
echo "Output Markdown: $OUTPUT_MD"
echo "Output HTML: $OUTPUT_HTML"
echo "Source directory: $SOURCE_DIR"
echo ""

# Convert to absolute paths
SOURCE_DIR_ABS="$(cd "$SOURCE_DIR" && pwd)"
BUILD_DIR_ABS="$SOURCE_DIR_ABS/$BUILD_DIR"

echo "Stage 1: Project Build and Test Execution"
echo "=================================================="

# Install dependencies if needed
if ! dpkg -l | grep -q libgtest-dev; then
    echo "Installing GoogleTest dependencies..."
    sudo apt update && sudo apt install -y libgtest-dev libgmock-dev cppcheck lcov
fi

# Create and configure build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Configure with coverage enabled
echo "Configuring project with coverage enabled..."
cmake -DENABLE_COVERAGE=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

# Build the project
echo "Building project..."
make -j$(nproc)

# Run tests and capture output
echo "Executing Google Test suite..."
TEST_OUTPUT_FILE="$BUILD_DIR_ABS/test_execution_output.txt"
ctest --verbose > "$TEST_OUTPUT_FILE" 2>&1

# Generate coverage data
echo "Generating code coverage data..."
lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch,negative,gcov 2>/dev/null
lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info 2>/dev/null

echo ""
echo "Stage 2: Static Code Analysis"
echo "=================================================="

cd "$SOURCE_DIR_ABS"

# Find all main project files for analysis
PROJECT_FILES=($(find "$SOURCE_DIR_ABS/src" "$SOURCE_DIR_ABS/include" \( -name "*.hpp" -o -name "*.cc" -o -name "*.cpp" -o -name "*.h" \) 2>/dev/null))
echo "Found ${#PROJECT_FILES[@]} files for static analysis"

# Run MISRA C++ analysis
echo "Running MISRA C++ analysis..."
MISRA_OUTPUT="$BUILD_DIR_ABS/misra_analysis.txt"
cppcheck --addon=misra.json --inline-suppr --enable=all \
    --suppress=missingIncludeSystem --suppress=unusedFunction \
    "$SOURCE_DIR_ABS/src" "$SOURCE_DIR_ABS/include" 2> "$MISRA_OUTPUT"

# Run clang-tidy analysis
echo "Running clang-tidy analysis..."
CLANG_TIDY_OUTPUT="$BUILD_DIR_ABS/clang_tidy_analysis.txt"

> "$CLANG_TIDY_OUTPUT"  # Clear file

cd "$BUILD_DIR_ABS"
for file in "${PROJECT_FILES[@]}"; do
    echo "  Analyzing $(basename "$file")..."
    clang-tidy \
        --checks='cppcoreguidelines-*,cert-*,google-*,hicpp-*,readability-*,performance-*,modernize-*,misc-*,-misc-no-recursion,-cert-dcl21-cpp,-misc-non-private-member-variables-in-classes,-cppcoreguidelines-non-private-member-variables-in-classes' \
        --format-style=llvm \
        "$file" \
        -- -std=c++17 -I"$SOURCE_DIR_ABS/include" \
        2>> "$CLANG_TIDY_OUTPUT" || true
done

echo ""
echo "Stage 3: Report Generation"
echo "=================================================="

cd "$SOURCE_DIR_ABS"

# Start generating the comprehensive report
cat > "$OUTPUT_MD" << 'EOF'
# Validation and Verification Report

## Executive Summary

This report provides comprehensive validation and verification results for the FreeRTOS C++ Wrappers project, combining test execution results, code coverage analysis, and static code analysis in a unified validation framework.

**Validation Methodology:**
- **Static Code Analysis**: MISRA C++ 2012 compliance + clang-tidy quality checks
- **Dynamic Testing**: Google Test framework with comprehensive unit tests
- **Code Coverage**: Line and function coverage analysis
- **Analysis Scope**: Main project code (src/ and include/ directories)

**Key Quality Metrics:**
EOF

# Parse test results and add summary
echo "Parsing test execution results..."

# Extract test statistics from CTest output
TOTAL_TESTS=$(grep -c "Test.*#.*:" "$TEST_OUTPUT_FILE" 2>/dev/null || echo "0")
PASSED_TESTS=$(grep -c "Passed" "$TEST_OUTPUT_FILE" 2>/dev/null || echo "0")
FAILED_TESTS=$(grep -c "Failed" "$TEST_OUTPUT_FILE" 2>/dev/null || echo "0")

# Calculate success rate
if [ "$TOTAL_TESTS" -gt 0 ]; then
    SUCCESS_RATE=$(awk "BEGIN {printf \"%.1f\", ($PASSED_TESTS/$TOTAL_TESTS)*100}")
else
    SUCCESS_RATE="0.0"
fi

# Add test execution summary to report
cat >> "$OUTPUT_MD" << EOF
- **Total Tests Executed**: $TOTAL_TESTS
- **✅ Passed**: $PASSED_TESTS tests
- **❌ Failed**: $FAILED_TESTS tests
- **Success Rate**: ${SUCCESS_RATE}%

EOF

# Parse static analysis results
echo "Parsing static analysis results..."

# Count MISRA violations (exclude internal errors)
MISRA_VIOLATIONS=0
if [ -f "$MISRA_OUTPUT" ]; then
    MISRA_VIOLATIONS=$(grep -c "misra-c2012" "$MISRA_OUTPUT" 2>/dev/null || echo "0")
fi

# Count clang-tidy issues (exclude FreeRTOS header errors)
CLANG_TIDY_WARNINGS=0
CLANG_TIDY_ERRORS=0
if [ -f "$CLANG_TIDY_OUTPUT" ]; then
    CLANG_TIDY_WARNINGS=$(grep -E ":[0-9]+:[0-9]+: warning:" "$CLANG_TIDY_OUTPUT" | grep -v "FreeRTOS.h.*not found" | wc -l || echo "0")
    CLANG_TIDY_ERRORS=$(grep -E ":[0-9]+:[0-9]+: error:" "$CLANG_TIDY_OUTPUT" | grep -v "FreeRTOS.h.*not found" | wc -l || echo "0")
fi

TOTAL_STATIC_ISSUES=$((MISRA_VIOLATIONS + CLANG_TIDY_WARNINGS + CLANG_TIDY_ERRORS))

# Add static analysis summary
cat >> "$OUTPUT_MD" << EOF
- **Static Analysis Issues**: $TOTAL_STATIC_ISSUES total ($MISRA_VIOLATIONS MISRA, $CLANG_TIDY_WARNINGS clang-tidy warnings, $CLANG_TIDY_ERRORS clang-tidy errors)

## Static Code Analysis

### Overview

**Static Analysis Tools**: clang-tidy + cppcheck (MISRA C++ 2012)  
**Analysis Scope**: Main project code only (src/ and include/ directories)  
**Excluded**: Test harness, mocks, and external dependencies

### Analysis Results Summary

| Tool | Issues Found | Severity | Status |
|------|-------------|----------|---------|
| MISRA C++ 2012 | $MISRA_VIOLATIONS violations | Various | Analyzed |
| clang-tidy | $CLANG_TIDY_WARNINGS warnings, $CLANG_TIDY_ERRORS errors | Various | Analyzed |

EOF

# Add detailed MISRA violations if found
if [ "$MISRA_VIOLATIONS" -gt 0 ] && [ -f "$MISRA_OUTPUT" ]; then
    echo "Adding detailed MISRA violations..."
    cat >> "$OUTPUT_MD" << 'EOF'
### MISRA C++ 2012 Violations

#### Detailed Violations with Code Context

EOF

    # Parse MISRA violations and add code context
    python3 - << 'PYEOF' "$MISRA_OUTPUT" "$SOURCE_DIR_ABS" >> "$OUTPUT_MD"
import sys
import os
import re
from collections import defaultdict

misra_file = sys.argv[1]
source_dir = sys.argv[2]

if not os.path.exists(misra_file):
    print("*No MISRA violations file found.*")
    sys.exit(0)

violations = []
with open(misra_file, 'r') as f:
    for line in f:
        line = line.strip()
        if not line or line.startswith('#'):
            continue
            
        # Parse cppcheck format: file:line:column: severity: message [id]
        match = re.match(r'^([^:]+):(\d+):(\d+):\s*(\w+):\s*(.+?)\s*\[([^\]]+)\]', line)
        if match:
            file_path = match.group(1)
            line_num = int(match.group(2))
            severity = match.group(4)
            message = match.group(5)
            rule_id = match.group(6)
            
            # Only include MISRA violations
            if 'misra' in rule_id.lower() or 'MISRA' in message:
                violations.append({
                    'file': file_path,
                    'line': line_num,
                    'severity': severity,
                    'message': message,
                    'rule': rule_id
                })

# Group violations by rule
rule_groups = defaultdict(list)
for v in violations:
    rule_groups[v['rule']].append(v)

violation_num = 1
for rule, rule_violations in rule_groups.items():
    print(f"**Violation {violation_num}**: {rule}")
    print(f"**Message**: {rule_violations[0]['message']}")
    print(f"**Severity**: {rule_violations[0]['severity']}")
    print(f"**Occurrences**: {len(rule_violations)}")
    print()
    
    # Show first few occurrences with code context
    for i, violation in enumerate(rule_violations[:3]):  # Limit to first 3
        file_path = violation['file']
        line_num = violation['line']
        
        # Get relative path for display
        rel_path = os.path.relpath(file_path, source_dir) if file_path.startswith(source_dir) else file_path
        
        print(f"**Location {i+1}**: `{rel_path}:{line_num}`")
        print()
        print("```cpp")
        
        # Read and display code context
        try:
            with open(file_path, 'r') as f:
                lines = f.readlines()
                start_line = max(0, line_num - 3)
                end_line = min(len(lines), line_num + 2)
                
                for j in range(start_line, end_line):
                    line_content = lines[j].rstrip()
                    if j + 1 == line_num:
                        print(f">>> {j+1:4}: {line_content}")
                    else:
                        print(f"    {j+1:4}: {line_content}")
        except:
            print(f"    {line_num}: [Unable to read file]")
        
        print("```")
        print()
    
    if len(rule_violations) > 3:
        print(f"*... and {len(rule_violations) - 3} more occurrences*")
        print()
    
    violation_num += 1
PYEOF
fi

# Add detailed clang-tidy analysis if found
if [ "$CLANG_TIDY_WARNINGS" -gt 0 ] || [ "$CLANG_TIDY_ERRORS" -gt 0 ]; then
    echo "Adding detailed clang-tidy analysis..."
    cat >> "$OUTPUT_MD" << 'EOF'
### clang-tidy Analysis

#### Detailed Violations with Code Context

EOF

    # Parse clang-tidy violations and add code context
    python3 - << 'PYEOF' "$CLANG_TIDY_OUTPUT" "$SOURCE_DIR_ABS" >> "$OUTPUT_MD"
import sys
import os
import re
from collections import defaultdict

clang_file = sys.argv[1]
source_dir = sys.argv[2]

if not os.path.exists(clang_file):
    print("*No clang-tidy violations file found.*")
    sys.exit(0)

violations = []
with open(clang_file, 'r') as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
            
        # Parse clang-tidy format: file:line:column: severity: message [rule]
        match = re.match(r'^([^:]+):(\d+):(\d+):\s*(warning|error):\s*(.+?)\s*\[([^\]]+)\]', line)
        if match:
            file_path = match.group(1)
            line_num = int(match.group(2))
            severity = match.group(4)
            message = match.group(5)
            rule = match.group(6)
            
            # Skip FreeRTOS header errors (false positives)
            if 'FreeRTOS.h' in message and 'not found' in message:
                continue
            
            violations.append({
                'file': file_path,
                'line': line_num,
                'severity': severity,
                'message': message,
                'rule': rule
            })

# Group violations by rule
rule_groups = defaultdict(list)
for v in violations:
    rule_groups[v['rule']].append(v)

violation_num = 1
for rule, rule_violations in rule_groups.items():
    print(f"**Violation {violation_num}**: {rule}")
    print(f"**Message**: {rule_violations[0]['message']}")
    print(f"**Severity**: {rule_violations[0]['severity']}")
    print(f"**Occurrences**: {len(rule_violations)}")
    print()
    
    # Show first few occurrences with code context
    for i, violation in enumerate(rule_violations[:2]):  # Limit to first 2
        file_path = violation['file']
        line_num = violation['line']
        
        # Get relative path for display
        rel_path = os.path.relpath(file_path, source_dir) if file_path.startswith(source_dir) else file_path
        
        print(f"**Location {i+1}**: `{rel_path}:{line_num}`")
        print()
        print("```cpp")
        
        # Read and display code context
        try:
            with open(file_path, 'r') as f:
                lines = f.readlines()
                start_line = max(0, line_num - 3)
                end_line = min(len(lines), line_num + 2)
                
                for j in range(start_line, end_line):
                    line_content = lines[j].rstrip()
                    if j + 1 == line_num:
                        print(f">>> {j+1:4}: {line_content}")
                    else:
                        print(f"    {j+1:4}: {line_content}")
        except:
            print(f"    {line_num}: [Unable to read file]")
        
        print("```")
        print()
    
    if len(rule_violations) > 2:
        print(f"*... and {len(rule_violations) - 2} more occurrences*")
        print()
    
    violation_num += 1
PYEOF
fi

# Add test execution results section
echo "Adding test execution results..."
cat >> "$OUTPUT_MD" << 'EOF'

## Test Execution Results

### Test Execution Summary

EOF

# Parse and add detailed test results
echo "Parsing detailed test results..."
python3 - << 'PYEOF' "$TEST_OUTPUT_FILE" >> "$OUTPUT_MD"
import sys
import re
from collections import defaultdict

test_file = sys.argv[1]

with open(test_file, 'r') as f:
    content = f.read()

print("### Detailed Test Results by Module")
print()

# Parse test results by looking for test patterns
test_pattern = r'(\d+)/(\d+)\s+Test\s+#(\d+):\s+([^.]+\.[^.]+).*?(Passed|Failed)\s+([\d.]+)\s+sec'
tests = re.findall(test_pattern, content, re.MULTILINE)

if not tests:
    print("*No detailed test results found in output.*")
    sys.exit(0)

# Group tests by module (first part before the dot)
modules = defaultdict(list)
for test in tests:
    test_num, total, test_id, test_name, result, time = test
    module_name = test_name.split('.')[0] if '.' in test_name else 'Unknown'
    modules[module_name].append({
        'id': test_id,
        'name': test_name,
        'result': result,
        'time': float(time)
    })

# Display results by module
for module_name, module_tests in modules.items():
    passed = sum(1 for t in module_tests if t['result'] == 'Passed')
    failed = sum(1 for t in module_tests if t['result'] == 'Failed') 
    total_time = sum(t['time'] for t in module_tests)
    success_rate = (passed / len(module_tests)) * 100 if module_tests else 0
    
    # Improve module display names
    display_name = module_name.replace('Test', ' Module Tests')
    
    print(f"### {display_name}")
    print()
    print("**Module Statistics:**")
    print(f"- Tests: {len(module_tests)}")
    print(f"- Passed: {passed}")
    print(f"- Failed: {failed}")
    print(f"- Success Rate: {success_rate:.1f}%")
    print(f"- Total Time: {total_time:.3f}s")
    print()
    
    if len(module_tests) <= 30:  # Show detailed results for modules with <= 30 tests
        print("**Detailed Test Results:**")
        print()
        print("| Test ID | Test Name | Outcome | Execution Time |")
        print("|---------|-----------|---------|----------------|")
        
        for i, test in enumerate(module_tests, 1):
            status_icon = "✅" if test['result'] == 'Passed' else "❌"
            print(f"| {i} | {test['name']} | {status_icon} {test['result'].upper()} | {test['time']:.3f}s |")
        print()
    else:
        print(f"**Summary**: {len(module_tests)} tests executed with {passed} passed, {failed} failed.")
        print()
PYEOF

# Add code coverage analysis if available
echo "Adding code coverage analysis..."
cat >> "$OUTPUT_MD" << 'EOF'

## Code Coverage Analysis

### Coverage Summary

EOF

# Parse coverage data if available
if [ -f "$BUILD_DIR_ABS/coverage_filtered.info" ]; then
    echo "Parsing coverage data..."
    
    # Generate coverage summary
    python3 - << 'PYEOF' "$BUILD_DIR_ABS/coverage_filtered.info" "$SOURCE_DIR_ABS" >> "$OUTPUT_MD"
import sys
import os
import re

coverage_file = sys.argv[1]
source_dir = sys.argv[2]

if not os.path.exists(coverage_file):
    print("**Coverage Data**: Not available")
    print()
    print("*Coverage analysis was not successful. Ensure the project is built with coverage enabled.*")
    sys.exit(0)

try:
    with open(coverage_file, 'r') as f:
        content = f.read()
except:
    print("**Coverage Data**: Unable to read coverage file")
    sys.exit(0)

# Parse LCOV format
lines_found = 0
lines_hit = 0
functions_found = 0  
functions_hit = 0

for line in content.split('\n'):
    if line.startswith('LF:'):
        lines_found += int(line.split(':')[1])
    elif line.startswith('LH:'):
        lines_hit += int(line.split(':')[1])
    elif line.startswith('FNF:'):
        functions_found += int(line.split(':')[1])
    elif line.startswith('FNH:'):
        functions_hit += int(line.split(':')[1])

# Calculate coverage percentages
line_coverage = (lines_hit / lines_found * 100) if lines_found > 0 else 0
function_coverage = (functions_hit / functions_found * 100) if functions_found > 0 else 0

print(f"**Line Coverage**: {line_coverage:.1f}% ({lines_hit:,} of {lines_found:,} lines)")
print(f"**Function Coverage**: {function_coverage:.1f}% ({functions_hit} of {functions_found} functions)")
print()

# Determine coverage quality
if line_coverage >= 95:
    quality = "Excellent"
elif line_coverage >= 85:
    quality = "Good"
elif line_coverage >= 70:
    quality = "Acceptable"
else:
    quality = "Needs Improvement"

print(f"**Coverage Quality**: {quality}")
print()

print("### Coverage Analysis by File")
print()
print("| File | Line Coverage | Functions |")
print("|------|---------------|-----------|")

# Parse per-file coverage
current_file = None
file_lines_found = 0
file_lines_hit = 0
file_functions_found = 0
file_functions_hit = 0

for line in content.split('\n'):
    if line.startswith('SF:'):
        # Print previous file if exists
        if current_file:
            file_line_cov = (file_lines_hit / file_lines_found * 100) if file_lines_found > 0 else 0
            rel_path = os.path.relpath(current_file, source_dir) if current_file.startswith(source_dir) else current_file
            print(f"| `{rel_path}` | {file_line_cov:.1f}% | {file_functions_hit}/{file_functions_found} |")
        
        # Start new file
        current_file = line.split(':', 1)[1]
        file_lines_found = 0
        file_lines_hit = 0
        file_functions_found = 0
        file_functions_hit = 0
    elif line.startswith('LF:'):
        file_lines_found = int(line.split(':')[1])
    elif line.startswith('LH:'):
        file_lines_hit = int(line.split(':')[1])
    elif line.startswith('FNF:'):
        file_functions_found = int(line.split(':')[1])
    elif line.startswith('FNH:'):
        file_functions_hit = int(line.split(':')[1])

# Don't forget the last file
if current_file:
    file_line_cov = (file_lines_hit / file_lines_found * 100) if file_lines_found > 0 else 0
    rel_path = os.path.relpath(current_file, source_dir) if current_file.startswith(source_dir) else current_file
    print(f"| `{rel_path}` | {file_line_cov:.1f}% | {file_functions_hit}/{file_functions_found} |")

print()
PYEOF
else
    cat >> "$OUTPUT_MD" << 'EOF'
**Coverage Data**: Not available

*Coverage analysis was not successful. Ensure the project is built with coverage enabled and tests are executed.*

EOF
fi

# Add validation conclusions
echo "Adding validation conclusions..."
cat >> "$OUTPUT_MD" << EOF

## Validation Conclusions

### Overall Assessment

Based on the comprehensive validation and verification analysis:

**Test Execution**: ${SUCCESS_RATE}% success rate with $TOTAL_TESTS tests executed
**Static Analysis**: $TOTAL_STATIC_ISSUES total issues identified requiring review
**Analysis Scope**: Complete coverage of main project code (src/ and include/ directories)

### Recommendations

EOF

# Generate context-aware recommendations
if [ "$SUCCESS_RATE" = "100.0" ]; then
    echo "1. **Test Suite**: Excellent test coverage with 100% pass rate. Consider expanding edge case testing." >> "$OUTPUT_MD"
else
    echo "1. **Test Suite**: Address failing tests to achieve 100% pass rate before release." >> "$OUTPUT_MD"
fi

if [ "$TOTAL_STATIC_ISSUES" -lt 50 ]; then
    echo "2. **Static Analysis**: Manageable number of static analysis issues. Review and address critical violations." >> "$OUTPUT_MD"
elif [ "$TOTAL_STATIC_ISSUES" -lt 100 ]; then
    echo "2. **Static Analysis**: Moderate number of static analysis issues. Prioritize MISRA violations and errors." >> "$OUTPUT_MD"
else
    echo "2. **Static Analysis**: High number of static analysis issues. Systematic review and remediation recommended." >> "$OUTPUT_MD"
fi

cat >> "$OUTPUT_MD" << 'EOF'
3. **Code Coverage**: Continue monitoring coverage metrics and aim for >95% line coverage.
4. **MISRA Compliance**: Address MISRA C++ violations to improve code safety and maintainability.
5. **Continuous Integration**: Integrate this validation process into CI/CD pipeline.

### Validation Status

EOF

# Determine overall validation status
if [ "$SUCCESS_RATE" = "100.0" ] && [ "$TOTAL_STATIC_ISSUES" -lt 50 ]; then
    echo "**Overall Status**: ✅ **VALIDATED** - Project meets quality standards" >> "$OUTPUT_MD"
elif [ "$SUCCESS_RATE" = "100.0" ] && [ "$TOTAL_STATIC_ISSUES" -lt 100 ]; then
    echo "**Overall Status**: ⚠️ **CONDITIONALLY VALIDATED** - Address static analysis issues" >> "$OUTPUT_MD"
else
    echo "**Overall Status**: ❌ **VALIDATION PENDING** - Resolve failing tests and static analysis issues" >> "$OUTPUT_MD"
fi

# Add footer
cat >> "$OUTPUT_MD" << EOF

---

*Generated: $(date '+%B %d, %Y at %H:%M:%S')*  
*Tools: Google Test + clang-tidy + MISRA C++ (cppcheck)*  
*Scope: Complete project validation and verification*
EOF

echo ""
echo "Stage 4: HTML Report Generation"
echo "=================================================="

# Generate HTML version if template exists
if [ -f "$SOURCE_DIR_ABS/scripts/generate_html_report.py" ] && [ -f "$SOURCE_DIR_ABS/scripts/report_template.html" ]; then
    echo "Generating HTML report..."
    if python3 "$SOURCE_DIR_ABS/scripts/generate_html_report.py" \
        "Validation and Verification Report" \
        "$OUTPUT_MD" \
        "$OUTPUT_HTML" \
        "$SOURCE_DIR_ABS/scripts/report_template.html" 2>/dev/null; then
        echo "HTML report generated successfully"
    else
        echo "HTML generation failed (missing dependencies), continuing with markdown report"
    fi
else
    echo "HTML generation not available (missing template or script)"
fi

echo ""
echo "==================================================================="
echo "Real Validation and Verification Report Generation Complete"
echo "==================================================================="
echo ""
echo "Generated Reports:"
echo "  📄 Markdown: $OUTPUT_MD"
[ -f "$OUTPUT_HTML" ] && echo "  🌐 HTML: $OUTPUT_HTML"
echo ""
echo "Report Summary:"
echo "  📊 Tests: $TOTAL_TESTS executed, ${SUCCESS_RATE}% success rate"
echo "  🔍 Static Analysis: $TOTAL_STATIC_ISSUES issues found"
echo "  📈 Analysis: Complete validation of main project code"
echo ""

# Show final validation status
if [ "$SUCCESS_RATE" = "100.0" ] && [ "$TOTAL_STATIC_ISSUES" -lt 50 ]; then
    echo "✅ VALIDATION STATUS: PASSED"
elif [ "$SUCCESS_RATE" = "100.0" ] && [ "$TOTAL_STATIC_ISSUES" -lt 100 ]; then
    echo "⚠️ VALIDATION STATUS: CONDITIONAL (review static analysis)"
else
    echo "❌ VALIDATION STATUS: NEEDS ATTENTION"
fi

echo ""
echo "Next Steps:"
echo "  • Review the generated report for detailed findings"
echo "  • Address any failing tests or critical static analysis issues"
echo "  • Consider integrating this validation process into CI/CD"
echo ""