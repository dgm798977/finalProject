#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "secrets.h"

#include <ArduinoBLE.h>



const char* ssid = "wifiSSID";            // Replace with your Wi-Fi SSID
const char* password = "passsword";    // Replace with your Wi-Fi password

const char* edgeImpulseURL = "ingestion.edgeimpulse.com";
const int edgeImpulsePort = 443;
const char* apiKey = valueKey;  // Replace with your API key

#define MAX_DATA 100
String accX[MAX_DATA];
String accY[MAX_DATA];
String accZ[MAX_DATA];

int i = 0;

WiFiClientSecure client;


BLEDevice peripheral;
BLECharacteristic accelChar;



void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  delay(4000);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
  }

  if (!BLE.begin()) {
  Serial.println("Fallo al iniciar BLE");
  while (1);
  }

  Serial.println(" BLE Initialized! || Wi-Fi connected!");
  
  client.setInsecure();  // Use only if needed (bypasses SSL verification)
  BLE.scan();  // Inicia escaneo una vez

  if (client.connect(edgeImpulseURL, edgeImpulsePort)) {
    Serial.println("Connected to Edge Impulse!");
    //connected = true;  // Set connected flag
  } else {
    Serial.println("Connection to Edge Impulse failed!");
  }

  // Esperamos hasta que encontremos el periférico deseado
  while (true) {
    peripheral = BLE.available();
    
    delay(1000);
    if (peripheral && peripheral.localName() == "NanoBLE-IMU") {
      Serial.println("BLE device found, connecting...");
      BLE.stopScan();

      if (peripheral.connect()) {
        Serial.println("Connected to BLE peripheral.");
        if (peripheral.discoverAttributes()) {
          accelChar = peripheral.characteristic("2C06");

          if (accelChar.subscribe()) {
            Serial.println("Subscribed to accelChar!");
            break;
          }
          else {
            Serial.println("Failed to subscribe!");
          }
        }
      } else {
        Serial.println("No se pudo conectar. Reintentando...");
        BLE.scan(); // Reinicia escaneo si falla la conexión
      }
    }

    delay(500); // Espera entre intentos de conexión
  }
  Serial.print("Properties: ");
  Serial.println(accelChar.properties(), HEX);
  
}

void loop() {
  // Suponiendo que ya tienes acceso a `accelChar` y `peripheral`
  BLE.poll();

  if (peripheral.connected()) {
    if (accelChar.valueUpdated()) {
      // Data is updated, read it
      Serial.println("Reading data from accelChar...");
      uint8_t buffer[12];
      accelChar.readValue(buffer, 12);  // Read the latest value into the buffer
      Serial.println("Data received: ");
      printData(buffer, 12);  // Print the received data in hexadecimal format
      //Serial.println(accelChar.valueLength());
        
      float x, y, z;
      memcpy(&x, &buffer[0], 4);
      memcpy(&y, &buffer[4], 4);
      memcpy(&z, &buffer[8], 4);
      x = x * 9.81;
      y = y * 9.81;
      z = z * 9.81;

      Serial.print("X: "); Serial.print(x, 2);
      Serial.print(" Y: "); Serial.print(y, 2);
      Serial.print(" Z: "); Serial.println(z, 2);

      accX[i] = String(x,2);
      accY[i] = String(y,2);
      accZ[i] = String(z,2);
      i = i + 1;
    } 
    else {
      Serial.println("No data update detected.");
    }
  }
  else {
    Serial.println("Peripheral disconnected.");
  }
  if(i == MAX_DATA){
    i = 0;
    
    // Example: Sending a small chunk of accelerometer data
    String payload = "{";

      // Add the "protected" section
      payload += "\"protected\": {";
      payload += "\"ver\": \"v1\",";
      payload += "\"alg\": \"none\",";
      payload += "\"iat\": 1625252817";
      payload += "},";
      
      // Add the "signature" section
      payload += "\"signature\": \"none\",";
      
      // Add the "payload" section
      payload += "\"payload\": {";
      payload += "\"device_name\": \"ESP32\",";
      payload += "\"device_type\": \"ESP32\",";
      payload += "\"interval_ms\": 50,";
      
      // Add the "sensors" array
      payload += "\"sensors\": [";
      payload += "{ \"name\": \"accX\", \"units\": \"m/s2\" },";
      payload += "{ \"name\": \"accY\", \"units\": \"m/s2\" },";
      payload += "{ \"name\": \"accZ\", \"units\": \"m/s2\" }";
      payload += "],";
      
      // Add the "values" array
      payload += "\"values\": [";
      
      // Add the accX, accY, accZ data as arrays
      
      for (int i = 0; i < MAX_DATA; i++) {
        payload += "[";
        payload += accX[i];
        payload += ", ";
        payload += accY[i];
        payload += ", ";
        payload += accZ[i];
        payload += " ]";
        if (i < MAX_DATA - 1) {
          payload += ", ";
        }
      }
      
      
      
      // Close the "values" and "payload" sections
      payload += "]";
      payload += "}";

      // Close the entire JSON object
      payload += "}";
    /*
    Serial.println("Payload:");
    Serial.println(payload);
    
    // Print the accX array values
    Serial.println("accX Array:");
    for (int i = 0; i < MAX_DATA; i++) {
      Serial.println(accX[i]);
    }
    */
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

    //delay(1000);  // Wait before sending the next data chunk

    while (client.available()) {
        String response = client.readString();
        Serial.println(response);
    }

  }
  delay(50);  // Small delay before next loop iteration
}

void printData(const unsigned char data[], int length) {
  for (int i = 0; i < length; i++) {
    unsigned char b = data[i];

    if (b < 16) {
      Serial.print("0");
    }

    Serial.print(b, HEX);
  }
  Serial.println();  // Print a newline after the data
}