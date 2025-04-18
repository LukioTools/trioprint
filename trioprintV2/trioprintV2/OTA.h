/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
  Arduino IDE example: Examples > Arduino OTA > BasicOTA.ino
*********/
#pragma once
#include "config.h"
#include "MemoryManager.h"


#if defined(ESP8266)
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
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
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