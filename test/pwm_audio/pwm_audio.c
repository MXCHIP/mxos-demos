/**
 ******************************************************************************
 * @file    pwm_audio.c
 * @author  yhb
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
#include "pcm.h"

#define app_log(M, ...)             custom_log("App", M, ##__VA_ARGS__)
#if 0
enum {
    TIMER0 = 0,
    TIMER1 = 1,
    TIMER2 = 2,
    TIMER3 = 3,

    GTIMER_MAX = 4
};

typedef struct gtimer_s {
 void *handler;
 uint32_t hid;
 uint8_t timer_id;
 uint8_t is_periodcal;
}gtimer_t;
    
#define AUDIO_PWM       MXOS_PWM_6
#define AUDIO_TIMER     TIMER3
#define PWM_DEFAULT_FREQ 32000
#define PWM_DEFAULT_DUTY 50

static gtimer_t gtimer;
static int current_i=0;
static float current_cycle = 50;
static int16_t *pcm_16 = (int16_t*)user_pcm;
static int pcm_16_size = sizeof(user_pcm)/2;

void timer_event(int arg)
{
    current_cycle = pcm_16[current_i]+32767;
    current_cycle /= 655.36;
    current_cycle =( current_cycle-50)*4 +50;

    mhal_pwm_config(AUDIO_PWM, PWM_DEFAULT_FREQ, current_cycle);
    current_i++;
    if (current_i >= pcm_16_size) {
        current_i = 0;
    }
}

static void pwma_audio_init(void)
{
    mhal_pwm_open( AUDIO_PWM, PWM_DEFAULT_FREQ, PWM_DEFAULT_DUTY );
    mhal_pwm_start( AUDIO_PWM );

    gtimer_init(&gtimer, AUDIO_TIMER);
    gtimer_start_periodical(&gtimer,50,timer_event,55);
}

int main( void )
{
    app_log("pwm audio start");

    pwma_audio_init();
    app_log("pcm size %d", sizeof(user_pcm));
    return 0;
}
#else

static int current_i=0;
static float current_cycle = 50;
static int16_t *pcm_16 = (int16_t*)user_pcm;
static int pcm_16_size = sizeof(user_pcm)/2;

static float cycle_update(void)
{
    current_cycle = pcm_16[current_i]+32767;
    current_cycle /= 655.36;
    
    current_cycle =( current_cycle-50)*4 +50; // volume inc

    current_i++;
    if (current_i >= pcm_16_size) {
        current_i = 0;
    }

    return current_cycle;
}

int main( void )
{
    app_log("pwm audio start");

    mhal_pwm_audio_start(MXOS_PWM_6, 16000, cycle_update);
    app_log("pcm size %d", sizeof(user_pcm));
    return 0;
}

#endif
