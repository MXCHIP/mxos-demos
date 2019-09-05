/**
 ******************************************************************************
 * @file    pwm.c
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

#define app_log(M, ...)       MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

int main(void)
{
	merr_t err = kNoErr;
	int value;

	/* mxos gpio init*/
    mhal_gpio_open(MODULE_PIN_8,OUTPUT_PUSH_PULL);
    mhal_gpio_open(MODULE_PIN_7,INPUT_PULL_DOWN);

	mhal_gpio_high(MODULE_PIN_8);

	while (1)
	{
		value = mhal_gpio_value(MODULE_PIN_7);
		mos_sleep(1.0);
		app_log("gpio value is %d",value);
	}
}
