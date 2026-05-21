#ifndef SENSOR_MAGNETICO_H
#define SENSOR_MAGNETICO_H
#include <Arduino.h>

class SensorMagnetico {
    private:
        int pin;
        int status;

    public:
        SensorMagnetico(int p);
        void init();
        int readStatus();
};
#endif