#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "ssd1306oled.h"
#include "hx711scale.h"
#include "WifiConnect.h"
#include "HtmlGenerator.h"
#include "Schedule.h"

const size_t BAUDRATE = 9600;
const size_t EEPROM_SIZE = 512;
const char* AP_MODE_SSID = "ESP12AP";
const char* AP_MODE_PASSWORD = "123123123";
const char* ROOT_PATH = "/";
const char* SUBMIT_PATH = "/submit";
const char* SSID_ERROR_MESSAGE = "SSID must be between 2 and 30 characters length. "
                                  "It cannot contain ; :";
const char* PASSWORD_ERROR_MESSAGE = "Password must be between 8 and 30 characters length. "
                                      "It cannot contain ; : \' \"";
// const char* MQTT_SERVER = "35.197.155.112";
// const size_t MQTT_PORT = 1883;
const char* MQTT_SERVER = "192.168.137.1";
const size_t MQTT_PORT = 1883;
const size_t MQTT_TIMEOUT = 10000;
String COMMAND_TOPIC = "command/";
String DEVICE_TOPIC = "device/";
enum CommandCode{UnuseCode, RestartDevice, UpdateSchedule, AlertUser};
const size_t DEFAULT_REMINDER_INTERVAL_SECOND = 60;
const uint8_t PUBLISH_READING_INTERVAL_SECOND = 10;
const uint8_t WEIGHT_CHANGE_THRESHOLD = 10;

ESP8266WebServer server(80);
WiFiClient espWifiClient;
PubSubClient mqttClient(espWifiClient);
size_t reminderIntervalInSecond = DEFAULT_REMINDER_INTERVAL_SECOND;
size_t publishTimerInSecond = PUBLISH_READING_INTERVAL_SECOND;  // publish reading when begin
size_t reminderTimerInSecond = DEFAULT_REMINDER_INTERVAL_SECOND;// reminder when begin

bool startStationMode();
bool connectMqtt();
void callback(char* topic, byte* payload, unsigned int length);
void startAccessPointMode();
void startServer();
void handleRoot();
void handleSubmit();
void handleNotFound();
void reconnect();
void stationAction();
void alertUser();
bool isWeightChange(size_t currentWeight);
void checkPublishTime(size_t currentWeight);
void publishReading(size_t data);
void displayWeight();

size_t lastWeight = 0;
bool reminderFlag = false;
bool weightChange = false;

void setup() {
    Serial.begin(BAUDRATE);
    EEPROM.begin(EEPROM_SIZE);
    delay(2000);
    scale::init();
    oled::init();
    
    if (startStationMode()) {
        oled::displayTextCenter("In station mode");
        size_t storedReminderInterval = schedule::readReminderIntervalFromEEPROM();

        if (storedReminderInterval > 0) {
            reminderIntervalInSecond = storedReminderInterval * 60;
            reminderTimerInSecond = reminderIntervalInSecond;
        }

        Serial.printf("Interval: %d seconds. ", reminderIntervalInSecond);
    } else {
        startAccessPointMode();    
    }       
}

bool startStationMode() {
    oled::displayTextCenter("Start station mode\nIt should take less than one minute");
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    delay(1000);
    return wifi::connect() && connectMqtt();
}

bool connectMqtt() {
    Serial.print("Attempting MQTT connection...");
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);
    size_t waitingTime = 0;
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    while (!mqttClient.connect(clientId.c_str()) && waitingTime < MQTT_TIMEOUT) {
        Serial.print(".");
        delay(1000);
        waitingTime += 1000;
    }

    if (waitingTime < MQTT_TIMEOUT) {
        Serial.println("Success.");
        String chipId = String(ESP.getChipId()); // getChipId return the last three portions of the MAC address
        String topic = COMMAND_TOPIC + chipId;
        mqttClient.subscribe(topic.c_str(), 1); // qos = 1

        Serial.print("Subscribed topic: ");
        Serial.println(topic);
        return true;
    }

    Serial.println("Failed.");
    return false;
}

// callback when receive message from subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
    uint8_t commandCode = payload[0] - '0'; // command code has 1 digit
    Serial.print("Command code: ");
    Serial.println(commandCode);
    if (payload[1] != ':') {
        Serial.println("Invalid payload, must have separator [:] right after the first digit(command code)");
        return;
    }

    switch (commandCode) {
    case RestartDevice:
        oled::displayTextCenter("Restarting device after 3 seconds");
        delay(3000);
        ESP.restart();
        break;
    case UpdateSchedule:
        schedule::updateReminderIntervalInEEPROM(payload + 2, length - 2);
        break;
    case AlertUser:
        alertUser();
        break;
    default:
        Serial.println("Unuse command code");
        break;
    }
}

