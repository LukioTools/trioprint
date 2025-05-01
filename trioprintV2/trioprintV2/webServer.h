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
#include "stdExtension.h"
#include "config.h"
#include "TinyMap.h"

namespace WBW {

DevM::GCodeManager* gcodeManager;

namespace Handlers {
void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "not found");
}

namespace Root {
char* root_cache_data = nullptr;
std::size_t root_cache_size = -1;

void RootLoadCache() {
}

void RootClearCache() {
  delete root_cache_data;
  root_cache_data = nullptr;
}
void RootReloadCache() {
  RootClearCache();
  RootLoadCache();
}
void Root(AsyncWebServerRequest* request) {
  if (root_cache_data) {
    AsyncWebServerResponse* response = request->beginResponse(200, "text/html", (uint8_t*)root_cache_data, root_cache_size);  //Sends 404 File Not Found
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  } else {
    auto SDRequest = std::make_unique<SDM::HANDLER::WebRootLoad>(request->pause(), root_cache_data, root_cache_size);
    SDM::HANDLER::SDHandlerManager.addHandler(std::move(SDRequest));
  }
}
}

void serverStatus(AsyncWebServerRequest* request) {

  StringStream responce = "{";
  responce
    << "\"cardSize\":" << String(SDM::cardSize()) << ","
    << "\"freeSpace\":" << String(SDM::freeSize()) << ","
    << "}";
  request->send(200, "text/plain", responce.str());
}

void sendCommand(AsyncWebServerRequest* request) {
  String command = request->arg("command") + "\n";
  gcodeManager->deviceManager->print(command);
  request->send(200, "text/plain", "command sent");
}

void listFolder(AsyncWebServerRequest* request) {
  auto SDRequest = std::make_unique<SDM::HANDLER::WebListDir>(request->pause(), request->arg("path"));
  SDM::HANDLER::SDHandlerManager.addHandler(std::move(SDRequest));
}

void remove(AsyncWebServerRequest* request) {

  if (!request->hasArg("path")) {
    request->send(500, "text/plain", "failed: " + request->arg("path"));
    return;
  }

  auto SDRequest = std::make_unique<SDM::HANDLER::WebRemove>(request->pause(), request->arg("path"));
  SDM::HANDLER::SDHandlerManager.addHandler(std::move(SDRequest));
}

void downloadFile(AsyncWebServerRequest* request) {
  if (!request->hasArg("filename")) {
    request->send(400, "text/plain", "Missing filename parameter");
    return;
  }

  auto SDRequest = std::make_unique<SDM::HANDLER::WebDownloadfile>(request->pause());
  SDM::HANDLER::SDHandlerManager.addHandler(std::move(SDRequest));
}

void mkdir(AsyncWebServerRequest* request) {

  if (request->hasArg("path")) {
    auto SDRequest = std::make_unique<SDM::HANDLER::WebMakeDir>(request->pause(), request->arg("path"));
    SDM::HANDLER::SDHandlerManager.addHandler(std::move(SDRequest));
  } else
    request->send(500, "text/plain", "Failed to create a directory");
}

void print(AsyncWebServerRequest* request) {
  if (!request->hasArg("path")) {
    request->send(400, "text/plain", "missing filepath");
    return;
  }


  String filename = request->arg("path");

  if (gcodeManager == nullptr)
    request->send(500, "text/plain", "gcode manager not found. reboot device");


  gcodeManager->startPrint(filename);
  request->send(200, "text/plain", "Started print: " + filename);
}

void ems(AsyncWebServerRequest* request) {
  gcodeManager->ems();
  request->send(200, "text/plain", "ems activated");
}

void pause(AsyncWebServerRequest* request) {
  if (request->hasArg("isCommand")) {
    DevM::GCodeManager::PrintState state = gcodeManager->pause();
    if (state == DevM::GCodeManager::PRINTING)
      request->send(200, "text/plain", "running");
    else if (state == DevM::GCodeManager::PAUSE)
      request->send(200, "text/plain", "paused");
  } else {
    if (gcodeManager->printState != DevM::GCodeManager::PAUSE)
      request->send(200, "text/plain", "running");
    else
      request->send(200, "text/plain", "paused");
  }
}

