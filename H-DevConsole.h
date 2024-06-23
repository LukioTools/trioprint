#include "target_device.h"
#include "WebSockets.h"

using namespace WebServerW;
namespace Handlers {
    void GetConsole(){
        const char* temp = TD::devSerial.serialBuffer.c_str();
        server.send(200, "text/plain", temp, strlen(temp));
    }
}