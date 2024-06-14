#pragma once
#include "WebServer.h"
using namespace WebServerW;
namespace Handlers {
    void ListFolder(){
        Serial.println(server.arg("plain"));
        auto e = SDW::listDir(server.arg("plain"));
        server.send(200, "application/json", e.c_str(), e.length());
    }
}