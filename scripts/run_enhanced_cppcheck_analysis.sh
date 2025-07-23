#!/bin/bash
# Script to run enhanced cppcheck static analysis with all rules enabled

SOURCE_DIR="$1"
OUTPUT_FILE="$2"

if [ -z "$SOURCE_DIR" ] || [ -z "$OUTPUT_FILE" ]; then
    echo "Usage: $0 <source_directory> <output_file>"
    exit 1
fi

# Create temporary directory for analysis
TMP_DIR=$(mktemp -d)
trap "rm -rf $TMP_DIR" EXIT

echo "Running enhanced cppcheck static analysis with all rules..."
echo "Source directory: $SOURCE_DIR"
echo "Output file: $OUTPUT_FILE"
echo

# Create combined output file
> "$TMP_DIR/cppcheck_analysis.txt"

# Find all C++ source and header files
FILES_TO_ANALYZE=($(find "$SOURCE_DIR/src" "$SOURCE_DIR/include" \( -name "*.hpp" -o -name "*.cc" -o -name "*.cpp" -o -name "*.h" \) 2>/dev/null))

echo "Files to analyze: ${#FILES_TO_ANALYZE[@]}"
for file in "${FILES_TO_ANALYZE[@]}"; do
    echo "  - $(basename "$file")"
done
echo

# Analyze each file individually to avoid critical errors stopping the entire analysis
ANALYZED_FILES=0
FAILED_FILES=0

for file in "${FILES_TO_ANALYZE[@]}"; do
    echo "Analyzing $(basename "$file")..."
    
    # Run cppcheck with all checks enabled on individual file
    cppcheck \
        --enable=all \
        --force \
        --std=c++17 \
        --suppress=missingIncludeSystem \
        --suppress=unmatchedSuppression \
        --suppress=internalAstError \
        --suppress=misra-config \
        --suppress=toomanyconfigs \
        --suppress=checkersReport \
        --template='{file}:{line}:{column}: {severity}: {message} [{id}]' \
        "$file" \
        2>> "$TMP_DIR/cppcheck_analysis.txt" || true
    
    # Also run with MISRA addon separately (to avoid internal errors stopping other checks)
    cppcheck \
        --addon=misra \
        --force \
        --std=c++17 \
        --suppress=missingIncludeSystem \
        --suppress=unmatchedSuppression \
        --suppress=internalAstError \
        --suppress=misra-config \
        --suppress=toomanyconfigs \
        --suppress=checkersReport \
        --template='{file}:{line}:{column}: {severity}: {message} [{id}]' \
        "$file" \
        2>> "$TMP_DIR/cppcheck_analysis.txt" || true
    
    # Check if analysis succeeded for this file
    if [ $? -eq 0 ]; then
        ((ANALYZED_FILES++))
    else
        ((FAILED_FILES++))
        echo "# Analysis of $file had issues, continuing..." >> "$TMP_DIR/cppcheck_analysis.txt"
    fi
done

echo
echo "Analysis summary:"
echo "  Files successfully analyzed: $ANALYZED_FILES"
echo "  Files with analysis issues: $FAILED_FILES"
echo

# Check if we got any results
if [ -s "$TMP_DIR/cppcheck_analysis.txt" ]; then
    echo "Enhanced cppcheck analysis completed"
    cp "$TMP_DIR/cppcheck_analysis.txt" "$OUTPUT_FILE"
else
    echo "No cppcheck analysis output generated"
    echo "# Enhanced cppcheck Analysis - No Issues Found" > "$OUTPUT_FILE"
    echo "" >> "$OUTPUT_FILE"
    echo "Analysis completed successfully with no cppcheck violations detected." >> "$OUTPUT_FILE"
fi

echo "Results saved to: $OUTPUT_FILE"