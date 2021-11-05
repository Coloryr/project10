#include "Arduino.h"
#include "BLE.h"
#include "FFT.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLECharacteristic *txC;
BLEService *pService;
BLEServer *pServer;
bool deviceConnected = false;
char BLEbuf[32] = {0};
uint32_t cnt = 0;

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        Serial.println("deviceConnected");
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        Serial.println("deviceDisconnected");
        deviceConnected = false;
    }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0)
        {
            Serial.print("------>Received Value: ");

            for (int i = 0; i < rxValue.length(); i++)
            {
                Serial.print(rxValue[i]);
            }
            Serial.println();

            if (rxValue.find("A") != -1)
            {
                Serial.print("Rx A!");
            }
            else if (rxValue.find("B") != -1)
            {
                Serial.print("Rx B!");
            }
            Serial.println();
        }
    }
};
void setupBLE()
{
    Serial.begin(115200);

    // Create the BLE Device
    BLEDevice::init("ESP32 BLE Test");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    txC = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    txC->addDescriptor(new BLE2902());

    BLECharacteristic *rxC = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
    rxC->setCallbacks(new MyCallbacks());

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");
}

void loopBLE()
{
    if (deviceConnected && digitalRead(GPIO_NUM_0) == LOW)
    {
        ffttest();
        uint32_t time = millis();
        String temp1 = "thd:" + String(THD, 6);
        txC->setValue(temp1.c_str());
        txC->notify();
        delay(10);
        temp1 = "range0:" + String(range[0], 6);
        txC->setValue(temp1.c_str());
        txC->notify();
        delay(10);
        temp1 = "range1:" + String(range[1], 6);
        txC->setValue(temp1.c_str());
        txC->notify();
        delay(10);
        temp1 = "range2:" + String(range[2], 6);
        txC->setValue(temp1.c_str());
        txC->notify();
        delay(10);
        temp1 = "range3:" + String(range[3], 6);
        txC->setValue(temp1.c_str());
        txC->notify();
        delay(10);
        temp1 = "range4:" + String(range[4], 6);
        txC->setValue(temp1.c_str());
        txC->notify();
        delay(10);

        Serial.printf("points: %d\n", points);
        txC->setValue("points:1600");
        txC->notify();

        uint8_t *data2 = new uint8_t[1600];

        if (points > 300)
        {
            uint16_t g = (points / 1600) + 1;
            for (uint16_t i = 0; i < 1600; i++)
            {
                data2[i] = uint8_t((data[i * g] + 500) * 0.1);
            }
        }
        else
        {
            for (uint16_t i = 0; i < 1600; i++)
            {
                data2[i] = uint8_t((data[i] + 500) * 0.1);
            }
        }

        uint16_t all = 1600;

        for (uint16_t now = 0; now < all;)
        {
            if ((all - now) > 20)
            {
                txC->setValue((uint8_t *)data2 + now, 20);
                txC->notify();
                delay(50);
                now += 20;
            }
            else
            {
                txC->setValue((uint8_t *)data2 + now, all - now);
                txC->notify();
                break;
            }
        }

        delete data2;

        Serial.print(millis() - time);
        Serial.println(" ms");
        Serial.print("Send data\n");
    }
    delay(100);
}