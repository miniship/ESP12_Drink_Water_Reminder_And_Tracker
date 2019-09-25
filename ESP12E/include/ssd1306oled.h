#ifndef ssd1306oled_h
#define ssd1306oled_h

#include <stdint.h>

namespace oled {
    void init();
    void displayTextCenter(const char* text, bool shouldDisplayWifiMode);
    void displayTextCenter(const char* text, uint8_t textSize, bool shouldDisplayWifiMode);
    void displayText(const char* text, uint16_t x, uint16_t y, uint8_t textSize, bool shouldDisplayWifiMode);
    void blinkScreen();
}

#endif