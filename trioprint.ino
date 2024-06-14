#include "config.h"
#include "WIFI.h"
#include "OTA.h"
#include "SD.h"
#include "Handlers.h"
#include "log.h"



void setup(){
  ESP.wdtEnable(5000);
  Serial.begin(115200);
  WiFiW::begin();
  OTAW::begin();
  Serial.println(SDW::init(15));
  WebServerW::begin();
}

void loop(){  
  OTAW::handle();
  WebServerW::handle();
}

