#ifndef hx711scale_h
#define hx711scale_h

#include <HX711.h>

namespace scale {
    void init();
    float readFloat();
    size_t readInt();
    const char* readIntAsText(size_t weightInt);
    const char* readIntAsText(size_t weightInt, const char* unit);
}

#endif