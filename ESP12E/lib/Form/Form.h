#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "Network.h"

#ifndef Form_h
#define Form_h

String generateNotFoundHTMLString(String uri, HTTPMethod method);
String generateIndexHTMLString(Network* networks, uint8_t size);
String generateIndexHTMLString(Network* networks, String* ssidInputMessage, String* passwordInputMessage, uint8_t size);

#endif