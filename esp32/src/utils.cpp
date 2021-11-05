#include "Arduino.h"
#include "utils.h"

#define f 1000 //原正弦波频率

void addpoint1(float *data, float *res, uint16_t count)
{
    double num = count;
    double y = 0, num2 = 4;
    uint16_t n = 0;
    num2++;
    for (uint16_t u = 0; u <= num; u++)
    {
        if (u != num)
        {
            res[n] = data[n];
            n++;
            for (int k = 1; k < num2; k++)
            {
                double t = u * 1 / (num * f) + k / (num2 * f * num);
                for (int m = 0; m <= num; m++)
                {
                    y += data[m] * sin(PI * (t * num * f - m)) / (PI * (t * num * f - m));
                }
                res[n] = y;
                n++;
                y = 0;
            }
        }
    }
}