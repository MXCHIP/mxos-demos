/**
 ******************************************************************************
 * @file    i2c_test.c
 * @author  guidx
 * @version V1.0.0
 * @date    2-Feb-2019
 * @brief   First MXOS application to test i2c interface!
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

mxos_i2c_device_t mxos_i2c_test =
	{
		.port = MXOS_I2C_1,
		.address = 0x39,
		.address_width = I2C_ADDRESS_WIDTH_7BIT,
		.speed_mode = I2C_STANDARD_SPEED_MODE,};

int main(void)
{
	merr_t err = kNoErr;

	mxos_i2c_message_t i2c_message[100];

	uint8_t send_buf[1] = {0x00};
	uint8_t recv_buf[1] = {0x00};

	send_buf[0] = 0x12|0xA0;

	/* i2c init */
	err = mxos_i2c_init(&mxos_i2c_test);

	err = mxos_i2c_build_comb_msg(i2c_message, send_buf, recv_buf, 1, 1, 1);

	while (1)
	{
		err = mxos_i2c_transfer(&mxos_i2c_test,i2c_message,1);
		mos_sleep(1);
		printf("recv is %x\r\n",recv_buf[0]);
	}

exit:
	return 0;
}
