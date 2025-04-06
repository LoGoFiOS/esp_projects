#include <WiFi.h>
#include <time.h>
// #include <HTTPClient.h>
// #include <ArduinoJson.h>

#include "secrets_real.h" // TODO: change to secrets.h in the end!!!


extern const char* ssid;
extern const char* password;
// extern struct tm timeinfo;

// extern const char* timeZone;
// extern const char* rootCACert;

void printTime(struct tm timeinfo){

  char timeStr[6];
  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
  Serial.print("Time: ");
  Serial.println(timeStr);

  char dateStr[11];
  strftime(dateStr, sizeof(dateStr), "%d.%m.%Y", &timeinfo);
  Serial.print("Date: ");
  Serial.println(dateStr);

  char dayStr[10];
  strftime(dayStr, sizeof(dayStr), "%a", &timeinfo); 
  Serial.print("Day: ");
  Serial.println(dayStr);
}

bool connectToWiFi() {
    Serial.print("Connecting to Wi-Fi");
    WiFi.begin(ssid, password);
  
    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 10000;
  
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
      delay(500);
      Serial.print(".");
    }
  
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(" connected!");
      return true;
    } else {
      Serial.println(" failed.");
      return false;
    }
  }

bool getTime(struct tm& timeinfo, uint16_t maxAttempts = 20, uint16_t delayMs = 500) {
  for (int i = 0; i < maxAttempts; i++) {
    if (getLocalTime(&timeinfo)) {
      printTime(timeinfo);
      return true;
    }
    delay(delayMs);
  }
  // Serial.println("[ERROR] Failed to get time from NTP.");
  return false;
}



//   bool getTimeFromAPI(String& outTime) {
//     if (WiFi.status() != WL_CONNECTED) {
//       outTime = "No WiFi";
//       return false;
//     }
  
//     WiFiClientSecure client;                
//     client.setCACert(rootCACert);
  
//     HTTPClient http;
//     String url = "https://timeapi.io/api/Time/current/zone?timeZone=" + String(timeZone);
//     http.begin(client, url); 
  
//     int httpCode = http.GET();
//     if (httpCode != 200) {
//       http.end();
//       Serial.println("HTTP Error: " + String(httpCode));
//       outTime = "HTTP Error";
//       return false;
//     }
  
//     String payload = http.getString();
//     http.end();
  
//     JsonDocument doc;
//     DeserializationError err = deserializeJson(doc, payload);
//     if (err) {
//       outTime = "Parse Error";
//       return false;
//     }
  
//     uint8_t hour = doc["hour"];
//     uint8_t minute = doc["minute"];
//     char timeStr[6];
//     sprintf(timeStr, "%02d:%02d", hour, minute);
//     outTime = String(timeStr);
//     return true;
//   }
  