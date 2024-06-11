#pragma once

#include "WIFI.h"
#include "log.h"
#include "SD.h"
#include "target_device.h"

#include <ESP8266WebServer.h>

namespace WebServerW {

  void HandleRoot();
  
  void HandleServerStatus();// gives server status  
  void HandlePrint();       // Gets file name to print
  void HandlePause();       // Pauses print
  void HandlesStop();       // ends the print
  void HandleEmergencyStop();// handles emergy stop of 3d printter

  void HandleListFolder();  // Gets folder
  void HandleRemoveFile();  // Gets file or folder name and removes it from the sd card
  void Handlemkdir();       // Adds folder by name
  void HandlePrintStatus();  // gives targer device status

  void HandleNotFound();

  ESP8266WebServer server(80); 

  void begin(){
    cout << "starting web server" <<  "\n";
    
    server.on("/", HTTP_GET ,HandleRoot);
    server.on("/server/status/", HTTP_GET ,HandleServerStatus);
    
    server.on("/device/print/", HTTP_GET ,HandlePrint);
    server.on("/device/pause/", HTTP_GET ,HandlePause);
    server.on("/device/stop/", HTTP_GET ,HandlesStop);
    server.on("/device/ems/", HTTP_GET ,HandleEmergencyStop);
    server.on("/device/status/", HTTP_GET ,HandlePrintStatus);
    server.on("/device/ls/", HTTP_GET ,HandlePrintStatus);
    server.on("/device/console", HTTP_GET ,HandlePrintStatus);
    
    server.on("/fm/ls/", HTTP_GET ,HandleListFolder);
    server.on("/fm/remove/", HTTP_GET ,HandleRemoveFile);
    server.on("/fm/mkdir/", HTTP_GET ,Handlemkdir);

    server.onNotFound(HandleNotFound);
    
    server.begin();
    cout << "web server started" << "\n";

  }

  void handle(){
    server.handleClient();
  }

  void HandleRoot(){
      server.send(200, "text/plain", GUI);   // Send HTTP status 200 (Ok) and send some text to the browser/client
  }

  void HandleNotFound(){
    server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
  }

  void HandleServerStatus(){
    String responce = "{";
    responce += "\"cardSize\":" + String(SDW::cardSize()) + ",";
    responce += "\"freeSpace\":" + String(SDW::freeSize()) + ",";
    responce += "\"printting_status\":" + String(Target_device::isPrintting) + "";
    responce += "}";

    server.send(200, "text/plain", responce);
  }
  void HandlePrint(){}
  void HandlePause(){}
  void HandlesStop(){}
  void HandleEmergencyStop(){}

  void HandleListFolder(){
    
  }
  void HandleRemoveFile(){}
  void Handlemkdir(){}
  void HandlePrintStatus(){}
}