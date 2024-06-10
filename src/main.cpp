#include <Arduino.h>
#include <SD.h>

constexpr uint8_t SD_CARD_CS = PIN_SPI_SS; 

void setup() {
    Serial.begin(9600);
    SD.begin();
}

void loop() {

}