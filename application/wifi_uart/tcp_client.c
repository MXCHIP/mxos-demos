/**
 ******************************************************************************
 * @file    RemoteTcpClient.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   Create a TCP client thread, and connect to a remote server.
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

#define CLOUD_RETRY  1

void tcp_client_thread(void* arg)
{
  merr_t err = kUnknownErr;
  int len;
  app_context_t *context = (app_context_t *)arg;
  struct sockaddr_in addr;
  fd_set readfds;
  fd_set writeSet;
  char ipstr[16];
  struct timeval t;
  int remoteTcpClient_fd = -1;
  uint8_t *inDataBuffer = NULL;
  int eventFd = -1;
  mos_queue_id_t queue;
  socket_msg_t *msg;
  int sent_len, errno;
  struct hostent* hostent_content = NULL;
  char **pptr = NULL;
  struct in_addr in_addr;

  
  inDataBuffer = malloc(wlanBufferLen);
  require_action(inDataBuffer, exit, err = kNoMemoryErr);
  
  while(1) {
    if(remoteTcpClient_fd == -1 ) {

      hostent_content = gethostbyname( (char *)context->appConfig->remoteServerDomain );
      require_action_quiet( hostent_content != NULL, exit, err = kNotFoundErr);
      pptr=hostent_content->h_addr_list;
      in_addr.s_addr = *(uint32_t *)(*pptr);
      strcpy( ipstr, inet_ntoa(in_addr));
      remoteTcpClient_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = inet_addr(ipstr);
      addr.sin_port = htons(context->appConfig->remoteServerPort);
      
      err = connect(remoteTcpClient_fd, (struct sockaddr *)&addr, sizeof(addr));
      require_noerr_quiet(err, ReConnWithDelay);
      client_log("Remote server connected at port: %d, fd: %d",  context->appConfig->remoteServerPort,
                 remoteTcpClient_fd);
      
      err = socket_queue_create(context, &queue);
      require_noerr( err, exit );
      eventFd = mos_event_fd_new(queue);
      if (eventFd < 0) {
        client_log("create event fd error");
        socket_queue_delete(context, &queue);
        goto ReConnWithDelay;
      }
    }else{
      FD_ZERO(&readfds);
      FD_SET(remoteTcpClient_fd, &readfds);
      FD_SET(eventFd, &readfds); 
      t.tv_sec = 4;
      t.tv_usec = 0;
      select( Max(remoteTcpClient_fd, eventFd) + 1, &readfds, NULL, NULL, &t);
      /* send UART data */
      if (FD_ISSET( eventFd, &readfds )) {// have data 
        FD_ZERO(&writeSet );
        FD_SET(remoteTcpClient_fd, &writeSet );
        t.tv_usec = 100*1000; // max wait 100ms.
        select(1 + remoteTcpClient_fd, NULL, &writeSet, NULL, &t);
        if ((FD_ISSET(remoteTcpClient_fd, &writeSet )) && 
            (kNoErr ==  mos_queue_pop( queue, &msg, 0))) {
           sent_len = write(remoteTcpClient_fd, msg->data, msg->len);
           if (sent_len <= 0) {
            len = sizeof(errno);
            getsockopt(remoteTcpClient_fd, SOL_SOCKET, SO_ERROR, &errno, (socklen_t *)&len);
      
            socket_msg_free(msg);
            if (errno != ENOMEM) {
                client_log("write error, fd: %d, errno %d", remoteTcpClient_fd,errno );
                goto ReConnWithDelay;
            }
           } else {
                    socket_msg_free(msg);
                }
            }
      }
      /*recv wlan data using remote client fd*/
      if (FD_ISSET(remoteTcpClient_fd, &readfds)) {
        len = recv(remoteTcpClient_fd, inDataBuffer, wlanBufferLen, 0);
        if(len <= 0) {
          client_log("Remote client closed, fd: %d", remoteTcpClient_fd);
          goto ReConnWithDelay;
        }
        sppWlanCommandProcess(inDataBuffer, &len, remoteTcpClient_fd, context);
      }

      continue;
      
    ReConnWithDelay:
        if (eventFd >= 0) {
          mos_event_fd_delete(eventFd);
          eventFd = -1;
          socket_queue_delete(context, &queue);
        }
        if(remoteTcpClient_fd != -1){
          SocketClose(&remoteTcpClient_fd);
        }
        mos_msleep(CLOUD_RETRY*1000);
    }
  }
    
exit:
  if(inDataBuffer) free(inDataBuffer);
  client_log("Exit: Remote TCP client exit with err = %d", err);
  mos_thread_delete(NULL);
  return;
}

