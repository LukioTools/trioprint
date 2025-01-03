#pragma once
#if defined(ESP8266)
    #include <ESP_EEPROM.h>
#else
    #include <EEPROM.h>
#endif

#include "Debug.h"

#define EEPROM_SIZE 512 // Define the EEPROM size for ESP32

struct Memory{

    public:

    Memory(){}

    /*
        [0]:
            0: debug: via serial
            1: debug: via web interface
            2: debug: sent commands to printer
            3: debug: in queue
            4: debug: show failed commands
            5: debug: show print progress
        [1]:
    */
    unsigned char boolean_config[1];

    // puller frequency 0-255
    unsigned char puller_frequency[1];

    unsigned char progress_report[1];
    [3]:
        how often progress shall be reported (when modulus = 0)
    */

    void begin(){
        EEPROM.begin(sizeof(Memory)); // ESP8266: Must initialize with EEPROM size
        loadFromEEPROM();
    }

    void set_debug_via_serial(bool status) {
        set_bit(0, status);
    }

    bool get_debug_via_serial(){
        return get_bit(0);
    }

    void set_debug_via_web(bool status) {
        set_bit(1, status);
    }

    bool get_debug_via_web(){
        return get_bit(1);
    }

    void set_debug_sent_commands(bool status) {
        set_bit(2, status);
    }

    bool get_debug_sent_commands(){
        return get_bit(2);
    }

    void set_debug_in_queue(bool status) {
        set_bit(3, status);
    }

    bool get_debug_in_queue(){
        return get_bit(3);
    }

    void set_debug_show_failed_command(bool status) {
        set_bit(4, status);
    }

    bool get_debug_show_failed_command(){
        return get_bit(4);
    }

    void set_debug_show_progress(bool status) {
        set_bit(5, status);
    }

    bool get_debug_show_progress(){
        return get_bit(5);
    }



    void set_debug_show_prgress_rate(char status){
        set
    }

    void set_bit(int pos, bool status){
       if (status) {
            boolean_config[0] |= (1 << pos); 
        } else {
            boolean_config[0] &= ~(1 << pos);
        }
        saveToEEPROM();
    }

    void set_char(int pos, char c){
        
    }

    bool get_bit(int bit_position) {
        return (boolean_config[0] & (1 << bit_position)) != 0;
    }

    bool saveToEEPROM(){
        Memory tempMemory = *this;
        EEPROM.put(0, tempMemory);
        bool ok = EEPROM.commit();
        return ok;
    }

    void loadFromEEPROM(){
        Memory tempMemory;
        EEPROM.get(0, tempMemory);
        *this = tempMemory;
    }
};

Memory memory;
