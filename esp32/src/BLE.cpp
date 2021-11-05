#include "Arduino.h"
#include "BLE.h"
#include "FFT.h"
#include "utils.h"

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
            Serial.printf("eceived Value: %s\n", rxValue.c_str());
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

void go()
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

    float *temp = (float *)malloc(sizeof(float) * (points * 2));
    float *temp2 = (float *)malloc(sizeof(float) * (points * 8));
    uint8_t *data2 = (uint8_t *)malloc(sizeof(uint8_t) * (1600));

    uint16_t count = 0;

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
        Serial.println("add points");

        for (uint16_t i = 0; i < points * 2; i += 6)
        {
            temp[i] = float(data[i] * 0.1);
        }
        addpoint1(temp, temp2, points * 2);
        for (; count < points * 8; count++)
        {
            data2[count] = uint8_t((temp2[count] + 500));
        }
    }

    uint16_t all = count;

    char *buf = (char *)malloc(sizeof(char) * 10);
    sprintf(buf, "%d", all);

    Serial.printf("data size: %d\n", all);
    temp1 = "points:" + String(buf);
    txC->setValue(temp1.c_str());
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

    free(temp);
    free(temp2);
    free(buf);
    free(data2);

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