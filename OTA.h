
/*

 Copyright (c) <2025> <Vili Kervinen>
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
  Arduino IDE example: Examples > Arduino OTA > BasicOTA.ino
*********/
#pragma once
#include "config.h"
#include "MemoryManager.h"


#ifdef ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266mDNS.h>
#elif defined (ESP32)
    #include <WiFi.h>
    #include <ESPmDNS.h>
#endif

#include <WiFiUdp.h>
#include <ArduinoOTA.h>


namespace OTAW{
  void begin() { 
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });

    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("OTA: Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("OTA: Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("OTA: Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA: Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("OTA: End Failed");
    });

    char name[WEB_NAME_SIZE];
    char pwd[OTA_PWD_SIZE];
    flashMemory::get<FLASH_MEMORY::WEB_NAME>(name);
    flashMemory::get<FLASH_MEMORY::OTA_PWD>(pwd);
    ArduinoOTA.setHostname(name);
    ArduinoOTA.setPassword(pwd);

    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFiW::localIP());
  }

  void handle() {
    ArduinoOTA.handle();
  }
}