#include "config.h"
#include "MemoryManager.h"
#include "WiFi.h"
#include "OTA.h"
#include "WebSocket.h"
#include "device.h"
#include "Buffer.h"
#include "webServer.h"

//#define PORT 80
//#define WEB_SOCKET_PORT 81
//static constexpr uint64_t SD_SECTOR_SIZE = 512;
//#define FILE_CHUNK_SIZE 1024
//#define SD_SPI_SPEED SD_SCK_MHZ(16)
//#define SD_CARD_CONNECTION_ATTEMPTS 5
//#define DEVSERIAL 0
//#define DEV_CUSTOM_SERIAL 115200, SERIAL_8N1, 16, 17 //ONLY ESP32 SUPPORT. Use if the DEVSERIAL 0, 1, 2, 3 do not work.

//#define WIFI_SSID_SIZE 16
//#define WIFI_PWD_SIZE 16
//#define OTA_PWD_SIZE 16
//#define WEB_NAME_SIZE 10

WebSocketManager WSM;
DevM::GCodeManager GM;
DevM::DeviceManager DM;

RuntimeBuffer<char>* testBuffer = nullptr;


void setup() {
  Serial.begin(115200);
  Serial.println("kek");

  SDM::init();

  testBuffer = new RuntimeBuffer<char>(10);

  flashMemory::init();

  flashMemory::set<0>((ushort)(80));

  char ssid[WIFI_SSID_SIZE] = {};
  char pwd[WIFI_PWD_SIZE] = {};
  strncpy(ssid, "WS-TIMI-NET", WIFI_SSID_SIZE);
  strncpy(pwd, "Orava#19", WIFI_PWD_SIZE);
  flashMemory::set<FLASH_MEMORY::WIFI_SSID>(ssid);
  flashMemory::set<FLASH_MEMORY::WIFI_PWD>(pwd);

  char carr[WIFI_SSID_SIZE];
  flashMemory::get<FLASH_MEMORY::WIFI_SSID>(carr);
  Serial.println(carr);
  Serial.println(flashMemory::flush() ? "EEPROM commit success" : "EEPROM commit failed");

  WiFiW::begin();
  OTAW::begin();
  WSM.begin();  // Use global WSM

  WBW::begin();
  
  DM.begin();

  GM.startPrint(&DM, "/example.txt");
}


void loop() {
    GM.Handle();
}