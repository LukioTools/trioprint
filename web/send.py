import requests
import os

def upload_file(file_path, cwd):
    """
    Uploads a file to the specified server endpoint.

    :param file_path: Path to the file to be uploaded
    :param cwd: Current working directory or target directory on the server
    """
    print(f"Uploading file: '{os.path.basename(file_path)}' to path '{cwd}'")

    url = f"http://192.168.2.200/fm/uploadFile/?path={cwd}"  # Replace with the actual server URL
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