void stop(AsyncWebServerRequest* request) {
  request->send(200, "text/plain", String(gcodeManager->stop()));
}

void setDynamic(AsyncWebServerRequest* request) {
  String config = request->arg("config");  // takes in the pos of the config in memory; can be found in dynammic_config.h
  String status = request->arg("status");

  switch (static_cast<FLASH_MEMORY::NamesEeprom>(config.toInt())) {
    case FLASH_MEMORY::WIFI_SSID:
      if (status.length() <= WIFI_SSID_SIZE) {
        flashMemory::set<FLASH_MEMORY::WIFI_SSID>(status.c_str());
        request->send(200, "text/plain", "saved");
      } else
        request->send(413, "text/plain", "too long");
      break;
    case FLASH_MEMORY::WIFI_PWD:
      if (WIFI_PWD_SIZE <= WIFI_SSID_SIZE) {
        flashMemory::set<FLASH_MEMORY::WIFI_PWD>(status.c_str());
        request->send(200, "text/plain", "saved");
      } else
        request->send(413, "text/plain", "too long");
      break;
    case FLASH_MEMORY::OTA_PWD:
      if (WIFI_PWD_SIZE <= OTA_PWD_SIZE) {
        flashMemory::set<FLASH_MEMORY::OTA_PWD>(status.c_str());
        request->send(200, "text/plain", "saved");
      } else
        request->send(413, "text/plain", "too long");
      break;
    case FLASH_MEMORY::WEB_NAME:
      if (WIFI_PWD_SIZE <= WEB_NAME_SIZE) {
        flashMemory::set<FLASH_MEMORY::WEB_NAME>(status.c_str());
        request->send(200, "text/plain", "saved");
      } else
        request->send(413, "text/plain", "too long");
      break;
    case FLASH_MEMORY::WEB_SERVER_PORT:
      flashMemory::set<FLASH_MEMORY::WEB_SERVER_PORT>(status.toInt());
      request->send(200, "text/plain", "saved");
      break;
    case FLASH_MEMORY::WEB_SOCKET_PORT:
      flashMemory::set<FLASH_MEMORY::WEB_SOCKET_PORT>(status.toInt());
      request->send(200, "text/plain", "saved");
      break;
    case FLASH_MEMORY::SD_SECTOR_SIZE:
      flashMemory::set<FLASH_MEMORY::SD_SECTOR_SIZE>(status.toInt());
      request->send(200, "text/plain", "saved");
      break;
    case FLASH_MEMORY::FILE_CHUNK_SIZE:
      flashMemory::set<FLASH_MEMORY::FILE_CHUNK_SIZE>(status.toInt());
      request->send(200, "text/plain", "saved");
      break;
    case FLASH_MEMORY::SD_SPI_SPEED:
      flashMemory::set<FLASH_MEMORY::SD_SPI_SPEED>(status.toInt());
      request->send(200, "text/plain", "saved");
      break;
    case FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS:
      flashMemory::set<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>(status.toInt());
      request->send(200, "text/plain", "saved");
      break;
    case FLASH_MEMORY::DEVSERIAL:
      FLASH_MEMORY::DevSerialConfig devSerialConfig;
      devSerialConfig.baudRate = request->arg("br").toInt();
      devSerialConfig.config = request->arg("c").toInt();
      devSerialConfig.serial = request->arg("s").toInt();
      devSerialConfig.rx = request->arg("rx").toInt();
      devSerialConfig.tx = request->arg("tx").toInt();
      devSerialConfig.custom = request->arg("cm").toInt();
      flashMemory::set<FLASH_MEMORY::DEVSERIAL>(devSerialConfig);
      request->send(200, "text/plain", "saved");
      break;
    case FLASH_MEMORY::DEBSERIAL:
      FLASH_MEMORY::DebugSerialConfig debSerialConfig;
      debSerialConfig.baudRate = request->arg("br").toInt();
      debSerialConfig.config = request->arg("c").toInt();
      debSerialConfig.serial = request->arg("s").toInt();
      debSerialConfig.rx = request->arg("rx").toInt();
      debSerialConfig.tx = request->arg("tx").toInt();
      debSerialConfig.custom = request->arg("cm").toInt();
      debSerialConfig.enabled = request->arg("e").toInt();
      flashMemory::set<FLASH_MEMORY::DEBSERIAL>(debSerialConfig);
      break;
    case FLASH_MEMORY::PRINTER_BUFFER_SIZE:
      flashMemory::set<FLASH_MEMORY::PRINTER_BUFFER_SIZE>(status.toInt());
      request->send(200, "text/plain", "saved");
      break;
    case FLASH_MEMORY::PRINTER_COMMAND_SIZE:
      flashMemory::set<FLASH_MEMORY::PRINTER_COMMAND_SIZE>(status.toInt());
      request->send(200, "text/plain", "saved");
      break;
    case FLASH_MEMORY::PRINTER_TIMEOUT:
      flashMemory::set<FLASH_MEMORY::PRINTER_TIMEOUT>(status.toInt());
      request->send(200, "text/plain", "saved");
      break;
  }
}

