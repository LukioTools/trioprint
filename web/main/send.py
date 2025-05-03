
'''
 Copyright (c) <2025> <Vili Kervinen>
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import requests
import os

#this script is used to upload files to a server using HTTP POST requests.
# use this in case something goes wrong with the server and you are unable to see the web interface.

#modify these variables to match your server settings
ip = "192.168.2.200"

def upload_file(file_path, cwd):
    """
    Uploads a file to the specified server endpoint.

    :param file_path: Path to the file to be uploaded
    :param cwd: Current working directory or target directory on the server
    """
    print(f"Uploading file: '{os.path.basename(file_path)}' to path '{cwd}'")

    url = f"http:/{ip}/fm/uploadFile/?path={cwd}"  # Replace with the actual server URL
    with open(file_path, 'rb') as file:
        files = {'file': file}
        try:
            response = requests.post(url, files=files)
            print(f"Response: {response.status_code}, {response.text}")
            if response.status_code == 200:
                print("File uploaded successfully.")
            else:
                print("Failed to upload file.")
        except requests.RequestException as e:
            print(f"An error occurred: {e}")

def handle_drop(files, cwd):
    """
    Handles the drop event for files.

    :param files: List of file paths dropped
    :param cwd: Current working directory or target directory on the server
    """
    print("File(s) dropped")
    for file_path in files:
        if os.path.isfile(file_path):
            upload_file(file_path, cwd)
        else:
            print(f"Skipped: '{file_path}' is not a valid file.")

# Example usage
if __name__ == "__main__":
    dropped_files = ["./compiled.html.gz"]  # Replace with actual file paths
    current_directory = "/"  # Replace with the desired server directory

    handle_drop(dropped_files, current_directory)
