#pragma once
#include "SdFat.h"

#if defined(ESP32) //Hate to do these things but ig I have to. IDK why can't they be the same on esp8266 and esp32
    #define uint8 uint8_t
    #define uint32 uint32_t

    #define PIN_SPI_SS 5 //17 for my printer
#elif defined(ESP8266)
    // nothing here
#endif

// WIFI
#define WIFI_SSID ""
#define WIFI_PWD ""

// OTA
#define OTA_PWD "12345678"

// mDNS
#define WEB_NAME "trioprint"

// WEB SERVER
#define PORT 80
#define WEB_SOCKET_PORT 81

// File management
static constexpr const char * ROOT_FILE = "compiled.html.gz"; 
static constexpr uint64_t SD_SECTOR_SIZE = 512;

// You can change it, if your sd card supports faster speeds. 
#define SD_SPI_SPEED SD_SCK_MHZ(16) 
#define FILE_CHUNK_SIZE 1024

#define USE_UTF8_LONG_NAMES 1
#define USE_LONG_FILE_NAMES 1

//Comment them if you dont like debugging :(
#define DEBUG_SERIAL
#define DEBUG_SOCKET 
#define DEBUG_SERIAL_INSTANCE Serial

// Device (printer)
#define DEVSERIAL 0
//#define DEV_CUSTOM_SERIAL 115200, SERIAL_8N1, 16, 17 //ONLY ESP32 SUPPORT. Use if the DEVSERIAL 0, 1, 2, 3 do not work.

#define OUTPUT_QUEUE_LENGHT 15

// in gcode there are some comments and I have to remove them before sending the commands to printer.
#define COMMENTCHAR ";"

// Array of G-code commands
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

static constexpr const char* EMS_COMMANDS[] = {
    "M112",               
};