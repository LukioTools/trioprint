/*

 Copyright (c) <vuosi> <tekijä>
 
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

bool isNewFirmware(char* firmware) {
  char firmwareVersion[9];
  flashMemory::get<FLASH_MEMORY::FIRMWARE_VERSION>(firmwareVersion);
  firmware = firmwareVersion;
  return strcmp(firmwareVersion, __TIME__) != 0;
}


void resetDynamicMemory() {
  flashMemory::set<FLASH_MEMORY::WEB_SERVER_PORT>((ushort)(80));
  flashMemory::set<FLASH_MEMORY::WEB_SOCKET_PORT>((ushort)(81));

  char ssid[WIFI_SSID_SIZE] = {};
  char pwd[WIFI_PWD_SIZE] = {};
  char ota_pwd[OTA_PWD_SIZE] = {};
  char name[WEB_NAME_SIZE] = {};

  // you can change these to your home credentials. So everytime you reflash the esp you won't have to go trough the configuration phase.
  strncpy(ssid, "trioprint", WIFI_SSID_SIZE);
  strncpy(pwd, "trioprint", WIFI_PWD_SIZE);
  strncpy(name, "trioprint", WEB_NAME_SIZE);

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
  flashMemory::set<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>((FLASH_MEMORY::Ep_sd_card_max_attempts)SD_SPI_TRIES);

  flashMemory::set<FLASH_MEMORY::PIN_SPI_SELECT_PIN>(5);

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
  debugSerialConfig.enabled = 1;
  flashMemory::set<FLASH_MEMORY::DEBSERIAL>(debugSerialConfig);

  flashMemory::set<FLASH_MEMORY::FIRMWARE_VERSION>(__TIME__);

  flashMemory::flush();
  FLASH_MEMORY::isInNeedOfReconfiguration = true;
}

void setup() {

  Serial.begin(250000);
  Serial.println("kek");

  flashMemory::init();


  char firmwareVersion[9];
  bool newFirmware = isNewFirmware(firmwareVersion);
  if (newFirmware) {
    resetDynamicMemory();
    FLASH_MEMORY::isInNeedOfReconfiguration = true;
  }

  SDM::init(flashMemory::get<FLASH_MEMORY::PIN_SPI_SELECT_PIN>());


  FLASH_MEMORY::DebugSerialConfig debugSerialConfig = flashMemory::get<FLASH_MEMORY::DEBSERIAL>();

  //if (!debugSerialConfig.custom)
  //  if (debugSerialConfig.serial == 1)

  Serial.print("\n");
  Serial.printf("needed reset: %d, current firmware version: %s, old version: %s\n", newFirmware, __TIME__, firmwareVersion);

  FLASH_MEMORY::DevSerialConfig devserialConfig = flashMemory::get<FLASH_MEMORY::DEVSERIAL>();


  Serial.printf(
    "timeout: %d\n"
    "buffer size: %d\n"
    "command size: %d\n"
    "sector size: %d\n"
    "chunk size: %d\n"
    "spi tries: %d\n"
    "select pin: %d\n"
    "spi speed: %d\n"
    "dev serial config:\n"
    "  baud rate: %d, custom: %d, serial instance: %d\n"
    "debug serial config:\n"
    "  baud rate: %d, custom: %d, serial instance: %d, enabled: %d\n",
    flashMemory::get<FLASH_MEMORY::PRINTER_TIMEOUT>(),
    flashMemory::get<FLASH_MEMORY::PRINTER_BUFFER_SIZE>(),
    flashMemory::get<FLASH_MEMORY::PRINTER_COMMAND_SIZE>(),
    flashMemory::get<FLASH_MEMORY::SD_SECTOR_SIZE>(),
    flashMemory::get<FLASH_MEMORY::FILE_CHUNK_SIZE>(),
    flashMemory::get<FLASH_MEMORY::SD_CARD_MAX_ATTEMPTS>(),
    flashMemory::get<FLASH_MEMORY::PIN_SPI_SELECT_PIN>(),
    flashMemory::get<FLASH_MEMORY::SD_SPI_SPEED>(),
    devserialConfig.baudRate,
    devserialConfig.custom,
    devserialConfig.serial,
    debugSerialConfig.baudRate,
    debugSerialConfig.custom,
    debugSerialConfig.serial,
    debugSerialConfig.enabled);

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


  if (!SDM::sdCardInitialized)
    SDM::init<true>(flashMemory::get<FLASH_MEMORY::PIN_SPI_SELECT_PIN>());


  GM.Handle();
  OTAW::handle();
  SDM::HANDLER::SDHandlerManager.handle();
}