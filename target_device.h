#include "FsLib/FsFile.h"
#pragma once
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

    class GCode{
        String filename;
        FsFile file

        int steps = 0;
        int currentStep = 0;

        bool shutdownProtection;
        bool recovery = false;

        GCode(String fn, bool sP) : filename(fn), shutdownProtection(sP) {
            file = SDW::openFile(filename);

        }



    }; 

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