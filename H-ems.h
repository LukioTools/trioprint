#pragma once

#include "target_device.h"
#include "webServerClass.h"


using namespace WebServerW;
namespace Handlers {
    void Ems(){
        TD::printManager.ems();
        server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
    }
}