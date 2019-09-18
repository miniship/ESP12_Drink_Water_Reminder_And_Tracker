#include "wifiConnect.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>

const size_t CONNECT_TIMEOUT = 10000;

bool wifi::connect() {
    Network* networks = readNetworksFromEEPROM();
    for (uint8_t i = 0; i < MAX_NETWORK; i++) {
        if (networks[i].ssid == NULL || networks[i].password == NULL || 
            networks[i].ssid[0] == 0 || networks[i].password[0] == 0) {
                continue;
        }

        size_t waitingTime = 0;
        char* ssid = networks[i].ssid;
        char* password = networks[i].password;
        Serial.print("Connecting to: ");
        Serial.print(ssid);

        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED && waitingTime < CONNECT_TIMEOUT) {
            Serial.print(".");
            delay(1000);
            waitingTime += 1000;
        }

        if (waitingTime < CONNECT_TIMEOUT) {
            Serial.println("Success.");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            free(networks);
            return true;
        } else {
            Serial.println("Failed.");
        }
    }

    free(networks);
    return false;
}

wifi::Network* wifi::readNetworksFromEEPROM() {
    Network* networks = (Network*)calloc(MAX_NETWORK, sizeof(Network));
    for (uint8_t i = 0; i < MAX_NETWORK; i++) {
        networks[i].ssid[0] = 0;
        networks[i].password[0] = 0;
    }

    uint8_t networkIndex = 0;
    uint8_t readingIndex = 0;
    bool isSsid = true;

    for (size_t i = 0; i < EEPROM.length(); i++) {
        char curChar = EEPROM.read(i);
        if (curChar < 32 || curChar > 126) {    // invalid char
            break;
        }

        if (curChar == ':') {   // separator between ssid and password
            isSsid = false;
            readingIndex = 0;
            continue;
        }

        if (curChar == ';') {   // separator between 2 networks, an extra ';' marks the end of all networks 
            if (EEPROM.read(i + 1) == ';' || networkIndex == MAX_NETWORK) {
                return networks;
            }
            networkIndex++;
            isSsid = true;
            readingIndex = 0;
            continue;
        }

        if (isSsid) {
            networks[networkIndex].ssid[readingIndex++] = curChar;
        } else {
            networks[networkIndex].password[readingIndex++] = curChar;
        }
    }

    return networks;
}

bool wifi::validateSsid(String ssid) {
    if (ssid == NULL || ssid.length() < 2 || ssid.length() > 30 || ssid.indexOf(':') >= 0 || ssid.indexOf(';') >= 0) {
        return false;
    }
    return true;
}

bool wifi::validatePassword(String password) {
    if (password == NULL || password.length() < 8 || password.length() > 30 || 
        password.indexOf(':') >= 0 || password.indexOf(';') >= 0 || password.indexOf('"') >= 0 || password.indexOf('\'') >= 0) {
        return false;
    }
    return true;
}

uint8_t wifi::writeNetworkToEEPROM(size_t address, Network network) {
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

uint8_t wifi::writeNetworkArrayToEEPROM(Network* networks, uint8_t size) {
    return writeNetworkArrayToEEPROM(0, networks, size);
}

uint8_t wifi::writeNetworkArrayToEEPROM(size_t address, Network* networks, uint8_t size) {
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

bool wifi::clearEEPROM(size_t size) {
    for (size_t i = 0; i < size; i++) {
        EEPROM.write(i, 0);
    }
    return EEPROM.commit();
}