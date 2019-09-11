#include <ESP8266WiFiMulti.h>
#include "WifiManager.h"
#include "Network.h"

bool connect(uint8_t maxNetwork, size_t timeout) {
    Network* networks = readNetworksFromEEPROM(maxNetwork);
    for (uint8_t i = 0; i < maxNetwork; i++) {
        if (networks[i].ssid[0] != 0 && networks[i].password[0] != 0) {
            size_t waitingTime = 0;
            char* ssid = networks[i].ssid;
            char* password = networks[i].password;
            Serial.print("Connecting to: ");
            Serial.println(ssid);
            WiFi.begin(ssid, password);

            while (WiFi.status() != WL_CONNECTED && waitingTime < timeout) {
                Serial.print(".");
                delay(1000);
                waitingTime += 1000;
            }

            if (waitingTime < timeout) {
                Serial.println("Success.");
                Serial.print("Connected to: ");
                Serial.println(ssid);
                Serial.print("IP address: ");
                Serial.println(WiFi.localIP());
                free(networks);
                return true;
            } else {
                Serial.println("Failed.");
            }
        }
    }
    free(networks);
    return false;
}

// Not work yet
bool connectByWiFiMulti(uint8_t maxNetwork, size_t timeout) {
    ESP8266WiFiMulti wifiMulti;
    Network* networks = readNetworksFromEEPROM(maxNetwork);

    for (uint8_t i = 0; i < maxNetwork; i++) {
        if (networks[i].ssid[0] != 0 && networks[i].password[0] != 0) {
            wifiMulti.addAP(networks[i].ssid, networks[i].password);
        }
    } 

    Serial.print("Connecting...");
    size_t waitingTime = 0;

    while (wifiMulti.run() != WL_CONNECTED && waitingTime < timeout) {
        Serial.print(".");
        waitingTime += 1000;
        delay(1000);
    }

    if (waitingTime >= timeout) {
        Serial.println("Failed.");
        free(networks);
        return false;
    }

    Serial.println("Success.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    free(networks);
    return true;
}