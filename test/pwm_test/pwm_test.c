/**
 ******************************************************************************
 * @file    hello_world.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    2-Feb-2019
 * @brief   First MXOS application to say hello world!
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
#define PWM_HZ (100)

int main(void)
{

	printf("start pwm init!\r\n");
	/* mxos pwm init*/
	mxos_pwm_init(MXOS_PWM_6, PWM_K_HZ, 80);

	mxos_pwm_start(MXOS_PWM_6);

	mxos_pwm_stop(MXOS_PWM_6);

	/* Toggle mxos system led available on most mxosKits */
	while (1)
	{
		mos_sleep(1.0);
	}
}
