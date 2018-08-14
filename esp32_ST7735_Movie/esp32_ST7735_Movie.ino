#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include <FS.h>
#include <SD.h>

#define TFT_CS     5
#define TFT_RST    16
#define TFT_DC     17
#define SD_CS      4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

uint16_t picture[120][160];

void setup(void) {
  SD.begin(SD_CS, SPI, 20000000);
  Serial.begin(115200);
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  Serial.println();
  Serial.println("Enter \"list\", \"play\", \"pause\", or \"stop\" for control");
  Serial.println();
}

void loop() {
  static File file;
  static bool play = false;
  static bool pause = false;
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim();
    if (s == "list") {
      if (play || pause) {
        play = false;
        pause = false;
        tft.fillScreen(ST7735_BLACK);
        file.close();
      }
      Serial.println("list");
      File root = SD.open("/");
      file = root.openNextFile();
      while (file) {
        if (file.isDirectory()) {
          Serial.print("DIR: ");
          Serial.println(file.name());
        }
        else {
          Serial.print("FILE: ");
          Serial.print(file.name());
          Serial.print("  SIZE: ");
          Serial.println(file.size());
        }
        file = root.openNextFile();
      }
      Serial.println();
    }
    if (s == "play") {
      if (!pause) {
        Serial.println("play");
        Serial.println("Enter filename");
        while (!Serial.available());
        String filename = Serial.readStringUntil('\n');
        filename.trim();
        file = SD.open(filename);
        tft.fillScreen(ST7735_BLACK);
        Serial.println(filename);
      }
      else Serial.println("play");
      Serial.println();
      play = true;
      pause = false;
    }
    if (s == "pause") {
      if (play) {
        play = false;
        pause = true;
        Serial.println("pause");
      }
      else Serial.println("invalid");
      Serial.println();
    }
    if (s == "stop") {
      play = false;
      pause = false;
      file.close();
      tft.fillScreen(ST7735_BLACK);
      Serial.println("stop");
      Serial.println();
    }
  }
  if (play) {
    file.read((uint8_t*)picture, 120*160*2);
    tft.fillImage(picture, 0, 4, 160, 120);
    if (!file.available()) {
      play = false;
      pause = false;
      file.close();
    }
  }
}

