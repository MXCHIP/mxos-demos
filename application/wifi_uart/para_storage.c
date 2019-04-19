/**
 ******************************************************************************
 * @file    para_storage.c
 * @author  William Xu
 * @version V1.0.0
 * @date    12-Apr-2019
 * @brief   Configuration parameters save&read on volatile storage.
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
#include "mkv.h"


static application_config_t app_config;

application_config_t* app_contex_init(void)
{
    merr_t err = kNoErr, kv_err;
    uint32_t configDataVer = 0;
    int n;
    application_config_t *p_config = &app_config;

    kv_err = mkv_init();
    require_noerr_action( kv_err, exit, err = kReadErr );

    CONTEXT_READ( kv_err, p_config, configDataVer );

    /* Restore default configuration */
    if ( kv_err == KV_ERR_NOT_FOUND || p_config->configDataVer != CONFIGURATION_VERSION ) {
        app_log( "Use default APP configuration!" );
        p_config->configDataVer = CONFIGURATION_VERSION;
        p_config->localServerPort = LOCAL_PORT;
        p_config->USART_BaudRate = 115200;
        p_config->tcp_client_enable = true;
        sprintf( p_config->remoteServerDomain, DEAFULT_REMOTE_SERVER );
        p_config->remoteServerPort = DEFAULT_REMOTE_SERVER_PORT;
    }
    else {
        CONTEXT_READ( kv_err, p_config, configDataVer );
        CONTEXT_READ( kv_err, p_config, localServerPort );
        CONTEXT_READ( kv_err, p_config, USART_BaudRate );
        CONTEXT_READ( kv_err, p_config, tcp_client_enable );
        CONTEXT_READ( kv_err, p_config, remoteServerDomain );
        CONTEXT_READ( kv_err, p_config, remoteServerPort );
    }

exit:
    if( err != kNoErr) return NULL;
    return &app_config;
}

static void app_contex_save(void)
{
    merr_t kv_err;
    application_config_t *p_config = &app_config;

    CONTEXT_SAVE( kv_err, p_config, configDataVer );
    CONTEXT_SAVE( kv_err, p_config, localServerPort );
    CONTEXT_SAVE( kv_err, p_config, USART_BaudRate );
    CONTEXT_SAVE( kv_err, p_config, tcp_client_enable );
    CONTEXT_SAVE( kv_err, p_config, remoteServerDomain );
    CONTEXT_SAVE( kv_err, p_config, remoteServerPort );

    UNUSED_PARAMETER(kv_err);
}

/* Config server callback: Current Device configuration sent to config client */
USED void config_server_delegate_report( json_object *app_menu, mxos_Context_t *in_context )
{
    merr_t err = kNoErr;

    // SPP protocol remote server connection enable
    err = config_server_create_bool_cell( app_menu, "Connect SPP Server", app_config.tcp_client_enable, "RW" );
    require_noerr( err, exit );

    //Server address cell
    err = config_server_create_string_cell( app_menu, "SPP Server", app_config.remoteServerDomain, "RW", NULL );
    require_noerr( err, exit );

    //Server port cell
    err = config_server_create_number_cell( app_menu, "SPP Server Port", app_config.remoteServerPort, "RW", NULL );
    require_noerr( err, exit );

    /*UART Baurdrate cell*/
    json_object *selectArray;
    selectArray = json_object_new_array( );
    require( selectArray, exit );
    json_object_array_add( selectArray, json_object_new_int( 9600 ) );
    json_object_array_add( selectArray, json_object_new_int( 19200 ) );
    json_object_array_add( selectArray, json_object_new_int( 38400 ) );
    json_object_array_add( selectArray, json_object_new_int( 57600 ) );
    json_object_array_add( selectArray, json_object_new_int( 115200 ) );
    err = config_server_create_number_cell( app_menu, "Baurdrate", app_config.USART_BaudRate, "RW", selectArray );
    require_noerr( err, exit );

    exit:
    return;
}

/* Config server callback: New Device configuration received from config client */
USED void config_server_delegate_recv( const char *key, json_object *value, bool *need_reboot,
                                       mxos_Context_t *in_context )
{
    if ( !strcmp( key, "Connect SPP Server" ) )
    {
        app_config.tcp_client_enable = json_object_get_boolean( value );
        *need_reboot = true;
    } else if ( !strcmp( key, "SPP Server" ) )
    {
        strncpy( app_config.remoteServerDomain, json_object_get_string( value ), 64 );
        *need_reboot = true;
    } else if ( !strcmp( key, "SPP Server Port" ) )
    {
        app_config.remoteServerPort = json_object_get_int( value );
        *need_reboot = true;
    } else if ( !strcmp( key, "Baurdrate" ) )
    {
        app_config.USART_BaudRate = json_object_get_int( value );
        *need_reboot = true;
    }

    app_contex_save();
}
