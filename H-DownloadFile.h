#pragma once
#include "WebServer.h"
namespace Handlers {
  void DownloadFile(){
    server.sendHeader("Content-Encoding", "application/octet-stream");
    Serial.println("downloading file");
    size_t filesize = 0;
    const String& name = server.arg("fileName");
    uint8_t* filedata = SDW::readFile(name.c_str(), filesize);
    server.send(200, "application/octet-stream", (char *)filedata, filesize);
    Serial.println("Download completed");
  }
}
