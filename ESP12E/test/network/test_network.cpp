#include <Arduino.h>
#include <unity.h>
#include <EEPROM.h>
#include "Network.h"

const size_t eepromSize = 128;
Network n1 = {"Tuan Nguyen", "12345678"};
Network n2 = {"Phu Nguyen", "qwertyui"};
Network n3 = {"Khang Le", "abc123xyz"};
Network invalidNetwork = {"", ""};
uint8_t n1SsidLen = strlen(n1.ssid);
uint8_t n1PassLen = strlen(n1.password);
uint8_t n2SsidLen = strlen(n2.ssid);
uint8_t n2PassLen = strlen(n2.password);
uint8_t n3SsidLen = strlen(n3.ssid);
uint8_t n3PassLen = strlen(n3.password);

void test_validateSsid() {
    TEST_ASSERT_TRUE(validateSsid("Tuan Nguyen"));
    TEST_ASSERT_FALSE(validateSsid("A"));
    TEST_ASSERT_FALSE(validateSsid("012345678901234567890123456789A"));
    TEST_ASSERT_FALSE(validateSsid("Abc:123"));
    TEST_ASSERT_FALSE(validateSsid("Abc123;"));
}

void test_validatePassword() {
    TEST_ASSERT_TRUE(validatePassword("12345678"));
    TEST_ASSERT_FALSE(validatePassword("Abc1234"));
    TEST_ASSERT_FALSE(validatePassword("012345678901234567890123456789A"));
    TEST_ASSERT_FALSE(validatePassword("Abc:123"));
    TEST_ASSERT_FALSE(validatePassword("Abc123;"));
    TEST_ASSERT_FALSE(validatePassword("\'Abc123\'"));
    TEST_ASSERT_FALSE(validatePassword("\"Abc123\""));
}

void test_writeNetworkToEEPROM_success() {
    size_t address = 0;

    clearEEPROM(eepromSize);
    uint8_t result = writeNetworkToEEPROM(0, n1);

    TEST_ASSERT_EQUAL_UINT8(n1SsidLen + n1PassLen + 2, result);
    for (uint8_t i = 0; i < n1SsidLen; i++) {
        TEST_ASSERT_EQUAL_UINT8(n1.ssid[i], EEPROM.read(address++));
    }
    TEST_ASSERT_EQUAL_UINT8(':', EEPROM.read(address++));
    for (uint8_t j = 0; j < n1PassLen; j++) {
        TEST_ASSERT_EQUAL_UINT8(n1.password[j], EEPROM.read(address++));
    }
    TEST_ASSERT_EQUAL_UINT8(';', EEPROM.read(address++));
}

