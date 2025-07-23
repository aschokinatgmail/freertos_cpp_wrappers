#!/usr/bin/env python3
"""
Script to parse clang-tidy output and generate statistics for the static analysis report.
"""

import sys
import re
import os
from collections import defaultdict

def parse_clang_tidy_output(input_file):
    """Parse clang-tidy output and extract statistics."""
    if not os.path.exists(input_file):
        return None
    
    stats = {
        'total_warnings': 0,
        'total_errors': 0,
        'suppressed_warnings': 0,
        'non_user_code_warnings': 0,
        'nolint_warnings': 0,
        'files_analyzed': set(),
        'check_categories': defaultdict(int),
        'issues_by_file': defaultdict(int),
        'unique_check_types': set()
    }
    
    try:
        with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # Extract total warnings and errors from the summary line
        # Pattern: "X warnings and Y errors generated."
        total_match = re.search(r'(\d+)\s+warnings?\s+and\s+(\d+)\s+errors?\s+generated', content)
        if total_match:
            stats['total_warnings'] = int(total_match.group(1))
            stats['total_errors'] = int(total_match.group(2))
        
        # Extract suppressed warnings info
        # Pattern: "Suppressed X warnings (Y in non-user code, Z NOLINT)."
        suppressed_match = re.search(r'Suppressed\s+(\d+)\s+warnings?\s+\((\d+)\s+in\s+non-user\s+code,\s+(\d+)\s+NOLINT\)', content)
        if suppressed_match:
            stats['suppressed_warnings'] = int(suppressed_match.group(1))
            stats['non_user_code_warnings'] = int(suppressed_match.group(2))
            stats['nolint_warnings'] = int(suppressed_match.group(3))
        
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
            
            # Extract filename for file analysis - exclude mocks and tests
            filename = os.path.basename(file_path)
            # Skip files from mocks directory and test files
            if '/mocks/' in file_path or 'mocks/' in file_path or 'test_' in filename or '/tests/' in file_path:
                continue
            # Only include files from src/ and include/ directories
            if not ('/src/' in file_path or '/include/' in file_path or 
                    file_path.endswith('.hpp') or file_path.endswith('.cc') or file_path.endswith('.cpp')):
                continue
            stats['files_analyzed'].add(filename)
            stats['issues_by_file'][filename] += 1
            
            # Categorize by check type
            stats['unique_check_types'].add(check_name)
            
            # Extract check category (e.g., "cppcoreguidelines" from "cppcoreguidelines-special-member-functions")
            category = check_name.split('-')[0] if '-' in check_name else check_name
            stats['check_categories'][category] += 1
        
        # Also extract files mentioned in error processing lines - exclude mocks and tests
        # Pattern: "Error while processing /path/to/file.cc"
        error_files = re.findall(r'Error while processing ([^\s]+)', content)
        for file_path in error_files:
            # Skip files from mocks directory and test files
            if '/mocks/' in file_path or 'mocks/' in file_path or 'test_' in os.path.basename(file_path) or '/tests/' in file_path:
                continue
            # Only include files from src/ and include/ directories
            if not ('/src/' in file_path or '/include/' in file_path or 
                    file_path.endswith('.hpp') or file_path.endswith('.cc') or file_path.endswith('.cpp')):
                continue
            filename = os.path.basename(file_path)
            stats['files_analyzed'].add(filename)
        
        # Extract all files that were processed - these show up as "[X/Y] Processing file /path/to/file"
        # Pattern: "[X/Y] Processing file /path/to/file.ext."
        processed_files = re.findall(r'\[\d+/\d+\] Processing file ([^.]+\.[^.]+)\.', content)
        for file_path in processed_files:
            # Skip files from mocks directory and test files
            if '/mocks/' in file_path or 'mocks/' in file_path or 'test_' in os.path.basename(file_path) or '/tests/' in file_path:
                continue
            # Only include files from src/ and include/ directories
            if not ('/src/' in file_path or '/include/' in file_path or 
                    file_path.endswith('.hpp') or file_path.endswith('.cc') or file_path.endswith('.cpp')):
                continue
            filename = os.path.basename(file_path)
            stats['files_analyzed'].add(filename)
    
    except Exception as e:
        print(f"Error parsing clang-tidy output: {e}", file=sys.stderr)
        return None
    
    return stats

def generate_statistics_markdown(stats):
    """Generate markdown statistics section."""
    if not stats:
        return "## Statistics\n\n*No statistics available - clang-tidy output could not be parsed.*\n\n"
    
    md = "## Statistics\n\n"
    
    # Overall summary
    md += "### Summary\n\n"
    md += f"- **Total Issues Found**: {stats['total_warnings'] + stats['total_errors']}\n"
    md += f"- **Warnings**: {stats['total_warnings']}\n"
    md += f"- **Errors**: {stats['total_errors']}\n"
    md += f"- **Files Analyzed**: {len(stats['files_analyzed'])}\n"
    md += f"- **Unique Check Types**: {len(stats['unique_check_types'])}\n"
    md += "\n"
    
    # Suppressed warnings info
    if stats['suppressed_warnings'] > 0:
        md += "### Suppressed Issues\n\n"
        md += f"- **Total Suppressed**: {stats['suppressed_warnings']}\n"
        md += f"- **Non-user Code**: {stats['non_user_code_warnings']}\n"
        md += f"- **NOLINT Directives**: {stats['nolint_warnings']}\n"
        md += "\n"
    
    # Issues by check category
    if stats['check_categories']:
        md += "### Issues by Check Category\n\n"
        # Sort categories by count (descending)
        sorted_categories = sorted(stats['check_categories'].items(), key=lambda x: x[1], reverse=True)
        for category, count in sorted_categories:
            md += f"- **{category}**: {count} issues\n"
        md += "\n"
    
    # Files with issues
    if stats['issues_by_file']:
        md += "### Files with Issues\n\n"
        # Sort files by issue count (descending)
        sorted_files = sorted(stats['issues_by_file'].items(), key=lambda x: x[1], reverse=True)
        for filename, count in sorted_files:
            md += f"- **{filename}**: {count} issues\n"
        md += "\n"
    
    # All analyzed files
    if stats['files_analyzed']:
        md += "### Analyzed Files\n\n"
        for filename in sorted(stats['files_analyzed']):
            md += f"- {filename}\n"
        md += "\n"
    
    return md

def main():
    """Main function."""
    if len(sys.argv) != 3:
        print("Usage: parse_clang_tidy_stats.py <input_file> <output_file>", file=sys.stderr)
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    # Parse the clang-tidy output
    stats = parse_clang_tidy_output(input_file)
    
    # Generate statistics markdown
    stats_md = generate_statistics_markdown(stats)
    
    # Write to output file
    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(stats_md)
        print(f"Statistics written to: {output_file}")
    except Exception as e:
        print(f"Error writing statistics: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()