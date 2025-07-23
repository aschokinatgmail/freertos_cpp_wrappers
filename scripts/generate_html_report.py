#!/usr/bin/env python3
"""
Generate HTML report from Markdown with Apple Developer styling
"""

import sys
import re
import markdown
from datetime import datetime
import os


def parse_markdown_to_html_content(md_file):
    """Convert markdown to HTML content for the template"""
    with open(md_file, 'r') as f:
        content = f.read()
    
    # Convert markdown to HTML
    md = markdown.Markdown(extensions=['tables', 'fenced_code', 'toc'])
    html_content = md.convert(content)
    
    # Extract title from the first heading
    title_match = re.search(r'<h1[^>]*>(.*?)</h1>', html_content)
    title = title_match.group(1) if title_match else "Report"
    
    return title, html_content


def create_summary_cards(content):
    """Extract key metrics and create summary cards"""
    cards_html = '<div class="summary-cards">'
    
    # Look for statistics section
    stats_match = re.search(r'<h2[^>]*>Statistics</h2>(.*?)(?=<h2|$)', content, re.DOTALL)
    if stats_match:
        stats_content = stats_match.group(1)
        
        # Extract metrics
        metrics = {
            'Total Issues Found': ('metric', 'Issues'),
            'Warnings': ('metric warning', 'Warnings'),
            'Errors': ('metric error', 'Errors'),
            'Files Analyzed': ('metric', 'Files'),
            'Total Tests Executed': ('metric', 'Tests'),
            'Passed': ('metric success', 'Passed'),
            'Failed': ('metric error', 'Failed'),
            'Success Rate': ('metric success', 'Success'),
            'Line Coverage': ('metric', 'Coverage'),
            'Function Coverage': ('metric', 'Coverage')
        }
        
        for metric_name, (css_class, label) in metrics.items():
            # Look for the metric in the content
            pattern = rf'<strong>{re.escape(metric_name)}</strong>:?\s*(\d+(?:\.\d+)?%?)'
            match = re.search(pattern, stats_content)
            if match:
                value = match.group(1)
                cards_html += f'''
                <div class="card">
                    <h3>{label}</h3>
                    <div class="{css_class}">{value}</div>
                </div>
                '''
    
    cards_html += '</div>'
    return cards_html


def apply_custom_styling(content):
    """Apply custom styling to specific elements"""
    # Wrap code blocks with custom styling
    content = re.sub(r'<pre><code>', '<div class="code-block"><pre><code>', content)
    content = re.sub(r'</code></pre>', '</code></pre></div>', content)
    
    # Add section wrapper to h2 elements and their content
    sections = re.split(r'(<h2[^>]*>.*?</h2>)', content)
    new_content = ''
    
    for i, section in enumerate(sections):
        if section.startswith('<h2'):
            # This is a heading, start a new section
            new_content += '<div class="section">' + section
        elif i > 0 and sections[i-1].startswith('<h2'):
            # This is content following a heading
            new_content += section + '</div>'
        else:
            # This is content before any h2 heading
            new_content += section
    
    return new_content


def generate_html_report(title, md_file, html_file, template_file):
    """Generate HTML report from markdown using the template"""
    
    # Parse markdown
    page_title, content = parse_markdown_to_html_content(md_file)
    
    # Create summary cards
    summary_cards = create_summary_cards(content)
    
    # Apply custom styling
    content = apply_custom_styling(content)
    
    # Combine summary cards with content
    full_content = summary_cards + content
    
    # Read template
    with open(template_file, 'r') as f:
        template = f.read()
    
    # Determine subtitle based on report type
    if 'static analysis' in title.lower():
        subtitle = 'Automated code quality analysis using clang-tidy'
    elif 'validation' in title.lower() or 'verification' in title.lower():
        subtitle = 'Comprehensive test execution and coverage analysis'
    else:
        subtitle = 'Quality assurance report for FreeRTOS C++ Wrappers'
    
    # Replace template variables
    html_output = template.replace('{{TITLE}}', page_title)
    html_output = html_output.replace('{{SUBTITLE}}', subtitle)
    html_output = html_output.replace('{{CONTENT}}', full_content)
    html_output = html_output.replace('{{TIMESTAMP}}', datetime.now().strftime('%B %d, %Y at %I:%M %p'))
    
    # Write HTML file
    with open(html_file, 'w') as f:
        f.write(html_output)


if __name__ == '__main__':
    if len(sys.argv) != 5:
        print("Usage: generate_html_report.py <title> <markdown_file> <html_file> <template_file>")
        sys.exit(1)
    
    title = sys.argv[1]
    md_file = sys.argv[2]
    html_file = sys.argv[3]
    template_file = sys.argv[4]
    
    try:
        generate_html_report(title, md_file, html_file, template_file)
        print(f"HTML report generated: {html_file}")
    except Exception as e:
        print(f"Error generating HTML report: {e}")
        sys.exit(1)