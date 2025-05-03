
/*

 Copyright (c) <2025> <Vili Kervinen>
 
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

#pragma once

#define PIN_SPI_SS 5
#define SD_SPI_TRIES 10

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
#define FIRMWARE_VERSION_SIZE 9

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
