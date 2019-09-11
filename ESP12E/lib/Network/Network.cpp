#include <EEPROM.h>
#include "Network.h"

bool validateSsid(String ssid) {
    if (ssid == NULL || ssid.length() < 2 || ssid.length() > 30 || ssid.indexOf(':') >= 0 || ssid.indexOf(';') >= 0) {
        return false;
    }
    return true;
}

bool validatePassword(String password) {
    if (password == NULL || password.length() < 8 || password.length() > 30 || 
        password.indexOf(':') >= 0 || password.indexOf(';') >= 0 || password.indexOf('"') >= 0 || password.indexOf('\'') >= 0) {
        return false;
    }
    return true;
}

uint8_t writeNetworkToEEPROM(size_t address, Network network) {
    if (address < 0 || address > EEPROM.length() || 
    network.ssid == NULL || network.password == NULL ||
    strlen(network.ssid) < 1 || strlen(network.password) < 1) {
        return 0;
    }

    uint8_t ssidLen = strlen(network.ssid);
    uint8_t passwordLen = strlen(network.password);

    for (uint8_t i = 0; i < ssidLen; i++) {
        EEPROM.write(address++, network.ssid[i]);
    }
    EEPROM.write(address++, ':');
    for (uint8_t j = 0; j < passwordLen; j++) {
        EEPROM.write(address++, network.password[j]);
    }
    EEPROM.write(address++, ';');
    
    if (EEPROM.commit()) {
        return ssidLen + passwordLen + 2;
    }
    return 0;    
}

uint8_t writeNetworkArrayToEEPROM(Network* networks, uint8_t size) {
    return writeNetworkArrayToEEPROM(0, networks, size);
}

uint8_t writeNetworkArrayToEEPROM(size_t address, Network* networks, uint8_t size) {
    uint8_t result = 0;

    for (uint8_t i = 0; i < size; i++) {
        uint8_t writtenChars = writeNetworkToEEPROM(address, networks[i]);
        if (writtenChars > 0) {
            result++;
            address += writtenChars;
        }
    }

    EEPROM.write(address, ';');
    if (EEPROM.commit()) {
        return result;
    }
    return 0;
}

Network* readNetworksFromEEPROM(uint8_t size) {
    Network* networks = (Network*)calloc(size, sizeof(Network));
    for (uint8_t i = 0; i < size; i++) {
        networks[i].ssid[0] = 0;
        networks[i].password[0] = 0;
    }

    uint8_t networkIndex = 0;
    uint8_t index = 0;
    bool isSsid = true;

    for (size_t i = 0; i < EEPROM.length(); i++) {
        char curChar = EEPROM.read(i);

        if (curChar < 32 || curChar > 126) {
            break;
        }

        if (curChar == ':') {
            isSsid = false;
            index = 0;
            continue;
        }

        if (curChar == ';') {
            if (EEPROM.read(i + 1) == ';' || networkIndex == size) {
                return networks;
            }
            networkIndex++;
            isSsid = true;
            index = 0;
            continue;
        }

        if (isSsid) {
            networks[networkIndex].ssid[index++] = curChar;
        } else {
            networks[networkIndex].password[index++] = curChar;
        }
    }

    return networks;
}

bool clearEEPROM(size_t size) {
    for (size_t i = 0; i < size; i++) {
        EEPROM.write(i, 0);
    }
    return EEPROM.commit();
}