
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
#include <memory>

#include "config.h"
#include <SdFat.h>
#include <type_traits>

#include "Buffer.h"

#if defined(ESP8266)
#include <ESP_EEPROM.h>
#else
#include <EEPROM.h>
#endif

#include "TinyMap.h"
#include <ESPAsyncWebServer.h>
#include "stdExtension.h"



#define EEPROM_SIZE 512  // Define the EEPROM size for ESP32

namespace EPRM {

// Primary template.
/// Define a member typedef @c type to one of two argument types.
template<bool _Cond, typename _Iftrue, typename _Iffalse>
struct conditional {
  using type = _Iftrue;
};

// Partial specialization for false.
template<typename _Iftrue, typename _Iffalse>
struct conditional<false, _Iftrue, _Iffalse> {
  using type = _Iffalse;
};


template<std::size_t Index, typename... SholdBeEmpty>
struct type_at {
  //static constexpr std::size_t index = Index;
  using type = void;
};

// Recursive case: get the type at the current index and recurse for the rest
template<std::size_t Index, typename Current, typename... Rest>
struct type_at<Index, Current, Rest...> {
  //static constexpr std::size_t index = (Index == 0 ? 0: Index);
  using type = typename conditional<Index == 0, Current, typename type_at<Index - 1, Rest...>::type>::type;
};

template<uint offset, typename... args>
struct DynamicMemory {

  inline static constexpr size_t begin() {
    return offset;
  }

  inline static constexpr size_t end() {
    return offset + size();
  }

  inline static constexpr size_t size() {
    return (sizeof(args) + ...);
  }

  template<uint index>
  inline static constexpr size_t __helper_size_at() {
    return 0;
  }

  template<uint index, typename first, typename... rest>
  inline static constexpr size_t __helper_size_at() {
    return index == 0 ? sizeof(first) : __helper_size_at<index - 1, rest...>();
  }

  template<int index>
  inline static constexpr size_t size_at() {
    return __helper_size_at<index, args...>();
  }

  template<uint index>
  inline static constexpr size_t __helper_offset_at() {
    return 0;
  }

  template<uint index, typename first, typename... rest>
  inline static constexpr size_t __helper_offset_at() {
    return index == 0 ? 0 : sizeof(first) + __helper_offset_at<index - 1, rest...>();
  }

  template<uint index>
  inline static constexpr size_t offset_at() {
    return __helper_offset_at<index, args...>();
  }

  template<uint index>
  inline static void set(const typename type_at<index, args...>::type& data) {
    EEPROM.put(begin() + offset_at<index>(), data);
  }

  template<uint index>
  inline static void set(const char* data) {
    using TargetType = typename type_at<index, args...>::type;
    static_assert(std::is_array<TargetType>::value, "Target must be an array type");
    static_assert(std::is_same<typename std::remove_extent<TargetType>::type, char>::value, "Target must be an array of char");

    TargetType temp = { 0 };
    strncpy((char*)temp, data, sizeof(temp) - 1);
    EEPROM.put(begin() + offset_at<index>(), temp);
  }

  template<uint index>
  inline static typename type_at<index, args...>::type get() {
    typename type_at<index, args...>::type v;
    EEPROM.get(begin() + offset_at<index>(), v);
    return v;
  }

  template<uint index>
  inline static void get(typename type_at<index, args...>::type& data) {
    EEPROM.get(begin() + offset_at<index>(), data);
  }

  inline static void init() {
    Serial.printf("initing the flash at size: %d\n", end());
    EEPROM.begin(end());
  }

  inline static char flush() {
    return EEPROM.commit();
  }
};
}

