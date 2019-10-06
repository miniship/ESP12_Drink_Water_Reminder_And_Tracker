#include "DeviceController.h"
#include <Arduino.h>
#include "ssd1306oled.h"
#include "hx711scale.h"
#include "DeviceSettings.h"
#include "AccessPointMode.h"
#include "StationMode.h"
#include "TickerManager.h"
#include "Mqtt.h"
#include <Keypad.h>

const byte ROWS = 1;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1','2','3','4'},
};
byte rowPins[ROWS] = {3};
byte colPins[COLS] = {D6, D5, D8, D7};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const uint8_t WEIGHT_CHANGE_THRESHOLD = 10;
const uint16_t VESSEL_WEIGHT_IGNORE = 20;
const char* WEIGHT_UNIT = "g";
uint16_t lastReadingWeight = 9999;
uint16_t lastPublishWeight = 9999;
controller::DeviceMode deviceMode = controller::DeviceMode::Station;
controller::AlertMode alertMode = controller::AlertMode::Schedule;

void startDevice();
void readWeight();
bool isWeightChange(uint16_t last, uint16_t current);
void displayWeight();
void publishWeight();
void softRestartDevice();
void handleKeyInput();
bool isLiftup = false;

void controller::doSetup() {
    oled::init();
    scale::init();
    settings::loadDeviceSettings();
    startDevice();
}

void startDevice() {
    if (deviceMode == controller::DeviceMode::AccessPoint || !station::startStationMode()) {
        deviceMode = controller::DeviceMode::AccessPoint;       
        alertMode = controller::AlertMode::Interval;
        accessPoint::startAccessPointMode();
    }

    if (alertMode == controller::AlertMode::Schedule) {
        ticker::startScheduleAlertTicker();
    } else if (alertMode == controller::AlertMode::Interval) {
        ticker::startIntervalAlertTicker();
    }
    
    ticker::startReadWeightTicker();
    ticker::startPublishWeightTicker();
}

void controller::doLoop() {
    if (ticker::isAlertUserTime()) { // alert user to drink by blinking the screen
        Serial.println("Drink!!!");
        ticker::startBlinkScreenTicker();
        ticker::changeAlertUserFlag(false);
    }

    if (ticker::isBlinkScreenTime()) {
        oled::blinkScreen();
        ticker::autoStopBlinkScreenTicker();
    }

    if (ticker::isReadWeightTime()) {
        readWeight();
        ticker::changeReadWeightFlag(false);
    }

    if (ticker::isPublishWeightTime()) {
        publishWeight();
        ticker::changePublishWeightFlag(false);
    }

    if (deviceMode == controller::DeviceMode::Station) {
        mqtt::handleCommand(); // handle commands received at subscribed mqtt
    } else {
        accessPoint::handleClient();
    }

    handleKeyInput();
}

void readWeight() {
    uint16_t currentReadingWeight = round(scale::readData());

    if (isWeightChange(lastReadingWeight, currentReadingWeight)) {
        ticker::stopBlinkScreenTicker(); // stop blinking screen immediately
    }

    lastReadingWeight = currentReadingWeight;

    if (lastReadingWeight < VESSEL_WEIGHT_IGNORE && !isLiftup) {
        isLiftup = true;
        mqtt::publishLiftup();
    } else if (lastReadingWeight >= VESSEL_WEIGHT_IGNORE && isLiftup) {
        isLiftup = false;
        mqtt::publishPutdown();
    }

    displayWeight();
}

bool isWeightChange(uint16_t last, uint16_t current) {
    return abs(current - last) >= WEIGHT_CHANGE_THRESHOLD;
}

void displayWeight() {
    char buff[8];
    itoa(lastReadingWeight, buff, 10);
    strcat(buff, WEIGHT_UNIT);
    oled::displayTextCenter(buff, 2, true);
}

void publishWeight() {
    if (lastReadingWeight >= VESSEL_WEIGHT_IGNORE && isWeightChange(lastPublishWeight, lastReadingWeight)) {
        mqtt::publishReading(lastReadingWeight);
        lastPublishWeight = lastReadingWeight;
    }
}

controller::DeviceMode controller::getDeviceMode() {
    return deviceMode;
}

controller::AlertMode controller::getAlertMode() {
    return alertMode;
}

void controller::switchDeviceMode() {
    if (deviceMode == AccessPoint) {
        switchToDeviceMode(Station);
    } else if (deviceMode == Station) {
        switchToDeviceMode(AccessPoint);
    }
}

void controller::switchToDeviceMode(DeviceMode mode) {
    deviceMode = mode;
    softRestartDevice();
}

void controller::switchAlertMode() {
    if (alertMode == Schedule) {
        switchToAlertMode(Interval);
    } else if (alertMode == Interval) {
        switchToAlertMode(Schedule);
    }
}

void controller::switchToAlertMode(AlertMode mode) {
    if (mode == Schedule && deviceMode == AccessPoint) {
        return;
    }    

    alertMode = mode;
    if (alertMode == Schedule) {
        ticker::stopIntervalAlertTicker();
        ticker::startScheduleAlertTicker();
    } else if (alertMode == Interval) {
        ticker::stopScheduleAlertTicker();
        ticker::startIntervalAlertTicker();
    }
}

void controller::switchModes(DeviceMode dMode, AlertMode aMode) {
    deviceMode = dMode;
    alertMode = aMode;
    softRestartDevice();
}

void softRestartDevice() {
    ticker::stopAllTickers();
    startDevice();
}

void handleKeyInput() {
    char key = keypad.getKey();
    if (key < '1' || key > '4') {
        return;
    }
    
    switch (key) {
        case '1':
            ESP.restart();
            break;

        case '2':
            controller::switchDeviceMode();
            break;

        case '3':
            controller::switchAlertMode();
            break;

        default:
            Serial.println("Unuse command code");
            break;
    }
}