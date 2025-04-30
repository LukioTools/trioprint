
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

void setup() {
  Serial.begin(250000);

  flashMemory::init();

  testBuffer = new RuntimeBuffer<char>(10);

  flashMemory::set<FLASH_MEMORY::WEB_SERVER_PORT>((ushort)(80));
  flashMemory::set<FLASH_MEMORY::WEB_SOCKET_PORT>((ushort)(81));

  char ssid[WIFI_SSID_SIZE] = {};
  char pwd[WIFI_PWD_SIZE] = {};
  char ota_pwd[OTA_PWD_SIZE] = {};
  strncpy(ssid, "WS-TIMI-NET", WIFI_SSID_SIZE);
  strncpy(pwd, "Orava#19", WIFI_PWD_SIZE);
  strncpy(ota_pwd, "12345678", OTA_PWD_SIZE);

  flashMemory::set<FLASH_MEMORY::WIFI_SSID>(ssid);
  flashMemory::set<FLASH_MEMORY::WIFI_PWD>(pwd);
  flashMemory::set<FLASH_MEMORY::OTA_PWD>(ota_pwd);


  flashMemory::set<FLASH_MEMORY::PRINTER_TIMEOUT>(1000);
  flashMemory::set<FLASH_MEMORY::PRINTER_BUFFER_SIZE>(1152);
  flashMemory::set<FLASH_MEMORY::PRINTER_COMMAND_SIZE>(12);
  
  

  flashMemory::set<FLASH_MEMORY::SD_SECTOR_SIZE>(512);
  flashMemory::set<FLASH_MEMORY::FILE_CHUNK_SIZE>(1024);
  FLASH_MEMORY::Ep_sd_card_max_attempts attempts = 10;
  flashMemory::set<FLASH_MEMORY::SD_SPI_SPEED>((FLASH_MEMORY::Ep_sd_spi_speed)16);

  FLASH_MEMORY::DevSerialConfig serialConfig;
  serialConfig.baudRate = 250000;
  serialConfig.custom = false;
  serialConfig.serial = 2;

  flashMemory::set<FLASH_MEMORY::DEVSERIAL>(serialConfig);

  flashMemory::flush();

  Serial.printf("save: %d\n", flashMemory::get<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>());

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

/*
  if(deviceStatus == DevM::DeviceManager::Offline && deviceStatus != deviceOldStatus) {
    Serial.println("Printer is offline");
    deviceOldStatus = deviceStatus;
  } else if(deviceStatus == DevM::DeviceManager::Idle && deviceStatus != deviceOldStatus) {
    Serial.println("Printer is idling");
    deviceOldStatus = deviceStatus;
  } else if(deviceStatus == DevM::DeviceManager::Busy && deviceStatus != deviceOldStatus) {
    Serial.println("Printer is busy and can't handle input currently");
    deviceOldStatus = deviceStatus;
  } else if(deviceStatus == DevM::DeviceManager::CommandSuccess && deviceStatus != deviceOldStatus) {
    Serial.println("Printer ran the command successfully");
    deviceOldStatus = deviceStatus;
  } else if(deviceStatus == DevM::DeviceManager::ColdExtrusion && deviceStatus != deviceOldStatus) {
    Serial.println("Printer prevented cold extrusion. Turn off the printer!");
    deviceOldStatus = deviceStatus;
  } else if(deviceStatus == DevM::DeviceManager::Error && deviceStatus != deviceOldStatus) {
    Serial.println("Error occured. (likely a unknown command!)");
    deviceOldStatus = deviceStatus;
  }
  */
  GM.Handle();
  OTAW::handle();
  SDM::HANDLER::SDHandlerManager.handle();
}