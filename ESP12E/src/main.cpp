#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "ssd1306oled.h"
#include "hx711scale.h"
#include "WifiConnect.h"
#include "StationMode.h"
#include "AccessPointMode.h"
#include "Mqtt.h"
#include "DrinkSchedule.h"

const uint16_t BAUDRATE = 9600;
const uint16_t EEPROM_SIZE = 1024;
const uint8_t BLINK_SCREEN_DURATION_SECOND = 5;
const uint16_t READ_WEIGHT_INTERVAL_MS = 500;
const uint16_t PUBLISH_WEIGHT_INTERVAL_SECOND = 10;
const char* WEIGHT_UNIT = "g";
const uint8_t WEIGHT_CHANGE_THRESHOLD = 10;
uint16_t lastWeight = 9999;
uint16_t lastPublishWeight = 0;
bool isStationMode = false;

Ticker readWeightTicker;
bool readWeightFlag = false;

Ticker remindUserTicker;
bool remindUserFlag = false;
bool isRemindTimeHasSet = false;

Ticker blinkScreenTicker;
bool isBlinkScreenOn = false;
uint8_t blinkScreenCounter = 0;

Ticker publishWeightTicker;
bool publishWeightFlag = false; 

void setNextRemindTime();
uint32_t caculateDurationToNextRemindTimeInSecond();
void remindUser();
void readWeight();
bool isWeightChange(uint16_t currentWeight);
void displayWeight(uint16_t weight);
void publishWeight();

void setup() {
    // Serial.begin(BAUDRATE);
    EEPROM.begin(EEPROM_SIZE);
    delay(2000);
    oled::init();
    scale::init();
    wifi::loadNetworkList();
    sched::loadSchedule();
    
    if (station::startStationMode()) { // try connecting to stored wifi networks
        isStationMode = true;
        // check to publish weight every 5s
        publishWeightTicker.attach(PUBLISH_WEIGHT_INTERVAL_SECOND, [](){
            publishWeightFlag = true;
        });

    } else {
        accessPoint::startAccessPointMode(); // cannot connect wifi, start accesspoint mode
        isStationMode = false;
    }

    // read weight every 500ms
    readWeightTicker.attach_ms(READ_WEIGHT_INTERVAL_MS, [](){
        readWeightFlag = true;
    });
}

void loop() {
    if (remindUserFlag && !isBlinkScreenOn) { // remind user to drink by blinking the screen 
        isBlinkScreenOn = true;
        blinkScreenTicker.attach(1, remindUser);        
    }

    if (!isRemindTimeHasSet) {
        setNextRemindTime();
    }

    if (readWeightFlag) {
        readWeight();        
        readWeightFlag = false;
    }

    if (publishWeightFlag) {
        publishWeight();
        publishWeightFlag = false;
    }

    if (isStationMode) {
        mqtt::handleCommand(); // handle commands received at subscribed mqtt
    } else {
        accessPoint::handleClient();
    }
}

void setNextRemindTime() { // after reminding user, calculate duration to next remind time
    uint32_t duration = caculateDurationToNextRemindTimeInSecond();

    if (duration > 0) { // if calculated dureation = 0, stop reminding user
        // Serial.print("Duration in second to next remind time: ");
        // Serial.println(duration);
        remindUserTicker.once(duration, [](){
            remindUserFlag = true;
            isRemindTimeHasSet = false;
        });

    } 
    // else { 
    //     Serial.print("Stop reminding user");
    // }

    isRemindTimeHasSet = true;
}

uint32_t caculateDurationToNextRemindTimeInSecond() {
    if (isStationMode) {
        return station::caculateDurationToNextRemindTimeInSecond();
    }

    // in accesspoint mode, remind user at fixed interval
    return sched::getOfflineRemindIntervalInSecond();
}

void remindUser() {
    if (blinkScreenCounter++ >= BLINK_SCREEN_DURATION_SECOND) {
        blinkScreenTicker.detach();
        blinkScreenCounter = 0;
        remindUserFlag = false;
        isBlinkScreenOn = false;
    }

    if (remindUserFlag) {
        oled::blinkScreen();
    }
}

void readWeight() {
    uint16_t currentWeight = round(scale::readData());

    if (isWeightChange(currentWeight)) { // if weight change is detected
        lastWeight = currentWeight;
        remindUserFlag = false; // stop blinking screen immediately
    }

    displayWeight(lastWeight);
}

bool isWeightChange(uint16_t currentWeight) {
    return abs(currentWeight - lastWeight) >= WEIGHT_CHANGE_THRESHOLD;
}

void displayWeight(uint16_t weight) {
    char buff[8];
    itoa(weight, buff, 10);
    strcat(buff, WEIGHT_UNIT);
    oled::displayTextCenter(buff, 2, true);
}

void publishWeight() {
    if (abs(lastWeight - lastPublishWeight) >= WEIGHT_CHANGE_THRESHOLD && lastWeight >= WEIGHT_CHANGE_THRESHOLD) {
        mqtt::publishReading(lastWeight);
        lastPublishWeight = lastWeight;
    }
}