#pragma once

#include "c_types.h"
#include "webServerClass.h"

#ifndef FILE_CHUNK_SIZE
#define FILE_CHUNK_SIZE 1024
#endif

namespace TD {
    String filename = "";
    bool printStarted = false;
    bool printRunning = false;
}

using namespace WebServerW;
namespace Handlers {
    void Print(){
        TD::filename = server.arg("path");
        TD::printStarted = true;
        server.send(200, "text/html", "Printing...");


    }
}