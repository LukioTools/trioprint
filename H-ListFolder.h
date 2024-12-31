#include "Debug.h"
#pragma once
#include "webServerClass.h"
#include "SD_Manager.h"

using namespace WebServerW;
namespace Handlers {
    void ListFolder(){
        auto e = SDW::listDir(server.arg("path"));
        Debugger::print("listing files");
        server.send(200, "application/json", e.c_str());
    }
}