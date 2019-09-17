#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <HX711.h>
#include <PubSubClient.h>
#include "Form.h"
#include "WifiManager.h"
#include "Schedule.h"

const size_t BAUDRATE = 9600;
const size_t EEPROM_SIZE = 512;
const uint8_t MAX_NETWORK = 3;
const size_t CONNECT_TIMEOUT = 10000;
const size_t DEFAULT_REMINDER_INTERVAL_SECOND = 60;
const uint8_t READING_WEIGHT_INTERVAL_SECOND = 10;
const char* AP_MODE_SSID = "ESP12AP";
const char* AP_MODE_PASSWORD = "123123123";
const char* ROOT_PATH = "/";
const char* SUBMIT_PATH = "/submit";
const char* SSID_ERROR_MESSAGE = "SSID must be between 2 and 30 characters length. "
                                  "It cannot contain ; :";
const char* PASSWORD_ERROR_MESSAGE = "Password must be between 8 and 30 characters length. "
                                      "It cannot contain ; : \' \"";
const float CALIBRATION_FACTOR = 414;
// const char* MQTT_SERVER = "35.197.155.112";
const char* MQTT_SERVER = "192.168.137.1";
const size_t MQTT_PORT = 1883;
// const size_t MQTT_PORT = 4443;
String COMMAND_TOPIC = "command/";
String DEVICE_TOPIC = "device/";

size_t readingTimer = READING_WEIGHT_INTERVAL_SECOND;
size_t reminderIntervalInSecond = DEFAULT_REMINDER_INTERVAL_SECOND;
size_t reminderTimer = DEFAULT_REMINDER_INTERVAL_SECOND;

enum CommandCode{UnuseCode, RestartDevice, UpdateSchedule};

ESP8266WebServer server(80);
HX711 scale;
WiFiClient espWifiClient;
PubSubClient mqttClient(espWifiClient);

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

// callback when receive message from subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
    uint8_t commandCode = payload[0] - '0'; // command code has 1 digit
    Serial.print("Command code: ");
    Serial.println(commandCode);
    if (payload[1] != ':') {
        Serial.println("Invalid payload, must have separator [:]");
        return;
    }

    switch (commandCode) {
    case RestartDevice:
        ESP.restart();
        break;
    case UpdateSchedule:
        updateReminderIntervalInEEPROM(payload + 2, length - 2);
        break;
    default:
        Serial.println("Unuse command code");
        break;
    }

    // Switch on the LED if an 1 was received as first character
    // if ((char)payload[0] == '1') {
    //     digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is active low on the ESP-01)
    // } else {
    //     digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    // }
}

bool connectMqtt() {
    Serial.print("Attempting MQTT connection...");
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);
    size_t waitingTime = 0;
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    while (!mqttClient.connect(clientId.c_str()) && waitingTime < CONNECT_TIMEOUT) {
        Serial.print(".");
        delay(1000);
        waitingTime += 1000;
    }

    if (waitingTime < CONNECT_TIMEOUT) {
        Serial.println("Success.");
        // getChipId return the last three portions of the MAC address
        String chipId = String(ESP.getChipId());
        String topic = COMMAND_TOPIC + chipId;
        mqttClient.subscribe(topic.c_str());
        Serial.print("Subscribed topic: ");
        Serial.println(topic);
        return true;
    }

    Serial.println("Failed.");
    return false;
}

void sendWeightReading(size_t data) {
    String topic = DEVICE_TOPIC + ESP.getChipId();
    mqttClient.publish(topic.c_str(), String(data).c_str());
}

void stationAction() {
    if (reminderTimer >= reminderIntervalInSecond) {
        Serial.println("Time to drink some water!!!");
        reminderTimer = 0;
    }

    mqttClient.loop();

    if (readingTimer >= READING_WEIGHT_INTERVAL_SECOND) {
        scale.power_up();
        float data = scale.get_units();
        sendWeightReading(round(data));
        scale.power_down();
        Serial.println(data, 2);
        readingTimer = 0;
    }

    delay(1000);
    reminderTimer++;
    readingTimer++;    
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
    return connect(MAX_NETWORK, CONNECT_TIMEOUT) && connectMqtt();
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
        scale.set_scale(CALIBRATION_FACTOR);
        scale.tare();

        size_t storedReminderInterval = readReminderIntervalFromEEPROM() * 60;
        if (storedReminderInterval > 0) {
            reminderIntervalInSecond = storedReminderInterval;
            reminderTimer = storedReminderInterval;
        }
        Serial.printf("Interval: %d seconds. ", reminderIntervalInSecond);

        Serial.println("HTTP client started.");  
    } else {
        startAccessPointMode();
    }    
}

void reconnect() {
    do {
        Serial.println("Try to reconnect.");
    } while (!connect(MAX_NETWORK, CONNECT_TIMEOUT) || !connectMqtt());
}

void loop() {
    if (WiFi.getMode() == WIFI_STA) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Wifi loss.");
            reconnect();
        } 
        stationAction();
    } else if (WiFi.getMode() == WIFI_AP) {
        server.handleClient();
    }
}