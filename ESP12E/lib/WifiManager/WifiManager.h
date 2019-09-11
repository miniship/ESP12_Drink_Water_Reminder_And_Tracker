#ifndef WifiManager_h
#define WifiManager_h

bool connect(uint8_t maxNetwork, size_t timeout);
bool connectByWiFiMulti(uint8_t maxNetwork, size_t timeout);

#endif