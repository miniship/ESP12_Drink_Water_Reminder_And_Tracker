#include "StationMode.h"
#include <ESP8266WiFi.h>
#include "ssd1306oled.h"
#include "WifiConnect.h"
#include "Mqtt.h"
#include "TimeHelper.h"

bool station::startStationMode() {
    oled::displayTextCenter("Start station mode\nIt should take less than one minute", false);
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    delay(1000);

    return wifi::connect() && timehelper::start() && mqtt::startMqtt();
}