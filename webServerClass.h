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
}