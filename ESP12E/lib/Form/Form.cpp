#include "Form.h"

String generateNotFoundHTMLString(String uri, HTTPMethod method) {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += uri;
    message += "\nMethod: ";
    message += (method == HTTP_GET)?"GET":"POST";
    return message;
}

String generateIndexHTMLString(Network* networks, uint8_t size) {
    String* ssidInputMessage = new String[size];
    String* passwordInputMessage = new String[size];

    for (uint8_t i = 0; i < size; i++) {
        ssidInputMessage[i] = "";
        passwordInputMessage[i] = "";
    }

    return generateIndexHTMLString(networks, ssidInputMessage, passwordInputMessage, size);
}

String generateIndexHTMLString(Network* networks, String* ssidInputMessage, String* passwordInputMessage, uint8_t size) {
    String 
    indexHtml =  "<!DOCTYPE HTML>";  
    indexHtml += "<html>";
    indexHtml += "<head>";
    indexHtml +=    "<meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">";
    indexHtml +=    "<title>ESP8266 WebServer</title>";
    indexHtml += "</head>";
    indexHtml += "<body>";
    indexHtml +=    "<h1>ESP8266 Web Form Demo</h1>";
    indexHtml +=    "<FORM action=\"/submit\" method=\"post\">";
    indexHtml +=        "<P>";

    for (uint8_t i = 0; i < size; i++) {
        String ssid = "";
        String password = "";
        if (networks[i].ssid[0] != 0) {
            ssid = networks[i].ssid;
        }
        if (networks[i].password[0] != 0) {
            password = networks[i].password;
        }

    indexHtml +=        ssidInputMessage[i];
    indexHtml +=        "<br>";
    indexHtml +=        "<label>ssid:&nbsp;</label>";
    indexHtml +=        "<input size=\"30\" maxlength=\"30\" value=\"";
    indexHtml +=            ssid;
    indexHtml +=            "\" name=\"ssid";
    indexHtml +=            i;
    indexHtml +=        "\">";
    indexHtml +=        "<br>";
    indexHtml +=        passwordInputMessage[i];
    indexHtml +=        "<br>";
    indexHtml +=        "<label>Password:&nbsp;</label>";
    indexHtml +=        "<input size=\"30\" maxlength=\"30\" value=\"";
    indexHtml +=            password;
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