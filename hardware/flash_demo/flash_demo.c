/**
 ******************************************************************************
 * @file    flash_demo.c
 * @author  Yang Haibo
 * @version V1.0.0
 * @date    14-May-2019
 * @brief   
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

static void dump_hex(uint8_t *data, int len)
{
    int i;

    for(i=0; i<len; i++) {
        printf("%02x ", data[i]);
        if (i % 16 == 15)
            printf("\r\n");
    }
    printf("\r\n");
}

int main( void )
{
    int part, i;
    uint32_t offset=0;
    uint8_t *tmp = malloc(1024);

#if 0
    printf("current index %d\r\n", ota_get_cur_index());
    for(part = MXOS_PARTITION_APPLICATION; part<MXOS_PARTITION_MAX; part++) {
        offset=0;
        memset(tmp, 0, 1024);
        mhal_flash_read( part, &offset, tmp, 1024);
        printf("~~~~~~~~~~part %d: \r\n", part);
        dump_hex(tmp, 1024);
    }
#endif
    printf("~~~~~~~~~~OTA partition test ++: \r\n");
    part = MXOS_PARTITION_OTA_TEMP;
    offset=0;
    memset(tmp, 0, 1024);
    mhal_flash_read( part, &offset, tmp, 1024);
    printf("~~~~~~read out: \r\n");
    dump_hex(tmp, 1024);

    
    offset=0;
    mhal_flash_erase(part, offset, 1024);

    for(i=0; i<1024; i++)
        tmp[i] ++;
    mhal_flash_write( part, &offset, tmp, 1024);
    
    offset=0;
    memset(tmp, 0, 1024);
    mhal_flash_read( part, &offset, tmp, 1024);
    printf("~~~~~~write -> read out: \r\n");
    dump_hex(tmp, 1024);

    printf("~~~~~~~~~~USER partition test +i: \r\n");
    part = MXOS_PARTITION_USER;
    offset=0;
    memset(tmp, 0, 1024);
    mhal_flash_read( part, &offset, tmp, 1024);
    printf("~~~~~~read out: \r\n");
    dump_hex(tmp, 1024);

    offset=0;
    mhal_flash_erase(part, offset, 1024);

    for(i=0; i<1024; i++)
        tmp[i] += i;
    mhal_flash_write( part, &offset, tmp, 1024);
    
    offset=0;
    memset(tmp, 0, 1024);
    mhal_flash_read( part, &offset, tmp, 1024);
    printf("~~~~~~write -> read out: \r\n");
    dump_hex(tmp, 1024);
    return kNoErr;
}

