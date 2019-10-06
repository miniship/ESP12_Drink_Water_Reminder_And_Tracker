#ifndef TickerManager_h
#define TickerManager_h

#include <stdint.h>

namespace ticker {
    void startReadWeightTicker();
    void startPublishWeightTicker();
    void startIntervalAlertTicker();
    void startScheduleAlertTicker();
    uint32_t getAlertTime();
    void startBlinkScreenTicker();

    bool isReadWeightTime();
    bool isPublishWeightTime();
    bool isAlertUserTime();
    bool isBlinkScreenTime();

    void changeReadWeightFlag(bool flag);
    void changePublishWeightFlag(bool flag);
    void changeAlertUserFlag(bool flag);
    void autoStopBlinkScreenTicker();

    void stopReadWeightTicker();
    void stopPublishWeightTicker();
    void stopIntervalAlertTicker();
    void stopScheduleAlertTicker();
    void stopBlinkScreenTicker();
    void stopAllTickers();
}

#endif
