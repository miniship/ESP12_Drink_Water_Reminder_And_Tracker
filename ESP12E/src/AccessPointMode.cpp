#include "AccessPointMode.h"
#include "ssd1306oled.h"
#include <ESP8266WebServer.h>
#include "DeviceSettings.h"
#include "HtmlGenerator.h"
#include "DeviceController.h"

const char* AP_MODE_SSID = "ESP12AP";
const char* AP_MODE_PASSWORD = "123123123";
const char* ROOT_PATH = "/";
const char* SUBMIT_PATH = "/submit";
const char* SSID_ERROR_MESSAGE = " is invalid. SSID must be between 2 and 30 characters length. "
                                  "It cannot contain ; :";
const char* PASSWORD_ERROR_MESSAGE = " is invalid. Password must be between 8 and 30 characters length. "
                                      "It cannot contain ; : \' \"";
const char* INTERVAL_ALERT_ERROR_MESSAGE = "Interval alert in second must be an integer from 30 to 86400";
const char* SCHEDULE_ALERT_ERROR_MESSAGE = " is invalid time of day";

const uint8_t SSID_MIN_LENGTH = 2;
const uint8_t SSID_MAX_LENGTH = 30;
const uint8_t PASSWORD_MIN_LENGTH = 8;
const uint8_t PASSWORD_MAX_LENGTH = 30;

ESP8266WebServer server(80);

void startServer();
void handleRoot();
void handleSubmit();
settings::DeviceSettings getInputSettings();
bool validateInputNetworkList(settings::Network* inputNetworkList, uint8_t networkCounter, String* ssidInputMessage, String* passwordInputMessage);
bool validateNetwork(settings::Network network, uint8_t index, String* ssidInputMessage, String* passwordInputMessage);
bool validateSsid(char ssid[], uint8_t ssidLen);
bool validatePassword(char password[], uint8_t passLen);
bool validateInputScheduleAlertList(uint32_t* inputScheduleAlertList, uint8_t alertsPerDay, String* scheduleAlertInputMessage);
void handleNotFound();

void accessPoint::startAccessPointMode() {
    oled::displayTextCenter("Starting access point mode\nIt should take less than one minute", false);
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
    settings::DeviceSettings inputSettings = getInputSettings();
    String* ssidInputMessage = new String[settings::MAX_STORED_NETWORK];
    String* passwordInputMessage = new String[settings::MAX_STORED_NETWORK];
    String intervalAlertInputMessage = "";
    String* scheduleAlertInputMessage = new String[settings::MAX_ALERT_PER_DAY];
    bool isValid = true;

    if (!validateInputNetworkList(inputSettings.networkList, inputSettings.networkCount, ssidInputMessage, passwordInputMessage)) {
        isValid = false;
    }

    if (inputSettings.intervalAlertInSecond < settings::MIN_INTERVAL_ALERT_IN_SECOND || inputSettings.intervalAlertInSecond > settings::MAX_INTERVAL_ALERT_IN_SECOND) {
        intervalAlertInputMessage = INTERVAL_ALERT_ERROR_MESSAGE;
        isValid = false;
    }

    if (!validateInputScheduleAlertList(inputSettings.timeOfDayInSecondList, inputSettings.alertTimesPerDay, scheduleAlertInputMessage)) {
        isValid = false;
    } else {
        std::sort(inputSettings.timeOfDayInSecondList, inputSettings.timeOfDayInSecondList + inputSettings.alertTimesPerDay);
    }

    if (!isValid) {
        String html = html::generateIndex(inputSettings, 
                                    ssidInputMessage,
                                    passwordInputMessage,
                                    intervalAlertInputMessage,
                                    scheduleAlertInputMessage);
        server.send(400, "text/html", html);
        return;
    }

    if (!settings::updateDeviceSettings(inputSettings)) {
        server.send(500, "text/html", "Could not update device settings");
    }

    String message = "Successfully save device settings. Switch to station mode" ;
    server.send(200, "text/html", message);
    delay(1000);
    controller::switchModes(controller::DeviceMode::Station, controller::AlertMode::Schedule);
}

