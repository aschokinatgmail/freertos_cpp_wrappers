#!/usr/bin/env python3
"""
Script to parse enhanced cppcheck static analysis output (all rules) and generate a formatted report with code context.
This includes MISRA rules as well as all other cppcheck checks (style, performance, portability, etc.).
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

def categorize_rule(check_id):
    """Categorize cppcheck rule by type."""
    if 'misra-c2012-' in check_id:
        return 'MISRA C 2012'
    elif check_id in ['unusedFunction', 'unusedStructMember', 'unusedVariable', 'unreadVariable']:
        return 'Unused Code'
    elif check_id in ['constParameter', 'constParameterReference', 'constParameterCallback']:
        return 'Const Correctness'
    elif check_id in ['passedByValue', 'postfixOperator', 'useStlAlgorithm']:
        return 'Performance'
    elif check_id in ['cstyleCast', 'variableScope', 'redundantAssignment']:
        return 'Style'
    elif 'cert-' in check_id or 'security' in check_id.lower():
        return 'Security'
    elif 'portability' in check_id.lower():
        return 'Portability'
    else:
        return 'Other'

def parse_enhanced_cppcheck_output(input_file, source_dir):
    """Parse enhanced cppcheck analysis output and extract statistics with code context."""
    if not os.path.exists(input_file):
        return None
    
    stats = {
        'total_violations': 0,
        'violations_by_rule': defaultdict(int),
        'violations_by_file': defaultdict(int),
        'violations_by_severity': defaultdict(int),
        'violations_by_category': defaultdict(int),
        'files_analyzed': set(),
        'rules': set(),
        'violations_with_context': [],
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
            match = re.match(r'^(.+?):(\d+):(\d+):\s+(\w+):\s+(.+?)\s+\[([^\]]+)\]$', line)
            if match:
                file_path, line_num, col_num, severity, message, check_id = match.groups()
                
                # Extract file name from path
                file_name = os.path.basename(file_path)
                stats['files_analyzed'].add(file_name)
                
                # Skip purely informational messages that aren't actionable violations
                if severity == 'information' and check_id in ['missingIncludeSystem', 'toomanyconfigs', 'checkersReport']:
                    continue
                
                # Process all violations (not just MISRA)
                if not ('internalError' in check_id or 'error:' in line.lower()):
                    stats['total_violations'] += 1
                    stats['violations_by_rule'][check_id] += 1
                    stats['violations_by_file'][file_name] += 1
                    stats['violations_by_severity'][severity] += 1
                    
                    # Categorize the rule
                    category = categorize_rule(check_id)
                    stats['violations_by_category'][category] += 1
                    
                    stats['rules'].add(check_id)
                    
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
                        'category': category,
                        'context': code_context
                    })
                else:
                    stats['analysis_errors'].append(line)
        
    except Exception as e:
        print(f"Error parsing enhanced cppcheck output: {e}", file=sys.stderr)
        return None
    
    return stats

def print_enhanced_cppcheck_report(stats):
    """Print formatted enhanced cppcheck report to stdout."""
    
    print("### Summary")
    print()
    print(f"- **Total Violations**: {stats['total_violations']}")
    print(f"- **Unique Rules Violated**: {len(stats['rules'])}")
    print(f"- **Files Analyzed**: {len(stats['files_analyzed'])}")
    print(f"- **Analysis Errors**: {len(stats['analysis_errors'])}")
    print()
    
    if stats['violations_by_severity']:
        print("### Violations by Severity")
        print()
        for severity, count in sorted(stats['violations_by_severity'].items()):
            print(f"- **{severity.title()}**: {count}")
        print()
    
    if stats['violations_by_category']:
        print("### Violations by Category")
        print()
        for category, count in sorted(stats['violations_by_category'].items(), key=lambda x: x[1], reverse=True):
            print(f"- **{category}**: {count} violation(s)")
        print()
    
    if stats['violations_by_rule']:
        print("### Violations by Rule")
        print()
        for rule, count in sorted(stats['violations_by_rule'].items()):
            category = categorize_rule(rule)
            if 'misra-c2012-' in rule:
                rule_display = f"MISRA C 2012 Rule {rule.replace('misra-c2012-', '')}"
            else:
                rule_display = rule
            print(f"- **{rule_display}** ({category}): {count} violation(s)")
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
        
        # Group by category for better organization
        violations_by_category = defaultdict(list)
        for violation in stats['violations_with_context']:
            violations_by_category[violation['category']].append(violation)
        
        for category in sorted(violations_by_category.keys()):
            violations = violations_by_category[category]
            print(f"#### {category} ({len(violations)} violation(s))")
            print()
            
            # Sort violations within category by rule
            violations.sort(key=lambda x: x['rule'])
            
            current_rule = None
            rule_count = 0
            
            for violation in violations:
                if violation['rule'] != current_rule:
                    current_rule = violation['rule']
                    rule_count = 0
                    if 'misra-c2012-' in current_rule:
                        rule_display = f"MISRA C 2012 Rule {current_rule.replace('misra-c2012-', '')}"
                    else:
                        rule_display = current_rule
                    print(f"##### {rule_display}")
                    print()
                
                rule_count += 1
                print(f"**Violation {rule_count}**: {violation['file']}:{violation['line']}:{violation['column']}")
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
    print("- **Tool**: cppcheck with all rules enabled (--enable=all)")
    print("- **Checks**: All available cppcheck checks including MISRA C 2012, style, performance, portability, security")
    print("- **Scope**: Library modules only (src/, include/)")
    print("- **Integration**: This analysis complements the existing clang-tidy static analysis")
    print("- **MISRA Compliance**: MISRA rule texts cannot be displayed due to licensing restrictions")
    print()

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 parse_enhanced_cppcheck_report.py <input_file> <source_directory>", file=sys.stderr)
        sys.exit(1)
    
    input_file = sys.argv[1]
    source_dir = sys.argv[2]
    
    stats = parse_enhanced_cppcheck_output(input_file, source_dir)
    if stats is None:
        print("### Enhanced cppcheck Analysis")
        print()
        print("*Enhanced cppcheck analysis not available or failed to parse.*")
        print()
        sys.exit(1)
    
    print_enhanced_cppcheck_report(stats)

if __name__ == "__main__":
    main()