#pragma once
#include "SD_Manager.h"
#include "webServerClass.h"
#include "H-Root.h"

using namespace WebServerW;
namespace Handlers {
  FsFile upload_file;
  void UploadFile(){
    const String& filepath = server.arg("path");
    const char* filepathp;
    if(filepath == "") filepathp = "/";
    else filepathp = filepath.c_str();

    HTTPUpload& upload = server.upload();
      Serial.print("Status: ");
      Serial.println(upload.status);

    if (upload.status == UPLOAD_FILE_START) {
      Serial.println("Opening file:" + upload.filename + " ...");
      upload_file = SDW::SD.open((filepath+upload.filename).c_str(), O_CREAT | O_WRITE | O_TRUNC);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      Serial.println("Writing...");
      Serial.printf(
        "Wrote %i/%i to '%s'", 
        upload_file.write(upload.buf, upload.currentSize), 
        upload.currentSize, 
        (filepath+upload.filename).c_str()
      );
    } else if (upload.status == UPLOAD_FILE_END) {
      upload_file.close();
      Serial.println("File Upload handle was successfull!");
      if(upload.filename == ROOT_FILE && String(filepathp) == "/"){
        RootReloadCache();
      }
    }

  };
}
