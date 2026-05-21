#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Elevador.h"
#include "Button.h"
#include "Led.h"


#define MSG_SIZE 1024

const char* ssid = "ProjetoMInDS";
const char* password = "Doi39x-Wa!";

// JSON
DynamicJsonDocument doc(MSG_SIZE);
DynamicJsonDocument doc_send(MSG_SIZE);

// Broker MQTT
const char* mqtt_server = "192.168.1.114";
const int mqtt_port = 1883;
const char* pub_topic = "grupo5/elevador/estado";
const char* sub_topic = "grupo5/hall/chamada_andar";

// Prepara o cliente MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

Button buttons[TOTAL_ANDARES] = {
  Button(5),
  Button(4),
  Button(2),
  Button(15)
};
Led leds[TOTAL_ANDARES] = {
  Led(26),
  Led(27),
  Led(14),
  Led(12)
};
Elevador elevador;

void setup_wifi(){
  delay(100);
  Serial.println();
  Serial.println("Conectando-se a: " + String(ssid));

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi conectado\nEndereço de IP: " + WiFi.localIP().toString());
}

void verify_wifi(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi desconectado, reconectando...");
    setup_wifi();
  }
}

void initializeJson(){
    doc.clear();
    doc_send.clear();
}

bool serializeAndSend(const char* topic){
    char sendBuffer[MSG_SIZE];
    serializeJson(doc_send, sendBuffer);

    Serial.println("MQTT msg :");
    Serial.println(sendBuffer);

    return mqttClient.publish(topic, sendBuffer, strlen(sendBuffer));
}

// Callback: executa sempre que receber mensagem no tópico inscrito (qualquer subscribe chama este metodo)
void callback(char* topic, byte* payload, unsigned int length) {

    //Apenas mostra a mensagem recebida, sem processar. Também transforma o payload em string para facilitar a leitura.
    String message;
    Serial.print("MQTT Recv: ");
    for (int i = 0; i < length; i++) {
    message += (char)payload[i];
    }
    Serial.println(message);

    //Deserializa e processa a mensagem recebida
    doc.clear();
    deserializeJson(doc, message);
    
    if(strcmp(topic, sub_topic) == 0){
        int andarDestino = doc["andarDestino"];
        Serial.print("Andar destino recebido: ");
        Serial.println(andarDestino);

        if(andarDestino >= 0 && andarDestino <= ULTIMO_ANDAR){
            elevador.setAndarDestino(andarDestino);
        }
    }
}

void setup_mqtt(){
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}

// conecta no MQTT e se inscreve para escuta de mensagens em tópico
void reconnect_mqtt(){
  while(!mqttClient.connected()){
    Serial.println("Conectando ao broker MQTT...");
    //Talvez precise mudar este "ESP32Client" para algo mais específico para nao gerar conflitos com outros trabalhos
    if(mqttClient.connect("ESP32Client")){
      Serial.println("Conectado ao broker MQTT");
      mqttClient.subscribe(sub_topic);
    } else {
      Serial.print("Falha na conexão, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  //verify_wifi();
  //setup_mqtt();
  //initializeJson();
  for (int i = 0; i < TOTAL_ANDARES; i++) {
    buttons[i].init();
  }
  for (int i = 0; i < TOTAL_ANDARES; i++) {
    leds[i].init();
  }
  elevador.initSensor();
}

void loop() {

  verify_wifi();

  if(!mqttClient.connected()){
    reconnect_mqtt();
  }
  mqttClient.loop();

  //TODO: Verificar se o botão funciona a qualquer momento ja que esta chamada esta no começo do loop e nao dentro de uma condicional. Talvez seja necessario criar uma tarefa separada para isso.
  for (int i = 0; i < TOTAL_ANDARES; i++) {
    if (buttons[i].readStatus() == HIGH) {
      elevador.setAndarDestino(i);
    }
  }
  elevador.moverElevador();
  doc_send.clear();
  doc_send["andar_atual"] = elevador.getAndarAtual();
  doc_send["porta_status"] = elevador.getPortaStatus();
  doc_send["elevador_chegada"] = elevador.getElevadorChegada();
  if(serializeAndSend(pub_topic)){
    Serial.println("Estado do elevador enviado com sucesso!");
  } else {
    Serial.println("Falha ao enviar estado do elevador.");
  }

  for (int i = 0; i < TOTAL_ANDARES; i++) {
    if (elevador.getAndarAtual() == i) {
      leds[i].ligar();
    } else {
      leds[i].desligar();
    }
  }

  delay(1000);
}
