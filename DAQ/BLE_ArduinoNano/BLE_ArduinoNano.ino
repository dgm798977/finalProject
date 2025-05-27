#include <Arduino.h>
// Peripheral (Nano 33 BLE Sense)
#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

BLEService deviceInfoService("180A");
BLEService accelService("180F");
BLECharacteristic accelChar("2C06", BLENotify, 12);

void setup() {
  Serial.begin(9600);
  IMU.begin();
  if (!BLE.begin()) {
  Serial.println("Fallo al iniciar BLE");
  while (1);
  }
  BLE.setLocalName("NanoBLE-IMU");
  BLE.setAdvertisedService(accelService);
  accelService.addCharacteristic(accelChar);
  BLE.addService(accelService);
  BLE.advertise();
}

void loop() {
  float x, y, z;
  BLEDevice central = BLE.central();
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    uint8_t packet[12];
    memcpy(&packet[0], &x, 4);
    memcpy(&packet[4], &y, 4);
    memcpy(&packet[8], &z, 4);
    /*
    Serial.print("Raw bytes: ");
    for (int i = 0; i < 12; i++) {
        Serial.print(packet[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    */
    accelChar.writeValue(packet, 12);
    delay(50);
  }
}
