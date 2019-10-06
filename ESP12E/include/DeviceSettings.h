#ifndef DeviceSettings_h
#define DeviceSettings_h

#include <stdint.h>

namespace settings {
    const uint8_t MAX_STORED_NETWORK = 5;
    const uint8_t MIN_INTERVAL_ALERT_IN_SECOND = 30;
    const uint32_t MAX_INTERVAL_ALERT_IN_SECOND = 86400;
    const uint8_t MAX_ALERT_PER_DAY = 24;

    struct Network {
        char ssid[31];
        char password[31];
    };

    struct DeviceSettings{
        uint8_t networkCount;
        Network networkList[MAX_STORED_NETWORK];
        uint32_t intervalAlertInSecond;
        uint8_t alertTimesPerDay;
        uint32_t timeOfDayInSecondList[MAX_ALERT_PER_DAY];
    };
    
    void loadDeviceSettings();
    DeviceSettings getDeviceSettings();
    uint8_t getNetworkCount();
    Network* getNetworkList();
    uint32_t getIntervalAlertInSecond();
    uint8_t getAlertTimesPerDay();
    uint32_t* getTimeOfDayInSecondList();
    bool updateDeviceSettings(DeviceSettings settings);
    bool clearEEPROM(uint16_t size);  // test purpose
}

#endif