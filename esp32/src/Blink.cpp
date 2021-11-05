#include <Arduino.h>
#include "BLE.h"
#include "FFT.h"

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    setupBLE();
    pinMode(GPIO_NUM_0, INPUT);
}

void loop()
{    
    loopBLE();
}
