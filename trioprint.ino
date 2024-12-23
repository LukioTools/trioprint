#include "device_Types.h"
#include "config.h"
#include "WIFI.h"
#include "OTA.h"
#include "SD_Manager.h"
#include "webServerClass.h"
#include "Handlers.h"
#include "log.h"
#include "webSocketClass.h"
#include "target_device.h"
#include "Recovery.h"


FsFile file;

int recoveryLine = 0;

TD::GCode printRunningCurrently;

void setup(){

    TD::devSerial.SerialBegin();
    ESP.wdtEnable(5000);

    #if DEVSERIAL != 0
    Serial.begin(115200);
    #endif  

    WiFiW::begin();
    OTAW::begin();
    SDW::init(15);
    WebServerW::begin();
    /*
    WebSocketW::begin();


    recoveryLine = RV::CheckForRecovering();

    #if DEBUG
        if(recoveryLine != -1){
            Serial.println("There is print to recover. Check the web interface! (Recovering isn't supported currently)");
        } else{
            Serial.println("No print to recovery. You may proceed normally");
        }
    #endif

    // = TD::GCode("m1.gcode", true, 0);*/
}

void loop(){  /*
    //auto start = millis();
    OTAW::handle();
    //auto end = millis();
    //Serial.printf("otaw: %lu\n", end-start);

    //start = millis();
    WebServerW::handle();
    //end = millis();
    //Serial.printf("webServerW: %lu\n", end-start);

    //start = millis();
    WebSocketW::Handle();
    //end = millis();
    //Serial.printf("webSocketW: %lu\n", end-start);

    //start = millis();
    TD::devSerial.Handle();
    //end = millis();
    //Serial.printf("devSerial: %lu\n", end-start);
    
    if(TD::printStarted){

        TD::printRunning = true;
        TD::printStarted = false;
        printRunningCurrently = TD::GCode(TD::filename, false, 0);
    }

    //start = millis();
    if(TD::printRunning){
        printRunningCurrently.Handle();
    }
    //end = millis();
    //Serial.printf("printRunningCurrently: %lu\n", end-start);


    //TD::devSerial.inQueue = 0;

    //Serial.println(ESP.getFreeHeap(),DEC);
    //Serial.println("data" + String((char*)data));
    //Serial.println(SDW::lineCount(file));
    */
}

