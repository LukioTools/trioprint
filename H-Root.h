#pragma once
#include "../Server.h"
#include "../SD.h"
namespace Handlers {
  void HandleRoot(){
      size_t fileSize = 0;
      uint8_t* fileData = SDW::readFile("compiled.html.gz", fileSize);

      if(fileData){
          Serial.println("File loaded: "+ String(fileSize));
          server.sendHeader("Content-Encoding", "gzip");
          server.send(200, "text/html", fileData, fileSize);
      }else{
          Serial.println("failed to load");
          server.send(500, "text/html", "Failed to load file from SD");
      }
    delete[] fileData;
  }
}