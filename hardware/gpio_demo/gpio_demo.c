/**
 ******************************************************************************
 * @file    wifi_scan.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   MiCO scan wifi hot spots demo
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#include "mxos.h"

/* A1=41, A2=38, D15=40, D14=39, D13=29, D12=28, D11=27, D10=30, D7=33, D6=34*/
#define Arduino_GPIO_A1 MXOS_GPIO_41
#define Arduino_GPIO_A2 MXOS_GPIO_38
#define Arduino_GPIO_D15 MXOS_GPIO_40
#define Arduino_GPIO_D14 MXOS_GPIO_39
#define Arduino_GPIO_D13 MXOS_GPIO_29
#define Arduino_GPIO_D12 MXOS_GPIO_28
#define Arduino_GPIO_D11 MXOS_GPIO_27
#define Arduino_GPIO_D10 MXOS_GPIO_30
#define Arduino_GPIO_D7 MXOS_GPIO_33
#define Arduino_GPIO_D6 MXOS_GPIO_34
#define Arduino_GPIO_D1 MXOS_GPIO_37
#define Arduino_GPIO_D0 MXOS_GPIO_42

static const uint8_t gpio[] = {
    Arduino_GPIO_A1, Arduino_GPIO_A2,Arduino_GPIO_D15,Arduino_GPIO_D14,
    Arduino_GPIO_D13,Arduino_GPIO_D12,Arduino_GPIO_D11,Arduino_GPIO_D10,
    Arduino_GPIO_D7,Arduino_GPIO_D6, Arduino_GPIO_D1, Arduino_GPIO_D0
};
static uint8_t gpio_state[sizeof(gpio)];

static void gpio_irq( void* arg )
{
    int i=(int)arg;
    
    gpio_state[i] = 1;
}

int main( void )
{
    int i, j;
    
    
#if 0
    printf("init gpio\r\n");
    app_log("init all gpio as output");
    for(i=0; i<sizeof(gpio); i++) {
        mhal_gpio_open(gpio[i], OUTPUT_PUSH_PULL);
    }
    
    for(j=0;j<5;j++) {
        mos_sleep(5);
        printf("gpio output high\r\n");
        app_log("gpio output high");
        for(i=0; i<sizeof(gpio); i++) {
            mhal_gpio_high(gpio[i]);
        }

        mos_sleep(5);
        printf("gpio output low\r\n");
        app_log("gpio output low");
        for(i=0; i<sizeof(gpio); i++) {
            mhal_gpio_low(gpio[i]);
        }
    }
#endif
    memset(gpio_state, 0, sizeof(gpio_state));
    printf("init gpio as input\r\n");
    app_log("init all gpio as input");
    for(i=0; i<sizeof(gpio); i++) {
        mhal_gpio_open(gpio[i], INPUT_PULL_DOWN);
    }

    for(i=0;i<sizeof(gpio);i++) {
        mhal_gpio_int_on( gpio[i], IRQ_TRIGGER_RISING_EDGE, gpio_irq, i );
    }
    while(1) {
        for(i=0;i<sizeof(gpio_state);i++) {
            if (gpio_state[i] != 0) {
                gpio_state[i] = 0;
                printf("gpio %d changed: %d\r\n", i, mhal_gpio_value(gpio[i]));
            }
        }
        mos_msleep(2);
    }
    return kNoErr;
}

