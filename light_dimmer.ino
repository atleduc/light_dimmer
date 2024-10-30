#include "IRremote.hpp"

#define IR_RECEIVE_PIN 2
#define DECREASE_K 12
#define INCREASE_K 16
#define FULL_WHITE 13
#define MEDIUM_WHITE 14
#define MEDIUM_YELLOW 17
#define FULL_YELLOW 18
#define OFF 6
#define ON 7
#define DECREASE_P 4
#define INCREASE_P 5

// Définition des broches
const int pinWarm = 6;  // PWM pour W- (blanc chaud)
const int pinCool = 5;  // PWM pour C- (blanc froid)
const int potPin = A0;  // Potentiomètre connecté à l'entrée analogique A0
int potValue;
int coolValue = 127;  // Intensité du blanc froid
int warmValue = 127;  // Intensité du blanc chaud (inverse)
int power, ratio;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  pinMode(pinWarm, OUTPUT);
  pinMode(pinCool, OUTPUT);
  pinMode(potPin, INPUT);
  analogWrite(3, 255);
  analogWrite(5, 255);
  analogWrite(6, 255);
}

void loop() {

  if (IrReceiver.decode()) {
    uint16_t command = IrReceiver.decodedIRData.command;
    Serial.println(command);
    delay(50);  // wait a bit
    IrReceiver.resume();
    potValue = command;
    power = coolValue + warmValue;
    ratio = coolValue / power;
      
    switch (potValue) {
      case DECREASE_K:
        coolValue++;
        warmValue = power - coolValue;
        break;
      case INCREASE_K:
        coolValue--;
        warmValue = power - coolValue;
        break;
      case DECREASE_P:
        if (coolValue > 0 && warmValue > 0) {
        coolValue--;
        warmValue--;
        }
        break;
      case INCREASE_P:
        if (coolValue < 255 && warmValue < 255) {
          coolValue++;
          warmValue++;
        }
        break;
      case FULL_WHITE:
        coolValue=power;
        warmValue=0;
        break;
      case MEDIUM_WHITE:
        coolValue=3*(power)/4;
        warmValue=power/4;
        break;
      case MEDIUM_YELLOW:
        coolValue=power/2;
        warmValue=power/2;
        break;
      case FULL_YELLOW:
        coolValue=0;
        warmValue=power;
        break;
      case OFF:
        coolValue=0;
        break;
      case ON:
        coolValue=0;
        break;
    }
    
    coolValue = constrain(coolValue, 0, 255);
    warmValue = constrain(warmValue, 0, 255);
  }

  // Écriture des valeurs PWM pour chaque couleur
  analogWrite(pinWarm, warmValue);  // Ajuster l'intensité des LED blanc chaud
  analogWrite(pinCool, coolValue);  // Ajuster l'intensité des LED blanc froid

  Serial.print(warmValue);Serial.print(",");
  Serial.println(coolValue);
}