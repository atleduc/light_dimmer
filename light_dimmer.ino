#include "IRremote.hpp"
#include <EEPROM.h>

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
#define WARM_ADDRESS 0
#define COOL_ADDRESS 1

// Définition des broches
const uint8_t pinWarm = 6;  // PWM pour W- (blanc chaud)
const uint8_t pinCool = 5;  // PWM pour C- (blanc froid)
uint8_t potValue;
uint8_t coolValue = 127;  // Intensité du blanc froid
uint8_t warmValue = 127;  // Intensité du blanc chaud (inverse)
int power, ratio;
bool hasChanged = false;
unsigned long time;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  pinMode(pinWarm, OUTPUT);
  pinMode(pinCool, OUTPUT);
  coolValue = EEPROM.read(COOL_ADDRESS);
  warmValue = EEPROM.read(WARM_ADDRESS);
  analogWrite(pinWarm, warmValue);  // Ajuster l'intensité des LED blanc chaud
  analogWrite(pinCool, coolValue);  // Ajuster l'intensité des LED blanc froid
}

void loop() {
  if(hasChanged) {
    if (millis() - time > 10000) {
      // Serial.println("save to EEprom");
      EEPROM.write(WARM_ADDRESS, warmValue);
      EEPROM.write(COOL_ADDRESS, coolValue);
      hasChanged = false;
    }
  }
  if (IrReceiver.decode()) {
    uint16_t command = IrReceiver.decodedIRData.command;
    Serial.println(command);
    delay(50);  // wait a bit
    IrReceiver.resume();
    potValue = command;
    power = coolValue + warmValue;
    ratio = coolValue / power;
    time = millis();
    switch (potValue) {
      case DECREASE_K:
        coolValue++;
        warmValue = power - coolValue;
        hasChanged = true;
        break;
      case INCREASE_K:
        coolValue--;
        warmValue = power - coolValue;
        hasChanged = true;
        break;
      case DECREASE_P:
        coolValue--;
        warmValue--;
        hasChanged = true;
        break;
      case INCREASE_P:
        coolValue++;
        warmValue++;
        hasChanged = true;
        break;
      case FULL_WHITE:
        coolValue=power;
        warmValue=0;
        hasChanged = true;
        break;
      case MEDIUM_WHITE:
        coolValue=3*(power)/4;
        warmValue=power/4;
        hasChanged = true;
        break;
      case MEDIUM_YELLOW:
        coolValue=power/2;
        warmValue=power/2;
        hasChanged = true;
        break;
      case FULL_YELLOW:
        coolValue=0;
        warmValue=power;
        hasChanged = true;
        break;
      case OFF:
        coolValue=0;
        warmValue=0;
        break;
      case ON:
        coolValue=EEPROM.read(COOL_ADDRESS);
        warmValue=EEPROM.read(WARM_ADDRESS);
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