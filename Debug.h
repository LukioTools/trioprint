#pragma once
#include "config.h"
#include "webSocketClass.h"

namespace Debugger {
    
    #if defined (DEBUG_SERIAL)
    HardwareSerial* debug_serial = & DEBUG_SERIAL_INSTANCE;
    #endif

    void print(const String& data){
        #if defined (DEBUG_SERIAL)
            debug_serial->println(data);
        #endif

        #if defined (DEBUG_SOCKET)
        String& temp = const_cast<String&>(data);
        WebSocketW::brodcastAllTXT(temp);
        #endif
    }

    void print(const int& data){
        #if defined (DEBUG_SERIAL)
        debug_serial->println(String(data));
        #endif

        #if defined (DEBUG_SOCKET)
        String temp = String(const_cast<int&>(data));
        WebSocketW::brodcastAllTXT(temp);
        #endif
    }
    //TODO: keep adding these

    //TODO: add printf

    void serialPrint(const String& data){
        #if defined (DEBUG_SERIAL)
        debug_serial->println(data);
        #endif
    }
}