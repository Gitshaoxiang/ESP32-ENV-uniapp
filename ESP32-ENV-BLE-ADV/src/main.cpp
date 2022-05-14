#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "SHT3X.h"

SHT3X sht30;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// #define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define SERVICE_UUID        "0000c201-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer;
BLEService *pService;
BLEAdvertising *pAdvertising;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE work!");
    BLEDevice::init("ESP32_SHT30");
    pServer                            = BLEDevice::createServer();
    pService                           = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    pCharacteristic->setValue("Hello this is ESP32 SHT30 SENSOR");
    pService->start();

    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(
        0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();
    Wire.begin(26, 32);
}

void advData(char *payload) {
    pAdvertising->stop();
    BLEAdvertisementData advert;
    advert.setName("ESP32_SHT30");
    advert.setManufacturerData(payload);
    advert.setServiceData(pService->getUUID(), payload);
    pAdvertising->setAdvertisementData(advert);
    pAdvertising->start();
    delay(800);
}

void loop() {
    if (sht30.get() == 0) {
        char data[50];
        float Temp = sht30.cTemp;
        sprintf(data, "{\"Temp\":%.1f}", Temp);
        Serial.println(data);
        advData(data);
        float Hum = sht30.humidity;
        sprintf(data, "{\"Hum\":%.1f}", Hum);
        Serial.println(data);
        advData(data);
    }
}