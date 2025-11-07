const int PIN1 = 2; const int PIN2 = 4; const int PIN3 = 5;
const int PIN4 = 34; const int PIN5 = 13; const int PIN6 = 14;
const int PIN7 = 15; const int PIN8 = 16; const int PIN9 = 17;
const int PIN10 = 18; const int PIN11 = 19; const int PIN12 = 21;
const int PIN13 = 22; const int PIN14 = 32; const int PIN15 = 33;

unsigned long LD1 = 0; unsigned long LD2 = 0; unsigned long LD3 = 0; unsigned long LD4 = 0; unsigned long LD5 = 0; 
unsigned long LD6 = 0; unsigned long LD7 = 0; unsigned long LD8 = 0; unsigned long LD9 = 0; unsigned long LD10 = 0; 
unsigned long LD11 = 0; unsigned long LD12 = 0; unsigned long LD13 = 0; unsigned long LD14 = 0; unsigned long LD15 = 0;

unsigned long lastStatusPrint = 0;  // for 2 s updates

void setup() {
  Serial.begin(115200);

  pinMode(PIN1, INPUT_PULLUP); pinMode(PIN2, INPUT_PULLUP); pinMode(PIN3, INPUT_PULLUP);
  pinMode(PIN4, INPUT_PULLUP); pinMode(PIN5, INPUT_PULLUP); pinMode(PIN6, INPUT_PULLUP);
  pinMode(PIN7, INPUT_PULLUP); pinMode(PIN8, INPUT_PULLUP); pinMode(PIN9, INPUT_PULLUP);
  pinMode(PIN10, INPUT_PULLUP); pinMode(PIN11, INPUT_PULLUP); pinMode(PIN12, INPUT_PULLUP);
  pinMode(PIN13, INPUT_PULLUP); pinMode(PIN14, INPUT_PULLUP); pinMode(PIN15, INPUT_PULLUP); 

  Serial.println("IR receiver status monitor started.");
}

void loop() {
  //constantly check for connection
  if (digitalRead(PIN1) == LOW){
    LD1 = millis();
  }
  if (digitalRead(PIN2) == LOW){
    LD2 = millis();
  }
  if (digitalRead(PIN3) == LOW){
    LD3 = millis();
  }
  if (digitalRead(PIN4) == LOW){
    LD4 = millis();
  }
  if (digitalRead(PIN5) == LOW){
    LD5 = millis();
  }
  if (digitalRead(PIN6) == LOW){
    LD6 = millis();
  }
  if (digitalRead(PIN7) == LOW){
    LD7 = millis();
  }
  if (digitalRead(PIN8) == LOW){
    LD8 = millis();
  }
  if (digitalRead(PIN9) == LOW){
    LD9 = millis();
  }
  if (digitalRead(PIN10) == LOW){
    LD10 = millis();
  }
  if (digitalRead(PIN11) == LOW){
    LD11 = millis();
  }
  if (digitalRead(PIN12) == LOW){
    LD12 = millis();
  }
  if (digitalRead(PIN13) == LOW){
    LD13 = millis();
  }
  if (digitalRead(PIN14) == LOW){
    LD14 = millis();
  }
  if (digitalRead(PIN15) == LOW){
    LD15 = millis();  
  }


  //Every 4 seconds, calculate and print out the current parking array status
  if (millis() - lastStatusPrint >= 4000) {
    lastStatusPrint = millis();

    //checks if these have made a connection in the last 5 seeconds
    bool LEFT1 = (millis() - LD1) < 5000;
    bool LEFT2 = (millis() - LD2) < 5000;
    bool LEFT3 = (millis() - LD3) < 5000;
    bool MIDDLE1 = (millis() - LD4) < 5000;
    bool MIDDLE2 = (millis() - LD5) < 5000;
    bool MIDDLE3 = (millis() - LD6) < 5000;
    bool RIGHT1 = (millis() - LD7) < 5000;
    bool RIGHT2 = (millis() - LD8) < 5000;
    bool RIGHT3 = (millis() - LD9) < 5000;
    bool FRONT1 = (millis() - LD10) < 5000;
    bool FRONT2 = (millis() - LD11) < 5000;
    bool FRONT3 = (millis() - LD12) < 5000;
    bool FRONT4 = (millis() - LD13) < 5000;
    bool CEN1 = (millis() - LD14) < 5000;
    bool CEN2 = (millis() - LD15) < 5000;

    //sorts all led connections into rows
    bool LEFT = LEFT1 && LEFT2 && LEFT3;
    bool MIDDLE = MIDDLE1 && MIDDLE2 && MIDDLE3; 
    bool RIGHT = RIGHT1 && RIGHT2 && RIGHT3;
    bool FRONTL = FRONT1 && FRONT2;
    bool FRONTR = FRONT3 && FRONT4;

    //validates if the parking spots are valid for raising bollards
    bool PARKINGL = LEFT && MIDDLE && FRONTL && !CEN1;
    bool PARKINGR = RIGHT && MIDDLE && FRONTR && !CEN2;

    //serial printing for debugging purposes
    Serial.print("Left: ");
    Serial.print(LEFT ? "O" : "X");
    Serial.print(" | Middle: ");
    Serial.println(MIDDLE ? "O" : "X");
    Serial.print("| Right: ");
    Serial.print(RIGHT ? "O" : "X");
    Serial.print(" | Front Left ");
    Serial.println(FRONTL ? "O" : "X");
    Serial.print("| Front Right ");
    Serial.print(FRONTR ? "O" : "X");
    Serial.print(" | Center Left ");
    Serial.println(CEN1 ? "O" : "X");
    Serial.print(" | Center Right ");
    Serial.println(CEN2 ? "O" : "X");
    Serial.print(" | Parking Space Left ");
    Serial.println(PARKINGL ? "O" : "X");
    Serial.print(" | Parking Space Right ");
    Serial.println(PARKINGR ? "O" : "X");
  }
}
