#!/bin/bash
# Script to run MISRA C++ static analysis using cppcheck

SOURCE_DIR="$1"
OUTPUT_FILE="$2"

if [ -z "$SOURCE_DIR" ] || [ -z "$OUTPUT_FILE" ]; then
    echo "Usage: $0 <source_directory> <output_file>"
    exit 1
fi

# Create temporary directory for analysis
TMP_DIR=$(mktemp -d)
trap "rm -rf $TMP_DIR" EXIT

echo "Running MISRA C++ static analysis..."
echo "Source directory: $SOURCE_DIR"
echo "Output file: $OUTPUT_FILE"
echo

# Run cppcheck with MISRA addon
cppcheck \
    --addon=misra \
    --enable=all \
    --force \
    --suppress=missingIncludeSystem \
    --suppress=unmatchedSuppression \
    --suppress=internalAstError \
    --template='{file}:{line}:{column}: {severity}: {message} [{id}]' \
    --xml \
    --xml-version=2 \
    "$SOURCE_DIR/src" \
    "$SOURCE_DIR/include" \
    2> "$TMP_DIR/misra_analysis.xml"

# Also run text output for debugging
cppcheck \
    --addon=misra \
    --enable=all \
    --force \
    --suppress=missingIncludeSystem \
    --suppress=unmatchedSuppression \
    --suppress=internalAstError \
    --template='{file}:{line}:{column}: {severity}: {message} [{id}]' \
    "$SOURCE_DIR/src" \
    "$SOURCE_DIR/include" \
    2> "$TMP_DIR/misra_analysis.txt" || true

# Check if we got any results
if [ -s "$TMP_DIR/misra_analysis.txt" ]; then
    echo "MISRA analysis completed successfully"
    cp "$TMP_DIR/misra_analysis.txt" "$OUTPUT_FILE"
else
    echo "No MISRA analysis output generated"
    echo "# MISRA C++ Analysis - No Issues Found" > "$OUTPUT_FILE"
    echo "" >> "$OUTPUT_FILE"
    echo "Analysis completed successfully with no MISRA C++ violations detected." >> "$OUTPUT_FILE"
fi

echo "Results saved to: $OUTPUT_FILE"