void startAccessPointMode() {
    oled::displayTextCenter("Station mode failed\nStarting access point mode\nIt should take less than one minute");
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
    oled::displayTextCenter("In access point mode\nConnect to 192.168.4.1 to register your wifi");
}

void handleRoot() {
    wifi::Network* networks = wifi::readNetworksFromEEPROM();
    String index = html::generateIndex(networks);
    free(networks);
    server.send(200, "text/html", index);
}

void handleSubmit() {
    Serial.println("Handling submit request.");
    wifi::Network* networks = new wifi::Network[wifi::MAX_NETWORK];
    String* ssidInputMessage = new String[wifi::MAX_NETWORK];
    String* passwordInputMessage = new String[wifi::MAX_NETWORK];
    bool error = false;

    for (uint8_t i = 0; i < wifi::MAX_NETWORK; i++) {
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

        Serial.printf("Submitted ssid%d: ", i);
        Serial.println(ssid);
        Serial.printf("Submitted password%d: ", i);
        Serial.println(password);

        if (ssid.length() > 0 || password.length() > 0) {
            if (ssid.length() == 0 || !wifi::validateSsid(ssid)) {
                ssidInputMessage[i] = SSID_ERROR_MESSAGE;
                error = true;
            }

            if (password.length() == 0 || !wifi::validatePassword(password)) {
                passwordInputMessage[i] = PASSWORD_ERROR_MESSAGE;
                error = true;
            }
        }
    }

    if (error) {
        String html = html::generateIndex(networks, ssidInputMessage, passwordInputMessage);
        server.send(400, "text/html", html);

    } else {
        uint8_t savedNetworks = wifi::writeNetworkArrayToEEPROM(networks, wifi::MAX_NETWORK);
        String message = "Successfully save ";
        message += savedNetworks + " networks. Device will be restarted" ;

        server.send(200, "text/html", message);
        oled::displayTextCenter("Restarting device after 3 seconds");
        delay(3000);
        ESP.restart();
    }
}

void handleNotFound() {
    String html = html::generateNotFound(server.uri(), server.method());
    server.send(404, "text/plain", html);
}

void loop() {
    if (WiFi.getMode() == WIFI_STA) {
        if (WiFi.status() != WL_CONNECTED) {
            oled::displayTextCenter("Wifi loss\nTrying to reconnect");
            reconnect();
        } 
        stationAction();
    } else if (WiFi.getMode() == WIFI_AP) {
        server.handleClient();
    }
}

void reconnect() {        
    while (!wifi::connect() || !connectMqtt()) {
        // do nothing
    }
    if (reminderFlag) {
        oled::displayTextCenter("Drink!!!", 2);
    }
    displayWeight();
}

void stationAction() {
    if (reminderTimerInSecond >= reminderIntervalInSecond) {
        alertUser();
    }
    mqttClient.loop(); // check command topic

    size_t currentWeight = scale::readInt();
    Serial.print("Current weight: ");
    Serial.println(currentWeight);

    if (isWeightChange(currentWeight)) {
        checkPublishTime(currentWeight);        
        reminderFlag = false; // if there is weight change, user has noticed the alert and took action
        lastWeight = currentWeight;
    }

    displayWeight(); // display last weight reading on the oled
    reminderTimerInSecond++;
    publishTimerInSecond++;

    delay(1000); 
}

void alertUser() {
    reminderFlag = true;
    oled::displayTextCenter("Drink!!!", 2);
    reminderTimerInSecond = 0;
}

bool isWeightChange(size_t currentWeight) {
    return abs(currentWeight - lastWeight) >= WEIGHT_CHANGE_THRESHOLD;
}

void checkPublishTime(size_t currentWeight) {
    if (publishTimerInSecond >= PUBLISH_READING_INTERVAL_SECOND && WiFi.isConnected()) {
        publishReading(currentWeight);
        Serial.print("Published weight: ");
        Serial.println(currentWeight);
        publishTimerInSecond = 0;
    }
}

void publishReading(size_t data) {
    String topic = DEVICE_TOPIC + ESP.getChipId();
    mqttClient.publish(topic.c_str(), String(data).c_str());
}

void displayWeight() {
    if (reminderFlag) {
        oled::blinkScreen();
    } else {
        const char* displayData = scale::readIntAsText(lastWeight);
        oled::displayTextCenter(displayData, 2);
        free((char*)displayData);
    }    
}