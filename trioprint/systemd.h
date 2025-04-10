#pragma once

#include "target_device.h"


namespace systemd {
    class Infopuller{

        u_char freq = 5; // in seconds
        String commands = "";

        size_t lastUpdate = 0; //millis when last time updated;

        public:

        void begin(u_char f, String c) { //commands sepreated by "|"
            lastUpdate = millis();
            freq = f;
            c.replace("_", "\n");
            commands = c;  
        }

        void handle(){
            if(millis() - lastUpdate > ((int)freq)*1000 && TD::devSerial.canWrite){
                TD::devSerial.println(commands);
                lastUpdate = millis();
            }
        }
    };

    Infopuller infopuller;

}