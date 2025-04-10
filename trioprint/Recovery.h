#pragma once

namespace RV {
    int CheckForRecovering(){
        String recoveryFileData = "";
        uint64_t recoveredLine;

        FsFile recoveryFile = SDW::openFile("recoveryFile");

        char c;
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