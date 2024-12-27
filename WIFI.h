#include "Debug.h"
#pragma once


#include "config.h"


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
        Debugger::serialPrint("connection to WiFi");
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PWD);

        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Debugger::serialPrint("Connection Failed! Rebooting...");
            delay(5000);
            ESP.restart();
        }
    }

    IPAddress localIP(){
        return WiFi.localIP();
    }
}