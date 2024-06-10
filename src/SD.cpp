#include <SD.h>


#define WRAPPPER_NAMESPACE SDW

namespace WRAPPPER_NAMESPACE
{
    
    template<bool dont_repeat = false>
    inline static bool init(uint8 chip_select_pin = PIN_SPI_SS){
    label:
        if (SD.begin(chip_select_pin)) return true;
        else if(dont_repeat){
            Serial.println("Failed to initialize SD, NOT retrying....");
            return false;
        }
        delay(15);
        Serial.println("Failed to initialize SD, retrying....");
        goto label;
    }

    //inline static bool exists(const char* filename){
    //    return  SD.exists(filename)
    //}
    //inline static bool exists(const String& filename){
    //    return  SD.exists(filename);
    //}
    //inline static bool mkdir(const char* path){
    //    return SD.mkdir(path);
    //}
    //inline static bool mkdir(const String&  path){
    //    return SD.mkdir(path);
    //}
    //
    //inline static bool remove(const char* path){
    //    return SD.remove(path);
    //}
    //inline static bool remove(const String& path){
    //    return SD.remove(path);
    //}
    //
    //inline static bool rmdir(const char* path){
    //    return SD.rmdir(path);
    //}
    //inline static bool rmdir(const String& path){
    //    return SD.rmdir(path);
    //}
    //
    //enum FILE_MODE : uint8_t{
    //    READ = FILE_READ;
    //    WRITE = FILE_WRITE;
    //}
    //
    //inline static File open(const char* filepath, FILE_MODE mode = (FILE_MODE)1U){
    //    return SD.open(filepath, mode);
    //}
    //inline static File open(const String& filepath, FILE_MODE mode = (FILE_MODE)1U){
    //    return SD.open(filepath, mode);
    //}

} // namespace WRAPPPER_NAMESPACE
