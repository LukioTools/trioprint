#pragma once

#include "target_device.h"
#include "webServerClass.h"

using namespace WebServerW;
using namespace TD;
namespace Handlers {
    void Print(){
        printManager.start(server.arg("path"), false, 0);
        WebServerW::server.send(200, "text/html", "Printing...");
    }
}