#!/bin/bash
#
# Script to generate documentation with dynamic project version
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

cd "$PROJECT_ROOT"

# Get the project version
PROJECT_VERSION=$("$SCRIPT_DIR/get_project_version.sh")

echo "=== FreeRTOS C++ Wrappers - Documentation Generation ==="
echo "Project Root: $PROJECT_ROOT"
echo "Project Version: $PROJECT_VERSION"
echo

# Create a temporary Doxyfile with the actual version
TEMP_DOXYFILE="$(mktemp)"
sed "s/@PROJECT_VERSION@/$PROJECT_VERSION/g" Doxyfile > "$TEMP_DOXYFILE"

# Generate documentation
echo "Generating documentation..."
doxygen "$TEMP_DOXYFILE"

# Clean up
rm "$TEMP_DOXYFILE"

echo
echo "=== Documentation Generated Successfully ==="
echo "Generated documentation in: $PROJECT_ROOT/docs/html"
echo "Open $PROJECT_ROOT/docs/html/index.html in a browser to view"