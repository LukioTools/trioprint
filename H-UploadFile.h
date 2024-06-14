#pragma once
#include "SD.h"
#include "WebServer.h"
using namespace WebServerW;
namespace Handlers {
  FsFile upload_file;
  void UploadFile(){
    const String& filepath = server.arg("path");
    const char* filepathp;
    if(filepath == "") filepathp = "/";
    else filepathp = filepath.c_str();
    
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
      upload_file = SDW::SD.open(filepath+upload.filename);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      Serial.printf(
        "Wrote %i/%i to '%s'", 
        upload_file.write(upload.buf, upload.currentSize), 
        upload.currentSize, 
        (filepath+upload.filename).c_str()
      );
    } else if (upload.status == UPLOAD_FILE_END) {
      upload_file.close();
      Serial.println("File Upload handle was successfull!!½!½!!!!");
    }

  };
}
