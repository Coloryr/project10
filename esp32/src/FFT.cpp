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
float vReal[samples];
float *vImag;

const char cData[] = {0x63, 0x6C, 0x65, 0x20, 0x31, 0x2C, 0x30, 0xff, 0xff, 0xff};
const char nData[15] = {0x61, 0x64, 0x64, 0x74, 0x20, 0x31, 0x2C, 0x30, 0x2C, 0x32, 0x34, 0x30, 0xff, 0xff, 0xff};

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

float baseFrequency[5];
float range[5];

uint16_t points;

// double tau(double x)
// {
//     double p1 = log(3 * pow(x, 2.0) + 6 * x + 1);
//     double part1 = x + 1 - sqrt(2.0 / 3.0);
//     double part2 = x + 1 + sqrt(2.0 / 3.0);
//     double p2 = log(part1 / part2);
//     return (1.0 / 4.0 * p1 - sqrt(6) / 24 * p2);
// }

// double peakFreq(uint16_t peakPosIndex, float *vReal, float *vImag, double sampleRate, uint16_t N)
// {
//     uint16_t k = peakPosIndex;
//     double divider = pow(vReal[k], 2.0) + pow(vImag[k], 2.0);
//     double ap = (vReal[k + 1] * vReal[k] + vImag[k + 1] * vImag[k]) / divider;
//     double dp = -ap / (1.0 - ap);
//     double am = (vReal[k - 1] * vReal[k] + vImag[k - 1] * vImag[k]) / divider;

//     double dm = am / (1.0 - am);
//     double d = (dp + dm) / 2 + tau(dp * dp) - tau(dm * dm);

//     double adjustedBinLocation = (double)k + d;
//     double peakFreqAdjusted = (sampleRate * adjustedBinLocation / (double)N);

//     return peakFreqAdjusted;
// }

int round_double(double number)
{
    return (number > 0.0) ? floor(number + 0.5) : ceil(number - 0.5);
}

uint16_t getPos(double f)
{
    return round_double(((double)samples * f) / samplingFrequency);
}

float getH(uint16_t index, float *vData)
{
    return fabs(vData[index - 1] - (2.0 * vData[index]) + vData[index + 1]);
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

        if ((abscissa > 700 && abscissa < 1300) || (abscissa > 1700 && abscissa < 2300) || (abscissa > 2700 && abscissa < 3300) || (abscissa > 3700 && abscissa < 4300) || (abscissa > 4700 && abscissa < 5300))
        {
            Serial.printf("index: %d ", i);
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
    points = samplingFrequency / baseFrequency[0];
    if (points > 300)
    {
        uint16_t g = (points / 240) + 1;
        for (uint16_t i = 0; i < 240; i++)
        {
            Serial2.write(data[i * g] + 100);
        }
    }
    else
    {
        for (uint16_t i = 0; i < 240; i++)
        {
            Serial2.write(data[i] + 100);
        }
    }
}

void ffttest()
{
    vImag = new float[samples];
    unsigned long time = millis();
    /* Build raw data */
    for (uint16_t i = 0; i < samples; i++)
    {
        vReal[i] = (float)data[i] / (255 / 10); /* Build data with positive and negative values*/
        vImag[i] = 0.0;                         //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
    }
    FFT.DCRemoval(vReal, samples);
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HANN, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD);              /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */

    float x, y;
    FFT.MajorPeak(vReal, samples, samplingFrequency, &x, &y);

    y = y / (samples / 2);

    Serial.printf("x: %f y: %f\n", x, y);

    baseFrequency[0] = x + 6;
    range[0] = y;

    for (int16_t i = 1; i < 5; i++)
    {
        baseFrequency[i] = (baseFrequency[0]) * (i + 1);
        uint16_t index = getPos(baseFrequency[i]);
        Serial.printf("index: %d\n", index);
        // range[i] = getH(index, vReal);
        range[i] = vReal[index];
        range[i] = range[i] / (samples / 2);
    }

    for (uint16_t i = 0; i < samples; i++)
    {
        vReal[i] = vReal[i] / (samples / 2);
    }

    PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
    Serial.printf("base: %f range1: %f range2: %f range3: %f range4: %f range5: %f\n",
                  baseFrequency[0], range[0], range[1], range[2], range[3], range[4]);

    Serial2.write(cData, 10);
    delay(50);

    Serial2.write(nData, 15);
    delay(200);
    showpoint();

    Serial.print(millis() - time);
    Serial.println(" ms");

    delete vImag;
}