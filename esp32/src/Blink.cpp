#include <Arduino.h>
#include "BLE.h"
#include "RBLE.h"

void ledtask(void *arg)
{
    for (;;)
    {
        digitalWrite(GPIO_NUM_2, HIGH);
        delay(1000);
        digitalWrite(GPIO_NUM_2, LOW);
        delay(1000);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200, 134217756U, GPIO_NUM_15, GPIO_NUM_4);
    setupBLE();
    pinMode(GPIO_NUM_0, INPUT);
    pinMode(GPIO_NUM_2, OUTPUT);
    pinMode(GPIO_NUM_16, OUTPUT);
    xTaskCreate(ledtask, "led", 1024, NULL, 3, NULL);
    xTaskCreate(rbletask, "task", 8192 * 5, NULL, 1, NULL);
}

void loop()
{
    loopBLE();
}
