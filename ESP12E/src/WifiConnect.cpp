#include "wifiConnect.h"
#include <ESP8266WiFi.h>

const uint16_t CONNECT_TIMEOUT = 10000;

bool wifi::connect() {
    settings::Network* networkList = settings::getNetworkList();

    for (uint8_t i = 0; i < settings::getNetworkCount(); i++) {
        settings::Network network = networkList[i];

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

bool wifi::connect(settings::Network network) {
    uint16_t waitingTime = 0;
    char* ssid = network.ssid;
    char* password = network.password;
    Serial.print("Connecting to: ");
    Serial.print(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && waitingTime < CONNECT_TIMEOUT) { // try connecting until timeout reached
        Serial.print(".");
        delay(1000);
        waitingTime += 1000;
    }

    if (waitingTime < CONNECT_TIMEOUT) { // connected
        Serial.println("Success.");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    
    Serial.println("Failed.");
    return false;
}

