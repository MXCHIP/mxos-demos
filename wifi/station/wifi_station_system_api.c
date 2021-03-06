/**
 ******************************************************************************
 * @file    wifi_station_system_api.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   Wlan configuration using EasyLink and connect to access point!
 ******************************************************************************
 *
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

#include "mico.h"

int main( void )
{ 
  merr_t err = kNoErr;
  /* Start MiCO system functions according to mxos_config.h, 
     Define macro mxos_WLAN_CONNECTION_ENABLE to enable wlan connection function
     Select wlan configuration mode: mxos_WLAN_CONFIG_MODE
     Define EasyLink settings */
  mxos_Context_t* context = system_context_init( 0 );
  mxos_system_init( context );
  
  return err;
}

