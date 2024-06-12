#pragma once

#include <SdFat.h>
#include "log.h"

#define WRAPPPER_NAMESPACE SDW



namespace WRAPPPER_NAMESPACE
{
    SdFs SD;
    csd_t csd;
    template<bool dont_repeat = false>
    inline static bool init(uint8 chip_select_pin = PIN_SPI_SS){
    label:
        if (SD.begin(chip_select_pin, 100000000)) {
          SD.card()->readCSD(&csd);
          return true;
        }
        else if(dont_repeat){
            SD.initErrorHalt();
            Serial.println("Failed to initialize SD, NOT retrying....");
            return false;
        }
        delay(15);
        Serial.println("Failed to initialize SD, retrying....");
        goto label;
    }

    float cardSize(){ // full size in megabytes
      return 0.000512*sdCardCapacity(&csd); // returns in MB (1,000,000 bytes)
    }

    float freeSize(){   //megabytes hopefully
      float freeKB = SD.freeClusterCount();
      freeKB *= SD.sectorsPerCluster()/2; 
      freeKB = freeKB/1000;
      return freeKB;//freeKB / 1000;
    }

    String listDir(String path){
      FsFile folder = SD.open(path);
      if(!folder){
        return "Failed to open directory";
      }
      if (!folder.isDirectory()){
          return "Not a directory";
      }

      FsFile file = folder.openNextFile();

      String files = "[";

      while (file) {
        yield();
        if (file.isDirectory()) {
          char name[15];
          file.getName(name, 15);
          files += String(name) + "/, ";
        }
        else {
          char name[15];
          file.getName(name, 15);
          files += String(name) + "_" + String(file.size()) + ",";
        }
        file = folder.openNextFile();
      }
      files.remove(files.length());
      files += "]";
      return files;
    }
} // namespace WRAPPPER_NAMESPACE