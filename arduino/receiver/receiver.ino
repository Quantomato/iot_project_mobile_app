#include <ESP32Servo.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include "credentials.h"

const int pins[] = {13, 18, 19, 21, 22};
bool LOS[] = {false, false, false, false, false};

bool parkingAllowed1 = false;
bool parkingAllowed2 = false;

unsigned long lastDetect[5] = {};
unsigned long lastStatusPrint = 0;
int count = 0;
unsigned long lastUpdate = 0;

//Wifi setup stuff
const char* ssid = "Juan's Z Fold5";
const char* password = "whatthehelly";

//Firebase stuff
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void updateFirestoreBollard(bool canRaiseBollard, int index);

void setup() {
  Serial.begin(115200);

  pinMode(pins[0], INPUT);
  pinMode(pins[1], INPUT);
  pinMode(pins[2], INPUT);
  pinMode(pins[3], INPUT);
  pinMode(pins[4], INPUT);

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

  //Serial.println(projectId);
  //Serial.println(clientEmail);
  //-------------------------------------------------------------------------------------------------------------------------------------------
}

void loop() {
  for(int i = 0; i < 5; i++){
    LOS[i] = (digitalRead(pins[i]) == HIGH);
  }

  parkingAllowed1 = (LOS[0] && LOS[1] && !LOS[3]);
  parkingAllowed2 = (LOS[0] && LOS[2] && !LOS[4]);

  //Update values in database
  if (millis() - lastUpdate > 2000) {
    updateFirestoreBollard(parkingAllowed1, 1);
    updateFirestoreBollard(parkingAllowed2, 2);
  }

  //print out current status for debugging
  if (millis() - lastStatusPrint >= 200) {
    lastStatusPrint = millis();
    count++;

    Serial.println("---- IR Status ----");
    for (int i = 0; i < 5; i++) {
      Serial.print(" Pin ["); Serial.print(pins[i]); Serial.print("] : ");
      bool seen = LOS[i];
      if(seen){
        Serial.print("✅");
      } else{
        Serial.print("❌");
      }
    }
    Serial.println("");
    Serial.print("Parking spot 1: ");
    if(parkingAllowed1){
      Serial.println("✅");
    } else{
      Serial.println("❌");
    }
    Serial.print("Parking spot 2: ");
    if(parkingAllowed2){
      Serial.println("✅");
    } else{
      Serial.println("❌");
    }
    Serial.println("\n-------------------");
  }
}

void updateFirestoreBollard(bool canRaiseBollard, int index) {
  if (!Firebase.ready()) {
    Serial.println("Firebase not ready yet!");
    return;
  }
  
  //Get document path
  String documentPath;
  if(index == 1){
    documentPath = "parkingSpaces/parkingSpace1"; 
  }else if(index == 2){
    documentPath = "parkingSpaces/parkingSpace2"; 
  }

  String updateJson = "{\"fields\": {\"canRaiseBollard\": {\"booleanValue\": ";
  updateJson += canRaiseBollard ? "true" : "false";
  updateJson += "}}}";

  Serial.println("Updating Firestore...");
  Serial.println(updateJson);

  if (Firebase.Firestore.patchDocument(&fbdo, config.service_account.data.project_id, "", documentPath.c_str(), updateJson.c_str(), "")) {
    Serial.println("Successfully updated Firestore!");
  } else {
    Serial.print("Firestore update failed: ");
    Serial.println(fbdo.errorReason());
  }
}
