#include "config.h"
#include "WIFI.h"
#include "OTA.h"
#include "target_device.h"
#include "webServerClass.h"
#include "webSocketClass.h"
#include "Handlers.h"
#include "Recovery.h"


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
    SDW::init(SD_CONFIG);
    WebServerW::begin();
    WebSocketW::begin();

    Serial.println("files in root: ");
    Serial.println(SDW::listDir("/"));


    recoveryLine = RV::CheckForRecovering();

    #if DEBUG
        if(recoveryLine != -1){
            Serial.println("There is print to recover. Check the web interface! (Recovering isn't supported currently)");
            Serial.println(recoveryLine);
        } else{
            Serial.println("No print to recovery. You may proceed normally");
        }
    #endif

    // = TD::GCode("m1.gcode", true, 0);
}

void loop(){  
    auto start = millis();
    OTAW::handle();
    WebServerW::handle();
    WebSocketW::Handle();
    TD::devSerial.Handle();

    if(TD::printStarted){

        TD::printRunning = true;
        TD::printStarted = false;
        printRunningCurrently = TD::GCode(TD::filename, false, 0);
    }

    if(TD::printRunning){
        printRunningCurrently.Handle();
    }
    auto end = millis();


    //TD::devSerial.inQueue = 0;

    //Serial.println(ESP.getFreeHeap(),DEC);
    //Serial.println("data" + String((char*)data));
    //Serial.println(SDW::lineCount(file));
    
}

