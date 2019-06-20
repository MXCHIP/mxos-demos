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
#include <stdio.h>
#include <stdint.h>

#include "mos.h"

#include "mwifi.h"
#include "mxos_socket.h"
#include "mtls.h"

static void monitor_cb(uint8_t *data, int len)
{
#if 0
    int i;
    
    printf("[%d]: ", len);
    for(i=0; i<24; i++) {
        printf("%02x ", data[i]);
    }
    printf("\r\n");
#endif
}

static const uint8_t beacon[] = {
    0x80,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0x2F,0xEF,0xB6,
    0x97,0x9C,0xFC,0x2F,0xEF,0xB6,0x97,0x9C,0x20,0xA4,0x61,0x11,0x90,0x61,
    0x9E,0x00,0x00,0x00,0x64,0x00,0x31,0x04,0x00,0x0C,0x31,0x41,0x41,0x41,
    0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x01,0x08,0x82,0x84,0x8B,0x96,
    0x12,0x24,0x48,0x6C,0x03,0x01,0x06,0x32,0x04,0x0C,0x18,0x30,0x60,0x05,
    0x04,0x00,0x01,0x00,0x00,0x2A,0x01,0x00,0xDD,0x1A,0x00,0x50,0xF2,0x01,
    0x01,0x00,0x00,0x50,0xF2,0x02,0x02,0x00,0x00,0x50,0xF2,0x02,0x00,0x50,
    0xF2,0x04,0x01,0x00,0x00,0x50,0xF2,0x02,0x30,0x18,0x01,0x00,0x00,0x0F,
    0xAC,0x02,0x02,0x00,0x00,0x0F,0xAC,0x02,0x00,0x0F,0xAC,0x04,0x01,0x00,
    0x00,0x0F,0xAC,0x02,0x00,0x00,0xDD,0x18,0x00,0x50,0xF2,0x02,0x01,0x01,
    0x00,0x00,0x03,0xA4,0x00,0x00,0x27,0xA4,0x00,0x00,0x42,0x43,0x5E,0x00,
    0x62,0x32,0x2F,0x00,0x0B,0x05,0x03,0x00,0x9B,0x12,0x7A,0xDD,0x07,0x00,
    0x0C,0x43,0x07,0x00,0x00,0x00,};

static const uint8_t qos_data[] = {
    0x88,0x41,0x75,0x00,0xFC,0x2F,0xEF,0xA7,0x17,0xD9,0x90,0xF0,0x52,0x49,
    0xF9,0x03,0xE8,0xBD,0xD1,0x00,0x39,0x01,0x80,0x3C,0x00,0x00,0xC2,0x15,
    0x00,0x20,0x00,0x00,0x00,0x00,0xC3,0x27,0xE0,0x97,0x1D,0x10,0x7F,0x3C,
    0x3D,0x6C,0x2F,0xAB,0x72,0x03,0xA4,0x1F,0xAD,0xB0,0x66,0xD5,0x38,0xCD,
    0xA2,0x02,0xE1,0xF4,0xAB,0x9D,0x5B,0xCA,0x7C,0x91,0x17,0x5D,0x75,0xEC,
    0xE0,0x2C,0x0B,0x81,0xAE,0x0B,0xD3,0x4C,0x09,0x56,0x68,0x94,0x7B,0x93,
    0x82,0x43,0x6C,0xFB,0x78,0xCE,0x17,0x49,0xF2,0x4D,0xEC,0x4A,0x54,0x9D,
    0xE5,0x15,0xB6,0x81,0x52,0xFB,0x9B,0x6D,0x33,0x48,0x81,0x30,0xDD,0x47,
    0x25,0x7A,0xCA,0x04,0x20,0x53,0x59,0x34,0x39,0x72,0x0E,0x91,0x7B,0x3E,
    0x45,0x34,0xE6,0x91,0x9A,0x96,0xD8,0x92,0x32,0x4F,0x30,0x81,0xE5,0xEA,
    0x81,0xD8,0xB9,0xBA,0xC6,0x26,0x57,0xB2,0x13,0x64,0x91,0xF0,0x7B,0x33,
    0xAE,0x1C,0x01,0x4E,0x97,0x6B,0x8D,0x8F,0xAE,0xC6,0xAC,0x06,0x0C,0x63,
    0x79,0x90,0x82,0x0F,0xE9,0x3E,0x87,0xAF,0x09,0x21,0xA7,0xDD,0xB4,0x73,
    0x45,0x86,0x8B,0xBC,0x14,0x7F,0x04,0x42,0x1B,0x6A,0x11,0x9B,0x53,0xE2,
    0xCA,0x72,0x2C,0x84,0xAC,0x3C,0x22,0x5B,0x2E,0x36,0x28,0xF9,0x64,
};

static void monitor_demo(void)
{
    uint8_t channel = 1;
    int i=0;
    
    mwifi_monitor_reg_cb(monitor_cb);
    mwifi_monitor_start();
    mwifi_monitor_set_channel(3);
    while(i<100) {
        mwifi_monitor_send_frame(beacon, sizeof(beacon));// 802.11data, exclude FCS
        mos_msleep(100);
        mwifi_monitor_send_frame(qos_data, sizeof(qos_data));
        mos_msleep(100);
        i++;
    }
    while(1) {
        printf("set channel %d\r\n", channel);
        mwifi_monitor_set_channel(channel++);
        if (channel == 14)
            channel = 1;
        mos_msleep(1000);
    }
}

