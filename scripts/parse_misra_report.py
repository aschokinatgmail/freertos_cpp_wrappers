#!/usr/bin/env python3
"""
Script to parse MISRA C++ static analysis output and generate a formatted report with code context.
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

def parse_misra_output(input_file, source_dir):
    """Parse MISRA analysis output and extract statistics with code context."""
    if not os.path.exists(input_file):
        return None
    
    stats = {
        'total_violations': 0,
        'violations_by_rule': defaultdict(int),
        'violations_by_file': defaultdict(int),
        'violations_by_severity': defaultdict(int),
        'files_analyzed': set(),
        'misra_rules': set(),
        'violations_with_context': [],
        'other_issues': 0,
        'analysis_errors': []
    }
    
    try:
        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        
        for line in lines:
            line = line.strip()
            if not line:
                continue
                
            # Parse issue lines: file:line:column: severity: message [id]
            match = re.match(r'^([^:]+):(\d+):(\d+):\s+(\w+):\s+(.+?)\s+\[([^\]]+)\]$', line)
            if match:
                file_path, line_num, col_num, severity, message, check_id = match.groups()
                
                # Extract file name from path
                file_name = os.path.basename(file_path)
                stats['files_analyzed'].add(file_name)
                
                # Check if this is a MISRA violation
                if 'misra-c2012-' in check_id:
                    stats['total_violations'] += 1
                    stats['violations_by_rule'][check_id] += 1
                    stats['violations_by_file'][file_name] += 1
                    stats['violations_by_severity'][severity] += 1
                    stats['misra_rules'].add(check_id)
                    
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
                    
                    stats['violations_with_context'].append({
                        'file': file_name,
                        'line': line_num,
                        'column': col_num,
                        'severity': severity,
                        'message': message,
                        'rule': check_id,
                        'context': code_context
                    })
                elif 'internalError' in check_id or 'error:' in line.lower():
                    stats['analysis_errors'].append(line)
                else:
                    stats['other_issues'] += 1
        
    except Exception as e:
        print(f"Error parsing MISRA output: {e}", file=sys.stderr)
        return None
    
    return stats

def print_misra_report(stats):
    """Print formatted MISRA report to stdout."""
    
    print("### Summary")
    print()
    print(f"- **Total MISRA Violations**: {stats['total_violations']}")
    print(f"- **Unique Rules Violated**: {len(stats['misra_rules'])}")
    print(f"- **Files Analyzed**: {len(stats['files_analyzed'])}")
    print(f"- **Analysis Errors**: {len(stats['analysis_errors'])}")
    print(f"- **Other Issues**: {stats['other_issues']}")
    print()
    
    if stats['violations_by_severity']:
        print("### Violations by Severity")
        print()
        for severity, count in sorted(stats['violations_by_severity'].items()):
            print(f"- **{severity.title()}**: {count}")
        print()
    
    if stats['violations_by_rule']:
        print("### Violations by Rule")
        print()
        for rule, count in sorted(stats['violations_by_rule'].items()):
            rule_num = rule.replace('misra-c2012-', '')
            print(f"- **MISRA C 2012 Rule {rule_num}**: {count} violation(s)")
        print()
    
    if stats['violations_by_file']:
        print("### Violations by File")
        print()
        for file_name, count in sorted(stats['violations_by_file'].items()):
            print(f"- **{file_name}**: {count} violation(s)")
        print()
    
    if stats['violations_with_context']:
        print("### Detailed Violations with Code Context")
        print()
        
        # Group by rule for better organization
        violations_by_rule = defaultdict(list)
        for violation in stats['violations_with_context']:
            violations_by_rule[violation['rule']].append(violation)
        
        for rule in sorted(violations_by_rule.keys()):
            rule_num = rule.replace('misra-c2012-', '')
            violations = violations_by_rule[rule]
            print(f"#### MISRA C 2012 Rule {rule_num} ({len(violations)} violation(s))")
            print()
            
            for i, violation in enumerate(violations, 1):
                print(f"**Violation {i}**: {violation['file']}:{violation['line']}:{violation['column']}")
                print(f"*{violation['severity'].title()}*: {violation['message']}")
                print()
                print("```cpp")
                print(violation['context'])
                print("```")
                print()
    
    if stats['analysis_errors']:
        print("### Analysis Errors")
        print()
        print("Some files could not be fully analyzed:")
        print()
        print("```")
        for error in stats['analysis_errors']:
            print(error)
        print("```")
        print()
    
    print("### Analysis Notes")
    print()
    print("- **Standard**: MISRA C 2012 (applicable rules for C++)")
    print("- **Tool**: cppcheck with MISRA addon")
    print("- **Scope**: Library modules only (src/, include/)")
    print("- **Rule texts**: Cannot be displayed due to MISRA licensing restrictions")
    print("- **Compatibility**: Many MISRA C 2012 rules overlap with MISRA C++ 2008 requirements")
    print("- **Integration**: This analysis complements the existing clang-tidy static analysis")
    print()

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 parse_misra_report.py <input_file> <source_directory>", file=sys.stderr)
        sys.exit(1)
    
    input_file = sys.argv[1]
    source_dir = sys.argv[2]
    
    stats = parse_misra_output(input_file, source_dir)
    if stats is None:
        print("### MISRA Analysis")
        print()
        print("*MISRA analysis not available or failed to parse.*")
        print()
        sys.exit(1)
    
    print_misra_report(stats)

if __name__ == "__main__":
    main()