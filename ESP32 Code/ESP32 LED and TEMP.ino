#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "DESKTOP-B2IQ8IM 1427"
#define WIFI_PASSWORD "%04876Ms"

#define LED1 23

#define API_KEY "AIzaSyCI8mwcEIyx27MXa-VgZekVHMwZvT5ZM_Q"
#define SECRET "DCUNTwkNgZ6xf599Lbdo0WaeySr4u6y3i7pY6gPC"
#define DATABASE_URL "my-iot-2-a4be2-default-rtdb.asia-southeast1.firebasedatabase.app"
// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

#ifdef __cplusplus
  extern "C" {
#endif
  uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

unsigned long sendDataPrevMillis = 0;
int getData = 0;
uint8_t temprature_sens_read();
uint8_t inTemp;

void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = SECRET;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // for open-close led1
  inTemp = ((temprature_sens_read() - 32) / 1.8);

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    Serial.print("Sending to Firebase :");
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.getInt(&fbdo, "ESP32/status/LED/LED_1", &getData)) {
      Serial.println("SUCCESS");
      Serial.println("DATA: " + String(getData));
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
  if (getData == 1) {
    digitalWrite(LED1, HIGH);
    delay(200);
  } else {
    digitalWrite(LED1, LOW);
    delay(200);
  }
  if (Firebase.RTDB.setInt(&fbdo, "ESP32/status/TEMP/TEMP_1", inTemp)) {
    Serial.println("SUCCESS");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println(inTemp);
  } else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
  Serial.println();
}
