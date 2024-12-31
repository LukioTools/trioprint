#include "Debug.h"
#pragma once


#include "config.h"
#include "SD_Manager.h"
#include "HardwareSerial.h"
#include "webSocketClass.h"
#include "H-Print.h"
#include "dynamic_config.h"

namespace TD {

    class DevSerial{
        public: 
        
        size_t inQueue = 0;
        bool canWrite = true;

        bool abortPrint;

        size_t failed_commands;

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
            #if defined(DEV_CUSTOM_SERIAL) && defined(ESP32)
                serial->begin(115200, SERIAL_8N1, 16, 17);
            #else
                serial->begin(115200);
            #endif
        }

        void changeBaundRate(int newRate){
            serial->println("M575 B" + String(newRate));
            delay(1000);
            serial->end();
            delay(1000);
            #if defined(DEV_CUSTOM_SERIAL) && defined(ESP32)
                serial->begin(newRate, SERIAL_8N1, 16, 17);
            #else
                serial->begin(newRate);
            #endif
        }

        size_t println(const String& data){
            inQueue++; 
            return serial->println(data);
        }

        size_t print(const String& data){
            //if(isCommand(data))
                inQueue++; 
                
            return serial->print(data);
        }

        size_t write(uint8_t c){            
            return serial->write(c);
        }

        size_t write(uint8_t* c, size_t size){
            inQueue++; // we assume everything is command :)
            return serial->write(c, size);
        }

        void Handle(){

            String data;
            if(!serial->available()) return;
            
            while( serial->available() && !(data = serial->readStringUntil('\n')).isEmpty()){

<<<<<<< HEAD
=======
                if(memory.get_debug_sent_commands())
                    Debugger::print("printer: " + data);

>>>>>>> da3619a (quick commit -_-)
                if(data.startsWith("ok")){
                    if(inQueue) 
                        inQueue--;

                    canWrite = true;
                    if(memory.get_debug_in_queue())
                        Debugger::print("in queue: " + String(inQueue));
                
                } else if(data.startsWith("echo: cold extrusion prevented")){
                    abortPrint = true;
                } else if(data.startsWith("echo:Unknown command:")){
                    failed_commands++;
                    if(memory.get_debug_show_failed_command())
                    Debugger::print("command failed: " + data);
                }

                if(data.startsWith("echo:busy: processing")){
                    canWrite = false;
                }
            }
        }

    };

    DevSerial devSerial;

    bool isPrintting = false;
    
    class GCode{
        String filename;
        bool shutdownProtection = true;
        uint64_t currentStep = 0;

        FsFile file;

        String fileContent;
        

        uint64_t steps = 0;


        FsFile recoveryFile;



        public:
        bool printRunning = false;

        GCode(){}

        GCode(String fn, bool sP, uint64_t c_step) : filename(fn), shutdownProtection(sP), currentStep(c_step) {
            file = SDW::openFile(filename);

            char c;
            while (file.read(&c, 1) == 1) {
                if (c == '\n') {
                steps++;
                }
            }

            // Let's move to beginning after reading the file line count.
            file.seek(0);

            Debugger::print("Line amount in gcode file: " + String(steps));

            //recovery isn't supported right now. I have to think more how will it work.
            //if(shutdownProtection){
            //    recoveryFile = SDW::openFile("recoveryFile");
            //    }
            
        }

        bool isCommand(const String& data){
            return (data.startsWith("M") || data.startsWith("G"));
        }

        void Handle(){
            if(!devSerial.canWrite) return;

            if(devSerial.inQueue > OUTPUT_QUEUE_LENGHT) return;

            if(!printRunning) return;

            while(devSerial.inQueue < OUTPUT_QUEUE_LENGHT){
                
                String line;
                bool readState = readGCodeFromSDCard(line);

                if(!readState){
                    printRunning = false;
                    Debugger::print("print finished, " + String(devSerial.failed_commands));
                    return;
                }

                if(line.startsWith(COMMENTCHAR)) continue;

                int delimiterIndex = line.indexOf(COMMENTCHAR);
                if(delimiterIndex != -1){
                    line = line.substring(0, delimiterIndex);
                }

                if(line.isEmpty() || !isCommand(line)) continue;

                devSerial.println(line);

                currentStep++;
                Debugger::print("commands completed: " + String(currentStep) + "/" + String(steps));



                if(currentStep >= steps){
                        printRunning = false;
                        Debugger::print("print finished, " + String(devSerial.failed_commands));
                        return;
                    };
            }
        }

        private:
        bool readGCodeFromSDCard(String& output){
            while (true){
                int d = file.read();
                if(d == -1){
                    return false;
                }

                if((char)d == '\n'){
                    return true;
                }
                output += (char)d;
            }
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