/**
 ******************************************************************************
 * @file    rtc.c
 * @author  guidx
 * @version V1.0.0
 * @date    17-May-2019
 * @brief   RTC demo
 ******************************************************************************
 */

#include "mxos.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

#define YEAR 2019
#define MONTH 9
#define DAY 12
#define HOUR 12
#define MIN 30
#define SEC 30
#define WEEKDAY 4

int main(void)
{
	time_t t;
	struct tm *get_time;
	struct tm set_time = {
		.tm_year = YEAR - 1900, /* years since 1900 */
		.tm_mon = MONTH - 1,	/* months since January [0-11] */
		.tm_mday = DAY,
		.tm_hour = HOUR,
		.tm_min = SEC,
		.tm_sec = MIN,
		.tm_wday = WEEKDAY == 7 ? 0 : WEEKDAY, /* days since Sunday [0-6] */
	};

	mhal_rtc_open();

	t = mktime(&set_time);

	mhal_rtc_set(t);

	while (1)
	{
		mos_sleep(1.0);
		t = mhal_rtc_get();
		get_time = localtime(&t);
		app_log("%d-%d-%d, %d, %d:%d:%d",
				get_time->tm_year + 1900, /* years since 1900 */
				get_time->tm_mon + 1,	 /* months since January [0-11] */
				get_time->tm_mday,
				get_time->tm_wday == 0 ? 7 : get_time->tm_wday, /* days since Sunday [0-6] */
				get_time->tm_hour,
				get_time->tm_min,
				get_time->tm_sec);
	}
}
