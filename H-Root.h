#pragma once
#include "WebServer.h"
using namespace WebServerW;
#include "SD.h"
#include <cstddef>
namespace Handlers {

  unsigned char* root_cache_data = nullptr;
  std::size_t root_cache_size = -1;

  void RootPreload(){
    if(!root_cache_data) {
      root_cache_data = SDW::readFile("compiled.html.gz", root_cache_size);
      Serial.println("root cached now!");
    }
  }

  void Root(){
      RootPreload();

      if(root_cache_data){
          //Serial.println("File loaded: "+ String(fileSize));
          server.sendHeader("Content-Encoding", "gzip");
          server.send(200, "text/html", root_cache_data, root_cache_size);
      }else{
          Serial.println("failed to load");
          server.send(500, "text/html", "Failed to load file from SD");
      }
  }
}