void getDynamic(AsyncWebServerRequest* request) {
}

namespace Upload {
TinyMap<String, std::shared_ptr<FsFile>, 100> activeUploads;
void uploadFile(AsyncWebServerRequest* request, const String& filename, const size_t& index, uint8_t* data, const size_t& len, const bool& final) {
  Serial.printf("uploading: %s, idx:%d, len:%d, final:%d \n", filename.c_str(), index, len, final);
  String path = request->arg("path");
  auto SDRequest = std::make_unique<SDM::HANDLER::WebUploadfile>(activeUploads, path, filename, index, data, len, final);
  bool status = SDM::HANDLER::SDHandlerManager.addHandler(std::move(SDRequest));
  if (!status)
    Serial.println("was not added");
}
}
}

static AsyncWebServer* server;

void begin(DevM::GCodeManager* dm) {
  gcodeManager = dm;
  server = new AsyncWebServer(flashMemory::get<FLASH_MEMORY::WEB_SERVER_PORT>());

  Serial.printf("starting web server at port: %d\n", flashMemory::get<FLASH_MEMORY::WEB_SERVER_PORT>());

  server->on("/", HTTP_GET, Handlers::Root::Root);

  server->on("/server/status", HTTP_GET, Handlers::serverStatus);

  server->on("/device/print", HTTP_GET, Handlers::print);
  server->on("/device/pause", HTTP_GET, Handlers::pause);
  server->on("/device/stop", HTTP_GET, Handlers::stop);
  server->on("/device/ems", HTTP_GET, Handlers::ems);
  server->on("/device/console", HTTP_GET, Handlers::notFound);
  server->on("/device/recoveryStatus", HTTP_GET, Handlers::notFound);
  server->on("/device/sendCommand", HTTP_GET, Handlers::sendCommand);

  server->on("/fm/ls", HTTP_GET, Handlers::listFolder);
  server->on("/fm/remove", HTTP_GET, Handlers::remove);
  server->on("/fm/mkdir", HTTP_GET, Handlers::mkdir);
  server->on("/fm/downloadFile", HTTP_GET, Handlers::downloadFile);
  server->on(
    "/fm/uploadFile/", HTTP_POST, [](AsyncWebServerRequest* request) {
      request->send(200);
    },  // Send status 200 (OK) to tell the client we are ready to receive
    Handlers::Upload::uploadFile);

  server->on("/config/setDynamicConfig", HTTP_GET, Handlers::notFound);
  server->on("/config/getDynamicConfig", HTTP_GET, Handlers::notFound);

  server->onNotFound(Handlers::notFound);

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
