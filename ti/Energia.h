/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Energia_h
#define Energia_h

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "avr/dtostrf.h"
#include "avr/pgmspace.h"
#include "avr/interrupt.h"

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/gates/GateMutex.h>

#include "stdio.h"
#include "printf.h"

//#define _POSIX_SOURCE /* prevent conflicting #defines of FD_SET, fd_set, ... from arm compiler's sys/types.h and simplelink.h */

#ifdef __cplusplus
extern "C" {
#endif

/* interrupt edges */
#define CHANGE          4
#define FALLING         3
#define RISING          2
#define HIGH            1
#define LOW             0

/* digital i/o configurations */
#define INPUT           0x0
#define OUTPUT          0x1
#define INPUT_PULLUP    0x2
#define INPUT_PULLDOWN  0x3

/* analogReference() stuff */
#define DEFAULT         1
#define INTERNAL        2
#define EXTERNAL        3

#define INTERNAL1V2     4
#define INTERNAL1V45    5
#define INTERNAL2V5     6

/* SPI stuff */
#define SPI_LAST 0
#define SPI_CONTINUE 1
#define LSBFIRST 0
#define MSBFIRST 1

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

typedef uint8_t boolean;
typedef uint8_t byte;

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define clockCyclesPerMicrosecond() ( getCpuFrequency() / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

typedef unsigned int word;

#define bit(b) (1UL << (b))

void delay(uint32_t milliseconds);

/* Implemented in wiring.c */
void delayMicroseconds(unsigned int us);
unsigned long micros();
unsigned long millis();

void setDelayResolution(uint32_t milliseconds);

/* our interrupt APIs take pin numbers */
#define digitalPinToInterrupt(pin) pin

/* implemented in WInterrupts.c */
void attachInterrupt(uint8_t, void (*)(void), int mode);
void detachInterrupt(uint8_t);

void disablePinInterrupt(uint8_t pin);
void enablePinInterrupt(uint8_t pin);

void interrupts(void);
void noInterrupts(void);

void senddata(char* data, uint32_t size);
void putdata(char* data, uint32_t size);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#endif

void senddata(char* data);
void senddata(uint8_t *data, uint32_t size);
void senddata(const char *data, uint32_t size);

void putdata(char *data);
void putdata(uint8_t *data,  uint32_t size);
void putdata(const char* data, uint32_t size);

#endif
