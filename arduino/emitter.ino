#include <ESP32Servo.h>

const int emitterPin = 23;
#define LED2_PIN 26
#define SERVO1_PIN 32
#define SERVO2_PIN 33

bool servo1Open = false;
bool servo2Open = false;

int servo1Low = 0;
int servo1High = 90;
int servo2Low = 0;
int servo2High = 90;

Servo servo1;
Servo servo2;

void setup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  analogWriteFrequency(emitterPin, 38000);
  Serial.begin(115200);
}

void loop() {
  Serial.println("Emitting burst...");
  unsigned long start = millis();
  //Burst LEDS for 38hz so the receivers can properly see them
  while (millis() - start < 200) {
    analogWrite(emitterPin, 170);
    delay(2);
    analogWrite(emitterPin, 0);
    delay(2);
  }
  analogWrite(emitterPin, 0);
  //delay 5 seconds between each burst 
  delay(5000);

  //open or close servo 1 bollards
  if(servo1Open){
    servo1.write(servo1Low);
  } else{
    servo1.write(servo1High);
  }

  //open or close servo 2 bollards
  if(servo2Open){
    servo2.write(servo2Low);
  } else{
    servo2.write(servo2High);
  }
}
  
