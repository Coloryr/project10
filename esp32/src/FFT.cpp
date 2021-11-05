#include "arduinoFFT.h"
#include "FFT.h"
#include "math.h"

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
const char nData[] = {0x61, 0x64, 0x64, 0x74, 0x20, 0x31, 0x2C, 0x30, 0x2C, 0x32, 0x34, 0x30, 0xff, 0xff, 0xff};
const char eData[] = {0xff, 0xff, 0xff};

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

float baseFrequency[5];
float range[5];
float THD;

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
            Serial2.write(uint16_t(data[i * g] * 0.1) + 100);
        }
    }
    else
    {
        for (uint16_t i = 0; i < 240; i++)
        {
            Serial2.write(uint16_t(data[i] * 0.1) + 100);
        }
    }
}

float findmax(float *data)
{
    float maxY = 0;
    for (uint16_t i = 1; i < ((20 >> 1) + 1); i++)
    {
        if ((data[i - 1] < data[i]) && (data[i] > data[i + 1]))
        {
            if (data[i] > maxY)
            {
                maxY = data[i];
            }
        }
    }
    return maxY;
}

#define REF_LEN 5 //参考数据长度
#define INTER 4   //插值比例
float sinx[INTER * REF_LEN][REF_LEN];
void sinx_init()
{
    for (int t = 0; t < INTER * REF_LEN; t++)
    {
        for (int m = 0; m < REF_LEN; m++)
        {
            int temp = t - m * INTER;
            if (temp == 0)
            {
                sinx[t][m] = 1;
            }
            else
            {
                float x = PI * ((float)temp) / INTER;
                sinx[t][m] = sin(x) / x;
            }
        }
    }
}
void sinx_do(float ref[], float result[])
{
    int off = 0;
    for (int t = 0; t < INTER * REF_LEN; t++)
    {
        float sum = 0;
        for (int m = 0; m < REF_LEN; m++)
            sum += ref[m] * sinx[t][m];
        result[off] = (sum);
        off++;
    }
}

float addpoint(float *data, uint16_t start)
{
    sinx_init();
    float buf[REF_LEN];
    float buf_inter[REF_LEN * INTER];
    start -= 2;
    for (uint16_t a = 0; a < 5; a++, start++)
    {
        buf[a] = data[start];
    }
    sinx_do(buf, buf_inter);
    return findmax(buf_inter);
}

void ffttest()
{
    vReal = (float *)malloc(sizeof(float) * (samples));
    vImag = (float *)malloc(sizeof(float) * (samples));
    unsigned long time = millis();
    /* Build raw data */
    for (uint16_t i = 0; i < samples; i++)
    {
        vReal[i] = (float)data[i] / (4096 / 10); /* Build data with positive and negative values*/
        vImag[i] = 0.0;                          //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
    }
    FFT.DCRemoval(vReal, samples);
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HANN, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD);              /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples);                /* Compute magnitudes */

    float x = FFT.MajorPeak(vReal, samples, samplingFrequency);

    baseFrequency[0] = x + 6;

    for (int16_t i = 0; i < 5; i++)
    {
        baseFrequency[i] = (baseFrequency[0]) * (i + 1);
        uint16_t index = getPos(baseFrequency[i]);
        range[i] = addpoint(vReal, index);
        range[i] = range[i] / (samples / 2);
    }

    for (uint16_t i = 0; i < samples; i++)
    {
        vReal[i] = vReal[i] / (samples / 2);
    }

    PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);

    THD = sqrt(sq(range[1]) + sq(range[2]) + sq(range[3]) + sq(range[4])) / range[0];
    THD = THD * 100;

    Serial2.write(cData, 10);
    delay(50);

    Serial2.write(nData, 15);
    delay(200);
    showpoint();
    delay(50);

    Serial2.printf("t1.txt=\"%.2f%c\"", THD, '%');
    Serial2.write(eData, 3);

    Serial2.printf("t3.txt=\"%.5f\"", range[0]);
    Serial2.write(eData, 3);

    Serial2.printf("t4.txt=\"%.5f\"", range[1]);
    Serial2.write(eData, 3);

    Serial2.printf("t5.txt=\"%.5f\"", range[2]);
    Serial2.write(eData, 3);

    Serial2.printf("t6.txt=\"%.5f\"", range[3]);
    Serial2.write(eData, 3);

    Serial2.printf("t7.txt=\"%.5f\"", range[4]);
    Serial2.write(eData, 3);

    Serial.print(millis() - time);
    Serial.println(" ms");

    Serial.printf("base: %f range1: %f range2: %f range3: %f range4: %f range5: %f THD: %f%c\n",
                  baseFrequency[0], range[0], range[1], range[2], range[3], range[4], THD, '%');

    free(vReal);
    free(vImag);
}