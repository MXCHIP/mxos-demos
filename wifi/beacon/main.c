/**
 ******************************************************************************
 * @file    main.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    2019/08/30
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
#include <stdarg.h>

#include "mxos.h"
#include "80211sim.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

const uint8_t ssid0[] = {0x41, 0x41, 0x41, 0x20, 0xE5, 0x8D, 0x81, 0xE9, 0x87, 0x8C, 0xE5, 0xB9, 0xB3, 0xE6, 0xB9, 0x96, 0xE9, 0x9C, 0x9C, 0xE6, 0xBB, 0xA1, 0xE5, 0xA4, 0xA9};
const uint8_t ssid1[] = {0x41, 0x41, 0x41, 0x20, 0xE5, 0xAF, 0xB8, 0xE5, 0xAF, 0xB8, 0xE9, 0x9D, 0x92, 0xE4, 0xB8, 0x9D, 0xE6, 0x84, 0x81, 0xE5, 0x8D, 0x8E, 0xE5, 0xB9, 0xB4};
const uint8_t ssid2[] = {0x41, 0x41, 0x41, 0x20, 0xE5, 0xBD, 0xA2, 0xE5, 0x8D, 0x95, 0xE5, 0xAF, 0xB9, 0xE6, 0x9C, 0x88, 0xE6, 0x9C, 0x9B, 0xE7, 0x9B, 0xB8, 0xE4, 0xBA, 0x92};
const uint8_t ssid3[] = {0x41, 0x41, 0x41, 0x20, 0xE5, 0x8F, 0xAA, 0xE7, 0xBE, 0xA1, 0xE9, 0xB8, 0xB3, 0xE9, 0xB8, 0xAF, 0xE4, 0xB8, 0x8D, 0xE7, 0xBE, 0xA1, 0xE4, 0xBB, 0x99};

int main(void)
{
  uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00};
  char ssidbuf[33];

  mxos_network_init();
  mwifi_monitor_start();

  while (1)
  {
    for (int i = 1; i <= 13; i++)
    {
      mwifi_monitor_set_channel(i);
      mac[5] = i;
      beacon_send(mac, ssid0, sizeof(ssid0), "MXCHIP", strlen("MXCHIP"));
      mos_msleep(2);
      beacon_send(mac, ssid1, sizeof(ssid1), "MXCHIP", strlen("MXCHIP"));
      mos_msleep(2);
      beacon_send(mac, ssid2, sizeof(ssid2), "MXCHIP", strlen("MXCHIP"));
      mos_msleep(2);
      beacon_send(mac, ssid3, sizeof(ssid3), "MXCHIP", strlen("MXCHIP"));
      mos_msleep(2);
    }
  }

  return 0;
}
