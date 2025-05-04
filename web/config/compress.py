import re
import gzip
import base64

# this script is used to compress the configuration web page for initial configuration.
# When run, it will automatically make config_html__template.h into the main folder that is included by the code.

def compress_html(html: str, remove_comments: bool = True) -> str:
    if remove_comments:
        # Remove HTML comments
        html = re.sub(r'<!--.*?-->', '', html, flags=re.DOTALL)

        # Remove JavaScript/CSS single-line comments (// ...)
        html = re.sub(r'//.*?$', '', html, flags=re.MULTILINE)

        # Remove JavaScript/CSS multi-line comments (/* ... */)
        html = re.sub(r'/\*.*?\*/', '', html, flags=re.DOTALL)

    # Clean up the HTML
    html = '\n'.join(line.strip() for line in html.splitlines())  # Strip empty lines
    html = re.sub(r'>\s+<', '><', html)  # Remove extra spaces between tags
    html = re.sub(r'\s{2,}', ' ', html)  # Collapse multiple spaces into one
    html = html.replace('\n', '')  # Remove newline characters to make it one long string
    return html

def gzip_compress(html: str) -> bytes:
    # Convert the string to bytes and gzip it
    html_bytes = html.encode('utf-8')
    compressed_html = gzip.compress(html_bytes)
    return compressed_html

def base64_encode(compressed_html: bytes) -> str:
    # Base64 encode the gzipped HTML
    return base64.b64encode(compressed_html).decode('utf-8')

def html_to_c_string(base64_encoded_html: str, max_line_length: int = 100) -> str:
    # Split the base64 encoded string into manageable lines first
    lines = [base64_encoded_html[i:i+max_line_length] for i in range(0, len(base64_encoded_html), max_line_length)]
    
    # Wrap each line in double quotes for C++ syntax
    return '\n'.join([f'"{line}"' for line in lines])

def generate_html_header_file(input_file_path: str, output_file_path: str, var_name: str = "html_page"):
    with open(input_file_path, 'r', encoding='utf-8') as f:
        html_content = f.read()

    # Compress the HTML
    compressed_html = compress_html(html_content)
    
    # Gzip compress the HTML
    gzipped_html = gzip_compress(compressed_html)

    # Base64 encode the gzipped HTML
    base64_encoded_html = base64_encode(gzipped_html)

    # Convert it to C++ string format with escaping
    c_string_html = html_to_c_string(base64_encoded_html)

    # Create the header content with proper inclusion guards
    header_content = f"""\
    #pragma once
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
