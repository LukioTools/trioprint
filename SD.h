#include "c_types.h"
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

      String files = "[";
      constexpr size_t filename_max = 32;
      char name[filename_max];

      while (FsFile file = folder.openNextFile()) {
        file.getName(name, filename_max);
        (files += "\"") += name;
        if (file.isDirectory()) files += "/\",";
        else files += + '_' + String(file.size()) + "\",";
      }

      files[files.length()-1]=']';
      return files;
    }

    uint8_t* readFile(const char* filename, size_t& fileSize) {
      FsFile file = SD.open(filename, O_READ);
      if (!file){
        return nullptr;
      }

      if (file.isDir()){
        return nullptr;
      }

      fileSize = file.fileSize();
      auto fileData = new uint8_t[fileSize];
      if (file.read(fileData, fileSize) != fileSize) {
        Serial.println("Failed to read file into RAM");
        delete[] fileData;
        file.close();
        return nullptr;
      }

      file.close();
      return fileData;

    }

    bool WriteFile(const char* name, const uint8_t* fileData, size_t size){
      FsFile file = SD.open(name, O_WRITE | O_CREAT);
      if(file.write(fileData, size) == size){
        file.close();
        return true;
      }
      file.close();
      return false;
    }

} // namespace WRAPPPER_NAMESPACE