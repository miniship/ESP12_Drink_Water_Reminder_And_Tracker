#ifndef WifiConnect_h
#define WifiConnect_h

#include <stdint.h>

namespace wifi {
    struct Network {
        char ssid[31];
        char password[31];
    };

    const uint8_t MAX_NETWORK = 3; // maximum stored networks in EEPROM;    

    void loadNetworkList();
    Network* getNetworkList();
    bool updateNetworkList();
    bool connect();
    bool connect(Network network);
    bool clearEEPROM(uint16_t size);  // test purpose
}

#endif