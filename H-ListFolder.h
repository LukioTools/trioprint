#pragma once
#include "../Server.h"
namespace Handlers {
    void ListFolder(){
        Serial.println(server.arg("plain"));
        server.send(200, "application/json", SDW::listDir(server.arg("plain")).c_str());
    }
}