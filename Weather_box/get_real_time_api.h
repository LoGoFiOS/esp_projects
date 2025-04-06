#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "secrets.h" // TODO: change to secrets.h in the end!!!


extern const char* ssid;
extern const char* password;
extern const char* timeZone;
extern const char* rootCACert;

bool connectToWiFi() {
    Serial.print("Connecting to Wi-Fi");
    WiFi.begin(ssid, password);
  
    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 10000;  // 10 seconds timeout
  
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

  bool getTimeFromAPI(String& outTime) {
    if (WiFi.status() != WL_CONNECTED) {
      outTime = "No WiFi";
      return false;
    }
  
    WiFiClientSecure client;                
    client.setCACert(rootCACert);
  
    HTTPClient http;
    String url = "https://timeapi.io/api/Time/current/zone?timeZone=" + String(timeZone);
    http.begin(client, url); 
  
    int httpCode = http.GET();
    if (httpCode != 200) {
      http.end();
      Serial.println("HTTP Error: " + String(httpCode));
      outTime = "HTTP Error";
      return false;
    }
  
    String payload = http.getString();
    http.end();
  
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err) {
      outTime = "Parse Error";
      return false;
    }
  
    uint8_t hour = doc["hour"];
    uint8_t minute = doc["minute"];
    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", hour, minute);
    outTime = String(timeStr);
    return true;
  }
  