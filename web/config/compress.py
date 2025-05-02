import re

def compress_html(html: str, remove_comments: bool = True) -> str:
    if remove_comments:
        # Remove HTML comments
        html = re.sub(r'<!--.*?-->', '', html, flags=re.DOTALL)

    # Clean up the HTML
    html = '\n'.join(line.strip() for line in html.splitlines())  # Strip empty lines
    html = re.sub(r'>\s+<', '><', html)  # Remove extra spaces between tags
    html = re.sub(r'\s{2,}', ' ', html)  # Collapse multiple spaces into one
    html = html.replace('\n', '')  # Remove newline characters to make it one long string
    return html

def escape_for_cpp(html: str) -> str:
    # Escape special characters for C++ string literals
    html = html.replace('\\', '\\\\')  # Escape backslashes
    html = html.replace('"', '\\"')    # Escape double quotes
    html = html.replace("'", "\\'")    # Escape single quotes
    html = html.replace('\n', '\\n')   # Escape newlines for readability
    html = html.replace('\r', '')      # Remove carriage returns
    return html

def html_to_c_string(html: str, max_line_length: int = 100) -> str:
    # Split the HTML content into manageable lines first
    lines = [html[i:i+max_line_length] for i in range(0, len(html), max_line_length)]
    
    # Escape each line separately for C++ syntax
    escaped_lines = [escape_for_cpp(line) for line in lines]
    
    # Wrap each line in double quotes for C++ syntax
    return '\n'.join([f'"{line}"' for line in escaped_lines])

def generate_html_header_file(input_file_path: str, output_file_path: str, var_name: str = "html_page"):
    with open(input_file_path, 'r', encoding='utf-8') as f:
        html_content = f.read()

    # Compress the HTML
    compressed_html = compress_html(html_content)
    
    # Convert it to C++ string format with escaping
    c_string_html = html_to_c_string(compressed_html)

    # Create the header content with proper inclusion guards
    header_content = f"""\
#ifndef HTML_TEMPLATE_H
#define HTML_TEMPLATE_H

const char {var_name}[] PROGMEM = 
{c_string_html}
;

#endif // HTML_TEMPLATE_H
"""

    # Write the content to the header file
    with open(output_file_path, 'w', encoding='utf-8') as f:
        f.write(header_content)

if __name__ == "__main__":
    input_path = 'index.html'
    output_path = '../../config_html_template.h'
    generate_html_header_file(input_path, output_path, "CONFIGURATION_HTML_TEMPLATE")
    print(f"Header file generated at {output_path}")
