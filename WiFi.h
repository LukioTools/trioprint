#pragma once
#include "config.h"
#include "MemoryManager.h"
#include <WiFiUdp.h>

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266mDNS.h>
#elif defined(ESP32)
    #include <WiFi.h>
    #include <ESPmDNS.h>
#endif

#include <WiFiClient.h>

namespace WiFiW{
    void begin(){
        WiFi.mode(WIFI_STA);
        char ssid[WIFI_SSID_SIZE];
        char pwd[WIFI_PWD_SIZE];
        flashMemory::get<FLASH_MEMORY::WIFI_SSID>(ssid);
        flashMemory::get<FLASH_MEMORY::WIFI_PWD>(pwd);

        Serial.printf("ssid: %s, pwd: %s\n", ssid, pwd);

        WiFi.begin(ssid, pwd);

        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.println("failed to connect...");
            delay(5000);
            ESP.restart();
        }
    }

    IPAddress localIP(){
        return WiFi.localIP();
    }
}