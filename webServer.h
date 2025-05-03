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
#include "config_html_template.h"

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

void configRoot(AsyncWebServerRequest* request) {
  request->send(200, "text/html", CONFIGURATION_HTML_TEMPLATE);
}

void Root(AsyncWebServerRequest* request) {
  Serial.println("client");
  if (FLASH_MEMORY::isInNeedOfReconfiguration) {
    configRoot(request);
    return;
  }
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

void reboot(AsyncWebServerRequest* request) {
  request->send(200, "text/plain", "rebooted");
  delay(100);
  ESP.restart();
}

void serverStatus(AsyncWebServerRequest* request) {

  StringStream responce = "{";
  responce
    << "\"cardSize\":" << String(SDM::cardSize()) << ","
    << "\"freeSpace\":" << String(SDM::freeSize()) << ","
    << "}";
  request->send(200, "text/plain", responce.str());
}

void sdCardStatus(AsyncWebServerRequest* request) {
  String response = "{\"cardStatus\":" + String(SDM::sdCardInitialized ? "true" : "false") + "}";
  request->send(200, "application/json", response);
}

void serverFirmwareVersion(AsyncWebServerRequest* request){
  char version[FIRMWARE_VERSION_SIZE];
  flashMemory::get<FLASH_MEMORY::FIRMWARE_VERSION>(version);
  request->send(200, "text/plain", String(version));
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
  String config = request->arg("config");  // Config identifier
  String status = request->arg("status");  // Value to set

  Serial.printf("config: %s saved: %s\n", config, status);

  switch (static_cast<FLASH_MEMORY::NamesEeprom>(config.toInt())) {
    case FLASH_MEMORY::WIFI_SSID:
      if (status.length() <= WIFI_SSID_SIZE) {
        flashMemory::set<FLASH_MEMORY::WIFI_SSID>(status.c_str());
        request->send(200, "text/plain", "saved");
      } else {
        request->send(413, "text/plain", "too long");
      }
      break;

    case FLASH_MEMORY::WIFI_PWD:
      if (status.length() <= WIFI_PWD_SIZE) {
        flashMemory::set<FLASH_MEMORY::WIFI_PWD>(status.c_str());
        request->send(200, "text/plain", "saved");
      } else {
        request->send(413, "text/plain", "too long");
      }
      break;

    case FLASH_MEMORY::OTA_PWD:
      if (status.length() <= OTA_PWD_SIZE) {
        flashMemory::set<FLASH_MEMORY::OTA_PWD>(status.c_str());
        request->send(200, "text/plain", "saved");
      } else {
        request->send(413, "text/plain", "too long");
      }
      break;

    case FLASH_MEMORY::WEB_NAME:
      if (status.length() <= WEB_NAME_SIZE) {
        flashMemory::set<FLASH_MEMORY::WEB_NAME>(status.c_str());
        request->send(200, "text/plain", "saved");
      } else {
        request->send(413, "text/plain", "too long");
      }
      break;

    case FLASH_MEMORY::WEB_SERVER_PORT:
      flashMemory::set<FLASH_MEMORY::WEB_SERVER_PORT>(static_cast<uint16_t>(status.toInt()));
      request->send(200, "text/plain", "saved");
      break;

    case FLASH_MEMORY::WEB_SOCKET_PORT:
      flashMemory::set<FLASH_MEMORY::WEB_SOCKET_PORT>(static_cast<uint16_t>(status.toInt()));
      request->send(200, "text/plain", "saved");
      break;

    case FLASH_MEMORY::SD_SECTOR_SIZE:
      flashMemory::set<FLASH_MEMORY::SD_SECTOR_SIZE>(static_cast<uint32_t>(status.toInt()));
      request->send(200, "text/plain", "saved");
      break;

    case FLASH_MEMORY::FILE_CHUNK_SIZE:
      flashMemory::set<FLASH_MEMORY::FILE_CHUNK_SIZE>(static_cast<uint32_t>(status.toInt()));
      request->send(200, "text/plain", "saved");
      break;

    case FLASH_MEMORY::SD_SPI_SPEED:
      flashMemory::set<FLASH_MEMORY::SD_SPI_SPEED>(static_cast<uint8_t>(status.toInt()));
      request->send(200, "text/plain", "saved");
      break;

    case FLASH_MEMORY::PIN_SPI_SELECT_PIN:
      flashMemory::set<FLASH_MEMORY::PIN_SPI_SELECT_PIN>(static_cast<uint8_t>(status.toInt()));
      request->send(200, "text/plain", "saved");
      break;

    case FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS:
      flashMemory::set<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>(static_cast<uint8_t>(status.toInt()));
      request->send(200, "text/plain", "saved");
      break;

    case FLASH_MEMORY::DEVSERIAL:
      {
        FLASH_MEMORY::DevSerialConfig devSerialConfig;
        devSerialConfig.baudRate = request->arg("br").toInt();
        devSerialConfig.config = request->arg("c").toInt();
        devSerialConfig.serial = request->arg("s").toInt();
        devSerialConfig.rx = request->arg("rx").toInt();
        devSerialConfig.tx = request->arg("tx").toInt();
        devSerialConfig.custom = request->arg("cm").toInt();

        Serial.printf("baud rate: %d\n"
                      "config: %d\n"
                      "serial: %d\n"
                      "rx: %d\n"
                      "tx: %d\n"
                      "custom: %d\n",
                      devSerialConfig.baudRate,
                      devSerialConfig.config,
                      devSerialConfig.serial,
                      devSerialConfig.rx,
                      devSerialConfig.tx,
                      devSerialConfig.custom);

        flashMemory::set<FLASH_MEMORY::DEVSERIAL>(devSerialConfig);
        request->send(200, "text/plain", "saved");
        break;
      }

    case FLASH_MEMORY::DEBSERIAL:
      {
        FLASH_MEMORY::DebugSerialConfig debSerialConfig;
        debSerialConfig.baudRate = request->arg("br").toInt();
        debSerialConfig.config = request->arg("c").toInt();
        debSerialConfig.serial = request->arg("s").toInt();
        debSerialConfig.rx = request->arg("rx").toInt();
        debSerialConfig.tx = request->arg("tx").toInt();
        debSerialConfig.custom = request->arg("cm").toInt();
        debSerialConfig.enabled = request->arg("e").toInt();

        Serial.printf("baud rate: %d\n"
                      "config: %d\n"
                      "serial: %d\n"
                      "rx: %d\n"
                      "tx: %d\n"
                      "custom: %d\n",
                      debSerialConfig.baudRate,
                      debSerialConfig.config,
                      debSerialConfig.serial,
                      debSerialConfig.rx,
                      debSerialConfig.tx,
                      debSerialConfig.custom,
                      debSerialConfig.enabled);

        flashMemory::set<FLASH_MEMORY::DEBSERIAL>(debSerialConfig);
        request->send(200, "text/plain", "saved");
        break;
      }

    case FLASH_MEMORY::PRINTER_BUFFER_SIZE:
      flashMemory::set<FLASH_MEMORY::PRINTER_BUFFER_SIZE>(static_cast<uint16_t>(status.toInt()));
      request->send(200, "text/plain", "saved");
      break;

    case FLASH_MEMORY::PRINTER_COMMAND_SIZE:
      flashMemory::set<FLASH_MEMORY::PRINTER_COMMAND_SIZE>(static_cast<uint16_t>(status.toInt()));
      request->send(200, "text/plain", "saved");
      break;

    case FLASH_MEMORY::PRINTER_TIMEOUT:
      flashMemory::set<FLASH_MEMORY::PRINTER_TIMEOUT>(static_cast<uint16_t>(status.toInt()));
      request->send(200, "text/plain", "saved");
      break;

    case FLASH_MEMORY::FIRMWARE_VERSION:
      if (status.length() <= FIRMWARE_VERSION_SIZE) {
        flashMemory::set<FLASH_MEMORY::FIRMWARE_VERSION>(status.c_str());
        request->send(200, "text/plain", "saved");
      } else {
        request->send(413, "text/plain", "too long");
      }
      break;
  }
  flashMemory::flush();
}

void getDynamic(AsyncWebServerRequest* request) {
  String config = request->arg("config");  // Config identifier

  Serial.printf("config: %s read\n", config.c_str());

  switch (static_cast<FLASH_MEMORY::NamesEeprom>(config.toInt())) {
    case FLASH_MEMORY::WIFI_SSID:
      {
        char ssid[WIFI_SSID_SIZE] = { 0 };
        flashMemory::get<FLASH_MEMORY::WIFI_SSID>(ssid);
        request->send(200, "text/plain", ssid);
        break;
      }

    case FLASH_MEMORY::WIFI_PWD:
      {
        char pwd[WIFI_PWD_SIZE] = { 0 };
        flashMemory::get<FLASH_MEMORY::WIFI_PWD>(pwd);
        request->send(200, "text/plain", pwd);
        break;
      }

    case FLASH_MEMORY::OTA_PWD:
      {
        char pwd[OTA_PWD_SIZE] = { 0 };
        flashMemory::get<FLASH_MEMORY::OTA_PWD>(pwd);
        request->send(200, "text/plain", pwd);
        break;
      }

    case FLASH_MEMORY::WEB_NAME:
      {
        char webName[WEB_NAME_SIZE] = { 0 };
        flashMemory::get<FLASH_MEMORY::WEB_NAME>(webName);
        request->send(200, "text/plain", webName);
        break;
      }

    case FLASH_MEMORY::WEB_SERVER_PORT:
      {
        uint16_t port = flashMemory::get<FLASH_MEMORY::WEB_SERVER_PORT>();
        request->send(200, "text/plain", String(port));
        break;
      }

    case FLASH_MEMORY::WEB_SOCKET_PORT:
      {
        uint16_t port = flashMemory::get<FLASH_MEMORY::WEB_SOCKET_PORT>();
        request->send(200, "text/plain", String(port));
        break;
      }

    case FLASH_MEMORY::SD_SECTOR_SIZE:
      {
        uint32_t sectorSize = flashMemory::get<FLASH_MEMORY::SD_SECTOR_SIZE>();
        request->send(200, "text/plain", String(sectorSize));
        break;
      }

    case FLASH_MEMORY::FILE_CHUNK_SIZE:
      {
        uint32_t chunkSize = flashMemory::get<FLASH_MEMORY::FILE_CHUNK_SIZE>();
        request->send(200, "text/plain", String(chunkSize));
        break;
      }

    case FLASH_MEMORY::SD_SPI_SPEED:
      {
        uint8_t spiSpeed = flashMemory::get<FLASH_MEMORY::SD_SPI_SPEED>();
        request->send(200, "text/plain", String(spiSpeed));
        break;
      }

    case FLASH_MEMORY::PIN_SPI_SELECT_PIN:
      {
        uint8_t selectPin = flashMemory::get<FLASH_MEMORY::PIN_SPI_SELECT_PIN>();
        request->send(200, "text/plain", String(selectPin));
        break;
      }

    case FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS:
      {
        uint8_t maxAttempts = flashMemory::get<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>();
        request->send(200, "text/plain", String(maxAttempts));
        break;
      }

    case FLASH_MEMORY::DEVSERIAL:
      {
        FLASH_MEMORY::DevSerialConfig devSerialConfig;
        flashMemory::get<FLASH_MEMORY::DEVSERIAL>(devSerialConfig);
        String response = String(devSerialConfig.baudRate) + "," + String(devSerialConfig.config) + "," + String(devSerialConfig.serial) + "," + String(devSerialConfig.rx) + "," + String(devSerialConfig.tx) + "," + String(devSerialConfig.custom);
        request->send(200, "text/plain", response);
        break;
      }

    case FLASH_MEMORY::DEBSERIAL:
      {
        FLASH_MEMORY::DebugSerialConfig debSerialConfig;
        flashMemory::get<FLASH_MEMORY::DEBSERIAL>(debSerialConfig);
        String response = String(debSerialConfig.baudRate) + "," + String(debSerialConfig.config) + "," + String(debSerialConfig.serial) + "," + String(debSerialConfig.rx) + "," + String(debSerialConfig.tx) + "," + String(debSerialConfig.custom) + "," + String(debSerialConfig.enabled);
        request->send(200, "text/plain", response);
        break;
      }

    case FLASH_MEMORY::PRINTER_BUFFER_SIZE:
      {
        uint16_t bufferSize = flashMemory::get<FLASH_MEMORY::PRINTER_BUFFER_SIZE>();
        request->send(200, "text/plain", String(bufferSize));
        break;
      }

    case FLASH_MEMORY::PRINTER_COMMAND_SIZE:
      {
        uint16_t cmdSize = flashMemory::get<FLASH_MEMORY::PRINTER_COMMAND_SIZE>();
        request->send(200, "text/plain", String(cmdSize));
        break;
      }

    case FLASH_MEMORY::PRINTER_TIMEOUT:
      {
        uint16_t timeout = flashMemory::get<FLASH_MEMORY::PRINTER_TIMEOUT>();
        request->send(200, "text/plain", String(timeout));
        break;
      }

    case FLASH_MEMORY::FIRMWARE_VERSION:
      {
        char firmware[FIRMWARE_VERSION_SIZE] = { 0 };
        flashMemory::get<FLASH_MEMORY::FIRMWARE_VERSION>(firmware);
        request->send(200, "text/plain", firmware);
        break;
      }

    default:
      request->send(400, "text/plain", "Invalid config");
      break;
  }
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
  server->on("/server/config", HTTP_GET, Handlers::Root::configRoot);
  server->on("/server/sdCardStatus", HTTP_GET, Handlers::sdCardStatus);
  server->on("/server/reboot", HTTP_GET, Handlers::reboot);
  server->on("/server/version", HTTP_GET, Handlers::reboot);

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

  server->on("/config/setDynamicConfig", HTTP_POST, Handlers::setDynamic);
  server->on("/config/getDynamicConfig", HTTP_GET, Handlers::getDynamic);

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
