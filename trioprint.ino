#include "config.h"
#include "WIFI.h"
#include "OTA.h"
#include "target_device.h"
#include "webServerClass.h"
#include "webSocketClass.h"
#include "Handlers.h"
#include "Recovery.h"
#include "Debug.h"
#include "dynamic_config.h"
#include "systemd.h"


//When using esp32 you have to install SdFat library made by Bill Greiman and on esp8266 you must delete that one and use the built in sdat library.

FsFile file;

int recoveryLine = 0;

void setup(){

    memory.begin();

    TD::devSerial.SerialBegin();
    //TD::devSerial.changeBaundRate(250000);

    #if defined(ESP8266)
    ESP.wdtEnable(5000);
    #endif

    #if defined(DEVSERIAL) &&  DEVSERIAL != 0
        Serial.begin(115200);
    #endif

    WiFiW::begin();
    OTAW::begin();
    SDW::init(5);
    WebServerW::begin();
    WebSocketW::begin();

    systemd::infopuller.begin(5, "M105");

    recoveryLine = RV::CheckForRecovering();

    #if DEBUG
        if(recoveryLine != -1){
            Debugger::print("There is print to recover. Check the web interface! (Recovering isn't supported currently)");
            Debugger::print(recoveryLine);
        } else{
            Debugger::print("No print to recovery. You may proceed normally");
        }
    #endif

    // = TD::GCode("m1.gcode", true, 0);
}

void loop(){  
    OTAW::handle();
    WebServerW::handle();
    WebSocketW::Handle();
    TD::devSerial.Handle();
    systemd::infopuller.handle();

    if(TD::printStarted){

        TD::printRunning = true;
        TD::printStarted = false;
        TD::devSerial.inQueue = 0;
        WebSocketW::brodcastAllTXT("print started");
        TD::printManager = TD::GCode(TD::filename, false, 0);
        TD::printManager.printRunning = true;
    }

    if(TD::printRunning){
        if(TD::devSerial.abortPrint) {
            TD::printRunning = false;
            TD::devSerial.abortPrint = false;
            Debugger::print("print aborted");
        }else{
            TD::printManager.Handle();
        }
    }
    
}

