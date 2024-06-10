#include "config.h"
#include "WIFI.h"
#include "OTA.h"

void setup(){
  Serial.begin(115200);
  tri_WiFi::begin();
  tri_OTA::begin();
}
void loop(){
  tri_OTA::handle();
}