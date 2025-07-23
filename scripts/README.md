# Report Generation Scripts

This directory contains scripts for generating various reports for the FreeRTOS C++ Wrappers project.

## Combined Validation and Verification Report

### `generate_combined_validation_report.sh`

Generates a comprehensive validation and verification report that combines static analysis results with test execution and coverage data.

**Usage:**
```bash
./scripts/generate_combined_validation_report.sh <build_dir> <output_md> <output_html> <source_dir>
```

**Parameters:**
- `build_dir`: Directory containing build artifacts and test results
- `output_md`: Output markdown file path
- `output_html`: Output HTML file path  
- `source_dir`: Source code directory (usually `.`)

**Example:**
```bash
./scripts/generate_combined_validation_report.sh build VALIDATION_VERIFICATION_REPORT.md VALIDATION_VERIFICATION_REPORT.html .
```

**Required build artifacts:**
- `coverage_filtered.info`: LCOV coverage data
- `clang_tidy_report.txt`: clang-tidy analysis results (optional)
- `misra_report.txt`: MISRA analysis results (optional)
- `enhanced_cppcheck_report.txt`: Enhanced cppcheck results (optional)

**Output includes:**
- Executive summary with key metrics
- **Static Code Analysis** section with:
  - clang-tidy analysis statistics
  - MISRA C++ compliance analysis with detailed violations
  - Code context for each violation
  - Rule descriptions and rationales
- **Test Execution Results** by module
- **Code Coverage Analysis** with detailed uncovered area explanations
- **Validation Conclusions** and recommendations

## Individual Report Scripts

### `generate_static_analysis_report.sh`
Generates standalone static analysis report with detailed violations.

### `generate_validation_verification_report.py`
Generates standalone validation and verification report with test results and coverage.

### Supporting Scripts

- `parse_clang_tidy_stats.py`: Parses clang-tidy output for statistics
- `parse_misra_report.py`: Parses MISRA analysis with code context
- `parse_enhanced_cppcheck_report.py`: Parses enhanced cppcheck results
- `generate_html_report.py`: Converts markdown reports to styled HTML
- `misra_rule_descriptions.py`: MISRA rule descriptions and rationales

## Report Features

### Detailed Static Analysis
- **Comprehensive Coverage**: Includes clang-tidy, MISRA C++, and enhanced cppcheck results
- **Code Context**: Shows actual code snippets for each violation
- **Rule Documentation**: Provides explanations and rationales for each rule
- **Statistics**: Summary tables with counts by rule, file, and severity

### Test Results Analysis  
- **Module Breakdown**: Results organized by FreeRTOS module (Task, Semaphore, Queue, etc.)
- **Performance Metrics**: Execution times and performance distribution
- **Success Tracking**: Pass/fail rates and detailed test outcomes

### Coverage Analysis
- **Detailed Explanations**: Categorizes uncovered code with explanations
- **Context-Aware**: Shows specific code areas that cannot be unit tested
- **Production-Ready Assessment**: Validates code quality for production use

## Integration

The combined report script is designed to be integrated into CI/CD pipelines:

```bash
# After building and running tests
make build test coverage static-analysis

# Generate comprehensive report
./scripts/generate_combined_validation_report.sh build VALIDATION_VERIFICATION_REPORT.md VALIDATION_VERIFICATION_REPORT.html .
```

This replaces the need to maintain separate static analysis and V&V reports while providing all detailed information in a single, well-organized document.