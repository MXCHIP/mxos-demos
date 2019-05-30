/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */


#include "mico.h"
#include "bt_packet_internal.h"
#include "bt_transport_driver.h"
#include "bt_transport_thread.h"

/******************************************************
 *                      Macros
 ******************************************************/

#ifdef DEBUG
#define DUMP_PACKET( direction, start, end )                        \
do                                                                  \
{                                                                   \
    if ( bt_transport_enable_packet_dump == mxos_TRUE )             \
    {                                                               \
        uint8_t* current = start;                                   \
        if ( direction == 0 )                                       \
            WPRINT_LIB_INFO(( "\n[BT Transport] TX -----\n" ));     \
        else                                                        \
            WPRINT_LIB_INFO(( "\n[BT Transport] RX -----\n" ));     \
        while ( current != end )                                    \
        {                                                           \
            WPRINT_LIB_INFO(( "%.2X ", (int)*current++ ));          \
            if ( ( current - start ) % 16 == 0 )                    \
            {                                                       \
                WPRINT_LIB_INFO(( "\n" ));                          \
            }                                                       \
        }                                                           \
        WPRINT_LIB_INFO(( "\n-----------------------\n" ));         \
    }                                                               \
}                                                                   \
while (0)
#else
#define DUMP_PACKET( direction, start, end )
#endif

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static merr_t bt_transport_thread_send_packet_handler   ( void* arg );
static merr_t bt_transport_thread_receive_packet_handler( void* arg );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static mxos_worker_thread_t                         bt_transport_thread;
static mxos_bool_t                                  bt_transport_thread_initialised      = mxos_FALSE;
static bt_transport_thread_received_packet_handler_t bt_transport_received_packet_handler = NULL;
#ifdef DEBUG
static mxos_bool_t                                    bt_transport_enable_packet_dump      = mxos_FALSE;
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/

merr_t bt_transport_thread_init( bt_transport_thread_received_packet_handler_t handler )
{
    merr_t result;

    if ( bt_transport_thread_initialised == mxos_TRUE )
    {
        return mxos_BT_SUCCESS;
    }

    bt_transport_received_packet_handler = handler;

    /* Create MPAF worker thread */
    result = mxos_rtos_create_worker_thread( &bt_transport_thread, BT_TRANSPORT_THREAD_PRIORITY, BT_TRANSPORT_STACK_SIZE, BT_TRANSPORT_QUEUE_SIZE );
    if ( result == kNoErr )
    {
        return mxos_BT_SUCCESS;
    }
    else
    {
        check_string( 0!=0, "Error creating BT transport thread" );
        bt_transport_received_packet_handler = NULL;
        return result;
    }
}

merr_t bt_transport_thread_deinit( void )
{
    if ( bt_transport_thread_initialised == mxos_FALSE )
    {
        return mxos_BT_SUCCESS;
    }

    /* Deliberately not checking for result. Deinit always assumed successful */
    mxos_rtos_delete_worker_thread( &bt_transport_thread );
    bt_transport_received_packet_handler = NULL;
    bt_transport_thread_initialised      = mxos_FALSE;

    return mxos_BT_SUCCESS;
}

merr_t bt_transport_thread_send_packet( bt_packet_t* packet )
{
    if ( packet == NULL )
    {
        return mxos_BT_BADARG;
    }

    if ( mxos_rtos_is_current_thread( &bt_transport_thread.thread ) == kNoErr )
    {
        return bt_transport_thread_send_packet_handler( (void*)packet );
    }
    else
    {
        return mos_worker_send_async_event( &bt_transport_thread, bt_transport_thread_send_packet_handler, (void*)packet );
    }
}

merr_t bt_transport_thread_notify_packet_received( void )
{
    return mos_worker_send_async_event( &bt_transport_thread, bt_transport_thread_receive_packet_handler, NULL );
}

merr_t bt_transport_thread_execute_callback( bt_transport_thread_callback_handler_t callback_handler, void* arg )
{
    return mos_worker_send_async_event( &bt_transport_thread, callback_handler, arg );
}

merr_t bt_transport_thread_enable_packet_dump( void )
{
#ifdef DEBUG
    bt_transport_enable_packet_dump = mxos_TRUE;
    return kNoErr;
#else
    return mxos_BT_UNSUPPORTED;
#endif
}

merr_t bt_transport_thread_disable_packet_dump( void )
{
#ifdef DEBUG
    bt_transport_enable_packet_dump = mxos_FALSE;
    return kNoErr;
#else
    return mxos_BT_UNSUPPORTED;
#endif
}

static merr_t bt_transport_thread_send_packet_handler( void* arg )
{
    bt_packet_t* packet = (bt_packet_t*) arg;

    DUMP_PACKET( 0, packet->packet_start, packet->data_end );

    return bt_transport_driver_send_packet( packet );
}

static merr_t bt_transport_thread_receive_packet_handler( void* arg )
{
    bt_packet_t*   packet = NULL;
    merr_t result = bt_transport_driver_receive_packet( &packet );

    check_string( bt_transport_received_packet_handler != NULL, "bt_transport_received_packet_handler isn't set" );

    UNUSED_PARAMETER( arg );

    if ( result == mxos_BT_SUCCESS && packet != NULL )
    {
        DUMP_PACKET( 1, packet->packet_start, packet->data_end );
        result = bt_transport_received_packet_handler( packet );
    }

    return result;
}
