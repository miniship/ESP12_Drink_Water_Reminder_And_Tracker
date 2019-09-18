#include <Arduino.h>

#ifndef Schedule_h
#define Schedule_h

namespace schedule {
    size_t readReminderIntervalFromEEPROM();
    size_t readReminderIntervalFromEEPROM(size_t address);
    void updateReminderIntervalInEEPROM(byte* paramPayload, unsigned int length);
    void updateReminderIntervalInEEPROM(size_t address, byte* paramPayload, unsigned int length);    
}

#endif