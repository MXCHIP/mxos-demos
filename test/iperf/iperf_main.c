/**
 ******************************************************************************
 * @file    iperf_main.c
 * @author  Libo
 * @version V1.0.0
 * @date    5-Jan-2018
 * @brief   This file provide the iperf test application.
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mxos.h"
#include "iperf_cli.h"

#define  iperf_test_log(M, ...) custom_log("Iperf", M, ##__VA_ARGS__)

uint8_t uap_up = 0;

static void Notify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
  switch (status) 
  {
  case NOTIFY_STATION_UP:
    break;
  case NOTIFY_STATION_DOWN:
    break;
  case NOTIFY_AP_UP:
    uap_up = 1;
    break;
  case NOTIFY_AP_DOWN:
    uap_up = 0;
    break;
  default:
    break;
  }
}

int main( void )
{
    merr_t err = kNoErr;

    /*Register user function for MiCO nitification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED,
                                       (void *) Notify_WifiStatusHandler, NULL );
    require_noerr( err, exit );

    /* Start MiCO system functions according to mxos_config.h */
    mxos_system_init( );

    /* Register iperf command to test   */
    iperf_cli_register();
    iperf_test_log( "iPerf tester started, input \"iperf -h\" for help." );

exit:
    return err;
}

