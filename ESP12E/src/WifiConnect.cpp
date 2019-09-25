#include "wifiConnect.h"
#include <EEPROM.h>
#include <ESP8266WiFi.h>

const uint16_t CONNECT_TIMEOUT = 10000;
const uint16_t EEPROM_NETWORKS_ADDRESS = 0;
wifi::Network networkList[wifi::MAX_NETWORK];

void wifi::loadNetworkList() {
    EEPROM.get(EEPROM_NETWORKS_ADDRESS, networkList);
}

wifi::Network* wifi::getNetworkList() {
    return networkList;
}

bool wifi::updateNetworkList() {
    EEPROM.put(EEPROM_NETWORKS_ADDRESS, networkList);
    return EEPROM.commit();
}

bool wifi::connect() {
    for (uint8_t i = 0; i < MAX_NETWORK; i++) {
        Network network = networkList[i];

        if (network.ssid == NULL || network.password == NULL
            || strlen(network.ssid) == 0 || strlen(network.password) == 0) {
                continue;
        }

        if (connect(network)) {
            return true;
        }
    }

    return false;
}

bool wifi::connect(Network network) {
    uint16_t waitingTime = 0;
    char* ssid = network.ssid;
    char* password = network.password;
    // Serial.print("Connecting to: ");
    // Serial.print(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && waitingTime < CONNECT_TIMEOUT) { // try connecting until timeout reached
        // Serial.print(".");
        delay(1000);
        waitingTime += 1000;
    }

    if (waitingTime < CONNECT_TIMEOUT) { // connected
        // Serial.println("Success.");
        // Serial.print("IP address: ");
        // Serial.println(WiFi.localIP());
        return true;
    }
    
    // Serial.println("Failed.");
    return false;
}

bool wifi::clearEEPROM(uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        EEPROM.write(i, 0);
    }
    return EEPROM.commit();
}