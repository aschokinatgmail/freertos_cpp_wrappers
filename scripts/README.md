# Report Generation Scripts

This directory contains scripts for generating validation and verification reports for the FreeRTOS C++ Wrappers project.

## Single Comprehensive Report Generator

### `generate_validation_verification_report.sh`

**NEW: Single comprehensive script** that consolidates all validation and verification activities into one streamlined process. This script replaces multiple separate scripts and eliminates intermediate files for simplified report generation.

**Complete Process:**
- Project build and test execution
- Code coverage analysis  
- Static code analysis (clang-tidy + MISRA C++)
- Comprehensive report generation (markdown + HTML)

**Usage:**
```bash
./scripts/generate_validation_verification_report.sh <build_dir> <output_md> <output_html> <source_dir>
```

**Parameters:**
- `build_dir`: Directory for build artifacts and test results
- `output_md`: Output markdown file path
- `output_html`: Output HTML file path  
- `source_dir`: Source code directory (usually `.`)

**Example:**
```bash
./scripts/generate_validation_verification_report.sh build VALIDATION_VERIFICATION_REPORT.md VALIDATION_VERIFICATION_REPORT.html .
```

**Features:**
- **All-in-one execution**: No intermediate files or multiple script coordination
- **Robust error handling**: Continues with existing data if build fails
- **Comprehensive analysis**: Test execution + static analysis + coverage
- **Professional reporting**: Detailed violations with code context
- **Main project focus**: Analyzes src/ and include/ directories only

**Generated Report Sections:**
- Executive summary with key quality metrics
- Static code analysis (MISRA C++ 2012 + clang-tidy) with detailed violations
- Test execution results by module with performance metrics
- Code coverage analysis with line-by-line context
- Validation conclusions and actionable recommendations

## Legacy Scripts (Deprecated)

The following scripts are maintained for compatibility but the single comprehensive script above is recommended:

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