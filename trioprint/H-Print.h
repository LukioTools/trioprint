#pragma once

#include "target_device.h"
#include "webServerClass.h"

using namespace WebServerW;
namespace Handlers {
    void Print(){
        TD::printManager.start(server.arg("path"), false, 0);
        WebServerW::server.send(200, "text/html", "Printing...");
    }
}