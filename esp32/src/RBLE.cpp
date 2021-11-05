#include "Arduino.h"
#include "RBLE.h"

const uint16_t samples = 8192 * 2; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 2000000;

uint16_t points;
float range[5];
float THD;
float baseFrequency;
float res[240];
uint16_t data[4096];

bool mode = false;

union RTest
{
    int16_t u16;
    uint8_t u8[2];
};

union FTest
{
    float f;
    uint8_t u8[2];
};

void rbletask(void *arg)
{
    Serial.println("start read");
    uint8_t resdata[8192 + 14];
    Serial2.setRxBufferSize(8192 + 14);
    Serial2.setTimeout(100);
    for (;;)
    {
        if (mode)
        {
            Serial2.readBytes(resdata, 8192 + 14);
            mode = false;
            Serial.println("read start");
            RTest test;
            uint16_t i = 0;
            for (uint16_t size = 0; size < 8192;)
            {
                test.u8[0] = resdata[size];
                size++;
                test.u8[1] = resdata[size];
                size++;
                data[i++] = test.u16;
            }

            FTest test1;
            test1.u8[0] = resdata[8192];
            test1.u8[1] = resdata[8193];
            THD = test1.f;

            for (uint16_t i = 0; i < 5; i++)
            {
                test1.u8[0] = resdata[8194 + (i * 2)];
                test1.u8[1] = resdata[8194 + (i * 2) + 1];
                range[i] = test1.f;
            }
            test1.u8[0] = resdata[8204];
            test1.u8[1] = resdata[8205];
            baseFrequency = test1.f;

            points = samplingFrequency / baseFrequency;

            for (uint16_t size = 0; size < 8192 + 14; size++)
            {
                Serial.printf("%d, ", resdata[size]);
            }

            Serial.println("read done");

            Serial.printf("base: %f range1: %f range2: %f range3: %f range4: %f range5: %f THD: %f%c\r",
                          baseFrequency, range[0], range[1], range[2], range[3], range[4], THD, '%');
        }
        else
        {
            String data1 = Serial2.readString();
            if (data1.startsWith("start:32768"))
            {
                Serial.println("mode true");
                mode = true;
            }
        }
        Serial2.flush();
        delay(50);
    }
}
