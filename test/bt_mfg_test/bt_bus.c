/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 */

/** @file
 *
 */
#include "bt_bus.h"
#include "platform.h"
#include "platform_config.h"
#include "mxos_bt_constants.h"
#include "mico.h"
#include "mxos_rtos.h"
#include "platform_bluetooth.h"
#include "RingBufferUtils.h"

/******************************************************
 *                      Macros
 ******************************************************/

/* Verify if WICED Platform API returns success.
 * Otherwise, returns the error code immediately.
 * Assert in DEBUG build.
 */
#define RETURN_IF_FAILURE( x ) \
    do \
    { \
        wiced_result_t _result = (x); \
        if ( _result != mxos_SUCCESS ) \
        { \
            return _result; \
        } \
    } while( 0 )


/* Macro for checking of bus is initialised */
#define IS_BUS_INITIALISED( ) \
do \
{ \
    if ( bus_initialised == mxos_FALSE ) \
    { \
        check_string(0!=0, "bus uninitialised"); \
        return kNotInitializedErr; \
    } \
}while ( 0 )

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

static merr_t bluetooth_mxos_init_platform   ( void );
merr_t bluetooth_mxos_init_config_uart( const platform_uart_config_t* bt_uart_config );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static volatile mxos_bool_t bus_initialised = mxos_FALSE;
static volatile mxos_bool_t device_powered  = mxos_FALSE;

/* RX ring buffer. Bluetooth chip UART receive can be asynchronous, therefore a ring buffer is required */
static volatile ring_buffer_t rx_ring_buffer;
static volatile uint8_t             rx_data[BT_BUS_RX_FIFO_SIZE];

/******************************************************
 *               Function Definitions
 ******************************************************/

merr_t bt_mfg_bus_init( void )
{
    merr_t init_result = kNoErr;
    if ( bus_initialised == mxos_FALSE )
    {
        init_result = bluetooth_mxos_init_platform( );
        if ( init_result == kNoErr )
        {
            bus_initialised = mxos_TRUE;
        }
    }
    return init_result;
}

merr_t bt_mfg_bus_deinit( void )
{
    merr_t err = kNoErr;

    if ( bus_initialised == mxos_FALSE )
    {
        return mxos_BT_SUCCESS;
    }

    if ( mxos_bt_control_pins[ mxos_BT_PIN_RESET ] != (platform_gpio_t *)NULL )
    {
        err = platform_gpio_output_low( mxos_bt_control_pins[mxos_BT_PIN_RESET] );
        require_noerr(err, exit);
    }

    if ( mxos_bt_uart_config.flow_control == FLOW_CONTROL_DISABLED )
    {
        err = platform_gpio_output_high( mxos_bt_uart_pins[mxos_BT_PIN_UART_RTS] ); // RTS deasserted
        require_noerr(err, exit);
    }

    if ( mxos_bt_control_pins[ mxos_BT_PIN_POWER ] != NULL )
    {
        err = platform_gpio_output_low ( mxos_bt_control_pins[mxos_BT_PIN_POWER] ) ; // Bluetooth chip regulator off
        require_noerr(err, exit);
    }

    device_powered = mxos_FALSE;

    /* Deinitialise UART */
    err =  platform_uart_deinit( mxos_bt_uart_driver );
    require_noerr(err, exit);
    bus_initialised = mxos_FALSE;

exit:
    return mxos_BT_SUCCESS;
}

merr_t bt_mfg_bus_transmit( const uint8_t* data_out, uint32_t size )
{
    merr_t err = kNoErr;

    require_action( bus_initialised == mxos_TRUE, exit, err = kNotInitializedErr);

    BT_BUS_WAIT_UNTIL_READY();

    err = platform_uart_transmit_bytes( mxos_bt_uart_driver, data_out, size );

exit:
    return err;
}

merr_t bt_mfg_bus_receive( uint8_t* data_in, uint32_t size, uint32_t timeout_ms )
{
    IS_BUS_INITIALISED();

    return platform_uart_receive_bytes( mxos_bt_uart_driver, (void*)data_in, size, timeout_ms );
}

mxos_bool_t bt_mfg_bus_is_ready( void )
{
    return ( bus_initialised == mxos_FALSE ) ? mxos_FALSE : ( ( platform_gpio_input_get( mxos_bt_uart_pins[mxos_BT_PIN_UART_CTS] ) == mxos_TRUE ) ? mxos_FALSE : mxos_TRUE );
}

mxos_bool_t bt_mfg_bus_is_on( void )
{
    return device_powered;
}

