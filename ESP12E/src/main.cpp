#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <HX711.h>
#include "Form.h"
#include "WifiManager.h"

const size_t BAUDRATE = 9600;
const size_t EEPROM_SIZE = 512;
const uint8_t MAX_NETWORK = 3;
const size_t CONNECT_TIMEOUT = 10000;
const char* AP_MODE_SSID = "ESP12AP";
const char* AP_MODE_PASSWORD = "123123123";
const char* ROOT_PATH = "/";
const char* SUBMIT_PATH = "/submit";
const char* SSID_ERROR_MESSAGE = "SSID must be between 2 and 30 characters length. "
                                  "It cannot contain ; :";
const char* PASSWORD_ERROR_MESSAGE = "Password must be between 8 and 30 characters length. "
                                      "It cannot contain ; : \' \"";
const float calibration_factor = 414;

ESP8266WebServer server(80);
HX711 scale;

void handleRoot() {
    Serial.println("Handling request to root.");
    Network* networks = readNetworksFromEEPROM(MAX_NETWORK);
    String html = generateIndexHTMLString(networks, MAX_NETWORK);
    free(networks);
    server.send(200, "text/html", html);
}

void handleSubmit() {
    Serial.println("Handling submit request.");
    Network* networks = new Network[MAX_NETWORK];
    String* ssidInputMessage = new String[MAX_NETWORK];
    String* passwordInputMessage = new String[MAX_NETWORK];
    bool error = false;

    for (uint8_t i = 0; i < MAX_NETWORK; i++) {
        String ssidArg = "ssid";
        String passwordArg = "password";
        ssidArg += i;
        passwordArg += i;
        String ssid = server.arg(ssidArg);
        String password = server.arg(passwordArg);
        ssid.trim();
        password.trim();
        ssid.toCharArray(networks[i].ssid, 31);
        password.toCharArray(networks[i].password, 31);

        Serial.print("Submitted ssid");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(ssid);
        Serial.print("Submitted password");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(password);

        if (ssid.length() > 0 || password.length() > 0) {
            if (ssid.length() == 0 || !validateSsid(ssid)) {
                ssidInputMessage[i] = SSID_ERROR_MESSAGE;
                error = true;
            }

            if (password.length() == 0 || !validateSsid(password)) {
                passwordInputMessage[i] = PASSWORD_ERROR_MESSAGE;
                error = true;
            }
        }
    }

    if (error) {
        String html = generateIndexHTMLString(networks, ssidInputMessage, passwordInputMessage, MAX_NETWORK);
        Serial.println("Invalid input.");
        server.send(400, "text/html", html);
    } else {
        uint8_t savedNetworks = writeNetworkArrayToEEPROM(networks, MAX_NETWORK);
        Serial.print("Successfully save ");
        Serial.print(savedNetworks);
        Serial.println(" networks");
        server.send(200, "text/html", "Restarting device in 3 seconds");
        delay(3000);
        ESP.restart();
    }
}

void handleNotFound() {
    Serial.println("Handling not found request.");
    String html = generateNotFoundHTMLString(server.uri(), server.method());
    server.send(404, "text/plain", html);
}

void stationAction() {
    scale.power_up();

    float data = scale.get_units();
    // sendWeight(data);

    scale.power_down();
  
    Serial.println(data, 2);
    delay(2000);
}

void startServer() {
    server.on(ROOT_PATH, handleRoot);
    server.on(SUBMIT_PATH, handleSubmit);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started.");
}

bool startStationMode() {
    Serial.println("Starting station mode...");
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    delay(1000);
    return connect(MAX_NETWORK, CONNECT_TIMEOUT);
}

void startAccessPointMode() {
    Serial.print("Starting access point mode...");
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_AP);
    delay(1000);
    while (!WiFi.softAP(AP_MODE_SSID, AP_MODE_PASSWORD)) {
        Serial.print(".");
        delay(1000);
    }
    startServer();
}

void setup() {
    Serial.begin(BAUDRATE);
    EEPROM.begin(EEPROM_SIZE);
    delay(2000);
    if (startStationMode()) {
        scale.begin(D2, D1);
        scale.set_scale(calibration_factor);
        scale.tare();
        Serial.println("HTTP client started.");  
    } else {
        startAccessPointMode();
    }    
}

void loop() {
    if (WiFi.getMode() == WIFI_STA) {
        stationAction();
    } else if (WiFi.getMode() == WIFI_AP) {
        server.handleClient();
    }
}