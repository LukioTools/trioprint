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

    if (upload.status == UPLOAD_FILE_START) {
        Debugger::print("Opening file:" + upload.filename + " ...");
        upload_file = SDW::SD.open((filepath+upload.filename).c_str(), O_CREAT | O_WRITE | O_TRUNC);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        size_t amount = upload_file.write(upload.buf, upload.currentSize);
        Debugger::print("file Writing_" + String(filepath.length()+upload.filename.length()) + ", " + String(filepath) + String(upload.filename) + ", " + String(amount) + "/" + String(upload.currentSize));

    } else if (upload.status == UPLOAD_FILE_END) {
        upload_file.close();
        Debugger::print("File Upload handle was successfull!");
        if(upload.filename == ROOT_FILE && String(filepathp) == "/"){
            RootReloadCache();
        }
    }

  };
}
