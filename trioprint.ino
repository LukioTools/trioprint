#include "config.h"
#include "WIFI.h"
#include "OTA.h"
#include "SD.h"
#include "Handlers.h"
#include "log.h"
#include "WebSocket.h"


FsFile file;

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
    WebSocketW::begin();
}

void loop(){  
    OTAW::handle();
    WebServerW::handle();
    WebSocketW::Handle();
    TD::devSerial.Handle();
    //Serial.println(ESP.getFreeHeap(),DEC);
    //Serial.println("data" + String((char*)data));
    //Serial.println(SDW::lineCount(file));
}

