#include "Arduino.h"
#include "BLE.h"
#include "utils.h"
#include "RBLE.h"

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

const char *command1 = "points:";
const char *command2 = "thd:";
const char *command3 = "range0:";
const char *command4 = "range1:";
const char *command5 = "range2:";
const char *command6 = "range3:";
const char *command7 = "range4:";

bool start = false;

void go();

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
        pServer->getAdvertising()->start();
    }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0)
        {
            Serial.printf("Received Value: %s\n", rxValue.c_str());
            String temp = String(rxValue.c_str());
            if (temp.equals("start"))
            {
                Serial.println("go!");
                start = true;
            }
        }
    }
};
void setupBLE()
{
    Serial.begin(115200);

    // Create the BLE Device
    BLEDevice::init("A题测试机014");

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

void go()
{
    uint32_t time = millis();
    String temp1 = command2 + String(THD, 6);
    txC->setValue(temp1.c_str());
    txC->notify();
    delay(10);
    temp1 = command3 + String(range[0], 6);
    txC->setValue(temp1.c_str());
    txC->notify();
    delay(10);
    temp1 = command4 + String(range[1], 6);
    txC->setValue(temp1.c_str());
    txC->notify();
    delay(10);
    temp1 = command5 + String(range[2], 6);
    txC->setValue(temp1.c_str());
    txC->notify();
    delay(10);
    temp1 = command6 + String(range[3], 6);
    txC->setValue(temp1.c_str());
    txC->notify();
    delay(10);
    temp1 = command7 + String(range[4], 6);
    txC->setValue(temp1.c_str());
    txC->notify();
    delay(10);

    uint8_t data2[1600];

    uint16_t all;

    if (points > 100)
    {
        uint16_t g = (points / 240) + 1;
        for (uint16_t i = 0; i < 240; i++)
        {
            data2[i] = uint8_t((data[i * g] + 500) * 0.1);
        }
        all = 240;
    }
    else
    {
        for (uint16_t i = 0; i < points * 2; i++)
        {
            data2[i] = uint8_t((data[i] + 500) * 0.1);
        }

        all = points * 2;
    }

    char buf[10];
    uint16_t size = sprintf(buf, "%d", all);
    char buf1[20];
    for (uint16_t i = 0; i < 7; i++)
    {
        buf1[i] = command1[i];
    }
    for (uint16_t i = 0; i < size; i++)
    {
        buf1[i + 7] = buf[i];
    }

    txC->setValue((uint8_t *)buf1, 7 + size);
    txC->notify();

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

    Serial.print(millis() - time);
    Serial.println(" ms");
    Serial.print("Send data\n");
}

void loopBLE()
{
    if (deviceConnected && digitalRead(GPIO_NUM_0) == LOW)
    {
        start = true;
    }
    if (start)
    {
        go();
        start = false;
    }
    delay(100);
}