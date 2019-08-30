/**
 ******************************************************************************
 * @file    80211sim.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    2017-10-15
 * @brief   Low level drivers for protocol.
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

#include "mxos.h"
#include "80211sim.h"

/*
The frame format for a management frame is:
| <------------------------- MAC Header ----------------------------------------------------> |
+---------------+----------+--------------+--------------+-----------------+------------------+---------------+---------+
| Frame control | Duration | Address1(DA) | Address2(SA) | Address3(BSSID) | Sequence control | Frame body    | FCS     |
+---------------+----------+--------------+--------------+-----------------+------------------+---------------+---------+
| 2 bytes       | 2 bytes  | 6 bytes      | 6 bytes      | 6 bytes         | 2 bytes          | 0 -2312 bytes | 4 bytes |     
+---------------+----------+--------------+--------------+-----------------+------------------+---------------+---------+
The FCS field is a 32-bit field containing a 32-bit CRC. 
The FCS is calculated over all the fields of the MAC header and the Frame Body field.
We use Vendor Specific information element as user data storage:
+------------+---------+---------+-------------------------+
| Element ID | Length  | OUI     | Vendor Specific Content |
+------------+---------+---------+-------------------------+
| 1 byte     | 1 bytes | 3 bytes | 0 - 252 bytes           |
+------------+---------+---------+-------------------------+
*/

#define OUI_SIZE 3

#define SSID_MIN_SIZE 1
#define SSID_MAX_SIZE 32

#define WIFI_NFC_BEACON_MIN_SIZE (sizeof(management_header_t) + sizeof(beacon_fixed_params_t) + sizeof(info_element_header_t) + SSID_MIN_SIZE + sizeof(info_element_header_t) + OUI_SIZE + VENDOR_SPEC_DATA_MIN_SIZE)
#define WIFI_NFC_BEACON_MAX_SIZE (sizeof(management_header_t) + sizeof(beacon_fixed_params_t) + sizeof(info_element_header_t) + SSID_MAX_SIZE + sizeof(info_element_header_t) + OUI_SIZE + VENDOR_SPEC_DATA_MAX_SIZE)

#define WIFI_NFC_ACTION_MIN_SIZE (sizeof(management_header_t) + sizeof(action_frame_body_header_t) + sizeof(info_element_header_t) + OUI_SIZE + VENDOR_SPEC_DATA_MIN_SIZE)
#define WIFI_NFC_ACTION_MAX_SIZE (sizeof(management_header_t) + sizeof(action_frame_body_header_t) + sizeof(info_element_header_t) + OUI_SIZE + VENDOR_SPEC_DATA_MAX_SIZE)

uint8_t oui[OUI_SIZE] = {0xC8, 0x93, 0x46};
uint8_t bc_adr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void beacon_send(uint8_t *mac, char *ssid, uint8_t ssid_len, uint8_t *data, uint8_t size)
{
	management_header_t *headr;
	beacon_fixed_params_t *fixed_params;
	info_element_header_t *element_header;
	uint8_t *element_data;
	uint8_t *frame_ptr;
	uint8_t *buf = NULL;

	require(ssid_len <= 32 && ssid_len >= 1 && size <= VENDOR_SPEC_DATA_MAX_SIZE, exit);

	buf = (uint8_t *)malloc(WIFI_NFC_BEACON_MAX_SIZE);
	require(buf != NULL, exit);

	frame_ptr = buf;
	// ======== Management frame's header
	headr = (management_header_t *)frame_ptr;
	memset(buf, 0, sizeof(management_header_t));
	// Beacon's subtype is 0x08
	headr->type = 0x00;
	headr->subtype = 0x08;
	// Beacon is broadcast frame
	memcpy(headr->dst_adr, bc_adr, 6);
	// Beacon's source MAC and BSSID is host's MAC address
	memcpy(headr->src_adr, mac, 6);
	memcpy(headr->bssid, mac, 6);
	frame_ptr += sizeof(management_header_t);

	// ======== Mangement frame's payload
	fixed_params = (beacon_fixed_params_t *)frame_ptr;
	// -------- Beacon interval (2 bytes)
	fixed_params->beacon_interval = 100;
	// -------- Capability (2 bytes)
	fixed_params->capability = 0x0401;
	frame_ptr += sizeof(beacon_fixed_params_t);
	// --------  SSID element
	element_header = (info_element_header_t *)frame_ptr;
	// ID
	element_header->id = 0; // SSID parameter set (0)
	// length
	element_header->length = ssid_len; // 0 - 32
	frame_ptr += sizeof(info_element_header_t);
	// SSID
	element_data = frame_ptr;
	memcpy(element_data, ssid, ssid_len); // SSID string
	frame_ptr += ssid_len;
	// -------- Vendor Specific information element
	element_header = (info_element_header_t *)frame_ptr;
	// ID
	element_header->id = 221; // "221" is vendor-specific element ID
	// length
	element_header->length = OUI_SIZE + size;
	frame_ptr += sizeof(info_element_header_t);
	// OUI
	element_data = frame_ptr;
	memcpy(element_data, oui, OUI_SIZE);
	frame_ptr += OUI_SIZE;
	// Vendor Specific Data
	memcpy(frame_ptr, data, size);

	// Send beacon frame
	mwifi_monitor_send_frame(buf, sizeof(management_header_t) + sizeof(beacon_fixed_params_t) + sizeof(info_element_header_t) + ssid_len + sizeof(info_element_header_t) + OUI_SIZE + size);

exit:
	if (buf)
	{
		free(buf);
	}
}

