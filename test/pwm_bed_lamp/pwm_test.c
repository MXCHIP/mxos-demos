/**
 ******************************************************************************
 * @file    pwm_test.c
 * @author  guidx
 * @version V1.0.0
 * @date    17-May-2019
 * @brief   MXOS application to test pwm interface!
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2019 MXCHIP Inc.
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
 *
 ******************************************************************************
 */

#include "mxos.h"

#define app_log(M, ...)             custom_log("App", M, ##__VA_ARGS__)

#define LED_PWM_R               MXOS_PWM_5
#define LED_PWM_G               MXOS_PWM_4
#define LED_PWM_B               MXOS_PWM_3
#define LED_PWM_C               MXOS_PWM_6
#define LED_PWM_C_UP            MXOS_PWM_6

#define LED_PWM_R_FREQUENCY     (10*1000)
#define LED_PWM_G_FREQUENCY     (10*1000)
#define LED_PWM_B_FREQUENCY     (10*1000)
#define LED_PWM_C_FREQUENCY     (1*1000)

#define LED_IO_C_UPDOWN         MXOS_GPIO_39

void led_breath( mhal_pwm_t pwm, uint32_t frequency, uint32_t cycle )
{
    float duty = 0;
    bool is_increase = true;
    uint32_t count = 0, i = 0;

    for(i = 0; i < cycle; i ++)
    {
        while(1)
        {
            mhal_pwm_open( pwm, frequency, duty );
            mhal_pwm_start( pwm );

            mos_msleep( 15 );

            if ( duty == 0 )
            {
                is_increase = true;
            }

            if ( duty == 100 )
            {
                is_increase = false;
            }

            if ( is_increase == true )
            {
                duty++;
            } else
            {
                duty--;
            }

            app_log("count = %ld, duty:%.2f", count, duty);

            if ( duty == 0 )
            {
                break;
            }
        }
    }

    mhal_pwm_open( pwm, frequency, 0 );
    mhal_pwm_start( pwm );

    return;
}

int main( void )
{
    app_log("pwm unit test!");

    // mhal_pwm_open( LED_PWM_R, LED_PWM_R_FREQUENCY, 80 );
    // mhal_pwm_start( LED_PWM_R );

    // mhal_pwm_open( LED_PWM_G, LED_PWM_G_FREQUENCY, 80 );
    // mhal_pwm_start( LED_PWM_G );

    mhal_pwm_open( LED_PWM_B, LED_PWM_B_FREQUENCY, 80 );
    mhal_pwm_start( LED_PWM_B );

    mhal_pwm_open( LED_PWM_C, LED_PWM_C_FREQUENCY, 80 );
    mhal_pwm_start( LED_PWM_C );

    mhal_gpio_open( LED_IO_C_UPDOWN, OUTPUT_PUSH_PULL );

    while ( 1 )
    {
        app_log("R-----------------");
        mos_msleep(3000);

//        app_log("R-----------------");
//        led_breath(LED_PWM_R, LED_PWM_R_FREQUENCY, 1);
//
//        app_log("G-----------------");
//        led_breath(LED_PWM_G, LED_PWM_G_FREQUENCY, 1);
//
//        app_log("B-----------------");
//        led_breath(LED_PWM_B, LED_PWM_B_FREQUENCY, 1);

//        app_log("C DOWN-----------------");
//        mhal_gpio_low( LED_IO_C_UPDOWN );
//        led_breath(LED_PWM_C, LED_PWM_C_FREQUENCY, 1);
//
//        app_log("C UP-----------------");
//        mhal_gpio_high( LED_IO_C_UPDOWN );
//        led_breath(LED_PWM_C, LED_PWM_C_FREQUENCY, 1);
    }

    return 0;
}
