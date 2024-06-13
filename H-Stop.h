#pragma once
namespace Handlers {
    void Stop(){
        server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
    }
}