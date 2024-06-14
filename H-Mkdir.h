#pragma once
#include "WebServer.h"
#include "SD.h"

using namespace WebServerW;
namespace Handlers {
  void Mkdir(){
    if(SDW::mkdir(server.arg("path"))) server.send(200, "text/plain", "Directory created succesfully!");
  }
}
