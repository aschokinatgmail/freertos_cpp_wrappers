#!/bin/bash
#
# Code formatting script
# This script formats all C++ source files according to the project's clang-format configuration
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "=== FreeRTOS C++ Wrappers - Code Formatting ==="
echo "Project Root: $PROJECT_ROOT"
echo

# Ensure we're in the right directory
cd "$PROJECT_ROOT"

# Check if clang-format is available
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format not found. Please install clang-format to run code formatting."
    exit 1
fi

# Check if .clang-format file exists
if [ ! -f ".clang-format" ]; then
    echo "Error: .clang-format configuration file not found in project root."
    exit 1
fi

echo "Formatting C++ source files in include/ and src/ directories..."

# Find and format all relevant C++ files
FILES_TO_FORMAT=$(find include src -name "*.hpp" -o -name "*.cc" -o -name "*.cpp" -o -name "*.h" 2>/dev/null)

if [ -z "$FILES_TO_FORMAT" ]; then
    echo "No C++ source files found to format."
    exit 0
fi

echo "Files to be formatted:"
echo "$FILES_TO_FORMAT"
echo

# Apply formatting
echo "$FILES_TO_FORMAT" | xargs clang-format -i

echo "Code formatting completed successfully."
echo
echo "To check if any files were modified, run:"
echo "  git status"
echo "  git diff"