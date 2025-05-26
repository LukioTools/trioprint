import re
import gzip

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

def html_to_c_array(gzipped_html: bytes) -> str:
    # Convert the gzipped bytes into a C++ array declaration
    hex_bytes = ", ".join(f"0x{byte:02X}" for byte in gzipped_html)
    return f"const uint8_t CONFIGURATION_HTML_TEMPLATE[] PROGMEM = {{ {hex_bytes} }};"

def generate_html_header_file(input_file_path: str, output_file_path: str):
    with open(input_file_path, 'r', encoding='utf-8') as f:
        html_content = f.read()

    # Compress the HTML
    compressed_html = compress_html(html_content)
    
    # Gzip compress the HTML
    gzipped_html = gzip_compress(compressed_html)

    # Convert the gzipped HTML into a C++ array
    c_array = html_to_c_array(gzipped_html)

    # Create the header content with proper inclusion guards
    header_content = f"""\
#pragma once
#ifndef HTML_TEMPLATE_H
#define HTML_TEMPLATE_H

{c_array}

#endif // HTML_TEMPLATE_H
"""

    # Write the content to the header file
    with open(output_file_path, 'w', encoding='utf-8') as f:
        f.write(header_content)

if __name__ == "__main__":
    input_path = 'index.html'
    output_path = '../../config_html_template.h'
    generate_html_header_file(input_path, output_path)
    print(f"Header file generated at {output_path}")
