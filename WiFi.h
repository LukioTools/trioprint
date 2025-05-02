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
