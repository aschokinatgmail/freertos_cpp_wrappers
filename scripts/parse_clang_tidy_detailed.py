#!/usr/bin/env python3
"""
Script to parse clang-tidy detailed output and generate a formatted report with code context.
Similar to MISRA/cppcheck parsers but for clang-tidy warnings.
"""

import sys
import re
import os
from collections import defaultdict

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

def categorize_clang_tidy_check(check_name):
    """Categorize clang-tidy check by type."""
    if check_name.startswith('cppcoreguidelines-'):
        return 'C++ Core Guidelines'
    elif check_name.startswith('cert-'):
        return 'CERT Secure Coding'
    elif check_name.startswith('google-'):
        return 'Google Style'
    elif check_name.startswith('hicpp-'):
        return 'High Integrity C++'
    elif check_name.startswith('modernize-'):
        return 'Modernization'
    elif check_name.startswith('performance-'):
        return 'Performance'
    elif check_name.startswith('readability-'):
        return 'Readability'
    elif check_name.startswith('bugprone-'):
        return 'Bug Prevention'
    elif check_name.startswith('clang-analyzer-'):
        return 'Static Analysis'
    else:
        return 'Other'

def format_violation_description(check_name, message):
    """Format violation description to be more meaningful."""
    # Extract violation type from check name and message
    if 'special-member-functions' in check_name:
        return 'Reason: Missing Special Member Functions'
    elif 'rvalue-reference-param-not-moved' in check_name:
        return 'Reason: Unused RValue Reference Parameter'
    elif 'returns-a-reference' in check_name and 'constant object' in message:
        return 'Reason: Incorrect Return Type for Overloaded Operator'
    elif 'unused' in check_name or 'unused' in message.lower():
        return 'Reason: Unused Code'
    elif 'const' in check_name or 'const' in message.lower():
        return 'Reason: Const Correctness'
    elif 'move' in check_name or 'move' in message.lower():
        return 'Reason: Move Semantics'
    elif 'performance' in check_name:
        return 'Reason: Performance Optimization'
    elif 'style' in check_name or 'naming' in check_name:
        return 'Reason: Coding Style'
    elif 'security' in check_name or 'cert-' in check_name:
        return 'Reason: Security'
    else:
        return 'Reason: Code Quality'

def parse_clang_tidy_detailed_output(input_file, source_dir):
    """Parse clang-tidy detailed output and extract issues with code context."""
    if not os.path.exists(input_file):
        return None
    
    stats = {
        'total_warnings': 0,
        'total_errors': 0,
        'issues_by_category': defaultdict(int),
        'issues_by_check': defaultdict(int),
        'issues_by_file': defaultdict(int),
        'files_with_issues': set(),
        'issues_with_context': [],
        'compilation_errors': []
    }
    
    try:
        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # Parse individual warnings/errors for detailed categorization
        # Pattern: "file:line:column: warning/error: message [check-name]"
        issue_pattern = r'([^:]+):(\d+):(\d+):\s+(warning|error):\s+([^\[]+)\s+\[([^\]]+)\]'
        
        for match in re.finditer(issue_pattern, content):
            file_path = match.group(1)
            line_num = match.group(2)
            col_num = match.group(3)
            issue_type = match.group(4)
            message = match.group(5).strip()
            check_name = match.group(6)
            
            # Skip files from mocks directory and system headers
            if '/mocks/' in file_path or 'mocks/' in file_path:
                continue
            if not any(ext in file_path for ext in ['.hpp', '.cpp', '.cc', '.h']):
                continue
            if 'FreeRTOS.h' in file_path:  # Skip mock headers
                continue
                
            filename = os.path.basename(file_path)
            stats['files_with_issues'].add(filename)
            stats['issues_by_file'][filename] += 1
            
            # Count by type
            if issue_type == 'warning':
                stats['total_warnings'] += 1
            else:
                stats['total_errors'] += 1
            
            # Categorize by check type
            category = categorize_clang_tidy_check(check_name)
            stats['issues_by_category'][category] += 1
            stats['issues_by_check'][check_name] += 1
            
            # Get code context
            full_path = os.path.join(source_dir, file_path) if not os.path.isabs(file_path) else file_path
            if not os.path.exists(full_path):
                # Try alternative paths
                for possible_path in [
                    os.path.join(source_dir, filename),
                    os.path.join(source_dir, "include", filename),
                    os.path.join(source_dir, "src", filename)
                ]:
                    if os.path.exists(possible_path):
                        full_path = possible_path
                        break
            
            code_context = get_code_context(full_path, line_num)
            
            stats['issues_with_context'].append({
                'file': filename,
                'line': line_num,
                'column': col_num,
                'type': issue_type,
                'message': message,
                'check': check_name,
                'category': category,
                'context': code_context
            })
        
        # Look for compilation errors
        compilation_error_pattern = r'.*file not found|.*error:.*|.*fatal error:.*'
        for line in content.split('\n'):
            if re.search(compilation_error_pattern, line, re.IGNORECASE):
                if 'error:' in line.lower() and '[clang-diagnostic-error]' in line:
                    stats['compilation_errors'].append(line.strip())
    
    except Exception as e:
        print(f"Error parsing clang-tidy detailed output: {e}", file=sys.stderr)
        return None
    
    return stats

