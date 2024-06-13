#pragma once
#include <ESP8266WebServer.h>
#ifndef PORT
#define PORT 80
#endif
namespace WebServerW {
  ESP8266WebServer server(PORT); 
}