#include <ESP32Servo.h>

const int pins[] = {13, 18, 19, 21, 22};

bool parkingAllowed1 = false;
bool parkingAllowed2 = false;

unsigned long lastDetect[5] = {};
unsigned long lastStatusPrint = 0;
int count = 0;

void setup() {
  Serial.begin(115200);

  pinMode(pins[0], INPUT_PULLUP);
  pinMode(pins[1], INPUT_PULLUP);
  pinMode(pins[2], INPUT_PULLUP);
  pinMode(pins[3], INPUT_PULLUP);
  pinMode(pins[4], INPUT_PULLUP);
}

void loop() {
  //if low, then pins line of sight is not broken, aka currently detected LOS
  for (int i = 0; i < 5; i++) {
    if (digitalRead(pins[i]) == LOW) {
      lastDetect[i] = millis();
    }
  }

  bool LOS1 = (digitalRead(pins[0]) == LOW);
  bool LOS2 = (digitalRead(pins[1]) == LOW);
  bool LOS3 = (digitalRead(pins[2]) == LOW);
  bool LOS4 = (digitalRead(pins[3]) == LOW);
  bool LOS5 = (digitalRead(pins[4]) == LOW);

  parkingAllowed1 = (LOS1 && LOS2 && !LOS4);
  parkingAllowed2 = (LOS2 && LOS4 && !LOS5);

  //pring out current status for debugging
  if (millis() - lastStatusPrint >= 1000) {
    lastStatusPrint = millis();
    count++;

    Serial.println("---- IR Status ----");
    for (int i = 0; i < 5; i++) {
      Serial.print(" Pin ["); Serial.print(pins[i]); Serial.print("] : ");
      bool seen = (millis() - lastDetect[i]) < 2000;
      if(seen){
        Serial.print("✅");
      } else{
        Serial.print("❌");
      }
    }
    Serial.println("\n-------------------");

    if (count > 4) {
      count = 0;
      Serial.println("\nSANITY CHECK\n");
    }
  }
}
