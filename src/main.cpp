#include <Arduino.h>
#include "Button.h"

Button button_t(15);
Button button_second(2);
Button button_third(0);
Button button_fourth(4);
Button button_fifth(16);
int andar_atual = 0;
bool porta_status = false;
bool elevador_chegada = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  button_t.init();
  button_second.init();
  button_third.init();
  button_fourth.init();
  button_fifth.init();

}

void loop() {
  if(button_fifth.readStatus()){
    Serial.println("Opa opa");
  }
  delay(10); // this speeds up the simulation
}
