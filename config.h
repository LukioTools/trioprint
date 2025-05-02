#pragma once
#include "SdFat.h"

// sd card select pin
#define PIN_SPI_SS 5

#if defined(ESP32)  //Hate to do these things but ig I have to. IDK why can't they be the same on esp8266 and esp32
#define uint8 uint8_t
#define uint32 uint32_t
#elif defined(ESP8266)
#endif

// eeprom char array sizes.
//  Can edit:
#define WIFI_SSID_SIZE 16
#define WIFI_PWD_SIZE 16
#define OTA_PWD_SIZE 16
#define WEB_NAME_SIZE 16

// do not edit:
#define ROOT_FILE_SIZE 16
#define FIRMWARE_VERSION_SIZE 8

#define WIFI_CONNECTING_TIMEMOUT 5000

// how many web server request can be handled at once
#define WEB_REQUESTS 1000

// character that defines comments in gcode
#define COMMENTCHAR ";"

// this is the filename of file that is the main html page.
#define ROOT_FILE "compiled.html.gz"

// End commands. You should check if these are correct ones for your printer
static constexpr const char* END_COMMANDS[] = {
  "M17",               // Engage stepper motors (optional)
  "G91",               // Set to relative positioning
  "G1 Z10 F300",       // Move nozzle up 10mm
  "G90",               // Set to absolute positioning
  "G0 X0 Y200 F6000",  // Move bed forward (adjust Y200 as needed)
  "M104 S0",           // Turn off hotend heater
  "M140 S0",           // Turn off bed heater
  "M106 S0",           // Turn off fan
  "M84"                // Disable stepper motors
};

// ems command. check your printer documentation. This one is for marlin firmware.
static constexpr const char* EMS_COMMANDS[] = {
  "M112",
};
