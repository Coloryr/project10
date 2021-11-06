#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <ti/sysbios/BIOS.h>

#include <ti/drivers/Board.h>

#include "FFT.h"
#include "Energia.h"
#include "printf.h"

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/ADCBuf.h>

/* Driver configuration */
#include "ti_drivers_config.h"
#include "ti/devices/msp432e4/driverlib/driverlib.h"

#include "ADC.h"

extern void *mainThread(void *arg0);
extern void *init(void *arg0);
extern void *led(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE 8192

uint32_t g_ui32SysClock;

UART_Handle uart;
UART_Params uartParams;

bool go = false;

void senddata(const char *data, uint32_t size)
{
    UART_write(uart, data, size);
}

void senddata(uint8_t *data, uint32_t size)
{
    UART_write(uart, data, size);
}

void senddata(char *data, uint32_t size)
{
    UART_write(uart, data, size);
}

void senddata(char *data)
{
    UART_write(uart, data, sizeof(data));
}

void putdata(uint8_t *data, uint32_t size)
{
    while (size--)
    {
        UARTCharPut(UART7_BASE, *data++);
        while (UARTBusy(UART7_BASE))
            ;
    }
}

void putdata(char *data)
{
    putdata((uint8_t *)data, strlen(data));
}

void putdata(char *data, uint32_t size)
{
    while (size--)
    {
        UARTCharPut(UART7_BASE, *data++);
        while (UARTBusy(UART7_BASE))
            ;
    }
}

void putdata(const char *data, uint32_t size)
{
    while (size--)
    {
        UARTCharPut(UART7_BASE, *data++);
        while (UARTBusy(UART7_BASE))
            ;
    }
}

void toesp(uint8_t *data, uint32_t size)
{
    while (size--)
    {
        UARTCharPut(UART6_BASE, *data++);
        while (UARTBusy(UART6_BASE))
            ;
    }
}

void toesp(char *data)
{
    toesp((uint8_t *)data, strlen(data));
}

void toesp(char *data, uint32_t size)
{
    while (size--)
    {
        UARTCharPut(UART6_BASE, *data++);
        while (UARTBusy(UART6_BASE))
            ;
    }
}

void toesp(const char *data, uint32_t size)
{
    while (size--)
    {
        UARTCharPut(UART6_BASE, *data++);
        while (UARTBusy(UART6_BASE))
            ;
    }
}

void error()
{
    while (1)
    {
        /* Turn on user LED */
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
        delay(200);
        /* Turn on user LED */
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
        delay(200);
    }
}

uint8_t pui8ControlTable[1024];

void usart()
{
    /* Call driver init functions */
    GPIO_init();
    UART_init();

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.baudRate = 115200;

    uart = UART_open(CONFIG_UART_0, &uartParams);

    if (uart == NULL)
    {
        /* UART_open() failed */
        error();
    }

    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480),
                                            120000000);

    /* Enable the clock to the GPIO Port A and wait for it to be ready */
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)))
        ;

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP)))
        ;

    GPIOPinConfigure(GPIO_PC4_U7RX);
    GPIOPinConfigure(GPIO_PC5_U7TX);

    GPIOPinConfigure(GPIO_PP0_U6RX);
    GPIOPinConfigure(GPIO_PP1_U6TX);

    MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    MAP_GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART7)))
        ;

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_UART6)))
        ;

    MAP_UARTConfigSetExpClk(UART7_BASE, g_ui32SysClock, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    MAP_UARTConfigSetExpClk(UART6_BASE, g_ui32SysClock, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
}

/*
 *  ======== main ========
 */
int main(void)
{
    pthread_t thread;
    pthread_attr_t attrs;
    struct sched_param priParam;
    int retc;

    Board_init();

    adcinit();

    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    MAP_GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_7);
    GPIOM->PUR |= GPIO_PIN_7;
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7, 1);

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs);

    /* Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 1;
    retc = pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0)
    {
        /* failed to set attributes */
        while (1)
        {
        }
    }

    retc = pthread_create(&thread, &attrs, init, NULL);
    if (retc != 0)
    {
        printf1("task fail\r");
        /* pthread_create() failed */
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
        while (1)
            ;
    }

    BIOS_start();

    return (0);
}

void *init(void *arg0)
{
    pthread_t thread;
    pthread_attr_t attrs;
    struct sched_param priParam;
    int retc;

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs);

    /* Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 1;
    retc = pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0)
    {
        /* failed to set attributes */
        error();
    }

    usart();
    retc = pthread_create(&thread, &attrs, mainThread, NULL);
    retc = pthread_create(&thread, &attrs, led, NULL);
    pthread_exit(0);
}

union testT
{
    int16_t u16;
    uint8_t u8[2];
};

union testF
{
    float f;
    uint8_t u8[4];
};

void sendonce()
{
    uint16_t i;
    unsigned long time = millis();
    testT test;
    testF test1;

    toesp("start:32768");
    delay(200);

    for (i = 0; i < 4096; i++)
    {
        test.u16 = (int16_t)(vReal[i] * 60);
        toesp(test.u8, 2);
    }

    test1.f = THD;
    toesp(test1.u8, 4);

    for (i = 0; i < 5; i++)
    {
        test1.f = range[i];
        toesp(test1.u8, 4);
    }

    test1.f = baseFrequency[0];
    toesp(test1.u8, 4);

    printf1("%d", millis() - time);
    printf1(" ms\r");
}

void *led(void *arg0)
{
    for (;;)
    {
        /* Turn on user LED */
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
        delay(1000);
        /* Turn on user LED */
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
        delay(1000);
    }
}

void *mainThread(void *arg0)
{
    for (;;)
    {
        if (GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_7) == 0)
        {
            adcstart();
        }
        if (go)
        {
            ffttest();
            sendonce();
            go = false;
        }
        delay(50);
    }
}
