/**
******************************************************************************
* @file    mxos_config.h
* @author  Snow Yang
* @version V1.0.0
* @date    08-Aug-2018
* @brief   This file provide application options diff to default.
******************************************************************************
*/

#ifndef __MXOS_CONFIG_H__
#define __MXOS_CONFIG_H__

#include "mxkit.h"

#define MXOS_LOG_UART MXKIT_LOG_UART
#define MXOS_LOG_UART_BAUDRATE 115200
#define MXOS_LOG_UART_RXD MXKIT_LOG_UART_RXD
#define MXOS_LOG_UART_TXD MXKIT_LOG_UART_TXD

#define MXOS_APP_UART MXKIT_APP_UART
#define MXOS_APP_UART_BAUDRATE 115200
#define MXOS_APP_UART_RXD MXKIT_APP_UART_RXD
#define MXOS_APP_UART_TXD MXKIT_APP_UART_TXD
#define MXOS_APP_UART_RTS MXKIT_APP_UART_RTS
#define MXOS_APP_UART_CTS MXKIT_APP_UART_CTS

#define MXOS_ELINK_PIN MXKIT_ELINK_PIN

#define MXOS_I2C MXKIT_I2C
#define MXOS_SDA MXKIT_SDA
#define MXOS_SCL MXKIT_SCK

#endif
