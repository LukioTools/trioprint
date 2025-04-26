#pragma once

#include "config.h"
#include <SdFat.h>
#include <type_traits>

#include "Buffer.h"

#if defined(ESP8266)
#include <ESP_EEPROM.h>
#else
#include <EEPROM.h>
#endif

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

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
  inline static void get(typename type_at<index, args...>::type& data) {
    EEPROM.get(begin() + offset_at<index>(), data);
  }


  template<uint index>
  inline static typename type_at<index, args...>::type get() {
    typename type_at<index, args...>::type v;
    EEPROM.get(begin() + offset_at<index>(), v);
    return v;
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

enum NamesEeprom {
  WEB_SERVER_PORT,
  WEB_SOCKET_PORT,
  SD_SECTOR_SIZE,
  FILE_CHUNK_SIZE,
  SD_SPI_SPEED,
  SD_CARD_MAX_ATTEMPTS,
  DEVSERIAL,
  PRINTER_BUFFER_SIZE,
  PRINTER_COMMAND_SIZE,
  WIFI_SSID,
  WIFI_PWD,
  OTA_PWD,
  WEB_NAME,
  PRINTER_TIMEOUT,
};

struct DevSerialConfig {
  uint baudRate, config;
  u_char serial, rx, tx;
  bool custom;
};


using Ep_web_server_port = ushort;
using Ep_web_socket_port = ushort;
using Ep_sd_sector_size = uint;
using Ep_file_chunk_size = uint;
using Ep_sd_spi_speed = u_char;
using Ep_sd_card_max_attempts = u_char;
using Ep_devserial = DevSerialConfig;
using Ep_printer_buffer_size = ushort;
using Ep_printer_command_size = ushort;
using Ep_wifi_ssid = char[WIFI_SSID_SIZE];
using Ep_wifi_pwd = char[WIFI_PWD_SIZE];
using Ep_ota_pwd = char[OTA_PWD_SIZE];
using Ep_web_name = char[WEB_NAME_SIZE];
using Ep_printer_timout = ushort;

}

using flashMemory = EPRM::DynamicMemory<0,
                                        FLASH_MEMORY::Ep_web_server_port,
                                        FLASH_MEMORY::Ep_web_socket_port,
                                        FLASH_MEMORY::Ep_sd_sector_size,
                                        FLASH_MEMORY::Ep_file_chunk_size,
                                        FLASH_MEMORY::Ep_sd_spi_speed,
                                        FLASH_MEMORY::Ep_sd_card_max_attempts,
                                        FLASH_MEMORY::Ep_devserial,
                                        FLASH_MEMORY::Ep_printer_buffer_size,
                                        FLASH_MEMORY::Ep_printer_command_size,
                                        FLASH_MEMORY::Ep_wifi_ssid,
                                        FLASH_MEMORY::Ep_wifi_pwd,
                                        FLASH_MEMORY::Ep_ota_pwd,
                                        FLASH_MEMORY::Ep_web_name,
                                        FLASH_MEMORY::Ep_printer_timout>;

