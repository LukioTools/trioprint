#pragma once
//#include "WebServer.h"
#include "SD_Manager.h"
#include "StringStream.h"
#include "target_device.h"

using namespace WebServerW;
namespace Handlers {
    void ServerStatus(){
        StringStream responce = "{";
        responce 
            << "\"cardSize\":" << String(SDW::cardSize()) << ","
            << "\"freeSpace\":" << String(SDW::freeSize()) << ","
            << "\"printStatus\":" << String(TD::printManager.printState)
            << "}";
        server.send(200, "text/plain", responce.str());
    }
}