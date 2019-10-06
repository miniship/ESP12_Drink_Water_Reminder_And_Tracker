#include "Mqtt.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "hx711scale.h"
#include "DeviceSettings.h"
#include "DeviceController.h"

// const char* MQTT_SERVER = "192.168.137.1";
// const char* MQTT_SERVER = "192.168.1.3";
// const uint16_t MQTT_PORT = 1883;
const char* MQTT_SERVER = "35.197.155.112";
const uint16_t MQTT_PORT = 4443;
const uint16_t MQTT_TIMEOUT = 10000;
const uint8_t REMIND_INTERVAL_PAYLOAD_MAX_LENGTH = 5;
enum CommandCode{
    UnuseCode,
    RestartDevice,
    SwitchDeviceMode,
    SwitchAlertMode
};

WiFiClient wifiCLient;
PubSubClient mqttClient(wifiCLient);
String clientId = "ESP8266Client-";
String commandTopic = "/3ml/command/";
String deviceTopic = "/3ml/device/";

void setupMqtt();
bool connectMqtt();
void callback(char* topic, byte* payload, unsigned int length);
void reconnectMqtt();

bool mqtt::startMqtt() {    
    setupMqtt();
    return connectMqtt();    
}

void setupMqtt() {
    String chipId = String(ESP.getChipId()); // getChipId return the last three portions of the MAC address
    if (clientId.indexOf(chipId) < 0) {
        clientId += chipId;
    }
    if (commandTopic.indexOf(chipId) < 0) {
        commandTopic += chipId;
    }
    if (deviceTopic.indexOf(chipId) < 0) {
        deviceTopic += chipId;
    }

    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);
}

bool connectMqtt() {
    Serial.print("Attempting MQTT connection...");
    uint16_t waitingTime = 0;    

    while (!mqttClient.connect(clientId.c_str()) && waitingTime < MQTT_TIMEOUT) {
        Serial.print(".");
        delay(1000);
        waitingTime += 1000;
    }

    if (waitingTime < MQTT_TIMEOUT) {
        Serial.println("Success.");
        mqttClient.subscribe(commandTopic.c_str(), 1); // subscribe a topic to receive command from server

        Serial.print("Subscribed topic: ");
        Serial.println(commandTopic);
        return true;
    }

    Serial.println("Failed.");
    return false;
}

// callback when receive message from subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
    uint8_t commandCode = payload[0] - '0'; // valid command code has 1 digit [0-9]
    Serial.print("Command code: ");
    Serial.println(commandCode);

    if (payload[1] != ':') {
        Serial.println("Invalid payload, must have separator [:] right after the command code");
        return;
    }

    switch (commandCode) {
        case RestartDevice:
            ESP.restart();
            break;

        case SwitchDeviceMode:
            controller::switchDeviceMode();
            break;

        case SwitchAlertMode:
            controller::switchAlertMode();
            break;

        default:
            Serial.println("Unuse command code");
            break;
    }
}

void mqtt::publishReading(uint16_t data) {
    if (mqttClient.connected()) {
        char buff[5];
        itoa(data, buff, 10);
        mqttClient.publish(deviceTopic.c_str(), buff);
    }    
}

void mqtt::handleCommand() {
    if (mqttClient.connected()) {
        mqttClient.loop();
    } else if (WiFi.status() == WL_CONNECTED) {
        reconnectMqtt();
    }
}

void reconnectMqtt() {
    mqttClient.connect(clientId.c_str());
    mqttClient.subscribe(commandTopic.c_str(), 1);
}