settings::DeviceSettings getInputSettings() {
    settings::DeviceSettings inputSettings = settings::getDeviceSettings();
    settings::Network* inputNetworkList = inputSettings.networkList;
    uint32_t* inputScheduleAlertList = inputSettings.timeOfDayInSecondList;
    uint8_t counter = 0;

    for (uint8_t i = 1; i <= settings::MAX_STORED_NETWORK; i++) {
        String ssidArg = "ssid";
        String passwordArg = "password";
        ssidArg += i;
        passwordArg += i;
        String inputSsid = "";
        String inputPassword = ""; 
        
        if (server.hasArg(ssidArg)) {
            inputSsid = server.arg(ssidArg);
        }

        if (server.hasArg(passwordArg)) {
            inputPassword = server.arg(passwordArg);
        }

        inputSsid.trim();
        inputPassword.trim();
        if (inputSsid.length() == 0 && inputPassword.length() == 0) {
            continue;
        }

        inputSsid.toCharArray(inputNetworkList[counter].ssid, 31);
        inputPassword.toCharArray(inputNetworkList[counter].password, 31);
        counter++;
    }
    inputSettings.networkCount = counter;

    String inputIntervalAlertString = server.arg("intervalAlert");
    inputIntervalAlertString.trim();
    uint32_t inputIntervalAlert = inputIntervalAlertString.toInt();
    inputSettings.intervalAlertInSecond = inputIntervalAlert;

    counter = 0;
    for (uint8_t i = 1; i <= settings::MAX_ALERT_PER_DAY; i++) {
        String hourArg = "hour";
        String minuteArg = "minute";
        hourArg += i;
        minuteArg += i;
        String inputHour = "";
        String inputMinute = ""; 
        
        if (server.hasArg(hourArg)) {
            inputHour = server.arg(hourArg);
        }

        if (server.hasArg(minuteArg)) {
            inputMinute = server.arg(minuteArg);
        }

        inputHour.trim();
        inputMinute.trim();
        if (inputHour.length() == 0 && inputMinute.length() == 0) {
            continue;
        }

        uint8_t hour = inputHour.toInt();
        uint8_t minute = inputMinute.toInt();
        inputScheduleAlertList[counter++] = hour * 3600 + minute * 60;
    }
    inputSettings.alertTimesPerDay = counter;

    return inputSettings;
}

bool validateInputNetworkList(settings::Network* inputNetworkList, uint8_t networkCounter, String* ssidInputMessage, String* passwordInputMessage) {
    bool isValid = true;

    for (uint8_t i = 0; i < networkCounter; i++) {
        if (validateNetwork(inputNetworkList[i], i, ssidInputMessage, passwordInputMessage)) {
            continue;
        }

        isValid = false;
    }

    return isValid;
}

bool validateNetwork(settings::Network network, uint8_t index, String* ssidInputMessage, String* passwordInputMessage) {
    uint8_t ssidLen = strlen(network.ssid);
    uint8_t passLen = strlen(network.password);
    bool isValid = true;

    if (!validateSsid(network.ssid, ssidLen)) {
        String errorMessage = "SSID";
        errorMessage += index + 1;
        errorMessage += SSID_ERROR_MESSAGE;
        ssidInputMessage[index] = errorMessage;
        isValid = false;
    }

    if (!validatePassword(network.password, passLen)) {
        String errorMessage = "Password";
        errorMessage += index + 1;
        errorMessage += PASSWORD_ERROR_MESSAGE;
        passwordInputMessage[index] = errorMessage;
        isValid = false;
    }

    return isValid;
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

bool validateInputScheduleAlertList(uint32_t* inputScheduleAlertList, uint8_t alertsPerDay, String* scheduleAlertInputMessage) {
    bool isValid = true;

    for (uint8_t i = 0; i < alertsPerDay; i++) {
        if (inputScheduleAlertList[i] > settings::MAX_INTERVAL_ALERT_IN_SECOND) {
            String errorMessage = "Schedule ";
            errorMessage += i + 1;
            errorMessage += SCHEDULE_ALERT_ERROR_MESSAGE;
            scheduleAlertInputMessage[i] = errorMessage;
            isValid = false;
        }
    }

    return isValid;
}

void handleNotFound() {
    String html = html::generateNotFound(server.uri(), server.method());
    server.send(404, "text/plain", html);
}

void accessPoint::handleClient() {
    server.handleClient();
}