namespace FLASH_MEMORY {

bool isInNeedOfReconfiguration = false;

enum NamesEeprom {
  WIFI_SSID,
  WIFI_PWD,
  OTA_PWD,
  WEB_NAME,
  WEB_SERVER_PORT,
  WEB_SOCKET_PORT,
  SD_SECTOR_SIZE,
  FILE_CHUNK_SIZE,
  SD_SPI_SPEED,
  PIN_SPI_SELECT_PIN,
  SD_CARD_MAX_ATTEMPTS,
  DEVSERIAL,
  DEBSERIAL,
  PRINTER_BUFFER_SIZE,
  PRINTER_COMMAND_SIZE,
  PRINTER_TIMEOUT,
  FIRMWARE_VERSION
};

struct DebugSerialConfig {
  uint baudRate, config;
  u_char serial, rx, tx;
  bool custom;
  bool enabled;
};

struct DevSerialConfig {
  uint baudRate, config;
  u_char serial, rx, tx;
  bool custom;
};

using Ep_wifi_ssid = char[WIFI_SSID_SIZE];
using Ep_wifi_pwd = char[WIFI_PWD_SIZE];
using Ep_ota_pwd = char[OTA_PWD_SIZE];
using Ep_web_name = char[WEB_NAME_SIZE];
using Ep_web_server_port = ushort;
using Ep_web_socket_port = ushort;
using Ep_sd_sector_size = uint;
using Ep_file_chunk_size = uint;
using Ep_sd_spi_speed = u_char;
using Ep_sd_spi_ss = u_char;
using Ep_sd_card_max_attempts = u_char;
using Ep_devserial = DevSerialConfig;
using Ep_debserial = DebugSerialConfig;
using Ep_printer_buffer_size = ushort;
using Ep_printer_command_size = ushort;
using Ep_printer_timout = ushort;
using Ep_firmware_version = char[FIRMWARE_VERSION_SIZE];
}

using flashMemory = EPRM::DynamicMemory<0,
                                        FLASH_MEMORY::Ep_wifi_ssid,
                                        FLASH_MEMORY::Ep_wifi_pwd,
                                        FLASH_MEMORY::Ep_ota_pwd,
                                        FLASH_MEMORY::Ep_web_name,
                                        FLASH_MEMORY::Ep_web_server_port,
                                        FLASH_MEMORY::Ep_web_socket_port,
                                        FLASH_MEMORY::Ep_sd_sector_size,
                                        FLASH_MEMORY::Ep_file_chunk_size,
                                        FLASH_MEMORY::Ep_sd_spi_speed,
                                        FLASH_MEMORY::Ep_sd_spi_ss,
                                        FLASH_MEMORY::Ep_sd_card_max_attempts,
                                        FLASH_MEMORY::Ep_devserial,
                                        FLASH_MEMORY::Ep_debserial,
                                        FLASH_MEMORY::Ep_printer_buffer_size,
                                        FLASH_MEMORY::Ep_printer_command_size,
                                        FLASH_MEMORY::Ep_printer_timout,
                                        FLASH_MEMORY::Ep_firmware_version>;

