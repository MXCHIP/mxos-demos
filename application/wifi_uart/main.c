/**
 ******************************************************************************
 * @file    MICOAppEntrance.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   Mico application entrance, addd user application functons and threads.
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

volatile ring_buffer_t rx_buffer;
volatile uint8_t rx_data[UART_BUFFER_LENGTH];
app_context_t app_context;

int main( void )
{
    merr_t err = kNoErr;
    mxos_uart_config_t uart_config;

    mxos_Context_t* mxos_context;
    mos_thread_id_t thread;

    /* Create mico system context and read application's config data from flash */
    mxos_context = mxos_system_context_init();
    
    /* Create application context */
    memset( &app_context, 0x0, sizeof(app_context_t) );
    app_context.appConfig = app_contex_init();
    require( app_context.appConfig, exit );

    /* MXOS system initialize */
    err = mxos_system_init( mxos_context );
    require_noerr( err, exit );

    /* Protocol initialize */
    sppProtocolInit( &app_context );

    /*UART receive thread*/
    uart_config.baud_rate = app_context.appConfig->USART_BaudRate;
    uart_config.data_width = DATA_WIDTH_8BIT;
    uart_config.parity = NO_PARITY;
    uart_config.stop_bits = STOP_BITS_1;
    uart_config.flow_control = FLOW_CONTROL_DISABLED;
    uart_config.flags = ( mxos_context->mxosSystemConfig.mcuPowerSaveEnable == true )? UART_WAKEUP_ENABLE:UART_WAKEUP_DISABLE;

    ring_buffer_init( (ring_buffer_t *) &rx_buffer, (uint8_t *) rx_data, UART_BUFFER_LENGTH );
    mhal_uart_open( MXOS_UART_FOR_APP, &uart_config, (ring_buffer_t *) &rx_buffer );
    thread = mos_thread_new( MXOS_APPLICATION_PRIORITY, "UART Recv", uartRecv_thread,
                             STACK_SIZE_UART_RECV_THREAD, &app_context );
    require_string( thread, exit, "ERROR: Unable to start the uart recv thread." );

    /* Local TCP server thread */
    if ( app_context.appConfig->localServerEnable == true )
    {
        thread = mos_thread_new( MXOS_APPLICATION_PRIORITY, "Local Server", localTcpServer_thread,
                                 STACK_SIZE_LOCAL_TCP_SERVER_THREAD, &app_context );
        require_string( thread, exit, "ERROR: Unable to start the local server thread." );
    }

    /* Remote TCP client thread */
    if ( app_context.appConfig->remoteServerEnable == true )
    {
        thread = mos_thread_new( MXOS_APPLICATION_PRIORITY, "Remote Client", remoteTcpClient_thread,
                                 STACK_SIZE_REMOTE_TCP_CLIENT_THREAD, &app_context );
        require_string( thread, exit, "ERROR: Unable to start the remote client thread." );
    }

exit:
    return err;
}
