#include <Arduino.h>
#include "BLE.h"
#include "RBLE.h"

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    // setupBLE();
    pinMode(GPIO_NUM_0, INPUT);
    xTaskCreate(rbletask, "task", 8192 * 5, NULL, 1, NULL);
}

void loop()
{
    // loopBLE();
}