#define WRAPPPER_NAMESPACE SDM
namespace WRAPPPER_NAMESPACE {

SdFs SD;
csd_t csd;

bool sdCardInitialized = false;

template<bool dont_repeat = false>
inline static bool init(SdCsPin_t chip_select_pin = PIN_SPI_SS_SD) {
  int count = 0;
  bool stop_trying = false;
label:
  if (SD.begin(chip_select_pin, SD_SCK_MHZ(flashMemory::get<FLASH_MEMORY::SD_SPI_SPEED>()))) {

    SD.card()->readCSD(&csd);
    sdCardInitialized = true;
    return true;
  } else if (dont_repeat || stop_trying) {
    Serial.printf("max attempts: %d, current attempt: %d, select pin: %d, spi speed: %d\n", flashMemory::get<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>(), count, chip_select_pin, flashMemory::get<FLASH_MEMORY::SD_SPI_SPEED>());
    //SD.initErrorHalt();
    sdCardInitialized = false;
    return false;
  }
  delay(15);
  count++;
  if (count >= flashMemory::get<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>()) {
    stop_trying = true;
  }
  goto label;
}

//cache this
using freebyte_t = uint64_t;
using freebyte_return_t = const freebyte_t&;

constexpr freebyte_t freebyte_invalid_state = 0;
freebyte_t free_bytes = freebyte_invalid_state;
//toughest of lucks
//constexpr uint_t SdSectorSize = 512;

freebyte_t cardSize() {  // full size in bytes
  return SD.clusterCount() * SD.sectorsPerCluster() * flashMemory::get<FLASH_MEMORY::SD_SECTOR_SIZE>();
}

void clearFreeSizeCache() {
  free_bytes = freebyte_invalid_state;
}
void calculateFreeSizeCache() {
  free_bytes = SD.freeClusterCount() * SD.bytesPerCluster();
}
freebyte_return_t freeSize() {  //megabytes hopefully
  if (free_bytes == freebyte_invalid_state) calculateFreeSizeCache();
  return free_bytes;
}
freebyte_return_t refreshFreeSizeCache() {
  clearFreeSizeCache();
  return freeSize();
}

String listDir(String path) {
  FsFile folder = SD.open(path);
  if (!folder) {
    return "Failed to open directory: " + path;
  };
  if (!folder.isDirectory()) return "Not a directory";

  String files = "[ ";
  constexpr size_t filename_max = 32;
  char name[filename_max];

  while (FsFile file = folder.openNextFile()) {
    file.getName(name, filename_max);
    (files += "\"") += name;
    if (file.isDirectory()) files += "/\",";
    else
      files += '_' + String(file.size()) + "\",";
  }

  files[files.length() - 1] = ']';
  folder.close();
  return files;
}

char* readFile(const char* filename, size_t& fileSize) {
  FsFile file = SD.open(filename, oflag_t(O_READ));
  if (!file) {
    return nullptr;
  }

  if (file.isDir()) {
    file.close();
    return nullptr;
  }

  fileSize = file.fileSize();
  auto fileData = new char[fileSize];
  if (file.read(fileData, fileSize) != (long)fileSize) {
    delete[] fileData;
    file.close();
    return nullptr;
  }

  file.close();
  return fileData;
}

char* readFile(const char* filename) {
  FsFile file = SD.open(filename, oflag_t(O_READ));
  if (!file) {
    return nullptr;
  }

  if (file.isDir()) {
    file.close();
    return nullptr;
  }

  size_t fileSize = file.fileSize();
  auto fileData = new char[fileSize];
  if (file.read(fileData, fileSize) != (long)fileSize) {
    delete[] fileData;
    file.close();
    return nullptr;
  }

  file.close();
  return fileData;
}

int readChunk(char* data, FsFile& file, uint32 chunkSize) {
  return file.read(data, chunkSize);
}

bool WriteFile(const char* name, const uint8_t* fileData, size_t size) {
  FsFile file = SD.open(name, oflag_t(O_WRITE | O_CREAT));
  if (file.write(fileData, size) == size) {
    file.close();
    return true;
  }
  file.close();
  return false;
}

/*
      bool WriteFile(FsFile& name, const uint8_t* fileData, size_t size){
        FsFile file = SD.open(name, oflag_t(O_WRITE | O_CREAT));
        if(file.write(fileData, size) == size){
          file.close();
          return true;
        }
        file.close();
        return false;
      }
  */
FsFile openFile(const char* name) {
  return SD.open(name, oflag_t(O_RDWR | O_CREAT));
}

FsFile openFile(const String& name) {
  return SD.open(name.c_str(), oflag_t(O_RDWR | O_CREAT));
}

bool mkdir(const char* path) {
  return SD.mkdir(path, true);
  ;
}

bool mkdir(const String& path) {
  return SD.mkdir(path.c_str(), true);
  ;
}

bool remove(const char* path) {
  return SD.remove(path);
  ;
}

bool remove(const String& path) {
  return SD.remove(path.c_str());
}

bool exists(const String& filename) {
  bool temp = SD.exists(filename);
  return temp;
}

int lineCount(FsFile& file) {
  int lc = 0;
  char line[100];
  while (file.fgets(line, sizeof(line)) > 0) {
    lc++;
  }
  return lc;
}


namespace HANDLER {

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
class Handler {
public:
  virtual bool run() {  //return true when task is completed and can be removed
    Serial.println("kek should be handling sd card shit, but I aint doing that because the sauce was lost on the way");
    return true;
   };
};

class WebRootLoad : public Handler {

