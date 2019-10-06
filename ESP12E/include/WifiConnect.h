#ifndef WifiConnect_h
#define WifiConnect_h

#include "DeviceSettings.h"

namespace wifi {
    bool connect();
    bool connect(settings::Network network);
}

#endif