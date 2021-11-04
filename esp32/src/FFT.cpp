/*

	Example of use of the FFT libray
        Copyright (C) 2014 Enrique Condes

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
  In this example, the Arduino simulates the sampling of a sinusoidal 1000 Hz
  signal with an amplitude of 100, sampled at 5000 Hz. Samples are stored
  inside the vReal array. The samples are windowed according to Hamming
  function. The FFT is computed using the windowed samples. Then the magnitudes
  of each of the frequencies that compose the signal are calculated. Finally,
  the frequency with the highest peak is obtained, being that the main frequency
  present in the signal. This frequency is printed, along with the magnitude of
  the peak.
*/

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

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void PrintVector(float *vData, uint16_t bufferSize, uint8_t scaleType)
{
    for (uint16_t i = 0; i < bufferSize; i++)
    {
        float abscissa;
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
        if (abscissa > 90000 && abscissa < 110000)
        {
            Serial.print(abscissa, 6);
            if (scaleType == SCL_FREQUENCY)
                Serial.print("Hz");
            Serial.print(" ");
            Serial.println(vData[i], 4);
        }
        else if (abscissa > 190000 && abscissa < 210000)
        {
            Serial.print(abscissa, 6);
            if (scaleType == SCL_FREQUENCY)
                Serial.print("Hz");
            Serial.print(" ");
            Serial.println(vData[i], 4);
        }
        else if (abscissa > 290000 && abscissa < 310000)
        {
            Serial.print(abscissa, 6);
            if (scaleType == SCL_FREQUENCY)
                Serial.print("Hz");
            Serial.print(" ");
            Serial.println(vData[i], 4);
        }
        else if (abscissa > 390000 && abscissa < 410000)
        {
            Serial.print(abscissa, 6);
            if (scaleType == SCL_FREQUENCY)
                Serial.print("Hz");
            Serial.print(" ");
            Serial.println(vData[i], 4);
        }
        else if (abscissa > 490000 && abscissa < 510000)
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

void ffttest()
{
    vReal = new float[samples];
    vImag = new float[samples];
    unsigned long time = millis();
    /* Build raw data */
    for (uint16_t i = 0; i < samples; i++)
    {
        vReal[i] = data[i]; /* Build data with positive and negative values*/
        vImag[i] = 0.0;     //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
    }
    FFT.DCRemoval(vReal, samples);
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD);                 /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples);                   /* Compute magnitudes */

    for (uint16_t i = 0; i < samples; i++)
    {
        vReal[i] = vReal[i] / (samples / 2);
    }

    Serial.print(millis() - time);
    Serial.println(" ms");
    PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
    while (1)
        ; /* Run Once */
          // delay(2000); /* Repeat after delay */
}