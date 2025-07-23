#!/usr/bin/env python3
"""
Script to parse clang-tidy output and generate detailed violation report with code context.
"""

import sys
import re
import os
from collections import defaultdict
from datetime import datetime

def get_code_context(file_path, line_number, context_lines=3):
    """Get code context around a specific line."""
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        
        line_num = int(line_number)
        start = max(1, line_num - context_lines)
        end = min(len(lines), line_num + context_lines)
        
        context = []
        for i in range(start - 1, end):  # Convert to 0-based indexing
            line_marker = ">>> " if (i + 1) == line_num else "    "
            context.append(f"{line_marker}{i + 1:4d}: {lines[i].rstrip()}")
        
        return "\n".join(context)
    except Exception:
        return f"    {line_number}: [Code context unavailable]"

def parse_clang_tidy_violations(input_file, source_dir):
    """Parse clang-tidy output and extract detailed violations with code context."""
    if not os.path.exists(input_file):
        return None
    
    violations = []
    stats = {
        'total_violations': 0,
        'violations_by_check': defaultdict(int),
        'violations_by_file': defaultdict(int),
        'violations_by_severity': defaultdict(int),
        'check_descriptions': {}
    }
    
    # Common check descriptions
    check_descriptions = {
        'cppcoreguidelines-special-member-functions': 'Define special member functions (copy/move constructors, destructors, assignment operators) appropriately',
        'cppcoreguidelines-pro-type-vararg': 'Avoid using variable argument functions',
        'cert-err58-cpp': 'Construction of objects with static storage duration must not throw exceptions',
        'google-explicit-constructor': 'Single-argument constructors should be marked explicit',
        'hicpp-special-member-functions': 'Define special member functions appropriately (HIC++ variant)',
        'readability-identifier-naming': 'Follow consistent naming conventions for identifiers',
        'modernize-use-override': 'Use override keyword for virtual function overrides',
        'performance-unnecessary-copy-initialization': 'Avoid unnecessary copies in variable initialization'
    }
    
    try:
        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # Parse individual violations
        # Pattern: "file:line:column: warning/error: message [check-name]"
        issue_pattern = r'([^:]+):(\d+):(\d+):\s+(warning|error):\s+([^\[]+)\s+\[([^\]]+)\]'
        
        for match in re.finditer(issue_pattern, content):
            file_path = match.group(1)
            line_num = match.group(2)
            col_num = match.group(3)
            severity = match.group(4)
            message = match.group(5).strip()
            check_name = match.group(6)
            
            # Skip violations from test files and mocks
            if '/mocks/' in file_path or 'test_' in os.path.basename(file_path) or '/tests/' in file_path:
                continue
            
            # Only include violations from src/ and include/ directories
            if not ('/src/' in file_path or '/include/' in file_path or 
                    file_path.endswith('.hpp') or file_path.endswith('.cc') or file_path.endswith('.cpp')):
                continue
            
            file_name = os.path.basename(file_path)
            
            # Get code context
            full_path = os.path.join(source_dir, file_path) if not os.path.isabs(file_path) else file_path
            if not os.path.exists(full_path):
                # Try alternative paths
                for possible_path in [
                    os.path.join(source_dir, file_name),
                    os.path.join(source_dir, "include", file_name),
                    os.path.join(source_dir, "src", file_name)
                ]:
                    if os.path.exists(possible_path):
                        full_path = possible_path
                        break
            
            code_context = get_code_context(full_path, line_num)
            
            violations.append({
                'file': file_name,
                'line': line_num,
                'column': col_num,
                'severity': severity,
                'message': message,
                'check': check_name,
                'context': code_context
            })
            
            # Update statistics
            stats['total_violations'] += 1
            stats['violations_by_check'][check_name] += 1
            stats['violations_by_file'][file_name] += 1
            stats['violations_by_severity'][severity] += 1
            
            # Store check description if available
            if check_name in check_descriptions:
                stats['check_descriptions'][check_name] = check_descriptions[check_name]
    
    except Exception as e:
        print(f"Error parsing clang-tidy violations: {e}", file=sys.stderr)
        return None
    
    return violations, stats

def print_clang_tidy_violations_report(violations, stats):
    """Print formatted clang-tidy violations report to stdout."""
    
    if not violations:
        print("### Detailed clang-tidy Analysis")
        print()
        print("No clang-tidy violations found in main project code (src/ and include/ directories).")
        print()
        return
    
    print("### Detailed clang-tidy Analysis")
    print()
    
    # Group violations by check type for better organization
    violations_by_check = defaultdict(list)
    for violation in violations:
        violations_by_check[violation['check']].append(violation)
    
    for check_name in sorted(violations_by_check.keys()):
        check_violations = violations_by_check[check_name]
        print(f"#### {check_name} ({len(check_violations)} violation(s))")
        print()
        
        # Add check description if available
        if check_name in stats['check_descriptions']:
            print(f"**Description**: {stats['check_descriptions'][check_name]}")
            print()
        
        for i, violation in enumerate(check_violations, 1):
            print(f"**Violation {i}**: {violation['file']}:{violation['line']}:{violation['column']}")
            print(f"*Severity*: {violation['severity'].title()}")
            print(f"*Message*: {violation['message']}")
            print()
            print("```cpp")
            print(violation['context'])
            print("```")
            print()

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 parse_clang_tidy_violations.py <input_file> <source_directory>", file=sys.stderr)
        sys.exit(1)
    
    input_file = sys.argv[1]
    source_dir = sys.argv[2]
    
    result = parse_clang_tidy_violations(input_file, source_dir)
    if result is None:
        print("### Detailed clang-tidy Analysis")
        print()
        print("*clang-tidy analysis not available or failed to parse.*")
        print()
        sys.exit(1)
    
    violations, stats = result
    print_clang_tidy_violations_report(violations, stats)

if __name__ == "__main__":
    main()