/* 
The Action field provides a mechanism for specifying extended management actions. 
The format of the Action field is:
+----------+----------------+
| Category | Action Details |
+----------+----------------+
| 1 byte   | variable       |
+----------+----------------+
We use Vendor-specific (127) here.
The Vendor Specific Action frame is defined for vendor-specific signaling. 
The format of the Vendor Specific Action frame's Action Details is:
+---------+-------------------------+
| OUI     | Vendor Specific Content |
+---------+-------------------------+
| 3 bytes | variable                |
+---------+-------------------------+
The OUI field is a public OUI assigned by the IEEE. It is 3 octets in length. 
It contains the OUI of the entity that has defined the content of the particular vendor-specific action.
The Vendor Specific Content contains vendor-specific field(s). 
We use Vendor Specific information element as it's content:
+------------+---------+---------+----------------------+
| Element ID | Length  | OUI     | Vendor Specific Data	|
+------------+---------+---------+----------------------+
| 1 byte     | 1 bytes | 3 bytes | 0 - 252 bytes        |
+------------+---------+---------+----------------------+
*/
void wifi_nfc_low_level_send_action(uint8_t *dst_adr, uint8_t *data, uint8_t size)
{
	management_header_t *headr;
	action_frame_body_header_t *action_frame_header;
	info_element_header_t *element_header;
	uint8_t *element_data;
	uint8_t *frame_ptr;
	uint8_t *buf = NULL;

	require(size <= VENDOR_SPEC_DATA_MAX_SIZE, exit);

	buf = (uint8_t *)malloc(WIFI_NFC_ACTION_MAX_SIZE);
	require(buf != NULL, exit);

	frame_ptr = buf;
	// ======== Management frame's header
	headr = (management_header_t *)frame_ptr;
	memset(buf, 0, sizeof(management_header_t));
	// Action's subtype is 0x0D
	headr->type = 0x00;
	headr->subtype = 0x0D;
	// Action's destination MAC
	memcpy(headr->dst_adr, dst_adr, 6);
	// Action's source MAC and BSSID is host's MAC address
	mwifi_get_mac(headr->src_adr);
	mwifi_get_mac(headr->bssid);
	frame_ptr += sizeof(management_header_t);

	// ======== Mangement frame's payload
	action_frame_header = (action_frame_body_header_t *)frame_ptr;
	// -------- Action's Category code
	action_frame_header->category = 127; // "127" is vendor-specific action code
	// -------- Action's Details
	// ........ OUI
	memcpy(action_frame_header->oui, oui, OUI_SIZE);
	frame_ptr += sizeof(action_frame_body_header_t);
	// ........ Vendor Specific Content
	// ,,,,,,,, Vendor Specific information element
	element_header = (info_element_header_t *)frame_ptr;
	// Element ID
	element_header->id = 221; // "221" is vendor-specific element ID
	// length
	element_header->length = OUI_SIZE + size;
	frame_ptr += sizeof(info_element_header_t);
	// OUI
	element_data = frame_ptr;
	memcpy(element_data, oui, OUI_SIZE);
	frame_ptr += OUI_SIZE;
	// Vendor Specific Data
	memcpy(frame_ptr, data, size);

	// Send action frame
	mwifi_monitor_send_frame(buf, sizeof(management_header_t) + sizeof(action_frame_body_header_t) + sizeof(info_element_header_t) + OUI_SIZE + size);

exit:
	if (buf)
	{
		free(buf);
	}
}
