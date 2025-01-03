#pragma once
#include "webServerClass.h"
#include "Debug.h"
#include "dynamic_config.h"

using namespace WebServerW;
namespace Handlers {
    void setDynamicConfig(){
        String config = server.arg("config"); // takes in the pos of the config in memory; can be found in dynammic_config.h
        String status = server.arg("status"); // takes in 0/1 (=false/true);

        memory.set_bit(config.toInt(), status.toInt());
        memory.saveToEEPROM();

        Debugger::print("saving to eeprom");

        server.send(200, "text/plain", "saved"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
    }
}