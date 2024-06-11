#pragma once


#include "./config.h"


#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

namespace WiFiW{
    void begin(){
        Serial.println("connection to WiFi");
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PWD);

        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.println("Connection Failed! Rebooting...");
            delay(5000);
            ESP.restart();
        }

        if (MDNS.begin(WEB_NAME)) {              // Start the mDNS responder for esp8266.local
          Serial.println("mDNS responder started");
        } else {
          Serial.println("Error setting up MDNS responder!");
        }
    }

    IPAddress localIP(){
        return WiFi.localIP();
    }
}