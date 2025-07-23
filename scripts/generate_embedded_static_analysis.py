#!/usr/bin/env python3
"""
Script to generate embedded static analysis section with actual MISRA and clang-tidy results
"""

import sys
import os
import re
from collections import defaultdict

def parse_misra_violations(misra_file, source_dir):
    """Parse MISRA violations from cppcheck output"""
    violations = []
    rule_counts = defaultdict(int)
    file_counts = defaultdict(int)
    
    if not os.path.exists(misra_file):
        return violations, rule_counts, file_counts
    
    with open(misra_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
                
            # Parse cppcheck format: file:line:column: severity: message [id]
            match = re.match(r'^([^:]+):(\d+):(\d+):\s*(\w+):\s*(.+?)\s*\[([^\]]+)\]', line)
            if match:
                file_path = match.group(1)
                line_num = int(match.group(2))
                severity = match.group(4)
                message = match.group(5)
                rule_id = match.group(6)
                
                # Only include MISRA violations
                if 'misra' in rule_id.lower() or 'MISRA' in message:
                    # Extract rule number from message or ID
                    rule_match = re.search(r'MISRA\s+C\s+\d+\s+Rule\s+([\d.]+)', message)
                    if rule_match:
                        rule_num = rule_match.group(1)
                    else:
                        rule_num = rule_id
                    
                    violations.append({
                        'file': file_path,
                        'line': line_num,
                        'severity': severity,
                        'message': message,
                        'rule': rule_num
                    })
                    
                    rule_counts[rule_num] += 1
                    file_counts[os.path.basename(file_path)] += 1
    
    return violations, rule_counts, file_counts

def parse_clang_tidy_violations(clang_tidy_file, source_dir):
    """Parse clang-tidy violations"""
    violations = []
    
    if not os.path.exists(clang_tidy_file):
        return violations
    
    with open(clang_tidy_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
                
            # Parse clang-tidy format: file:line:column: severity: message [check-name]
            match = re.match(r'^([^:]+):(\d+):(\d+):\s*(\w+):\s*(.+?)\s*\[([^\]]+)\]', line)
            if match:
                file_path = match.group(1)
                line_num = int(match.group(2))
                severity = match.group(4)
                message = match.group(5)
                check = match.group(6)
                
                violations.append({
                    'file': file_path,
                    'line': line_num,
                    'severity': severity,
                    'message': message,
                    'check': check
                })
    
    return violations

def get_code_context(file_path, line_num, context_lines=3):
    """Get code context around a specific line"""
    try:
        with open(file_path, 'r') as f:
            lines = f.readlines()
        
        start = max(0, line_num - context_lines - 1)
        end = min(len(lines), line_num + context_lines)
        
        context = []
        for i in range(start, end):
            line_no = i + 1
            line_content = lines[i].rstrip()
            marker = ">>> " if line_no == line_num else "     "
            context.append(f"{marker}{line_no:3d}: {line_content}")
        
        return "\n".join(context)
    except:
        return f"     {line_num}: [Code context unavailable]"

def generate_static_analysis_report(clang_tidy_file, misra_file, source_dir, output_file):
    """Generate the embedded static analysis section"""
    
    # Parse violations
    misra_violations, misra_rule_counts, misra_file_counts = parse_misra_violations(misra_file, source_dir)
    clang_tidy_violations = parse_clang_tidy_violations(clang_tidy_file, source_dir)
    
    with open(output_file, 'w') as f:
        f.write("## Static Code Analysis\n\n")
        f.write("### Overview\n\n")
        f.write("**Static Analysis Tools**: clang-tidy + cppcheck (MISRA C++ 2012)\n")
        f.write("**Analysis Scope**: Main project code only (src/ and include/ directories)\n")
        f.write("**Excluded**: Test harness, mocks, and external dependencies\n\n")
        
        f.write("### Combined Static Analysis Results\n\n")
        f.write("#### Summary\n\n")
        
        # Summary stats
        total_misra = len(misra_violations)
        total_clang_tidy = len(clang_tidy_violations)
        
        f.write(f"- **MISRA C++ 2012 Violations**: {total_misra}\n")
        f.write(f"- **clang-tidy Issues**: {total_clang_tidy}\n")
        f.write(f"- **Total Issues Found**: {total_misra + total_clang_tidy}\n\n")
        
        if total_misra > 0:
            f.write("#### MISRA C++ 2012 Analysis\n\n")
            f.write("**Violations by Rule:**\n\n")
            
            # Sort rules by count
            sorted_rules = sorted(misra_rule_counts.items(), key=lambda x: x[1], reverse=True)
            for rule, count in sorted_rules:
                f.write(f"- **MISRA C 2012 Rule {rule}**: {count} violation(s)\n")
            
            f.write("\n**Violations by File:**\n\n")
            sorted_files = sorted(misra_file_counts.items(), key=lambda x: x[1], reverse=True)
            for filename, count in sorted_files:
                f.write(f"- **{filename}**: {count} violation(s)\n")
            
            # Show top 10 MISRA violations with code context
            f.write("\n**Detailed MISRA Violations (Top 10):**\n\n")
            for i, violation in enumerate(misra_violations[:10]):
                f.write(f"**Violation {i+1}**: {violation['file']}:{violation['line']}\n")
                f.write(f"*Rule*: {violation['rule']}\n")
                f.write(f"*Severity*: {violation['severity']}\n")
                f.write(f"*Message*: {violation['message']}\n\n")
                f.write("```cpp\n")
                f.write(get_code_context(violation['file'], violation['line']))
                f.write("\n```\n\n")
        
        if total_clang_tidy > 0:
            f.write("#### clang-tidy Analysis\n\n")
            
            # Group by check type
            check_counts = defaultdict(int)
            for violation in clang_tidy_violations:
                check_counts[violation['check']] += 1
            
            f.write("**Issues by Check Type:**\n\n")
            sorted_checks = sorted(check_counts.items(), key=lambda x: x[1], reverse=True)
            for check, count in sorted_checks[:15]:  # Top 15 check types
                f.write(f"- **{check}**: {count} issue(s)\n")
            
            # Show top 10 clang-tidy violations with code context  
            f.write("\n**Detailed clang-tidy Issues (Top 10):**\n\n")
            for i, violation in enumerate(clang_tidy_violations[:10]):
                f.write(f"**Issue {i+1}**: {violation['file']}:{violation['line']}\n")
                f.write(f"*Check*: {violation['check']}\n")
                f.write(f"*Severity*: {violation['severity']}\n")
                f.write(f"*Message*: {violation['message']}\n\n")
                f.write("```cpp\n")
                f.write(get_code_context(violation['file'], violation['line']))
                f.write("\n```\n\n")
        
        if total_misra == 0 and total_clang_tidy == 0:
            f.write("#### Analysis Results\n\n")
            f.write("✅ **No static analysis issues found!**\n\n")
            f.write("All analyzed files passed MISRA C++ 2012 and clang-tidy checks.\n\n")

if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage: generate_embedded_static_analysis.py <clang_tidy_file> <misra_file> <source_dir> <output_file>")
        sys.exit(1)
    
    clang_tidy_file = sys.argv[1]
    misra_file = sys.argv[2]
    source_dir = sys.argv[3]
    output_file = sys.argv[4]
    
    generate_static_analysis_report(clang_tidy_file, misra_file, source_dir, output_file)
    print(f"Embedded static analysis section generated: {output_file}")