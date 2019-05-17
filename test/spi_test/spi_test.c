/**
 ******************************************************************************
 * @file    spi_test.c
 * @author  guidx
 * @version V1.0.0
 * @date    17-May-2019
 * @brief   MXOS application to test spi interface!
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

const mxos_spi_device_t mxos_spi_test =
	{
		.port = MXOS_SPI_1,
		.chip_select = MXOS_GPIO_30,
		.speed = 2400000,
		.mode = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_LOW | SPI_USE_DMA | SPI_MSB_FIRST),
		.bits = 8,};

merr_t _spi_recv_packet(uint8_t *data)
{
	merr_t err = kNoErr;
	mxos_spi_message_segment_t recv_one_message = {NULL, data, 7};

	err = mxos_spi_transfer(&mxos_spi_test, (const mxos_spi_message_segment_t *)&recv_one_message, 1);

	return err;
}

merr_t _spi_send_packet(uint8_t *data)
{
	merr_t err = kNoErr;
	mxos_spi_message_segment_t send_one_message = {data, NULL, 7};

	err = mxos_spi_transfer(&mxos_spi_test, (const mxos_spi_message_segment_t *)&send_one_message, 1);

	return err;
}


merr_t _spi_init(void)
{
	merr_t err = kNoErr;

	err = mxos_spi_init(&mxos_spi_test);

	return err;
}


int main(void)
{
	merr_t err = kNoErr;
	uint8_t send_buf[7] = {'S', 'P', 'I', 'T', 'E', 'S', 'T'};
	uint8_t recv_buf[7] = {0x00};

	/* spi init */
	err = _spi_init();

	while (1)
	{
		/* spi send */
		err = _spi_send_packet(send_buf);

		/* spi recv */
		err = _spi_recv_packet(recv_buf);

		mos_sleep(1);
	}

exit:
	return 0;
}
