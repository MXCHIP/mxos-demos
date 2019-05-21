/**
 ******************************************************************************
 * @file    rtc_demo.c
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

int main( void )
{
    time_t t;
    struct tm *timeinfo;
    struct tm time;
    uint32_t *p = (uint32_t*)&t;
    mxos_rtc_init();

    
    mxos_rtc_get_time(&t);
    printf("init: rtc time %lld\r\n", sizeof(time_t), t);
    timeinfo = localtime(&t);
    printf("1: %d-%d-%d %d:%d:%d\r\n",
        timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday,
        timeinfo->tm_hour, timeinfo->tm_min,timeinfo->tm_sec);

    time.tm_year = 2019; time.tm_mon=05; time.tm_mday=15;
    time.tm_hour=15; time.tm_min = 28; time.tm_sec = 0;

    t = mktime(&time);
    timeinfo = localtime(&t);
    printf("localtime of mktime: %d-%d-%d %d:%d:%d\r\n",
        timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday,
        timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
    
    printf("set: rtc time %lld\r\n", t);
    mxos_rtc_set_time(t);
    
    mxos_rtc_get_time(&t);
    printf("init: rtc time %lld\r\n", sizeof(time_t), t);
    timeinfo = localtime(&t);
    printf("1: %d-%d-%d %d:%d:%d\r\n",
        timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday,
        timeinfo->tm_hour, timeinfo->tm_min,timeinfo->tm_sec);

    t += 1000;
    printf("set: rtc time %lld\r\n", t);
    mxos_rtc_set_time(t);
    
    mxos_rtc_get_time(&t);
    printf("get rtc time %lld\r\n", t);
    timeinfo = localtime(&t);
    printf("2: %d-%d-%d %d:%d:%d\r\n",
        timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday,
        timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);

    mos_sleep(10);
    sys_reset();
    
    return kNoErr;
}

