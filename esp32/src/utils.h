#ifndef UTILS_H
#define UTILS_H
#include "Arduino.h"

void addpoint1(float *data, float *res, uint16_t count, float f, uint16_t fix);
float findmax(float *data);
void sinx_init();
void sinx_do(float ref[], float result[]);
float addpoint(float *data, uint16_t start);

#endif 
