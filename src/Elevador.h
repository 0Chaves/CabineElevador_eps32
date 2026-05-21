#ifndef ELEVADOR_H
#define ELEVADOR_H
#include <Arduino.h>
#include "Sensor_Magnetico.h"
#define ULTIMO_ANDAR 3
#define TOTAL_ANDARES (ULTIMO_ANDAR + 1)

enum Andar{
  TERREO = 0,
  SEGUNDO = 1,
  TERCEIRO = 2,
  QUARTO = 3
};

enum Porta{
  FECHADA = 0,
  ABERTA = 1
};

enum Chegada{
  SIM = true,
  NAO = false
};

enum EstadoElevador{
  PARADO = 0,
  SUBINDO = 1,
  DESCENDO = 2
};

class Elevador{
  private:
    Andar andar_atual;
    Porta porta_status;
    Chegada elevador_chegada;
    bool andarDestino[TOTAL_ANDARES];
    EstadoElevador estado;
    
  public:
    SensorMagnetico sensorMagnetico;
    Elevador();
    void initSensor();
    int getPortaStatus();
    void moverElevador();
    int getAndarAtual();
    bool getElevadorChegada();
    void setAndarDestino(int andar);
    void setElevadorChegada(bool chegada);
    bool existemChamadasAbaixo();
    bool existemChamadasAcima();
};
#endif