#include <stdint.h>
#include <stdio.h>
#include <mqueue.h>

/* Driver Header files */
#include <ti/drivers/ADCBuf.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include <ti/devices/msp432e4/driverlib/driverlib.h>

#include "FFT.h"
#include "ADC.h"
#include "Energia.h"

#define ADCSAMPLESIZE (1024)

uint16_t sampleBufferOne[ADCSAMPLESIZE];
uint16_t sampleBufferTwo[ADCSAMPLESIZE];

uint8_t a = 0, b = 0, c = 0, d = 0;

const float refVoltage = 3.3f;

uint_fast16_t index = 0, offset = 0;

void adcBufCallback(ADCBuf_Handle handle, ADCBuf_Conversion *conversion,
                    void *completedADCBuffer, uint32_t completedChannel, int_fast16_t status)
{
    uint_fast16_t i = 0;
    //    if(a == 0)
    //    {
    //        MAP_uDMAChannelTransferSet(UDMA_CH14_ADC0_0 | UDMA_PRI_SELECT,
    //                                UDMA_MODE_PINGPONG,
    //                                (void *)&ADC0->SSFIFO2,
    //                                c == 0? sampleBufferOne[0] : sampleBufferOne[1],
    //                                1024);
    //        c == 0? 1:0;
    //        a = 1;
    //    }
    //    else
    //    {
    //        if(a == 0)
    //            {
    //                MAP_uDMAChannelTransferSet(UDMA_CH14_ADC0_0 | UDMA_PRI_SELECT,
    //                                        UDMA_MODE_PINGPONG,
    //                                        (void *)&ADC0->SSFIFO2,
    //                                        d == 0? sampleBufferTwo[0] : sampleBufferTwo[1],
    //                                        1024);
    //                d == 0? 1:0;
    //                a=0;
    //            }
    //    }

    /* Adjust raw adc values and convert them to microvolts */

    offset = index * 1024;
    uint16_t *adjustedRawSampleBuf = (uint16_t *)completedADCBuffer;
    for (i = 0; i < ADCSAMPLESIZE; i++)
    {
        if (adjustedRawSampleBuf[i] == 0x800)
        {
            vReal[i + offset] = 0;
        }
        else
        {
            vReal[i + offset] = ((refVoltage * (adjustedRawSampleBuf[i] - 0x800)) / 0x800);
        }
    }
    index++;
    if (index == 16)
    {
        adcstop();
        go = true;
        index = 0;
    }
}

ADCBuf_Handle adcBuf;
ADCBuf_Conversion continuousConversion;

/*
 *  ======== mainThread ========
 */
void adcinit()
{
    ADCBuf_Params adcBufParams;

    /* Call driver init functions */
    ADCBuf_init();

    /* Set up an ADCBuf peripheral in ADCBuf_RECURRENCE_MODE_CONTINUOUS */
    ADCBuf_Params_init(&adcBufParams);
    adcBufParams.callbackFxn = adcBufCallback;
    adcBufParams.recurrenceMode = ADCBuf_RECURRENCE_MODE_CONTINUOUS;
    adcBufParams.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
    adcBufParams.samplingFrequency = samplingFrequency;
    adcBuf = ADCBuf_open(0, &adcBufParams);

    /* Configure the conversion struct */
    continuousConversion.arg = NULL;
    continuousConversion.adcChannel = 0;
    continuousConversion.sampleBuffer = sampleBufferOne;
    continuousConversion.sampleBufferTwo = sampleBufferTwo;
    continuousConversion.samplesRequestedCount = ADCSAMPLESIZE;

    if (adcBuf == NULL)
    {
        printf("ADCBuf failed to open.\n");
        /* ADCBuf failed to open. */
        while (1)
            ;
    }
}

void adcstart()
{
    /* Start converting. */
    if (ADCBuf_convert(adcBuf, &continuousConversion, 1) !=
        ADCBuf_STATUS_SUCCESS)
    {
        /* Did not start conversion process correctly. */
        printf("Did not start conversion process correctly.\n");
        while (1)
            ;
    }
}

void adcstop()
{
    ADCBuf_convertCancel(adcBuf);
}
