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

bool BLEsend = false;

void go();

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        pServer->getAdvertising()->stop();
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
                digitalWrite(GPIO_NUM_16, LOW);
                delay(100);
                digitalWrite(GPIO_NUM_16, HIGH);
            }
        }
    }
};
void setupBLE()
{
    Serial.begin(115200);

    // Create the BLE Device
    BLEDevice::init("A题测试机");

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

    if (points >= 80)
    {
        uint16_t g = (points / 500) + 1;
        for (uint16_t i = 0; i < 500; i++)
        {
            data2[i + 500] = uint8_t((data[i * g] + 500) * 0.1) + 50;
        }
        for (uint16_t i = 0; i < 500; i++)
        {
            data2[i] = data2[999 - i];
        }
        all = 500;
    }
    else
    {
        float vReal[1000];
        double fix = 240 / (points + 20);
        for (uint16_t i = 0; i < points + 20; i++)
        {
            vReal[i] = (float)data[i] / (4096 / 10);
        }
        addpoint1(vReal, res, points + 20, baseFrequency, fix);
        for (uint16_t i = 0; i < 240; i++)
        {
            uint16_t data1 = uint16_t(res[240 - i] * 0.1 * (4096 / 10)) + 100;
            data2[i] = data1;
        }

        all = 240;
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
}

void loopBLE()
{

    if (digitalRead(GPIO_NUM_0) == LOW)
    {
        digitalWrite(GPIO_NUM_16, LOW);
        delay(100);
        digitalWrite(GPIO_NUM_16, HIGH);
    }
    if (BLEsend)
    {
        go();
        BLEsend = false;
    }
    delay(100);
}