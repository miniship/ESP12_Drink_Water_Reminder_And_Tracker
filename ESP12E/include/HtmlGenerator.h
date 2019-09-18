#ifndef HtmlGenerator_h
#define HtmlGenerator_h

#include <ESP8266WebServer.h>
#include "WifiConnect.h"

namespace html {
    String generateIndex(wifi::Network* networks);
    String generateIndex(wifi::Network* networks, String* ssidInputMessage, String* passwordInputMessage);
    String generateNotFound(String uri, HTTPMethod method);   
}

#endif