#pragma once
#include "webServerClass.h"
#include "Debug.h"
#include "dynamic_config.h"

using namespace WebServerW;
namespace Handlers {
    void getDynamicConfig(){
        String result = "";
        for (int i = 7; i >= 0; i--) {
            result += (memory.boolean_config[0] & (unsigned char)(1 << i)) ? '1' : '0';
        }
        server.send(200, "text/plain", result); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
    }
}