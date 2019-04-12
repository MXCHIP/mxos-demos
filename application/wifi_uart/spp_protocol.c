/**
 ******************************************************************************
 * @file    SppProtocol.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   SPP protocol deliver any data received from UART to wlan and deliver
 * wlan data to UART.
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
#include "SocketUtils.h"

#define MAX_SOCK_MSG_LEN (10*1024)
int sockmsg_len = 0;


void socket_msg_take(socket_msg_t*msg);
void socket_msg_free(socket_msg_t*msg);


merr_t sppProtocolInit(app_context_t * const inContext)
{
  int i;
  
  (void)inContext;

  for(i=0; i < MAX_QUEUE_NUM; i++) {
    inContext->appStatus.socket_out_queue[i] = NULL;
  }
  inContext->appStatus.queue_mtx = mos_mutex_new();
  return kNoErr;
}

merr_t sppWlanCommandProcess(unsigned char *inBuf, int *inBufLen, int inSocketFd, app_context_t * const inContext)
{
  UNUSED_PARAMETER(inSocketFd);
  UNUSED_PARAMETER(inContext);
  merr_t err = kUnknownErr;

  err = mhal_uart_write(MXOS_UART_FOR_APP, inBuf, *inBufLen);

  *inBufLen = 0;
  return err;
}

merr_t sppUartCommandProcess(uint8_t *inBuf, int inLen, app_context_t * const inContext)
{
  merr_t err = kNoErr;
  int i;
  mos_queue_id_t p_queue = NULL;
  socket_msg_t *real_msg;

  for(i=0; i < MAX_QUEUE_NUM; i++) {
    p_queue = inContext->appStatus.socket_out_queue[i];
    if(p_queue  != NULL ){
      break;
    }
  }
  if (p_queue == NULL)
    return kNoErr;
  
  if (MAX_SOCK_MSG_LEN < sockmsg_len)
    return kNoMemoryErr;
  real_msg = (socket_msg_t*)malloc(sizeof(socket_msg_t) - 1 + inLen);

  if (real_msg == NULL)
    return kNoMemoryErr;
  sockmsg_len += (sizeof(socket_msg_t) - 1 + inLen);
  real_msg->len = inLen;
  memcpy(real_msg->data, inBuf, inLen);
  real_msg->ref = 0;
  
  mos_mutex_lock(inContext->appStatus.queue_mtx);
  socket_msg_take(real_msg);
  for(i=0; i < MAX_QUEUE_NUM; i++) {
    p_queue = inContext->appStatus.socket_out_queue[i];
    if(p_queue  != NULL ){
      socket_msg_take(real_msg);
      if (kNoErr != mos_queue_push(p_queue, &real_msg, 0)) {
        socket_msg_free(real_msg);
    }
  }
  }        
  socket_msg_free(real_msg);
  mos_mutex_unlock(inContext->appStatus.queue_mtx);
  return err;
}

void socket_msg_take(socket_msg_t*msg)
{
    msg->ref++;
}

void socket_msg_free(socket_msg_t*msg)
{
    msg->ref--;
    if (msg->ref == 0) {
        sockmsg_len -= (sizeof(socket_msg_t) - 1 + msg->len);
        free(msg);
    
    }
}

int socket_queue_create(app_context_t * const inContext, mos_queue_id_t *queue)
{
    merr_t err;
    int i;
    mos_queue_id_t _queue, p_queue;
    *queue = NULL;
    
    _queue = mos_queue_new( MAX_QUEUE_LENGTH, sizeof(int) );
    if (_queue == kNoErr)
        return -1;

    mos_mutex_lock(inContext->appStatus.queue_mtx);

    for(i=0; i < MAX_QUEUE_NUM; i++) {
        p_queue = inContext->appStatus.socket_out_queue[i];
        if(p_queue == NULL ){
            inContext->appStatus.socket_out_queue[i] = _queue;
            *queue = _queue;
            mos_mutex_unlock(inContext->appStatus.queue_mtx);
            return 0;
        }
    }        
    mos_mutex_unlock(inContext->appStatus.queue_mtx);
    mos_queue_delete(_queue);
    return -1;
}

int socket_queue_delete(app_context_t * const inContext, mos_queue_id_t *queue)
{
    int i;
    socket_msg_t *msg;
    int ret = -1;
    mos_queue_id_t _queue = *queue;

    mos_mutex_lock(inContext->appStatus.queue_mtx);
    // remove queue
    for(i=0; i < MAX_QUEUE_NUM; i++) {
        if (_queue == inContext->appStatus.socket_out_queue[i]) {
            inContext->appStatus.socket_out_queue[i] = NULL;
            ret = 0;
        }
    }
    mos_mutex_unlock(inContext->appStatus.queue_mtx);
    // free queue buffer
    while(kNoErr == mos_queue_pop( _queue, &msg, 0)) {
        socket_msg_free(msg);
    }

    // deinit queue
    mos_queue_delete(_queue);
    *queue = NULL;
    
    return ret;
}

