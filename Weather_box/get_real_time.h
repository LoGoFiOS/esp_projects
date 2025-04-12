#pragma once

#include <WiFi.h>
#include <time.h>
// #include <HTTPClient.h>
// #include <ArduinoJson.h>

#include "secrets.h" // TODO: change to secrets.h in the end!!!


extern const char* ssid;
extern const char* password;
// extern struct tm timeinfo;


// extern const char* timeZone;
// extern const char* rootCACert;

String getHour() {
  struct tm now;
  getLocalTime(&now);
  char buf[3];
  strftime(buf, sizeof(buf), "%H", &now);
  // Serial.print("Hour: "); Serial.print(buf);
  return String(buf);
}

String getMinute() {
  struct tm now;
  getLocalTime(&now);
  char buf[3];
  strftime(buf, sizeof(buf), "%M", &now);
  // Serial.print(", Minute: "); Serial.println(buf);
  return String(buf);
}

String getDate() {
  struct tm now;
  getLocalTime(&now);
  char buf[7];
  strftime(buf, sizeof(buf), "%d.%m", &now);
  return String(buf);
}

String getDayName() {
  struct tm now;
  getLocalTime(&now);
  char buf[4];
  strftime(buf, sizeof(buf), "%a", &now);
  return String(buf);
}

// void printTime(){
//   struct tm now;
//   getLocalTime(&now);
//   char buf[9];
//   strftime(buf, sizeof(buf), "%H:%M:%S", &now);
//   Serial.print("Time: ");
//   Serial.println(buf);

//   char dateStr[11];
//   strftime(dateStr, sizeof(dateStr), "%d.%m.%Y", &now);
//   Serial.print("Date: ");
//   Serial.println(dateStr);

//   char dayStr[4];
//   strftime(dayStr, sizeof(dayStr), "%a", &now); 
//   Serial.print("Day: ");
//   Serial.println(dayStr);
// }

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

bool NTPSyncTyme(uint16_t maxAttempts = 20, uint16_t delayMs = 500) {
  struct tm now;
  for (int i = 0; i < maxAttempts; i++) {
    if (getLocalTime(&now)) {
      // printTime(timeinfo);
      return true;
    }
    delay(delayMs);
  }
  // Serial.println("[ERROR] Failed to get time from NTP.");
  return false;
}



//   bool NTPSyncTymeFromAPI(String& outTime) {
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
  