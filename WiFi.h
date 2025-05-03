
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

namespace WiFiW {
void begin() {
  WiFi.mode(WIFI_STA);
  char ssid[WIFI_SSID_SIZE];
  char pwd[WIFI_PWD_SIZE];
  flashMemory::get<FLASH_MEMORY::WIFI_SSID>(ssid);
  flashMemory::get<FLASH_MEMORY::WIFI_PWD>(pwd);

  Serial.printf("Trying to connect to SSID: %s\n", ssid);

  WiFi.begin(ssid, pwd);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_CONNECTING_TIMEMOUT) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi!");
  } else {
    Serial.println("\nFailed to connect. Starting Access Point...");

    WiFi.mode(WIFI_AP);

    IPAddress apIP(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.softAPConfig(apIP, gateway, subnet);

    char *ap_ssid = ssid;
    char *ap_password = pwd; 

    if (WiFi.softAP(ap_ssid, ap_password)) {
      Serial.println("Access Point started!");
      Serial.print("AP IP address: ");
      Serial.println(WiFi.softAPIP());
    } else {
      Serial.println("Failed to start Access Point!");
    }
  }
}

IPAddress localIP() {
  if (WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_AP) {
    return WiFi.softAPIP();
  } else {
    return WiFi.localIP();
  }
}
}
