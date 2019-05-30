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
#include "bt_bus.h"
#include "bt_transport_driver.h"
#include "LinkListUtils.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* Driver thread priority is set to 1 higher than BT transport thread */
#define BT_UART_THREAD_PRIORITY MOS_NETWORK_WORKER_PRIORITY - 2
#define BT_UART_THREAD_NAME     "BT UART"
#define BT_UART_STACK_SIZE      600
#define BT_UART_PACKET_TYPE     0x0A

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

static void bt_transport_driver_uart_thread_main  ( void* arg );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static bt_transport_driver_event_handler_t    driver_event_handler    = NULL;
static bt_transport_driver_bus_read_handler_t driver_bus_read_handler = NULL;
static volatile mxos_bool_t                   driver_initialised      = mxos_FALSE;
static volatile mxos_bool_t                   uart_thread_running     = mxos_FALSE;
static mxos_thread_t                          uart_thread;
static mxos_mutex_t                           packet_list_mutex;
static linked_list_t                          uart_rx_packet_list;

/******************************************************
 *               Function Definitions
 ******************************************************/

merr_t bt_transport_driver_init( bt_transport_driver_event_handler_t event_handler, bt_transport_driver_bus_read_handler_t bus_read_handler )
{
    merr_t result;
    mxos_bool_t   ready;

    if ( event_handler == NULL || bus_read_handler == NULL )
    {
        return mxos_BT_BADARG;
    }

    if ( driver_initialised == mxos_TRUE )
    {
        return mxos_BT_SUCCESS;
    }

    /* Check if bus is ready */
    ready = bt_mfg_bus_is_ready( );
    require_action_string( ready == mxos_TRUE, error, result = mxos_BT_BUS_UNINITIALISED, "BT bus is NOT ready" );

    driver_initialised = mxos_TRUE;

    /* Create a linked list to hold packet momentarily before being passed to
     * the transport thread.
     */
    result = linked_list_init( &uart_rx_packet_list );
    require_noerr_string(result, error, "Error creating UART RX packet linked list");

    /* Create a semaphore. Once set, this semaphore is used to notify the UART
     * thread that the packet has been read by the upper layer. The UART thread
     * can now continue polling for another packet from the UART circular buffer.
     */
    result = mxos_rtos_init_mutex( &packet_list_mutex );
    require_noerr_string(result, error, "Error creating UART driver mutex");

    /* Create UART thread. WICED UART API does not support callback mechanism.
     * The API blocks in semaphore until the transmission is complete.
     * Consequently, a dedicated thread is required to recieve and dispatch
     * incoming packets to the upper layer.
     */
    uart_thread_running     = mxos_TRUE;
    driver_event_handler    = event_handler;
    driver_bus_read_handler = bus_read_handler;

    result = mxos_rtos_create_thread( &uart_thread, BT_UART_THREAD_PRIORITY, BT_UART_THREAD_NAME, bt_transport_driver_uart_thread_main, BT_UART_STACK_SIZE, NULL );
    require_noerr_string(result, error, "Error creating UART driver thread");

    return result;

    error:
    bt_transport_driver_deinit();
    return result;
}

merr_t bt_transport_driver_deinit( void )
{
    if ( driver_initialised == mxos_FALSE )
    {
        return mxos_BT_SUCCESS;
    }

    uart_thread_running = mxos_FALSE;
    mxos_rtos_delete_thread( &uart_thread );
    mxos_rtos_deinit_mutex( &packet_list_mutex );
    linked_list_deinit( &uart_rx_packet_list );
    driver_event_handler    = NULL;
    driver_bus_read_handler = NULL;
    driver_initialised      = mxos_FALSE;
    return mxos_BT_SUCCESS;
}

merr_t bt_transport_driver_send_packet( bt_packet_t* packet )
{
    merr_t result;

    result = bt_mfg_bus_transmit( packet->packet_start, (uint32_t)(packet->data_end - packet->packet_start) );
    require_noerr_string(result, exit, "Error transmitting MPAF packet");

    /* Destroy packet */
    result = bt_packet_pool_free_packet( packet );
exit:
    return result;

}

merr_t bt_transport_driver_receive_packet( bt_packet_t** packet )
{
    uint32_t            count;
    linked_list_node_t* node;
    merr_t            result;

    linked_list_get_count( &uart_rx_packet_list, &count );

    if ( count == 0 )
    {
        return mxos_BT_PACKET_POOL_EXHAUSTED;
    }

    mxos_rtos_lock_mutex( &packet_list_mutex );

    result = linked_list_remove_node_from_front( &uart_rx_packet_list, &node );
    if ( result == mxos_BT_SUCCESS )
    {
        *packet = (bt_packet_t*)node->data;
    }

    mxos_rtos_unlock_mutex( &packet_list_mutex );
    return result;
}

static void bt_transport_driver_uart_thread_main( void* arg )
{
    check_string( driver_bus_read_handler != NULL, "driver_bus_read_handler isn't set" );
    check_string( driver_event_handler    != NULL, "driver_event_handler isn't set"    );

    while ( uart_thread_running == mxos_TRUE )
    {
        bt_packet_t* packet = NULL;

        if ( driver_bus_read_handler( &packet ) != mxos_BT_SUCCESS )
        {
            continue;
        }

        /* Read successful. Notify upper layer via driver_callback that a new packet is available */
        mxos_rtos_lock_mutex( &packet_list_mutex );
        linked_list_set_node_data( &packet->node, (void*)packet );
        linked_list_insert_node_at_rear( &uart_rx_packet_list, &packet->node );
        mxos_rtos_unlock_mutex( &packet_list_mutex );
        driver_event_handler( TRANSPORT_DRIVER_INCOMING_PACKET_READY );
    }

    mxos_rtos_delete_thread(NULL);
}
