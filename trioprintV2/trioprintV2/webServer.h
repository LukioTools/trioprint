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
    auto SDRequest = std::make_unique<SDM::HANDLER::WebRootLoad>(request->pause(), &root_cache_data, &root_cache_size);
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
  bool status = SDM::remove(request->arg("path"));
  if (status) request->send(200, "text/plain", "File removed successfully");
  else {
    bool exists = SDM::exists(request->arg("path"));
    request->send(500, "text/plain", "failed: " + request->arg("path") + " " + String(status) + " " + String(exists));
  }
}

void downloadFile(AsyncWebServerRequest* request) {
  if (!request->hasArg("filename")) {
    request->send(400, "text/plain", "Missing filename parameter");
    return;
  }

  String filename = request->arg("filename");
  FsFile* file = new FsFile(SDM::openFile(filename));  // Allocate on heap
  if (!file || !file->available()) {
    delete file;
    request->send(404, "text/plain", "File not found");
    return;
  }

  AsyncWebServerResponse* response = request->beginChunkedResponse(
    "application/octet-stream",
    [file](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
      if (!file->available()) {
        file->close();
        delete file;
        return 0;
      }
      return file->read(buffer, maxLen);
    });

  response->addHeader("Content-Disposition", "attachment; filename=\"" + filename + "\"");
  request->send(response);
}

void mkdir(AsyncWebServerRequest* request) {
  if (SDM::mkdir(request->arg("path")))
    request->send(200, "text/plain", "Directory created succesfully!");
  else
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

namespace Upload {
FsFile upload_file;
void uploadFile(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
  const String& filepath = request->arg("path");
  String fullpath = filepath.isEmpty() ? "/" : filepath;
  fullpath += filename;

  Serial.printf("uploading file to sd card: filename: %s, len:%d\n", fullpath, len);

  if (index == 0) {
    upload_file = SDM::SD.open(fullpath.c_str(), O_CREAT | O_WRITE | O_TRUNC);
  }

  if (upload_file) {
    size_t amount = upload_file.write(data, len);
  }

  if (final) {
    upload_file.close();
    if (filename == ROOT_FILE && fullpath == String("/") + ROOT_FILE) {
      Root::RootReloadCache();
    }
  }
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
  server->on("/fm/uploadFile/", HTTP_POST, [](AsyncWebServerRequest* request) {
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
