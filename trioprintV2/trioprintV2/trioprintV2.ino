
#include "config.h"
#include "MemoryManager.h"
#include "WiFi.h"
#include "OTA.h"
#include "WebSocket.h"
#include "device.h"
#include "Buffer.h"
#include "webServer.h"

WebSocketManager WSM;
DevM::GCodeManager GM;
DevM::DeviceManager DM;

RuntimeBuffer<char>* testBuffer = nullptr;

void setup() {
  Serial.begin(115200);

  flashMemory::init();

  testBuffer = new RuntimeBuffer<char>(10);

  flashMemory::set<0>((ushort)(80));

  char ssid[WIFI_SSID_SIZE] = {};
  char pwd[WIFI_PWD_SIZE] = {};
  char ota_pwd[OTA_PWD_SIZE] = {};
  strncpy(ssid, "WS-TIMI-NET", WIFI_SSID_SIZE);
  strncpy(pwd, "Orava#19", WIFI_PWD_SIZE);
  strncpy(ota_pwd, "12345678", OTA_PWD_SIZE);
  FLASH_MEMORY::Ep_sd_card_max_attempts attempts = 10;

  flashMemory::set<FLASH_MEMORY::WIFI_SSID>(ssid);
  flashMemory::set<FLASH_MEMORY::WIFI_PWD>(pwd);
  flashMemory::set<FLASH_MEMORY::OTA_PWD>(ota_pwd);
  flashMemory::set<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>(attempts);
  flashMemory::set<FLASH_MEMORY::SD_SPI_SPEED>((FLASH_MEMORY::Ep_sd_spi_speed)16);

  FLASH_MEMORY::DevSerialConfig serialConfig;
  serialConfig.baudRate = 115200;
  serialConfig.custom = false;
  serialConfig.serial = 2;

  flashMemory::set<FLASH_MEMORY::DEVSERIAL>(serialConfig);

  flashMemory::flush();

  Serial.printf("save: %d\n", flashMemory::get<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>());

  SDM::init();

  WiFiW::begin();
  OTAW::begin();
  WSM.begin();

  DM.begin();

  GM.attachDeviceManager(&DM);

  WBW::begin(&GM);
}

void loop() {
  GM.Handle();
  OTAW::handle();
  SDM::HANDLER::SDHandlerManager.handle();
}