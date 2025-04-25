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

void Remove(AsyncWebServerRequest* request) {
  bool status = SDM::remove(request->arg("path"));
  if (status) request->send(200, "text/plain", "File removed successfully");  // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
  else {
    bool exists = SDM::exists(request->arg("path"));
    request->send(500, "text/plain", "failed: " + request->arg("path") + " " + String(status) + " " + String(exists));
  }  // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void DownloadFile(AsyncWebServerRequest* request) {
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

void Mkdir(AsyncWebServerRequest* request) {
  if (SDM::mkdir(request->arg("path")))
    request->send(200, "text/plain", "Directory created succesfully!");
  else
    request->send(500, "text/plain", "Failed to create a directory");
}

void Ems(AsyncWebServerRequest* request) {
  //deviceManager->ems();
  request->send(200, "text/plain", "ems activated");
}

void SendCommand(AsyncWebServerRequest* request) {
  String command = request->arg("command");
  request->send(200, "text/plain", "command sent");  // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

namespace Upload {
FsFile upload_file;
void UploadFile(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
  const String& filepath = request->arg("path");
  String fullpath = filepath.isEmpty() ? "/" : filepath;
  fullpath += filename;

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

void begin(DevM::DeviceManager* dm) {
  deviceManager = dm;
  server = new AsyncWebServer(flashMemory::get<FLASH_MEMORY::WEB_SERVER_PORT>());

  Serial.printf("starting web server at port: %d\n", flashMemory::get<FLASH_MEMORY::WEB_SERVER_PORT>());

  server->on("/", HTTP_GET, Handlers::Root::Root);

  server->on("/server/status", HTTP_GET, Handlers::ServerStatus);
  server->on("/server/pause", HTTP_GET, Handlers::notFound);
  server->on("/server/continue", HTTP_GET, Handlers::notFound);
  server->on("/server/stop", HTTP_GET, Handlers::notFound);
  server->on("/server/ems", HTTP_GET, Handlers::Ems);
  server->on("/server/status", HTTP_GET, Handlers::notFound);
  server->on("/server/console", HTTP_GET, Handlers::notFound);
  server->on("/server/recoveryStatus", HTTP_GET, Handlers::notFound);

  server->on("/device/sendCommand", HTTP_GET, Handlers::SendCommand);

  server->on("/fm/ls", HTTP_GET, Handlers::ListFolder);
  server->on("/fm/remove", HTTP_GET, Handlers::Remove);
  server->on("/fm/mkdir", HTTP_GET, Handlers::notFound);
  server->on("/fm/downloadFile", HTTP_GET, Handlers::DownloadFile);
  server->on("/fm/uploadFile", HTTP_POST, Handlers::notFound);

  server->on("/config/setDynamicConfig", HTTP_GET, Handlers::notFound);
  server->on(
    "/config/getDynamicConfig", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send(200, "text/plain", "Upload complete");
    },
    Handlers::Upload::UploadFile);

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
