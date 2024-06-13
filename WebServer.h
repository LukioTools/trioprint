#pragma once
#include <ESP8266WebServer.h>
#ifndef PORT
#define PORT 80
#endif
namespace WebServerW {
  /*
  void HandleRoot();
  
  
  void HandleServerStatus();// gives server status  
  void HandlePrint();       // Gets file name to print
  void HandlePause();       // Pauses print
  void HandlesStop();       // ends the print
  void HandleEmergencyStop();// handles emergy stop of 3d printter

  void HandleListFolder();  // Gets folder
  void HandleRemoveFile();  // Gets file or folder name and removes it from the sd card
  void Handlemkdir();       // Adds folder by name
  void HandleDownloadFile(); // Gives file to client
  void HandlePrintStatus();  // gives targer device status
  void HandleUpload();

  void HandleNotFound();

  void CacheIndexHtml();

  ESP8266WebServer server(80); 

  void begin(){
    
    server.on("/", HTTP_GET ,HandleRoot);
    server.on("/server/status/", HTTP_GET ,HandleRoot);
    
    server.on("/device/print/", HTTP_GET ,HandlePrint);
    server.on("/device/pause/", HTTP_GET ,HandlePause);
    server.on("/device/stop/", HTTP_GET ,HandlesStop);
    server.on("/device/ems/", HTTP_GET ,HandleEmergencyStop);
    server.on("/device/status/", HTTP_GET ,HandlePrintStatus);
    server.on("/device/ls/", HTTP_GET ,HandlePrintStatus);
    server.on("/device/console", HTTP_GET ,HandlePrintStatus);
    
    server.on("/fm/ls/", HTTP_POST ,HandleListFolder);
    server.on("/fm/remove/", HTTP_GET ,HandleRemoveFile);
    server.on("/fm/mkdir/", HTTP_GET ,Handlemkdir);
    server.on("/fm/downloadFile/", HTTP_GET, HandleDownloadFile);
    server.on("/fm/upload/", HTTP_POST, HandleUpload);

    server.onNotFound(HandleNotFound);

    // chache shit
    CacheIndexHtml();
    
    // start the server
    server.begin();
  }

  void handle(){
    server.handleClient();
  }

  uint8_t* IndexHtmlData;
  size_t IndexHtmlfileSize = 0;
  void CacheIndexHtml(){
    IndexHtmlData = SDW::readFile("compiled.html.gz", IndexHtmlfileSize);
  }

  void HandleRoot(){
      if(IndexHtmlData){
          server.sendHeader("Content-Encoding", "gzip");
          server.send(200, "text/html", IndexHtmlData, IndexHtmlfileSize);
      }else{
          server.send(500, "text/html", "Failed to load file from SD");
      }
  }

  void HandleNotFound(){
    Serial.println("page not found");
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
    Serial.println(server.arg("plain"));
    server.send(200, "application/json", SDW::listDir(server.arg("plain")).c_str());
  }
  void HandleRemoveFile(){}
  void Handlemkdir(){}
  void HandleDownloadFile(){
    server.sendHeader("Content-Encoding", "application/octet-stream");
    Serial.println("downloading file");
    size_t filesize = 0;
    const String& name = server.arg("filename");
    uint8_t* filedata = SDW::readFile(name.c_str(), filesize);
    server.sendHeader("Content-Disposition", "inline; filename=\""+name+'"');
    server.send(200, "application/octet-stream", (char *)filedata, filesize);
    Serial.println("Download completed");
  }

  
  void HandlePrintStatus(){}
  */
}