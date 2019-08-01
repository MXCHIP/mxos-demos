/**
******************************************************************************
* @file    mxos_config.h
* @author  William Xu
* @version V1.0.0
* @date    08-Aug-2018
* @brief   This file provide application options diff to default.
******************************************************************************
*/

#ifndef __MXOS_CONFIG_H__
#define __MXOS_CONFIG_H__

#define APP_INFO   "IPerf"

#define FIRMWARE_REVISION   "iperf"
#define MANUFACTURER        "MXCHIP Inc."
#define SERIAL_NUMBER       "20190411"
#define PROTOCOL            "com.mxchip.spp"


/************************************************************************
 * Application thread stack size */
#define MXOS_DEFAULT_APPLICATION_STACK_SIZE         (4096)


/************************************************************************
 * MXOS system debug info  */
#define CONFIG_SYSTEM_DEBUG                         MXOS_DEBUG_ON

//#define MXOS_CLI_ENABLE                           0
#define MXOS_WLAN_CONNECTION_ENABLE    0

#endif
