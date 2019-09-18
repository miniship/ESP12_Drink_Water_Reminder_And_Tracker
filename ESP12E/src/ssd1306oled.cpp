#include "ssd1306oled.h"
#include <ESP8266WiFi.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

const size_t DEFAULT_X = 10;
const size_t DEFAULT_Y = 10;
const size_t DEFAULT_TEXT_SIZE = 1;
bool isInvertDisplay = false;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void oled::init() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextSize(DEFAULT_TEXT_SIZE);
    display.setTextColor(WHITE);
}

void displayWifiIcon() {
    if (WiFi.isConnected()) {
        display.setCursor(SCREEN_WIDTH - 4 * 6, 0);
        display.setTextSize(1);
        display.print("Wifi");
    }
}

void oled::displayText(const char* text) {
    displayText(text, DEFAULT_X, DEFAULT_Y, DEFAULT_TEXT_SIZE);
}

void oled::displayText(const char* text, uint8_t textSize) {
    displayText(text, DEFAULT_X, DEFAULT_Y, textSize);
}

void oled::displayText(const char* text, int16_t x, int16_t y) {
    displayText(text, x, y, DEFAULT_TEXT_SIZE);
}

void oled::displayText(const char* text, int16_t x, int16_t y, uint8_t textSize) {
    display.clearDisplay();
    display.invertDisplay(false);
    isInvertDisplay = false;
    displayWifiIcon();

    display.setCursor(x, y);
    display.setTextSize(textSize);
    display.println(text);
    display.display();
}

void oled::displayTextCenter(const char* text) {
    displayTextCenter(text, DEFAULT_TEXT_SIZE);
}    

void oled::displayTextCenter(const char* text, uint8_t textSize) {
    int16_t pxLength = strlen(text) * textSize * 6;
    int16_t pxHeight = (pxLength / SCREEN_WIDTH + 1) * textSize * 8;
    int16_t x = pxLength >= SCREEN_WIDTH ? 0 : (SCREEN_WIDTH - pxLength) / 2;
    int16_t y = pxHeight >= SCREEN_HEIGHT ? 0 : (SCREEN_HEIGHT - pxHeight) / 2;
    displayText(text, x, y, textSize);
}

void oled::blinkScreen() {
    if (isInvertDisplay) {
        display.invertDisplay(false);
        isInvertDisplay = false;
    } else {
        display.invertDisplay(true);
        isInvertDisplay = true;
    }
}

