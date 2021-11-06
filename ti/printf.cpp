/*
 * printf library for the MSP432
 *
 * Largely taken from and inspired from:
 *  http://www.msp430launchpad.com/2012/06/using-printf.html
 *  http://www.43oh.com/forum/viewtopic.php?f=10&t=1732
 *
 * See http://www.samlewis.me for an example implementation.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "Energia.h"

void printf1(char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);
    if (len < 0)
    {
        va_end(arg);
        return;
    };
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            va_end(arg);
            return;
        }
        len = vsnprintf(temp, len + 1, format, arg);
    }
    va_end(arg);
    senddata(temp, len);
    if (temp != loc_buf)
    {
        free(temp);
    }
}

void printf2(char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);
    if (len < 0)
    {
        va_end(arg);
        return;
    };
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            va_end(arg);
            return;
        }
        len = vsnprintf(temp, len + 1, format, arg);
    }
    va_end(arg);
    putdata(temp, len);
    if (temp != loc_buf)
    {
        free(temp);
    }
}
