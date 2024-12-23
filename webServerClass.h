#pragma once

#include <ESP8266WebServer.h>

#ifndef WEB_SERVER_PORT
#define WEB_SERVER_PORT 80
#endif

namespace WebServerW {
  ESP8266WebServer server(WEB_SERVER_PORT); 
}