void test_writeNetworkToEEPROM_failed() {
    uint8_t result = writeNetworkToEEPROM(1024, n1);
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

void test_writeNetworkArrayToEEPROM_success() {
    Network networks[] = {n2, n1};
    size_t address = 10;

    clearEEPROM(eepromSize);
    uint8_t result = writeNetworkArrayToEEPROM(address, networks, 2);
    
    TEST_ASSERT_EQUAL_UINT8(2, result);
    for (uint8_t i = 0; i < n2SsidLen; i++) {
        TEST_ASSERT_EQUAL_UINT8(n2.ssid[i], EEPROM.read(address++));
    }
    TEST_ASSERT_EQUAL_UINT8(':', EEPROM.read(address++));
    for (uint8_t j = 0; j < n2PassLen; j++) {
        TEST_ASSERT_EQUAL_UINT8(n2.password[j], EEPROM.read(address++));
    }
    TEST_ASSERT_EQUAL_UINT8(';', EEPROM.read(address++));

    for (uint8_t m = 0; m < n1SsidLen; m++) {
        TEST_ASSERT_EQUAL_UINT8(n1.ssid[m], EEPROM.read(address++));
    }
    TEST_ASSERT_EQUAL_UINT8(':', EEPROM.read(address++));
    for (uint8_t n = 0; n < n1PassLen; n++) {
        TEST_ASSERT_EQUAL_UINT8(n1.password[n], EEPROM.read(address++));
    }
    TEST_ASSERT_EQUAL_UINT8(';', EEPROM.read(address++));
    TEST_ASSERT_EQUAL_UINT8(';', EEPROM.read(address++));
    TEST_ASSERT_EQUAL_UINT8(0, EEPROM.read(address++));
}

void test_writeNetworkArrayToEEPROM_hasInvalidNetwork() {
    Network networks[] = {invalidNetwork, n3};
    uint8_t address = 0;

    clearEEPROM(eepromSize);
    uint8_t result = writeNetworkArrayToEEPROM(networks, 2);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    for (uint8_t i = 0; i < n3SsidLen; i++) {
        TEST_ASSERT_EQUAL_UINT8(n3.ssid[i], EEPROM.read(address++));
    }
    TEST_ASSERT_EQUAL_UINT8(':', EEPROM.read(address++));
    for (uint8_t j = 0; j < n3PassLen; j++) {
        TEST_ASSERT_EQUAL_UINT8(n3.password[j], EEPROM.read(address++));
    }
    TEST_ASSERT_EQUAL_UINT8(';', EEPROM.read(address++));
    TEST_ASSERT_EQUAL_UINT8(';', EEPROM.read(address++));
    TEST_ASSERT_EQUAL_UINT8(0, EEPROM.read(address++));
}

void test_readNetworksFromEEPROM_hasLessNetworkThanRequired() {
    uint8_t address = 0;

    clearEEPROM(eepromSize);
    for (uint8_t i = 0; i < n1SsidLen; i++) {
        EEPROM.write(address++, n1.ssid[i]);
    }
    EEPROM.write(address++, ':');
    for (uint8_t j = 0; j < n1PassLen; j++) {
        EEPROM.write(address++, n1.password[j]);
    }
    EEPROM.write(address++, ';');
    EEPROM.write(address++, ';');
    EEPROM.commit();

    Network* networks = readNetworksFromEEPROM(3);

    for (uint8_t i = 0; i < n1SsidLen; i++) {
        TEST_ASSERT_EQUAL_UINT8(n1.ssid[i], networks[0].ssid[i]);
    }
    for (uint8_t j = 0; j < n1PassLen; j++) {
        TEST_ASSERT_EQUAL_UINT8(n1.password[j], networks[0].password[j]);
    }
    TEST_ASSERT_EQUAL_UINT8(0, networks[1].ssid[0]);
    TEST_ASSERT_EQUAL_UINT8(0, networks[1].password[0]);
    TEST_ASSERT_EQUAL_UINT8(0, networks[2].ssid[0]);
    TEST_ASSERT_EQUAL_UINT8(0, networks[2].password[0]);
    
    free(networks);
}

void test_readNetworksFromEEPROM_hasMoreNetworkThanRequired() {
    uint8_t address = 0;

    clearEEPROM(eepromSize);
    for (uint8_t i = 0; i < n1SsidLen; i++) {
        EEPROM.write(address++, n1.ssid[i]);
    }
    EEPROM.write(address++, ':');
    for (uint8_t j = 0; j < n1PassLen; j++) {
        EEPROM.write(address++, n1.password[j]);
    }
    EEPROM.write(address++, ';');

    for (uint8_t i = 0; i < n2SsidLen; i++) {
        EEPROM.write(address++, n2.ssid[i]);
    }
    EEPROM.write(address++, ':');
    for (uint8_t j = 0; j < n2PassLen; j++) {
        EEPROM.write(address++, n2.password[j]);
    }
    EEPROM.write(address++, ';');

    for (uint8_t i = 0; i < n3SsidLen; i++) {
        EEPROM.write(address++, n3.ssid[i]);
    }
    EEPROM.write(address++, ':');
    for (uint8_t j = 0; j < n3PassLen; j++) {
        EEPROM.write(address++, n3.password[j]);
    }
    EEPROM.write(address++, ';');
    EEPROM.write(address++, ';');
    EEPROM.commit();

    Network* networks = readNetworksFromEEPROM(3);

    for (uint8_t i = 0; i < n1SsidLen; i++) {
        TEST_ASSERT_EQUAL_UINT8(n1.ssid[i], networks[0].ssid[i]);
    }
    for (uint8_t j = 0; j < n1PassLen; j++) {
        TEST_ASSERT_EQUAL_UINT8(n1.password[j], networks[0].password[j]);
    }
    for (uint8_t i = 0; i < n2SsidLen; i++) {
        TEST_ASSERT_EQUAL_UINT8(n2.ssid[i], networks[1].ssid[i]);
    }
    for (uint8_t j = 0; j < n2PassLen; j++) {
        TEST_ASSERT_EQUAL_UINT8(n2.password[j], networks[1].password[j]);
    }
    for (uint8_t i = 0; i < n3SsidLen; i++) {
        TEST_ASSERT_EQUAL_UINT8(n3.ssid[i], networks[2].ssid[i]);
    }
    for (uint8_t j = 0; j < n3PassLen; j++) {
        TEST_ASSERT_EQUAL_UINT8(n3.password[j], networks[2].password[j]);
    }

    free(networks);
}

void setup() {
    UNITY_BEGIN();
    EEPROM.begin(eepromSize);
}

void loop() {
    delay(2000);
    RUN_TEST(test_validateSsid);
    RUN_TEST(test_validatePassword);
    RUN_TEST(test_writeNetworkToEEPROM_success);
    RUN_TEST(test_writeNetworkToEEPROM_failed);
    RUN_TEST(test_writeNetworkArrayToEEPROM_success);
    RUN_TEST(test_writeNetworkArrayToEEPROM_hasInvalidNetwork);
    RUN_TEST(test_readNetworksFromEEPROM_hasLessNetworkThanRequired);
    RUN_TEST(test_readNetworksFromEEPROM_hasMoreNetworkThanRequired);
    UNITY_END();
}