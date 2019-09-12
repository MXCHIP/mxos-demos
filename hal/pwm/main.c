/**
 ******************************************************************************
 * @file    pwm.c
 * @author  guidx
 * @version V1.0.0
 * @date    17-May-2019
 * @brief   PWM demo
 ******************************************************************************
 */

#include "mxos.h"

#define PWM_FREQ 10000

int main(void)
{
	int duty;

	mhal_pwm_open(MXKIT_PWM, 0, MXKIT_PWM_PIN);

	while (1)
	{
		mhal_pwm_set_freq(MXKIT_PWM, 1000);
		for (duty = 0; duty <= 100; duty++)
		{
			mhal_pwm_set_duty(MXKIT_PWM, duty);
			mos_msleep(20);
		}
		mhal_pwm_set_freq(MXKIT_PWM, 10000);
		for (duty = 100; duty >= 0; duty--)
		{
			mhal_pwm_set_duty(MXKIT_PWM, duty);
			mos_msleep(20);
		}
	}

	return 0;
}