  AsyncWebServerRequestPtr requestPtr;

  char* root_cache_data;
  std::size_t& root_cache_size;

public:
  WebRootLoad(AsyncWebServerRequestPtr r, char* rcd, std::size_t& rcs)
    : requestPtr(r), root_cache_data(rcd), root_cache_size(rcs) {}

  bool run() override {
    root_cache_data = readFile(ROOT_FILE, root_cache_size);
    if (requestPtr.expired()) return true;

    if (auto request = requestPtr.lock()) {
      if (!request) {
        return true;
      }
      if (root_cache_data == nullptr) {
        FLASH_MEMORY::isInNeedOfReconfiguration = true;
        AsyncWebServerResponse* response = request->beginResponse(500, "text/html", "reboot the device to configure the file");
        return true;
      }
      AsyncWebServerResponse* response = request->beginResponse(200, "text/html", (uint8_t*)root_cache_data, root_cache_size);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
    return true;
  }
};

class WebListDir : public Handler {
  AsyncWebServerRequestPtr requestPtr;
  const String filename;

public:
  WebListDir(AsyncWebServerRequestPtr r, const String fn)
    : requestPtr(r), filename(fn) {}

  bool run() override {
    auto e = SDM::listDir(filename);
    if (requestPtr.expired()) return true;
    if (auto request = requestPtr.lock()) {
      if (!request) {
        return true;
      }
      request->send(200, "plain/text", e);
    }
    return true;
  }
};

class WebMakeDir : public Handler {
  AsyncWebServerRequestPtr requestPtr;
  const String path;

public:
  WebMakeDir(AsyncWebServerRequestPtr r, const String p)
    : requestPtr(r), path(p) {}

  bool run() override {
    bool status = SDM::mkdir(path);

    if (requestPtr.expired()) return true;
    if (auto request = requestPtr.lock()) {
      if (status)
        request->send(200, "text/plain", "Directory created succesfully!");
      else
        request->send(500, "text/plain", "Failed to create directory");
    }
    return true;
  }
};

class WebRemove : public Handler {
  AsyncWebServerRequestPtr requestPtr;
  const String path;

public:
  WebRemove(AsyncWebServerRequestPtr r, const String p)
    : requestPtr(r), path(p) {}

  bool run() override {
    bool status = SDM::remove(path);

    if (requestPtr.expired()) return true;
    if (auto request = requestPtr.lock()) {
      if (status)
        request->send(200, "text/plain", "removed successfully");
      else
        request->send(500, "text/plain", "failed to remove");
    }
    return true;
  }
};

class WebUploadfile : public Handler {
  String filename;
  size_t index;
  std::unique_ptr<uint8_t[]> buffer;
  size_t buflen;
  bool final;
  TinyMap<String, std::shared_ptr<FsFile>, 100>& activeUploads;
  String fullpath;

public:
  WebUploadfile(TinyMap<String, std::shared_ptr<FsFile>, 100>& au, String p, const String& fn, size_t i, uint8_t* d, size_t l, bool fi)
    : activeUploads(au), fullpath(p), filename(fn), index(i), final(fi), buflen(l) {
    buffer = std::make_unique<uint8_t[]>(l);
    memcpy(buffer.get(), d, l);
  }

  bool run() override {
    if (fullpath.isEmpty()) fullpath = "/";
    fullpath += filename;

    if (index == 0) {
      FsFile file = SDM::SD.open(fullpath.c_str(), O_CREAT | O_WRITE | O_TRUNC);
      if (!file) {
        return true;
      }
      Serial.printf("new: %d\n", activeUploads.insert(fullpath, std::make_shared<FsFile>(std::move(file))));
    }

    auto it = activeUploads.find(fullpath);
    if (it == activeUploads.end()) {
      Serial.println("file instance was not found in map");
      return true;
    }

    std::shared_ptr<FsFile> file = it->second;
    if (!(*file)) {
      Serial.println("file instance was not found");
      return true;
    }

    int amount = file->write(buffer.get(), buflen);  // write from local buffer
    Serial.printf("writing file idx:%d, wrote:%d bytes\n", index, amount);

    file->flush();
    if (final) {
      Serial.println("final, closing file");
      file->close();
    }
    return true;
  }
};

class WebDownloadfile : public Handler {

