#ifndef StationMode_h
#define StationMode_h

#include <stdint.h>

namespace station {
    bool startStationMode();
    uint32_t caculateDurationToNextRemindTimeInSecond();
}

#endif