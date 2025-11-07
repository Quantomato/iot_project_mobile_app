#include <ESP32Servo.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <ArduinoJson.h>

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

const char* project_id = "bollardbros-1afdb";
const char* client_email = "firebase-adminsdk-fbsvc@bollardbros-1afdb.iam.gserviceaccount.com";
const char* private_key = "-----BEGIN PRIVATE KEY-----\n"MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQC8VhBK8HLAEbCg\nvHQxE9vV3n3WXpouMR03/20yXL6cIWdZfRyULQXofAnxjHVIPRShl0QtqUYZE05I\nB6WsXGlP8gDCdF7tEYFYvRfOljaGkNzF++veaL/p2pJXeI/pyIGFwyZ04VC6ONjY\nAmgT9ijRR2EhL5RtyuvWBihRaEIKdwSmX9DHuqru/a4+21tTva2/nn40o1YqDedG\nSxSpR7GEEsW3tHRdRrYlkm6pafrvBfTeyDh7CMLeS+hNNPLK72slr/lNWXaCLDJk\nryTNdZxrxFmk6NqH9l8XJCX/+043Q/xFeqnVu3O0OgCmRS9XRRNv4ED0Y2WnRaUA\naxTKRmj7AgMBAAECggEAAWJ5RIZyXt13w3lvOex6nslqem4qoFv7FNjQvYs3WYrY\nqYHvzlprDd5aQlzf1d6hGAkL1sbE/aqJ1ky8F+wmrWWgQiMR5ENPL08AmE5mwI7b\nGKzf+zwg68AQIwrUtV7PLk5qKiIn8kk4YD+FnV6fcTPJpTFQ3DfJi8LIKW+59egO\nivYQGibYPO2ZMHaXwAPgw04F2SEVMGnyH90CIg48vECiwH5OTwKOZO2sScAEPi6x\n6OZWcc3DWM38oIBWZgFe5NpcAflpkjoNEae2ZUHkQYKTyQoGyNACpjT4P9eJmmGl\nVusVZDCU8hl3vtv8a05ce3wUxrw+9F13G6BzmFwFAQKBgQDckit3szLaKLCks7XL\n90XgLs2JulSZRP/fC6ZYjoNnYqQPsQXmUkqqc7gPmJwGJQ7wrCwt3oUnTfOYyoU7\nPmGO6W3UWrVYmsUqzM7ljcuIFQVZ4DrURh2D1NYnKlOfUmemUdXHBS7WGmncwVrN\nCncxrHdNAtSy6QRnHnhRLI4RAQKBgQDalmgHJTTQRoVLCc0pYc0x+T4lMDozu0aY\nfbHA8NzXMoCvOkazmQDKLOcYnsQ4KKiZnvPgvYWQdYOPRfWjAA7un6eb3wZ48tle\nWhhcfIXxlzYIB7ldNgB1fMYBPKoDxML7PePyV8fIt5yvoWfNBoXp/eiBGjC7zg85\nlVID6m69+wKBgGk1X35s/2iH2Bn3jlOH0tCoAmobcpbZDqM5T1MWRx9dMZw6d/Hw\ndqTvJXBq/qD/tCygM0J9zU1+JR9QGlSz4JzmQvjze2SUDk/n+RgNlnOGc1L3XZg7\nCYEEWFG+Irzv9XijuDCCDGEboDsoD0OWJlyPQyh8nS/pbawyIHm3otwBAoGAQD3m\nEvuHWxLux8/4oQniYaaL8igLYvrOU7DVVgw0C3cHRPiOoOf9yG2L+btnlc21R3jk\n2g85OuyenrQD7kTtHoqDmxbf8xoEcmtuL3hVcVxsf82tV8ywM9dY5jIrEXJ5j5dv\n7+CGkeIMxb1dIW+iWdPKym6lFf0IknblJMt4gpMCgYBdiahUPvh2k8JRURwVZLNa\nJmkO33mO9QbH1cvPt/x5odBpzzpT0WaM6BMxZ5EpxDR42DPo9/Iw9CIJ/o7GnRQE\nlz835j5gKOrPrHDa+XzRwyQbk60AH0HVkbrONZeYqP+/4R7nNms9SOA6X0mci6fb\n1+3gI3+Cja/b6Q85t9kuIQ=="\n-----END PRIVATE KEY-----\n";

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

  config.service_account.data.project_id = project_id;
  config.service_account.data.client_email = client_email;
  config.service_account.data.private_key = private_key;
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
