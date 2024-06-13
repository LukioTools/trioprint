#include "config.h"
#include "WIFI.h"
#include "OTA.h"
#include "SD.h"
#include "WebServer.h"
#include "log.h"

void setup(){
  ESP.wdtEnable(5000);
  Serial.begin(115200);
  WiFiW::begin();
  OTAW::begin();
  Serial.println(SDW::init(15));
  WebServerW::begin();
  Serial.println("lol");
}

void loop(){  
  OTAW::handle();
  delay(0);
  WebServerW::handle();
  delay(0);
  delay(1000);
  
}

