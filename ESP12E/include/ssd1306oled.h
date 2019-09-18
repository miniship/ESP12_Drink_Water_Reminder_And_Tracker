#ifndef ssd1306oled_h
#define ssd1306oled_h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace oled {
    void init();
    void displayText(const char* text);
    void displayText(const char* text, uint8_t textSize);
    void displayText(const char* text, int16_t x, int16_t y);
    void displayText(const char* text, int16_t x, int16_t y, uint8_t textSize);
    void displayTextCenter(const char* text);
    void displayTextCenter(const char* text, uint8_t textSize);
    void blinkScreen();
}

#endif