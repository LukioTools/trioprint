#include "c_types.h"
#pragma once
#include "config.h"
#include "webServerClass.h"
#include "SD_Manager.h"

using namespace WebServerW;
namespace Handlers {

    char* root_cache_data = nullptr;
    std::size_t root_cache_size = -1;

    void RootPreload(){
        if(!root_cache_data) {
        root_cache_data = (char*)SDW::readFile(ROOT_FILE, root_cache_size);
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
        Serial.println("Handling client");

        RootPreload();

        if(root_cache_data){
            //Serial.println("File loaded: "+ String(fileSize));
            server.sendHeader("Content-Encoding", "gzip");
            server.send(200, "text/html", root_cache_data);
        }else{
            Serial.println("failed to load");
            server.send(500, "text/html", "Failed to load file from SD");
        }
    }
}