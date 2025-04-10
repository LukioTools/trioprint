#pragma once
#include "SD_Manager.h"
#include "webServerClass.h"
#include "target_device.h"
#include "Debug.h"

using namespace WebServerW;
namespace Handlers {
    void sendCommand(){
        String command = server.arg("command");
        TD::devSerial.println(command);
        Debugger::print("command received from web: " + command);
        server.send(200, "text/plain", "command sent"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
    }
}