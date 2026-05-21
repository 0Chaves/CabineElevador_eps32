#include "Elevador.h"

Elevador::Elevador() : sensorMagnetico(19){
    andar_atual = Andar::TERREO;
    porta_status = Porta::ABERTA;
    elevador_chegada = Chegada::NAO;
    estado = EstadoElevador::PARADO;
    for(int i = 0; i < TOTAL_ANDARES; i++){
        andarDestino[i] = false;
    }
    pinMode(relePin, OUTPUT);
    digitalWrite(relePin, LOW);
}

void Elevador::initSensor(){
    sensorMagnetico.init();
}

int Elevador::getPortaStatus(){
    porta_status = static_cast<Porta>(sensorMagnetico.readStatus());
    return porta_status;
}

void Elevador::moverElevador(Led* leds){
    elevador_chegada = Chegada::NAO;
    getPortaStatus();
    switch(estado){
        case EstadoElevador::PARADO:{
            // Varre todos os andares procurando a primeira chamada ativa
            for (int i = 0; i <= ULTIMO_ANDAR; i++) {
                if (andarDestino[i] == true) {
                    // Define a direção inicial do elevador com base na posição atual e no destino
                    if (i > andar_atual) {
                        estado = EstadoElevador::SUBINDO;
                    } else if (i < andar_atual) {
                        estado = EstadoElevador::DESCENDO;
                    }
                    break;
                }
            }
            break;
        }
        case EstadoElevador::SUBINDO:{
            bool tem_chamada_acima = false;
            digitalWrite(relePin, LOW);
            // Varre apenas para cima
            for (int i = andar_atual + 1; i <= ULTIMO_ANDAR; i++) {
                if (andarDestino[i] == true) {
                    while(sensorMagnetico.readStatus() == HIGH){
                        delay(100);
                    }
                    for(int j = andar_atual; j < i; j++){
                        delay(1000);
                        leds[j].desligar();
                        delay(1000);
                        leds[j+1].ligar();
                    }
                    andar_atual = static_cast<Andar>(i);
                    andarDestino[i] = false;
                    elevador_chegada = Chegada::SIM;
                    while(sensorMagnetico.readStatus() == LOW){
                        delay(100);
                    }
                    porta_status = Porta::ABERTA;
                    tem_chamada_acima = true;
                    digitalWrite(relePin, HIGH);
                    break;
                }
            }
            
            // Se acabou a subida, verifica se deve descer ou parar
            if (!tem_chamada_acima) {
                if (existemChamadasAbaixo()) {
                    estado = EstadoElevador::DESCENDO;
                } else {
                    estado = EstadoElevador::PARADO;
                }
            }
            break;
        }
        case EstadoElevador::DESCENDO: {
            bool tem_chamada_abaixo = false;
            digitalWrite(relePin, LOW);
            // Varre apenas para baixo
            for (int i = andar_atual - 1; i >= 0; i--) {
                if (andarDestino[i] == true) {
                    while(sensorMagnetico.readStatus() == HIGH){
                        delay(100);
                    }
                    for(int j = andar_atual; j > i; j--){
                        delay(1000);
                        leds[j].desligar();
                        delay(1000);
                        leds[j-1].ligar();
                    }
                    andar_atual = static_cast<Andar>(i);
                    andarDestino[i] = false;
                    elevador_chegada = Chegada::SIM;
                    while(sensorMagnetico.readStatus() == LOW){
                        delay(100);
                    }
                    porta_status = Porta::ABERTA;
                    tem_chamada_abaixo = true;
                    digitalWrite(relePin, HIGH);
                    break;
                }
            }
            
            // Se acabou a descida, verifica se deve subir ou parar
            if (!tem_chamada_abaixo) {
                if (existemChamadasAcima()) {
                    estado = EstadoElevador::SUBINDO;
                } else {
                    estado = EstadoElevador::PARADO;
                }
            }
            break;
        }
    }
}

int Elevador::getAndarAtual(){
    return andar_atual;
}

bool Elevador::getElevadorChegada(){
    return elevador_chegada;
}

void Elevador::setAndarDestino(int andar){
    andarDestino[andar] = true;
}

void Elevador::setElevadorChegada(bool chegada){
    elevador_chegada = static_cast<Chegada>(chegada);
}

bool Elevador::existemChamadasAbaixo(){
    for (int i = andar_atual - 1; i >= 0; i--) {
        if (andarDestino[i] == true) {
            return true;
        }
    }
    return false;
}

bool Elevador::existemChamadasAcima(){
    for (int i = andar_atual + 1; i <= ULTIMO_ANDAR; i++) {
        if (andarDestino[i] == true) {
            return true;
        }
    }
    return false;
}