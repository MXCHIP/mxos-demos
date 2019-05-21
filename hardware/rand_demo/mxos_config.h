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

#define APP_INFO   "GPIO DEMO"

#define FIRMWARE_REVISION   "MXOS_SPP_1_0"
#define MANUFACTURER        "MXCHIP Inc."
#define SERIAL_NUMBER       "20190411"
#define PROTOCOL            "com.mxchip.spp"


/************************************************************************
 * Application thread stack size */
#define MXOS_DEFAULT_APPLICATION_STACK_SIZE         (4096)

/************************************************************************
 * MiCO TCP server used for configuration and ota. */
#define MXOS_CONFIG_SERVER_ENABLE                   1

/************************************************************************
 * Start standard QC test function other than application  */
#define MXOS_QUALITY_CONTROL_ENABLE                 1

/************************************************************************
 * Set local server port to system discovery mdns service  */
#define MXOS_SYSTEM_DISCOVERY_PORT                  8080

/************************************************************************
 * MXOS system debug info  */
#define CONFIG_SYSTEM_DEBUG                         MXOS_DEBUG_ON

/************************************************************************
 * Command line interface  */
//#define MXOS_CLI_ENABLE                           0

/************************************************************************
 * IPV6 */
//#define MXOS_CONFIG_IPV6                          1

#endif
