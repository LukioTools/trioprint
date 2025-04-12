#pragma once
#include "SdFat.h"

#if defined(ESP32) //Hate to do these things but ig I have to. IDK why can't they be the same on esp8266 and esp32
    #define uint8 uint8_t
    #define uint32 uint32_t

    #define PIN_SPI_SS 17
#elif defined(ESP8266)
    // nothing here
#endif

#define WIFI_SSID_SIZE 16
#define WIFI_PWD_SIZE 16

#define OTA_PWD_SIZE 16

#define WEB_NAME_SIZE 10

//#define PORT 80
//#define WEB_SOCKET_PORT 81
//static constexpr uint64_t SD_SECTOR_SIZE = 512;
//#define FILE_CHUNK_SIZE 1024
//#define SD_SPI_SPEED SD_SCK_MHZ(16) 
//#define SD_CARD_CONNECTION_ATTEMPTS 5
//#define DEVSERIAL 0
//#define DEV_CUSTOM_SERIAL 115200, SERIAL_8N1, 16, 17 //ONLY ESP32 SUPPORT. Use if the DEVSERIAL 0, 1, 2, 3 do not work.

// how many web server request can be handled at once
#define REQUESTS 20

#define BUFFER_SIZE_ON_PRINTER 
#define COMMENTCHAR ";"

#define ROOT_FILE_SIZE 16

//static constexpr const char * ROOT_FILE = "compiled.html.gz"; 
 
#define USE_UTF8_LONG_NAMES 1
#define USE_LONG_FILE_NAMES 1

//Comment them if you dont like debugging :(
#define DEBUG_SERIAL
#define DEBUG_SOCKET 
#define DEBUG_SERIAL_INSTANCE Serial

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