#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "secrets.h"
/*#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
*/

const char* ssid = "WLAN";
const char* password = "testing1";

const char* edgeImpulseURL = "ingestion.edgeimpulse.com";
const int edgeImpulsePort = 443;
const char* apiKey = valueKey;  // Replace with your API key

WiFiClientSecure client;
/*
BLEScan* pBLEScan;
*/
void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println(" Connected!");
    
    client.setInsecure();  // Use only if needed (bypasses SSL verification)
/*
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();  // Crear escáner BLE
    pBLEScan->setActiveScan(true);    // Activar escaneo activo
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // Define la ventana de escaneo
    */
}

void loop() {
    if (client.connect(edgeImpulseURL, edgeImpulsePort)) {
        Serial.println("Connected to Edge Impulse!");

        // Example: Sending a small chunk of accelerometer data
        String payload = R"({
            "protected": {
                "ver": "v1",
                "alg": "none",
                "iat": 1625252817
            },
            "signature": "none",
            "payload": {
                "device_name": "ESP32",
                "device_type": "ESP32",
                "interval_ms": 1,
                "sensors": [
                    { "name": "accX", "units": "m/s2" },
                    { "name": "accY", "units": "m/s2" },
                    { "name": "accZ", "units": "m/s2" }
                ],
                "values": [
                    [5.01, -9.02, 9.81],
                    [3.02, 7.01, 4.80],
                    [0.00, -9.03, 9.82]
                ]
            }
        })";

        // Send HTTP POST request
        client.println("POST /api/training/data HTTP/1.1");
        client.println("Host: ingestion.edgeimpulse.com");
        client.println("Content-Type: application/json");
        client.print("x-api-key: ");
        client.println(apiKey);
        client.println("x-file-name: esp32-data.json");  // This header is required
        client.print("Content-Length: ");
        client.println(payload.length());
        client.println();
        client.println(payload);

        delay(1000);  // Wait before sending the next data chunk

        while (client.available()) {
            String response = client.readString();
            Serial.println(response);
        }
        
        client.stop();
    } else {
        Serial.println("Failed to connect to Edge Impulse.");
    }

    delay(5000);  // Adjust based on your streaming frequency
}
