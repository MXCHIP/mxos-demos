/**
 ******************************************************************************
 * @file    main.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   Application entrance.
 ******************************************************************************
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
 ******************************************************************************
 */

#include "main.h"
#include "spp_protocol.h"

/* Application's context data, variables */
app_context_t app_context;


merr_t app_uart_init(uint32_t baudrate)
{
    merr_t err = kNoErr;

    /* Initialize uart driver */
    err = mhal_uart_open( MXOS_UART_FOR_APP, baudrate, 1024, NULL );
    require_noerr( err, exit );

exit:
    return err;
}



int main( void )
{
    merr_t err = kNoErr;
    mos_thread_id_t thread;

    /* MXOS system initialize */
    err = mxos_system_init( );
    require_noerr( err, exit );
    
    /* Create application context */
    memset( &app_context, 0x0, sizeof(app_context_t) );
    app_context.appConfig = app_contex_init();
    require( app_context.appConfig, exit );

    /* Protocol initialize */
    sppProtocolInit( &app_context );

    /* Initialise uart driver */
    err = app_uart_init( app_context.appConfig->USART_BaudRate );
    require_noerr( err, exit );

    /* Create UART receive thread*/
    thread = mos_thread_new( MOS_APPLICATION_PRIORITY, "UART Recv", uart_recv_thread, 
                             STACK_SIZE_UART_RECV_THREAD, &app_context );
    require_string( thread, exit, "ERROR: Unable to start the uart recv thread." );

    /* Local TCP server thread */
    thread = mos_thread_new( MOS_APPLICATION_PRIORITY, "TCP Server", tcp_server_thread,
                             STACK_SIZE_TCP_SERVER_THREAD, &app_context );
    require_string( thread, exit, "ERROR: Unable to start the tcp server thread." );

    /* Remote TCP client thread, maybe disabled by configuration */
    if ( app_context.appConfig->tcp_client_enable == true )
    {
        thread = mos_thread_new( MOS_APPLICATION_PRIORITY, "TCP Client", tcp_client_thread,
                                 STACK_SIZE_TCP_CLIENT_THREAD, &app_context );
        require_string( thread, exit, "ERROR: Unable to start the tcp client thread." );
    }

exit:
    return err;
}
