#include <Arduino.h>
#include <unity.h>
#include <ESP8266WebServer.h>
#include "Form.h"

void test_generateNotFoundHTMLString() {
    const char* expectedString = "File Not Found\n\nURI: /testPath\nMethod: GET";

    String html = generateNotFoundHTMLString("/testPath", HTTP_GET);
    size_t len = html.length() + 1;
    char buff[len];
    html.toCharArray(buff, len);

    TEST_ASSERT_EQUAL_STRING(expectedString, buff);
}

void test_generateIndexHTMLString_success_1emptyNetwork() {
    const char* expectedString = 
        "<!DOCTYPE HTML><html><head><meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\"><title>ESP8266 WebServer</title></head><body><h1>ESP8266 Web Form Demo</h1><FORM action=\"/submit\" method=\"post\"><P>"
        "<br><label>ssid:&nbsp;</label><input size=\"30\" maxlength=\"30\" value=\"\" name=\"ssid0\"><br><br><label>Password:&nbsp;</label><input size=\"30\" maxlength=\"30\" value=\"\" name=\"password0\"><br><br>"
        "<input type=\"submit\" value=\"Send\"><input type=\"reset\"></P></FORM></body></html>";
    
    Network networks[] = {{"", ""}};

    String html = generateIndexHTMLString(networks, 1);
    size_t len = html.length() + 1;
    char buff[len];
    html.toCharArray(buff, len);

    TEST_ASSERT_EQUAL_STRING(expectedString, buff);
}

void test_generateIndexHTMLString_success_1populatedNetwork() {
    const char* expectedString = 
        "<!DOCTYPE HTML><html><head><meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\"><title>ESP8266 WebServer</title></head><body><h1>ESP8266 Web Form Demo</h1><FORM action=\"/submit\" method=\"post\"><P>"
        "<br><label>ssid:&nbsp;</label><input size=\"30\" maxlength=\"30\" value=\"Tuan Nguyen\" name=\"ssid0\"><br><br><label>Password:&nbsp;</label><input size=\"30\" maxlength=\"30\" value=\"123123123\" name=\"password0\"><br><br>"
        "<input type=\"submit\" value=\"Send\"><input type=\"reset\"></P></FORM></body></html>";
    
    Network networks[] = {{"Tuan Nguyen", "123123123"}};

    String html = generateIndexHTMLString(networks, 1);
    size_t len = html.length() + 1;
    char buff[len];
    html.toCharArray(buff, len);

    TEST_ASSERT_EQUAL_STRING(expectedString, buff);
}

void test_generateIndexHTMLString_success_1populatedNetwork_1emptyNetwork() {
    const char* expectedString = 
        "<!DOCTYPE HTML><html><head><meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\"><title>ESP8266 WebServer</title></head><body><h1>ESP8266 Web Form Demo</h1><FORM action=\"/submit\" method=\"post\"><P>"
        "<br><label>ssid:&nbsp;</label><input size=\"30\" maxlength=\"30\" value=\"Tuan Nguyen\" name=\"ssid0\"><br><br><label>Password:&nbsp;</label><input size=\"30\" maxlength=\"30\" value=\"123123123\" name=\"password0\"><br><br>"
        "<br><label>ssid:&nbsp;</label><input size=\"30\" maxlength=\"30\" value=\"\" name=\"ssid1\"><br><br><label>Password:&nbsp;</label><input size=\"30\" maxlength=\"30\" value=\"\" name=\"password1\"><br><br>"
        "<input type=\"submit\" value=\"Send\"><input type=\"reset\"></P></FORM></body></html>";
    
    Network networks[] = {{"Tuan Nguyen", "123123123"}, {"", ""}};

    String html = generateIndexHTMLString(networks, 2);
    size_t len = html.length() + 1;
    char buff[len];
    html.toCharArray(buff, len);

    TEST_ASSERT_EQUAL_STRING(expectedString, buff);
}

void test_generateIndexHTMLString_error() {
    const char* expectedString = 
        "<!DOCTYPE HTML><html><head><meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\"><title>ESP8266 WebServer</title></head><body><h1>ESP8266 Web Form Demo</h1><FORM action=\"/submit\" method=\"post\"><P>"
        "Invalid ssid<br><label>ssid:&nbsp;</label><input size=\"30\" maxlength=\"30\" value=\"abc:123\" name=\"ssid0\"><br>Invalid password<br><label>Password:&nbsp;</label><input size=\"30\" maxlength=\"30\" value=\"pass\" name=\"password0\"><br><br>"
        "<input type=\"submit\" value=\"Send\"><input type=\"reset\"></P></FORM></body></html>";
    
    Network networks[] = {{"abc:123", "pass"}};
    String ssidInputMessage[] = {"Invalid ssid"};
    String passwordInputMessage[] = {"Invalid password"};

    String html = generateIndexHTMLString(networks, ssidInputMessage, passwordInputMessage, 1);
    size_t len = html.length() + 1;
    char buff[len];
    html.toCharArray(buff, len);

    TEST_ASSERT_EQUAL_STRING(expectedString, buff);
}

void setup() {
    UNITY_BEGIN();
}

void loop() {
    delay(2000);
    RUN_TEST(test_generateNotFoundHTMLString);
    RUN_TEST(test_generateIndexHTMLString_success_1emptyNetwork);
    RUN_TEST(test_generateIndexHTMLString_success_1populatedNetwork);
    RUN_TEST(test_generateIndexHTMLString_success_1populatedNetwork_1emptyNetwork);
    RUN_TEST(test_generateIndexHTMLString_error);
    UNITY_END();
}