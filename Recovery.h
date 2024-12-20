#include "c_types.h"
#pragma once

namespace RV {
    int CheckForRecovering(){
        String recoveryFileData = "";
        uint64_t recoveredLine;

        recoveryFile = SDW::openFile("recoveryFile");

        char g;
        while (recoveryFile.read(&c, 1) == 1) {
            recoveryFileData += String(c);
        }

        if(recoveryFileData == ""){
            return -1;
        }

        recoveredLine = recoveryFileData.toInt();
        return recoveredLine;

    }
}