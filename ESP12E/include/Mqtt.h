#ifndef Mqtt_h
#define Mqtt_h

#include <stdint.h>

namespace mqtt {
    bool startMqtt();    
    void publishReading(uint16_t data);
    void publishLiftup();
    void publishPutdown();
    void handleCommand();
}

#endif