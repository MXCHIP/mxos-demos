/**
 ******************************************************************************
 * @file    adc_test.c
 * @author  guidx
 * @version V1.0.0
 * @date    17-May-2019
 * @brief   MXOS application to use adc interface!
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

#define AD2MV(ad,offset) ((ad*3.3)/offset)	

int main(void)
{
	merr_t err = kNoErr;

	uint16_t adc_out;
	float voltage = 0;

	/* adc init */
	err = mxos_adc_init(MXOS_ADC_1,10000);

	while (1)
	{
		/* adc take sample */
		err = mxos_adc_take_sample(MXOS_ADC_1,&adc_out);

		voltage = AD2MV(adc_out,4096);//4096 2^12

		mos_sleep(1);

		printf("adc is %d:voltage is %.2f\r\n",adc_out,voltage);
	}

exit:
	return 0;
}
