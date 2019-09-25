#include "HtmlGenerator.h"

String html::generateIndex() {
    String* ssidInputMessage = new String[wifi::MAX_NETWORK];
    String* passwordInputMessage = new String[wifi::MAX_NETWORK];

    return generateIndex(wifi::getNetworkList(), ssidInputMessage, passwordInputMessage);
}

String html::generateIndex(wifi::Network* networksList, String* ssidInputMessage, String* passwordInputMessage) {
    String 
    indexHtml =  "<!DOCTYPE HTML>";  
    indexHtml += "<html>";
    indexHtml += "<head>";
    indexHtml +=    "<meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">";
    indexHtml +=    "<title>ESP8266 WebServer</title>";
    indexHtml += "</head>";
    indexHtml += "<body>";
    indexHtml +=    "<h1>Drink Water Reminder</h1>";
    indexHtml +=    "<FORM action=\"/submit\" method=\"post\">";
    indexHtml +=        "<P>";

    for (uint8_t i = 0; i < wifi::MAX_NETWORK; i++) {
    indexHtml +=        ssidInputMessage[i];
    indexHtml +=        "<br>";
    indexHtml +=        "<label>ssid:&nbsp;</label>";
    indexHtml +=        "<input size=\"30\" maxlength=\"30\" value=\"";
    indexHtml +=            networksList[i].ssid;
    indexHtml +=            "\" name=\"ssid";
    indexHtml +=            i;
    indexHtml +=        "\">";
    indexHtml +=        "<br>";
    indexHtml +=        passwordInputMessage[i];
    indexHtml +=        "<br>";
    indexHtml +=        "<label>Password:&nbsp;</label>";
    indexHtml +=        "<input size=\"30\" maxlength=\"30\" value=\"";
    indexHtml +=            networksList[i].password;
    indexHtml +=            "\" name=\"password";
    indexHtml +=            i;
    indexHtml +=        "\">";
    indexHtml +=        "<br><br>";
    }
     
    indexHtml +=        "<input type=\"submit\" value=\"Send\">";
    indexHtml +=        "<input type=\"reset\">";
    indexHtml +=        "</P>";
    indexHtml +=    "</FORM>";
    indexHtml += "</body>";
    indexHtml += "</html>";

    return indexHtml;
}

String html::generateNotFound(String uri, HTTPMethod method) {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += uri;
    message += "\nMethod: ";
    message += (method == HTTP_GET)?"GET":"POST";
    return message;
}