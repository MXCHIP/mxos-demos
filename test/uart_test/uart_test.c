/**
 ******************************************************************************
 * @file    uart_test.c
 * @author  guidx
 * @version V1.0.0
 * @date    17-May-2019
 * @brief   MXOS application to use uart interface!
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

#define UART_RECV_LENGTH 	(512)
#define UART_RECV_TIMEOUT 	(100)

uint8_t recv_buf[UART_RECV_LENGTH] = {0x00};

uint32_t recv_len = 0;

uint32_t _uart_get_one_packet(uint8_t *inBuf, int inBufLen)
{

	uint32_t datalen;
	int err = 0;
	while (1)
	{
		// printf("datalen is %d\r\n",datalen);
		if (mhal_uart_read_buf(MXOS_UART_FOR_APP, inBuf, inBufLen, UART_RECV_TIMEOUT) == kNoErr)
		{
			
			return inBufLen;
		}
		else
		{
			datalen = mhal_uart_recved_len(MXOS_UART_FOR_APP); //  printf("datalen is %d\r\n",datalen);
			
			if (datalen)
			{
				printf("datalen is %d\r\n",datalen);
				err = mhal_uart_read_buf(MXOS_UART_FOR_APP, inBuf, datalen, UART_RECV_TIMEOUT);
				printf("data is %x err:%d\r\n",inBuf[0],err);
				return datalen;
			}
		}
	}
}


int main(void)
{
	merr_t err = kNoErr;

	err = mhal_uart_open(MXOS_UART_FOR_APP, 921600,1024,NULL);
	
	while (1)
	{
		recv_len = _uart_get_one_packet(recv_buf, UART_RECV_LENGTH);

		if (recv_len <= 0)
		{
			continue;
		}
		else
		{
			err = mhal_uart_write(MXOS_UART_FOR_APP, recv_buf, recv_len);
		}
	}

exit:
	return 0;
}
