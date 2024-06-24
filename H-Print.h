#include "c_types.h"
#pragma once
#include "WebServer.h"

#ifndef FILE_CHUNK_SIZE
#define FILE_CHUNK_SIZE 1024
#endif

using namespace WebServerW;
namespace Handlers {
    void Print(){
        String filename = server.arg("path");
        FsFile file = SDW::openFile(filename);
        uint64_t size = file.fileSize();

        server.sendHeader("Content-Length", (String)size);
        server.send(200, "text/html", "");

        uint64_t currentSize = size;
        char temp[FILE_CHUNK_SIZE];
        while(currentSize >= 0){
            int wroteSize = SDW::readChunk(temp, file, FILE_CHUNK_SIZE);
            if(wroteSize <= 0) break;

            server.sendContent(temp);
            
            currentSize -= wroteSize;
        }

    }
}