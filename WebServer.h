#include <sys/_stdint.h>
#include "HardwareSerial.h"
#include "FsLib/FsFile.h"
#pragma once

#include "WIFI.h"
#include "log.h"
#include "SD.h"
#include "target_device.h"

#include <ESP8266WebServer.h>
#include <cstdio>

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
  void HandleDownloadFile(); // Gives file to client
  void HandlePrintStatus();  // gives targer device status
  void HandleUpload();

  void HandleNotFound();

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
    
    server.begin();
  }

  void handle(){
    server.handleClient();
  }

  void HandleRoot(){
      size_t fileSize = 0;
      uint8_t* fileData = SDW::readFile("compiled.html.gz", fileSize);

      if(fileData){
          Serial.println("File loaded: "+ String(fileSize));
          server.sendHeader("Content-Encoding", "gzip");
          server.send(200, "text/html", fileData, fileSize);
      }else{
          Serial.println("failed to load");
          server.send(500, "text/html", "Failed to load file from SD");
      }
    delete[] fileData;
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
    Serial.println(server.arg("plain"));
    server.send(200, "application/json", SDW::listDir(server.arg("plain")).c_str());
  }
  void HandleRemoveFile(){}
  void Handlemkdir(){}
  void HandleDownloadFile(){
    server.sendHeader("Content-Encoding", "application/octet-stream");
    Serial.println("downloading file");
    size_t filesize = 0;
    const String& name = server.arg("fileName");
    uint8_t* filedata = SDW::readFile(name.c_str(), filesize);
    server.send(200, "application/octet-stream", (char *)filedata, filesize);
    Serial.println("Download completed");
  }

  FsFile upload_file;
  void HandleUpload(){
    const String& filepath = server.arg("path");
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      upload_file = SDW::SD.open(filepath+upload.filename);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      Serial.printf(
        "Wrote %i/%i to '%s'", 
        upload_file.write(upload.buf, upload.currentSize), 
        upload.currentSize, 
        (filepath+upload.filename).c_str()
      );
    } else if (upload.status == UPLOAD_FILE_END) {
      upload_file.close();
    }    
  };
  void HandlePrintStatus(){}
}