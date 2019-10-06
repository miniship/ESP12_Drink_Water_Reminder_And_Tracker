#include "TickerManager.h"
#include <Ticker.h>
#include "DeviceSettings.h"
#include "DeviceController.h"
#include "StationMode.h"
#include <Arduino.h>
#include "TimeHelper.h"

const uint16_t READ_WEIGHT_INTERVAL_MS = 500;
const uint8_t PUBLISH_WEIGHT_INTERVAL_SECOND = 2;
const uint32_t ONE_DAY_IN_SECOND = 24 * 60 * 60;
const uint16_t BLINK_SCREEN_INTERVAL_MS = 1000;
const uint16_t BLINK_SCREEN_DURATION_MS = 5000;

Ticker readWeightTicker;
Ticker publishWeightTicker;
Ticker intervalAlertTicker;
Ticker scheduleAlertTicker;
Ticker blinkScreenTicker;

bool readWeightFlag = false;
bool publishWeightFlag = false;
bool alertUserFlag = false;
bool blinkScreenFlag = false;
uint16_t blinkScreenCounterMilis = 0;
uint32_t currentTimeOfDayInSecond = 0;
uint32_t alertTimeOfDayInSecond = 0;

uint32_t calculateDurationFromCurrentToAlertTimeInSecond();

void ticker::startReadWeightTicker() {
    readWeightTicker.attach_ms(READ_WEIGHT_INTERVAL_MS, [](){
        readWeightFlag = true;
    });
}

void ticker::startPublishWeightTicker() {
    publishWeightTicker.attach(PUBLISH_WEIGHT_INTERVAL_SECOND, [](){
        publishWeightFlag = true;
    });
}

void ticker::startIntervalAlertTicker() {
    intervalAlertTicker.attach(settings::getIntervalAlertInSecond(), [](){
        alertUserFlag = true;
    });
}

void ticker::startScheduleAlertTicker() {
    if (settings::getAlertTimesPerDay() == 0) {
        return;
    }

    scheduleAlertTicker.once(calculateDurationFromCurrentToAlertTimeInSecond(), [](){
        alertUserFlag = true;
        startScheduleAlertTicker();
    });
}

uint32_t calculateDurationFromCurrentToAlertTimeInSecond() {
    uint32_t* scheduleTimeList = settings::getTimeOfDayInSecondList();
    
    Serial.printf("Current alert time: %d:%d:%d", alertTimeOfDayInSecond / 3600, (alertTimeOfDayInSecond % 3600) / 60, (alertTimeOfDayInSecond % 3600) % 60);
    Serial.println();

    currentTimeOfDayInSecond = timehelper::getCurrentTimeOfDayInSecond();
    if (currentTimeOfDayInSecond < alertTimeOfDayInSecond) {
        currentTimeOfDayInSecond = alertTimeOfDayInSecond;   
    }        

    for (uint8_t i = 0; i < settings::getAlertTimesPerDay(); i++) {
        if (currentTimeOfDayInSecond < scheduleTimeList[i]) {
            alertTimeOfDayInSecond = scheduleTimeList[i];
            return alertTimeOfDayInSecond - currentTimeOfDayInSecond;
        }
    }

    alertTimeOfDayInSecond = scheduleTimeList[0];
    return ONE_DAY_IN_SECOND - (currentTimeOfDayInSecond - alertTimeOfDayInSecond);
}

uint32_t ticker::getAlertTime() {
    return alertTimeOfDayInSecond;
}

void ticker::startBlinkScreenTicker() {
    blinkScreenTicker.attach_ms(BLINK_SCREEN_INTERVAL_MS, [](){
        blinkScreenFlag = true;
    });
}

bool ticker::isReadWeightTime() {
    return readWeightFlag;
}

bool ticker::isPublishWeightTime() {
    return publishWeightFlag;
}

bool ticker::isAlertUserTime() {
    return alertUserFlag;
}

bool ticker::isBlinkScreenTime() {
    return blinkScreenFlag;
}

void ticker::changeReadWeightFlag(bool flag) {
    readWeightFlag = flag;
}

void ticker::changePublishWeightFlag(bool flag) {
    publishWeightFlag = flag;
}

void ticker::changeAlertUserFlag(bool flag) {
    alertUserFlag = flag;
}

void ticker::autoStopBlinkScreenTicker() {
    if (blinkScreenCounterMilis >= BLINK_SCREEN_DURATION_MS) {
        stopBlinkScreenTicker();
    } else {
        blinkScreenFlag = false;
        blinkScreenCounterMilis += BLINK_SCREEN_INTERVAL_MS;
    }
}

void ticker::stopReadWeightTicker() {
    readWeightTicker.detach();
    readWeightFlag = false;
}

void ticker::stopPublishWeightTicker() {
    publishWeightTicker.detach();
    publishWeightFlag = false;
}

void ticker::stopIntervalAlertTicker() {
    intervalAlertTicker.detach();
    alertUserFlag = false;
}

void ticker::stopScheduleAlertTicker() {
    scheduleAlertTicker.detach();
    alertUserFlag = false;
    currentTimeOfDayInSecond = 0;
    alertTimeOfDayInSecond = 0;
}

void ticker::stopBlinkScreenTicker() {
    blinkScreenTicker.detach();
    blinkScreenFlag = false;
    blinkScreenCounterMilis = 0;
}

void ticker::stopAllTickers() {
    stopReadWeightTicker();
    stopPublishWeightTicker();
    stopIntervalAlertTicker();
    stopScheduleAlertTicker();
    stopBlinkScreenTicker();
}