def print_clang_tidy_detailed_report(stats):
    """Print formatted clang-tidy detailed report to stdout."""
    
    if not stats['issues_with_context'] and not stats['compilation_errors']:
        print("No specific issues found to display with code context.")
        print()
        return
    
    # Show summary of actionable issues
    actionable_issues = len(stats['issues_with_context'])
    if actionable_issues > 0:
        print(f"### Actionable Issues with Code Context ({actionable_issues} issues)")
        print()
        
        # Group by category for better organization
        issues_by_category = defaultdict(list)
        for issue in stats['issues_with_context']:
            issues_by_category[issue['category']].append(issue)
        
        for category in sorted(issues_by_category.keys()):
            issues = issues_by_category[category]
            print(f"#### {category} ({len(issues)} issue(s))")
            print()
            
            # Group by check within category
            issues_by_check = defaultdict(list)
            for issue in issues:
                issues_by_check[issue['check']].append(issue)
            
            for check_name in sorted(issues_by_check.keys()):
                check_issues = issues_by_check[check_name]
                print(f"##### {check_name} ({len(check_issues)} issue(s))")
                print()
                
                for i, issue in enumerate(check_issues, 1):
                    print(f"**Issue {i}**: {issue['file']}:{issue['line']}:{issue['column']}")
                    violation_desc = format_violation_description(issue['check'], issue['message'])
                    print(f"*{violation_desc}*")
                    print()
                    print(f"**Message**: {issue['message']}")
                    print()
                    print("```cpp")
                    print(issue['context'])
                    print("```")
                    print()
    
    # Show compilation errors if any
    if stats['compilation_errors']:
        print("### Compilation Errors")
        print()
        print("Some files had compilation errors that prevented full analysis:")
        print()
        print("```")
        for error in stats['compilation_errors'][:10]:  # Limit to first 10 errors
            print(error)
        if len(stats['compilation_errors']) > 10:
            print(f"... and {len(stats['compilation_errors']) - 10} more errors")
        print("```")
        print()

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 parse_clang_tidy_detailed.py <input_file> <source_directory>", file=sys.stderr)
        sys.exit(1)
    
    input_file = sys.argv[1]
    source_dir = sys.argv[2]
    
    stats = parse_clang_tidy_detailed_output(input_file, source_dir)
    if stats is None:
        print("### Detailed clang-tidy Analysis")
        print()
        print("*Detailed clang-tidy analysis not available or failed to parse.*")
        print()
        sys.exit(1)
    
    print_clang_tidy_detailed_report(stats)

if __name__ == "__main__":
    main()