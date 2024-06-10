#include <Arduino.h>
#include <SD.h>


constexpr uint8_t SD_CARD_CS = PIN_SPI_SS; 

void setup() {
    Serial.begin(115200);
    Serial.println("");

    while (!SD.begin(SD_CARD_CS))
    {
        Serial.println("wasn't able to open SD card");
        delay(10);
    }

    Serial.println("moi");
    
}

void loop() {

}