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

#define PWM_K_HZ (10 * 1000)
#define PWM_HZ (1000)

/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

// #include "device.h"
// #include "main.h"


#define PWM_TIMER	5
#define PWM_PERIOD  20000
#define PWM_PRESCALER	12
#define PWM_STEP    (PWM_PERIOD/20)

int pwms[4]={0, PWM_PERIOD/4, PWM_PERIOD/2, PWM_PERIOD/4*3};
int steps[4]={PWM_STEP,PWM_STEP,PWM_STEP,PWM_STEP};

#define TIM5				TIMM05
#define TIMx_irq			TIMx_irq_LP
#define PINMUX_FUNCTION_PWM_HS		PINMUX_FUNCTION_PWM_LP
u8 PWM_pin2chan[4][2]={
//pwm channel pin, channel num
	{_PA_12, 0},
	{_PA_13, 1},
	{_PB_4, 2},
	{_PB_5, 3}
};




void raw_pwm_demo(void)
{
	RTIM_TimeBaseInitTypeDef		RTIM_InitStruct;
	TIM_CCInitTypeDef		TIM_CCInitStruct;
	int pwm_chan=0;
	int i;

	RTIM_TimeBaseStructInit(&RTIM_InitStruct);
	RTIM_InitStruct.TIM_Idx = PWM_TIMER;
	RTIM_InitStruct.TIM_Prescaler = PWM_PRESCALER;
	RTIM_InitStruct.TIM_Period = PWM_PERIOD;

	RTIM_TimeBaseInit(TIM5, (&RTIM_InitStruct), TIMx_irq[5], NULL, NULL);

	RTIM_CCStructInit(&TIM_CCInitStruct);
	RTIM_CCxInit(TIM5, &TIM_CCInitStruct, PWM_pin2chan[0][1]);
	RTIM_CCRxSet(TIM5, pwms[0], PWM_pin2chan[0][1]);
	RTIM_CCxCmd(TIM5, PWM_pin2chan[0][1], TIM_CCx_Enable);

	Pinmux_Config(PWM_pin2chan[0][0], PINMUX_FUNCTION_PWM_HS);

	RTIM_Cmd(TIM5, 1);

	
	RTIM_CCRxSet(TIM5, pwms[0], PWM_pin2chan[0][1]);

}





int main(void)
{
	merr_t err = kNoErr;

	/* mxos pwm init*/


	err = mhal_pwm_open(MXOS_PWM_3, PWM_K_HZ, 100);

	err = mhal_pwm_open(MXOS_PWM_4, PWM_K_HZ, 100);

	err = mhal_pwm_open(MXOS_PWM_5, PWM_HZ, 100);

	// err = mhal_pwm_open(MXOS_PWM_6, PWM_HZ, 100);
	raw_pwm_demo();

	err = mhal_pwm_start(MXOS_PWM_3);

	err = mhal_pwm_start(MXOS_PWM_4);

	err = mhal_pwm_start(MXOS_PWM_5);

	// err = mhal_pwm_start(MXOS_PWM_6);

	system_log("hello");


	while (1)
	{
		mos_sleep(1.0);
	}
}
