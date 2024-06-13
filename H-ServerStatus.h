#pragma once
#include "WebServer.h"
namespace Handlers {
    void ServerStatus(){
        String responce = "{";
        responce += "\"cardSize\":" + String(SDW::cardSize()) + ",";
        responce += "\"freeSpace\":" + String(SDW::freeSize()) + ",";
        responce += "\"printting_status\":" + String(Target_device::isPrintting) + "";
        responce += "}";
        server.send(200, "text/plain", responce);
    }
}