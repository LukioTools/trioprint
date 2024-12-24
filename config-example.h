#pragma once
//#include <device_Types.h>

#if defined(ESP32) //Hate to do these things but ig I have to. IDK why can't they be the same on esp8266 and esp32
#define uint8 uint8_t
#define uint32 uint32_t

#define PIN_SPI_SS 5

#elif defined(ESP8266)
#endif

// WIFI
#define WIFI_SSID ""
#define WIFI_PWD ""

// mDNS
#define WEB_NAME "esp8266"

// WEB SERVER
#define PORT 80
#define WEB_SOCKET_PORT 81

// File management
static constexpr const char * ROOT_FILE = "compiled.html.gz"; 
static constexpr uint64_t SD_SECTOR_SIZE = 512;

#define FILE_CHUNK_SIZE 1024

//debugging active;
#define DEBUG true


// Device (printer)
#define DEVSERIAL 0 
#define OUTPUT_QUEUE_LENGHT 10

// in gcode there are some comments and I have to remove them before sending the commands to printer.
#define COMMENTCHAR ";"


