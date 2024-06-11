#include "config.h"
#include "WIFI.h"
#include "OTA.h"
#include "SD.h"
#include "WebServer.h"
#include "log.h"

void setup(){
  Serial.begin(115200);
  WiFiW::begin();
  OTAW::begin();
  SDW::init(15);
  WebServerW::begin();
}


void loop(){
  unsigned long startTime = millis();
  
  WebServerW::handle();
  cout << SDW::cardSize() << "" << "\n";
  cout << SDW::freeSize() << "\n";
  cout << SDW::listDir("/")<<"\n";

  delay(1000);
}