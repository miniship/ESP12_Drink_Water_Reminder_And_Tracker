#ifndef WifiConnect_h
#define WifiConnect_h

#include <Arduino.h>

namespace wifi {
    struct Network {
        char ssid[31];
        char password[31];
    };

    const uint8_t MAX_NETWORK = 3;

    bool connect();
    Network* readNetworksFromEEPROM();
    bool validateSsid(String ssid);
    bool validatePassword(String password);

    // write a Network to EEPROM at a specific address in format ssid:password;
    uint8_t writeNetworkToEEPROM(size_t address, Network network);

    /*
    write an array of Network to EEPROM at address 0 in format 
        ssid1:password1;ssid2:password2;ssid3:password3;; 
        (extra ; to mark the end off all networks)
    */
    uint8_t writeNetworkArrayToEEPROM(Network* networks, uint8_t size);

    /*
    write an array of Network to EEPROM at a specific address in format 
        ssid1:password1;ssid2:password2;ssid3:password3;; 
        (extra ; to mark the end off all networks)
    */
    uint8_t writeNetworkArrayToEEPROM(size_t address, Network* networks, uint8_t size);

    bool clearEEPROM(size_t size);  // test purpose
}

#endif