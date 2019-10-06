#include <Arduino.h>
#include <EEPROM.h>
#include "DeviceController.h"
#include "DeviceSettings.h"

const uint16_t BAUDRATE = 9600;
const uint16_t EEPROM_SIZE = 1024;

void initEEPROMData() {
    settings::DeviceSettings settings;
    settings.networkCount = 0;
    settings.alertTimesPerDay = 0;
    settings.intervalAlertInSecond = 60;
    settings::updateDeviceSettings(settings);
}

void setup() {
    Serial.begin(BAUDRATE);
    EEPROM.begin(EEPROM_SIZE);
    delay(2000);

    // initEEPROMData(); 
    
    controller::doSetup();
}

void loop() {
    controller::doLoop();
}

