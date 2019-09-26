#include "Mqtt.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DrinkSchedule.h"
#include "hx711scale.h"

// const char* MQTT_SERVER = "192.168.137.1";
// const uint16_t MQTT_PORT = 1883;
const char* MQTT_SERVER = "35.197.155.112";
const uint16_t MQTT_PORT = 4443;
const uint16_t MQTT_TIMEOUT = 10000;
const uint8_t REMIND_INTERVAL_PAYLOAD_MAX_LENGTH = 5;
enum CommandCode{
    UnuseCode,
    RestartDevice,
    UpdateSchedule,
    UpdateOfflineRemindInterval,
    Tare
};

WiFiClient wifiCLient;
PubSubClient mqttClient(wifiCLient);
String clientId = "ESP8266Client-";
String commandTopic = "command/";
String deviceTopic = "device/";

void setupMqtt();
bool connectMqtt();
void callback(char* topic, byte* payload, unsigned int length);
uint8_t extractRemindTimeListFromPayload(byte* payload, unsigned int length);
bool validateRemindTime(uint8_t hour, uint8_t minute);
uint16_t extractRemindIntervalFromPayload(byte* payload, unsigned int length);
void reconnectMqtt();

bool mqtt::startMqtt() {    
    setupMqtt();
    return connectMqtt();    
}

void setupMqtt() {
    String chipId = String(ESP.getChipId()); // getChipId return the last three portions of the MAC address
    clientId += chipId;
    commandTopic += chipId;
    deviceTopic += chipId;

    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);
}

bool connectMqtt() {
    // Serial.print("Attempting MQTT connection...");
    uint16_t waitingTime = 0;    

    while (!mqttClient.connect(clientId.c_str()) && waitingTime < MQTT_TIMEOUT) {
        // Serial.print(".");
        delay(1000);
        waitingTime += 1000;
    }

    if (waitingTime < MQTT_TIMEOUT) {
        // Serial.println("Success.");
        mqttClient.subscribe(commandTopic.c_str(), 1); // subscribe a topic to receive command from server

        // Serial.print("Subscribed topic: ");
        // Serial.println(commandTopic);
        return true;
    }

    // Serial.println("Failed.");
    return false;
}

// callback when receive message from subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
    uint8_t commandCode = payload[0] - '0'; // valid command code has 1 digit [0-9]
    // Serial.print("Command code: ");
    // Serial.println(commandCode);

    if (payload[1] != ':') {
        // Serial.println("Invalid payload, must have separator [:] right after the command code");
        return;
    }

    switch (commandCode) {
        case RestartDevice:
            ESP.restart();
            break;

        case UpdateSchedule: {
            uint8_t extractedRemindTimeCount = extractRemindTimeListFromPayload(payload + 2, length - 2);
            if (extractedRemindTimeCount > 0) {
                sched::updateRemindTimeList(extractedRemindTimeCount);
            }           
            break;
        }            

        case UpdateOfflineRemindInterval:
            sched::updateOfflineRemindIntervalInSecond(extractRemindIntervalFromPayload(payload + 2, length - 2));
            break;
        
        case Tare:
            scale::tare();
            break;

        default:
            // Serial.println("Unuse command code");
            break;
    }
}

uint8_t extractRemindTimeListFromPayload(byte* payload, unsigned int length) {
    sched::RemindTime* remindTimeList = sched::getRemindTimeList();
    uint8_t remindTimeCount = 0;
    uint8_t hour = 0;
    uint8_t minute = 0;
    bool isHour = true;

    for (uint8_t i = 0; i < length; i++) {
        char curChar = payload[i];

        if (curChar < '0' && curChar > ';') {
            // Serial.print("Remind time list payload contains invalid character: ");
            // Serial.println(char(curChar));
            return 0;
        }

        if (curChar == ':') {
            isHour = false;
            continue;
        }
        
        if (curChar == ';') {
            if (validateRemindTime(hour, minute)) {
                remindTimeList[remindTimeCount++] = sched::RemindTime{hour, minute};
                // Serial.printf("%d:%d;", hour, minute);
            }
            if (remindTimeCount >= sched::MAX_REMIND_PER_DAY) {
                return remindTimeCount;
            }
            hour = 0;
            minute = 0;
            isHour = true;
            continue;
        }
        
        if (isHour) {
            hour = hour * 10 + curChar - '0';
        } else {
            minute = minute * 10 + curChar - '0';
        }
    }

    // Serial.println();
    return remindTimeCount;
}

bool validateRemindTime(uint8_t hour, uint8_t minute) {
    return hour >= 0 && hour <=23 && minute >=0 && minute <= 59;
}

uint16_t extractRemindIntervalFromPayload(byte* payload, unsigned int length) {
    if (length > REMIND_INTERVAL_PAYLOAD_MAX_LENGTH) {
        // Serial.println("Remind interval payload exceeds maximum value");
        return 0;
    }

    if (payload[length - 1] != ';') {
        // Serial.println("Remind interval payload must end with ';'");
        return 0;
    }

    uint16_t interval = 0;
    for (uint8_t i = 0; i < length; i++) {
        char curChar = payload[i];
        if (curChar == ';') {
            break;
        }

        if (curChar < '0' || curChar > '9') {
            // Serial.println("Remind interval must be an integer");
            return 0;
        }

        interval = interval * 10 + curChar - '0';
    }

    // Serial.print("Received new interval: ");
    // Serial.println(interval);
    return interval;
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