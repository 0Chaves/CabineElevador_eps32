#include "Sensor_Magnetico.h"

SensorMagnetico::SensorMagnetico(int p){
    pin = p;
    status = HIGH; //Sem sinal magnetico, ou seja, porta aberta
}

void SensorMagnetico::init(){
    pinMode(pin, INPUT);
    status = digitalRead(pin);
}

int SensorMagnetico::readStatus(){
    status = digitalRead(pin);
    return status;
}