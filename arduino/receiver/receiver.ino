#include <ESP32Servo.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <SPIFFS.h>
#include <addons/TokenHelper.h>

const int pins[] = {13, 18, 19, 21, 22};

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

  pinMode(pins[0], INPUT_PULLUP);
  pinMode(pins[1], INPUT_PULLUP);
  pinMode(pins[2], INPUT_PULLUP);
  pinMode(pins[3], INPUT_PULLUP);
  pinMode(pins[4], INPUT_PULLUP);

  //Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi.");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  // Mount SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("Failed to mount SPIFFS");
    return;
  }

  // Read JSON file ---------------------------------------------------------------------------------------------------------------------------
  File file = SPIFFS.open("/espDatabaseInfo.json", "r");
  if(!file){
    Serial.println("Failed to open espDatabaseInfo.json");
    return;
  }

  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, file);
  file.close();
  if (error) {
    Serial.println("Failed to parse espDatabaseInfo.json");
    return;
  }

  config.service_account.data.project_id = doc["project_id"].as<String>().c_str();
  config.service_account.data.client_email = doc["client_email"].as<String>().c_str();
  config.service_account.data.private_key = doc["private_key"].as<String>().c_str();
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase initialized with Service Account!");

  //Serial.println(projectId);
  //Serial.println(clientEmail);
  //-------------------------------------------------------------------------------------------------------------------------------------------
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

  //Update values in database
  if (millis() - lastUpdate > 2000) { // every 2 seconds
    lastUpdate = millis();
    updateFirestoreBollard(parkingAllowed1, 1);
    updateFirestoreBollard(parkingAllowed2, 2);
  }

  //print out current status for debugging
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
    Serial.println("✅ Successfully updated Firestore!");
  } else {
    Serial.print("❌ Firestore update failed: ");
    Serial.println(fbdo.errorReason());
  }
}
