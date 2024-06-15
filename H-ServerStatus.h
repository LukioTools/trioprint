#pragma once
//#include "WebServer.h"
#include "SD.h"
#include "StringStream.h"
#include "target_device.h"
using namespace WebServerW;
namespace Handlers {
    void ServerStatus(){
        StringStream responce = "{";
        responce 
            << "\"cardSize\":" << String(SDW::cardSize()) << ","
            << "\"freeSpace\":" << String(SDW::freeSize()) << ","
            << "\"printStatus\":" << String(Target_device::isPrintting)
            << "}";
        server.send(200, "text/plain", responce.str());
    }
}