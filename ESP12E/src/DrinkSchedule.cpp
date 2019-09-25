#include "DrinkSchedule.h"
#include <EEPROM.h>
#include <algorithm>

const uint16_t MAX_OFFLINE_REMIND_INTERVAL_IN_SECOND = 9999;
const uint16_t EEPROM_SCHEDULE_ADDRESS = 200;
const uint16_t EEPROM_OFFLINE_REMIND_INTERVAL_ADDRESS = 1000;
sched::RemindTime remindTimeList[sched::MAX_REMIND_PER_DAY];
uint8_t remindTimeCount = 0;
uint16_t offlineRemindIntervalInSecond = 0;

void sched::loadSchedule() {
    EEPROM.get(EEPROM_SCHEDULE_ADDRESS, remindTimeCount);
    EEPROM.get(EEPROM_SCHEDULE_ADDRESS + sizeof(uint8_t), remindTimeList);
    EEPROM.get(EEPROM_OFFLINE_REMIND_INTERVAL_ADDRESS, offlineRemindIntervalInSecond);
}

uint8_t sched::getRemindTimeCount() {
    return remindTimeCount < MAX_REMIND_PER_DAY ? remindTimeCount : MAX_REMIND_PER_DAY;
}

sched::RemindTime* sched::getRemindTimeList() {
    return remindTimeList;
}

uint16_t sched::getOfflineRemindIntervalInSecond() {
    return offlineRemindIntervalInSecond < MAX_OFFLINE_REMIND_INTERVAL_IN_SECOND ? offlineRemindIntervalInSecond : MAX_OFFLINE_REMIND_INTERVAL_IN_SECOND;
}

bool sched::updateRemindTimeList(uint8_t newRemindTimeCount) {
    remindTimeCount = newRemindTimeCount < MAX_REMIND_PER_DAY ? newRemindTimeCount : MAX_REMIND_PER_DAY;
    std::sort(remindTimeList, remindTimeList + remindTimeCount);
    EEPROM.put(EEPROM_SCHEDULE_ADDRESS, remindTimeCount);
    EEPROM.put(EEPROM_SCHEDULE_ADDRESS + sizeof(uint8_t), remindTimeList);
    return EEPROM.commit();
}

bool sched::updateOfflineRemindIntervalInSecond(uint16_t newPeriod) {
    offlineRemindIntervalInSecond = newPeriod < MAX_OFFLINE_REMIND_INTERVAL_IN_SECOND ? newPeriod : MAX_OFFLINE_REMIND_INTERVAL_IN_SECOND;
    EEPROM.put(EEPROM_OFFLINE_REMIND_INTERVAL_ADDRESS, offlineRemindIntervalInSecond);
    return EEPROM.commit();
}