#include "HtmlGenerator.h"

String generateHeader();
String generateNetworksDiv(settings::Network* networkList, uint8_t networkCount, String* ssidInputMessage, String* passwordInputMessage);
String generateNetworkErrorMessagesDiv(String* ssidInputMessage, String* passwordInputMessage);
String generateIntervalAlertDiv(uint32_t intervalAlertInSecond, String intervalAlertInputMessage);
String generateScheduleAlertDiv(uint32_t* timeOfDayInSecondList, uint8_t alertCount, String* scheduleAlertInputMessage);
String generateScheduleAlertErrorMessagesDiv(String* scheduleAlertInputMessage);

String html::generateIndex() {
    String* ssidInputMessage = new String[settings::MAX_STORED_NETWORK];
    String* passwordInputMessage = new String[settings::MAX_STORED_NETWORK];
    String* scheduleAlertInputMessage = new String[settings::MAX_ALERT_PER_DAY];

    return generateIndex(settings::getDeviceSettings(), ssidInputMessage, passwordInputMessage, "", scheduleAlertInputMessage);
}

String html::generateIndex(settings::DeviceSettings settings, 
                            String* ssidInputMessage, 
                            String* passwordInputMessage,
                            String intervalAlertInputMessage,
                            String* scheduleAlertInputMessage) {
    String 
    indexHtml =  "<!DOCTYPE HTML>";  
    indexHtml += "<html>";
    indexHtml += generateHeader();
    indexHtml += "<body>";
    indexHtml +=    "<div align=\"center\">";
    indexHtml +=    "<h1>Drink Water Reminder Settings</h1>";
    indexHtml +=    "<h2> Device: ";
    indexHtml +=    String(ESP.getChipId());
    indexHtml +=    "</h2>";
    indexHtml +=    "<FORM action=\"/submit\" method=\"post\">";
    indexHtml +=        generateNetworksDiv(settings.networkList, settings.networkCount, ssidInputMessage, passwordInputMessage);
    indexHtml +=        "<br>";
    indexHtml +=        generateIntervalAlertDiv(settings.intervalAlertInSecond, intervalAlertInputMessage);
    indexHtml +=        "<br>";
    indexHtml +=        generateScheduleAlertDiv(settings.timeOfDayInSecondList, settings.alertTimesPerDay, scheduleAlertInputMessage);
    indexHtml +=        "<br>";
    indexHtml +=        "<div>";
	indexHtml +=            "<input type=\"submit\" value=\"Send\">";
	indexHtml +=        	"<input type=\"reset\" value=\"Reset\">";
	indexHtml +=        "</div>";
	indexHtml +=    "</FORM>";
	indexHtml +=    "</div>";
    indexHtml += "</body>";
    indexHtml += "</html>";

    return indexHtml;
}

String generateHeader() {
    String
    header =  "<head>";
	header += "<meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">";
	header += "<title>ESP8266 WebServer</title>";
	header += "<script type=\"text/javascript\">";
	header += "function addNetwork (argument) {";
	header +=   "var myTable = document.getElementById(\"networkTable\");";
	header += 	"var addNetworkButton = document.getElementById(\"addNetworkButton\");";
	header += 	"var currentIndex = myTable.rows.length;";
	header += 	"var currentRow = myTable.insertRow(-1);";

	header += 	"var ssidBox = document.createElement(\"input\");";
	header += 	"ssidBox.setAttribute(\"name\", \"ssid\" + currentIndex);";
	header += 	"ssidBox.setAttribute(\"size\", \"30\");";
	header += 	"ssidBox.setAttribute(\"maxlength\", \"30\");";

	header += 	"var passwordBox = document.createElement(\"input\");";
	header += 	"passwordBox.setAttribute(\"name\", \"password\" + currentIndex);";
	header += 	"passwordBox.setAttribute(\"size\", \"30\");";
	header += 	"passwordBox.setAttribute(\"maxlength\", \"30\");";

	header += 	"var currentCell = currentRow.insertCell(-1);";
	header += 	"currentCell.innerHTML = currentIndex;";

	header += 	"currentCell = currentRow.insertCell(-1);";
	header += 	"currentCell.appendChild(ssidBox);";

	header += 	"currentCell = currentRow.insertCell(-1);";
	header += 	"currentCell.appendChild(passwordBox);";

	header += 	"if(currentIndex >= ";
    header += 	settings::MAX_STORED_NETWORK;
    header += 	") {";
	header += 		"addNetworkButton.style.display = \"none\";";
	header += 	"}";
	header += "}";
	
	header += "function addSchedule (argument) {";
	header += 	"var myTable = document.getElementById(\"scheduleTable\");";
	header += 	"var addScheduleButton = document.getElementById(\"addScheduleButton\");";
	header += 	"var currentIndex = myTable.rows.length;";
	header += 	"var currentRow = myTable.insertRow(-1);";

	header += 	"var hourBox = document.createElement(\"input\");";
	header += 	"hourBox.setAttribute(\"name\", \"hour\" + currentIndex);";
	header += 	"hourBox.setAttribute(\"type\", \"number\");";
	header += 	"hourBox.setAttribute(\"min\", \"0\");";
    header += 	"hourBox.setAttribute(\"max\", \"23\");";

	header += 	"var minuteBox = document.createElement(\"input\");";
	header += 	"minuteBox.setAttribute(\"name\", \"minute\" + currentIndex);";
	header += 	"minuteBox.setAttribute(\"type\", \"number\");";
	header += 	"minuteBox.setAttribute(\"min\", \"0\");";
    header += 	"minuteBox.setAttribute(\"max\", \"59\");";

	header += 	"var currentCell = currentRow.insertCell(-1);";
	header += 	"currentCell.innerHTML = currentIndex;";

	header += 	"currentCell = currentRow.insertCell(-1);";
	header += 	"currentCell.appendChild(hourBox);";

	header += 	"currentCell = currentRow.insertCell(-1);";
	header += 	"currentCell.appendChild(minuteBox);";

	header += 	"if(currentIndex >= ";
    header += 	settings::MAX_ALERT_PER_DAY;
    header += 	") {";
	header += 		"addScheduleButton.style.display = \"none\";";
	header += 	"}";
	header += "}";
	header += "</script>";
    header += "</head>";

    return header;
}

