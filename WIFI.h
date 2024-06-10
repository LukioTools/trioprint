#pragma once


#include "./config.h"


#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
namespace tri_WiFi{
    void begin(){
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PWD);

        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.println("Connection Failed! Rebooting...");
            delay(5000);
            ESP.restart();
        }
    }

    IPAddress localIP(){
        return WiFi.localIP();
    }
}