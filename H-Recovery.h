#pragma once
#include "config.h"
#include "webServerClass.h"

using namespace WebServerW;

namespace Handlers {
    void RecoveryStatus(){
        server.send(200, "text/plain", "recovery functionality is not supported"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
    }
}