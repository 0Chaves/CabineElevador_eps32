#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Elevador.h"
#include "Button.h"

const char* ssid = "nome-da-rede";
const char* password = "senha-da-rede";

WiFiClient client;
PubSubClient mqttClient(client);
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


Button button_t(15);
Button button_second(2);
Button button_third(0);
Button button_fourth(4);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  button_t.init();
  button_second.init();
  button_third.init();
  button_fourth.init();

}

void loop() {
  delay(1000); // this speeds up the simulation
}
