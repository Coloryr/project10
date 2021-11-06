#include "Energia.h"

#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/family/arm/lm4/Timer.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432E4.h>

#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPIMSP432E4DMA.h>

#include <ti/devices/msp432e4/driverlib/rom.h>
#include <ti/devices/msp432e4/driverlib/rom_map.h>

static Timer_Handle clockTimer = 0;
static uint32_t clockTimerFreq = 0;

/*
 *  ======== micros ========
 */
unsigned long micros(void)
{
    uint32_t key;
    Types_FreqHz freq;
    uint64_t micros, expired;

    if (clockTimer == 0)
    {
        clockTimer = Timer_getHandle(Clock_timerId);
        Timer_getFreq(clockTimer, &freq);
        clockTimerFreq = freq.lo;
    }

    key = Hwi_disable();

    micros = Clock_getTicks() * Clock_tickPeriod;
    /* capture timer ticks since last Clock tick */
    expired = Timer_getExpiredCounts(clockTimer);

    Hwi_restore(key);

    micros += (expired * 1000000) / clockTimerFreq;

    return (micros);
}

/*
 *  ======== millis ========
 */
unsigned long millis(void)
{
    return (Clock_getTicks());
}

/*
 *  ======== delayMicroseconds ========
 *  Delay for the given number of microseconds.
 */
void delayMicroseconds(unsigned int us)
{
    if (us < 7)
    {
        //The overhead in calling and returning from this function takes about 6us
    }
    else if (us <= 20)
    {
        int time;
        for (time = 5 * (us - 6); time > 0; time--)
        {
            asm("   nop");
        }
    }
    else if (us < 70)
    {
        int time;
        for (time = 5 * us; time > 0; time--)
        {
            asm("   nop");
        }
    }
    else
    {
        uint32_t t0, deltaT;
        Types_FreqHz freq;

        Timestamp_getFreq(&freq);
        deltaT = us * (freq.lo / 1000000);

        t0 = Timestamp_get32();

        while ((Timestamp_get32() - t0) < deltaT)
        {
            ;
        }
    }
}

/*
 *  ======== delay ========
 */
void delay(uint32_t milliseconds)
{
    if (milliseconds == 0)
    {
        Task_yield();
        return;
    }

    /* timeout is always in milliseconds so that Clock_workFunc() behaves properly */
    Task_sleep(milliseconds);
}

/*
 *  ======== NDK_hookInit ========
 */
/*
 * Provide a weakly defined version of the NDK's Task register hook function
 * for when an application doesn't use NDK and doesn't link with the NDK
 * libraries.
 */
#ifdef __cplusplus
extern "C"
{
#endif
    void NDK_hookInit(int) __attribute__((weak));
    void NDK_hookInit(int x)
    {
    }
#ifdef __cplusplus
}
#endif /* __cplusplus */
