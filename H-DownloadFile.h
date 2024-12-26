#pragma once
#include "config.h"
#include "webServerClass.h"
#include "SD_Manager.h"

using namespace WebServerW;
namespace Handlers {
  void DownloadFile(){
    /*
    server.sendHeader("Content-Encoding", "application/octet-stream");
    Serial.println("downloading file");
    size_t filesize = 0;
    const String& name = server.arg("filename");
    uint8_t* filedata = SDW::readFile(name.c_str(), filesize);
    server.sendHeader("Content-Disposition", "inline; filename=\""+ name + '"');
    server.send(200, "application/octet-stream", (char *)filedata, filesize);
    delete[]filedata;
    Serial.println("Download completed");


    */

        String filename = server.arg("filename");
        FsFile file = SDW::openFile(filename);
        uint64_t size = file.fileSize();

        server.sendHeader("Content-Disposition", "attachment; filename=\""+ filename + '"');
        server.setContentLength(CONTENT_LENGTH_UNKNOWN);
        server.send(200, "application/octet-stream", "");

        uint64_t currentSize = size;
        char temp[FILE_CHUNK_SIZE];
        while(currentSize > 0){
            int wroteSize = SDW::readChunk(temp, file, FILE_CHUNK_SIZE);
            if(wroteSize <= 0) break;
            delay(0);
            server.sendContent(temp, wroteSize);
            currentSize -= wroteSize;
        }
        server.sendContent("");
  }
}
