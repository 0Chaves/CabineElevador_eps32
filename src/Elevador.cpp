#include "Elevador.h"

Elevador::Elevador(){
    andar_atual = Andar::TERREO;
    porta_status = Porta::FECHADA;
    elevador_chegada = Chegada::NAO;
    estado = EstadoElevador::PARADO;
}