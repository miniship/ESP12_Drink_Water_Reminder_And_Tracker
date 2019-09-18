#include "Schedule.h"
#include <EEPROM.h>

const size_t EEPROM_ADDRESS = 300;
const uint8_t PARAM_LENGTH_LIMIT = 3;   // interval in minute [1-99]

size_t schedule::readReminderIntervalFromEEPROM() {
    return readReminderIntervalFromEEPROM(EEPROM_ADDRESS);
}

size_t schedule::readReminderIntervalFromEEPROM(size_t address) {
    size_t result = 0;

    for (uint8_t i = 0; i < PARAM_LENGTH_LIMIT; i++) {
        char curChar = EEPROM.read(address++);

        if (curChar == ';') {
            return result;
        }

        if (curChar < '0' || curChar > '9') {
            Serial.println("[readInterval] interval must be an int");
            return 0;
        }

        result = result * 10 + curChar - '0';
    }

    Serial.println("[readInterval] interval value is too large");
    return 0;
}

void schedule::updateReminderIntervalInEEPROM(byte* paramPayload, unsigned int length) {
    updateReminderIntervalInEEPROM(EEPROM_ADDRESS, paramPayload, length);
}

void schedule::updateReminderIntervalInEEPROM(size_t address, byte* paramPayload, unsigned int length) {
    if (length < 2) {
        Serial.println("[updateInterval] param value is empty or not end with [;]");
        return;
    }
    if (length > PARAM_LENGTH_LIMIT) {
        Serial.println("[updateInterval] param value is too large");
        return;
    }

    char param[length];
    bool validParam = false;

    for (unsigned int i = 0; i < length; i++) {
        char curChar = paramPayload[i];

        param[i] = curChar;
        if (curChar == ';') {
            validParam = true;
            break;
        }

        if (curChar < '0' || curChar > '9') {
            Serial.println("[updateInterval] param must be an int");
            return;
        }
    }

    if (!validParam) {
        Serial.println("[updateInterval] must have [;] in the end");
        return;
    }

    for (uint8_t i = 0; i < length; i++) {
        EEPROM.write(address++, param[i]);
    }
    EEPROM.commit();
    Serial.printf("[updateInterval] Write param %s successfully, end at address %d ", param, address);
}