  AsyncWebServerRequestPtr requestPtr;
  FsFile file;
  AsyncClient* client;
  bool start = true;
  bool headersSent = false;
  String filename;

  int bytesSent = 0;
  int count = 0;

  bool canSend;

  FixedBuffer<uint, 5000> buffer;

public:
  WebDownloadfile(AsyncWebServerRequestPtr r)
    : requestPtr(r) {}

  bool run() override {

    if (start) {
      if (requestPtr.expired()) {
        Serial.println("request expired");
        return true;
      }
      if (auto request = requestPtr.lock()) {
        if (!file) {
          filename = request->arg("filename");
          file = SDM::openFile(filename);
          if (!file || !file.available()) {
            request->send(500, "text/plain", "File not found");
            Serial.println("file not found");
            return true;
          }
        }


        start = false;
        client = request->client();
        if (!client) {
          Serial.println("client not found");
          return true;
        }

        client->onAck([this](void* arg, AsyncClient* c, size_t len, uint32_t time) {
          this->canSend = true;
        });
      }
    }

    if (!headersSent) {
      size_t filesize = file.size();

      String headers =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/octet-stream\r\n"
        "Content-Disposition: attachment; filename=\""
        + filename + "\"\r\n"
                     "Content-Length: "
        + String(filesize) + "\r\n"
                             "Connection: close\r\n"
                             "\r\n";

      client->write(headers.c_str(), headers.length());
      headersSent = true;
      Serial.println("[WebDownloadfile] HTTP headers sent");
      return false;
    }



    if (!(client->connected())) {
      Serial.println("client disconnected");
      return true;
    }

    if (!canSend) return false;

    int bytesRead = file.read(buffer.end(), buffer.available());

    if (bytesRead == -1 || bytesRead == 0) {
      size_t filesize = file.size();
      //Serial.printf("file sent: sent bytes %d and file size: %d, count:%d", bytesSent, filesize, count);
      file.close();
      client->close();
      return true;
    }

    buffer.advanceEnd(bytesRead);

    while (!buffer.empty()) {
      int sent = client->write((char*)buffer.begin(), buffer.used());

      if (sent == 0) {
        canSend = false;
        buffer.finalize();
        return false;
      }

      buffer.consume(sent);
      bytesSent += sent;
    }


    return false;
  }
};

class GCodeInit : public Handler {
  char& stage;
  FsFile& file;
  const size_t BUFFER_SIZE;
  size_t bufferPos;
  size_t bufferLength;
  char* buffer;  // still raw pointer because it is a dynamic array

public:
  GCodeInit(char& d, FsFile& f, size_t bs, size_t bp, size_t bl, char* b)
    : stage(d), file(f), BUFFER_SIZE(bs), bufferPos(bp), bufferLength(bl), buffer(b) {
    stage = 0;
  }

  bool run() override {
    bufferLength = file.readBytes(buffer, BUFFER_SIZE);
    stage = 3;
    return true;
  }
};

class HandlerManager {
  FixedBuffer<std::unique_ptr<Handler>, WEB_REQUESTS> handlers;
  int count = 0;

public:
  bool addHandler(std::unique_ptr<Handler> h) {
    return handlers.push_back(std::move(h));
  }

  void removeHandler(uint8_t idx) {
    handlers.pop(idx);
  }

  void handle() {
    if (handlers.size() == 0) {
      return;
    }

    int shift = 0;
    for (int i = 0; i < handlers.size(); i++) {
      auto handler = handlers.peek();
      bool stage = (*handler)->run();
      if (stage) {
        handlers.pop(i - shift);
        shift++;
      }
    }
  }
};

HandlerManager SDHandlerManager;
}
}  // namespace WRAPPPER_NAMESPACE
