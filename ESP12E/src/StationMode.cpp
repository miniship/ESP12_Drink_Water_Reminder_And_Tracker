#include "StationMode.h"
#include "ssd1306oled.h"
#include <ESP8266WiFi.h>
#include "WifiConnect.h"
#include "Mqtt.h"
#include "DrinkSchedule.h"
#include <ESP8266HTTPClient.h>

// const String GET_CURRENT_TIME_LINK = "http://192.168.137.1:8080/getCurrentTime";
const String GET_CURRENT_TIME_LINK = "https://drink-water-reminder-1.appspot.com:8080/getCurrentTime";

sched::RemindTime currentTime {
    hour:24,
    minute:60
};

void updateCurrentTime();
uint8_t parseHour(const char* hourString);
uint8_t parseMinute(const char* sminuteString);
uint32_t caculateDurationInSecond(sched::RemindTime nextRemindTime, bool isDayEnd);

bool station::startStationMode() {
    oled::displayTextCenter("Start station mode\nIt should take less than one minute", false);
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    delay(1000);
    return wifi::connect() && mqtt::startMqtt();
}

uint32_t station::caculateDurationToNextRemindTimeInSecond() {
    if (WiFi.status() == WL_CONNECTED ) {
        updateCurrentTime();
    }
    // Serial.printf("Current time: %d:%d", currentTime.hour, currentTime.minute);
    // Serial.println();

    uint8_t remindTimeCount = sched::getRemindTimeCount();
    if (currentTime.hour > 23 || currentTime.minute > 59 || remindTimeCount < 1) {
        // Serial.println("Cannot initialize current time");
        return sched::getOfflineRemindIntervalInSecond();
    }

    sched::RemindTime* remindTimeList = sched::getRemindTimeList(); // remind time list is sorted ascendingly

    for (uint8_t i = 0; i < remindTimeCount; i++) {
        sched::RemindTime remindTime = remindTimeList[i];

        if (currentTime < remindTime) {
            return caculateDurationInSecond(remindTime, false);
        }
    }

    return caculateDurationInSecond(remindTimeList[0], true);
}

void updateCurrentTime() {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, GET_CURRENT_TIME_LINK);

    if (http.GET() == HTTP_CODE_OK) {
        String payload = http.getString();
        // Serial.print("Get current time payload: ");
        // Serial.println(payload);

        uint8_t index = payload.indexOf(':');
        uint8_t hour = parseHour(payload.substring(0, index).c_str());
        uint8_t minute = parseMinute(payload.substring(index + 1, payload.length()).c_str());

        if (hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59) {
            currentTime.hour = hour;
            currentTime.minute = minute;
        }            
    }

    http.end();
    client.stop();
}

uint8_t parseHour(const char* hourString) {
    uint8_t hour = 0;

    for (uint8_t i = 0; i < strlen(hourString); i++) {
        char c = hourString[i];
        if (c < '0' || c > '9') {
            return 24;
        }

        hour = hour * 10 + c - '0';
        if (hour > 23) {
            return 24;
        }
    }
    return hour;
}

uint8_t parseMinute(const char* minuteString) {
    uint8_t minute = 0;

    for (uint8_t i = 0; i < strlen(minuteString); i++) {        
        char c = minuteString[i];
        if (c < '0' || c > '9') {
            return 60;
        }

        minute = minute * 10 + c - '0';
        if (minute > 59) {
            return 60;
        }
    }
    return minute;
}

uint32_t caculateDurationInSecond(sched::RemindTime nextRemindTime, bool isDayEnd) {
    uint32_t timeDiffInMinute = (nextRemindTime.hour - currentTime.hour) * 60 + nextRemindTime.minute - currentTime.minute;
    
    if (isDayEnd) {
        timeDiffInMinute += 24 * 60; 
    }

    currentTime.hour = nextRemindTime.hour;
    currentTime.minute = nextRemindTime.minute;
    return timeDiffInMinute * 60;
}