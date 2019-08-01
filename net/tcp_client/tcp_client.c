/**
 ******************************************************************************
 * @file    tcp_client.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   MiCO tcp client Demo
 ******************************************************************************
 *
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

#include "mico.h"
#include "SocketUtils.h"

#define tcp_client_log(M, ...) custom_log("TCP", M, ##__VA_ARGS__)

static char tcp_remote_ip[16] = "192.168.3.53"; /*remote ip address*/
static int tcp_remote_port = 6000; /*remote port*/
static mxos_semaphore_t wait_sem = NULL;

static void micoNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
    switch ( status )
    {
        case NOTIFY_STATION_UP:
            mxos_rtos_set_semaphore( &wait_sem );
            break;
        case NOTIFY_STATION_DOWN:
        case NOTIFY_AP_UP:
        case NOTIFY_AP_DOWN:
            break;
    }
}

/*when client connected wlan success,create socket*/
void tcp_client_thread( mxos_thread_arg_t arg )
{
    UNUSED_PARAMETER( arg );

    merr_t err;
    struct sockaddr_in addr;
    struct timeval t;
    fd_set readfds;
    int tcp_fd = -1, len;
    char *buf = NULL;

    buf = (char*) malloc( 1024 );
    require_action( buf, exit, err = kNoMemoryErr );

    tcp_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    require_action( IsValidSocket( tcp_fd ), exit, err = kNoResourcesErr );

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr( tcp_remote_ip );
    addr.sin_port = htons(tcp_remote_port);

    tcp_client_log( "Connecting to server: ip=%s  port=%d!", tcp_remote_ip,tcp_remote_port );
    err = connect( tcp_fd, (struct sockaddr *)&addr, sizeof(addr) );
    require_noerr( err, exit );
    tcp_client_log( "Connect success!" );
    
    t.tv_sec = 2;
    t.tv_usec = 0;
    
    while ( 1 )
    {
        FD_ZERO( &readfds );
        FD_SET( tcp_fd, &readfds );

        require_action( select( tcp_fd + 1, &readfds, NULL, NULL, &t) >= 0, exit,
                        err = kConnectionErr );

        /* recv wlan data, and send back */
        if ( FD_ISSET( tcp_fd, &readfds ) )
        {
            len = recv( tcp_fd, buf, 1024, 0 );
            require_action( len >= 0, exit, err = kConnectionErr );

            if ( len == 0 )
            {
                tcp_client_log( "TCP Client is disconnected, fd: %d", tcp_fd );
                goto exit;
            }

            tcp_client_log("Client fd: %d, recv data %d", tcp_fd, len);
            len = send( tcp_fd, buf, len, 0 );
            tcp_client_log("Client fd: %d, send data %d", tcp_fd, len);
        }
    }

    exit:
    if ( err != kNoErr ) tcp_client_log( "TCP client thread exit with err: %d", err );
    if ( buf != NULL ) free( buf );
    SocketClose( &tcp_fd );
    mos_thread_delete( NULL );
}



int application_start( void )
{
    merr_t err = kNoErr;

    mxos_rtos_init_semaphore( &wait_sem, 1 );

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED,
                                       (void *) micoNotify_WifiStatusHandler, NULL );
    require_noerr( err, exit );

    /* Start MiCO system functions according to mxos_config.h */
    err = mxos_system_init( system_context_init( 0 ) );
    require_noerr( err, exit );

    /* Wait for wlan connection*/
    mxos_rtos_get_semaphore( &wait_sem, mxos_WAIT_FOREVER );
    tcp_client_log( "wifi connected successful" );

    /* Start TCP client thread */
    err = mxos_rtos_create_thread( NULL, mxos_APPLICATION_PRIORITY, "TCP_client", tcp_client_thread, 0x800, 0 );
    require_noerr_string( err, exit, "ERROR: Unable to start the tcp client thread." );

    exit:
    if ( wait_sem != NULL )
        mxos_rtos_deinit_semaphore( &wait_sem );
    mxos_rtos_delete_thread( NULL );
    return err;
}