String generateNetworksDiv(settings::Network* networkList, uint8_t networkCount, String* ssidInputMessage, String* passwordInputMessage) {
    String
    networksDiv =  "<div>";
    networksDiv +=     "<p>Enter your network information. Maximum ";
    networksDiv +=     settings::MAX_STORED_NETWORK;
    networksDiv +=     " items</p>";    
    networksDiv +=     generateNetworkErrorMessagesDiv(ssidInputMessage, passwordInputMessage);
    networksDiv +=     "<table id=\"networkTable\" border=\"1\" padding=\"5\">";
    networksDiv +=         "<tr>";
	networksDiv +=             "<th>No</th>";
	networksDiv +=             "<th>SSID</th>";
	networksDiv +=             "<th>Password</th>";
	networksDiv +=         "</tr>";

    if (networkCount <= 0) {
        networksDiv +=     "<tr>";
		networksDiv +=         "<td>1</td>";
		networksDiv +=         "<td><input size=\"30\" maxlength=\"30\" value=\"\" name=\"ssid1\"></td>";
		networksDiv +=         "<td><input size=\"30\" maxlength=\"30\" value=\"\" name=\"password1\"></td>";
		networksDiv +=     "</tr>";
    }

    for (uint8_t i = 0; i < networkCount; i++) {
        networksDiv +=     "<tr>";
		networksDiv +=         "<td>";
        networksDiv +=         i + 1;
        networksDiv +=         "</td>";
		networksDiv +=         "<td><input size=\"30\" maxlength=\"30\" value=\"";
        networksDiv +=         networkList[i].ssid;
        networksDiv +=         "\" name=\"ssid";
        networksDiv +=         i + 1;
        networksDiv +=         "\"></td>";
		networksDiv +=         "<td><input size=\"30\" maxlength=\"30\" value=\"";
        networksDiv +=         networkList[i].password;
        networksDiv +=         "\" name=\"password";
        networksDiv +=         i + 1;
        networksDiv +=         "\"></td>";
		networksDiv +=     "</tr>";
    }

    networksDiv +=      "</table>";
    networksDiv +=      "<br>";
    networksDiv +=      "<input id=\"addNetworkButton\" type=\"button\" class=\"button\" value=\"Add Network\" onclick=\"addNetwork();\">";
    networksDiv +=  "</div>";

    return networksDiv;
}

String generateNetworkErrorMessagesDiv(String* ssidInputMessage, String* passwordInputMessage) {
    String
    networkErrorDiv =  "<div>";

    for (uint8_t i = 0; i < settings::MAX_STORED_NETWORK; i++) {
        if (ssidInputMessage[i].length() > 0) {
            networkErrorDiv += ssidInputMessage[i];
            networkErrorDiv += "<br>";
        }
        if (passwordInputMessage[i].length() > 0) {
            networkErrorDiv += passwordInputMessage[i];
            networkErrorDiv += "<br>";
        }
    }

    networkErrorDiv += "</div>";

    return networkErrorDiv;
}

