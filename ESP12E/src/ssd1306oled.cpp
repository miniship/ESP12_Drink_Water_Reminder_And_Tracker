#include "ssd1306oled.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include "DeviceController.h"
#include "DeviceSettings.h"
#include "TickerManager.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define BASE_WIDTH 6 // base width for text size = 1
#define BASE_HEIGHT 8 // base height for text size = 1

const char* AP_ADDRESS = "192.168.4.1";
const char* AP_MODE = "AP";
const char* STA_MODE_WIFI_ON = "WIFI_ON";
const char* STA_MODE_WIFI_OFF = "WIFI_OFF";
const char* INTERVAL_MODE = "INTERVAL";
const char* SCHEDULE_MODE = "SCHEDULE";
const uint8_t DEFAULT_TEXT_SIZE = 1;
const uint8_t SSID_MAX_DISPLAY_LENGTH = 11;
bool isInvertDisplay = false;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void displayAdditionalInfo();
void displayDeviceModeInfo();
void displayAlertModeInfo();
void displayInfoTopLeft(const char* info);
void displayInfoTopRight(const char* info);
void displayInfoBottomLeft(const char* info);
void displayInfoBottomRight(const char* info);

void oled::init() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextSize(DEFAULT_TEXT_SIZE);
    display.setTextColor(WHITE);
}

void oled::displayTextCenter(const char* text, bool isDisplayingReadingWeight) {
    displayTextCenter(text, DEFAULT_TEXT_SIZE, isDisplayingReadingWeight);
}

void oled::displayTextCenter(const char* text, uint8_t textSize, bool isDisplayingReadingWeight) {
    int16_t pxLength = strlen(text) * textSize * BASE_WIDTH;
    int16_t pxHeight = (pxLength / SCREEN_WIDTH + 1) * textSize * BASE_HEIGHT;
    int16_t x = pxLength >= SCREEN_WIDTH ? 0 : (SCREEN_WIDTH - pxLength) / 2;
    int16_t y = pxHeight >= SCREEN_HEIGHT ? 0 : (SCREEN_HEIGHT - pxHeight) / 2;
    displayText(text, x, y, textSize, isDisplayingReadingWeight);
}

void oled::displayText(const char* text, uint16_t x, uint16_t y, uint8_t textSize, bool isDisplayingReadingWeight) {
    display.clearDisplay();
    display.invertDisplay(false);
    isInvertDisplay = false;

    if (isDisplayingReadingWeight) {
        displayAdditionalInfo();
    }   

    display.setCursor(x, y);
    display.setTextSize(textSize);
    display.print(text);
    display.display();
}

void displayAdditionalInfo() {
    display.setTextSize(DEFAULT_TEXT_SIZE);
    displayDeviceModeInfo();
    displayAlertModeInfo(); 
}

void displayDeviceModeInfo() {
    const char* mode = STA_MODE_WIFI_OFF;

    if (controller::getDeviceMode() == controller::DeviceMode::AccessPoint) {
        mode = AP_MODE;
        displayInfoBottomRight(AP_ADDRESS);
    } else if (WiFi.status() == WL_CONNECTED) {
        mode = STA_MODE_WIFI_ON;
        displayInfoBottomRight(WiFi.SSID().c_str());
    }

    displayInfoTopRight(mode);
}

void displayAlertModeInfo() {
    if (controller::getAlertMode() == controller::AlertMode::Interval) {
        displayInfoTopLeft(INTERVAL_MODE);

        uint32_t interval = settings::getIntervalAlertInSecond();
        uint8 hour = interval / 3600;
        interval -= hour * 3600; 
        uint8 minute = interval / 60;
        uint8 second = interval % 60;

        char result[10];
        char minuteBuff[3];
        char secondBuff[3];

        itoa(hour, result, 10);            
        itoa(minute, minuteBuff, 10);            
        itoa(second, secondBuff, 10);

        strcat(result, "h");
        strcat(result, minuteBuff);
        strcat(result, "m");
        strcat(result, secondBuff);
        strcat(result, "s");
        
        displayInfoBottomLeft(result);
    } else {
        displayInfoTopLeft(SCHEDULE_MODE);
        uint32_t alertTime = ticker::getAlertTime();

        uint8 hour = alertTime / 3600;
        uint8 minute = (alertTime % 3600) / 60;

        char result[6];
        char minuteBuff[3];

        itoa(hour, result, 10);            
        itoa(minute, minuteBuff, 10);

        strcat(result, ":");
        strcat(result, minuteBuff);

        displayInfoBottomLeft(result);
    }
}

void displayInfoTopLeft(const char* info) {
    display.setCursor(0, 0);
    display.print(info);
}

void displayInfoTopRight(const char* info) {
    display.setCursor(SCREEN_WIDTH - strlen(info) * DEFAULT_TEXT_SIZE * BASE_WIDTH, 0);
    display.print(info);
}

void displayInfoBottomLeft(const char* info) {
    display.setCursor(0, SCREEN_HEIGHT - DEFAULT_TEXT_SIZE * BASE_HEIGHT);
    display.print(info);
}

void displayInfoBottomRight(const char* info) {
    if (strlen(info) > SSID_MAX_DISPLAY_LENGTH) {
        display.setCursor(SCREEN_WIDTH - SSID_MAX_DISPLAY_LENGTH * DEFAULT_TEXT_SIZE * BASE_WIDTH, SCREEN_HEIGHT - DEFAULT_TEXT_SIZE * BASE_HEIGHT);
    } else {
        display.setCursor(SCREEN_WIDTH - strlen(info) * DEFAULT_TEXT_SIZE * BASE_WIDTH, SCREEN_HEIGHT - DEFAULT_TEXT_SIZE * BASE_HEIGHT);
    }
    display.print(info);
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

