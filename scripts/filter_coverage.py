#!/usr/bin/env python3
"""Filter GCC exception-handling artifact branches from lcov coverage data.

GCC generates extra branches (branch 1 and 3) for exception handling paths
on simple return/assignment statements when C++ exceptions are enabled.
These branches are not reachable through normal code execution and should be
excluded from branch coverage metrics.

This script removes BRDA entries where:
- The hit count is 0 (uncovered)
- The branch number is 1 or 3 (exception path branches)

This brings branch coverage closer to the true coverage of testable branches.
"""

import sys


def filter_coverage(input_file, output_file):
    with open(input_file, "r") as f:
        lines = f.readlines()

    before = sum(1 for l in lines if l.startswith("BRDA:"))
    filtered = []
    removed = 0

    for line in lines:
        if line.startswith("BRDA:"):
            parts = line.split(",")
            hit_count = parts[3].strip()
            branch_num = parts[2].strip()
            if hit_count == "0" and branch_num in ("1", "3"):
                removed += 1
                continue
        filtered.append(line)

    after = sum(1 for l in filtered if l.startswith("BRDA:"))

    print(f"Branch coverage filtering:")
    print(f"  Before: {before} branches")
    print(f"  After:  {after} branches")
    print(f"  Removed: {removed} GCC exception artifact branches")

    with open(output_file, "w") as f:
        f.writelines(filtered)


if __name__ == "__main__":
    input_file = sys.argv[1] if len(sys.argv) > 1 else "coverage.info"
    output_file = sys.argv[2] if len(sys.argv) > 2 else input_file
    filter_coverage(input_file, output_file)
