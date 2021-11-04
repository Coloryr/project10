#include <Arduino.h>
// #include <lvgl.h>
// #include "BLE.h"
#include "FFT.h"

void setup()
{
    Serial.begin(115200);
    // setupBLE();
}

void loop()
{
    ffttest();
}
