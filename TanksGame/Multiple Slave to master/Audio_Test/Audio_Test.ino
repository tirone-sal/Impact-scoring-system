// will play many audio file formats, mp3,aac,flac etc.
// See github page : https://github.com/schreibfaul1/ESP32-audioI2S

#include "Arduino.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"

// Digital I/O used
#define SD_CS          5
#define SPI_MOSI      23    // SD Card
#define SPI_MISO      19
#define SPI_SCK       18

#define I2S_DOUT      25
#define I2S_BCLK      27    // I2S
#define I2S_LRC       26

Audio audio;

void setup() {
    pinMode(SD_CS, OUTPUT);      
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    Serial.begin(115200);

    if(!SD.begin(SD_CS))
    {
      Serial.println("Error talking to SD card!");
      while(true);  // end program
    }
    
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(15); // 0...21
    audio.connecttoFS(SD,"/Sample3.mp3");
}

void loop()
{
    audio.loop();    
}

