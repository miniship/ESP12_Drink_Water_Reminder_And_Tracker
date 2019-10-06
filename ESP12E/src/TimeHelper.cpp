#include "TimeHelper.h"
#include <ESP8266WiFi.h>
#include <time.h>

const uint16_t START_TIMEOUT = 10000;
int timezone = 7 * 3600;
int dst = 0;

bool timehelper::start() {
    uint16_t waitingTime = 0;
    configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
    Serial.println("Waiting for Internet time...");

    while(!time(nullptr) && waitingTime < START_TIMEOUT){
        Serial.print(".");
        delay(1000);
        waitingTime += 1000;
    }

    if (waitingTime < START_TIMEOUT) {
        Serial.println("Success.");
        delay(2000);
        return true;
    }

    Serial.println("Failed.");
    return false;
}

uint32_t timehelper::getCurrentTimeOfDayInSecond() {
    time_t now = time(nullptr);
    struct tm* p_tm = localtime(&now);

    Serial.printf("Current time: %d:%d:%d", p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
    Serial.println();

    return p_tm->tm_hour * 3600 + p_tm->tm_min * 60 + p_tm->tm_sec;
}