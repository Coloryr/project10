#ifndef FFT_H
#define FFT_H
#include <stdint.h>

void ffttest();

const uint32_t samples = 16384;
const uint32_t samplingFrequency = 2000000;

extern float baseFrequency[5];
extern uint16_t points;
extern float range[5];
extern float THD;
extern float res[240];

extern float vReal[samples];
extern float vReal1[4096];

union testU
{
    int16_t u16;
    int8_t u8;
};

#endif
