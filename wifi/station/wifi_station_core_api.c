/**
 ******************************************************************************
 * @file    wifi_station.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   Connect to access point using core MiCO wlan APIs
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

#include "mxos.h"

#define wifi_station_log(M, ...) custom_log("WIFI", M, ##__VA_ARGS__)

#define STATION_SSID  "William Xu"
#define STATION_KEY   "mx099555"

static void mxosNotify_WifiStatusHandler(WiFiEvent event,  void* inContext)
{
  switch (event) 
  {
  case NOTIFY_STATION_UP:
    wifi_station_log("Station up");
    break;
  case NOTIFY_STATION_DOWN:
    wifi_station_log("Station down");
    break;
  default:
    break;
  }
}
int main( void )
{
  merr_t err = kNoErr;
  mwifi_connect_attr_t  wNetConfig;

  uint32_t mwifi_reconnect_time = 0;

  mxos_system_init(  );
  
  /* Register user function when wlan connection status is changed */
  err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED, (void *)mxosNotify_WifiStatusHandler, NULL );
  require_noerr( err, exit ); 

  /* Initialize wlan parameters */
  memset( &wNetConfig, 0x0, sizeof(wNetConfig) );
  wNetConfig.channel = 0;
  wNetConfig.security = SECURITY_TYPE_AUTO;

  /* Set wifi reconnect time(ms)! */
  mwifi_reconnect_time = 100;
  mwifi_set_reconnect_interval(mwifi_reconnect_time);
  
  /* Connect Now! */
  wifi_station_log("connecting to %s...", STATION_SSID);
  mwifi_connect(STATION_SSID,STATION_KEY,strlen(STATION_KEY),&wNetConfig,NULL);
  
exit:
  return err;
}

