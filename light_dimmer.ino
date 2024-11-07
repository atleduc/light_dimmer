#include <IRLib.h>  
#include <avr/eeprom.h>


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
#define pinWarm 0  // PWM pour W- (blanc chaud)
#define pinCool 1  // PWM pour C- (blanc froid)
#define IR_RECEIVE_PIN 5
// adresses de stockage des données
#define WARM_ADDRESS 0
#define COOL_ADDRESS 1

IRrecv IrReceiver(IR_RECEIVE_PIN);
IRdecode Decoder; 

const int potPin = A0;  // Potentiomètre connecté à l'entrée analogique A0
int potValue;
int coolValue = 127;  // Intensité du blanc froid
int warmValue = 127;  // Intensité du blanc chaud (inverse)
int power;
bool hasChanged = false;
unsigned long time;

void setup() {
  // Serial.begin(9600);
  IrReceiver.enableIRIn();
  pinMode(pinWarm, OUTPUT);
  pinMode(pinCool, OUTPUT);
  pinMode(potPin, INPUT);
  coolValue=eeprom_read_byte( (uint8_t*) COOL_ADDRESS );
  warmValue=eeprom_read_byte( (uint8_t*) WARM_ADDRESS);
  analogWrite(pinCool, coolValue);
  analogWrite(pinWarm, warmValue);
  analogWrite(6, 255);
}

void loop() {
   if(hasChanged) {
    if (millis() - time > 10000) {
      // Serial.println("save to EEprom");
      eeprom_write_byte( (uint8_t*) WARM_ADDRESS, warmValue );
      eeprom_write_byte( (uint8_t*) COOL_ADDRESS, coolValue );
      hasChanged = false;
    }
  }
  if (IrReceiver.GetResults(&Decoder)) {

    Decoder.decode();
    uint16_t command = Decoder.value;
    //Serial.println(command);
    time = millis();
    IrReceiver.resume();
    potValue = command;
    power = coolValue + warmValue;  
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
        break;
      case ON:
        coolValue=eeprom_read_byte( (uint8_t*) COOL_ADDRESS );
        warmValue=eeprom_read_byte( (uint8_t*) WARM_ADDRESS);
        break;
    }
    
    coolValue = constrain(coolValue, 0, 255);
    warmValue = constrain(warmValue, 0, 255);
  }

  // Écriture des valeurs PWM pour chaque couleur
  analogWrite(pinWarm, warmValue);  // Ajuster l'intensité des LED blanc chaud
  analogWrite(pinCool, coolValue);  // Ajuster l'intensité des LED blanc froid

  // Serial.print(warmValue);Serial.print(",");
  // Serial.println(coolValue);
}