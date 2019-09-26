#include "hx711scale.h"
#include <HX711.h>

const float CALIBRATION_FACTOR = 414; // for device 13892160
const long ZERO_FACTOR = 57866;
const byte DOUT = D6;
const byte PD_SCK = D5;
const float SCALE_LIMIT = 5000;

HX711 hx711;

void scale::init() { // start as a permanent scale - does not take account of vessel weight
    hx711.begin(DOUT, PD_SCK);
    hx711.set_scale(CALIBRATION_FACTOR);
    hx711.set_offset(ZERO_FACTOR);
}

float scale::readData() {
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

void scale::tare() { // take account of vessel weight and reset scale reading to 0
    hx711.tare(ZERO_FACTOR + hx711.read_average(5));
}