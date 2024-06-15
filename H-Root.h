#pragma once
#include "WebServer.h"
#include "config.h"
using namespace WebServerW;
#include "SD.h"
#include <cstddef>
namespace Handlers {

  unsigned char* root_cache_data = nullptr;
  std::size_t root_cache_size = -1;
  


  void RootPreload(){
    if(!root_cache_data) {
      root_cache_data = SDW::readFile(ROOT_FILE, root_cache_size);
      Serial.printf("Root cached (%p)[%i]!\n", root_cache_data, root_cache_size);
    }
  }
  void RootClearCache(){
    Serial.printf("Clearing root cache...\n");
    delete root_cache_data;
    root_cache_data = nullptr;
  }
  void RootReloadCache(){
    Serial.printf("Reloading root cache...\n");
    RootClearCache();
    RootPreload();
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