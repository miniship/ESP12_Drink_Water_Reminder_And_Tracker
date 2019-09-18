#include "hx711scale.h"

const float CALIBRATION_FACTOR = 414;
const byte DOUT = D6;
const byte PD_SCK = D5;
const size_t SCALE_LIMIT = 5000;
const char* DEFAULT_UNIT = "g";

HX711 hx711;

void scale::init() {
    hx711.begin(DOUT, PD_SCK);
    hx711.set_scale(CALIBRATION_FACTOR);
    hx711.tare();
}

float scale::readFloat() {
    hx711.power_up();
    float data = hx711.get_units();
    hx711.power_down();

    if (data < 0) {
        return 0;
    }
    if (data > SCALE_LIMIT) {
        return SCALE_LIMIT;
    }

    return data;
}

size_t scale::readInt() {
    float data = readFloat();    
    return round(data);
}

const char* scale::readIntAsText(size_t weightInt) {
    return readIntAsText(weightInt, DEFAULT_UNIT);
}

const char* scale::readIntAsText(size_t weightInt, const char* unit) {
    char buff[4];
    uint8_t index = 0;
    while (weightInt > 0) {
        buff[index++] = '0' + weightInt % 10;
        weightInt /= 10;
    }

    uint8_t unitLen = strlen(unit);
    char* result = (char*)calloc(index + unitLen + 1, sizeof(char));
    for (uint8_t i = 0; i < index; i++) {
        result[i] = buff[index - i - 1];
    }

    if (index == 0) {
        result[index++] = '0';
    }

    for (uint8_t i = 0; i < unitLen; i++) {
        result[index++] = unit[i];
    }

    result[index] = '\0';
    return result;
}