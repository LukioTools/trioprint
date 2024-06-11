#pragma once

#include <SdFat.h>

#define WRAPPPER_NAMESPACE SDW



namespace WRAPPPER_NAMESPACE
{
    namespace{
      SdFs sd;
      csd_t csd;
    }
    
    template<bool dont_repeat = false>
    inline static bool init(uint8 chip_select_pin = PIN_SPI_SS){
    label:
        if (sd.begin(chip_select_pin, SPI_FULL_SPEED)) {
          sd.card()->readCSD(&csd);
          return true;
        }
        else if(dont_repeat){
            sd.initErrorHalt();
            Serial.println("Failed to initialize SD, NOT retrying....");
            return false;
        }
        delay(15);
        Serial.println("Failed to initialize SD, retrying....");
        goto label;
    }

    float cardSize(){ // full size in megabytes
      return 0.000512*csd.capacity(); // returns in MB (1,000,000 bytes)
    }

    float freeSize(){   //megabytes hopefully
      long freeKB = sd.freeClusterCount();
      freeKB *= sd.sectorsPerCluster()/2; 
      return freeKB / 1000;
    }

    String listDir(String path){
      FsFile folder = sd.open(path);
      if(!folder){
        return "Failed to open directory";
      }
      if (!folder.isDirectory()){
          return "Not a directory";
      }

      FsFile file = folder.openNextFile();

      String files = "[";

      while (file) {
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
      files += "]"
      return files;
    }
} // namespace WRAPPPER_NAMESPACE