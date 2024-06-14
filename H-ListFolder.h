#pragma once
#include "WebServer.h"
using namespace WebServerW;
namespace Handlers {
    void ListFolder(){
        Serial.println(server.arg("path"));
        auto e = SDW::listDir(server.arg("path"));
        server.send(200, "application/json", e.c_str(), e.length());
    }
}