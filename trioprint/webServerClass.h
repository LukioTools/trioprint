#pragma once

#if defined(ESP8266)
    #include <ESP8266WebServer.h>
#elif defined(ESP32)
    #include <WebServer.h>
#endif

#ifndef WEB_SERVER_PORT
#define WEB_SERVER_PORT 80
#endif

namespace WebServerW {
    #if defined(ESP8266)
      ESP8266WebServer server(WEB_SERVER_PORT); 
    #elif defined(ESP32)
      WebServer server(WEB_SERVER_PORT); 
    #endif

    // for some reason esp32 doesnt have the server.send with size. Instead you have to use the send_P. I do not want to do this eve
    #if defined(ESP8266)
        #define SERVER_SEND_WITH_LENGTH(code, type, data, size) (server.send(code, type, data, size));
    #elif defined(ESP32)
        #define SERVER_SEND_WITH_LENGTH(code, type, data, size) (server.send_P(code, type, data, size));
    #endif

}