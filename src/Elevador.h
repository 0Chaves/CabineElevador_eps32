#ifndef ELEVADOR_H
#define ELEVADOR_H
#include <Arduino.h>
#define ULTIMO_ANDAR 4
#define TOTAL_ANDARES (ULTIMO_ANDAR + 1)

enum class Andar{
  TERREO = 0,
  SEGUNDO = 1,
  TERCEIRO = 2,
  QUARTO = 3,
  QUINTO = 4
};

enum class Porta{
  ABERTA = true,
  FECHADA = false
};

enum class Chegada{
  SIM = true,
  NAO = false
};

enum class EstadoElevador{
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
    Elevador();
    void abrirPorta();
    void fecharPorta();
    void moverElevador();
    int getAndarAtual();
    bool getPortaStatus();
    bool getElevadorChegada();
};
#endif