#pragma once

#include "webServerClass.h"

#include "H-DownloadFile.h"
#include "H-ListFolder.h"
#include "H-Mkdir.h"
#include "H-NotFound.h"
#include "H-Pause.h"
#include "H-PrintStatus.h"
#include "H-Print.h"
#include "H-Remove.h"
#include "H-Root.h"
#include "H-ServerStatus.h"
#include "H-Stop.h"
#include "H-UploadFile.h"
#include "H-DevConsole.h"
#include "H-Recovery.h"
#include "H-sendCommand.h"
#include "H-set_config.h"
#include "H-getDynamicConfig.h"

namespace WebServerW {
    inline static void handle(){server.handleClient();}
    
    void begin(){
        server.on("/", HTTP_GET ,Handlers::Root);
        
        server.on("/server/status/", HTTP_GET ,Handlers::ServerStatus);

        server.on("/device/print/", HTTP_GET , Handlers::Print);
        server.on("/device/pause/", HTTP_GET ,Handlers::Pause);
        server.on("/device/stop/", HTTP_GET ,Handlers::Stop);
        server.on("/device/ems/", HTTP_GET ,Handlers::NotFound);
        server.on("/device/status/", HTTP_GET ,Handlers::PrintStatus);
        server.on("/device/ls/", HTTP_GET ,Handlers::NotFound);
        server.on("/device/console/", HTTP_GET , Handlers::GetConsole);
        server.on("/device/recoveryStatus/", HTTP_GET, Handlers::RecoveryStatus);

        server.on("/device/sendCommand", HTTP_GET, Handlers::sendCommand);
        
        server.on("/fm/ls/", HTTP_GET , Handlers::ListFolder);
        server.on("/fm/remove/", HTTP_GET ,Handlers::Remove);
        server.on("/fm/mkdir/", HTTP_GET ,Handlers::Mkdir);
        server.on("/fm/downloadFile/", HTTP_GET, Handlers::DownloadFile);

        server.on("/config/setDynamicConfig/", HTTP_GET, Handlers::setDynamicConfig);
        server.on("/config/getDynamicConfig/", HTTP_GET, Handlers::getDynamicConfig);

        server.on("/fm/uploadFile/", HTTP_POST, [](){ server.send(200); },                          // Send status 200 (OK) to tell the client we are ready to receive
        Handlers::UploadFile);

        server.onNotFound(Handlers::NotFound);

        //PRELOADS
        Handlers::RootPreload();

        server.begin();

        Debugger::print("Server started");

    }
}