static void scan_demo(void)
{
    int i;

    for(i=0;i<5;i++) {
        mwifi_scan(NULL);
        vTaskDelay(5000);
        mwifi_scan("Xiaomi_yhb");
        vTaskDelay(5000);
    }
}

static void rx_mgnt_cb(uint8_t *data, int len)
{
    int i;

    if (data[0] == 0x80)// ignore beacon
        return;
    
    printf("len=%d: ", len);

    for(i=0; i<len;i++) {
        printf("%02x ", data[i]);
    }
    printf("\r\n");
}

static void station_rx_mgnt_demo(void)
{
    mwifi_connect("Xiaomi_yhb", "stm32f215", strlen("stm32f215"), NULL, NULL);
    //vTaskDelay(6000);
    mwifi_reg_mgnt_cb(rx_mgnt_cb);
    
    vTaskDelay(30000);
}

static void softap_start(void)
{
    mwifi_softap_attr_t attr;
    mwifi_ip_attr_t ip;

    strcpy(ip.localip, "10.0.0.1");
    strcpy(ip.netmask, "255.255.255.0");
    strcpy(ip.gateway, "10.0.0.1");
    attr.ip_attr = &ip;
    
    mwifi_softap_start("amebad-yhb", "88888888", 6, &attr);
}

static const uint8_t custom_ie[] = {0xc8, 0x93, 0x46, 0x01, 0x05, 0x30, 0x31};

static void custom_ie_demo(void)
{
    mwifi_custom_ie_add(STATION_INTERFACE, custom_ie, sizeof(custom_ie));
    mwifi_custom_ie_add(SOFTAP_INTERFACE, custom_ie, sizeof(custom_ie));
    mwifi_connect("Xiaomi_yhb", "stm32f215", strlen("stm32f215"), NULL, NULL);
    vTaskDelay(6000);
    mwifi_scan(NULL);
    vTaskDelay(6000);
}

static void ps_demo(void)
{
    uint8_t mac[6];
    mwifi_ip_attr_t attr;
    
    printf("Enable PS\r\n");
    mwifi_ps_on();
    mwifi_connect("Xiaomi_yhb", "stm32f215", strlen("stm32f215"), NULL, NULL);
    mos_sleep(10);
    mwifi_get_mac(mac);
    printf("mac: %02x-%02x-%02x-%02x-%02x-%02x\r\n", mac[0],
        mac[1],mac[2],mac[3],mac[4],mac[5]);
    mwifi_get_ip(&attr, STATION_INTERFACE);
    printf("IP: %s, Netmask: %s, Gateway: %s, dns: %s\r\n",
        attr.localip, attr.netmask, attr.gateway, attr.dnserver);
    printf("Disable PS\r\n");
    mwifi_ps_off();
    mos_sleep(10);
}

/*create udp socket*/
void udp_broadcast_thread( void* arg )
{
    struct sockaddr_in addr;
    int udp_fd = -1;

    printf("udp broad cast\r\n");
    /*Establish a UDP port to receive any data sent to this port*/
    udp_fd = lwip_socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

    while ( 1 )
    {
        
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_BROADCAST;
        addr.sin_port = lwip_htons( 20000 );
        lwip_sendto( udp_fd, qos_data, sizeof( qos_data ), 0, (struct sockaddr *) &addr, sizeof(addr) );

        mos_msleep(10);
    }

    exit:

   
    mos_thread_delete( NULL );
}

int main( void )
{
  merr_t err = kNoErr;
  int i=0;
  mos_mallinfo_t * info = mos_mallinfo();
  
  
  wifimgr_debug_enable(true);
  wifi_driver_init();
  #if 0
    dac_demo_main();
    mwifi_connect("Xiaomi_yhb", "stm32f215", strlen("stm32f215"), NULL, NULL);
    mos_thread_new(7, "udp", udp_broadcast_thread, 1024, NULL);
    
    return 0;
    #endif
    //ps_demo();
    
    //monitor_demo();
    
    //scan_demo();
    
    //station_rx_mgnt_demo();

    //custom_ie_demo();
    
    
    //softap_start();
    //custom_ie_demo();
    
    //vTaskDelay(30000);
  while(1) {
      mwifi_disconnect();
      mwifi_connect("AP057", "12345678", 8, NULL, NULL);
      vTaskDelay(8000);
      mwifi_disconnect();
      printf("free %d, chunk %d, min_free %d\r\n", info->free, info->chunks, info->min_free);
      mwifi_connect("AP060", "12345678", 8, NULL, NULL);
      vTaskDelay(8000);
      mwifi_disconnect();
      printf("free %d, chunk %d, min_free %d\r\n", info->free, info->chunks, info->min_free);
      mwifi_connect("snowyang", "12345678", 8, NULL, NULL);
      vTaskDelay(8000);
      mwifi_disconnect();
      printf("free %d, chunk %d, min_free %d\r\n", info->free, info->chunks, info->min_free);
      mwifi_connect("Xiaomi_yhb", "stm32f215", strlen("stm32f215"), NULL, NULL);
      vTaskDelay(8000);
      info = mos_mallinfo();
      printf("free %d, chunk %d, min_free %d\r\n", info->free, info->chunks, info->min_free);
  }
  

exit:
  return err;
}

