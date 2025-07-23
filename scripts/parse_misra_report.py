#!/usr/bin/env python3
"""
Script to parse MISRA C++ static analysis output and generate a formatted report.
"""

import sys
import re
import os
from collections import defaultdict
from datetime import datetime

def parse_misra_output(input_file):
    """Parse MISRA analysis output and extract statistics."""
    if not os.path.exists(input_file):
        return None
    
    stats = {
        'total_violations': 0,
        'violations_by_rule': defaultdict(int),
        'violations_by_file': defaultdict(int),
        'violations_by_severity': defaultdict(int),
        'files_analyzed': set(),
        'misra_rules': set(),
        'other_issues': [],
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
                elif 'internalError' in check_id or 'error:' in line.lower():
                    stats['analysis_errors'].append(line)
                else:
                    stats['other_issues'].append(line)
        
    except Exception as e:
        print(f"Error parsing MISRA output: {e}", file=sys.stderr)
        return None
    
    return stats

def generate_misra_report(stats, output_file):
    """Generate formatted MISRA report."""
    
    with open(output_file, 'w') as f:
        f.write("# MISRA C++ Static Analysis Report\n\n")
        
        f.write("## Overview\n\n")
        f.write("**Static Analysis Tool**: cppcheck with MISRA addon\n")
        f.write("**Analysis Scope**: Library modules only - src/ include/\n")
        f.write("**Standard**: MISRA C 2012 (applicable rules for C++)\n")
        f.write("**Note**: This analysis uses MISRA C 2012 rules as the foundation, ")
        f.write("which overlap significantly with MISRA C++ 2008.\n\n")
        
        f.write("## Statistics\n\n")
        f.write("### Summary\n\n")
        f.write(f"- **Total MISRA Violations**: {stats['total_violations']}\n")
        f.write(f"- **Unique Rules Violated**: {len(stats['misra_rules'])}\n")
        f.write(f"- **Files Analyzed**: {len(stats['files_analyzed'])}\n")
        f.write(f"- **Analysis Errors**: {len(stats['analysis_errors'])}\n\n")
        
        if stats['violations_by_severity']:
            f.write("### Violations by Severity\n\n")
            for severity, count in sorted(stats['violations_by_severity'].items()):
                f.write(f"- **{severity.title()}**: {count}\n")
            f.write("\n")
        
        if stats['violations_by_rule']:
            f.write("### Violations by Rule\n\n")
            for rule, count in sorted(stats['violations_by_rule'].items()):
                rule_num = rule.replace('misra-c2012-', '')
                f.write(f"- **MISRA C 2012 Rule {rule_num}**: {count} violation(s)\n")
            f.write("\n")
        
        if stats['violations_by_file']:
            f.write("### Violations by File\n\n")
            for file_name, count in sorted(stats['violations_by_file'].items()):
                f.write(f"- **{file_name}**: {count} violation(s)\n")
            f.write("\n")
        
        if stats['files_analyzed']:
            f.write("### Analyzed Files\n\n")
            for file_name in sorted(stats['files_analyzed']):
                f.write(f"- {file_name}\n")
            f.write("\n")
        
        f.write("## Analysis Notes\n\n")
        f.write("### MISRA C++ 2008 vs MISRA C 2012\n\n")
        f.write("This analysis uses the MISRA C 2012 standard as implemented by cppcheck's MISRA addon. ")
        f.write("While specifically designed for C, many MISRA C 2012 rules are directly applicable to C++ ")
        f.write("and overlap with MISRA C++ 2008 requirements:\n\n")
        f.write("- **Rule 5.8**: External identifier naming conflicts\n")
        f.write("- **Rule 8.6**: Function declaration consistency\n")
        f.write("- **Rule 8.7**: Objects with external linkage\n")
        f.write("- Additional rules covering similar code quality aspects\n\n")
        
        f.write("### Analysis Limitations\n\n")
        f.write("- Rule texts cannot be displayed due to MISRA licensing restrictions\n")
        f.write("- Some complex template and C++17 constructs may not be fully analyzed\n")
        f.write("- FreeRTOS-specific configurations may trigger false positives\n")
        f.write("- This analysis complements the existing clang-tidy static analysis\n\n")
        
        if stats['analysis_errors']:
            f.write("### Analysis Errors\n\n")
            f.write("Some files could not be fully analyzed:\n\n")
            f.write("```\n")
            for error in stats['analysis_errors']:
                f.write(f"{error}\n")
            f.write("```\n\n")
        
        f.write("---\n")
        f.write(f"*Generated: {datetime.now().strftime('%B %d, %Y')}*\n")
        f.write("*Tool: cppcheck with MISRA addon*\n")
        f.write("*Scope: Library modules only*\n")

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 parse_misra_report.py <input_file> <output_file>", file=sys.stderr)
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    stats = parse_misra_output(input_file)
    if stats is None:
        print("Failed to parse MISRA output", file=sys.stderr)
        sys.exit(1)
    
    generate_misra_report(stats, output_file)
    print(f"MISRA report generated: {output_file}")

if __name__ == "__main__":
    main()