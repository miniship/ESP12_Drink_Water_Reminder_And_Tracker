#include "AccessPointMode.h"
#include "ssd1306oled.h"
#include <ESP8266WebServer.h>
#include "WifiConnect.h"
#include "HtmlGenerator.h"

const char* AP_MODE_SSID = "ESP12AP";
const char* AP_MODE_PASSWORD = "123123123";
const char* ROOT_PATH = "/";
const char* SUBMIT_PATH = "/submit";
const char* SSID_ERROR_MESSAGE = "SSID must be between 2 and 30 characters length. "
                                  "It cannot contain ; :";
const char* PASSWORD_ERROR_MESSAGE = "Password must be between 8 and 30 characters length. "
                                      "It cannot contain ; : \' \"";
const uint8_t SSID_MIN_LENGTH = 2;
const uint8_t SSID_MAX_LENGTH = 30;
const uint8_t PASSWORD_MIN_LENGTH = 8;
const uint8_t PASSWORD_MAX_LENGTH = 30;

ESP8266WebServer server(80);

void startServer();
void handleRoot();
void handleSubmit();
wifi::Network* getInputNetworkList();
bool validateInputNetworkList(wifi::Network* networkList, String* ssidInputMessage, String* passwordInputMessage);
bool validateNetwork(wifi::Network network, uint8_t index, String* ssidInputMessage, String* passwordInputMessage);
bool validateSsid(char ssid[], uint8_t ssidLen);
bool validatePassword(char password[], uint8_t passLen);
void handleNotFound();

void accessPoint::startAccessPointMode() {
    oled::displayTextCenter("Station mode failed\nStarting access point mode\nIt should take less than one minute", false);
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_AP);
    delay(1000);
    WiFi.softAP(AP_MODE_SSID, AP_MODE_PASSWORD);
    delay(1000);
    startServer();
}

void startServer() {
    server.on(ROOT_PATH, handleRoot);
    server.on(SUBMIT_PATH, handleSubmit);
    server.onNotFound(handleNotFound);
    server.begin();
}

void handleRoot() {
    String index = html::generateIndex();
    server.send(200, "text/html", index);
}

void handleSubmit() {
    // Serial.println("Handling submit request.");

    wifi::Network* networkList = getInputNetworkList();
    String* ssidInputMessage = new String[wifi::MAX_NETWORK];
    String* passwordInputMessage = new String[wifi::MAX_NETWORK];

    if (validateInputNetworkList(networkList, ssidInputMessage, passwordInputMessage) && wifi::updateNetworkList()) {
        String message = "Successfully save networkList. Device will be restarted" ;
        server.send(200, "text/html", message);
        delay(1000);
        ESP.restart();      

    } else {
        String html = html::generateIndex(networkList, ssidInputMessage, passwordInputMessage);
        server.send(400, "text/html", html);        
    }
}

wifi::Network* getInputNetworkList() {
    wifi::Network* networkList = wifi::getNetworkList();

    for (uint8_t i = 0; i < wifi::MAX_NETWORK; i++) {
        String ssidArg = "ssid";
        String passwordArg = "password";
        ssidArg += i;
        passwordArg += i;
        String ssid = server.arg(ssidArg);
        String password = server.arg(passwordArg);
        ssid.trim();
        password.trim();
        ssid.toCharArray(networkList[i].ssid, 31);
        password.toCharArray(networkList[i].password, 31);        

        // Serial.printf("Submitted ssid%d: ", i);
        // Serial.println(ssid);
        // Serial.printf("Submitted password%d: ", i);
        // Serial.println(password);
    }

    return networkList;
}

bool validateInputNetworkList(wifi::Network* networkList, String* ssidInputMessage, String* passwordInputMessage) {
    bool isValidate = true;

    for (uint8_t i = 0; i < wifi::MAX_NETWORK; i++) {
        if (validateNetwork(networkList[i], i, ssidInputMessage, passwordInputMessage)) {
            continue;
        }

        isValidate = false;
    }

    return isValidate;
}

bool validateNetwork(wifi::Network network, uint8_t index, String* ssidInputMessage, String* passwordInputMessage) {
    uint8_t ssidLen = strlen(network.ssid);
    uint8_t passLen = strlen(network.password);
    bool isValidate = true; 

    if (ssidLen == 0 && passLen == 0) {
        return isValidate;
    }

    if (!validateSsid(network.ssid, ssidLen)) {
        ssidInputMessage[index] = SSID_ERROR_MESSAGE;
        isValidate = false;
    }

    if (!validatePassword(network.password, passLen)) {
        passwordInputMessage[index] = PASSWORD_ERROR_MESSAGE;
        isValidate = false;
    }

    return isValidate;
}

bool validateSsid(char ssid[], uint8_t ssidLen) {
    if (ssid == NULL || ssidLen < SSID_MIN_LENGTH || ssidLen > SSID_MAX_LENGTH) {
        return false;
    }

    for (uint8_t i = 0; i < ssidLen; i++) {
        char curChar = ssid[i];
        if (curChar == ':' || curChar == ';') {
            return false;
        }
    }

    return true;
}

bool validatePassword(char password[], uint8_t passLen) {
    if (password == NULL || passLen < PASSWORD_MIN_LENGTH || passLen > PASSWORD_MAX_LENGTH) {
        return false;
    }

    for (uint8_t i = 0; i < passLen; i++) {
        char curChar = password[i];
        if (curChar == ':' || curChar == ';' || curChar == '"' || curChar == '\'') {
            return false;
        }
    }   

    return true;
}

void handleNotFound() {
    String html = html::generateNotFound(server.uri(), server.method());
    server.send(404, "text/plain", html);
}

void accessPoint::handleClient() {
    server.handleClient();
}