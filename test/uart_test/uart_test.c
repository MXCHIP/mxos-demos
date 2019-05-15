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

#define UART_BUF_LENGTH (2048)
#define UART_RECV_LENGTH (1024)
#define UART_RECV_TIMEOUT (1000)

volatile ring_buffer_t rx_buffer;
volatile uint8_t rx_data[UART_BUF_LENGTH];

mxos_uart_config_t usr_uart;

uint8_t recv_buf[1024] = {0x00};

uint32_t recv_len = 0;

uint32_t _uart_get_one_packet(uint8_t *inBuf, int inBufLen)
{

	uint32_t datalen;
	while (1)
	{
		if (mhal_uart_read(MXOS_UART_2, inBuf, inBufLen, UART_RECV_TIMEOUT) == kNoErr)
		{
			printf("return\r\n");
			return inBufLen;
		}
		else
		{
			datalen = mhal_uart_readd_data_len(MXOS_UART_2); //  printf("datalen is %d\r\n",datalen);
			if (datalen)
			{
				mhal_uart_read(MXOS_UART_2, inBuf, datalen, UART_RECV_TIMEOUT);
				return datalen;
			}
		}
	}
}

merr_t uart_init(uint32_t baudrate)
{
	merr_t err = kNoErr;

	usr_uart.baud_rate = baudrate;
	usr_uart.data_width = DATA_WIDTH_8BIT;
	usr_uart.parity = NO_PARITY;
	usr_uart.stop_bits = STOP_BITS_1;
	usr_uart.flow_control = FLOW_CONTROL_DISABLED;

	ring_buffer_init((ring_buffer_t *)&rx_buffer, (uint8_t *)rx_data, UART_BUF_LENGTH);

	err = mhal_uart_open(MXOS_UART_2, &usr_uart, &rx_buffer);

	return err;
}

int main(void)
{
	merr_t err = kNoErr;

	err = uart_init(9600);

	uint32_t recv_length = 0;



	/* Toggle mxos system led available on most mxosKits */
	while (1)
	{
		recv_len = _uart_get_one_packet(recv_buf, UART_RECV_LENGTH);

		printf("recv data length is :%d\r\n",recv_len);

		printf("device->mcu:");
		for(int i = 0;i < recv_len;i++)
		{
			printf("%02X ",recv_buf[i]);
		}
		printf("\r\n");

		recv_length += recv_len;

		if (recv_len <= 0)
		{
			continue;
		}
		else
		{
			err  = mhal_uart_write(MXOS_UART_2, recv_buf, recv_len);

			printf("mcu->device:");
			for(int i = 0;i < recv_len;i++)
			{
				printf("%02X ",recv_buf[i]);
			}
			printf("\r\n");
		}
		printf("uart test recv data length is :%d\r\n",recv_length);
	}

exit:
	return 0;
}
