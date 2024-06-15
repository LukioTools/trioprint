#include "c_types.h"
#include <cstddef>
#include <cstdint>
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



    //cache this
    using freebyte_t = uint64_t;
    using freebyte_return_t = const freebyte_t&;

    constexpr freebyte_t freebyte_invalid_state = 0; 
    freebyte_t free_bytes = freebyte_invalid_state;
      //toughest of lucks
    //constexpr uint_t SdSectorSize = 512;

    freebyte_t cardSize(){ // full size in bytes
      return sdCardCapacity(&csd)*SD_SECTOR_SIZE;
    }

    void clearFreeSizeCache(){
      free_bytes = freebyte_invalid_state;
    }
    //takes fucking six seconds!
    void calculateFreeSizeCache(){
      free_bytes = SD.freeClusterCount() * SD.bytesPerCluster();
    }
    freebyte_return_t freeSize(){   //megabytes hopefully
      if(free_bytes == freebyte_invalid_state) calculateFreeSizeCache();
      return free_bytes;
    }
    freebyte_return_t refreshFreeSizeCache(){
      clearFreeSizeCache(); 
      return freeSize();
    }

    String listDir(String path){
      FsFile folder = SD.open(path);
      if(!folder) return "Failed to open directory";
      if (!folder.isDirectory()) return "Not a directory";

      String files = "[ ";
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
      if (file.read(fileData, fileSize) != (long) fileSize) {
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

    FsFile openFile(const char* name){
      return SD.open(name, O_WRITE | O_CREAT);
    }

    FsFile openFile(const String& name){
      return SD.open(name.c_str(), O_WRITE | O_CREAT);
    }

    bool mkdir(const char* path){
      return SD.mkdir(path, true);
    }

    bool mkdir(const String& path){
      return SD.mkdir(path.c_str(), true);
    }

    bool remove(const char* path){
      return SD.remove(path);
    }

    bool remove(const String& path){
      return SD.remove(path.c_str());
    }

} // namespace WRAPPPER_NAMESPACE