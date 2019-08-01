/**
 ******************************************************************************
 * @file    UartRecv.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file create a UART recv thread.
 ******************************************************************************
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
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
 ******************************************************************************
 */

#include "main.h"
#include "spp_protocol.h"


static size_t _uart_get_one_packet(uint8_t* buf, int maxlen);

void uart_recv_thread(void* arg)
{
  app_context_t *Context = (app_context_t *)arg;
  int recvlen;
  uint8_t *inDataBuffer;
  
  inDataBuffer = malloc(UART_ONE_PACKAGE_LENGTH);
  require(inDataBuffer, exit);
  
  while(1) {
    recvlen = _uart_get_one_packet(inDataBuffer, UART_ONE_PACKAGE_LENGTH);
    if (recvlen <= 0)
      continue; 
    sppUartCommandProcess(inDataBuffer, recvlen, Context);
  }
  
exit:
  if(inDataBuffer) free(inDataBuffer);
  mos_thread_delete(NULL);
}

/* Packet format: BB 00 CMD(2B) Status(2B) datalen(2B) data(x) checksum(2B)
* copy to buf, return len = datalen+10
*/
size_t _uart_get_one_packet(uint8_t* inBuf, int inBufLen)
{
  
  mhal_uart_read( MXOS_UART_FOR_APP, inBuf, &inBufLen, UART_RECV_TIMEOUT);
      return inBufLen;
  
}


