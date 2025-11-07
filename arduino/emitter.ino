#include <ESP32Servo.h>

const int emitters[] = {13, 14, 25, 26, 27};

#define servo1Pin 25
#define servo2Pin 26

int lowered = 10;
int raised = 120;

Servo servo1;
Servo servo2;

bool bollard1Raised = false;
bool bollard2Raised = false;

bool parkingAllowed1 = false;
bool parkingAllowed2 = false;

void setup() {
  Serial.begin(115200);

  //set up emitter frequency for proper reading later
  for (int i = 0; i < 5; i++) {
    analogWriteFrequency(emitters[i], 38000);
    analogWriteResolution(emitters[i], 8);
    pinMode(emitters[i], OUTPUT);
    analogWrite(emitters[i], 0);
  }

  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
}

void sendIRBurst(int pin, int bursts) {
  for (int i = 0; i < bursts; i++) {
    analogWrite(pin, 170);
    delayMicroseconds(600);
    analogWrite(pin, 0);
    delayMicroseconds(600);
  }
}

void loop() {
  Serial.println("bursting");

  for (int i = 0; i < 5; i++) {
    sendIRBurst(emitters[i], 25);
    delay(50);
  }

  delay(500);

  if(bollard1Raised && parkingAllowed1){
    servo1.write(raised);
  } else{
    servo1.write(lowered);
  }

  if(bollard2Raised && parkingAllowed2){
    servo2.write(raised);
  } else{
    servo2.write(lowered);
  }
}
