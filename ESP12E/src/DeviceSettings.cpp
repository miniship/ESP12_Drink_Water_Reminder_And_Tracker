#include "DeviceSettings.h"
#include <EEPROM.h>
#include <Arduino.h>

const uint16_t EEPROM_DEVICE_SETTINGS_ADDRESS = 0;

settings::DeviceSettings deviceSettings;

void settings::loadDeviceSettings() {
    EEPROM.get(EEPROM_DEVICE_SETTINGS_ADDRESS, deviceSettings);
}

settings::DeviceSettings settings::getDeviceSettings() {
    return deviceSettings;
}

uint8_t settings::getNetworkCount() {
    if (deviceSettings.networkCount < 0) {
        return 0;
    }

    if (deviceSettings.networkCount > MAX_STORED_NETWORK) {
        return MAX_STORED_NETWORK;
    }
    
    return deviceSettings.networkCount;
}

settings::Network* settings::getNetworkList() {
    return deviceSettings.networkList;
}

uint32_t settings::getIntervalAlertInSecond() {
    if (deviceSettings.intervalAlertInSecond < MIN_INTERVAL_ALERT_IN_SECOND) {
        return MIN_INTERVAL_ALERT_IN_SECOND;
    }

    if (deviceSettings.intervalAlertInSecond > MAX_INTERVAL_ALERT_IN_SECOND) {
        return MAX_INTERVAL_ALERT_IN_SECOND;
    }

    return deviceSettings.intervalAlertInSecond;
}

uint8_t settings::getAlertTimesPerDay() {
    if (deviceSettings.alertTimesPerDay < 0) {
        return 0;
    }

    if (deviceSettings.alertTimesPerDay > MAX_ALERT_PER_DAY) {
        return MAX_ALERT_PER_DAY;
    }
    
    return deviceSettings.alertTimesPerDay;
}

uint32_t* settings::getTimeOfDayInSecondList() {
    return deviceSettings.timeOfDayInSecondList;
}

bool settings::updateDeviceSettings(DeviceSettings settings) {
    deviceSettings = settings;
    EEPROM.put(EEPROM_DEVICE_SETTINGS_ADDRESS, deviceSettings);
    return EEPROM.commit();
}

bool settings::clearEEPROM(uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        EEPROM.write(i, 0);
    }
    return EEPROM.commit();
}