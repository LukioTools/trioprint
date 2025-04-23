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

#include "StringStream.h"

#include "config.h"

namespace WBW {

DevM::DeviceManager* deviceManager;

namespace Handlers {

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "not found");
}

namespace Root {
char* root_cache_data = nullptr;
std::size_t root_cache_size = -1;

void RootPreload() {
  if (!root_cache_data) {
    root_cache_data = SDM::readFile(ROOT_FILE, root_cache_size);
    Serial.printf("Root cached (%p)[%i]!\n", root_cache_data, root_cache_size);
  }
}
void RootClearCache() {
  delete root_cache_data;
  root_cache_data = nullptr;
}
void RootReloadCache() {
  RootClearCache();
  RootPreload();
}
void Root(AsyncWebServerRequest* request) {
  RootPreload();
  if (root_cache_data) {
    AsyncWebServerResponse* response = request->beginResponse(200, "text/html", (uint8_t*)root_cache_data, root_cache_size);  //Sends 404 File Not Found
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  } else {
    request->send(500, "text/plain", "file not fould");
  }
}
}

void ServerStatus(AsyncWebServerRequest* request) {
  Serial.println("1");

  StringStream responce = "{";
  responce
    << "\"cardSize\":" << String(SDM::cardSize()) << ","
    << "\"freeSpace\":" << String(SDM::freeSize()) << ","
    << "}";
  Serial.println("4");
  request->send(200, "text/plain", responce.str());
  Serial.println("5");
}

void sendCommand(AsyncWebServerRequest* request) {
  String command = request->arg("command") + "\n";
  deviceManager->print(command);
  request->send(200, "text/plain", "command sent");  // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void ListFolder(AsyncWebServerRequest* request) {
  auto e = SDM::listDir(request->arg("path"));
  request->send(200, "application/json", e.c_str());
}

void DownloadFile() {
  String filename = server.arg("filename");
  FsFile file = SDW::openFile(filename);
  uint64_t size = file.fileSize();

  server.sendHeader("Content-Disposition", "attachment; filename=\"" + filename + '"');
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "application/octet-stream", "");

  uint64_t currentSize = size;
  char temp[FILE_CHUNK_SIZE];
  while (currentSize > 0) {
    int wroteSize = SDW::readChunk(temp, file, FILE_CHUNK_SIZE);
    if (wroteSize <= 0) break;
    delay(0);
    server.sendContent(temp, wroteSize);
    currentSize -= wroteSize;
  }
  server.sendContent("");
}

}

static AsyncWebServer* server;

void begin(DevM::DeviceManager* dm) {
  deviceManager = dm;
  server = new AsyncWebServer(flashMemory::get<FLASH_MEMORY::WEB_SERVER_PORT>());

  Serial.printf("starting web server at port: %d\n", flashMemory::get<FLASH_MEMORY::WEB_SERVER_PORT>());

  server->on("/", HTTP_GET, Handlers::Root::Root);

  server->on("/server/status", HTTP_GET, Handlers::ServerStatus);
  server->on("/server/pause", HTTP_GET, Handlers::notFound);
  server->on("/server/continue", HTTP_GET, Handlers::notFound);
  server->on("/server/stop", HTTP_GET, Handlers::notFound);
  server->on("/server/ems", HTTP_GET, Handlers::notFound);
  server->on("/server/status", HTTP_GET, Handlers::notFound);
  server->on("/server/console", HTTP_GET, Handlers::notFound);
  server->on("/server/recoveryStatus", HTTP_GET, Handlers::notFound);

  server->on("/device/sendCommand", HTTP_GET, Handlers::notFound);

  server->on("/fm/ls", HTTP_GET, Handlers::ListFolder);
  server->on("/fm/remove", HTTP_GET, Handlers::notFound);
  server->on("/fm/mkdir", HTTP_GET, Handlers::notFound);
  server->on("/fm/downloadFile", HTTP_GET, Handlers::notFound);
  server->on("/fm/uploadFile", HTTP_GET, Handlers::notFound);

  server->on("/config/setDynamicConfig", HTTP_GET, Handlers::notFound);
  server->on("/config/getDynamicConfig", HTTP_GET, Handlers::notFound);

  server->onNotFound(Handlers::notFound);

  Handlers::Root::RootPreload();

  server->begin();

  Serial.println("web server started successfully");
}


// Helper macro for platform-specific sending (optional)
#if defined(ESP8266)
#define SERVER_SEND_WITH_LENGTH(code, type, data, size) (server.send(code, type, data, size));
#elif defined(ESP32)
#define SERVER_SEND_WITH_LENGTH(code, type, data, size) (server.send_P(code, type, data, size));
#endif

}  // namespace WBW