merr_t bluetooth_mxos_init_platform( void )
{
    merr_t err = kNoErr;

    if ( mxos_bt_control_pins[ mxos_BT_PIN_HOST_WAKE ] != NULL )
    {
        err = platform_gpio_init( mxos_bt_control_pins[mxos_BT_PIN_HOST_WAKE], INPUT_HIGH_IMPEDANCE );
        require_noerr(err, exit);
    }

    if ( mxos_bt_control_pins[ mxos_BT_PIN_DEVICE_WAKE ] != NULL )
    {
        err = platform_gpio_init( mxos_bt_control_pins[ mxos_BT_PIN_DEVICE_WAKE ], OUTPUT_PUSH_PULL ) ;
        require_noerr(err, exit);
        err = platform_gpio_output_low( mxos_bt_control_pins[ mxos_BT_PIN_DEVICE_WAKE ] );
        mxos_rtos_delay_milliseconds( 100 );
    }

    /* Configure Reg Enable pin to output. Set to HIGH */
    if ( mxos_bt_control_pins[ mxos_BT_PIN_POWER ] != NULL )
    {
        err = platform_gpio_init( mxos_bt_control_pins[ mxos_BT_PIN_POWER ], OUTPUT_OPEN_DRAIN_PULL_UP );
        require_noerr(err, exit);
        err = platform_gpio_output_high( mxos_bt_control_pins[ mxos_BT_PIN_POWER ] );
        require_noerr(err, exit);
    }

    if ( mxos_bt_uart_config.flow_control == FLOW_CONTROL_DISABLED )
    {
        /* Configure RTS pin to output. Set to HIGH */
        err = platform_gpio_init( mxos_bt_uart_pins[mxos_BT_PIN_UART_RTS], OUTPUT_OPEN_DRAIN_PULL_UP );
        require_noerr(err, exit);
        err = platform_gpio_output_low( mxos_bt_uart_pins[mxos_BT_PIN_UART_RTS] ); //William, working wrong if set high
        require_noerr(err, exit);

        /* Configure CTS pin to input pull-up */
        err = platform_gpio_init( mxos_bt_uart_pins[mxos_BT_PIN_UART_CTS], INPUT_PULL_UP );
        require_noerr(err, exit);
    }

    if ( mxos_bt_control_pins[ mxos_BT_PIN_RESET ] != NULL )
    {
        err = platform_gpio_init( mxos_bt_control_pins[ mxos_BT_PIN_RESET ], OUTPUT_PUSH_PULL );
        require_noerr(err, exit);

        err = platform_gpio_output_high( mxos_bt_control_pins[ mxos_BT_PIN_RESET ] );
        require_noerr(err, exit);

        /* Configure USART comms */
        err = bluetooth_mxos_init_config_uart( &mxos_bt_uart_config );
        require_noerr(err, exit);

        /* Reset bluetooth chip */
        err = platform_gpio_output_low( mxos_bt_control_pins[ mxos_BT_PIN_RESET ] );
        require_noerr(err, exit);
        mxos_rtos_delay_milliseconds( 10 );
        err = platform_gpio_output_high( mxos_bt_control_pins[ mxos_BT_PIN_RESET ] );
        require_noerr(err, exit);
    }
    else
    {
        /* Configure USART comms */
        err = bluetooth_mxos_init_config_uart( &mxos_bt_uart_config );
        require_noerr(err, exit);
    }

    mxos_rtos_delay_milliseconds( BLUETOOTH_CHIP_STABILIZATION_DELAY );

    if ( mxos_bt_uart_config.flow_control == FLOW_CONTROL_DISABLED )
    {
        /* Bluetooth chip is ready. Pull host's RTS low */
        err = platform_gpio_output_low( mxos_bt_uart_pins[mxos_BT_PIN_UART_RTS] );
        require_noerr(err, exit);
    }

    /* Wait for Bluetooth chip to pull its RTS (host's CTS) low. From observation using CRO, it takes the bluetooth chip > 170ms to pull its RTS low after CTS low */
    while ( platform_gpio_input_get( mxos_bt_uart_pins[ mxos_BT_PIN_UART_CTS ] ) == mxos_TRUE )
    {
        mxos_rtos_delay_milliseconds( 10 );
    }

exit:
    return err;
}

merr_t bluetooth_mxos_init_config_uart( const platform_uart_config_t* bt_uart_config )
{
    merr_t result;

    ring_buffer_init( (ring_buffer_t*) &rx_ring_buffer, (uint8_t*) rx_data, sizeof( rx_data ) );
    result = platform_uart_init( mxos_bt_uart_driver, mxos_bt_uart_peripheral, bt_uart_config, (ring_buffer_t*) &rx_ring_buffer );
    return result;
}

