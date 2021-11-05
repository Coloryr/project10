#include <Arduino.h>
#include "BLE.h"
#include "FFT.h"

void test(void *data)
{
    for (;;)
    {
        loopBLE();
    }
}

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    setupBLE();
    pinMode(GPIO_NUM_0, INPUT);
    xTaskCreate(test, "test", 8196 * 8, NULL, 1, NULL);
}

void loop()
{
}
