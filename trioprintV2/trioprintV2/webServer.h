#pragma once

#include <memory>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#endif

#include "config.h"

namespace WBW {

static AsyncWebServer* server;

void begin() {
  server = new AsyncWebServer(flashMemory::get<FLASH_MEMORY::WEB_SERVER_PORT>());

  Serial.printf("starting web server at port: %d\n", flashMemory::get<FLASH_MEMORY::WEB_SERVER_PORT>());
  server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("client connected");
    request->send(200, "text/plain", "lel");
  });

  server->on("/file", HTTP_GET, [](AsyncWebServerRequest* request) {
    const char* filename = "/example.txt";  // Change to your desired file
    size_t fileSize = 0;
    const uint8_t* fileData = (uint8_t*)SDM::readFile(filename, fileSize);

    if (fileData != nullptr) {
      // Send file content and then delete buffer
      request->send_P(200, "text/plain", fileData, fileSize);
      delete[] fileData;
    } else {
      request->send(404, "text/plain", "File not found or error reading file.");
    }
  });

  server->on("/createFile", HTTP_GET, [](AsyncWebServerRequest* request) {
    const char* filename = "/example.txt";
    const uint8_t* data = (uint8_t*)"kek";
    if (SDM::WriteFile(filename, data, 3))
      request->send(200, "text/plain", "file written");
    else
      request->send(404, "text/plain", "File not written");
  });

  server->onNotFound([](AsyncWebServerRequest* request) {
    Serial.println("client hit unknown route");
    request->send(404, "text/plain", "Not Found");
  });

  server->begin();
}

// Helper macro for platform-specific sending (optional)
#if defined(ESP8266)
#define SERVER_SEND_WITH_LENGTH(code, type, data, size) (server.send(code, type, data, size));
#elif defined(ESP32)
#define SERVER_SEND_WITH_LENGTH(code, type, data, size) (server.send_P(code, type, data, size));
#endif

}  // namespace WBW