#define WRAPPPER_NAMESPACE SDM
namespace WRAPPPER_NAMESPACE {


SdFs SD;
csd_t csd;

bool sdCardAvailable = true;

template<bool dont_repeat = false>
inline static bool init(SdCsPin_t chip_select_pin = PIN_SPI_SS) {
  int count = 0;
  bool stop_trying = false;
label:
  if (SD.begin(chip_select_pin, SD_SCK_MHZ(flashMemory::get<FLASH_MEMORY::SD_SPI_SPEED>()))) {

    SD.card()->readCSD(&csd);
    return true;
  } else if (dont_repeat || stop_trying) {
    Serial.printf("max attempts: %d, current attempt: %d, select pin: %d, spi speed: %d", flashMemory::get<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>(), count, chip_select_pin, flashMemory::get<FLASH_MEMORY::SD_SPI_SPEED>());
    SD.initErrorHalt();
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
#if defined(ESP8266)
  return sdCardCapacity(&csd) * SD_SECTOR_SIZE;
#elif defined(ESP32)
  return SD.clusterCount() * SD.sectorsPerCluster() * flashMemory::get<FLASH_MEMORY::SD_SECTOR_SIZE>();
#endif
}

void clearFreeSizeCache() {
  free_bytes = freebyte_invalid_state;
}
//takes fucking six seconds!
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
  sdCardAvailable = false;
  FsFile folder = SD.open(path);
  if (!folder) {
    sdCardAvailable = true;
    return "Failed to open directory";
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
  sdCardAvailable = true;
  return files;
}

char* readFile(const char* filename, size_t& fileSize) {
  sdCardAvailable = false;
  FsFile file = SD.open(filename, oflag_t(O_READ));
  if (!file) {
    sdCardAvailable = true;
    return nullptr;
  }

  if (file.isDir()) {
    file.close();
    sdCardAvailable = true;
    return nullptr;
  }

  fileSize = file.fileSize();
  auto fileData = new char[fileSize];
  if (file.read(fileData, fileSize) != (long)fileSize) {
    delete[] fileData;
    file.close();
    sdCardAvailable = true;
    return nullptr;
  }

  file.close();
  sdCardAvailable = true;
  return fileData;
}

char* readFile(const char* filename) {
  sdCardAvailable = false;
  FsFile file = SD.open(filename, oflag_t(O_READ));
  if (!file) {
    sdCardAvailable = true;
    return nullptr;
  }

  if (file.isDir()) {
    file.close();
    sdCardAvailable = true;
    return nullptr;
  }

  size_t fileSize = file.fileSize();
  auto fileData = new char[fileSize];
  if (file.read(fileData, fileSize) != (long)fileSize) {
    delete[] fileData;
    file.close();
    sdCardAvailable = true;
    return nullptr;
  }

  file.close();
  sdCardAvailable = true;
  return fileData;
}

int readChunk(char* data, FsFile& file, uint32 chunkSize) {
  sdCardAvailable = false;
  return file.read(data, chunkSize);
  sdCardAvailable = true;
}

bool WriteFile(const char* name, const uint8_t* fileData, size_t size) {
  sdCardAvailable = false;
  FsFile file = SD.open(name, oflag_t(O_WRITE | O_CREAT));
  if (file.write(fileData, size) == size) {
    file.close();
    sdCardAvailable = true;
    return true;
  }
  file.close();
  sdCardAvailable = true;
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
  sdCardAvailable = false;
  FsFile temp = SD.open(name, oflag_t(O_RDWR | O_CREAT));
  sdCardAvailable = true;
  return temp;
}

FsFile openFile(const String& name) {
  sdCardAvailable = false;
  FsFile temp = SD.open(name.c_str(), oflag_t(O_RDWR | O_CREAT));
  sdCardAvailable = true;
  return temp;
}

bool mkdir(const char* path) {
  sdCardAvailable = false;
  bool temp = SD.mkdir(path, true);
  sdCardAvailable = true;
  return temp;
}

bool mkdir(const String& path) {
  sdCardAvailable = false;
  bool temp = SD.mkdir(path.c_str(), true);
  sdCardAvailable = true;
  return temp;
}

bool remove(const char* path) {
  sdCardAvailable = false;
  bool temp = SD.remove(path);
  sdCardAvailable = true;
  return temp;
}

bool remove(const String& path) {
  sdCardAvailable = false;
  bool temp = SD.remove(path.c_str());
  sdCardAvailable = true;
  return temp;
}

bool exists(const String& filename) {
  sdCardAvailable = false;
  bool temp = SD.exists(filename);
  sdCardAvailable = true;
  return temp;
}

int lineCount(FsFile& file) {
  sdCardAvailable = false;
  int lc = 0;
  char line[100];
  while (file.fgets(line, sizeof(line)) > 0) {
    lc++;
  }
  sdCardAvailable = true;
  return lc;
}


namespace HANDLER {

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
class Handler {
public:
  virtual void run() {
    Serial.println("kek should be handling shit, but I aint doing that");
  };
};

class WebRootLoad : public Handler {

  AsyncWebServerRequestPtr requestPtr;

  char** root_cache_data = nullptr;
  std::size_t* root_cache_size = nullptr;

public:
  WebRootLoad(AsyncWebServerRequestPtr r, char** rcd, std::size_t* rcs)
    : requestPtr(r), root_cache_data(rcd), root_cache_size(rcs) {}

  void run() override {
    *root_cache_data = readFile(ROOT_FILE, *root_cache_size);

    if (requestPtr.expired()) return;

    if (auto request = requestPtr.lock()) {
      if (!request) {
        Serial.println("Couldn't lock request");
        return;
      }
      AsyncWebServerResponse* response = request->beginResponse(200, "text/html", (uint8_t*)*root_cache_data, *root_cache_size);  //Sends 404 File Not Found
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
  }
};

class WebListDir : public Handler {
  AsyncWebServerRequestPtr requestPtr;
  String filename;

public:
  WebListDir(AsyncWebServerRequestPtr r, String fn)
    : requestPtr(r), filename(fn) {}

  void run() override {
    auto e = SDM::listDir(filename);
    if (requestPtr.expired()) return;
    if (auto request = requestPtr.lock()) {
      if (!request) {
        Serial.println("Couldn't lock request");
        return;
      }
      request->send(200, "plain/text", e);
    }
  }
};


class GCodeInit : public Handler {
  char* stage = nullptr;
  FsFile* file = nullptr;
  size_t BUFFER_SIZE;
  size_t* bufferPos = nullptr;
  size_t* bufferLength = nullptr;
  char* buffer = nullptr;

public:
  GCodeInit(char* d, FsFile* f, size_t bs, size_t* bp, size_t* bl, char* b)
    : stage(d), file(f), BUFFER_SIZE(bs), bufferPos(bp), bufferLength(bl), buffer(b) {
    *stage = 0;
  }

  void run() override {
    Serial.println("starting to read in fuck");
    *bufferLength = file->readBytes(buffer, BUFFER_SIZE);
    Serial.println("read done in fuck");
    *stage = 1;
  }
};


class HandlerManager {
  FixedBuffer<std::unique_ptr<Handler>, 10> handlers;
  int count = 0;

public:
  void addHandler(std::unique_ptr<Handler> h) {
    handlers.push_back(std::move(h));
  }

  void removeHandler(uint8_t idx) {
    handlers.pop(idx);
  }

  void handle() {
    if (handlers.size() == 0) {
      return;
    }
    Serial.printf("thre is: %d \n", handlers.size());

    while (auto handler = handlers.read()) {  // 'read()' returns the first handler and removes it from the buffer
      if (handler == nullptr) {
        Serial.println("handler was nullptr, skip");
        continue;
      }
      (*handler)->run();

      // You can check if you need to perform additional removal logic or leave it to read() which already removes the item
    }
  }
};

HandlerManager SDHandlerManager;

}
}  // namespace WRAPPPER_NAMESPACE
