#pragma once
#include "WebServer.h"
namespace Handlers {
    void Mkdir(){
        server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
    }
}