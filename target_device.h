#pragma once

#include "c_types.h"
#include "WString.h"
#include "FsLib/FsFile.h"

#include "HardwareSerial.h"
#include "config.h"
#include "WebSocket.h"
#include "SD.h"

namespace TD {

    class DevSerial{
        public: 
            DevSerial(){}
    
            #if DEVSERIAL == 0
                HardwareSerial *serial = &Serial; 
            #elif DEVSERIAL == 1
                HardwareSerial *serial = &Serial1;
            #elif DEVSERIAL == 2
                HardwareSerial *serial = &Serial2;
            #elif DEVSERIAL == 3
                HardwareSerial *serial = &Serial3;
            #endif

            void SerialBegin(){
                serial->begin(115200);
            }

            void Handle(){
                if(serial->available()){
                    const String serialBuffer = serial->readStringUntil('\n');
                    WebSocketW::brodcastAllTXT(serialBuffer);

                }
            }

    };

    DevSerial devSerial;

    bool isPrintting = false;

    bool firmware(){ // M115
        return "FIRMWARE_NAME:Marlin V1.0.6 (Sep 19 2023 09:14:17) SOURCE_CODE_URL:github.com/MarlinFirmware/Marlin PROTOCOL_VERSION:1.0 MACHINE_TYPE:Ender-3 V3 SE EXTRUDER_COUNT:1 UUID:cede2a2f-41a2-4748-9b12-c55c62f367ff Cap:SERIAL_XON_XOFF:0 Cap:BINARY_FILE_TRANSFER:0 Cap:EEPROM:1 Cap:VOLUMETRIC:1 Cap:AUTOREPORT_POS:0 Cap:AUTOREPORT_TEMP:1 Cap:PROGRESS:0 Cap:PRINT_JOB:1 Cap:AUTOLEVEL:1 Cap:RUNOUT:0 Cap:Z_PROBE:1 Cap:LEVELING_DATA:1 Cap:BUILD_PERCENT:0 Cap:SOFTWARE_POWER:0 Cap:TOGGLE_LIGHTS:0 Cap:CASE_LIGHT_BRIGHTNESS:0 Cap:EMERGENCY_PARSER:1 Cap:HOST_ACTION_COMMANDS:0 Cap:PROMPT_SUPPORT:0 Cap:SDCARD:1 Cap:REPEAT:0 Cap:SD_WRITE:1 Cap:AUTOREPORT_SD_STATUS:0 Cap:LONG_FILENAME:1 Cap:THERMAL_PROTECTION:1 Cap:MOTION_MODES:0 Cap:ARCS:1 Cap:BABYSTEPPING:1 Cap:CHAMBER_TEMPERATURE:0 Cap:COOLER_TEMPERATURE:0 Cap:MEATPACK:0";
    }

    bool files(){   //M20
        return "Begin file list\nLOL~1.GCO 0\nEnd file list";
    }

    bool longName(String path){ // M33 LOL~1.GCO
        return "/lol.gcode";
    }

    bool temps(){ //M105
        return "ok T:31.67 /0.00 B:31.79 /0.00 @:0 B@:0 FAN0@:0";
    }

    bool position(){
        return "X:-13.00 Y:-15.00 Z:0.00 E:0.00 Count X:-1040 Y:-1200 Z:0\nok";
    }

}

/*commands to include for marlin
*   M115              // firmware version; https://support.th3dstudio.com/helpcenter/marlin-gcode-m115-display-firmware-version-info/
*   M20               // lists files
    M33               // Get Long path for file
    M23 <file> + M24  // Select file and print it
    M25               //Pause print
    M31               // print time
    M73 P<100%> R<43> // Set print time
    M105              // reports temperature
    M112              // quick shutdown
    M113              // Host Keepalive
    M114              // Get Current Position
    M624              // Abort SD print

*/