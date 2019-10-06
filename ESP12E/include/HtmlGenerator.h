#ifndef HtmlGenerator_h
#define HtmlGenerator_h

#include <ESP8266WebServer.h>
#include "DeviceSettings.h"

namespace html {
    String generateIndex();
    String generateIndex(settings::DeviceSettings settings, 
                        String* ssidInputMessage, 
                        String* passwordInputMessage,
                        String intervalAlertInputMessage,
                        String* scheduleAlertInputMessage);
    String generateNotFound(String uri, HTTPMethod method);
}

#endif