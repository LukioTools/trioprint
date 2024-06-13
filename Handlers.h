//
//void HandleRoot();//

//void HandleServerStatus();// gives server status  
//void HandlePrint();       // Gets file name to print
//void HandlePause();       // Pauses print
//void HandlesStop();       // ends the print
//void HandleEmergencyStop();// handles emergy stop of 3d printter//

//void HandleListFolder();  // Gets folder
//void HandleRemoveFile();  // Gets file or folder name and removes it from the sd card
//void Handlemkdir();       // Adds folder by name
//void HandleDownloadFile(); // Gives file to client
//void HandlePrintStatus();  // gives targer device status

//void HandleUpload();//
#include "H-NotFound.h"
#include "H-Root.h"
#include "H-Upload.h"

//void HandleNotFound();

namespace WebServerW {
    
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
}
