#include <ESP32Servo.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include "credentials.h"

#define servo1Pin 18
#define servo2Pin 19

int low1 = 30;
int high1 = 8;

int low2 = 8;
int high2 = 30;

Servo servo1;
Servo servo2;

bool bollard1Raised = false;
bool bollard2Raised = false;

bool parkingAllowed1 = false;
bool parkingAllowed2 = false;

//Wifi setup stuff
const char* ssid = "Juan's Z Fold5";
const char* password = "whatthehelly";

//Firebase stuff
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void readBollardData(String path, bool &bollard, bool &parking);

void setup() {
  Serial.begin(115200);

  //Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi.");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  
  config.api_key = PROJECT_API_KEY;
  config.service_account.data.project_id = PROJECT_ID;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (auth.token.uid.length() > 0) {
    Serial.println("Successfully authenticated!");
    Serial.print("UID: ");
    Serial.println(auth.token.uid.c_str());
  } else {
    Serial.println("FAILED to authenticate — UID is empty.");
  }

  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo1.write(low1);
  servo2.write(low2);
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
  //Get parkingAllowed and bollardRaised variables from database
  readBollardData("parkingSpaces/parkingSpace1", bollard1Raised, parkingAllowed1);
  readBollardData("parkingSpaces/parkingSpace2", bollard2Raised, parkingAllowed2);

  Serial.print("Bollard1Raised ");
  Serial.println(bollard1Raised);

  delay(500);

  if(bollard1Raised && parkingAllowed1){
    servo1.write(high1);
  } else{
    servo1.write(low1);
  }

  if(bollard2Raised && parkingAllowed2){
    servo2.write(high2);
  } else{
    servo2.write(low2);
  }
}

void readBollardData(String path, bool &bollard, bool &parking) {

    if (!Firebase.Firestore.getDocument(&fbdo, PROJECT_ID, "(default)", path.c_str())) {
        Serial.print("Failed to read document: ");
        Serial.println(fbdo.errorReason());
        Serial.print("HTTP: ");
        Serial.println(fbdo.httpCode());
        Serial.print("PAYLOAD: ");
        Serial.println(fbdo.payload());
        return;
    }

    String payload = fbdo.payload();

    if (payload.length() == 0) {
        Serial.println("Empty payload!");
        return;
    }

    Serial.print("Raw Payload: ");
    Serial.println(payload);

    // Extract boolean values from payload manually
    int idx1 = payload.indexOf("\"bollardsRaised\"");
    if (idx1 != -1) {
        int valPos = payload.indexOf("booleanValue", idx1);
        if (valPos != -1) {
            String sub = payload.substring(valPos, valPos + 40);
            bollard = sub.indexOf("true") != -1;
            Serial.print("bollardsRaised = ");
            Serial.println(bollard);
        }
    } else {
        Serial.println("bollardsRaised not found");
    }

    int idx2 = payload.indexOf("\"carIsParked\"");
    if (idx2 != -1) {
        int valPos = payload.indexOf("booleanValue", idx2);
        if (valPos != -1) {
            String sub = payload.substring(valPos, valPos + 40);
            parking = sub.indexOf("true") != -1;
            Serial.print("carIsParked = ");
            Serial.println(parking);
        }
    } else {
        Serial.println("carIsParked not found");
    }
}




