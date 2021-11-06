#include "utils.h"
#include "math.h"
#include "Energia.h"

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

void addpoint1(float *data, float *res, uint16_t count, float f, uint16_t fix)
{
    float num = count;
    float y = 0, num2 = fix;
    uint16_t n = 0, n1 = 0;
    num2++;
    for (uint16_t u = 0; u <= num; u++, n1++)
    {
        res[n] = data[n1];
        n++;
        if (u != num)
        {
            for (int k = 1; k < num2; k++)
            {
                float t = u * 1 / (num * f) + k / (num2 * f * num);
                for (int m = 0; m <= num; m++)
                {
                    y += data[m] * sin(PI * (t * num * f - m)) / (PI * (t * num * f - m));
                }
                res[n] = y;
                n++;
                y = 0;
                if (n >= 240)
                    return;
            }
        }
    }
}

#define REF_LEN 5 //閸欏倽锟藉啯鏆熼幑顕�鏆辨惔锟�
#define INTER 4   //閹绘帒锟藉吋鐦笟锟�
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
