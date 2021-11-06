#include "arduinoFFT.h"
#include "FFT.h"
#include "math.h"
#include "utils.h"
#include "Energia.h"

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */

float vReal[samples];
float vImag[samples];

const char cData[] = {0x63, 0x6C, 0x65, 0x20, 0x31, 0x2C, 0x30, 0xff, 0xff, 0xff};
const char nData[] = {0x61, 0x64, 0x64, 0x74, 0x20, 0x31, 0x2C, 0x30, 0x2C, 0x32, 0x34, 0x30, 0xff, 0xff, 0xff};
const char eData[] = {0xff, 0xff, 0xff};

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

float baseFrequency[5];
float range[5];
float THD;

float vReal1[4096];

uint16_t points;

int round_double(double number)
{
    return (number > 0.0) ? floor(number + 0.5) : ceil(number - 0.5);
}

uint16_t getPos(double f)
{
    return round_double(((double)samples * f) / samplingFrequency);
}

void PrintVector(float *vData, uint16_t bufferSize, uint8_t scaleType)
{
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

        if ((abscissa > (baseFrequency[0] - 300) && abscissa < (baseFrequency[0] + 300)) ||
            (abscissa > (baseFrequency[1] - 300) && abscissa < (baseFrequency[1] + 300)) ||
            (abscissa > (baseFrequency[2] - 300) && abscissa < (baseFrequency[2] + 300)) ||
            (abscissa > (baseFrequency[3] - 300) && abscissa < (baseFrequency[3] + 300)) ||
            (abscissa > (baseFrequency[4] - 300) && abscissa < (baseFrequency[4] + 300)))
        {
            printf1("index: %d ", i);
            printf1("%f", abscissa);
            if (scaleType == SCL_FREQUENCY)
                printf1(0, "Hz");
            printf1(" ");
            printf1("%f", vData[i]);
            printf1("\r");
        }
    }
    printf1("\r");
}

float res[240];

void showpoint()
{
    uint16_t i = 0;
    uint8_t data1;
    points = samplingFrequency / baseFrequency[0];
    printf1("points: %d\r", points);
    if (points > 100)
    {
        uint16_t g = (points / 240) + 1;
        for (i = 0; i < 240; i++)
        {
            data1 = uint16_t(vReal1[i * g] * 0.1 * (4096 / 10)) + 100;
            putdata(&data1, 1);
        }
    }
    else
    {
        double fix = 240 / (points + 20);
        addpoint1(vReal1, res, points + 20, baseFrequency[0], fix);
        for (i = 0; i < 240; i++)
        {
            data1 = uint16_t(res[240 - i] * 0.1 * (4096 / 10)) + 100;
            putdata((uint8_t *)&data1, 1);
        }
    }
}

void ffttest()
{
    uint16_t i;
    unsigned long time = millis();
    /* Build raw data */

    for(i=0; i < 4096;i++)
    {
        vReal1[i] = vReal[i];
    }

    for (i = 0; i < samples; i++)
    {
        vImag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
    }

    FFT.DCRemoval(vReal, samples);
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HANN, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD);              /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples);                /* Compute magnitudes */

    float x = FFT.MajorPeak(vReal, samples, samplingFrequency);

    baseFrequency[0] = x + 6;

    for (i = 0; i < 5; i++)
    {
        baseFrequency[i] = (baseFrequency[0]) * (i + 1);
        uint16_t index = getPos(baseFrequency[i]);
        range[i] = addpoint(vReal, index);
        range[i] = range[i] / (samples / 2);
    }

//    for (i = 0; i < samples; i++)
//    {
//        vReal[i] = vReal[i] / (samples / 2);
//    }
//
//    PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);

    if(range[0] < 0.01)
    {
        THD = 0;
        range[0] = range[1] = range[2] = range[3] = range[4] = 0;
    }

    else
    {
        THD = sqrt(sq(range[1]) + sq(range[2]) + sq(range[3]) + sq(range[4])) / range[0];
        THD = THD * 100;
    }

    putdata(eData, 3);
    printf2("t1.txt=\"%.2f%c\"", THD, '%');
    putdata(eData, 3);
    delay(10);
    printf2("t3.txt=\"%.5f\"", range[0]);
    putdata(eData, 3);
    delay(10);
    printf2("t4.txt=\"%.5f\"", range[1]);
    putdata(eData, 3);
    delay(10);
    printf2("t5.txt=\"%.5f\"", range[2]);
    putdata(eData, 3);
    delay(10);
    printf2("t6.txt=\"%.5f\"", range[3]);
    putdata(eData, 3);
    delay(10);
    printf2("t7.txt=\"%.5f\"", range[4]);
    putdata(eData, 3);
    delay(10);

    putdata(cData, 10);
    delay(20);

    putdata(nData, 15);
    delay(100);
    showpoint();

    printf1("%d", millis() - time);
    printf1(" ms\r");

    printf1("points %d base: %f range1: %f range2: %f range3: %f range4: %f range5: %f THD: %f%c\r",
            points, baseFrequency[0], range[0], range[1], range[2], range[3], range[4], THD, '%');
}
