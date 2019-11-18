#ifndef IN18CLOCK_RTC_H
#define IN18CLOCK_RTC_H

#include <USBAPI.h>

typedef struct rtc_info
{
	byte 	hours;
	byte 	minutes;
	byte 	seconds;
	byte 	days;
	byte 	months;
	byte 	years;
	byte 	day_of_week;
};

void rtcSetup();
rtc_info *rtcGetTime();
void rtcSetTime(rtc_info *rtcInfo);
#endif //IN18CLOCK_RTC_H
