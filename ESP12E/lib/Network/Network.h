#include <Arduino.h>

#ifndef Network_h
#define Network_h

struct Network {
    char ssid[31];
    char password[31];
};

/*
    validate ssid
    return:
        true if ssid is valid
        false if ssid is invalid
    parameters:
        ssid: ssid to vaidate
*/
bool validateSsid(String ssid);

/*
    validate password
    return:
        true if password is valid
        false if password is invalid
    parameters:
        password: password to vaidate
*/
bool validatePassword(String password);

/*
    write a Network to EEPROM at a specific address in format 
        ssid:password;
    return:
        bytes written to EEPROM
    parameters:
        address: start address in EEPROM
        network: Network to write  
*/
uint8_t writeNetworkToEEPROM(size_t address, Network network);

/*
    write an array of Network to EEPROM at address 0 in format 
        ssid1:password1;ssid2:password2;ssid3:password3;; 
        (extra ; to mark the end off all networks)
    return:
        number of networks written to EEPROM
    parameters:
        networks: array of Network to write
        size: size of array
*/
uint8_t writeNetworkArrayToEEPROM(Network* networks, uint8_t size); // 

/*
    write an array of Network to EEPROM at a specific address in format 
        ssid1:password1;ssid2:password2;ssid3:password3;; 
        (extra ; to mark the end off all networks)
    return:
        number of networks written to EEPROM
    parameters:
        address: start address in EEPROM
        networks: array of Network to write
        size: size of array
*/
uint8_t writeNetworkArrayToEEPROM(size_t address, Network* networks, uint8_t size);

/*
    read a number of Network from EEPROM
        if not enough Network existed, return default ones with ssid and password start with byte 0
    return:
        array (pointer) of networks read from EEPROM
    parameters:
        size: required number of Network
*/
Network* readNetworksFromEEPROM(uint8_t size);

/*
    write byte 0 to a number of first bytes in EEPROM
    parameters:
        size: required number of bytes
*/
bool clearEEPROM(size_t size);

#endif