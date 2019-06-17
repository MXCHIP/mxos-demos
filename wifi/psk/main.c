/**
 ******************************************************************************
 * @file    hello_world.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    2-Feb-2019
 * @brief   First MXOS application to say hello world!
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2019 MXCHIP Inc.
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
#define SSID "snowyang"
#define PASSPHRASE "mxchip123"

#define WC_SHA 1

int wc_PBKDF2(uint8_t *output, const uint8_t *passwd, int pLen,
			  const uint8_t *salt, int sLen, int iterations, int kLen,
			  int typeH);

int pbkdf2_sha1(const char *passphrase, const uint8_t *ssid, size_t ssid_len,
				int iterations, uint8_t *buf, size_t buflen)
{
	if (wc_PBKDF2(buf, (const uint8_t *)passphrase, strlen(passphrase), ssid,
				  ssid_len, iterations, buflen, WC_SHA) != 0)
		return -1;
	return 0;
}

int main(void)
{
	uint8_t psk[32];

	mxos_network_init();

	printf("ssid=\"%s\"\r\n", SSID);
	printf("passphrase=\"%s\"\r\n", PASSPHRASE);
	uint32_t t = mos_time();
	pbkdf2_sha1(PASSPHRASE, (const uint8_t *)SSID, strlen(SSID), 4096, psk, 32);
	printf("psk=");
	for (int i = 0; i < 32; i++)
		printf("%02x", psk[i]);
	printf("\r\n");
	printf("cost %ld ms\r\n", mos_time() - t);

	return 0;
}