String generateIntervalAlertDiv(uint32_t intervalAlertInSecond, String intervalAlertInputMessage) {
    String
    intervalDiv =  "<div>";
    intervalDiv +=     "<p>Enter your interval alert. Valid value [";
    intervalDiv +=     settings::MIN_INTERVAL_ALERT_IN_SECOND;
    intervalDiv +=     "-";
    intervalDiv +=     settings::MAX_INTERVAL_ALERT_IN_SECOND;
    intervalDiv +=     "]</p>";
    intervalDiv +=     "<div>";
    intervalDiv +=          intervalAlertInputMessage;
    intervalDiv +=     "</div>"; 
    intervalDiv +=     "<input required type=\"number\" min=\"30\" max=\"86400\" value=\"";
    intervalDiv +=     intervalAlertInSecond;
    intervalDiv +=     "\" name=\"intervalAlert\">";
    intervalDiv +=  "</div>";

    return intervalDiv;
}

String generateScheduleAlertDiv(uint32_t* timeOfDayInSecondList, uint8_t alertCount, String* scheduleAlertInputMessage) {
    String
    scheduleDiv =  "<div>";
    scheduleDiv +=      "<p>Enter your schedule information. Maximum ";
    scheduleDiv +=      settings::MAX_ALERT_PER_DAY;
    scheduleDiv +=      " items</p>";
    scheduleDiv +=      generateScheduleAlertErrorMessagesDiv(scheduleAlertInputMessage);
    scheduleDiv +=      "<table id=\"scheduleTable\" border=\"1\" padding=\"5\">";
    scheduleDiv +=          "<tr>";
	scheduleDiv +=              "<th>No</th>";
	scheduleDiv +=              "<th>Hour</th>";
	scheduleDiv +=              "<th>Minute</th>";
	scheduleDiv +=          "</tr>";

    if (alertCount <= 0) {
        scheduleDiv +=      "<tr>";
		scheduleDiv +=          "<td>1</td>";
		scheduleDiv +=          "<td><input type=\"number\" min=\"0\" max=\"23\" value=\"\" name=\"hour1\"></td>";
		scheduleDiv +=          "<td><input type=\"number\" min=\"0\" max=\"59\" value=\"\" name=\"minute1\"></td>";
		scheduleDiv +=      "</tr>";
    }

    for (uint8_t i = 0; i < alertCount; i++) {
        scheduleDiv +=      "<tr>";
		scheduleDiv +=          "<td>";
        scheduleDiv +=          i + 1;
        scheduleDiv +=          "</td>";
		scheduleDiv +=          "<td><input type=\"number\" min=\"0\" max=\"23\" value=\"";
        scheduleDiv +=          timeOfDayInSecondList[i] / 3600;
        scheduleDiv +=          "\" name=\"hour";
        scheduleDiv +=          i + 1;
        scheduleDiv +=          "\"></td>";
		scheduleDiv +=          "<td><input type=\"number\" min=\"0\" max=\"59\" value=\"";
        scheduleDiv +=          (timeOfDayInSecondList[i] % 3600) / 60;
        scheduleDiv +=          "\" name=\"minute";
        scheduleDiv +=          i + 1;
        scheduleDiv +=          "\"></td>";
		scheduleDiv +=      "</tr>";
    }

    scheduleDiv +=      "</table>";
    scheduleDiv +=      "<br>";
    scheduleDiv +=      "<input id=\"addScheduleButton\" type=\"button\" class=\"button\" value=\"Add Schedule Alert\" onclick=\"addSchedule();\">";
    scheduleDiv +=  "</div>";

    return scheduleDiv;
}

String generateScheduleAlertErrorMessagesDiv(String* scheduleAlertInputMessage) {
    String
    scheduleErrorDiv =  "<div>";

    for (uint8_t i = 0; i < settings::MAX_ALERT_PER_DAY; i++) {
        if (scheduleAlertInputMessage[i].length() > 0) {
            scheduleErrorDiv += scheduleAlertInputMessage[i];
            scheduleErrorDiv += "<br>";
        }        
    }

    scheduleErrorDiv += "</div>";

    return scheduleErrorDiv;
}

String html::generateNotFound(String uri, HTTPMethod method) {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += uri;
    message += "\nMethod: ";
    message += (method == HTTP_GET)?"GET":"POST";
    return message;
}