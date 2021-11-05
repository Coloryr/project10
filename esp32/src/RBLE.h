#ifndef RBLE_H
#define RBLE_H
#include "Arduino.h"

void rbletask(void *data);
void ffttask(void *arg);

extern uint16_t points;
extern float range[5];
extern float THD;
extern float res[240];
extern uint16_t data[];
#endif