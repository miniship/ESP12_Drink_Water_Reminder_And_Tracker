#include "ssd1306oled.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define BASE_WIDTH 6 // base width for text size = 1
#define BASE_HEIGHT 8 // base height for text size = 1

const char* SERVER_ADDRESS = "192.168.4.1";
const char* AP_MODE = "AP";
const char* STA_MODE_WIFI_ON = "WIFI_ON";
const char* STA_MODE_WIFI_OFF = "WIFI_OFF";
const uint8_t DEFAULT_TEXT_SIZE = 1;
bool isInvertDisplay = false;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void displayWifiMode();

void oled::init() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextSize(DEFAULT_TEXT_SIZE);
    display.setTextColor(WHITE);
}

void oled::displayTextCenter(const char* text, bool shouldDisplayWifiMode) {
    displayTextCenter(text, DEFAULT_TEXT_SIZE, shouldDisplayWifiMode);
}

void oled::displayTextCenter(const char* text, uint8_t textSize, bool shouldDisplayWifiMode) {
    int16_t pxLength = strlen(text) * textSize * BASE_WIDTH;
    int16_t pxHeight = (pxLength / SCREEN_WIDTH + 1) * textSize * BASE_HEIGHT;
    int16_t x = pxLength >= SCREEN_WIDTH ? 0 : (SCREEN_WIDTH - pxLength) / 2;
    int16_t y = pxHeight >= SCREEN_HEIGHT ? 0 : (SCREEN_HEIGHT - pxHeight) / 2;
    displayText(text, x, y, textSize, shouldDisplayWifiMode);
}

void oled::displayText(const char* text, uint16_t x, uint16_t y, uint8_t textSize, bool shouldDisplayWifiMode) {
    display.clearDisplay();
    display.invertDisplay(false);
    isInvertDisplay = false;

    if (shouldDisplayWifiMode) {
        displayWifiMode();
    }   

    display.setCursor(x, y);
    display.setTextSize(textSize);
    display.print(text);
    display.display();
}

void displayWifiMode() {
    display.setTextSize(DEFAULT_TEXT_SIZE);
    const char* mode = STA_MODE_WIFI_OFF;

    if (WiFi.getMode() == WIFI_AP) {
        mode = AP_MODE;        
        display.setCursor(SCREEN_WIDTH - strlen(SERVER_ADDRESS) * DEFAULT_TEXT_SIZE * BASE_WIDTH, SCREEN_HEIGHT - DEFAULT_TEXT_SIZE * BASE_HEIGHT);
        display.print(SERVER_ADDRESS);

    } else if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED) {
        mode = STA_MODE_WIFI_ON;
        String ssid = WiFi.SSID();
        display.setCursor(SCREEN_WIDTH - ssid.length() * DEFAULT_TEXT_SIZE * BASE_WIDTH, SCREEN_HEIGHT - DEFAULT_TEXT_SIZE * BASE_HEIGHT);
        display.print(ssid);
    }

    display.setCursor(SCREEN_WIDTH - strlen(mode) * DEFAULT_TEXT_SIZE * BASE_WIDTH, 0);
    display.print(mode);
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

