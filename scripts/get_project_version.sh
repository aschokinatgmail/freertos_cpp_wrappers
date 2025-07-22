#!/bin/bash
#
# Script to get project version from git tags
# Returns the latest tag if available, otherwise falls back to commit hash
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

cd "$PROJECT_ROOT"

# Get the latest tag, if any
LATEST_TAG=$(git describe --tags --abbrev=0 2>/dev/null || echo "")

if [ -n "$LATEST_TAG" ]; then
    # Tag exists, use it
    echo "$LATEST_TAG"
else
    # No tags exist, use short commit hash with v prefix
    COMMIT_HASH=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
    echo "v0.1.0-${COMMIT_HASH}"
fi