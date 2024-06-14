
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
        server.on("/device/console", HTTP_GET , Handlers::NotFound);
        
        server.on("/fm/ls/", HTTP_POST , Handlers::ListFolder);
        server.on("/fm/remove/", HTTP_GET ,Handlers::Remove);
        server.on("/fm/mkdir/", HTTP_GET ,Handlers::Mkdir);
        server.on("/fm/downloadFile/", HTTP_GET, Handlers::DownloadFile);
        server.on("/fm/uploadFile/", HTTP_POST, Handlers::UploadFile);

        server.onNotFound(Handlers::NotFound);

        //PRELOADS
        Handlers::RootPreload();

        server.begin();
    }
}
