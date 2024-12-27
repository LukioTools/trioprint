#include "config.h"
#include "WIFI.h"
#include "OTA.h"
#include "target_device.h"
#include "webServerClass.h"
#include "webSocketClass.h"
#include "Handlers.h"
#include "Recovery.h"
#include "Debug.h"


//When using esp32 you have to install SdFat library made by Bill Greiman and on esp8266 you must delete that one and use the built in sdat library.

FsFile file;

int recoveryLine = 0;

TD::GCode printRunningCurrently;

void setup(){
    TD::devSerial.SerialBegin();

    #if defined(ESP8266)
    ESP.wdtEnable(5000);
    #endif

    #if DEVSERIAL != 0
    Serial.begin(115200);
    #endif  

    WiFiW::begin();
    OTAW::begin();
    SDW::init(5);
    WebServerW::begin();
    WebSocketW::begin();

    Debugger::print("files in root: ");
    Debugger::print(SDW::listDir("/"));

    Debugger::print("filename type: ");
    Debugger::print(USE_LONG_FILE_NAMES);
    Debugger::print(" UTF8: ");
    Debugger::print(USE_UTF8_LONG_NAMES);

    //./xtensa-esp-elf-addr2line -fe /home/hiha/developer/arduino/trioprint/build/esp32.esp32.esp32/trioprint.ino.elf 0x40179b5f:0x3ffb2170 0x400e31a6:0x3ffb2190 0x400d2ee8:0x3ffb21b0 0x400d2f12:0x3ffb21d0 0x400d4a93:0x3ffb2210 0x400e672b:0x3ffb2270 0x4008d16e:0x3ffb2290

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

    if(TD::printStarted){

        TD::printRunning = true;
        TD::printStarted = false;
        WebSocketW::brodcastAllTXT("print started");
        printRunningCurrently = TD::GCode(TD::filename, false, 0);
        printRunningCurrently.printRunning = true;
    }

    if(TD::printRunning){
        printRunningCurrently.Handle();
    }
    
}

