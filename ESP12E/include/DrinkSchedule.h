#ifndef DrinkSchedule_h
#define DrinkSchedule_h

#include <stdint.h>

namespace sched {
    struct RemindTime {
        uint8_t hour;
        uint8_t minute;
        bool operator < (const RemindTime& remindTime) const {
            if (hour != remindTime.hour) {
                return hour < remindTime.hour; 
            } else {
                return minute < remindTime.minute;
            }            
        }
    };

    const uint8_t MAX_REMIND_PER_DAY = 24;

    void loadSchedule();
    uint8_t getRemindTimeCount();
    RemindTime* getRemindTimeList();
    uint16_t getOfflineRemindIntervalInSecond();
    bool updateRemindTimeList(uint8_t newRemindTimeCount);
    bool updateOfflineRemindIntervalInSecond(uint16_t newPeriod);
}

#endif