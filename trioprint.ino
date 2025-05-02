
#include "config.h"
#include "MemoryManager.h"
#include "WiFi.h"
#include "OTA.h"
#include "WebSocket.h"
#include "device.h"
#include "Buffer.h"
#include "webServer.h"

DevM::GCodeManager GM;
DevM::DeviceManager DM;

RuntimeBuffer<char>* testBuffer = nullptr;

bool isNewFirmware() {
  char firmwareVersion[8];
  flashMemory::get<FLASH_MEMORY::FIRMWARE_VERSION>(firmwareVersion);
  return strcmp(firmwareVersion, __TIME__) != 0;
}


void resetDynamicMemory() {
  flashMemory::set<FLASH_MEMORY::WEB_SERVER_PORT>((ushort)(80));
  flashMemory::set<FLASH_MEMORY::WEB_SOCKET_PORT>((ushort)(81));

  char ssid[WIFI_SSID_SIZE] = {};
  char pwd[WIFI_PWD_SIZE] = {};
  char ota_pwd[OTA_PWD_SIZE] = {};
  char name[WEB_NAME_SIZE] = {};
  strncpy(ssid, "", WIFI_SSID_SIZE);
  strncpy(pwd, "", WIFI_PWD_SIZE);
  strncpy(name, "", WEB_NAME_SIZE);

  flashMemory::set<FLASH_MEMORY::WIFI_SSID>(ssid);
  flashMemory::set<FLASH_MEMORY::WIFI_PWD>(pwd);
  flashMemory::set<FLASH_MEMORY::OTA_PWD>(ota_pwd);
  flashMemory::set<FLASH_MEMORY::WEB_NAME>(name);

  flashMemory::set<FLASH_MEMORY::PRINTER_TIMEOUT>(1000);
  flashMemory::set<FLASH_MEMORY::PRINTER_BUFFER_SIZE>(1152);
  flashMemory::set<FLASH_MEMORY::PRINTER_COMMAND_SIZE>(12);

  flashMemory::set<FLASH_MEMORY::SD_SECTOR_SIZE>(512);
  flashMemory::set<FLASH_MEMORY::FILE_CHUNK_SIZE>(1024);
  FLASH_MEMORY::Ep_sd_card_max_attempts attempts = SD_SPI_TRIES;
  flashMemory::set<FLASH_MEMORY::SD_SPI_SPEED>((FLASH_MEMORY::Ep_sd_spi_speed)16);

  FLASH_MEMORY::DevSerialConfig devserialConfig;
  devserialConfig.baudRate = 250000;
  devserialConfig.custom = false;
  devserialConfig.serial = 2;
  flashMemory::set<FLASH_MEMORY::DEVSERIAL>(devserialConfig);

  FLASH_MEMORY::DebugSerialConfig debugSerialConfig;
  debugSerialConfig.baudRate = 250000;
  debugSerialConfig.custom = false;
  debugSerialConfig.serial = 1;
  flashMemory::set<FLASH_MEMORY::DEBSERIAL>(debugSerialConfig);

  flashMemory::set<FLASH_MEMORY::FIRMWARE_VERSION>(__TIME__);

  flashMemory::flush();
}

void setup() {

  flashMemory::init();

  testBuffer = new RuntimeBuffer<char>(10);


  if (isNewFirmware()) {
    resetDynamicMemory();
  }

  FLASH_MEMORY::DebugSerialConfig debugSerialConfig = flashMemory::get<FLASH_MEMORY::DEBSERIAL>();

  if (!debugSerialConfig.custom)
    if (debugSerialConfig.serial == 1)
      Serial.begin(debugSerialConfig.baudRate);


  SDM::init();

  Serial.printf("card size: %d\n", SDM::cardSize());
  Serial.printf("free size: %d\n", SDM::freeSize());

  char carr[WIFI_SSID_SIZE];
  flashMemory::get<FLASH_MEMORY::WIFI_SSID>(carr);
  Serial.println(carr);
  Serial.println(flashMemory::flush() ? "EEPROM commit success" : "EEPROM commit failed");

  WiFiW::begin();
  OTAW::begin();
  WSM.begin();

  DM.begin();

  GM.attachDeviceManager(&DM);

  WBW::begin(&GM);
}

DevM::DeviceManager::PrinterStatus deviceOldStatus;

void loop() {
  DevM::DeviceManager::PrinterStatus deviceStatus = DM.read();

  GM.Handle();
  OTAW::handle();
  SDM::HANDLER::SDHandlerManager.handle();
  //Serial.printf("free ram: %d\n", esp_get_free_heap_size);
}