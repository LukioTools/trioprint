#pragma once
#include "config.h"
#include "webSocketClass.h"
#include "dynamic_config.h"

namespace Debugger {
    
    HardwareSerial* debug_serial = & DEBUG_SERIAL_INSTANCE;

    void print(const String& data){
        if(memory.get_debug_via_serial())
            debug_serial->println(data);

        if(memory.get_debug_via_web()){
            String& temp = const_cast<String&>(data);
            WebSocketW::brodcastAllTXT(temp);
        }
    }

    void print(const int& data){
        if(memory.get_debug_via_serial())
            debug_serial->println(String(data));
        
        if(memory.get_debug_via_web()) {
            String temp = String(const_cast<int&>(data));
            WebSocketW::brodcastAllTXT(temp);
        }
    }
    //TODO: keep adding these

    void serialPrint(const String& data){
        if(memory.get_debug_via_serial())
            debug_serial->println(data);
    }
}