#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "Atendance.h"


class IRsender : public Task {
public:
    IRsender(bool _enabled = false, unsigned long _interval = 0) 
        : Task(_enabled, _interval) {}

protected:
    const char* AC_CONTROL_IP = "192.168.1.179";
    const int AC_CONTROL_PORT = 80;
    const unsigned long SCAN_COOLDOWN = 10000; // 10 seconds cooldown

    unsigned long lastScanTime = 0;
    bool acState = false;

    void setup() override {
        // Optional initialization
    }

    void loop() override {
        if (globalState.acTurnOnRequested && 
            (millis() - lastScanTime > SCAN_COOLDOWN)) {
            
            // Toggle AC state
            acState = !acState;
            Serial.println(acState ? "Turning AC On" : "Turning AC Off");
            
            // Send AC control signal
            turnOnAC(acState);
            
            // Reset request flag and update last scan time
            globalState.acTurnOnRequested = false;
            lastScanTime = millis();
        }
    }

    void turnOnAC(bool x) {
        if (WiFi.status() == WL_CONNECTED) {
            // Prepare payload using ArduinoJson
            DynamicJsonDocument payload(256);
            payload["power"] = x;
            payload["temp"] = 22;
            payload["mode"] = 1;

            // Serialize JSON
            String jsonPayload;
            serializeJson(payload, jsonPayload);

            // Prepare URL
            String url = "http://" + String(AC_CONTROL_IP) + "/state";

            // HTTP Client setup
            WiFiClient client;
            HTTPClient http;
            http.begin(client, url);
            http.addHeader("Content-Type", "application/json");

            // Send PUT request
            int httpCode = http.PUT(jsonPayload);

            // Log and handle response
            if (httpCode == 200) {
                String response = http.getString();
                Serial.println(x ? "AC Turned On" : "AC Turned Off");
                Serial.println("Response: " + response);
            } else {
                Serial.printf("Failed to control AC. Status: %d\n", httpCode);
                Serial.println("Response: " + http.getString());
            }

            http.end();
        }
    }
};
