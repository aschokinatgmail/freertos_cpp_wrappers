#!/bin/bash
#
# Test script to verify documentation generation works properly
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "=== Testing Documentation Generation ==="
echo

# Clean up any existing documentation first
if [ -d "$PROJECT_ROOT/docs/html" ]; then
    echo "Cleaning existing documentation..."
    rm -rf "$PROJECT_ROOT/docs/html"
fi

# Generate documentation
echo "Running documentation generation..."
"$SCRIPT_DIR/generate_documentation.sh"

echo
echo "=== Verifying Generated Documentation ==="

# Check that the HTML output directory was created
if [ ! -d "$PROJECT_ROOT/docs/html" ]; then
    echo "❌ ERROR: Documentation output directory not found!"
    exit 1
fi

# Check that index.html was generated
if [ ! -f "$PROJECT_ROOT/docs/html/index.html" ]; then
    echo "❌ ERROR: index.html not generated!"
    exit 1
fi

# Check that custom.css was copied to output
if [ ! -f "$PROJECT_ROOT/docs/html/custom.css" ]; then
    echo "❌ ERROR: custom.css not found in output!"
    exit 1
fi

# Check that custom.css is referenced in index.html
if ! grep -q 'custom.css' "$PROJECT_ROOT/docs/html/index.html"; then
    echo "❌ ERROR: custom.css not referenced in index.html!"
    exit 1
fi

# Check that main documentation pages were generated
EXPECTED_FILES=(
    "classes.html"
    "files.html"
    "examples.html"
    "classfreertos_1_1task.html"
    "classfreertos_1_1queue.html"
    "classfreertos_1_1mutex.html"
)

for file in "${EXPECTED_FILES[@]}"; do
    if [ ! -f "$PROJECT_ROOT/docs/html/$file" ]; then
        echo "❌ ERROR: Expected file $file not found!"
        exit 1
    fi
done

# Verify CSS content has the new theme
if ! grep -q "Apple Developer-inspired light theme" "$PROJECT_ROOT/docs/html/custom.css"; then
    echo "❌ ERROR: Custom CSS doesn't contain the new theme styling!"
    exit 1
fi

# Check that doxygen.css (base styles) is also present
if [ ! -f "$PROJECT_ROOT/docs/html/doxygen.css" ]; then
    echo "❌ ERROR: doxygen.css not found!"
    exit 1
fi

echo "✅ All documentation generation tests passed!"
echo
echo "Documentation statistics:"
echo "  HTML files: $(find "$PROJECT_ROOT/docs/html" -name "*.html" | wc -l)"
echo "  CSS files: $(find "$PROJECT_ROOT/docs/html" -name "*.css" | wc -l)"
echo "  JavaScript files: $(find "$PROJECT_ROOT/docs/html" -name "*.js" | wc -l)"
echo
echo "Documentation is ready at: $PROJECT_ROOT/docs/html/index.html"