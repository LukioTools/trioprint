#pragma once
#include "SD_Manager.h"
#include "webServerClass.h"

using namespace WebServerW;
namespace Handlers {
    void Remove(){
        bool status = SDW::remove(server.arg("path"));
        if(status) server.send(200, "text/plain", "File removed successfully"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
        else{
            bool exists = SDW::exists(server.arg("path"));
            server.send(500, "text/plain", "failed: " + server.arg("path") + " " + String(status) + " " + String(exists));
        } // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
    }
}