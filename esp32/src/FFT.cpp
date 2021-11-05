#include "arduinoFFT.h"
#include "FFT.h"

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
These values can be changed in order to evaluate the functions
*/
const uint16_t samples = 8192 * 2; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 2000000;
/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
float *vReal;
float *vImag;

const char cData[] = {0x63, 0x6C, 0x65, 0x20, 0x31, 0x2C, 0x30, 0xff, 0xff, 0xff};
const char nData[15] = {0x61, 0x64, 0x64, 0x74, 0x20, 0x31, 0x2C, 0x30, 0x2C, 0x32, 0x34, 0x30, 0xff, 0xff, 0xff};

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

float baseFrequency;
float range[5];
float magin[4];

void PrintVector(float *vData, uint16_t bufferSize, uint8_t scaleType)
{
    bool find = false;
    float k = 50;
    uint8_t step = 0;
    range[1] = range[2] = range[3] = range[4] = 0;
    magin[0] = magin[1] = magin[2] = magin[3] = 0;
    for (uint16_t i = 0; i < bufferSize; i++)
    {
        float abscissa = 0;
        /* Print abscissa value */
        switch (scaleType)
        {
        case SCL_INDEX:
            abscissa = (i * 1.0);
            break;
        case SCL_TIME:
            abscissa = ((i * 1.0) / samplingFrequency);
            break;
        case SCL_FREQUENCY:
            abscissa = ((i * 1.0 * samplingFrequency) / samples);
            break;
        }
        if (!find)
        {
            if (abscissa > baseFrequency)
            {
                find = true;
            }
        }
        else if (step < 5)
        {
            double temp = abs(abscissa - (baseFrequency * (2 + step)));
            if (temp < k)
            {
                if (magin[step] >= temp)
                {
                    range[1 + step] = vData[i];
                }
                magin[step] = temp;
            }
            else if (range[1 + step] != 0)
            {
                step++;
            }
        }

        if (abscissa > 700 && abscissa < 1300)
        {
            Serial.print(abscissa, 6);
            if (scaleType == SCL_FREQUENCY)
                Serial.print("Hz");
            Serial.print(" ");
            Serial.println(vData[i], 4);
        }
        else if (abscissa > 1700 && abscissa < 2300)
        {
            Serial.print(abscissa, 6);
            if (scaleType == SCL_FREQUENCY)
                Serial.print("Hz");
            Serial.print(" ");
            Serial.println(vData[i], 4);
        }
        else if (abscissa > 2700 && abscissa < 3300)
        {
            Serial.print(abscissa, 6);
            if (scaleType == SCL_FREQUENCY)
                Serial.print("Hz");
            Serial.print(" ");
            Serial.println(vData[i], 4);
        }
        else if (abscissa > 3700 && abscissa < 4300)
        {
            Serial.print(abscissa, 6);
            if (scaleType == SCL_FREQUENCY)
                Serial.print("Hz");
            Serial.print(" ");
            Serial.println(vData[i], 4);
        }
        else if (abscissa > 4900 && abscissa < 5100)
        {
            Serial.print(abscissa, 6);
            if (scaleType == SCL_FREQUENCY)
                Serial.print("Hz");
            Serial.print(" ");
            Serial.println(vData[i], 4);
        }
    }
    Serial.println();
}

void showpoint()
{
    uint16_t f = samplingFrequency / baseFrequency;
    uint16_t g = (f / 240) + 1;
    for (uint16_t i = 0; i < 240; i++)
    {
        Serial2.write(data[i * g] + 100);
    }
}

void ffttest()
{
    vReal = new float[samples];
    vImag = new float[samples];
    unsigned long time = millis();
    /* Build raw data */
    for (uint16_t i = 0; i < samples; i++)
    {
        vReal[i] = (float)data[i] / (255 / 10); /* Build data with positive and negative values*/
        vImag[i] = 0.0;                         //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
    }
    FFT.DCRemoval(vReal, samples);
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD);                 /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples);                   /* Compute magnitudes */

    float x, y;
    FFT.MajorPeak(vReal, samples, samplingFrequency, &x, &y);

    Serial.printf("x: %f y: %f\n", x, y);

    baseFrequency = x;
    range[0] = y;

    for (uint16_t i = 0; i < samples; i++)
    {
        vReal[i] = vReal[i] / (samples / 2);
    }

    PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
    Serial.printf("base: %f range1: %f range2: %f range3: %f range4: %f range5: %f\n",
                  baseFrequency, range[0], range[1], range[2], range[3], range[4]);

    Serial2.write(cData, 10);
    delay(50);

    Serial2.write(nData, 15);
    delay(200);
    showpoint();

    Serial.print(millis() - time);
    Serial.println(" ms");

    while (1)
        ; /* Run Once */
          // delay(2000); /* Repeat after delay */
}