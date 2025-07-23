#!/usr/bin/env python3
"""
Simple HTML generator for the combined validation report
"""

import re
import sys

def markdown_to_html(markdown_content):
    """Convert basic markdown to HTML."""
    html_content = []
    lines = markdown_content.split('\n')
    in_code_block = False
    in_list = False
    
    for line in lines:
        if line.strip().startswith('```'):
            if in_code_block:
                html_content.append('</pre>')
                in_code_block = False
            else:
                html_content.append('<pre><code>')
                in_code_block = True
            continue
            
        if in_code_block:
            html_content.append(line)
            continue
            
        # Handle headers
        if line.startswith('# '):
            html_content.append(f'<h1>{line[2:]}</h1>')
        elif line.startswith('## '):
            html_content.append(f'<h2>{line[3:]}</h2>')
        elif line.startswith('### '):
            html_content.append(f'<h3>{line[4:]}</h3>')
        elif line.startswith('#### '):
            html_content.append(f'<h4>{line[5:]}</h4>')
        # Handle bold text
        elif '**' in line:
            line = re.sub(r'\*\*(.*?)\*\*', r'<strong>\1</strong>', line)
            html_content.append(f'<p>{line}</p>')
        # Handle lists
        elif line.startswith('- '):
            if not in_list:
                html_content.append('<ul>')
                in_list = True
            item = line[2:]
            item = re.sub(r'\*\*(.*?)\*\*', r'<strong>\1</strong>', item)
            html_content.append(f'<li>{item}</li>')
        else:
            if in_list:
                html_content.append('</ul>')
                in_list = False
            if line.strip():
                line = re.sub(r'\*\*(.*?)\*\*', r'<strong>\1</strong>', line)
                html_content.append(f'<p>{line}</p>')
            else:
                html_content.append('<br>')
    
    if in_list:
        html_content.append('</ul>')
    if in_code_block:
        html_content.append('</pre>')
    
    return '\n'.join(html_content)

def create_html_report(markdown_file, output_file):
    """Create HTML report from markdown file."""
    try:
        with open(markdown_file, 'r', encoding='utf-8') as f:
            markdown_content = f.read()
    except FileNotFoundError:
        print(f"Error: File {markdown_file} not found")
        return False
    
    html_body = markdown_to_html(markdown_content)
    
    html_template = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Combined Validation and Verification Report</title>
    <style>
        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            line-height: 1.6;
            margin: 0;
            padding: 20px;
            background-color: #f8f9fa;
            color: #333;
        }}
        .container {{
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            padding: 30px;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }}
        h1 {{
            color: #2c3e50;
            border-bottom: 3px solid #3498db;
            padding-bottom: 10px;
        }}
        h2 {{
            color: #34495e;
            margin-top: 30px;
            border-left: 4px solid #3498db;
            padding-left: 15px;
        }}
        h3 {{
            color: #2c3e50;
            margin-top: 25px;
        }}
        h4 {{
            color: #34495e;
            margin-top: 20px;
        }}
        pre {{
            background: #f4f4f4;
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 15px;
            overflow-x: auto;
            font-family: 'Courier New', monospace;
        }}
        ul {{
            margin: 10px 0;
            padding-left: 20px;
        }}
        li {{
            margin: 5px 0;
        }}
        p {{
            margin: 10px 0;
            text-align: justify;
        }}
        strong {{
            color: #2c3e50;
            font-weight: 600;
        }}
        .timestamp {{
            text-align: right;
            color: #7f8c8d;
            font-size: 0.9em;
            margin-top: 30px;
            border-top: 1px solid #ecf0f1;
            padding-top: 15px;
        }}
    </style>
</head>
<body>
    <div class="container">
        {html_body}
        <div class="timestamp">
            Report generated: {__import__('datetime').datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
        </div>
    </div>
</body>
</html>"""
    
    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(html_template)
        print(f"HTML report successfully generated: {output_file}")
        return True
    except Exception as e:
        print(f"Error writing HTML report: {e}")
        return False

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 create_html_report.py <input.md> <output.html>")
        sys.exit(1)
    
    markdown_file = sys.argv[1]
    output_file = sys.argv[2]
    
    success = create_html_report(markdown_file, output_file)
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()