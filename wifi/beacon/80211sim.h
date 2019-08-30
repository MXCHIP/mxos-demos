/**
 ******************************************************************************
 * @file    80211sim.h
 * @author  Snow Yang
 * @version V1.0.0
 * @date    2017-10-15
 * @brief   Header file of 80211sim.c
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2017 MXCHIP Inc.
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
 *
 ******************************************************************************
 */

#ifndef __LOW_LEVEL_DRIVER_H__
#define __LOW_LEVEL_DRIVER_H__

#include <stdint.h>
#include <stdbool.h>

#define MANAGEMENT_TYPE 0x00
#define BEACON_SUBTYE 0x08
#define ACTION_SUBTYE 0x0D

#define VENDOR_SPEC_DATA_MIN_SIZE 0
#define VENDOR_SPEC_DATA_MAX_SIZE 252

#pragma pack(1)
typedef struct
{
	uint8_t version : 2;
	uint8_t type : 2;
	uint8_t subtype : 4;
	uint8_t to_ds : 1;
	uint8_t from_ds : 1;
	uint8_t more_frag : 1;
	uint8_t retry : 1;
	uint8_t pwr_mgt : 1;
	uint8_t more_data : 1;
	uint8_t protected_frame : 1;
	uint8_t order : 1;
	uint16_t duration;
	uint8_t dst_adr[6];
	uint8_t src_adr[6];
	uint8_t bssid[6];
	uint16_t seq_ctl;
} management_header_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
	uint64_t timestamp;
	uint16_t beacon_interval;
	uint16_t capability;
} beacon_fixed_params_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
	uint8_t category;
	uint8_t oui[3];
} action_frame_body_header_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
	uint8_t id;
	uint8_t length;
} info_element_header_t;
#pragma pack()

/*
Bref:	Send Wi-Fi NFC beacon frame.
Param:	ssid		- SSID string, 1 - 32 bytes.
		data		- Data buffer.
		size		- Data length, 0 - 252.
*/
void beacon_send(uint8_t *mac, char *ssid, uint8_t ssid_len, uint8_t *data, uint8_t size);

#endif