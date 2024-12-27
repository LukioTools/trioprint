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
        root_cache_data = SDW::readFile(ROOT_FILE, root_cache_size);
        Serial.printf("Root cached (%p)[%i]!\n", root_cache_data, root_cache_size);
        }
    }
    void RootClearCache(){
        Debugger::print("Clearing root cache...\n");
        delete root_cache_data;
        root_cache_data = nullptr;
    }
    void RootReloadCache(){
        Debugger::print("Reloading root cache...\n");
        RootClearCache();
        RootPreload();
    }

    void Root(){
        Debugger::print("Handling client");

        RootPreload();

        if(root_cache_data){
            server.sendHeader("Content-Encoding", "gzip");
            SERVER_SEND_WITH_LENGTH(200, "text/html", root_cache_data, root_cache_size);
        }else{
            Debugger::print("failed to load");
            server.send(500, "text/html", "Failed to load file from SD");
        }
    }
}